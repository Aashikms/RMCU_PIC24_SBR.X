#include <xc.h>
#include <stdio.h>
#include <cTimer.h>
#include <config.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <apiTick.h>
#include <pSystem.h>
#include <apiTask.h>
#include <apiRTC.h>
#include <apiModem.h>
#include "apiModbus.h"
#include "apiDelay.h"
#include "apiEEPROM.h"
#include "apiHelpers.h"
#include "libModbus.h"
#include <libModemUART.h>


const BYTE Comma[2] = ",";
const BYTE TF_TCPPort[8] = "80";
const BYTE TCP[4] = "TCP";
const BYTE VODAFONE_APN[8] = "www";
const BYTE BSNL_APN[10] = "bsnlnet";
const BYTE IDEA_APN[10] = "internet";
const BYTE AIRCEL_APN[20] = "aircelgprs";
const BYTE AIRTEL_APN[20] = "airtelgprs.com";
const BYTE DoubleQuote[2] = {0x22, 0x00};

/*********** Persistent parameters ********************/
#ifdef TF_SERVER
const BYTE dServerAddress[50] = "live.acedc.in";
//static char dServerAddress[50]  = "182.18.168.17";
#else
static char dServerAddress[50] = "www.technosphere.in";
static char TSP_TCPPort[8] = "80";
#endif

const BYTE dDevType[6] = "C034";
const BYTE dTaskDuration[6] = "00.10";
const BYTE dAppId[10] = "00000002";
const BYTE dMobile_Number[20] = "9900038479";       // Configurable number
const BYTE TF_Support_Number[20] = "9900038479";    //TF support number
const BYTE dEmergency_Number1[20] = "8197663454";   // Naveen mobile number
const BYTE dEmergency_Number2[20] = "9945058980";   // Harsha mobile number
const BYTE dEmergency_Number3[20] = "8217027699";   // Aashik mobile number
//const BYTE dMobile_Number[20]   = "9739397428";
const BYTE dAPN[30] = "airtelgprs.com";
const BYTE dRMCUId[20] = "200000000000B00E";

/*********************************************************/

/************ RMCU SMS Commands *****************************/
const BYTE SMS_SET_SERVERADDR_CMD[] = "set serveraddr";
const BYTE SMS_SET_SIM_APN_CMD[] = "set apn";
const BYTE SMS_SET_APPID_CMD[] = "set appid";
const BYTE SMS_SET_DEVTYPE_CMD[] = "set devicetype";
const BYTE SMS_SET_RMCUID_CMD[] = "set rmcuaddress";
const BYTE SMS_SET_MN_CMD[] = "set configurenum";
const BYTE SMS_GET_RMCU_SETTINGS[] = "get rmcusettings";
const BYTE SMS_RESTART_RMCU[] = "restart";

const BYTE SMS_SET_RTC_CMD[] = "set rtc";

const BYTE SMS_WEBPERIOD_CMD[] = "set webperiod";
const BYTE SMS_MN_ACK_HEAD[] = "Mobile Phone ";
const BYTE SMS_MN_ACK_TAIL[] = " can now send commands to configure the ACE RMCUs";

//SMS ACK
const BYTE SMS_SUCCESS_ACK[] = " success";
const BYTE SMS_ERROR_ACK[] = " error ";
const BYTE SMS_INVALID_CMD_ACK[] = "Invalid Command";


const BYTE SMS_PARSE_STR1[] = "set";
const BYTE SMS_PARSE_STR2[] = "Set";
const BYTE SMS_PARSE_STR3[] = "SET";

const BYTE SMS_PARSE_STR4[] = "get";
const BYTE SMS_PARSE_STR5[] = "Get";
const BYTE SMS_PARSE_STR6[] = "GET";

const BYTE SMS_PARSE_STR7[] = "reset";
const BYTE SMS_PARSE_STR8[] = "Reset";
const BYTE SMS_PARSE_STR9[] = "RESET";

const BYTE SMS_PARSE_STR10[] = "restart";
const BYTE SMS_PARSE_STR11[] = "Restart";
const BYTE SMS_PARSE_STR12[] = "RESTART";
/*********************************************************/

/*************** SMS ERROR CODES ***************/
const BYTE INVALID_PARAMETER[] = "01";
const BYTE INVALID_COMMAND[] = "02";
const BYTE INCOMPLETE_PARAMETERS[] = "03";
const BYTE CONNECTION_FAILED[] = "04";
const BYTE FAILEDTOSEND_DATA[] = "05";
const BYTE PLC_COMM_FAILURE[] = "06";
const BYTE FAILEDTO_SAVEDATAIN_PLC[] = "07";
const BYTE FAILEDTO_SAVEDATAIN_RMCU[] = "08";

/*************************************************/

/*********** PLC SMS Commands ****************************/
/*const BYTE SMS_SET_TIMER1_CMD[] = "set timer1";
const BYTE SMS_SET_TIMER2_CMD[] = "set timer2";
const BYTE SMS_SET_TIMER3_CMD[] = "set timer3";
const BYTE SMS_SET_TIMER4_CMD[] = "set timer4";*/
const BYTE SMS_SET_TIMERS_CMD[] = "set timers";
/*const BYTE SMS_SET_P1CURRENT_CMD[] = "set pump1current";
const BYTE SMS_SET_P2CURRENT_CMD[] = "set pump2current";
const BYTE SMS_SET_P3CURRENT_CMD[] = "set pump3current";*/
const BYTE SMS_GET_PLCSETTINGS_CMD[] = "get sbrplcsettings";
const BYTE SMS_RESETPLC_CMD[] = "reset plc";
const BYTE SMS_MAINTENANCE_CMD[] = "set maintenancemode";
const BYTE SMS_SET_PLC_RTC_CMD[] = "set plc rtc";
const BYTE SERVER_TIME_TO_PLC_CMD[] = "writing server RTC to SBR PLC";
/*********************************************************/

/*********** HTTP Header for data Transmission ********************/
const BYTE HTTPSTR1[] = "POST http://live.acedc.in/parse.aspx HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\nHost: live.acedc.in\r\nContent-Length:";
const BYTE HTTPSTR3[] = "POST http://live.acedc.in/parse.aspx HTTP/1.1\r\n";
const BYTE HTTPSTR2[] = "\r\nConnection: Keep-Alive\r\n\r\n";
/******************************************************************/

extern BYTE APN[30];
extern BYTE APP_ID[10];
extern BYTE RMCU_ID[20];
extern BYTE DEV_TYPE[10];
extern BYTE WEBPERIOD[10];
extern BYTE MOBILENUMBER[20];
extern BYTE SERVER_ADDRESS[50];
extern _RMC_SBR_SYS_SET RMC_SBR_SYS_SET;

extern char Btime_buf[30];
extern char Ntime_buf[30];
extern UINT16 GPRS_Task_Count;
extern UINT8 RxCmdCount, CommandRxd;
extern UINT8 RetransmitCount, AckRxd, AckNotRxd;
extern BYTE uMsgRxFifo[uMSG_RX_BUFFER_SIZE];
extern BYTE pMsgRxFifo[pMSG_RX_FIFO_SIZE][pMSG_RX_WIDTH_SIZE];

BYTE SMS_Rxd_MobileNumber[14];
BYTE Old_Config_MobileNumber[14];
BYTE SMS_CmdStr[MAX_CHARS_IN_SMS];
BYTE SMS_RespBuffer[SMS_BUFF_SIZE];
BYTE SMS_CaseBuffer[MAX_CHARS_IN_SMS];
BYTE RECEIVED_MOBILENUMBER[20];
BYTE NEW_MOBILENUMBER[20];
_SMS_TASKS SmsTaskFifo[TASK_FIFO_SIZE];

BYTE Reserve_SMS_State = 0;

static volatile _SMS_TASKS *SmsTaskHeadPtr = SmsTaskFifo, *SmsTaskTailPtr = SmsTaskFifo;



DWORD MODEM_DLY_COUNT;
UINT8 PacketSent = 0;
UINT8 ImpPacketSent = 0;
UINT8 DefRecipient = 0;
static int mCSQCount = 0;
static int AT_RespCount = 0;
static UINT8 pMsgRxFifoHead = 0;
static UINT8 pMsgRxFifoTail = 0;
static UINT16 ModemTxBuffer_Index = 0;
static int mFailCount = 0, TotalFailCount = 0;


RXSMS_CMDS SCMD;
_SMS_TASKS SMS_TASKS;
_GPRS_DATA pGPRSTxFifo;
_GPRS_DATA ModemTxBuffer;
_MODEM_STATE MODEM_STATE;
_MODEM_STATE cMODEM_STATE;
_MODEM_STATE nMODEM_STATE;

extern _FLAGS FLAGS;
extern _MODEM_TASKS mTask;
extern _SYS_PARS SYS_PARS;
extern _SYS_PARS tSYS_PARS;
extern _DATETIME RTC_TSTAMP;
extern _MODEM_TASKS MODEM_TASK;
extern _GPRS_STATUS GPRS_STATUS;
extern _MODEM_STATUS MODEM_STATUS;
extern _NETWORK_TIMESTMAP NRTS;
extern _MDB_WRITE_DATA MDB_WRITE_DATA;


extern _ACE_SBR_MSG_PCKT ACE_SBR_EM_PCKT;
extern _ACE_SBR_MSG_PCKT ACE_SBR_ALM_PCKT;
extern _ACE_SBR_MSG_PCKT ACE_SBR_SUM_PCKT;
extern _ACE_SBR_MSG_PCKT ACE_SBR_SET_PCKT;
extern _ACE_SBR_MSG_PCKT ACE_SBR_STAT_PCKT;
extern _MODEM_TASK_STATES MODEM_TASK_STATE;
extern _MODEM_CONNECTION_STATUS MODEM_CONNECTION_STATUS;

void Modem_Stack(void) {
    char buf[SMS_BUFF_SIZE];
    char *ptr;

    switch (MODEM_STATE) {
        case MODEM_POWERDOWN:
        {
            ResetModem();
            InitSMSTasks();
            MODEM_STATE = MODEM_POWERUP;
            MODEM_STATUS = MODEM_NOT_INITIALIZED;
            break;
        }

        case MODEM_TURNOFF:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
            cMODEM_STATE = MODEM_TURNOFF;
            PutModemUART("+QPOWD=1", MODEM_COMMAND, strlen("+QPOWD=1"));
            break;
        }

#ifdef ENABLE_SLEEP
        case MODEM_HW_TURNOFF:
        {
            ModemPowerOff();
            MODEM_STATE = MODEM_WAIT;
            cMODEM_STATE = MODEM_HW_TURNOFF;
            nMODEM_STATE = MODEM_POWERDOWN;
            break;
        }
#endif

        case MODEM_TURNOFF_DELAY:
        {
            DelayMs(1000);
            MODEM_STATE = MODEM_POWERDOWN;
            break;
        }

        case MODEM_POWERUP:
        {
            ModemPowerOn();
            DelayMs(1800);
            nMODEM_STATE = MODEM_AT;
            MODEM_STATE = MODEM_SET_DLY;
            break;
        }

        case MODEM_ATE:
            cMODEM_STATE = MODEM_ATE;
            MODEM_STATE = MODEM_RESP_WAIT;
            PutModemUART("ATE0", MODEM_COMMAND, strlen("ATE0"));
            break;


        case MODEM_AT:
        {
            cMODEM_STATE = MODEM_AT;
            MODEM_STATE = MODEM_RESP_WAIT;
            PutModemUART("AT", MODEM_COMMAND, 2);
            break;
        }

        case MODEM_DCGATT:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
            cMODEM_STATE = MODEM_DCGATT;
            PutModemUART("+CGATT=0", MODEM_COMMAND, strlen("+CGATT=0"));
            break;
        }

        case MODEM_BASE_CSQ:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
            cMODEM_STATE = MODEM_BASE_CSQ;
            PutModemUART("+CSQ", MODEM_COMMAND, strlen("+CSQ"));
            break;

        }

#ifdef MODEM_VERSION_NUMBER
        case MODEM_VERSION:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
            cMODEM_STATE = MODEM_VERSION;
            PutModemUART("ATI", MODEM_COMMAND, strlen("ATI"));
            break;
        }

#endif

