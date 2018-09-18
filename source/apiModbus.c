#include <stdio.h>
#include <stdlib.h>
#include <cTimer.h>
#include "libI2C.h"
#include "apiRTC.h"
#include "apiTask.h"
#include <psystem.h>
#include <apiTick.h>
#include "apiDelay.h"
#include <apiModem.h>
#include <apiModbus.h>
#include "libModbus.h"
#include "apiEEPROM.h"
#include <apiHelpers.h>
#include <libModemUART.h>
#include <libModbusUART.h>
#include <ctype.h>

static UINT8 CLASS_Sys_settings_Read_flag = 0;

const BYTE  AES_SRT_BYTE[]          =   "3C";
const BYTE  AES_MSG_CODE[]          =   "A100";
const BYTE  AES_STATUS[]            =   "03";
const BYTE  AES_RTRY_CNTR[]         =   "00";
const BYTE  AES_RESP_FOLLOW[]       =   "00";
const BYTE  AES_SUMMERY_PARM_NUM[]  =   "06";
const BYTE  AES_STATUS_PARM_NUM[]   =   "0C";
const BYTE  AES_EM_PARM_NUM[]       =   "06";
const BYTE  AES_ALM_PARM_NUM[]      =   "01";
const BYTE  AES_DEV_ID[]            =   "200000000000B00D";  // New end point for EcoSan
const BYTE  AES_DEV_TYPE[]          =   "C034";
const BYTE  AES_APP_ID[]            =   "00000002";
const BYTE  AES_CONST1[]            =   "01";
const BYTE  AES_CONST2[]            =   "02";
const BYTE  AES_CONST3[]            =   "03";
const BYTE  AES_CONST4[]            =   "04";
const BYTE  AES_SUMMERY_CONST2[]    =   "F029";
const BYTE  AES_STATUS_CONST2[]     =   "F02A";
const BYTE  AES_SETTING_CONST2[]    =   "F035";
const BYTE  AES_EM_CONST2[]         =   "F02B";
const BYTE  AES_ALM_CONST2[]        =   "F02A";
const BYTE  AES_STOP_BYTE[]         =   "3E";
const BYTE  AES_CONST5[]            =   "F0";
const BYTE  AES_ID_CONST[]          =   "F001";


const BYTE  PARAM_CODE_1[]              =   "0001";
const BYTE  PARAM_CODE_2[]              =   "0002";
const BYTE  PARAM_CODE_3[]              =   "0003";
const BYTE  PARAM_CODE_4[]              =   "0004";
const BYTE  PARAM_CODE_5[]              =   "0005";
const BYTE  PARAM_CODE_6[]              =   "0006";
const BYTE  PARAM_CODE_7[]              =   "0007";
const BYTE  PARAM_CODE_8[]              =   "0008";
const BYTE  PARAM_CODE_9[]              =   "0009";
const BYTE  PARAM_CODE_10[]             =   "000A";
const BYTE  PARAM_CODE_11[]             =   "000B";
const BYTE  PARAM_CODE_12[]             =   "000C";
const BYTE  PARAM_CODE_13[]             =   "000D";
const BYTE  PARAM_CODE_14[]             =   "000E";
const BYTE  PARAM_CODE_15[]             =   "000F";
const BYTE  PARAM_CODE_16[]             =   "0010";
const BYTE  PARAM_CODE_17[]             =   "0011";
const BYTE  PARAM_CODE_18[]             =   "0012";
const BYTE  PARAM_CODE_19[]             =   "0013";
const BYTE  PARAM_CODE_20[]             =   "0014";
const BYTE  PARAM_CODE_21[]             =   "0015";
const BYTE  PARAM_CODE_22[]             =   "0016";
const BYTE  PARAM_CODE_23[]             =   "0017";
const BYTE  PARAM_CODE_24[]             =   "0018";
const BYTE  PARAM_CODE_25[]             =   "0019";
const BYTE  PARAM_CODE_26[]             =   "001A";
const BYTE  PARAM_CODE_27[]             =   "001B";

const BYTE  PARAM_CODE_ALM[]            =   "0101";

const BYTE SBR_SYS_Mode[10][13]={"POWER ON    ","FILL        ","AERATE      ","SETTLING    ","DECANT      ","IDLE        ",
                             "STOP        ","DEBUG       ","MAINTENANCE ","FILL,AERATE "};
const BYTE SBR_SEN_STATUS[2][4]={"OFF","ON "};

UINT32_VAL Read_res;
UINT32_VAL TmpResult;
UINT16 mRespBytCount = 0;
UINT16 ModbusErrCnt = 0;
UINT16 PrevDateTimeCount = 0;
UINT16 InvalidDateTimeCount = 0;
volatile unsigned char MBResponse = 0;
volatile unsigned char MBResponse_2 = 0;

//static UINT8 plc_reg_process_switch_flag = 0;

UINT8 TempRxBuffer[1024];
UINT8 DataBuffer[25];
volatile UINT8 mdbWriteBuffer[25];
extern UINT8 INTProcessPending;

UINT8 PLC_DateandTime = 0;
UINT8 MBWrite_Success = 0;
UINT8 Write_Count = 0;
UINT16 Dummy_SMS_Set = 0;

 _FLAGS FLAGS;
extern _SYS_PARS SYS_PARS;


extern BYTE RMCU_ID[20];
extern BYTE DEV_TYPE[6];

extern _MODEM_TASKS mTask;
extern MDB_REQ_ADU mdbReqAdu;
extern MDB_RSP_ADU mdbRspAdu;
extern MDB_RSP_ADU MdbRespBuffer;
extern _MODEM_TASKS MODEM_TASK;
extern _MODEM_STATE MODEM_STATE;


_DATETIME DATE_TIME;
_DATETIME Prev_Date_Time;
_RMC_ALARM RMC_ALARM;
_MDB_WRITE_DATA MDB_WRITE_DATA;
_RMC_SBR_SYS_PAR RMC_SBR_SYS_PAR;
_RMC_SBR_SYS_SET RMC_SBR_SYS_SET;
_RMC_ENG_MTR_PAR RMC_ENG_MTR_PAR;
_RMC_SBR_SYS_PAR_FLOAT RMC_SBR_SYS_PAR_FLOAT;

_ACE_SBR_MSG_PCKT ACE_SBR_SUM_PCKT;
_ACE_SBR_MSG_PCKT ACE_SBR_STAT_PCKT;
_ACE_SBR_MSG_PCKT ACE_SBR_EM_PCKT;
_ACE_SBR_MSG_PCKT ACE_SBR_SET_PCKT;
_ACE_SBR_MSG_PCKT ACE_SBR_ALM_PCKT;
_ACE_SBR_MSG_PCKT ACE_DEV_ID_PCKT;


 _MODBUS_STATE MODBUS_STATE;
 _MODBUS_STATE cMODBUS_STATE;
 _MODBUS_STATE nMODBUS_STATE;
 _MODBUS_STATE MODBUS_RESP_STATE;
 
 static unsigned long int calculateDatetimeInMinutes(_DATETIME dateTimeBuff);