#ifdef AT_CSCS
        case MODEM_CSCS:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
            cMODEM_STATE = MODEM_CSCS;
            strcat((char *) (buf, "+CSCS=");
                    strcat((char *) (buf, DoubleQuote);
                    strcat((char *) (buf, "HEX");
                    strcat((char *) (buf, DoubleQuote);

                    PutModemUART(buf, MODEM_COMMAND, strlen(buf));
            break;
        }
#endif

        case MODEM_CSQ:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_CSQ;
                    PutModemUART("+CSQ", MODEM_COMMAND, strlen("+CSQ"));
            break;
        }

        case MODEM_QNITZ_EN:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QNITZ_EN;
                    PutModemUART("+QNITZ=1", MODEM_COMMAND, strlen("+QNITZ=1"));
            break;
        }
        case MODEM_QLTS_EN:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QLTS_EN;
                    PutModemUART("+QLTS", MODEM_COMMAND, strlen("+QLTS"));
                    //nMODEM_STATE = MODEM_CSQ;
            break;
        }

        case MODEM_SET_CCLK:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_SET_CCLK;

                    buf[0] = '\0';
                    strcat((char *) buf, "+CCLK=\"");
                    strcat((char *) buf, Btime_buf);
                    strcat((char *) buf, "\"");
                    PutModemUART(buf, MODEM_COMMAND, strlen(buf));
            break;
        }

        case MODEM_GET_CCLK:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_GET_CCLK;
                    PutModemUART("+CCLK?", MODEM_COMMAND, strlen("+CCLK?"));
            break;
        }

        case MODEM_CFUN:
        {
            cMODEM_STATE = MODEM_CFUN;
                    MODEM_STATE = MODEM_RESP_WAIT;
                    PutModemUART("+CFUN=1", MODEM_COMMAND, strlen("+CFUN=1"));
            break;
        }

        case MODEM_QSPN:
        {
            cMODEM_STATE = MODEM_QSPN;
                    MODEM_STATE = MODEM_RESP_WAIT;
                    PutModemUART("+QSPN?", MODEM_COMMAND, strlen("+QSPN?"));
            break;
        }

        case MODEM_CREG:
        {
            cMODEM_STATE = MODEM_CREG;
                    MODEM_STATE = MODEM_RESP_WAIT;
                    PutModemUART("+CREG?", MODEM_COMMAND, strlen("+CREG?"));
            break;
        }

        case MODEM_CGREG:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_CGREG;
                    PutModemUART("+CGREG?", MODEM_COMMAND, strlen("+CGREG?"));
            break;
        }

        case MODEM_CGATT:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_CGATT;
                    PutModemUART("+CGATT?", MODEM_COMMAND, strlen("+CGATT?"));
            break;
        }

        case MODEM_CGATT_MATT:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_CGATT_MATT;
                    PutModemUART("+CGATT=1", MODEM_COMMAND, strlen("+CGATT=1"));
            break;
        }

        case MODEM_QINISTAT:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QINISTAT;
                    PutModemUART("+QINISTAT", MODEM_COMMAND, strlen("+QINISTAT"));
            break;
        }

        case MODEM_CMGF:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_CMGF;
                    PutModemUART("+CMGF=1", MODEM_COMMAND, strlen("+CMGF=1"));
            break;
        }

        case MODEM_CMGS:
        {
            UINT16 i;
            MODEM_STATE = MODEM_RESP_WAIT;
            cMODEM_STATE = MODEM_CMGS;
            
            if (SMS_TASKS.SMS_TASK_STATE == RESTART_RMCU)
            {
            GetReceivedMobileNumber();
            strcpy((char *)MOBILENUMBER,(const char *)SYS_PARS.ReceivedMobileNum);
            }
           /* if (!(isdigit(MOBILENUMBER[0])))
            {
                break;
                SMS_TASKS.SMS_TASK_STATE = Reserve_SMS_State;
                InsertSMSTask(SMS_TASKS);
                mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                InsertMODEMTask(mTask);
            }*/
            
            if(FLAGS.DefaultMobileNo)
            {
                for (i=0 ; i <= 10 ;i++)
                {
                    if (DefRecipient == 0)
                    {
                        MOBILENUMBER[i] = dEmergency_Number3[i];
                    }
                    else if (DefRecipient == 1)
                    {
                        MOBILENUMBER[i] = dEmergency_Number2[i];
                    }
                    else if (DefRecipient == 2)
                    {
                        MOBILENUMBER[i] = dEmergency_Number1[i];
                    }
                    else
                        break;
                }
                
                if (++DefRecipient <= 2)
                {
                    InsertSMSTask(SMS_TASKS);
                    mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                    InsertMODEMTask(mTask);
                }
                else
                {
                    FLAGS.DefaultMobileNo = 0;
                    DefRecipient = 0;
                }
            }
            
                for (i = 0; i<sizeof (buf); i++) 
                    buf[i] = '\0';
                    
                strcat((char *) buf, "+CMGS=\"");
                strcat((char *) buf, (const char *) MOBILENUMBER);
                strcat((char *) buf, "\"");
                PutModemUART(buf, MODEM_COMMAND, strlen(buf));
            //}
            break;
        }

        case MODEM_CMGR:
        {
            cMODEM_STATE = MODEM_CMGR;
                    MODEM_STATE = MODEM_RESP_WAIT;

                    char temp[3];
                    char *res;

                    memset(temp, 0, sizeof (temp));
            if ((res = PopMsgInedx()) != NULL) {
                temp[0] = *res++;
                        temp[1] = *res++;
                        temp[2] = '\0';

                        buf[0] = '\0';
                        strcat((char *) buf, "+CMGR=");
                        strcat((char *) buf, temp);
                        PutModemUART(buf, MODEM_COMMAND, strlen(buf));
            }
            else {
                ResetModem();
                        MODEM_STATE = MODEM_STANDBY;
            }


            break;
        }

        case MODEM_CMGS_TEXT:
        {
            BYTE *res;
                    MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_CMGS_TEXT;

                    res = CheckSMSCmd();
                    buf[0] = '\0';
                    strcat((char *) buf, (const char *) res);
                    PutModemUART(buf, MODEM_COMMAND, strlen(buf));
            break;
        }

        case MODEM_DEL_READ:
        {
            cMODEM_STATE = MODEM_DEL_READ;
                    MODEM_STATE = MODEM_RESP_WAIT;

                    char temp[3];
                    char *res;
            if ((res = PopMsgInedx()) != NULL) {
                temp[0] = *res++;
                        temp[1] = *res++;
                        temp[2] = '\0';
            }
            buf[0] = '\0';
                    strcat((char *) buf, (const char*) "+QMGDA=");
                    strcat((char *) buf, (const char*) DoubleQuote);
                    strcat((char *) buf, (const char*) "DEL READ");
                    strcat((char *) buf, (const char*) DoubleQuote);
                    PutModemUART(buf, MODEM_COMMAND, strlen((const char*) buf));
            break;
        }

        case MODEM_DEL_ALL:
        {
            cMODEM_STATE = MODEM_DEL_ALL;
                    MODEM_STATE = MODEM_RESP_WAIT;
                    buf[0] = '\0';
                    strcat((char *) buf, (const char*) "+QMGDA=");
                    strcat((char *) buf, (const char*) DoubleQuote);
                    strcat((char *) buf, (const char*) "DEL ALL");
                    strcat((char *) buf, (const char*) DoubleQuote);
                    PutModemUART(buf, MODEM_COMMAND, strlen(buf));
            break;
        }

        case MODEM_QIFGCNT:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QIFGCNT;
                    PutModemUART("+QIFGCNT=0", MODEM_COMMAND, strlen("+QIFGCNT=0"));
            break;
        }

        case MODEM_QICSGP:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QICSGP;

                    memset((char*) buf, 0, sizeof (buf));
                    strcat((char *) buf, "+QICSGP=1");
                    strcat((char *) buf, (const char *) Comma);
                    strcat((char *) buf, (const char*) DoubleQuote);

            if (GPRS_STATUS.SC_PROVIDER == AIRTEL) {
                strcat((char *) buf, (const char*) AIRTEL_APN);
            } else if (GPRS_STATUS.SC_PROVIDER == BSNL) {
                strcat((char *) buf, (const char*) BSNL_APN);
            } else if (GPRS_STATUS.SC_PROVIDER == IDEA) {
                strcat((char *) buf, (const char*) IDEA_APN);
            } else if (GPRS_STATUS.SC_PROVIDER == VODAFONE) {
                strcat((char *) buf, (const char*) VODAFONE_APN);
            } else if (GPRS_STATUS.SC_PROVIDER == AIRCEL) {
                strcat((char *) buf, (const char*) AIRCEL_APN);
            } else {
                SOFTRESET();
            }

            strcat((char *) buf, (const char*) DoubleQuote);
                    PutModemUART(buf, MODEM_COMMAND, strlen((const char*) buf));
            break;
        }

        case MODEM_QIMUX:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QIMUX;
                    PutModemUART("+QIMUX=0", MODEM_COMMAND, strlen("+QIMUX=0"));
            break;
        }

        case MODEM_QIMODE:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QIMODE;
                    CheckDataType();
            break;
        }

        case MODEM_QITCFG:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QITCFG;
                    PutModemUART("+QITCFG=3,2,1024,1", MODEM_COMMAND, strlen("+QITCFG=3,2,1024,1"));
            break;
        }

        case MODEM_QIDNSIP:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QIDNSIP;
#ifdef DOMAIN_NAME
                    PutModemUART("+QIDNSIP=1", MODEM_COMMAND, strlen("+QIDNSIP=1"));
#else
                    PutModemUART("+QIDNSIP=0", MODEM_COMMAND, strlen("+QIDNSIP=0"));
#endif
                    break;
        }

        case MODEM_QIREGAPP:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QIREGAPP;

                    buf[0] = '\0';
                    strcat((char *) buf, "+QIREGAPP");
                    PutModemUART(buf, MODEM_COMMAND, strlen(buf));
            break;
        }

        case MODEM_QIACT:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QIACT;
                    PutModemUART("+QIACT", MODEM_COMMAND, strlen("+QIACT"));
            break;
        }

#ifdef GET_LOCAL_IP
        case MODEM_GET_IP:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_GET_IP;
                    PutModemUART("+QILOCIP", MODEM_COMMAND, strlen("+QILOCIP"));
            break;
        }
#endif

        case MODEM_QIOPEN:
        {
            UINT i;
                    ReloadRestartCounter();
                    MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QIOPEN;

            for (i = 0; i<sizeof (SERVER_ADDRESS); i++) {
                SERVER_ADDRESS[i] = 0x00;
            }

            strcpy((char*) SERVER_ADDRESS, (const char*) SYS_PARS.Server_Address);

                    memset(buf, 0, sizeof (buf));
                    strcat((char *) buf, "+QIOPEN=");
                    strcat((char *) buf, (const char*) DoubleQuote);
                    strcat((char *) buf, (const char*) TCP);
                    strcat((char *) buf, (const char*) DoubleQuote);
                    strcat((char *) buf, (const char *) Comma);
                    strcat((char *) buf, (const char*) DoubleQuote);
                    strcat((char *) buf, (const char *) SERVER_ADDRESS);

                    strcat((char *) buf, (const char*) DoubleQuote);
                    strcat((char *) buf, (const char *) Comma);
                    strcat((char *) buf, (const char*) DoubleQuote);

#ifdef TF_SERVER
                    strcat((char *) buf, (const char*) TF_TCPPort);
#else
                    strcat((char *) buf, TSP_TCPPort);
#endif

                    strcat((char *) buf, (const char*) DoubleQuote);
                    PutModemUART(buf, MODEM_COMMAND, strlen(buf));
            break;
        }

        case MODEM_QISEND:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QISEND;
                    PutModemUART("+QISEND", MODEM_COMMAND, strlen("+QISEND"));
            break;
        }

        case MODEM_GREATER:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_GREATER;
                    PutModemUART((char *) pGPRSTxFifo.pGPRSData, pGPRSTxFifo.Type, pGPRSTxFifo.Size);

            break;
        }

        case MODEM_SEND_DATA:
            cMODEM_STATE = MODEM_SEND_DATA;
                    nMODEM_STATE = CheckConClose();
                    MODEM_STATE = MODEM_SET_DLY;
            break;

        case MODEM_CHECK_ACK:
            cMODEM_STATE = MODEM_CHECK_ACK;
                    nMODEM_STATE = CheckACKfromServer();
                    MODEM_STATE = MODEM_SET_DLY;
            break;

        case MODEM_STIMEOUT:
        {
            cMODEM_STATE = MODEM_STIMEOUT;
                    nMODEM_STATE = MODEM_CHECK_ACK;
                    MODEM_STATE = MODEM_SET_DLY;
            break;
        }

        case MODEM_QICLOSE:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QICLOSE;
                    PutModemUART("+QICLOSE", MODEM_COMMAND, strlen("+QICLOSE"));
            break;
        }

        case MODEM_QIDEACT:
        {
            //Incase of connection fail system restarts.To avoid that
            ReloadRestartCounter();
                    MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_QIDEACT;
                    PutModemUART("+QIDEACT", MODEM_COMMAND, strlen("+QIDEACT"));
            break;
        }

        case MODEM_TRIPPLE_PLUS:
        {
            MODEM_STATE = MODEM_RESP_WAIT;
                    cMODEM_STATE = MODEM_TRIPPLE_PLUS;
                    MODEM_CONNECTION_STATUS = SERVER_DISCONNECTED;
                    PutModemUART("+++", MODEM_COMMAND, strlen("+++"));
            break;
        }

        case MODEM_RESP_WAIT:
        {
            break;
        }

        case MODEM_ACK_WAIT:
        {
            break;
        }

        case MODEM_WAIT:
        {
            break;
        }

        case MODEM_DLY_WAIT:
        {
            break;
        }

        case MODEM_SET_DLY:
        {
            GPRS_STATUS.CMGR = 0;

            if (SetDlyTimer()) {
                ResetModemTimer();
                        MODEM_STATE = MODEM_DLY_WAIT;
            } else
                MODEM_STATE = nMODEM_STATE;
                break;
            }

        case MODEM_STANDBY:
        {
            if (MODEM_TASK.MODEM_TASK_STATE == MODEM_NO_TASK) {
                MODEM_TASK = GetMODEMTask();
            } else {
                mFailCount = 0;

                if (MODEM_TASK.MODEM_TASK_STATE == MODEM_READ_SMS_TASK) {
                    nMODEM_STATE = MODEM_CMGF;
                            MODEM_STATE = MODEM_SET_DLY;
                } else {
                    MODEM_STATE = MODEM_BASE_CSQ;
                }
            }

            if (pMsgRxFifo[pMSG_RX_FIFO_INDEX][0] != 0) {
                ProcessSMS();
            } else if ((ptr = pMsgRxFifoPop()) != NULL) {
                strcat((char *) pMsgRxFifo[pMSG_RX_FIFO_INDEX], ptr);
            }

            break;
        }

        case MODEM_TIMEOUT:
        {
            if (++mFailCount >= MODEM_FAIL_COUNT) {
                mFailCount = 0;
                        AT_RespCount = 0;
                if (TotalFailCount++ < 3) {
                    InsertMODEMTask(MODEM_TASK);
                } else {
                    TotalFailCount = 0;
                }
                MODEM_TASK.MODEM_TASK_STATE = MODEM_NO_TASK;
                        memset(MODEM_TASK.Parameters, 0, sizeof (MODEM_TASK.Parameters));
                        nMODEM_STATE = MODEM_TURNOFF;
            } else {
                if (cMODEM_STATE == MODEM_QIACT ||
                        cMODEM_STATE == MODEM_QIDNSIP ||
                        cMODEM_STATE == MODEM_QIOPEN ||
                        cMODEM_STATE == MODEM_QISEND) {
                    nMODEM_STATE = MODEM_QICLOSE;
                }
            }

            if (MODEM_STATE == MODEM_CMGF || MODEM_STATE == MODEM_CMGR) {
                nMODEM_STATE = MODEM_STANDBY;
            }

            MODEM_STATE = MODEM_SET_DLY;

            break;
        }

        default:
            break;
    }
}