void ModbusStack(void)
{

    switch(MODBUS_STATE)
    {
        case MODBUS_ENTRY_STATE:
            break;

            /*Read commands*/
        case MODBUS_RMC_SBR_SYS_PAR:
            #ifdef GET_TIMESTAMP_FROM_PLC
            {
                PLC_DateandTime = 1;
                ModbusRead(DEVICE, RMC_SYM_SECS,7);
            }
            #else
            PLC_DateandTime = 0;
            #endif
            PLC_DateandTime = 0;
            ModbusRead(DEVICE, RMC_SYM_RUN_HOUR,ACE_PLC_REGS_TO_READ);
            CLASS_Sys_settings_Read_flag = 1;
            ModbusRead(DEVICE, RMC_SBR_ISR_REG,SETT_PLC_REGS_TO_READ);
            
            if (RMC_SBR_SYS_SET.SBR_ISR.bits.b1 || RMC_SBR_SYS_SET.SBR_ISR.bits.b2)
            {
                RMC_SBR_SYS_SET.SBR_ISR.bits.b2 = 0;
                if (RMC_SBR_SYS_SET.SBR_ISR.bits.b1)
                {    Dummy_SMS_Set = 1;}
                DataBuffer[1] = 0x00;
                DataBuffer[0] = 0xFF;
                ModbusWrite(DEVICE,PLC_SBR_ISR_RESET,FORCE_SINGLE_COIL,DataBuffer,1);
                DataBuffer[1] = 0x00;
                DataBuffer[0] = 0x00;
                ModbusWrite(DEVICE,PLC_SBR_ISR_RESET,FORCE_SINGLE_COIL,DataBuffer,1);
            }
            if (!FLAGS.GetServerTime)
            {
                PrevDateTimeCount++;
                if (PrevDateTimeCount >= 10)
                {
                    PrevDateTimeCount = 0;
                    WriteDateTime(Prev_Date_Time , RMC_SYM_PREV_SECS);
                }
            }
            break;

        case MDB_SET_TIMERS:
            Write_Count=0;
            MDB_WRITE_DATA.SBR_SYM_MODE = 0x09;
            DataBuffer[1] = MDB_WRITE_DATA.SBR_SYM_MODE;
            DataBuffer[0] = 0x00;
            ModbusWrite(DEVICE,PLC_SYM_MODE_CHANGE,WRITE_SIN_REG,DataBuffer,1);
            
            if (MBWrite_Success)
            {
                DataBuffer[0] = MDB_WRITE_DATA.BATCH_CYCLE.byte.HB;
                DataBuffer[1] = MDB_WRITE_DATA.BATCH_CYCLE.byte.LB;
                DataBuffer[2] = MDB_WRITE_DATA.FILL_START.byte.HB;
                DataBuffer[3] = MDB_WRITE_DATA.FILL_START.byte.LB;
                DataBuffer[4] = MDB_WRITE_DATA.FILL_END.byte.HB;
                DataBuffer[5] = MDB_WRITE_DATA.FILL_END.byte.LB;
                DataBuffer[6] = MDB_WRITE_DATA.AERATION_START.byte.HB;
                DataBuffer[7] = MDB_WRITE_DATA.AERATION_START.byte.LB;
                DataBuffer[8] = MDB_WRITE_DATA.AERATION_END.byte.HB;
                DataBuffer[9] = MDB_WRITE_DATA.AERATION_END.byte.LB;
                DataBuffer[10] = MDB_WRITE_DATA.SETTLE_START.byte.HB;
                DataBuffer[11] = MDB_WRITE_DATA.SETTLE_START.byte.LB;
                DataBuffer[12] = MDB_WRITE_DATA.SETTLE_END.byte.HB;
                DataBuffer[13] = MDB_WRITE_DATA.SETTLE_END.byte.LB;
                DataBuffer[14] = MDB_WRITE_DATA.DECANT_START.byte.HB;
                DataBuffer[15] = MDB_WRITE_DATA.DECANT_START.byte.LB;
                DataBuffer[16] = MDB_WRITE_DATA.DECANT_END.byte.HB;
                DataBuffer[17] = MDB_WRITE_DATA.DECANT_END.byte.LB;
                DataBuffer[18] = MDB_WRITE_DATA.IDLE_START.byte.HB;
                DataBuffer[19] = MDB_WRITE_DATA.IDLE_START.byte.LB;
                DataBuffer[20] = MDB_WRITE_DATA.IDLE_END.byte.HB;
                DataBuffer[21] = MDB_WRITE_DATA.IDLE_END.byte.LB;
                ModbusWrite(DEVICE,RMC_SBR_BATCH_TIMER,WRITE_MUL_REG,DataBuffer,11);
                MBWrite_Success = 0;
            }
            else
            {
                /*SMS_TASKS.Parameters[0] = 0x07;
                SMS_TASKS.SMS_TASK_STATE = SET_TIMERS;
                InsertSMSTask(SMS_TASKS);
                mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
                InsertMODEMTask(mTask);*/
                break;
            }
                
            MDB_WRITE_DATA.SBR_SYM_MODE = 0x01;
            DataBuffer[1] = MDB_WRITE_DATA.SBR_SYM_MODE;
            DataBuffer[0] = 0x00;
            ModbusWrite(DEVICE,PLC_SYM_MODE_CHANGE,WRITE_SIN_REG,DataBuffer,1);                        
            break;

        case MDB_RESET_PLC:  
            MDB_WRITE_DATA.SBR_SYM_MODE = 0x01;
            DataBuffer[1] = MDB_WRITE_DATA.SBR_SYM_MODE;
            DataBuffer[0] = 0x00;
            ModbusWrite(DEVICE,PLC_SYM_MODE_CHANGE,WRITE_SIN_REG,DataBuffer,1);
            break;
            
        case MDB_SBR_MAINTENANCE:  
            MDB_WRITE_DATA.SBR_SYM_MODE = 0x09;
            DataBuffer[1] = MDB_WRITE_DATA.SBR_SYM_MODE;
            DataBuffer[0] = 0x00;
            ModbusWrite(DEVICE,PLC_SYM_MODE_CHANGE,WRITE_SIN_REG,DataBuffer,1);
            break;
            
        case MDB_SET_PLC_RTC:
            WriteCurrDateTime(DATE_TIME , RMC_SYM_CHANGE_YEARS);
            break;
            
        case MODBUD_SEND_DEV_ID_GPRS:
            mTask.MODEM_TASK_STATE = MODEM_SEND_DEV_ID_TASK;
            InsertMODEMTask(mTask);
            ConfigModbus();            
            break;
        
        case MODBUS_FRAME_BYTE_STREAM:
            SMS_TASKS.SMS_TASK_STATE = SERVER_TIME_TO_PLC;
            SMS_TASKS.Parameters[0] = 0xFF;
            WriteCurrDateTime(DATE_TIME , RMC_SYM_CHANGE_YEARS);
            FLAGS.GetServerTime = 0;
			CLASS_Sys_settings_Read_flag = 0;
            CopyServerTime();
            ConfigModbus(); 
            InsertSYSTask(CLT_SYS_UPLOAD_TASK);
            break;
            
        case MODBUS_DLY_WAIT:
            if(!MdbWaitctr)
            {
                MODBUS_STATE = nMODBUS_STATE;
                cMODBUS_STATE = nMODBUS_STATE;
            }
            break;

        case MODBUS_SET_DLY:
            MdbWaitctr = MODBUS_WAIT_DURATRATION;
            MODBUS_STATE = MODBUS_DLY_WAIT;
            cMODBUS_STATE = MODBUS_DLY_WAIT;
            break;

        case MODBUS_TRIGGER_SYM_SUMMERY_GPRS:
            mTask.MODEM_TASK_STATE = MODEM_SEND_CLT_SUMMERY_TASK;
            InsertMODEMTask(mTask);
            ConfigModbus();
            break;

        case MODBUS_TRIGGER_RMCU_ALARM_GPRS:
            mTask.MODEM_TASK_STATE = MODEM_SEND_ALARMS_TASK;
            InsertMODEMTask(mTask);
            ConfigModbus();
            
            #if 0
            mTask.MODEM_TASK_STATE = MODEM_SEND_ALARMS_TASK;
            InsertMODEMTask(mTask);
            MODEM_TASK.MODEM_TASK_STATE = MODEM_NO_TASK;
            memset(MODEM_TASK.Parameters, 0, sizeof(MODEM_TASK.Parameters));
            MODBUS_STATE = MODBUS_WAITTO_FINISH_GPRS;
            #endif
            break;
            
        case MODBUS_WAITTO_FINISH_GPRS:
            
            if(MODEM_STATE == MODEM_STANDBY)
            {
                MODEM_STATE = MODEM_TURNOFF;
                ConfigModbus();
            }
            break;

        case MODBUS_SEND_PLC_SETTINGS_GPRS:

            SMS_TASKS.SMS_TASK_STATE = GET_PLC_SETTINGS;
            InsertSMSTask(SMS_TASKS);
            mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
            InsertMODEMTask(mTask);
            ConfigModbus();
            break;

        case MODBUS_STANDBY:
            break;

        default:
            break;
    }
}


void ModbusRead(UINT16 DeviveID, UINT16 RegAddr,UINT8 NoofRegstoRead)
{
    PLC_RX_LED = 1;

    memset(mdbReqAdu.TxBuffer,0,sizeof(mdbReqAdu.TxBuffer));
    memset(mdbRspAdu.RxBuffer,0,sizeof(mdbRspAdu.RxBuffer));

    MBResponse = ModbusOP(DeviveID, READ_COMMAND, RegAddr, NoofRegstoRead, (unsigned char *)TempRxBuffer, 1);

    if(MBResponse == 0)
    {
        if (RegAddr == RMC_SYM_PREV_SECS)
        {
            GetMdbPrevTime();
        }
        else
        {
            GetMdbResponse();
            NextModbusState();
            INTProcessPending = 0;
        }
    }
    else
    {
        if((++ModbusErrCnt) >= MODBUS_RETRY_COUNT)
        {
            ModbusErrCnt = 0;
            if(FLAGS.GetPLCSettingsSMScmd)
            {
                SMS_TASKS.Parameters[0] = 0x06;
                FLAGS.GetPLCSettingsSMScmd = 0;
                MODBUS_STATE = MODBUS_SEND_PLC_SETTINGS_GPRS;
            }
            else
            {
                Construct_SBR_Alarms_Packet();
                MODBUS_STATE = MODBUS_TRIGGER_RMCU_ALARM_GPRS;
            }            
        }
    }
    PLC_RX_LED = 0;
}
  