void ModemCallBack(void) {
    char temp;
            mFailCount = 0;

    switch (MODEM_STATE) {
        case MODEM_POWERDOWN_RESP:
        {
            nMODEM_STATE = MODEM_POWERUP;
            break;
        }

        case MODEM_ATE_RESP:
            nMODEM_STATE = MODEM_STANDBY;
            break;

        case MODEM_AT_RESP:
        {
            if (++AT_RespCount >= 5) {
                MODEM_STATUS = MODEM_INITIALIZED;

#ifdef EN_NETWORK_TIME
                        nMODEM_STATE = MODEM_QNITZ_EN;
#else
                        nMODEM_STATE = MODEM_STANDBY;
#endif

            } else {
                nMODEM_STATE = MODEM_AT;
            }
            break;
        }

        case MODEM_DCGATT_RESP:
        {
            nMODEM_STATE = MODEM_BASE_CSQ;
            break;
        }

        case MODEM_BASE_CSQ_RESP:
        {
            if (GPRS_STATUS.SG_STNTH[1] == 0) {
                temp = GPRS_STATUS.SG_STNTH[0] - 0x30;
            } else {
                temp = (GPRS_STATUS.SG_STNTH[0] - 0x30) * 10;
                        temp = temp + GPRS_STATUS.SG_STNTH[1] - 0x30;
            }

            if (temp > 0 && temp < 35) {
#ifdef AT_CSCS
                nMODEM_STATE = MODEM_CSCS;
#else
                nMODEM_STATE = MODEM_CSQ;
                //nMODEM_STATE = MODEM_QLTS_EN;
#endif
            } else {
                nMODEM_STATE = MODEM_BASE_CSQ;
            }
            break;
        }

#ifdef AT_CSCS
        case MODEM_CSCS_RESP:
        {
            nMODEM_STATE = MODEM_GET_GPRS_DATA;
            break;
        }
#endif

        case MODEM_CSQ_RESP:
        {
            if (GPRS_STATUS.SG_STNTH[1] == 0) {
                temp = GPRS_STATUS.SG_STNTH[0] - 0x30;
            } else {
                temp = (GPRS_STATUS.SG_STNTH[0] - 0x30) * 10;
                        temp = temp + GPRS_STATUS.SG_STNTH[1] - 0x30;
            }

            if (temp > 0 && temp < 35) {
                nMODEM_STATE = MODEM_CFUN;
            } else {
                if (++mCSQCount > MODEM_NO_CSQ_COUNT) {
                    mCSQCount = 0;
                            nMODEM_STATE = MODEM_POWERDOWN;
                } else {
                    nMODEM_STATE = MODEM_CSQ;
                }
            }
            break;
        }

        case MODEM_QNITZ_EN_RESP:
        {
#ifdef ECHO_OFF
            nMODEM_STATE = MODEM_ATE;
#else
            nMODEM_STATE = MODEM_STANDBY;
#endif
                    break;
        }

        case MODEM_CFUN_RESP:
        {
            #ifdef GET_TIMESTAMP_FROM_PLC
                nMODEM_STATE = MODEM_QSPN;
            #else
                nMODEM_STATE = MODEM_QLTS_EN;
            #endif
            break;
        }
        case MODEM_QLTS_EN_RESP:
        {
            nMODEM_STATE = MODEM_QSPN;
            break;
        }

        case MODEM_QSPN_RESP:
        {
            nMODEM_STATE = MODEM_CREG;
            break;
        }

        case MODEM_CREG_RESP:
        {
            nMODEM_STATE = MODEM_CGREG;
            break;
        }

        case MODEM_CGREG_RESP:
        {
            nMODEM_STATE = MODEM_CGATT;
            break;
        }

        case MODEM_CGATT_RESP:
        {
            nMODEM_STATE = MODEM_QINISTAT;
            break;
        }

        case MODEM_QINISTAT_RESP:
        {
            nMODEM_STATE = CheckQINISTATTask();
            break;
        }

        case MODEM_CMGF_RESP:
        {
            nMODEM_STATE = CheckCMGFTask();
            break;
        }

        case MODEM_CMGS_RESP:
        {
            nMODEM_STATE = MODEM_CMGS_TEXT;
            break;
        }

        case MODEM_CMGS_TEXT_RESP:
        {
            nMODEM_STATE = MODEM_STANDBY;
                    MODEM_TASK.MODEM_TASK_STATE = MODEM_NO_TASK;
            break;
        }

        case MODEM_QIFGCNT_RESP:
        {
            nMODEM_STATE = MODEM_QICSGP;
            break;
        }

        case MODEM_QICSGP_RESP:
        {
            nMODEM_STATE = MODEM_QIMUX;
            break;
        }

        case MODEM_QIMUX_RESP:
        {
            nMODEM_STATE = MODEM_QIMODE;
            break;
        }

        case MODEM_QIMODE_RESP:
        {
            nMODEM_STATE = MODEM_QITCFG;
            break;
        }

        case MODEM_QITCFG_RESP:
        {
            nMODEM_STATE = MODEM_QIDNSIP;
            break;
        }

        case MODEM_QIDNSIP_RESP:
        {
            nMODEM_STATE = MODEM_QIREGAPP;
            break;
        }

        case MODEM_QIREGAPP_RESP:
        {
            nMODEM_STATE = MODEM_QIACT;
            break;
        }

        case MODEM_QIACT_RESP:
        {
            nMODEM_STATE = MODEM_QIOPEN;
#ifdef GET_LOCAL_IP
                    nMODEM_STATE = MODEM_GET_IP;
#endif
                    break;
        }

#ifdef GET_LOCAL_IP
        case MODEM_GET_IP_RESP:
        {
            nMODEM_STATE = MODEM_QIOPEN;
            break;
        }
#endif

        case MODEM_QIOPEN_RESP:
        {
            nMODEM_STATE = MODEM_QISEND;
            break;
        }

            //Ack from server
        case MODEM_SERVER_RESP:
            nMODEM_STATE = MODEM_SEND_DATA;
            break;

        case MODEM_QISEND_RESP:
        {
            nMODEM_STATE = MODEM_SEND_DATA;
            break;
        }

        case MODEM_GREATER_RESP:
        {
            memset(pGPRSTxFifo.pGPRSData, 0, sizeof (GPRS_TX_BUFFER_SIZE));
            break;
        }

        case MODEM_TRIPPLE_PLUS_RESP:
        {
            memset(pGPRSTxFifo.pGPRSData, 0, sizeof (GPRS_TX_BUFFER_SIZE));
                    nMODEM_STATE = MODEM_QICLOSE;

            break;
        }

        case MODEM_PRE_QICLOSE_RESP:
        {
            nMODEM_STATE = MODEM_QIDEACT;
            break;
        }

        case MODEM_QICLOSE_RESP:
        {
            ResetModem();
                    nMODEM_STATE = MODEM_QIDEACT;
            break;
        }

        case MODEM_QIDEACT_RESP:
        {
            ResetModem();
                    ReloadRestartCounter();
                    MODEM_STATUS = MODEM_CLOSED;
                    MODEM_CONNECTION_STATUS = SERVER_DISCONNECTED;
                    //need to check required here r not
#ifdef EN_SLEEP
                    nMODEM_STATE = MODEM_HW_TURNOFF;
#else
                    nMODEM_STATE = MODEM_STANDBY;
#endif
                    FLAGS.ConnectionTimeOut = 0;
            break;
        }

        case MODEM_CMGR_RESP:
        {
            nMODEM_STATE = MODEM_DEL_READ;
            break;
        }

        case MODEM_DEL_ALL_RESP:
        {
            ResetModem();
                    nMODEM_STATE = MODEM_STANDBY;
                    memset(GPRS_STATUS.MsgIndex, 0, sizeof (GPRS_STATUS.MsgIndex));
            break;
        }

        case MODEM_DEL_READ_RESP:
        {
            if (!CheckMsgIndexFifo()) {
                nMODEM_STATE = MODEM_DEL_ALL;
            } else {
                nMODEM_STATE = MODEM_STANDBY;
            }
            ResetModem();
            break;
        }

        case MODEM_TURNOFF_RESP:
        {
            nMODEM_STATE = MODEM_TURNOFF_DELAY;

            break;
        }

        default:
            break;
    }

    MODEM_STATE = MODEM_SET_DLY;
}


void ConfigModem(void) {
    //Power Key

    TRISBbits.TRISB11 = 0;
            LATBbits.LATB11 = 0;

            //GSM_Status
            TRISBbits.TRISB13 = 0;
            LATBbits.LATB13 = 0;

            DelayMs(10);

}

void ResetModem(void) {

    MODEM_TASK.MODEM_TASK_STATE = MODEM_NO_TASK;
}

void ModemPowerOn(void) {

    GSM_ON_OFF = 0;
            DelayMs(2000);
            GSM_ON_OFF = 1;

            DelayMs(2000);
            GSM_POWERKEY = 1;
            DelayMs(2000);
            GSM_POWERKEY = 0;

            DelayMs(1500);
            GSM_POWERKEY = 1;
            DelayMs(2000);
            GSM_POWERKEY = 0;

            DelayMs(1500);
            GSM_POWERKEY = 1;
            DelayMs(2000);
}