void ModbusWrite(UINT16 DeviveID, UINT16 RegAddr,UINT8 FunctionCode,UINT8 *pData,UINT8 NoofRegstoWrite)
{
    int i;
    PLC_TX_LED = 1;
    MBWrite_Success = 0;

    memset(mdbReqAdu.TxBuffer,0,sizeof(mdbReqAdu.TxBuffer));
    memset(mdbRspAdu.RxBuffer,0,sizeof(mdbRspAdu.RxBuffer));
    
    //copy data to be written
    /*if(NoofRegstoWrite == 1)
    {
        mdbWriteBuffer[0] = *pData++;
        mdbWriteBuffer[1] = *pData++;
    }

    else
    {
        mdbWriteBuffer[0] = *pData++;
        mdbWriteBuffer[1] = *pData++;
        mdbWriteBuffer[2] = *pData++;
        mdbWriteBuffer[3] = *pData++;
    }*/
    
    for (i = 0; i < (2*NoofRegstoWrite); i++)
    {
        mdbWriteBuffer[i] = *pData++;
    }

    MBResponse = ModbusOP(DeviveID, FunctionCode, RegAddr, NoofRegstoWrite, (unsigned char *)mdbWriteBuffer, 1);

    if(MBResponse == 0)
    {
        Write_Count++;
        
        if((MODBUS_STATE == MDB_RESET_PLC) ||(MODBUS_STATE == MDB_SBR_MAINTENANCE) ||
          ((Write_Count >= 3) && (MODBUS_STATE != MODBUS_TRIGGER_SYM_SUMMERY_GPRS) )) 
        {
          InsertSMSTask(SMS_TASKS);
          mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
          InsertMODEMTask(mTask);
          Write_Count = 0;
          NextWriteModbusState(); 
        }
        
        MBWrite_Success = 1;
    }
    else
    {
        if((++ModbusErrCnt) == MODBUS_RETRY_COUNT)
        {
            //PLC communication failure
            SMS_TASKS.Parameters[0] = 0x06;
            InsertSMSTask(SMS_TASKS);

             //SMS_TASKS.Parameters[0] = 0x07;//TBI in PIC24

            mTask.MODEM_TASK_STATE = MODEM_SEND_SMS_TASK;
            InsertMODEMTask(mTask);
            
            ModbusErrCnt = 0;
            ConfigModbus();
        }
    }

    PLC_TX_LED = 0;
}


void GetMdbResponse(void)
{
    UINT16 TotalRespbytes = 0,StAddrCount = 0;

    
    TotalRespbytes = MdbRespBuffer.RxBuffer[3];
    
    if(PLC_DateandTime)
    {
        MODBUS_RESP_STATE = MDB_SECS;
    }
    else if (CLASS_Sys_settings_Read_flag)
    {
        MODBUS_RESP_STATE = MDB_ISR_STATUS;
    }
    else
    {
        MODBUS_RESP_STATE = MDB_SYSTEM_RUNNING_HOURS;
    }

    for(mRespBytCount = 0; mRespBytCount < TotalRespbytes;)
    {
        switch(MODBUS_RESP_STATE)
        {
            case MDB_DATE:
                TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
                DATE_TIME.Date = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_MONTH:
                TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
                DATE_TIME.Month = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            case MDB_WEEK_DAY:
                TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
                DATE_TIME.Week_Day = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            case MDB_YEAR:
                TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
                DATE_TIME.Year.Val = TmpResult.word.LW;
                
                if (DATE_TIME.Year.Val < 2000)
                {
                    DATE_TIME.Year.Val += CURRENT_YEAR;
                }
                PLC_DateandTime = 0;
                mRespBytCount = TotalRespbytes;
                MODBUS_RESP_STATE = MDB_DUMMY_STATE;
                break;

            case MDB_HOUR:
                TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
                DATE_TIME.Hour = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_MINS:
                TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
                DATE_TIME.Mins = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_SECS:
                TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
                DATE_TIME.Seconds = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_ISR_STATUS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.SBR_ISR.Val = TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
                
            case MDB_SBR_BATCH_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.SBR_BATCH_TMR_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            
            case MDB_FILL_START_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.FILL_SBR_START_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            
            case MDB_FILL_STOP_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.FILL_SBR_STOP_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            
            case MDB_AERATE_START_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.AERATE_SBR_START_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            
            case MDB_AERATE_STOP_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.AERATE_SBR_STOP_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;    
            
            case MDB_SETTLE_START_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.PRE_SETTLE_SBR_START_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            
            case MDB_SETTLE_STOP_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.PRE_SETTLE_SBR_STOP_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;       
            
            case MDB_DECANT_START_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.DECANT_SBR_START_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            
            case MDB_DECANT_STOP_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.DECANT_SBR_STOP_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;    
            
            case MDB_IDLE_START_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.IDLE_SBR_START_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            
            case MDB_IDLE_STOP_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.IDLE_SBR_STOP_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
           
            case MDB_INT_AER_FRQ_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.INTER_AERATE_FRQ_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;    
            
            case MDB_INT_AER_RUN_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.INTER_AERATE_RUN_MIN.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
                
            case MDB_P1_CUR_MAX_THRLD:
                RMC_SBR_SYS_SET.P1_CUR_MAX_THRLD = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                ProcessFloatResult(&RMC_SBR_SYS_SET.P1_CUR_MAX_THRLD);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_P1_CUR_MIN_THRLD:
                RMC_SBR_SYS_SET.P1_CUR_MIN_THRLD = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                ProcessFloatResult(&RMC_SBR_SYS_SET.P1_CUR_MIN_THRLD);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_P2_CUR_MAX_THRLD:
                RMC_SBR_SYS_SET.P2_CUR_MAX_THRLD = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                ProcessFloatResult(&RMC_SBR_SYS_SET.P2_CUR_MAX_THRLD);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_P2_CUR_MIN_THRLD:
                RMC_SBR_SYS_SET.P2_CUR_MIN_THRLD = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                ProcessFloatResult(&RMC_SBR_SYS_SET.P2_CUR_MIN_THRLD);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_P3_CUR_MAX_THRLD:
                RMC_SBR_SYS_SET.P3_CUR_MAX_THRLD = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                ProcessFloatResult(&RMC_SBR_SYS_SET.P3_CUR_MAX_THRLD);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_P3_CUR_MIN_THRLD:
                RMC_SBR_SYS_SET.P3_CUR_MIN_THRLD = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                ProcessFloatResult(&RMC_SBR_SYS_SET.P3_CUR_MIN_THRLD);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
                        
            case MDB_SBR_HIGH_LEVEL_THRLD:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.SBR_TANK_HIGH_THRLD.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_SBR_LOW_LEVEL_THRLD:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.SBR_TANK_LOW_THRLD.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
                
            case MDB_SBR_HIGH_WINDOW_THRLD:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.SBR_TANK_HIGH_WINDOW_THRLD.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            
            case MDB_SBR_LOW_WINDOW_THRLD:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.SBR_TANK_LOW_WINDOW_THRLD.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
                
            case MDB_SBR_SENSOR_CHECK_THRLD:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.SBR_TANK_SENSOR_CHECK_THRLD.Val  = TmpResult.word.LW;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
                
            case MDB_SBR_OVERFILL_TIMER_THRLD:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_SET.SBR_TANK_OVERFILL_TIMER_THRLD.Val  = TmpResult.word.LW;
                mRespBytCount = TotalRespbytes;
                MODBUS_STATE = MDB_STOP_PARSE;
                break;
                
            case MDB_SYSTEM_RUNNING_HOURS:
                RMC_SBR_SYS_PAR.SYSTEM_RUNNING_HOURS = GetResponseBytes(StAddrCount,PLC_REG_TWO_REG_READ);
                StAddrCount += PLC_REG_TWO_REG_READ;
                break;

            case MDB_BATCH_RUNNING_COUNT:
                RMC_SBR_SYS_PAR.BATCH_COUNT = GetResponseBytes(StAddrCount, PLC_REG_TWO_REG_READ);
                StAddrCount += PLC_REG_TWO_REG_READ;
                break;
            
            case MDB_BATCH_CYCLE_COUNT:
                RMC_SBR_SYS_PAR.CYCLE_COUNT = GetResponseBytes(StAddrCount, PLC_REG_TWO_REG_READ);
                StAddrCount += PLC_REG_TWO_REG_READ;
                break;    
                
            case MDB_FLOW_RATE:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.FLOW_RATE = TmpResult.wordUnion.low;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_FLOW_TOTALIZER:
                RMC_SBR_SYS_PAR.FLOW_TOTALIZER = GetResponseBytes(StAddrCount, PLC_REG_TWO_REG_READ);
                StAddrCount += PLC_REG_TWO_REG_READ;
                break;
            
            case MDB_WATER_LEVEL:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.SBR_WATER_LEVEL = TmpResult.wordUnion.low;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;    
            
            case MDB_SBR_SYM_MODE:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.SBR_SYM_MODE = TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_SBR_SENSOR_STAT:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.SBR_SENSOR_STAT = TmpResult.wordUnion.low;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;   

            case MDB_LAST_POWEROFF_DATE:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Date  = TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_LAST_POWEROFF_MON:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Month  = TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_LAST_POWEROFF_YEAR:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Year.Val  = TmpResult.word.LW;
                if (RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Year.Val < 2000)
                {
                    RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Year.Val += CURRENT_YEAR;
                }
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_LAST_POWEROFF_HOUR:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Hour  = (int)TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_LAST_POWEROFF_MINS:
               TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Mins  = (int)TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_LAST_POWEROFF_SECS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Seconds  = TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_LAST_POWERON_DATE:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWERON_DT.Date  = TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_LAST_POWERON_MON:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWERON_DT.Month  = TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_LAST_POWERON_YEAR:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWERON_DT.Year.Val  = TmpResult.word.LW;
                if (RMC_SBR_SYS_PAR.LAST_POWERON_DT.Year.Val < 2000)
                {
                    RMC_SBR_SYS_PAR.LAST_POWERON_DT.Year.Val += CURRENT_YEAR;
                }
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_LAST_POWERON_HOUR:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWERON_DT.Hour  = TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_LAST_POWERON_MINS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWERON_DT.Mins  = TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
            
            case MDB_LAST_POWERON_SECS:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_SBR_SYS_PAR.LAST_POWERON_DT.Seconds  = TmpResult.byte.LB;
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;
                
            case MDB_TOTAL_SYS_ENERGY:
                RMC_ENG_MTR_PAR.TOTAL_SYS_ENERGY = GetResponseBytes1(StAddrCount, PLC_REG_TWO_REG_READ);
                ProcessFloatResultforEnergy(&RMC_ENG_MTR_PAR.TOTAL_SYS_ENERGY);
                StAddrCount += PLC_REG_TWO_REG_READ;
                break;

            case MDB_TOTAL_AVG_CUR:
                RMC_ENG_MTR_PAR.TOTAL_AVG_CUR = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                ProcessFloatResult(&RMC_ENG_MTR_PAR.TOTAL_AVG_CUR);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_TOTAL_AVG_PF:
                RMC_ENG_MTR_PAR.TOTAL_AVG_PF = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                ProcessFloatResult(&RMC_ENG_MTR_PAR.TOTAL_AVG_PF);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_P1_CURRENT:
                TmpResult = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_ENG_MTR_PAR.P1_CURRENT = TmpResult; 
                ProcessFloatResult(&RMC_ENG_MTR_PAR.P1_CURRENT);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_P2_CURRENT:
                RMC_ENG_MTR_PAR.P2_CURRENT = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                ProcessFloatResult(&RMC_ENG_MTR_PAR.P2_CURRENT);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_P3_CURRENT:
                RMC_ENG_MTR_PAR.P3_CURRENT = GetResponseBytes1(StAddrCount, PLC_REG_DEFAULT_SIZE);
                ProcessFloatResult(&RMC_ENG_MTR_PAR.P3_CURRENT);
                StAddrCount += PLC_REG_DEFAULT_SIZE;
                break;

            case MDB_FLT_CODE:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_TWO_REG_READ);
                RMC_ALARM.SBR_FAULT_CODE = TmpResult;
                StAddrCount += PLC_REG_TWO_REG_READ;
                break;

            case MDB_FLTCODE_VAL:
                TmpResult = GetResponseBytes(StAddrCount, PLC_REG_DEFAULT_SIZE);
                RMC_ALARM.FAULT_CODE_VAL = TmpResult.byte.LB;
                mRespBytCount = TotalRespbytes;
                MODBUS_STATE = MDB_DUMMY_STATE;
                break;               

                /*Not used just to avoid warnings*/
            case MODBUS_ENTRY_STATE:
                break;

            case MODBUS_RMC_SBR_SYS_PAR:
                break;

            case MODBUS_EXIT_STATE:
                break;

            /*case MDB_SET_TIMER1:
                break;

            case MDB_SET_TIMER2:
                break;

            case MDB_SET_TIMER3:
                break;
            
            case MDB_SET_TIMER4:
                break;*/

            case MDB_SET_TIMERS:
                break;
                
            case MODBUS_TRIGGER_SYM_SUMMERY_GPRS:
                break;

            case MODBUS_TRIGGER_SYM_STATUS_GPRS:
                break;

            case MODBUS_TRIGGER_EM_GPRS:
                break;

            case MODBUS_TRIGGER_RMCU_ALARM_GPRS:
                break;

            case MODBUS_WAITTO_FINISH_GPRS:
                break;

            case MODBUS_SEND_PLC_SETTINGS_GPRS:
                break;
                
            case MODBUS_SET_DLY:
                break;

            case MODBUS_DLY_WAIT:
                break;

            case MODBUS_STANDBY:
                break;
           /* case MDB_P1_CUR_MIN_THRLD_W:
                break;
            case MDB_P1_CUR_MAX_THRLD_W:
                break;
            case MDB_P2_CUR_MIN_THRLD_W:
                break;
            case MDB_P2_CUR_MAX_THRLD_W:
                break;
            case MDB_P3_CUR_MIN_THRLD_W:
                break;
            case MDB_P3_CUR_MAX_THRLD_W:
                break;*/
            case MDB_RESET_PLC:
                break;
            case MDB_SBR_MAINTENANCE:
                break;
            default:
                break;
        }

        MODBUS_RESP_STATE++;

    }
}