void ProcessSMS(void) {
    char *ptr;
            BYTE mData[20];
            UINT i, len = 0;
            UINT16 I_Res=0;
            BYTE SetTmrBuff[300];

    if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], "+91")) != NULL) {
        ptr += strlen("+91");
                memset(SMS_Rxd_MobileNumber, 0, sizeof (SMS_Rxd_MobileNumber));
                strncpy((char*) SMS_Rxd_MobileNumber, (const char*) ptr, 10);

#ifdef SMS_NUMBER_VALIDATION
                //TRUE:Your mobile number registered
        if ((!strcmp((const char*) SYS_PARS.MobileNum, (const char*) SMS_Rxd_MobileNumber)) ||
                (!strcmp((const char*) dEmergency_Number1, (const char*) SMS_Rxd_MobileNumber)) ||
                (!strcmp((const char*) dEmergency_Number2, (const char*) SMS_Rxd_MobileNumber)) ||
                (!strcmp((const char*) dEmergency_Number3, (const char*) SMS_Rxd_MobileNumber)) ||
                (!strcmp((const char*) TF_Support_Number, (const char*) SMS_Rxd_MobileNumber))
                )
        {
#endif

#ifdef HANDLE_CASE_SENSITIVE_SMS
            memset(SMS_CaseBuffer, 0, sizeof (SMS_CaseBuffer));
                    memset(SMS_CmdStr, 0, sizeof (SMS_CmdStr));

                    /*Do not edit below lines*/
            if (((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR7)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR8)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR9)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR4)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR5)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR6)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR1)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR2)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR3)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR10)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR11)) != NULL) ||
                    ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_PARSE_STR12)) != NULL)) 
            {
                
                memset(RECEIVED_MOBILENUMBER, '\0', sizeof (RECEIVED_MOBILENUMBER));
                strcpy((char *)RECEIVED_MOBILENUMBER,(char *)SMS_Rxd_MobileNumber);

                for (i = 0; i<sizeof (MOBILENUMBER); i++) {
                MOBILENUMBER[i] = 0x00;
                }
                strcpy((char*) MOBILENUMBER, (const char*) RECEIVED_MOBILENUMBER);
    
                for (i = 0; i < MAX_CHARS_IN_SMS; i++) {
                    if (*ptr == '\r') {
                        SMS_CaseBuffer[i] = *ptr++;
                        break;
                    }
                    SMS_CaseBuffer[i] = *ptr++;
                }
            }

            for (i = 0; i < MAX_CHARS_IN_SMS; i++) {

                if (SMS_CaseBuffer[i] == '\r') {
                    SMS_CmdStr[i] = SMS_CaseBuffer[i];
                    break;
                }

                if (((SMS_CaseBuffer[i] >= 0x41) && (SMS_CaseBuffer[i] <= 0x5A))) {
                    if (!FLAGS.SMSData)
                            SMS_CmdStr[i] = (SMS_CaseBuffer[i] + 0x20);
                    else
                        SMS_CmdStr[i] = SMS_CaseBuffer[i];
                    }
                else if (SMS_CaseBuffer[i] == 0x20) {
                    SMS_CmdStr[i] = SMS_CaseBuffer[i];
                }
                else if (((SMS_CaseBuffer[i] >= 0x61) && (SMS_CaseBuffer[i] <= 0x7A))) {
                    SMS_CmdStr[i] = SMS_CaseBuffer[i];
                }
                else {
                    FLAGS.SMSData = 1;
                            SMS_CmdStr[i] = SMS_CaseBuffer[i];
                }
            }
            FLAGS.SMSData = 0;
                    memset(pMsgRxFifo[pMSG_RX_FIFO_INDEX], 0, pMSG_RX_WIDTH_SIZE);
                    strcat((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_CmdStr);
#endif

                    /* SMS engine starts here*/

            //Check for Mobile number set command
                    
            if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_MN_CMD)) != NULL) {
                ptr += strlen((const char*) SMS_SET_MN_CMD);
                        ptr++; //To avoid after above string space

                        memset(tSYS_PARS.MobileNum, 0, sizeof (tSYS_PARS.MobileNum));

                        //Initial value is Success
                        SMS_TASKS.Parameters[0] = 0xFF;

                for (i = 0; i < 10; i++) {
                    if (*ptr == '\r') {
                        break;
                    }

                    tSYS_PARS.MobileNum[i] = *ptr++;

                            //found characters instead of numbers
                    if ((tSYS_PARS.MobileNum[i] < 0x30) || (tSYS_PARS.MobileNum[i] > 0x39)) {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }
                tSYS_PARS.MobileNum[i] = '\0';

                        len = strlen((const char*) tSYS_PARS.MobileNum);
                if (len < 10) {
                    SMS_TASKS.Parameters[0] = 0x03;

                }

                if (len > 10) {
                    SMS_TASKS.Parameters[0] = 0x01;
                }


                if (SMS_TASKS.Parameters[0] == 0xFF) {
                    FLAGS.MobileNumberChanged = 1;
                            strcpy((char*) Old_Config_MobileNumber, (const char*) SYS_PARS.MobileNum);
                            strcpy((char*) SYS_PARS.MobileNum, (const char*) tSYS_PARS.MobileNum);
                            strcpy((char*) NEW_MOBILENUMBER, (const char*) SYS_PARS.MobileNum);
                            Write_MobileNumber();
                }

                SMS_TASKS.SMS_TASK_STATE = MN_CMD;
                        InsertSMSTask(SMS_TASKS);
                        mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                        InsertMODEMTask(mTask);
            }
                    // Set server address SMS command
                    
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_SERVERADDR_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_SERVERADDR_CMD);
                ptr++; //To avoid space after above string

                memset(tSYS_PARS.Server_Address, '\0', sizeof (tSYS_PARS.Server_Address));

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                for (i = 0; i < SERVER_ADDRESS_LENGTH; i++) 
                {
                    tSYS_PARS.Server_Address[i] = *ptr;

                    #ifdef DOMAIN_NAME
                        if ((tSYS_PARS.Server_Address[i] >= 0x30) && (tSYS_PARS.Server_Address[i] <= 0x39)) 
                        {
                            //found numbers in string
                            SMS_TASKS.Parameters[0] = 0x01;
                        }
                    #endif

                    if (*ptr++ == '!') 
                    {
                        break;
                    }
                }
                tSYS_PARS.Server_Address[i] = '\0';

                if (i >= 39) 
                {
                    //Not found '!' out of 40 characters so Error
                    SMS_TASKS.Parameters[0] = 0x03;
                } 
                else {

                    if (SMS_TASKS.Parameters[0] == 0xFF) 
                    {
                        strcpy((char*) SYS_PARS.Server_Address, (const char*) tSYS_PARS.Server_Address);
                        strcpy((char*) SERVER_ADDRESS, (const char*) SYS_PARS.Server_Address);
                        Write_ServerAddress();
                    }
                }


                SMS_TASKS.SMS_TASK_STATE = SERVER_ADDR_CMD;
                InsertSMSTask(SMS_TASKS);
                mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                InsertMODEMTask(mTask);
            }
                    /* Set SIM APN address SMS command*/
                    
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_SIM_APN_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_SIM_APN_CMD);
                ptr++; //To avoid space after above string

                memset(tSYS_PARS.APN, '\0', sizeof (tSYS_PARS.APN));

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                for (i = 0; i < APN_LENGTH; i++) 
                {
                    tSYS_PARS.APN[i] = *ptr;

                    if ((tSYS_PARS.APN[i] >= 0x30) && (tSYS_PARS.APN[i] <= 0x39)) 
                    {
                        //found numbers in string
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                    if (*ptr++ == '!') 
                    {
                        break;
                    }
                }
                tSYS_PARS.APN[i] = '\0';

                if (i >= 29) 
                {
                    //Not found '!' out of 30 characters so Error
                    SMS_TASKS.Parameters[0] = 0x03;

                } 
                else 
                {
                    if (SMS_TASKS.Parameters[0] == 0xFF) 
                    {
                        strcpy((char*) SYS_PARS.APN, (const char*) tSYS_PARS.APN);
                                strcpy((char*) APN, (const char*) SYS_PARS.APN);
                                Write_APN();
                    }
                }


                SMS_TASKS.SMS_TASK_STATE = NET_APN_CMD;
                        InsertSMSTask(SMS_TASKS);
                        mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                        InsertMODEMTask(mTask);
            }
                    /* SET APP ID SMS command*/
                    
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_APPID_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_APPID_CMD);
                ptr++; //To avoid space after above string

                memset(tSYS_PARS.App_Id, '\0', sizeof (tSYS_PARS.App_Id));

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                for (i = 0; i < APP_ID_LENGTH; i++) 
                {
                    if (*ptr == '\r') 
                    {
                        break;
                    }

                    tSYS_PARS.App_Id[i] = *ptr++;
                }
                tSYS_PARS.App_Id[i] = '\0';

                if (strstr((char*) tSYS_PARS.App_Id, "00000000")) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                len = strlen((const char*) tSYS_PARS.App_Id);
                if (len < 8) 
                {
                    SMS_TASKS.Parameters[0] = 0x03;
                }

                if (len > 8) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                /*Filter for 0 to 9 and A(a) to F(f)*/
                for (i = 0; tSYS_PARS.App_Id[i] != '\0'; i++) 
                {
                    if (((tSYS_PARS.App_Id[i] >= 0x30) && (tSYS_PARS.App_Id[i] <= 0x39)) ||
                            ((tSYS_PARS.App_Id[i] >= 0x41) && (tSYS_PARS.App_Id [i] <= 0x46)) ||
                            ((tSYS_PARS.App_Id[i] >= 0x61) && (tSYS_PARS.App_Id[i] <= 0x66))) 
                    {
                        //No error, expected value
                    } 
                    else 
                    {
                        /*As per Document values should be between 0 to 9 and A to F so error*/
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    strcpy((char*) SYS_PARS.App_Id, (const char*) tSYS_PARS.App_Id);
                    strcpy((char*) APP_ID, (const char*) SYS_PARS.App_Id);
                    Write_AppId();
                }


                SMS_TASKS.SMS_TASK_STATE = APP_ID_CMD;
                InsertSMSTask(SMS_TASKS);
                mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                InsertMODEMTask(mTask);
            }
                    /*SET Device type SMS command*/
                    
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_DEVTYPE_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_DEVTYPE_CMD);
                ptr++; //To avoid space after above string

                memset(tSYS_PARS.Dev_Type, '\0', sizeof (tSYS_PARS.Dev_Type));

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                for (i = 0; i < DEV_TYPE_LENGTH; i++) 
                {
                    if (*ptr == '\r') 
                    {
                        break;
                    }

                    tSYS_PARS.Dev_Type[i] = *ptr++;
                }
                tSYS_PARS.Dev_Type[i] = '\0';

                if (strstr((char*) tSYS_PARS.Dev_Type, "0000")) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                len = strlen((const char*) tSYS_PARS.Dev_Type);
                if (len < 4) 
                {
                    SMS_TASKS.Parameters[0] = 0x03;
                }

                if (len > 4) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                /*Filter for 0 to 9 and A(a) to F(f)*/
                for (i = 0; tSYS_PARS.Dev_Type[i] != '\0'; i++) 
                {
                    if (((tSYS_PARS.Dev_Type[i] >= 0x30) && (tSYS_PARS.Dev_Type[i] <= 0x39)) ||
                            ((tSYS_PARS.Dev_Type[i] >= 0x41) && (tSYS_PARS.Dev_Type[i] <= 0x46)) ||
                            ((tSYS_PARS.Dev_Type[i] >= 0x61) && (tSYS_PARS.Dev_Type[i] <= 0x66))) 
                    {
                        //No error expected value
                    } 
                    else 
                    {
                        /*As per Document values should be between 0 to 9 and A to F so error*/
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    //success
                    strcpy((char*) SYS_PARS.Dev_Type, (const char*) tSYS_PARS.Dev_Type);
                    strcpy((char*) DEV_TYPE, (const char*) SYS_PARS.Dev_Type);
                    Write_DevType();
                }

                SMS_TASKS.SMS_TASK_STATE = DEV_TYPE_CMD;
                InsertSMSTask(SMS_TASKS);
                mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                InsertMODEMTask(mTask);
            }
                    /*SET RMCU ID SMS command*/
                    
            else if ((ptr = strstr((const char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char *) SMS_SET_RMCUID_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_RMCUID_CMD);
                ptr++; //To avoid space after above string

                memset(tSYS_PARS.RMCU_Id, '\0', sizeof (tSYS_PARS.RMCU_Id));

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                for (i = 0; i < (RMCU_ID_LENGTH - 2); i++) 
                {
                    if (*ptr == '\r') 
                    {
                        break;
                    }

                    tSYS_PARS.RMCU_Id[i] = *ptr++;
                }
                tSYS_PARS.RMCU_Id[i] = '\0';

                if (strstr((char*) tSYS_PARS.RMCU_Id, "0000000000000000")) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                len = strlen((const char *) tSYS_PARS.RMCU_Id);
                if (len < 16) 
                {
                    SMS_TASKS.Parameters[0] = 0x03;
                }

                if (len > 16) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                /*Filter for 0 to 9 and A(a) to F(f)*/
                for (i = 0; tSYS_PARS.RMCU_Id[i] != '\0'; i++) 
                {
                    if (((tSYS_PARS.RMCU_Id[i] >= 0x30) && (tSYS_PARS.RMCU_Id[i] <= 0x39)) ||
                            ((tSYS_PARS.RMCU_Id[i] >= 0x41) && (tSYS_PARS.RMCU_Id[i] <= 0x46)) ||
                            ((tSYS_PARS.RMCU_Id[i] >= 0x61) && (tSYS_PARS.RMCU_Id[i] <= 0x66))) 
                    {
                        //No error expected value
                    } 
                    else 
                    {
                        /*As per Document values should be between 0 to 9 and A to F so error*/
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    //success
                    strcpy((char *) SYS_PARS.RMCU_Id, (const char *) tSYS_PARS.RMCU_Id);
                    strcpy((char*) RMCU_ID, (const char*) SYS_PARS.RMCU_Id);
                    Write_RMCUId();
                }

                SMS_TASKS.SMS_TASK_STATE = RMCU_ID_CMD;
                InsertSMSTask(SMS_TASKS);
                mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                InsertMODEMTask(mTask);

            }
                    /*Set Web period SMS command*/
            else if ((ptr = strstr((const char *) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_WEBPERIOD_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_WEBPERIOD_CMD);
                ptr++; //To avoid space after above string

                memset(tSYS_PARS.TaskDuration, '\0', sizeof (tSYS_PARS.TaskDuration));

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                for (i = 0; i < 5; i++) 
                {
                    if (*ptr == '\r') 
                    {
                        break;
                    }

                    if (((*ptr >= 0x30) && (*ptr <= 0x39)) || (*ptr == 0x2E)) 
                    {
                        tSYS_PARS.TaskDuration[i] = *ptr++;

                    } 
                    else 
                    {
                        //Found ASCII charcters in string but only 0 to 9 and .(dot) is expected so error
                        SMS_TASKS.Parameters[0] = 0x01;
                    }

                }
                tSYS_PARS.TaskDuration[i] = '\0';

                len = strlen((const char *) tSYS_PARS.TaskDuration);

                /*If already 1 error code is found send that no need to check for one more*/
                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    if (len < 5) 
                    {
                        SMS_TASKS.Parameters[0] = 0x03;
                    }

                    if (len > 5) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }

                /*If already 1 error code is found send that no need to check for one more*/
                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    if (tSYS_PARS.TaskDuration[2] != '.') 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    /*success*/
                    strcpy((char *) SYS_PARS.TaskDuration, (const char *) tSYS_PARS.TaskDuration);
                    strcpy((char*) WEBPERIOD, (const char*) SYS_PARS.TaskDuration);
                    Write_WebPeriod();

                    strncpy((char*) tTask_Duration, (const char*) SYS_PARS.TaskDuration, 2);
                    Task_Duration_H = atoi((char*) tTask_Duration);

                    if ((ptr = strstr((char*) SYS_PARS.TaskDuration, ".")) != NULL) 
                    {
                        ptr++;
                        tTask_Duration[0] = *ptr++;
                        tTask_Duration[1] = *ptr++;
                        Task_Duration_M = atoi((char*) tTask_Duration);
                    }
                    else 
                    {
                        Task_Duration_M = 0;
                    }

                    WebPeriod = ((Task_Duration_H * 60) + Task_Duration_M);
                }

                SMS_TASKS.SMS_TASK_STATE = TASK_DURATION_CMD;
                InsertSMSTask(SMS_TASKS);
                mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                InsertMODEMTask(mTask);
            }
                    
                    /*get RMCU settings SMS command*/
            else if ((ptr = strstr((const char *) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_GET_RMCU_SETTINGS)) != NULL) 
            {
                SMS_TASKS.Parameters[0] = 0xFF;
                SMS_TASKS.SMS_TASK_STATE = GET_RMCU_SETTINGS;
                InsertSMSTask(SMS_TASKS);
                mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                InsertMODEMTask(mTask);
            }
                    
                    /*Restart RMCU SMS command*/
            else if ((ptr = strstr((const char *) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_RESTART_RMCU)) != NULL) 
            {
                strncpy((char *)SYS_PARS.ReceivedMobileNum,(const char *)MOBILENUMBER,10);
                Write_ReceivedMobileNumber();
                Write_RestartByte();
                SOFTRESET();
            }
                /********************* MODBUS WRITE Commands ***********************************/
                        
                    /*Set Timer 1 SMS command*/
            /*else if ((ptr = strstr((const char *) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_TIMER1_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_TIMER1_CMD);

                if ((*ptr != '\r') && (*ptr != '0'))
                        ptr++; //To avoid space after above string

                MDB_WRITE_DATA.TIMER1_SECS.Val = 0;

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                if (*ptr != '\r') 
                {
                        for (i = 0; *ptr != '\r'; i++) 
                        {
                            mData[i] = *ptr++;
                        }
                } 
                else 
                {
                        if ((*ptr == '\r') || (*ptr == ' ')) 
                        {
                            SMS_TASKS.Parameters[0] = 0x03;
                        }
                        else if ((*ptr == '0')) 
                        {
                            SMS_TASKS.Parameters[0] = 0x01;
                        }
                }

                MDB_WRITE_DATA.TIMER1_SECS.Val = atoi((const char *) mData);

                        //If already 1 error code is found send that no need to check for one more
                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    if (MDB_WRITE_DATA.TIMER1_SECS.Val == 0) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }

                    if (MDB_WRITE_DATA.TIMER1_SECS.Val > 65535) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }

                SMS_TASKS.SMS_TASK_STATE = SET_TIMER1;

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    //Valid data go for PLC write
                    InsertSYSTask(SET_TIMER1_TASK);
                } 
                else 
                {
                    //send Error response
                    InsertSMSTask(SMS_TASKS);
                    mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                    InsertMODEMTask(mTask);
                }

            }
                    //Set Timer 2 SMS command
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_TIMER2_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_TIMER2_CMD);

                if (*ptr != '\r')
                    ptr++; //To avoid space after above string

                MDB_WRITE_DATA.TIMER2_SECS.Val = 0;

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                if (*ptr != '\r') 
                {
                    for (i = 0; *ptr != '\r'; i++) 
                    {
                        mData[i] = *ptr++;
                    }
                } 
                else 
                {
                    if ((*ptr == '\r') || (*ptr == ' ')) 
                    {
                        SMS_TASKS.Parameters[0] = 0x03;
                    }
                    else if ((*ptr == '0')) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }

                }

                MDB_WRITE_DATA.TIMER2_SECS.Val = atoi((char *) mData);

                        //If already 1 error code is found send that no need to check for one more
                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    if (MDB_WRITE_DATA.TIMER2_SECS.Val == 0) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }

                    if (MDB_WRITE_DATA.TIMER2_SECS.Val > 65535) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }

                SMS_TASKS.SMS_TASK_STATE = SET_TIMER2;

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    //success
                    InsertSYSTask(SET_TIMER2_TASK);
                }
                else 
                {
                    //send Error response
                    InsertSMSTask(SMS_TASKS);
                    mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                    InsertMODEMTask(mTask);
                }
            }
                    // Set Timer 3 SMS commands
                    
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_TIMER3_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_TIMER3_CMD);

                if (*ptr != '\r')
                        ptr++; //To avoid space after above string

                MDB_WRITE_DATA.TIMER3_SECS.Val = 0;

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                if (*ptr != '\r') 
                {
                    for (i = 0; *ptr != '\r'; i++) 
                    {
                        mData[i] = *ptr++;
                    }
                } 
                else 
                {
                    if ((*ptr == '\r') || (*ptr == ' ')) 
                    {
                        SMS_TASKS.Parameters[0] = 0x03;
                    }
                    else if ((*ptr == '0')) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }

                MDB_WRITE_DATA.TIMER3_SECS.Val = atoi((char *) mData);

                        //If already 1 error code is found send that no need to check for one more
                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    if (MDB_WRITE_DATA.TIMER3_SECS.Val == 0) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }

                    if (MDB_WRITE_DATA.TIMER3_SECS.Val > 65535) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }
                SMS_TASKS.SMS_TASK_STATE = SET_TIMER3;

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    //success
                    InsertSYSTask(SET_TIMER3_TASK);
                }
                else 
                {
                    //send Error response
                    InsertSMSTask(SMS_TASKS);
                    mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                    InsertMODEMTask(mTask);
                }

            }
                    //Set Timer 4 SMS command
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_TIMER4_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_TIMER4_CMD);

                if (*ptr != '\r')
                    ptr++; //To avoid space after above string

                MDB_WRITE_DATA.TIMER4_SECS.Val = 0;

                //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                if (*ptr != '\r') 
                {
                    for (i = 0; *ptr != '\r'; i++) 
                    {
                        mData[i] = *ptr++;
                    }
                }
                else 
                {
                    if ((*ptr == '\r') || (*ptr == ' ')) 
                    {
                        SMS_TASKS.Parameters[0] = 0x03;
                    }
                    else if ((*ptr == '0')) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }

                MDB_WRITE_DATA.TIMER4_SECS.Val = atoi((char *) mData);

                        //If already 1 error code is found send that no need to check for one more
                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    if (MDB_WRITE_DATA.TIMER4_SECS.Val == 0) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }

                    if (MDB_WRITE_DATA.TIMER4_SECS.Val > 65535) 
                    {
                        SMS_TASKS.Parameters[0] = 0x01;
                    }
                }
                SMS_TASKS.SMS_TASK_STATE = SET_TIMER4;

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    //success
                    InsertSYSTask(SET_TIMER4_TASK);
                }
                else 
                {
                    //send Error response
                    InsertSMSTask(SMS_TASKS);
                    mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                    InsertMODEMTask(mTask);
                }

            } */
            /* Set Timers Threshold SMS*/ 
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_TIMERS_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_TIMERS_CMD);
                ptr++; //To avoid space after above string

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                memset(SetTmrBuff,0, sizeof (SetTmrBuff));
                strcat((char *) SetTmrBuff, ptr);

                if (((strstr((char *) SetTmrBuff, "BT:")) != NULL) || ((strstr((char *) SetTmrBuff, "bt:")) != NULL)) 
                {
                    //Found BT: increment pointer to get data
                    ptr += 3;
                }
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.BATCH_CYCLE.Val = 0;

                if (strstr((const char*) SetTmrBuff, ",FS:")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                        //if (mData[i]<48 || mData[i]>57)
                        //    SMS_TASKS.Parameters[0] = 0x01;
                    }
                } 
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char*) mData);
                MDB_WRITE_DATA.BATCH_CYCLE.Val = I_Res;

                if ((strstr((char *) SetTmrBuff, (const char*) ",FS:")) == NULL) 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found ,FS: increment ponter (4 times including ',') to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.FILL_START.Val = 0;

                if (strstr((const char*) SetTmrBuff, ",FE:")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                        //if (mData[i]<48 || mData[i]>57)
                        //    SMS_TASKS.Parameters[0] = 0x01;
                    }
                } 
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char *) mData);
                MDB_WRITE_DATA.FILL_START.Val = I_Res;
                
                if ((strstr((char *) SetTmrBuff, (const char*) ",FE:")) == NULL) 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found ,FE: increment pointer (4 times including ',') to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.FILL_END.Val = 0;

                if (strstr((const char*) SetTmrBuff, ",AS:")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                        //if (mData[i]<48 || mData[i]>57)
                        //    SMS_TASKS.Parameters[0] = 0x01;
                    }
                } 
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char *) mData);
                MDB_WRITE_DATA.FILL_END.Val = I_Res;

                if ((strstr((char *) SetTmrBuff, (const char*) ",AS:")) == NULL) 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found ,AS: increment ponter (4 times including ',') to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.AERATION_START.Val = 0;

                if (strstr((const char*) SetTmrBuff, ",AE:")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                        //if (mData[i]<48 || mData[i]>57)
                        //    SMS_TASKS.Parameters[0] = 0x01;
                    }
                } 
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char *) mData);
                MDB_WRITE_DATA.AERATION_START.Val = I_Res;
                
                if ((strstr((char *) SetTmrBuff, (const char*) ",AE:")) == NULL) 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found ,AE: increment pointer (4 times including ',') to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.AERATION_END.Val = 0;

                if (strstr((const char*) SetTmrBuff, ",SS:")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                        //if (mData[i]<48 || mData[i]>57)
                        //    SMS_TASKS.Parameters[0] = 0x01;
                    }
                } 
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char *) mData);
                MDB_WRITE_DATA.AERATION_END.Val = I_Res;
                
                if ((strstr((char *) SetTmrBuff, (const char*) ",SS:")) == NULL) 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found ,SS: increment pointer (4 times including ',') to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.SETTLE_START.Val = 0;

                if (strstr((const char*) SetTmrBuff, ",SE:")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                        //if (mData[i]<48 || mData[i]>57)
                        //    SMS_TASKS.Parameters[0] = 0x01;
                    }
                } 
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char *) mData);
                MDB_WRITE_DATA.SETTLE_START.Val = I_Res;
                
                if ((strstr((char *) SetTmrBuff, (const char*) ",SE:")) == NULL) 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found ,SE: increment pointer (4 times including ',') to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.SETTLE_END.Val = 0;

                if (strstr((const char*) SetTmrBuff, ",DS:")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                        //if (mData[i]<48 || mData[i]>57)
                        //    SMS_TASKS.Parameters[0] = 0x01;
                    }
                } 
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char *) mData);
                MDB_WRITE_DATA.SETTLE_END.Val = I_Res;
                
                if ((strstr((char *) SetTmrBuff, (const char*) ",DS:")) == NULL) 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found ,DS: increment pointer (4 times including ',') to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.DECANT_START.Val = 0;

                if (strstr((const char*) SetTmrBuff, ",DE:")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                        //if (mData[i]<48 || mData[i]>57)
                        //    SMS_TASKS.Parameters[0] = 0x01;
                    }
                } 
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char *) mData);
                MDB_WRITE_DATA.DECANT_START.Val = I_Res;
                
                if ((strstr((char *) SetTmrBuff, (const char*) ",DE:")) == NULL) 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found ,DE: increment pointer (4 times including ',') to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.DECANT_END.Val = 0;

                if (strstr((const char*) SetTmrBuff, ",IS:")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                        //if (mData[i]<48 || mData[i]>57)
                        //    SMS_TASKS.Parameters[0] = 0x01;
                    }
                } 
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char *) mData);
                MDB_WRITE_DATA.DECANT_END.Val = I_Res;
                
                if ((strstr((char *) SetTmrBuff, (const char*) ",IS:")) == NULL) 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found ,IS: increment pointer (4 times including ',') to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.IDLE_START.Val = 0;

                if (strstr((const char*) SetTmrBuff, ",IE:")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                        //if (mData[i]<48 || mData[i]>57)
                        //    SMS_TASKS.Parameters[0] = 0x01;
                    }
                } 
                else 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char *) mData);
                MDB_WRITE_DATA.IDLE_START.Val = I_Res;
                
                if ((strstr((char *) SetTmrBuff, (const char*) ",IE:")) == NULL) 
                {
                    //not found
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found ,IE: increment pointer (4 times including ',') to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.IDLE_END.Val = 0;

                for (i = 0; *ptr != '\r'; i++) 
                {
                    mData[i] = *ptr++;
                    //if (mData[i]<48 || mData[i]>57)
                    //    SMS_TASKS.Parameters[0] = 0x01;
                }

                I_Res = atoi((char *) mData);
                MDB_WRITE_DATA.IDLE_END.Val = I_Res;
                
                if ((MDB_WRITE_DATA.BATCH_CYCLE.Val == 0) || (MDB_WRITE_DATA.FILL_END.Val == 0) ||
                        (MDB_WRITE_DATA.AERATION_END.Val == 0) || (MDB_WRITE_DATA.SETTLE_START.Val == 0) ||
                        (MDB_WRITE_DATA.SETTLE_END.Val == 0) || (MDB_WRITE_DATA.DECANT_START.Val == 0) ||
                        (MDB_WRITE_DATA.DECANT_END.Val == 0) || (MDB_WRITE_DATA.IDLE_START.Val == 0) ||
                        (MDB_WRITE_DATA.IDLE_END.Val == 0))
                {
                    SMS_TASKS.Parameters[0] = 0x02;
                }
                
                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    if ((MDB_WRITE_DATA.FILL_START.Val > MDB_WRITE_DATA.FILL_END.Val) || (MDB_WRITE_DATA.AERATION_START.Val > MDB_WRITE_DATA.AERATION_END.Val) ||
                            (MDB_WRITE_DATA.SETTLE_START.Val > MDB_WRITE_DATA.SETTLE_END.Val) || (MDB_WRITE_DATA.DECANT_START.Val > MDB_WRITE_DATA.DECANT_END.Val) ||
                            (MDB_WRITE_DATA.IDLE_START.Val > MDB_WRITE_DATA.IDLE_END.Val) || (MDB_WRITE_DATA.FILL_START.Val > MDB_WRITE_DATA.AERATION_END.Val) ||
                            (MDB_WRITE_DATA.FILL_END.Val > MDB_WRITE_DATA.SETTLE_START.Val) || (MDB_WRITE_DATA.AERATION_END.Val > MDB_WRITE_DATA.SETTLE_START.Val) ||
                            (MDB_WRITE_DATA.SETTLE_END.Val > MDB_WRITE_DATA.DECANT_START.Val) || (MDB_WRITE_DATA.DECANT_END.Val > MDB_WRITE_DATA.IDLE_START.Val) ||
                            (MDB_WRITE_DATA.BATCH_CYCLE.Val < MDB_WRITE_DATA.IDLE_END.Val))
                    {
                        SMS_TASKS.Parameters[0] = 0x02;
                    }
                    
                }
                
                SMS_TASKS.SMS_TASK_STATE = SET_TIMERS;

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    //success
                    InsertSYSTask(SET_TIMERS_TASK);
                }
                else {
                    //send Error response
                    InsertSMSTask(SMS_TASKS);
                    mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                    InsertMODEMTask(mTask);
                }

            }
                    
            /* Set P1 CURRENT SMS*/ 
            /*else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_P1CURRENT_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_P1CURRENT_CMD);
                ptr++; //To avoid space after above string

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                memset(SetCurBuff,0, sizeof (SetCurBuff));
                strcat((char *) SetCurBuff, ptr);

                if ((strstr((char *) SetCurBuff, "min:")) == NULL) 
                {
                    //not found min
                    SMS_TASKS.Parameters[0] = 0x01;
                }
                else 
                {
                    //Found min: increment ponter to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.P1_CUR_MIN_THRLD.Val = 0;

                if (strstr((const char*) SetCurBuff, ",")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                    }
                } 
                else 
                {
                    //not found ','
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                F_Res = atof((char*) mData);
                F_Res *= 1000;
                MDB_WRITE_DATA.P1_CUR_MIN_THRLD.Val = F_Res;

                if ((strstr((char *) SetCurBuff, (const char*) "max:")) == NULL) 
                {
                    //not found max
                    SMS_TASKS.Parameters[0] = 0x03;
                }
                else 
                {
                    //Found max: increment ponter (5 times including ',') to get data
                    ptr += 5;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.P1_CUR_MAX_THRLD.Val = 0;

                for (i = 0; *ptr != '\r'; i++) 
                {
                    mData[i] = *ptr++;
                }

                F_Res = atof((char *) mData);
                F_Res *= 1000;

                if (F_Res == 0.0000) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                } 
                else 
                {
                    MDB_WRITE_DATA.P1_CUR_MAX_THRLD.Val = F_Res;
                }


                SMS_TASKS.SMS_TASK_STATE = SET_P1CURRENT;

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    //success
                    InsertSYSTask(SET_P1CURRENT_TASK);
                }
                else {
                    //send Error response
                    InsertSMSTask(SMS_TASKS);
                            mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                            InsertMODEMTask(mTask);
                }

            }
                    //Set P2 current SMS
                    
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_P2CURRENT_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_P2CURRENT_CMD);
                ptr++; //To avoid space after above string

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                memset(SetCurBuff, 0, sizeof (SetCurBuff));
                strcat((char *) SetCurBuff, (const char*) ptr);

                if ((strstr((char *) SetCurBuff, (const char*) "min:")) == NULL) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                }
                else 
                {
                    //Found min: increment ponter to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.P2_CUR_MIN_THRLD.Val = 0;

                if (strstr((char *) SetCurBuff, ",")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                    }
                } 
                else 
                {
                    //not found ','
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                F_Res = atof((char*) mData);
                F_Res *= 1000;

                MDB_WRITE_DATA.P2_CUR_MIN_THRLD.Val = F_Res;

                if ((strstr((char *) SetCurBuff, (const char*) "max:")) == NULL) 
                {
                    SMS_TASKS.Parameters[0] = 0x03;
                } 
                else 
                {
                    //Found max: increment ponter (5 times including ',') to get data
                    ptr += 5;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.P2_CUR_MAX_THRLD.Val = 0;

                for (i = 0; *ptr != '\r'; i++) 
                {
                    mData[i] = *ptr++;
                }

                F_Res = atof((char *) mData);
                F_Res *= 1000;

                if (F_Res == 0.0000) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                } 
                else 
                {
                    MDB_WRITE_DATA.P2_CUR_MAX_THRLD.Val = F_Res;
                }


                SMS_TASKS.SMS_TASK_STATE = SET_P2CURRENT;

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    //success
                    InsertSYSTask(SET_P2CURRENT_TASK);
                }
                else 
                {
                    //send Error response
                    InsertSMSTask(SMS_TASKS);
                    mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                    InsertMODEMTask(mTask);
                }

            }
                    //Set Pump 3 current SMS
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_P3CURRENT_CMD)) != NULL) 
            {
                ptr += strlen((const char*) SMS_SET_P3CURRENT_CMD);
                ptr++; //To avoid space after above string

                //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                memset(SetCurBuff, 0, sizeof (SetCurBuff));
                strcat((char *) SetCurBuff, ptr);

                if ((strstr((char *) SetCurBuff, (const char*) "min:")) == NULL) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                } 
                else 
                {
                    //Found min: increment pointer to get data
                    ptr += 4;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.P3_CUR_MIN_THRLD.Val = 0;

                if (strstr((char *) SetCurBuff, ",")) 
                {
                    for (i = 0; *ptr != ','; i++) 
                    {
                        mData[i] = *ptr++;
                    }
                } 
                else 
                {
                    //not found ','
                    SMS_TASKS.Parameters[0] = 0x01;
                }

                F_Res = atof((char*) mData);
                F_Res *= 1000;
                MDB_WRITE_DATA.P3_CUR_MIN_THRLD.Val = F_Res;

                if ((strstr((char *) SetCurBuff, (const char*) "max:")) == NULL) 
                {
                    SMS_TASKS.Parameters[0] = 0x03;
                } 
                else 
                {
                    //Found max: increment ponter (5 times including ',') to get data
                    ptr += 5;
                }

                memset(mData, 0, sizeof (mData));
                MDB_WRITE_DATA.P3_CUR_MAX_THRLD.Val = 0;

                for (i = 0; *ptr != '\r'; i++) 
                {
                    mData[i] = *ptr++;
                }

                F_Res = atof((char *) mData);
                F_Res *= 1000;

                if (F_Res == 0.0000) 
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                } 
                else 
                {
                    MDB_WRITE_DATA.P3_CUR_MAX_THRLD.Val = F_Res;
                }


                SMS_TASKS.SMS_TASK_STATE = SET_P3CURRENT;

                if (SMS_TASKS.Parameters[0] == 0xFF) 
                {
                    //success
                    InsertSYSTask(SET_P3CURRENT_TASK);
                }
                else 
                {
                    //send Error response
                    InsertSMSTask(SMS_TASKS);
                    mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                    InsertMODEMTask(mTask);
                }
            }*/
                    /*Reset SBR system */
                    
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char *) SMS_RESETPLC_CMD)) != NULL) 
            {
                //system mode is POWERON mode
                
                MDB_WRITE_DATA.SBR_SYM_MODE = 0x01;

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                InsertSYSTask(RESTART_PLC_TASK);
                SMS_TASKS.SMS_TASK_STATE = RESTART_PLC;
            }

                    /*Set sbr maintenancemode SMS command*/
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char *) SMS_MAINTENANCE_CMD)) != NULL) 
            {
                //sysytem mode is POWERON mode
                
                MDB_WRITE_DATA.SBR_SYM_MODE = 0x09;

                        //Initial value is Success
                SMS_TASKS.Parameters[0] = 0xFF;

                InsertSYSTask(SET_SBR_MAINTENANCE_TASK);
                SMS_TASKS.SMS_TASK_STATE = SET_SBR_MAINTENANCE;
            }
                    /* Write RTC to PLC*/
            else if ((ptr = strstr((const char *) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_PLC_RTC_CMD)) != NULL) {
                ptr += strlen((const char*) SMS_SET_PLC_RTC_CMD);
                ptr++;
                SMS_TASKS.Parameters[0] = 0xFF;
                SMS_TASKS.SMS_TASK_STATE = SET_PLC_RTC_CMD;
                if (PharseServerTime(ptr,1))
                {
                    InsertSYSTask(PLC_RTC_TASK);
                }
                else
                {
                    SMS_TASKS.Parameters[0] = 0x01;
                    InsertSMSTask(SMS_TASKS);
                    mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                    InsertMODEMTask(mTask);
                }
            }
                    /* Get PLC system settings*/
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char *) SMS_GET_PLCSETTINGS_CMD)) != NULL) 
            {
                SMS_TASKS.SMS_TASK_STATE = GET_PLC_SETTINGS;
                InsertSYSTask(GET_PLC_SETTINGS_TASK);
            }
            else if ((ptr = strstr((char*) pMsgRxFifo[pMSG_RX_FIFO_INDEX], (const char*) SMS_SET_RTC_CMD)) != NULL) {
                ptr += strlen((const char*) SMS_SET_RTC_CMD);
                        ptr++; //To avoid space after above string

                        memset(tSYS_PARS.Rtc_Buf, 0, sizeof (tSYS_PARS.Rtc_Buf));

                        //Initial value is Success
                        SMS_TASKS.Parameters[0] = 0xFF;

                for (i = 0; i < RTC_BUF_LENGTH; i++) {
                    tSYS_PARS.Rtc_Buf[i] = *ptr;

                    if (*ptr++ == '\r') {
                        break;
                    }
                }
                tSYS_PARS.Rtc_Buf[i] = '\0';

                if (SMS_TASKS.Parameters[0] == 0xFF) {
                    strcpy((char*) SYS_PARS.Rtc_Buf, (const char*) tSYS_PARS.Rtc_Buf);
                            SetRTC_Dt_Time();
                }

                SMS_TASKS.SMS_TASK_STATE = RTC_CMD;
                        InsertSMSTask(SMS_TASKS);
                        mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                        InsertMODEMTask(mTask);
            }
            else {

                SMS_TASKS.Parameters[0] = 0x02;
                        SMS_TASKS.SMS_TASK_STATE = INVALID_CMD;
                        InsertSMSTask(SMS_TASKS);
                        mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                        InsertMODEMTask(mTask);
            }
            /*******************************************************************************/