UINT32_VAL GetResponseBytes(UINT16 StAddress,UINT16 Length)
{
    BYTE *RespuffAddrPtr;
    UINT32_VAL Result;

    RespuffAddrPtr = &MdbRespBuffer.RxBuffer[4];
    RespuffAddrPtr += StAddress;

     if(Length >2)
    {
        Result.byte.HB = *RespuffAddrPtr++;
        Result.byte.LB = *RespuffAddrPtr++;
        Result.byte.MB = *RespuffAddrPtr++;
        Result.byte.UB = *RespuffAddrPtr++;
        mRespBytCount += 4;
    }
    else
    {
        Result.byte.MB = 0x00;
        Result.byte.UB = 0x00;
        Result.byte.HB = *RespuffAddrPtr++;
        Result.byte.LB = *RespuffAddrPtr++;
        mRespBytCount += 2;
    }
    return Result;
}


UINT32_VAL GetResponseBytes1(UINT16 StAddress,UINT16 Length)
{
    BYTE *RespuffAddrPtr;
    UINT32_VAL Result;

    RespuffAddrPtr = &MdbRespBuffer.RxBuffer[4];
    RespuffAddrPtr += StAddress;

     if(Length >2)
    {
        Result.byte.HB = *RespuffAddrPtr++;
        Result.byte.LB = *RespuffAddrPtr++;
        Result.byte.MB = *RespuffAddrPtr++;
        Result.byte.UB = *RespuffAddrPtr++;
        mRespBytCount += 4;
    }
    else
    {
        Result.byte.MB = 0x00;
        Result.byte.UB = 0x00;
        Result.byte.HB = *RespuffAddrPtr++;
        Result.byte.LB = *RespuffAddrPtr++;
        mRespBytCount += 2;
    }
    return Result;
}


void NextModbusState(void)
{   
    if (MODBUS_STATE != MDB_DUMMY_STATE)
        nMODBUS_STATE = MODBUS_STATE + 1;

    if(nMODBUS_STATE == MODBUS_EXIT_STATE)
    {
        if (ValidDateTime())
        {
            if (CLASS_Sys_settings_Read_flag)
            {
            //Modbus Done..
                Construct_SBR_Summery_ParamsPacket();
                Construct_SBR_Status_ParamsPacket();
                Construct_EMP_ParamsPacket();
                Construct_SBR_Settings_ParamsPacket();
        
                CLASS_Sys_settings_Read_flag = 0;
        
                if(FLAGS.GetPLCSettingsSMScmd)
                {
                    SMS_TASKS.Parameters[0] = 0xFF;
                    FLAGS.GetPLCSettingsSMScmd = 0;
                    MODBUS_STATE = MODBUS_SEND_PLC_SETTINGS_GPRS;
                }
                else
                {
                    MODBUS_STATE = MODBUS_TRIGGER_SYM_SUMMERY_GPRS;
                }
            }
        }
        else if(InvalidDateTimeCount <= 3)
        {
            FLAGS.GetServerTime = 1;
            FLAGS.DefaultMobileNo = 1;
            InvalidDateTimeCount++;
            Construct_Device_ID_Packet();
            MODBUS_STATE = MODBUD_SEND_DEV_ID_GPRS;
        }
        else
        {
            ConfigModbus();
        }
    }
    else
        MODBUS_STATE = MODBUS_SET_DLY;
    
        ModbusErrCnt = 0;
}

void NextWriteModbusState(void)
{
    /*nMODBUS_STATE = MODBUS_STATE + 1;

    if(nMODBUS_STATE == MDB_P1_CUR_MAX_THRLD_W || nMODBUS_STATE == MDB_P2_CUR_MAX_THRLD_W ||
       nMODBUS_STATE == MDB_P3_CUR_MAX_THRLD_W )
    {
        MODBUS_STATE = nMODBUS_STATE;
        cMODBUS_STATE = nMODBUS_STATE;
    }

    else
    {*/
        ConfigModbus();
    //}

    ModbusErrCnt = 0;
}



void ConfigModbus(void)
{
    MODBUS_STATE = MODBUS_STANDBY;
    nMODBUS_STATE = MODBUS_STANDBY;
    cMODBUS_STATE = MODBUS_STANDBY;
}



void Construct_SBR_Summery_ParamsPacket(void)
{
//    float vfd_percent=0.0;
//    UINT16_VAL vfd_percent_int;
    UINT32_VAL temp;
    
    temp.Val = 0;
    
    /*#ifdef GET_TIMESTAMP_FROM_PLC
    char Date;
    char Month;
    char Year;
    char Hour;
    char Min;
    char Sec;
    #endif*/

    char cData[128];
    UINT16_VAL cRes;
    UINT16 Datalen = 0,DataIndex = 0,i;

    memset((void *)cData, 0,sizeof(cData));
    memset(&ACE_SBR_SUM_PCKT, 0,sizeof(ACE_SBR_SUM_PCKT));

    for(i=0; i<sizeof(DEV_TYPE); i++)
    {
        DEV_TYPE[i] = 0x00;
    }
    
    for(i=0; i<sizeof(RMCU_ID); i++)
    {
        RMCU_ID[i] = 0x00;
    }

    strcpy((char*)RMCU_ID,(const char*)SYS_PARS.RMCU_Id);
    strcpy((char*)DEV_TYPE,(const char*)SYS_PARS.Dev_Type);

    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)AES_SRT_BYTE);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)AES_MSG_CODE);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)AES_STATUS);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)AES_RTRY_CNTR);

    /*#ifdef GET_TIMESTAMP_FROM_PLC
    //Disable oscillator
    WriteOnI2C(ADDR_SEC,0x00);
    DelayMs(2);

    Sec   = DATE_TIME.Seconds;
    Min   = DATE_TIME.Mins;
    Hour  = DATE_TIME.Hour;
    Date  = DATE_TIME.Date;
    Month = DATE_TIME.Month;
    Year  = DATE_TIME.Year.byte.LB;

    SetRtcTime(Sec, Min, Hour);
    SetRtcDate(Date, Month, Year);

    //Enable oscillator
    WriteOnI2C(ADDR_SEC,0x80);
    DelayMs(2);
    #else
    #ifdef RTC
    ReadRTC(&DATE_TIME);
    #endif
    #endif*/
    
    Datalen = strlen((const char*)ACE_SBR_SUM_PCKT.Data);
    DataIndex = Datalen;

    cRes = Hex2Ascii_Frame(DATE_TIME.Date);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(DATE_TIME.Month);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Year.byte.HB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    cRes = Hex2Ascii_Frame(DATE_TIME.Year.byte.LB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Hour);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Mins);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Seconds);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)AES_RESP_FOLLOW);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)AES_SUMMERY_PARM_NUM);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)"2D");              /*SBR_BYTES_AFTER_SIZE+RMC_SBR_SUM_DATA_LENGTH*/
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)RMCU_ID);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)DEV_TYPE);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)AES_CONST1);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)AES_SUMMERY_CONST2);

    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)PARAM_CODE_1);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)PARAM_CODE_2);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)PARAM_CODE_3);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)PARAM_CODE_4);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)PARAM_CODE_5);
    strcat((char*)ACE_SBR_SUM_PCKT.Data,(const char*)PARAM_CODE_6);
    
    Datalen = strlen((const char*)ACE_SBR_SUM_PCKT.Data);
    DataIndex = Datalen;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.SYSTEM_RUNNING_HOURS.byte.MB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.SYSTEM_RUNNING_HOURS.byte.UB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.SYSTEM_RUNNING_HOURS.byte.HB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.SYSTEM_RUNNING_HOURS.byte.LB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.BATCH_COUNT.byte.MB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.BATCH_COUNT.byte.UB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.BATCH_COUNT.byte.HB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.BATCH_COUNT.byte.LB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.FLOW_RATE.byte.HB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.FLOW_RATE.byte.LB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.FLOW_TOTALIZER.byte.MB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.FLOW_TOTALIZER.byte.UB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.FLOW_TOTALIZER.byte.HB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.FLOW_TOTALIZER.byte.LB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.CYCLE_COUNT.byte.MB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.CYCLE_COUNT.byte.UB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.CYCLE_COUNT.byte.HB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.CYCLE_COUNT.byte.LB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.SBR_WATER_LEVEL.byte.HB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_PAR.SBR_WATER_LEVEL.byte.LB);
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SUM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    
    Datalen = strlen((const char *)ACE_SBR_SUM_PCKT.Data);
    if(Datalen <= 50)
    {
        GetDeviceType();
        GetRMCUId();
        Construct_SBR_Summery_ParamsPacket();
    }

}