#ifdef SMS_NUMBER_VALIDATION
        }
#endif

        memset(pMsgRxFifo[pMSG_RX_FIFO_INDEX], 0, pMSG_RX_WIDTH_SIZE);

                memset(uMsgRxFifo, 0, uMSG_RX_BUFFER_SIZE);
    }
    else {

        memset(pMsgRxFifo[pMSG_RX_FIFO_INDEX], 0, pMSG_RX_WIDTH_SIZE);
                memset(uMsgRxFifo, 0, uMSG_RX_BUFFER_SIZE);
    }

}

void InsertSMSTask(_SMS_TASKS SmsTask) {
    *SmsTaskHeadPtr++ = SmsTask;

    if (SmsTaskHeadPtr >= SmsTaskFifo + sizeof (SmsTaskFifo))
            SmsTaskHeadPtr = SmsTaskFifo;
    }

_SMS_TASKS GetSMSTask(void) {
    _SMS_TASKS SmsTask;

    if (SmsTaskHeadPtr != SmsTaskTailPtr) {
        SmsTask = *SmsTaskTailPtr++;

        if (SmsTaskTailPtr >= SmsTaskFifo + sizeof (SmsTaskFifo))
                SmsTaskTailPtr = SmsTaskFifo;
        } else {

        SmsTask.SMS_TASK_STATE = NO_CMD;
    }

    return SmsTask;
}