void Construct_SBR_Status_ParamsPacket(void)
{
    BYTE cData[128];
    UINT16_VAL cRes;
    UINT16 Datalen = 0,DataIndex = 0,len = 0,i=0;
    char system_mode_char[25]={'\0'};
    //char Dummy_char[12]={'\0'};

    memset((void *)cData, 0,sizeof(cData));
    memset(&ACE_SBR_STAT_PCKT, 0,sizeof(ACE_SBR_STAT_PCKT));

    for(i=0; i<sizeof(DEV_TYPE); i++)
    {
        DEV_TYPE[i] = 0x00;
    }

    for(i=0; i<sizeof(RMCU_ID); i++)
    {
        RMCU_ID[i] = 0x00;
    }

    strcpy((char*)RMCU_ID,(const char*)SYS_PARS.RMCU_Id);
    strcpy((char*)DEV_TYPE,(const char*)SYS_PARS.Dev_Type);

    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)AES_SRT_BYTE);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)AES_MSG_CODE);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)AES_STATUS);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)AES_RTRY_CNTR);

    #ifndef GET_TIMESTAMP_FROM_PLC
    #ifdef RTC
    ReadRTC(&DATE_TIME);
    #endif
    #endif

    Datalen = strlen((const char*)ACE_SBR_STAT_PCKT.Data);
    DataIndex = Datalen;

    cRes = Hex2Ascii_Frame(DATE_TIME.Date);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Month);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Year.byte.HB);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    cRes = Hex2Ascii_Frame(DATE_TIME.Year.byte.LB);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Hour);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Mins);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Seconds);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)AES_RESP_FOLLOW);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)AES_STATUS_PARM_NUM);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,"71"); //(83 = SBR_BYTES_AFTER_SIZE+29+19+19+3)
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)RMCU_ID);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)DEV_TYPE);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,"02");
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)AES_STATUS_CONST2);

    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_1);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_2);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_3);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_4);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_5);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_6);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_7);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_8);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_9);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_10);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_11);
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*)PARAM_CODE_ALM);
    
    Datalen = strlen((const char*)ACE_SBR_STAT_PCKT.Data);
    DataIndex = Datalen;


    memset (system_mode_char,'\0',sizeof(system_mode_char));
    
    len = (UINT16)strlen((const char *)SBR_SYS_Mode[RMC_SBR_SYS_PAR.SBR_SYM_MODE-1]);
    for(i = 0; i<len; i++)
    {
        sprintf(system_mode_char+i*2, "%02X", SBR_SYS_Mode[RMC_SBR_SYS_PAR.SBR_SYM_MODE-1][i]);
    }
        
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*) system_mode_char);
    DataIndex += strlen((const char*)system_mode_char);
    
    memset (system_mode_char,'\0',sizeof(system_mode_char));
    
    //SBR tank higher level sensor Status
    len = (UINT16)strlen((const char *)SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b1]);
    for(i = 0; i<len; i++)
    {
        sprintf(system_mode_char+i*2, "%02X", SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b1][i]);
    }
        
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*) system_mode_char);
    DataIndex += strlen((const char*)system_mode_char);
    memset (system_mode_char,'\0',sizeof(system_mode_char));
    
    //SBR tank lower level Status
    len = (UINT16)strlen((const char *)SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b0]);
    for(i = 0; i<len; i++)
    {
        sprintf(system_mode_char+i*2, "%02X", SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b0][i]);
    }
        
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*) system_mode_char);
    DataIndex += strlen((const char*)system_mode_char);

    memset (system_mode_char,'\0',sizeof(system_mode_char));

    
    //Decant tank higher level sensor status
    len = (UINT16)strlen((const char *)SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b2]);
    for(i = 0; i<len; i++)
    {
        sprintf(system_mode_char+i*2, "%02X", SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b2][i]);
    }
    
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*) system_mode_char);
    DataIndex += strlen((const char*)system_mode_char);
    memset (system_mode_char,'\0',sizeof(system_mode_char));
    
    //SBR pump 1 Status
    len = (UINT16)strlen((const char *)SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b3]);
    for(i = 0; i<len; i++)
    {
        sprintf(system_mode_char+i*2, "%02X", SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b3][i]);
    }
    
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*) system_mode_char);
    DataIndex += strlen((const char*)system_mode_char);
    memset (system_mode_char,'\0',sizeof(system_mode_char));

    //SBR pump 2 Status
    len = (UINT16)strlen((const char *)SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b4]);
    for(i = 0; i<len; i++)
    {
        sprintf(system_mode_char+i*2, "%02X", SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b4][i]);
    }
    
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*) system_mode_char);
    DataIndex += strlen((const char*)system_mode_char);
    memset (system_mode_char,'\0',sizeof(system_mode_char));
    
    //SBR pump 3 Status
    len = (UINT16)strlen((const char *)SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b5]);
    for(i = 0; i<len; i++)
    {
        sprintf(system_mode_char+i*2, "%02X", SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b5][i]);
    }
    
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*) system_mode_char);
    DataIndex += strlen((const char*)system_mode_char);
    memset (system_mode_char,'\0',sizeof(system_mode_char));
        
    //Power outage Status
    len = (UINT16)strlen((const char *)SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b6]);
    for(i = 0; i<len; i++)
    {
        sprintf(system_mode_char+i*2, "%02X", SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b6][i]);
    }
    
    strcat((char*)ACE_SBR_STAT_PCKT.Data,(const char*) system_mode_char);
    DataIndex += strlen((const char*)system_mode_char);
    memset (system_mode_char,'\0',sizeof(system_mode_char));
    
    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Date,10);
    len = strlen((const char*)cData);
    if(len == 1)
    {
        cRes = Hex2Ascii_Frame('0');
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    else
    {
        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[1]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '2';
    ACE_SBR_STAT_PCKT.Data[DataIndex++] = 'F';

    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Month,10);
    len = strlen((const char*)cData);
    if(len == 1)
    {
        cRes = Hex2Ascii_Frame('0');
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    else
    {
        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[1]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '2';
    ACE_SBR_STAT_PCKT.Data[DataIndex++] = 'F';

    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Year.Val,10);
    cRes = Hex2Ascii_Frame(cData[0]);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(cData[1]);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(cData[2]);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(cData[3]);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '2';
    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '0';

    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Hour,10);
    len = strlen((const char*)cData);
    if(len == 1)
    {
        cRes = Hex2Ascii_Frame('0');
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    else
    {
        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[1]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '3';
    ACE_SBR_STAT_PCKT.Data[DataIndex++] = 'A';

    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Mins,10);

    len = strlen((const char*)cData);
    if(len == 1)
    {
        cRes = Hex2Ascii_Frame('0');
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    else
    {
        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[1]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '3';
    ACE_SBR_STAT_PCKT.Data[DataIndex++] = 'A';

    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWEROFF_DT.Seconds,10);
    len = strlen((const char*)cData);
    if(len == 1)
    {
        cRes = Hex2Ascii_Frame('0');
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    else
    {
        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[1]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }


    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWERON_DT.Date,10);
    len = strlen((const char*)cData);
    if(len == 1)
    {
        cRes = Hex2Ascii_Frame('0');
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    else
    {
        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[1]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '2';
    ACE_SBR_STAT_PCKT.Data[DataIndex++] = 'F';

    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWERON_DT.Month,10);
    len = strlen((const char*)cData);
    if(len == 1)
    {
        cRes = Hex2Ascii_Frame('0');
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    else
    {
        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[1]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '2';
    ACE_SBR_STAT_PCKT.Data[DataIndex++] = 'F';

    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWERON_DT.Year.Val,10);
    cRes = Hex2Ascii_Frame(cData[0]);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(cData[1]);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(cData[2]);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(cData[3]);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '2';
    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '0';

    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWERON_DT.Hour,10);
    len = strlen((const char*)cData);
    if(len == 1)
    {
        cRes = Hex2Ascii_Frame('0');
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    else
    {
        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[1]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '3';
    ACE_SBR_STAT_PCKT.Data[DataIndex++] = 'A';

    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWERON_DT.Mins,10);

    len = strlen((const char*)cData);
    if(len == 1)
    {
        cRes = Hex2Ascii_Frame('0');
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    else
    {
        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[1]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    ACE_SBR_STAT_PCKT.Data[DataIndex++] = '3';
    ACE_SBR_STAT_PCKT.Data[DataIndex++] = 'A';

    itoa((char*)cData,RMC_SBR_SYS_PAR.LAST_POWERON_DT.Seconds,10);
    len = strlen((const char*)cData);
    if(len == 1)
    {
        cRes = Hex2Ascii_Frame('0');
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }

    else
    {
        cRes = Hex2Ascii_Frame(cData[0]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

        cRes = Hex2Ascii_Frame(cData[1]);
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
        ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    }
    
    cRes = Hex2Ascii_Frame(RMC_ALARM.FAULT_CODE_VAL);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ALARM.SBR_FAULT_CODE.byte.MB);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_ALARM.SBR_FAULT_CODE.byte.UB);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ALARM.SBR_FAULT_CODE.byte.HB);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ALARM.SBR_FAULT_CODE.byte.LB);
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_STAT_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
     
    Datalen = strlen((const char *)ACE_SBR_STAT_PCKT.Data);
    
    if(Datalen <= 150)
    {
        GetDeviceType();
        GetRMCUId();
        Construct_SBR_Status_ParamsPacket();
    }

}


void Construct_EMP_ParamsPacket(void)
{
    UINT16_VAL cRes;
    UINT16 Datalen = 0,DataIndex = 0,i;
    
    memset(&ACE_SBR_EM_PCKT, 0,sizeof(ACE_SBR_EM_PCKT));


    for(i=0; i<sizeof(DEV_TYPE); i++)
    {
        DEV_TYPE[i] = 0x00;
    }

    for(i=0; i<sizeof(RMCU_ID); i++)
    {
        RMCU_ID[i] = 0x00;
    }

    strcpy((char*)RMCU_ID,(const char*)SYS_PARS.RMCU_Id);
    strcpy((char*)DEV_TYPE,(const char*)SYS_PARS.Dev_Type);

    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)AES_SRT_BYTE);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)AES_MSG_CODE);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)AES_STATUS);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)AES_RTRY_CNTR);

    #ifndef GET_TIMESTAMP_FROM_PLC
    #ifdef RTC
    ReadRTC(&DATE_TIME);
    #endif
    #endif

    Datalen = strlen((const char*)ACE_SBR_EM_PCKT.Data);
    DataIndex = Datalen;

    cRes = Hex2Ascii_Frame(DATE_TIME.Date);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Month);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Year.byte.HB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    cRes = Hex2Ascii_Frame(DATE_TIME.Year.byte.LB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Hour);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Mins);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Seconds);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)AES_RESP_FOLLOW);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)AES_EM_PARM_NUM);
    strcat((char*)ACE_SBR_EM_PCKT.Data,"31"); /*SBR_BYTES_AFTER_SIZE+42*/
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)RMCU_ID);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)DEV_TYPE);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)AES_CONST3);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)AES_EM_CONST2);

    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)PARAM_CODE_1);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)PARAM_CODE_2);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)PARAM_CODE_3);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)PARAM_CODE_4);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)PARAM_CODE_5);
    strcat((char*)ACE_SBR_EM_PCKT.Data,(const char*)PARAM_CODE_6);
   
    Datalen = strlen((const char*)ACE_SBR_EM_PCKT.Data);

    DataIndex = Datalen;
    
    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_SYS_ENERGY.byte.MB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_SYS_ENERGY.byte.UB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_SYS_ENERGY.byte.HB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_SYS_ENERGY.byte.LB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_AVG_CUR.byte.MB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_AVG_CUR.byte.UB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_AVG_CUR.byte.HB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_AVG_CUR.byte.LB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_AVG_PF.byte.MB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_AVG_PF.byte.UB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_AVG_PF.byte.HB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.TOTAL_AVG_PF.byte.LB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P1_CURRENT.byte.MB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P1_CURRENT.byte.UB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P1_CURRENT.byte.HB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P1_CURRENT.byte.LB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P2_CURRENT.byte.MB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P2_CURRENT.byte.UB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P2_CURRENT.byte.HB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P2_CURRENT.byte.LB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P3_CURRENT.byte.MB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P3_CURRENT.byte.UB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P3_CURRENT.byte.HB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_ENG_MTR_PAR.P3_CURRENT.byte.LB);
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_EM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    Datalen = strlen((const char *)ACE_SBR_EM_PCKT.Data);
    if(Datalen <= 100)
    {
        GetDeviceType();
        GetRMCUId();
        Construct_EMP_ParamsPacket();
    }

}


void Construct_SBR_Settings_ParamsPacket(void)
{
//    float vfd_percent=0.0;
//    UINT16_VAL vfd_percent_int;
    BYTE cData[128];
    UINT16_VAL cRes;
    UINT32_VAL temp;
    UINT16 Datalen = 0,DataIndex = 0,len = 0,i=0;
    char system_mode_char[25]={'\0'};
    
    temp.Val = 0;

    memset((void *)cData, 0,sizeof(cData));
    memset(&ACE_SBR_SET_PCKT, 0,sizeof(ACE_SBR_SET_PCKT));

    for(i=0; i<sizeof(DEV_TYPE); i++)
    {
        DEV_TYPE[i] = 0x00;
    }
    
    for(i=0; i<sizeof(RMCU_ID); i++)
    {
        RMCU_ID[i] = 0x00;
    }

    strcpy((char*)RMCU_ID,(const char*)SYS_PARS.RMCU_Id);
    strcpy((char*)DEV_TYPE,(const char*)SYS_PARS.Dev_Type);

    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)AES_SRT_BYTE);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)AES_MSG_CODE);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)AES_STATUS);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)AES_RTRY_CNTR);
    
    #ifndef GET_TIMESTAMP_FROM_PLC
    #ifdef RTC
    ReadRTC(&DATE_TIME);
    #endif
    #endif
    
    Datalen = strlen((const char*)ACE_SBR_SET_PCKT.Data);
    DataIndex = Datalen;

    cRes = Hex2Ascii_Frame(DATE_TIME.Date);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(DATE_TIME.Month);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Year.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    cRes = Hex2Ascii_Frame(DATE_TIME.Year.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Hour);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Mins);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(DATE_TIME.Seconds);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)AES_RESP_FOLLOW);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)"1B");
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)"7B");              /*SBR_BYTES_AFTER_SIZE+RMC_SBR_SUM_DATA_LENGTH*/
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)RMCU_ID);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)DEV_TYPE);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)AES_CONST4);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)AES_SETTING_CONST2);

    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_1);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_2);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_3);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_4);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_5);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_6);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_7);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_8);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_9);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_10);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_11);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_12);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_13);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_14);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_15);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_16);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_17);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_18);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_19);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_20);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_21);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_22);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_23);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_24);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_25);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_26);
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*)PARAM_CODE_27);
      
    Datalen = strlen((const char*)ACE_SBR_SET_PCKT.Data);
    DataIndex = Datalen;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_BATCH_TMR_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_BATCH_TMR_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.FILL_SBR_START_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.FILL_SBR_START_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.FILL_SBR_STOP_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.FILL_SBR_STOP_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.AERATE_SBR_START_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.AERATE_SBR_START_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.AERATE_SBR_STOP_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.AERATE_SBR_STOP_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.PRE_SETTLE_SBR_START_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.PRE_SETTLE_SBR_START_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.PRE_SETTLE_SBR_STOP_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.PRE_SETTLE_SBR_STOP_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.DECANT_SBR_START_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.DECANT_SBR_START_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.DECANT_SBR_STOP_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.DECANT_SBR_STOP_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.IDLE_SBR_START_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.IDLE_SBR_START_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.IDLE_SBR_STOP_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.IDLE_SBR_STOP_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.INTER_AERATE_FRQ_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.INTER_AERATE_FRQ_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.INTER_AERATE_RUN_MIN.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.INTER_AERATE_RUN_MIN.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P1_CUR_MIN_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P1_CUR_MIN_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P1_CUR_MAX_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P1_CUR_MAX_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P2_CUR_MIN_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P2_CUR_MIN_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P2_CUR_MAX_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P2_CUR_MAX_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P3_CUR_MIN_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P3_CUR_MIN_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P3_CUR_MAX_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.P3_CUR_MAX_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_HIGH_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_HIGH_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_LOW_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_LOW_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_HIGH_WINDOW_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_HIGH_WINDOW_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    memset (system_mode_char,'\0',sizeof(system_mode_char));
    
    //SBR interm aeration flag Status
    len = (UINT16)strlen((const char *)SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b7]);
    for(i = 0; i<len; i++)
    {
        sprintf(system_mode_char+i*2, "%02X", SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b7][i]);
    }
    
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*) system_mode_char);
    DataIndex += strlen((const char*)system_mode_char);
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_LOW_WINDOW_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_LOW_WINDOW_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_SENSOR_CHECK_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_SENSOR_CHECK_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_OVERFILL_TIMER_THRLD.byte.HB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(RMC_SBR_SYS_SET.SBR_TANK_OVERFILL_TIMER_THRLD.byte.LB);
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_SET_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    
    memset (system_mode_char,'\0',sizeof(system_mode_char));
    
    //SBR tank sensor switch over flag Status
    len = (UINT16)strlen((const char *)SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b8]);
    for(i = 0; i<len; i++)
    {
        sprintf(system_mode_char+i*2, "%02X", SBR_SEN_STATUS[RMC_SBR_SYS_PAR.SBR_SENSOR_STAT.bits.b8][i]);
    }
        
    strcat((char*)ACE_SBR_SET_PCKT.Data,(const char*) system_mode_char);
    DataIndex += strlen((const char*)system_mode_char);

    
    Datalen = strlen((const char *)ACE_SBR_SET_PCKT.Data);
    if(Datalen <= 50)
    {
        GetDeviceType();
        GetRMCUId();
        Construct_SBR_Settings_ParamsPacket();
    }

}

void Construct_SBR_Alarms_Packet(void)
{
    BYTE cData[500];
    UINT16_VAL cRes;
    UINT16 Datalen = 0,DataIndex = 0,i;

    memset((void *)cData, 0,sizeof(cData));
    memset(&ACE_SBR_ALM_PCKT, 0,sizeof(ACE_SBR_ALM_PCKT));

    for(i=0; i<sizeof(DEV_TYPE); i++)
    {
        DEV_TYPE[i] = 0x00;
    }

    for(i=0; i<sizeof(RMCU_ID); i++)
    {
        RMCU_ID[i] = 0x00;
    }

    strcpy((char*)RMCU_ID,(const char*)SYS_PARS.RMCU_Id);
    strcpy((char*)DEV_TYPE,(const char*)SYS_PARS.Dev_Type);

    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char *)AES_SRT_BYTE);
    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char*)AES_MSG_CODE);
    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char*)AES_STATUS);
    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char*)AES_RTRY_CNTR);

    memset(&DATE_TIME,0,sizeof(DATE_TIME));

    Datalen = strlen((const char*)ACE_SBR_ALM_PCKT.Data);
    DataIndex = Datalen;

    cRes = Hex2Ascii_Frame(Prev_Date_Time.Date);
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(Prev_Date_Time.Month);
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(Prev_Date_Time.Year.byte.HB);
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;
    cRes = Hex2Ascii_Frame(Prev_Date_Time.Year.byte.LB);
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(Prev_Date_Time.Hour);
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(Prev_Date_Time.Mins);
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    cRes = Hex2Ascii_Frame(Prev_Date_Time.Seconds);
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.HB;
    ACE_SBR_ALM_PCKT.Data[DataIndex++]   =  cRes.byte.LB;

    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char*)AES_RESP_FOLLOW);
    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char*)AES_ALM_PARM_NUM);
    strcat((char*)ACE_SBR_ALM_PCKT.Data,"13"); /*SBR_BYTES_AFTER_SIZE+3*/
    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char*)RMCU_ID);
    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char*)DEV_TYPE);
    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char*)AES_CONST2);
    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char*)AES_ALM_CONST2);

    strcat((char*)ACE_SBR_ALM_PCKT.Data,(const char*)PARAM_CODE_ALM);
    strcat((char*)ACE_SBR_ALM_PCKT.Data,"00000001");
}

void Construct_Device_ID_Packet(void)
{
    UINT i;

    memset(&ACE_DEV_ID_PCKT, 0,sizeof(ACE_DEV_ID_PCKT));

    for(i=0; i<sizeof(DEV_TYPE); i++)
    {
        DEV_TYPE[i] = 0x00;
    }

    for(i=0; i<sizeof(RMCU_ID); i++)
    {
        RMCU_ID[i] = 0x00;
    }

    strcpy((char*)RMCU_ID,(const char*)SYS_PARS.RMCU_Id);
    strcpy((char*)DEV_TYPE,(const char*)SYS_PARS.Dev_Type);

    DEV_TYPE[0] = (unsigned char)toupper(DEV_TYPE[0]);
    
    strcat((char*)ACE_DEV_ID_PCKT.Data,(const char *)AES_SRT_BYTE);
    strcat((char*)ACE_DEV_ID_PCKT.Data,(const char*)AES_MSG_CODE);
    strcat((char*)ACE_DEV_ID_PCKT.Data,(const char*)AES_RTRY_CNTR);

    strcat((char*)ACE_DEV_ID_PCKT.Data,(const char*)AES_RESP_FOLLOW);
    strcat((char*)ACE_DEV_ID_PCKT.Data,(const char*)AES_ALM_PARM_NUM);
    strcat((char*)ACE_DEV_ID_PCKT.Data,"13"); /*CLT_BYTES_AFTER_SIZE+3*/
    strcat((char*)ACE_DEV_ID_PCKT.Data,(const char*)RMCU_ID);
    strcat((char*)ACE_DEV_ID_PCKT.Data,(const char*)DEV_TYPE);
    strcat((char*)ACE_DEV_ID_PCKT.Data,(const char*)AES_CONST5);
    strcat((char*)ACE_DEV_ID_PCKT.Data,(const char*)AES_ID_CONST);

    strcat((char*)ACE_DEV_ID_PCKT.Data,(const char*)PARAM_CODE_1);
    strcat((char*)ACE_DEV_ID_PCKT.Data,"2000000B");
}

BYTE CalculateFCS(BYTE *DataAddrPtr,UINT16 DataLength)
{
    BYTE FCS_Buff[1024];
    
    UINT8 bytstrm[10];
    BYTE FCSResult = 0;
    UINT16 i = 0,Bytecnt = 0;
    UINT8 byt1 = 0,byt2 = 0;

    strcat((char *)FCS_Buff,(const char*)DataAddrPtr);
    DataLength = strlen((const char*)FCS_Buff);

    //Append FCS here
    //observed index goes beyond data and fetches NULL to fix that below lines are added.
    FCS_Buff[DataLength]     =  '0';
    FCS_Buff[DataLength+1]   =  '0';
    FCS_Buff[DataLength+2]   =  '0';
    FCS_Buff[DataLength+3]   =  '0';

   for( Bytecnt= 2 ;Bytecnt<= (DataLength-2);i++ )
    {

        bytstrm[0] = DataAddrPtr[Bytecnt++];
        bytstrm[1] = DataAddrPtr[Bytecnt++];
        bytstrm[2] = '\0';

        byt1 = ASCII_TO_HEX(bytstrm[0],bytstrm[1]);

        bytstrm[0] = DataAddrPtr[Bytecnt++];
        bytstrm[1] = DataAddrPtr[Bytecnt++];
        bytstrm[2] = '\0';
        i++;

        byt2 = ASCII_TO_HEX(bytstrm[0],bytstrm[1]);

        FCSResult ^= byt1;
        FCSResult ^= byt2;

    }
    return FCSResult;
}