void InitSMSTasks(void) {

    SmsTaskHeadPtr = SmsTaskFifo;
            SmsTaskTailPtr = SmsTaskFifo;
}


void pMsgRxFifoPush(BYTE *message) {
    pMsgRxFifo[pMsgRxFifoHead][0] = 0;

            //Replace it with memcpy
            strcpy((char *) pMsgRxFifo[pMsgRxFifoHead], (const char*) message);

    if (++pMsgRxFifoHead == pMsgRxFifoTail)
            --pMsgRxFifoHead;

        if (pMsgRxFifoHead >= pMSG_RX_FIFO_INDEX) {

            if (pMsgRxFifoTail != 0)
                    pMsgRxFifoHead = 0;
            }
}

char *pMsgRxFifoPop(void) {
    char *ptr;
    if (pMsgRxFifoTail >= pMSG_RX_FIFO_INDEX)
            pMsgRxFifoTail = 0;

        if (pCheckMsgRxFifo()) {
            ptr = (char *) pMsgRxFifo[pMsgRxFifoTail++];
            return ptr;
        }

          else
            return 0;
        }

UINT32 pCheckMsgRxFifo(void) {
    if (pMsgRxFifoHead != pMsgRxFifoTail)
        return 1;

    else
        return 0;
    }

_MODEM_STATE CheckQINISTATTask(void) {
    switch (MODEM_TASK.MODEM_TASK_STATE) {
        case MODEM_SEND_CLT_SUMMERY_TASK:
        case MODEM_SEND_CLT_STATUS_TASK:
        case MODEM_SEND_EM_PROFILE_TASK:
        case MODEM_SEND_ALARMS_TASK:
        case MODEM_SEND_DEV_INFO_TASK:
        case MODEM_SEND_DEV_ID_TASK:
        {
            return MODEM_QIFGCNT;
            break;
        }

        case MODEM_SEND_SMS_TASK:
        case MODEM_READ_SMS_TASK:
        {
            return MODEM_CMGF;

            break;
        }

        default:
            return 0;
            break;
    }

}

_MODEM_STATE CheckCMGFTask(void) {
    switch (MODEM_TASK.MODEM_TASK_STATE) {
        case MODEM_READ_SMS_TASK:
        {
            return MODEM_CMGR;
            break;
        }

        case MODEM_SEND_SMS_TASK:
        {
            return MODEM_CMGS;

            break;
        }

        default:
            return 0;
            break;
    }

}

void CheckDataType(void) {
    switch (MODEM_TASK.MODEM_TASK_STATE) {

        case MODEM_SEND_CLT_SUMMERY_TASK:
        case MODEM_SEND_CLT_STATUS_TASK:
        case MODEM_SEND_EM_PROFILE_TASK:
        case MODEM_SEND_ALARMS_TASK:
        case MODEM_SEND_DEV_INFO_TASK:
        case MODEM_SEND_DEV_ID_TASK:
        {
            //for domain name
#ifdef NON_TRANSPERANT_MODE
            PutModemUART("+QIMODE=0", MODEM_COMMAND, strlen("+QIMODE=0"));
#else

            PutModemUART("+QIMODE=1", MODEM_COMMAND, strlen("+QIMODE=1"));
#endif
                    break;
        }

        default:
            break;
    }
}