unsigned int Hex2Ascii (unsigned char DataByte)
{
    unsigned char Temp_DB_LSB	;
    unsigned char ch;

    Temp_DB_LSB = DataByte ;

    switch ( Temp_DB_LSB )
    {
        case 0x00:
            ch = '0';
            break;

        case 0x01:
            ch = '1';
            break;

        case 0x02:
            ch = '2';
            break;

        case 0x03:
            ch = '3';
            break;

        case 0x04:
            ch = '4';
            break;

        case 0x05:
            ch = '5';
            break;

        case 0x06:
            ch = '6';
            break;

        case 0x07:
            ch = '7';
            break;

        case 0x08:
            ch = '8';
            break;

        case 0x09:
            ch = '9';
            break;

        case 0x0A:
            ch = 'A';
            break;

        case 0x0B:
            ch = 'B';
            break;

        case 0x0C:
            ch = 'C';
            break;

        case 0x0D:
            ch = 'D';
            break;

        case 0x0E:
            ch = 'E';
            break;

        case 0x0F:
            ch = 'F';
            break;
    }
    return ch;
}



void ProcessFloatResult(UINT32_VAL *val)
{
    UINT64 temp;
    float F_Res = 0;

    temp = val->Val;
    F_Res = (float)(temp / 10);

    val->Val = 0;
    val->Val = (UINT32)F_Res;
}


void ProcessFloatResultforEnergy(UINT32_VAL *val)
{
    UINT64 temp;
    float F_Res = 0;
    
    temp = val->Val;
    F_Res = (float)(temp / 10);
    
    val->Val = 0;
    val->Val = (UINT32)F_Res;
}


void ProcessFloatResultforSMS(UINT32_VAL *val,double *Dst)
{
    UINT64 temp;
    double F_Res = 0;
    
    temp = val->Val;
    
    F_Res = (double)(temp);
    F_Res /=1000 ;

    *Dst = 0;
    *Dst = (double)F_Res;
}

BYTE ValidDateTime(void)
{
    unsigned long int prv_minutes = 0,cur_minutes = 0;
    
    prv_minutes = calculateDatetimeInMinutes(Prev_Date_Time);
    cur_minutes = calculateDatetimeInMinutes(DATE_TIME);
    // Compare year
    if (DATE_TIME.Year.Val < Prev_Date_Time.Year.Val)
    {
        return 0;
    }
    else if (DATE_TIME.Year.Val == Prev_Date_Time.Year.Val)
    {
        if(cur_minutes < prv_minutes )
            return 0;
    }
    Prev_Date_Time.Year.Val = DATE_TIME.Year.Val;
    Prev_Date_Time.Month = DATE_TIME.Month;
    Prev_Date_Time.Date = DATE_TIME.Date;
    Prev_Date_Time.Hour = DATE_TIME.Hour;
    Prev_Date_Time.Mins = DATE_TIME.Mins;
    Prev_Date_Time.Seconds = DATE_TIME.Seconds;
    Prev_Date_Time.Week_Day = DATE_TIME.Week_Day;
    return 1;

}
static unsigned long int calculateDatetimeInMinutes(_DATETIME dateTimeBuff)
{
    unsigned long int nomDays = 0,inMinutes=0;
    unsigned int index=0;
    unsigned int year = dateTimeBuff.Year.Val,month=dateTimeBuff.Month;
    unsigned int monthDays[12] = {31,28,31,30,31,31,31,31,30,31,30,31};
    
    if(((year%4) == 0) && (((year%100) != 0)||((year%400) == 0)))         // Leap year comparision
    {    if(month > 2)                            // Leap month comparision
         {   ++nomDays; }                                       // Increment days to 1 if it is leap year
    }
    for(index=0;index<(month-1);++index)                          // Add number of previous days in a month to day counter
    {    nomDays += monthDays[index]; }
    
    nomDays += (unsigned long int)dateTimeBuff.Date;               // Add day's value in a month to day counter
    
    inMinutes = (unsigned long int)(((nomDays * 24) + (dateTimeBuff.Hour)) * 60) + dateTimeBuff.Mins;                            // Convert day's into minutes
    
    return inMinutes;
}

void WriteDateTime(_DATETIME DT,UINT16 RegAddr)
{
    UINT16_VAL temp;
    
    temp.Val = DT.Seconds;
    DataBuffer[0] = temp.byte.HB;
    DataBuffer[1] = temp.byte.LB;
    
    temp.Val = DT.Mins;
    DataBuffer[2] = temp.byte.HB;
    DataBuffer[3] = temp.byte.LB;
    
    temp.Val = DT.Hour;
    DataBuffer[4] = temp.byte.HB;
    DataBuffer[5] = temp.byte.LB;
   
    temp.Val = DT.Date;
    DataBuffer[6] = temp.byte.HB;
    DataBuffer[7] = temp.byte.LB;
    
    temp.Val = DT.Month;
    DataBuffer[8] = temp.byte.HB;
    DataBuffer[9] = temp.byte.LB;
    
    temp.Val = DT.Week_Day;
    DataBuffer[10] = temp.byte.HB;
    DataBuffer[11] = temp.byte.LB;
    
    temp.Val = DT.Year.Val;
    DataBuffer[12] = temp.byte.HB;
    DataBuffer[13] = temp.byte.LB;
    
    ModbusWrite(DEVICE,RegAddr,WRITE_MUL_REG,DataBuffer,7);
}

void WriteCurrDateTime(_DATETIME DT,UINT16 RegAddr)
{
    UINT16_VAL temp;
    Write_Count=0;
    
    temp.Val = DT.Year.Val;;
    DataBuffer[0] = temp.byte.HB;
    DataBuffer[1] = temp.byte.LB;
    
    temp.Val = DT.Week_Day;
    DataBuffer[2] = temp.byte.HB;
    DataBuffer[3] = temp.byte.LB;
    
    temp.Val = DT.Month;
    DataBuffer[4] = temp.byte.HB;
    DataBuffer[5] = temp.byte.LB;
   
    temp.Val = DT.Date;
    DataBuffer[6] = temp.byte.HB;
    DataBuffer[7] = temp.byte.LB;
    
    temp.Val = DT.Hour;
    DataBuffer[8] = temp.byte.HB;
    DataBuffer[9] = temp.byte.LB;
    
    temp.Val = DT.Mins;
    DataBuffer[10] = temp.byte.HB;
    DataBuffer[11] = temp.byte.LB;
    
    temp.Val = DT.Seconds;
    DataBuffer[12] = temp.byte.HB;
    DataBuffer[13] = temp.byte.LB;
    
    ModbusWrite(DEVICE,RegAddr,WRITE_MUL_REG,DataBuffer,7);
    
    DataBuffer[1] = 0x00;
    DataBuffer[0] = 0xFF;
    ModbusWrite(DEVICE,RMC_SYM_SET_RTC,FORCE_SINGLE_COIL,DataBuffer,1);
    
    DataBuffer[1] = 0x00;    
    DataBuffer[0] = 0x00;
    ModbusWrite(DEVICE,RMC_SYM_SET_RTC,FORCE_SINGLE_COIL,DataBuffer,1);
    
    InvalidDateTimeCount = 0;
}

void ReadPrevTime(void)
{
    Prev_Date_Time.Year.Val = 0;
    Prev_Date_Time.Month = 0;
    Prev_Date_Time.Date = 0;
	
	ModbusRead(DEVICE, RMC_SYM_PREV_SECS,7);
    
    if ((Prev_Date_Time.Year.Val == 0) || (Prev_Date_Time.Month == 0) || (Prev_Date_Time.Date == 0))
    {
        Prev_Date_Time.Year.Val = 2018;
        Prev_Date_Time.Month = 1;
        Prev_Date_Time.Date = 1;
        Prev_Date_Time.Hour = 0;
        Prev_Date_Time.Mins = 0;
        Prev_Date_Time.Seconds = 0;
        Prev_Date_Time.Week_Day = 2;
    }
}

void GetMdbPrevTime(void)
{
    UINT16 StAddrCount = 0;
    TmpResult.Val = 0;
    
    TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
    Prev_Date_Time.Seconds = TmpResult.word.LW;
    StAddrCount += PLC_REG_DEFAULT_SIZE;
    
    TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
    Prev_Date_Time.Mins = TmpResult.word.LW;
    StAddrCount += PLC_REG_DEFAULT_SIZE;
    
    TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
    Prev_Date_Time.Hour = TmpResult.word.LW;
    StAddrCount += PLC_REG_DEFAULT_SIZE;
    
    TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
    Prev_Date_Time.Date = TmpResult.word.LW;
    StAddrCount += PLC_REG_DEFAULT_SIZE;
    
    TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
    Prev_Date_Time.Month = TmpResult.word.LW;
    StAddrCount += PLC_REG_DEFAULT_SIZE;
    
    TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
    Prev_Date_Time.Week_Day = TmpResult.word.LW;
    StAddrCount += PLC_REG_DEFAULT_SIZE;
    
    TmpResult = GetResponseBytes(StAddrCount,PLC_REG_DEFAULT_SIZE);
    Prev_Date_Time.Year.Val = TmpResult.word.LW;
}

void CopyServerTime(void)
{
    Prev_Date_Time.Year.Val = DATE_TIME.Year.Val;
    Prev_Date_Time.Month = DATE_TIME.Month;
    Prev_Date_Time.Date = DATE_TIME.Date;
    Prev_Date_Time.Hour = DATE_TIME.Hour;
    Prev_Date_Time.Mins = DATE_TIME.Mins;
    Prev_Date_Time.Seconds = DATE_TIME.Seconds;
    Prev_Date_Time.Week_Day = DATE_TIME.Week_Day;
}