BYTE* CheckSMSCmd(void) {
    //BYTE tmp[128];
            BYTE * SMS_RespBufferPtr;
            _SMS_TASKS SMS_TASK;

            memset(SMS_RespBuffer, '\0', sizeof (SMS_RespBuffer));

            SMS_TASK = GetSMSTask();
            SMS_TASKS.Parameters[0] = SMS_TASK.Parameters[0];

            Reserve_SMS_State = SMS_TASK.SMS_TASK_STATE;
            
    switch (SMS_TASK.SMS_TASK_STATE) {
        case SERVER_ADDR_CMD:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_SERVERADDR_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_SERVERADDR_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case NET_APN_CMD:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_SIM_APN_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_SIM_APN_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case APP_ID_CMD:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_APPID_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_APPID_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case DEV_TYPE_CMD:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_DEVTYPE_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_DEVTYPE_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case RMCU_ID_CMD:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_RMCUID_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_RMCUID_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case MN_CMD:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_MN_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                        strcat((char *) SMS_RespBuffer, "\r\n");
                        FLAGS.MobileNumberChanged = 0;
                        
                        strcpy((char*) MOBILENUMBER, (const char*) Old_Config_MobileNumber);
                        
                        SMS_TASKS.SMS_TASK_STATE = MN_CMD_RESP_CONFIG_NUM;
                        InsertSMSTask(SMS_TASKS);
                        mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                        InsertMODEMTask(mTask);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_MN_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case MN_CMD_RESP_CONFIG_NUM:
            strcat((char *) SMS_RespBuffer, (const char*) SMS_MN_ACK_HEAD);
            strcat((char *) SMS_RespBuffer, (const char*) SYS_PARS.MobileNum);
            strcat((char *) SMS_RespBuffer, (const char*) SMS_MN_ACK_TAIL);
            strcpy((char*) MOBILENUMBER, (const char*) NEW_MOBILENUMBER);
            return SMS_RespBuffer;
            break;

        case TASK_DURATION_CMD:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_WEBPERIOD_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_WEBPERIOD_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case RTC_CMD:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_RTC_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_RTC_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case GET_RMCU_SETTINGS:
            //Success

            memset(RMCU_ID, 0, sizeof (RMCU_ID));
                    memset(MOBILENUMBER, 0, sizeof (MOBILENUMBER));
                    memset(SERVER_ADDRESS, 0, sizeof (SERVER_ADDRESS));
                    memset(APN, 0, sizeof (APN));
                    memset(APP_ID, 0, sizeof (APP_ID));
                    memset(DEV_TYPE, 0, sizeof (DEV_TYPE));

                    strcpy((char*) RMCU_ID, (const char*) SYS_PARS.RMCU_Id);
                    strcpy((char*) MOBILENUMBER, (const char*) SYS_PARS.MobileNum);
                    strcpy((char*) SERVER_ADDRESS, (const char*) SYS_PARS.Server_Address);
                    strcpy((char*) APN, (const char*) SYS_PARS.APN);
                    strcpy((char*) APP_ID, (const char*) SYS_PARS.App_Id);
                    strcpy((char*) DEV_TYPE, (const char*) SYS_PARS.Dev_Type);

            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) "RMCU ID: ");
                        strcat((char *) SMS_RespBuffer, (const char*) SYS_PARS.RMCU_Id);
                        strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                        strcat((char *) SMS_RespBuffer, (const char*) "Configuration No: ");
                        strcat((char *) SMS_RespBuffer, (const char*) SYS_PARS.MobileNum);
                        strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                        strcat((char *) SMS_RespBuffer, (const char*) "Server Addr: ");
                        strcat((char *) SMS_RespBuffer, (const char*) SYS_PARS.Server_Address);
                        strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                        strcat((char *) SMS_RespBuffer, (const char*) "APN Addr: ");
                        strcat((char *) SMS_RespBuffer, (const char*) SYS_PARS.APN);
                        strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                        strcat((char *) SMS_RespBuffer, (const char*) "APP ID: ");
                        strcat((char *) SMS_RespBuffer, (const char*) SYS_PARS.App_Id);
                        strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                        strcat((char *) SMS_RespBuffer, (const char*) "Dev Type: ");
                        strcat((char *) SMS_RespBuffer, (const char*) SYS_PARS.Dev_Type);
                        strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_GET_RMCU_SETTINGS);
                return SMS_RespBufferPtr;
            }
            break;

        case RESTART_RMCU:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_RESTART_RMCU);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                        //GetReceivedMobileNumber();
                        strcpy((char *)MOBILENUMBER,(const char *)SYS_PARS.ReceivedMobileNum);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_RESTART_RMCU);
                return SMS_RespBufferPtr;
            }
            break;

        case RESTART_PLC:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_RESETPLC_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_RESETPLC_CMD);
                return SMS_RespBufferPtr;
            }
            break;
                    
        case SET_SBR_MAINTENANCE:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_MAINTENANCE_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_MAINTENANCE_CMD);
                return SMS_RespBufferPtr;
            }
            break;    
            
        /*case SET_TIMER1:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_TIMER1_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_TIMER1_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case SET_TIMER2:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_TIMER2_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_TIMER2_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case SET_TIMER3:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_TIMER3_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_TIMER3_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case SET_TIMER4:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_TIMER4_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_TIMER4_CMD);
                return SMS_RespBufferPtr;
            }
            break;*/

        case SET_TIMERS:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_TIMERS_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_TIMERS_CMD);
                return SMS_RespBufferPtr;
            }
            break;
            
        /*case SET_P1CURRENT:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_P1CURRENT_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_P1CURRENT_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case SET_P2CURRENT:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_P2CURRENT_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_P2CURRENT_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case SET_P3CURRENT:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_P3CURRENT_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_P3CURRENT_CMD);
                return SMS_RespBufferPtr;
            }
            break;*/
            
        case SET_PLC_RTC_CMD:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SMS_SET_PLC_RTC_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_SET_PLC_RTC_CMD);
                return SMS_RespBufferPtr;
            }
            break;
            
        case SERVER_TIME_TO_PLC:
            //Success
            if (SMS_TASKS.Parameters[0] == 0xFF) {
                strcat((char *) SMS_RespBuffer, (const char*) SERVER_TIME_TO_PLC_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_SUCCESS_ACK);
                return SMS_RespBuffer;
            } else {
                strcat((char *) SMS_RespBuffer, (const char*) SERVER_TIME_TO_PLC_CMD);
                        strcat((char *) SMS_RespBuffer, (const char*) SMS_ERROR_ACK);
                return SMS_RespBuffer;
            }
            break;

            
        case GET_PLC_SETTINGS:
            //Success

            memset(WEBPERIOD, 0, sizeof (WEBPERIOD));
            strcpy((char*) WEBPERIOD, (const char*) SYS_PARS.TaskDuration);

            if (SMS_TASKS.Parameters[0] == 0xFF) 
            {
                strcat((char *) SMS_RespBuffer, (const char*) "Web Period: ");
                strcat((char *) SMS_RespBuffer, (const char*) WEBPERIOD);
                strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                /*strcat((char *) SMS_RespBuffer, (const char*) "Timer1: ");
                itoa((char*) tmp, RMC_SBR_SYS_PAR.TIMER1_SECS.Val, 10);
                strcat((char *) SMS_RespBuffer, (const char*) tmp);
                strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                strcat((char *) SMS_RespBuffer, (const char*) "Timer2: ");
                itoa((char*) tmp, RMC_SBR_SYS_PAR.TIMER2_SECS.Val, 10);
                strcat((char *) SMS_RespBuffer, (const char*) tmp);
                strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                strcat((char *) SMS_RespBuffer, (const char*) "Timer3: ");
                sprintf((char*) tmp,"%u", RMC_SBR_SYS_PAR.TIMER3_SECS.Val);
                strcat((char *) SMS_RespBuffer, (const char*) tmp);
                strcat((char *) SMS_RespBuffer, (const char*) "\r\n");
                     
                strcat((char *) SMS_RespBuffer, (const char*) "Timer4: ");
                sprintf((char*) tmp,"%u", RMC_SBR_SYS_PAR.TIMER4_SECS.Val);
                strcat((char *) SMS_RespBuffer, (const char*) tmp);
                strcat((char *) SMS_RespBuffer, (const char*) "\r\n");
                
                strcat((char *) SMS_RespBuffer, (const char*) "Pump1Current:");
                strcat((char *) SMS_RespBuffer, (const char*) "min:");
                sprintf((char *) tmp, "%f", RMC_SBR_SYS_PAR_FLOAT.P1_CUR_MIN_THRLD);
                strcat((char *) SMS_RespBuffer, (const char*) tmp);
                strcat((char *) SMS_RespBuffer, (const char*) ",max: ");
                sprintf((char *) tmp, "%f", RMC_SBR_SYS_PAR_FLOAT.P1_CUR_MAX_THRLD);
                strcat((char *) SMS_RespBuffer, (const char*) tmp);
                strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                strcat((char *) SMS_RespBuffer, (const char*) "Pump2Current:");
                strcat((char *) SMS_RespBuffer, (const char*) "min:");
                sprintf((char *) tmp, "%f", RMC_SBR_SYS_PAR_FLOAT.P2_CUR_MIN_THRLD);
                strcat((char *) SMS_RespBuffer, (const char*) tmp);
                strcat((char *) SMS_RespBuffer, (const char*) ",max: ");
                sprintf((char *) tmp, "%f", RMC_SBR_SYS_PAR_FLOAT.P2_CUR_MAX_THRLD);
                strcat((char *) SMS_RespBuffer, (const char*) tmp);
                strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                strcat((char *) SMS_RespBuffer, (const char*) "Pump3Current:");
                strcat((char *) SMS_RespBuffer, (const char*) "min:");
                sprintf((char *) tmp, "%f", RMC_SBR_SYS_PAR_FLOAT.P3_CUR_MIN_THRLD);
                strcat((char *) SMS_RespBuffer, (const char*) tmp);
                strcat((char *) SMS_RespBuffer, (const char*) ",max: ");
                sprintf((char *) tmp, "%f", RMC_SBR_SYS_PAR_FLOAT.P3_CUR_MAX_THRLD);
                strcat((char *) SMS_RespBuffer, (const char*) tmp);*/
                strcat((char *) SMS_RespBuffer, (const char*) "\r\n");

                return SMS_RespBuffer;
            } else {
                SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_GET_PLCSETTINGS_CMD);
                return SMS_RespBufferPtr;
            }
            break;

        case INVALID_CMD:
            SMS_RespBufferPtr = ConstructErrorResponseString(SMS_TASKS.Parameters[0], SMS_INVALID_CMD_ACK);
            return SMS_RespBufferPtr;

            break;

        default:
            return 0;
            break;
    }

}


_MODEM_STATE CheckConClose(void) {
    switch (MODEM_TASK.MODEM_TASK_STATE) {
        case MODEM_SEND_CLT_SUMMERY_TASK:
        case MODEM_SEND_CLT_STATUS_TASK:
        case MODEM_SEND_EM_PROFILE_TASK:
        case MODEM_SEND_ALARMS_TASK:
        case MODEM_SEND_DEV_INFO_TASK:
        case MODEM_SEND_DEV_ID_TASK:

            return CheckDataTerminate();

            break;

        default:
            return 0;
            break;
    }

}

#if 0
_MODEM_STATE CheckDataTerminate(void) {

    switch (MODEM_TASK.MODEM_TASK_STATE) {
        case MODEM_SEND_CLT_SUMMERY_TASK:
        case MODEM_SEND_CLT_STATUS_TASK:
        case MODEM_SEND_EM_PROFILE_TASK:

            if (!PacketSent) {
                CopyPacket2ModemBuffer(&ACE_CLT_SUM_PCKT);
                        AckRxd = 0;
                return MODEM_GREATER;
            }
            else if (PacketSent == 1) {
                CopyPacket2ModemBuffer(&ACE_CLT_STAT_PCKT);
                        AckRxd = 0;
                return MODEM_GREATER;
            }
            else if (PacketSent == 2) {
                CopyPacket2ModemBuffer(&ACE_CLT_EM_PCKT);
                        AckRxd = 0;
                return MODEM_GREATER;
            }
            break;

        case MODEM_SEND_ALARMS_TASK:
            CopyPacket2ModemBuffer(&ACE_CLT_ALM_PCKT);
            return MODEM_GREATER;
            break;

        case MODEM_SEND_DEV_INFO_TASK:
            CopyPacket2ModemBuffer(&ACE_CLT_ALM_PCKT);
            return MODEM_GREATER;

            break;

        case MODEM_READ_SMS_TASK:
            break;

        case MODEM_SEND_SMS_TASK:
            break;

        default:
            return 0;
            break;
    }

}
#endif


_MODEM_STATE CheckDataTerminate(void) {
    _MODEM_STATE tMODEM_STATE;

    switch (MODEM_TASK.MODEM_TASK_STATE) {
        case MODEM_SEND_CLT_SUMMERY_TASK:
        case MODEM_SEND_CLT_STATUS_TASK:
        case MODEM_SEND_EM_PROFILE_TASK:

            if (!PacketSent) {
                memset(ModemTxBuffer.pGPRSData, 0, sizeof (ModemTxBuffer.pGPRSData));
                        CopyPacket2ModemBufferAll(&ACE_SBR_SUM_PCKT);
                        CopyPacket2ModemBufferAll(&ACE_SBR_STAT_PCKT);
                        CopyPacket2ModemBufferAll(&ACE_SBR_EM_PCKT);
                        
                        if (RMC_SBR_SYS_SET.SBR_ISR.bits.b1 || (Dummy_SMS_Set == 1))
                        {
                            CopyPacket2ModemBufferAll(&ACE_SBR_SET_PCKT);
                            RMC_SBR_SYS_SET.SBR_ISR.bits.b1 = 0;
                            Dummy_SMS_Set = 0;
                        }
                            ConstructGPRSPacket();
                        AckRxd = 0;
                        tMODEM_STATE = MODEM_GREATER;
            }
            break;

        case MODEM_SEND_ALARMS_TASK:
            CopyPacket2ModemBuffer(&ACE_SBR_ALM_PCKT);
                    tMODEM_STATE = MODEM_GREATER;
            break;

        case MODEM_SEND_DEV_INFO_TASK:
            CopyPacket2ModemBuffer(&ACE_SBR_ALM_PCKT);
                    tMODEM_STATE = MODEM_GREATER;

            break;
            
        case MODEM_SEND_DEV_ID_TASK:
            CopyPacket2ModemBuffer(&ACE_DEV_ID_PCKT);
            tMODEM_STATE = MODEM_GREATER;
            break;

        case MODEM_READ_SMS_TASK:
            break;

        case MODEM_SEND_SMS_TASK:
            break;

        default:
            return 0;
            break;
    }
    return tMODEM_STATE;
}

_MODEM_STATE CheckACKfromServer(void) {
    if ((AckNotRxd >= SERVER_RESP_TIMEOUT_COUNT) || (RetransmitCount >= SERVER_RESP_TIMEOUT_COUNT)) {
        AckNotRxd = 0;
                RetransmitCount = 0;
                DsModemTimerInterrupt();

#ifdef DOMAIN_NAME
                return MODEM_QICLOSE;
#else
                return MODEM_TRIPPLE_PLUS;
#endif
    }

    if (AckRxd && FLAGS.Server_Status_Success) {
        AckRxd = 0;
                AckNotRxd = 0;
                PacketSent++;
                FLAGS.Server_Status_Success = 0;
                DsModemTimerInterrupt();

        if ((MODEM_TASK.MODEM_TASK_STATE == MODEM_SEND_ALARMS_TASK) || (MODEM_TASK.MODEM_TASK_STATE == MODEM_SEND_DEV_ID_TASK)) {
            PacketSent = 0;
#ifdef DOMAIN_NAME
                    return MODEM_QICLOSE;
#else
                    return MODEM_TRIPPLE_PLUS;
#endif
        }
        else {
            if (PacketSent) {
                PacketSent = 0;
#ifdef DOMAIN_NAME
                        MODEM_CONNECTION_STATUS = SERVER_DISCONNECTED;
                return MODEM_QICLOSE;
#else
                        return MODEM_TRIPPLE_PLUS;
#endif
            } else {
                return MODEM_QISEND;
            }
        }
    }
    else if (AckRxd && (!FLAGS.Server_Status_Success)) {
        //ERROR rxd
        AckRxd = 0;
                AckNotRxd = 0;
                RetransmitCount++;
                DsModemTimerInterrupt();
                MODEM_CONNECTION_STATUS = SERVER_DISCONNECTED;
                InsertSYSTask(CLT_SYS_READ_TASK);
        return MODEM_QICLOSE;
    }
    else {
        //Response timeout
        AckNotRxd++;
                MODEM_CONNECTION_STATUS = SERVER_DISCONNECTED;
                InsertSYSTask(CLT_SYS_READ_TASK);
        if (!FLAGS.ConnectionTimeOut) {
            return MODEM_QICLOSE;
        } else {
            FLAGS.ConnectionTimeOut = 0;

            return MODEM_QIDEACT;
        }
    }

}



void CopyPacket2ModemBuffer(_ACE_SBR_MSG_PCKT *message) {
    UINT16_VAL cRes;
            UINT8 Fcc = 0, byt1 = 0, byt2 = 0;
            UINT8 temp[10], bytstrm[10];
            UINT16 i = 0, Bytecnt = 0, ModemTxBuffer_Index1;

            ModemTxBuffer_Index = 0;


            memset(ModemTxBuffer.pGPRSData, 0, sizeof (ModemTxBuffer.pGPRSData));
            memset(pGPRSTxFifo.pGPRSData, 0, sizeof (pGPRSTxFifo.pGPRSData));

            strcpy((char*) ModemTxBuffer.pGPRSData, (const char*) message->Data);
            ModemTxBuffer_Index = strlen((const char*) ModemTxBuffer.pGPRSData);

            //Append FCS and STOP byte here
            //observed index goes beyond data and feteches NULL to fix that below lines are added.
            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index] = '0';
            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index + 1] = '0';
            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index + 2] = '0';
            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index + 3] = '0';
            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index + 4] = '0';
            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index + 5] = '0';

    for (Bytecnt = 2; Bytecnt <= (ModemTxBuffer_Index - 2); i++) {

        bytstrm[0] = ModemTxBuffer.pGPRSData[Bytecnt++];
                bytstrm[1] = ModemTxBuffer.pGPRSData[Bytecnt++];
                bytstrm[2] = '\0';

                byt1 = ASCII_TO_HEX(bytstrm[0], bytstrm[1]);

                bytstrm[0] = ModemTxBuffer.pGPRSData[Bytecnt++];
                bytstrm[1] = ModemTxBuffer.pGPRSData[Bytecnt++];
                bytstrm[2] = '\0';
                i++;

                byt2 = ASCII_TO_HEX(bytstrm[0], bytstrm[1]);

                Fcc ^= byt1;
                Fcc ^= byt2;

    }

    cRes = Hex2Ascii_Frame(Fcc);
            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index++] = cRes.byte.HB;
            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index++] = cRes.byte.LB;

            cRes = Hex2Ascii_Frame(0x3E);
            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index++] = cRes.byte.HB;
            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index++] = cRes.byte.LB;

            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index++] = 0x1A; //added on 17/06/15

            ModemTxBuffer.Type = MODEM_SENSOR_DATA;
            ModemTxBuffer.Size = ModemTxBuffer_Index;

            //copy HTTP header
            strcat((char *) pGPRSTxFifo.pGPRSData, (const char*) HTTPSTR1);
            Bytecnt = ModemTxBuffer_Index - 1;
            itoa((char *) temp, Bytecnt, 10);
            strcat((char *) pGPRSTxFifo.pGPRSData, (const char*) temp);
            strcat((char*) pGPRSTxFifo.pGPRSData, (const char*) HTTPSTR2);

            //copy HTTP header size
            ModemTxBuffer_Index1 = strlen((const char*) pGPRSTxFifo.pGPRSData);
            strcat((char *) pGPRSTxFifo.pGPRSData, (const char*) ModemTxBuffer.pGPRSData);

            pGPRSTxFifo.Size = ModemTxBuffer.Size + ModemTxBuffer_Index1;
            pGPRSTxFifo.Type = ModemTxBuffer.Type;
}

void CopyPacket2ModemBufferAll(_ACE_SBR_MSG_PCKT *message) {
    UINT16_VAL cRes;
            BYTE tModemTxBuffer[1024];
            UINT8 Fcc = 0, byt1 = 0, byt2 = 0;
            UINT8 bytstrm[10];
            UINT16 Bytecnt = 0, i;

            ModemTxBuffer_Index = 0;


            memset(tModemTxBuffer, 0, sizeof (tModemTxBuffer));

            strcpy((char*) tModemTxBuffer, (const char*) message->Data);
            ModemTxBuffer_Index = strlen((const char*) tModemTxBuffer);

            //Append FCS and STOP byte here
            //observed index goes beyond data and fetches NULL to fix that below lines are added.
            tModemTxBuffer[ModemTxBuffer_Index] = '0';
            tModemTxBuffer[ModemTxBuffer_Index + 1] = '0';
            tModemTxBuffer[ModemTxBuffer_Index + 2] = '0';
            tModemTxBuffer[ModemTxBuffer_Index + 3] = '0';

    for (Bytecnt = 2; Bytecnt <= (ModemTxBuffer_Index - 2); i++) {

        bytstrm[0] = tModemTxBuffer[Bytecnt++];
                bytstrm[1] = tModemTxBuffer[Bytecnt++];
                bytstrm[2] = '\0';

                byt1 = ASCII_TO_HEX(bytstrm[0], bytstrm[1]);

                bytstrm[0] = tModemTxBuffer[Bytecnt++];
                bytstrm[1] = tModemTxBuffer[Bytecnt++];
                bytstrm[2] = '\0';
                i++;

                byt2 = ASCII_TO_HEX(bytstrm[0], bytstrm[1]);

                Fcc ^= byt1;
                Fcc ^= byt2;

    }

    cRes = Hex2Ascii_Frame(Fcc);
            tModemTxBuffer[ModemTxBuffer_Index++] = cRes.byte.HB;
            tModemTxBuffer[ModemTxBuffer_Index++] = cRes.byte.LB;

            cRes = Hex2Ascii_Frame(0x3E);
            tModemTxBuffer[ModemTxBuffer_Index++] = cRes.byte.HB;
            tModemTxBuffer[ModemTxBuffer_Index++] = cRes.byte.LB;

            strcat((char*) tModemTxBuffer, "\r\n");
            strcat((char*) ModemTxBuffer.pGPRSData, (const char*) tModemTxBuffer);
}

void ConstructGPRSPacket(void) {

    UINT8 temp[10];
            UINT16 Bytecnt = 0, ModemTxBuffer_Index1;

            memset(pGPRSTxFifo.pGPRSData, 0, sizeof (pGPRSTxFifo.pGPRSData));

            ModemTxBuffer_Index = strlen((const char*) ModemTxBuffer.pGPRSData);

            ModemTxBuffer.pGPRSData[ModemTxBuffer_Index++] = 0x1A; //added on 17/06/15

            ModemTxBuffer.Type = MODEM_SENSOR_DATA;
            ModemTxBuffer.Size = ModemTxBuffer_Index;

            //copy HTTP header
            strcat((char *) pGPRSTxFifo.pGPRSData, (const char*) HTTPSTR1);
            Bytecnt = ModemTxBuffer_Index - 1;
            itoa((char *) temp, Bytecnt, 10);
            strcat((char *) pGPRSTxFifo.pGPRSData, (const char*) temp);
            strcat((char*) pGPRSTxFifo.pGPRSData, (const char*) HTTPSTR2);

            //copy HTTP header size
            ModemTxBuffer_Index1 = strlen((const char*) pGPRSTxFifo.pGPRSData);
            strcat((char *) pGPRSTxFifo.pGPRSData, (const char*) ModemTxBuffer.pGPRSData);

            pGPRSTxFifo.Size = ModemTxBuffer.Size + ModemTxBuffer_Index1;
            pGPRSTxFifo.Type = ModemTxBuffer.Type;
}


BYTE *ConstructErrorResponseString(BYTE Errcode, const BYTE *RespString) {
    strcat((char *) SMS_RespBuffer, (const char*) RespString);
            strcat((char *) SMS_RespBuffer, (const char*) SMS_ERROR_ACK);

    switch (Errcode) {
        case INVALID_PARAMETER_ERR:
            strcat((char *) SMS_RespBuffer, (const char*) INVALID_PARAMETER);
            break;

        case INVALID_COMMAND_ERR:
            strcat((char *) SMS_RespBuffer, (const char*) INVALID_COMMAND);
            break;

        case INCOMPLETE_PARAMETERS_ERR:
            strcat((char *) SMS_RespBuffer, (const char*) INCOMPLETE_PARAMETERS);
            break;

        case PLC_COMM_FAILURE_ERR:
            strcat((char *) SMS_RespBuffer, (const char*) PLC_COMM_FAILURE);
            break;

        case FAILEDTO_SAVEDATAIN_PLC_ERR:
            strcat((char *) SMS_RespBuffer, (const char*) FAILEDTO_SAVEDATAIN_PLC);
            break;

        case FAILEDTO_SAVEDATAIN_RMCU_ERR:
            strcat((char *) SMS_RespBuffer, (const char*) FAILEDTO_SAVEDATAIN_RMCU);

            break;

        default:
            break;
    }

    return SMS_RespBuffer;
}

void SetRTC_Dt_Time(void) {

    NRTS.Date = ASCII_TO_HEX(SYS_PARS.Rtc_Buf[0], SYS_PARS.Rtc_Buf[1]);
            NRTS.Month = ASCII_TO_HEX(SYS_PARS.Rtc_Buf[3], SYS_PARS.Rtc_Buf[4]);
            NRTS.Year = ASCII_TO_HEX(SYS_PARS.Rtc_Buf[6], SYS_PARS.Rtc_Buf[7]);

            NRTS.Hour = ASCII_TO_HEX(SYS_PARS.Rtc_Buf[9], SYS_PARS.Rtc_Buf[10]);
            NRTS.Minute = ASCII_TO_HEX(SYS_PARS.Rtc_Buf[12], SYS_PARS.Rtc_Buf[13]);
            NRTS.Second = ASCII_TO_HEX(SYS_PARS.Rtc_Buf[15], SYS_PARS.Rtc_Buf[16]);

            RTC_Pre_Config();
            SetRtcDate(NRTS.Date, NRTS.Month, NRTS.Year);
            SetRtcTime(NRTS.Second, NRTS.Minute, NRTS.Hour);
            RTC_Post_Config();

            memset(tSYS_PARS.Rtc_Buf, 0, RTC_BUF_LENGTH);
}

void ReloadRestartCounter(void) {
    /*If execution doesn't reach the send part it will restart the unit
     This counter will be 2 mins plus web period value*/
    RestartCounter = ((WebPeriod + 2)*60000);
}

void RTCErrorResponse(void)
{
    SMS_TASKS.SMS_TASK_STATE = SERVER_TIME_TO_PLC;
    SMS_TASKS.Parameters[0] = 0x01;
    InsertSMSTask(SMS_TASKS);
    mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
    InsertMODEMTask(mTask);
}