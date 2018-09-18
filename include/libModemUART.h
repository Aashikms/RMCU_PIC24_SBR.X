/* 
 * File:   ModemUART.h
 * Author: Phoenix
 *
 * Created on August 25, 2013, 12:54 PM
 */

#ifndef MODEMUART_H
#define	MODEMUART_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "config.h"
#include "pSystem.h"


#define GSM_UART_BAUD_RATE 19200


#define BRG_REG_VALUE_GSM   (((GetPeripheralClock()/GSM_UART_BAUD_RATE)/4)-1)

    
#define SERVER_CMD_HF_LENGTH        13
#define SERVER_CMD_HEADER_LENGTH    67

#define AIRTEL        1
#define BSNL          2
#define IDEA          3
#define VODAFONE      4
#define AIRCEL        5

typedef enum
{
    MODEM_NO_TASK = 0,

    MODEM_SEND_CLT_SUMMERY_TASK,
    MODEM_SEND_CLT_STATUS_TASK,
    MODEM_SEND_EM_PROFILE_TASK,
    MODEM_SEND_ALARMS_TASK,
    MODEM_SEND_DEV_INFO_TASK,
    MODEM_SEND_DEV_ID_TASK,

    MODEM_READ_SMS_TASK,
    MODEM_SEND_SMS_TASK,
}_MODEM_TASK_STATES;

extern _MODEM_TASK_STATES MODEM_TASK_STATE;

typedef struct
{
    _MODEM_TASK_STATES MODEM_TASK_STATE;
    char Parameters[12];
}_MODEM_TASKS;

extern _MODEM_TASKS mTask;

typedef struct
{
    unsigned char Second;
    unsigned char Minute;
    unsigned char Hour;
    unsigned char Year;
    unsigned char Date;
    unsigned char Month;
}_NETWORK_TIMESTMAP;

extern _NETWORK_TIMESTMAP NRTS;

typedef enum
{
    MODEM_COMMAND = 0,
    MODEM_SENSOR_DATA,
    MODEM_IMAGE_DATA,
    MODEM_ACK_DATA,
    MODEM_HELLO_DATA,
}_MODEM_DATA_TYPE;

typedef enum
{   
    MODEM_TIMER_PERIOD_1S = 1,              /**< Timer period 1 seconds */
    MODEM_TIMER_PERIOD_2S = 2,              /**< Timer period 2 seconds */
    MODEM_TIMER_PERIOD_3S = 3,              /**< Timer period 3 seconds */
    MODEM_TIMER_PERIOD_5S = 5,              /**< Timer period 5 seconds */
    MODEM_TIMER_PERIOD_6S = 6,              /**< Timer period 6 seconds */
    MODEM_TIMER_PERIOD_7S = 7,              /**< Timer period 7 seconds */
    MODEM_TIMER_PERIOD_8S = 8,              /**< Timer period 8 seconds */
    MODEM_TIMER_PERIOD_9S = 9,              /**< Timer period 9 seconds */
    MODEM_TIMER_PERIOD_10S = 10,            /**< Timer period 10 seconds */
    MODEM_TIMER_PERIOD_20S = 20,            /**< Timer period 10 seconds */
    MODEM_TIMER_PERIOD_15S = 15,            /**< Timer period 15 seconds */
    MODEM_TIMER_PERIOD_40S = 40,            /**< Timer period 45 seconds*/
    MODEM_TIMER_PERIOD_60S = 60,            /**< Timer period 45 seconds*/
    MODEM_TIMER_PERIOD_45S = 45,            /**< Timer period 45 seconds*/
    MODEM_TIMER_PERIOD_15MS = 15,           /**< Timer period 15 milli seconds */
    MODEM_TIMER_PERIOD_200MS = 200,         /**< Timer period 200 milli seconds */
    MODEM_TIMER_PERIOD_800MS = 800,         /**< Timer period 800 milli seconds */
    MODEM_TIMER_PERIOD_1000MS = 1000,       /**< Timer period 1000 milli seconds */
    MODEM_TIMER_PERIOD_2000MS = 2000,       /**< Timer period 1500 milli seconds */
    MODEM_TIMER_PERIOD_2500MS = 2500,       /**< Timer period 1500 milli seconds */
    MODEM_TIMER_PERIOD_3SEC = 3000,       /**< Timer period 5 secs */
    MODEM_TIMER_PERIOD_5SEC = 5000,       /**< Timer period 5 secs */
    MODEM_TIMER_PERIOD_10SEC = 10000,       /**< Timer period 5 secs */
    MODEM_TIMER_PERIOD_15SEC = 15000,       /**< Timer period 5 secs */
} MODEM_TIMER_PERIOD;

#define  MODEM_TIMER_PERIOD_HALF_S = 0.5,              /**< Timer period Half seconds */

typedef enum
{
    MODEM_TIMER_PRESCALER_1 = T2_PS_1_1,     /**< Timer prescaler 1:1 */
    MODEM_TIMER_PRESCALER_2 = T2_PS_1_8,     /**< Timer prescaler 1:8 */
    MODEM_TIMER_PRESCALER_4 = T2_PS_1_64,     /**< Timer prescaler 1:64 */
    MODEM_TIMER_PRESCALER_256 = T2_PS_1_256,
    SERVER_TIMER_PRESCALER_256 = T2_PS_1_256,
    
} MODEM_TIMER_PRESCALER;

#define REORDER_BUF_SIZE        1024
#define SMS_REORDER_BUF_SIZE    MAX_CHARS_IN_SMS
#define SERVER_REORDER_BUF_SIZE 1200


#define T23_PERIOD     1   //milli seconds
#define PRESCALAR      256
#define T23_TICK      ((GetPeripheralClock()/PRESCALAR/1000)*T23_PERIOD)
#define DsModemTimerInterrupt() DisableIntT3

typedef enum
{
    MODEM_CLOSED = 0,
    MODEM_POWER_CYCLE,
    MODEM_NOT_INITIALIZED,
    MODEM_INITIALIZED,
    MODEM_TX_TRIGGRED,
}_MODEM_STATUS;

extern _MODEM_STATUS MODEM_STATUS;

typedef enum
{
    SERVER_DISCONNECTED = 0,
    SERVER_CONNECTED,
}_MODEM_CONNECTION_STATUS;

extern _MODEM_CONNECTION_STATUS MODEM_CONNECTION_STATUS;

typedef enum
{
    EOS_WAIT = 0,
    EOS_RXD,
    EOS_PROCESS,
}_MODEM_UARTRX_STATUS;

extern _MODEM_UARTRX_STATUS mUARTRX_STATUS;

typedef enum
{
    RECEIVE_WAIT = 0,
    RECEIVE_DONE,
}_MODEM_RX_STATUS;

extern _MODEM_RX_STATUS MODEM_RX_STATUS;

typedef enum
{
    MODEM_POWERDOWN = 0,
    MODEM_POWERUP,
    MODEM_TURNOFF,
    MODEM_HW_TURNOFF,
    MODEM_TURNOFF_DELAY,
    MODEM_WAIT,
    MODEM_STANDBY,
    MODEM_AT,
    MODEM_ATE,

    MODEM_DCGATT,
    MODEM_CSQ,
    MODEM_VERSION,
    MODEM_QNITZ_EN,
    MODEM_QLTS_EN,
    MODEM_SET_CCLK,
    MODEM_GET_CCLK,
    MODEM_GET_GPRS_DATA,
    MODEM_BASE_CSQ,

    #ifdef AT_CSCS
    MODEM_CSCS,
    #endif

    MODEM_CFUN,
    MODEM_QSPN,
    MODEM_CREG,
    MODEM_CGREG,
    MODEM_CGATT,
    MODEM_CGATT_MATT,
    MODEM_QINISTAT,

    MODEM_CMGS_MODE,
    MODEM_CMGF,
    MODEM_CMGR,
    MODEM_DEL_ALL,
    MODEM_DEL_READ,
    MODEM_CMGS,
    MODEM_CMGS_TEXT,
            
    MODEM_QIFGCNT,
    MODEM_QICSGP,
    MODEM_QIMUX,
    MODEM_QIMODE,
    MODEM_QITCFG,
    MODEM_QIDNSIP,
    MODEM_QIREGAPP,
    MODEM_QIACT,
    MODEM_GET_IP,
    MODEM_GET_IP_RESP,
    MODEM_QIOPEN,
    MODEM_QISEND,
    MODEM_GREATER,
    MODEM_ACK_SERVER,
    MODEM_SEND_READY,
    MODEM_SEND_DATA,
    MODEM_CHECK_ACK,
    MODEM_QICLOSE,
    MODEM_QIDEACT,
    MODEM_SLEEP,
    MODEM_FUN,

    MODEM_RESP_WAIT,
    MODEM_ACK_WAIT,
    MODEM_DLY_WAIT,
    MODEM_SET_DLY,
    
    MODEM_TIMEOUT,
    MODEM_STIMEOUT,
    MODEM_TRIPPLE_PLUS,

    MODEM_POWERDOWN_RESP,
    MODEM_AT_RESP,
    MODEM_ATE_RESP,

    MODEM_DCGATT_RESP,
    MODEM_CSQ_RESP,
    MODEM_SERVER_RESP,
    MODEM_QNITZ_EN_RESP,
    MODEM_QLTS_EN_RESP,
    MODEM_SET_CCLK_RESP,
    MODEM_GET_CCLK_RESP,
    MODEM_BUILD_STRING_RESP,
    MODEM_BASE_CSQ_RESP,

    #ifdef AT_CSCS
    MODEM_CSCS_RESP,
    #endif

    MODEM_CFUN_RESP,
    MODEM_QSPN_RESP,

    MODEM_CREG_RESP,
    MODEM_CGREG_RESP,
    MODEM_CGATT_RESP,
    MODEM_QINISTAT_RESP,
    MODEM_CGATT_MATT_RESP,
    MODEM_CMGS_MODE_RESP,
    MODEM_CMGS_RESP,
    MODEM_CMGS_TEXT_RESP,
    MODEM_QIFGCNT_RESP,
    MODEM_QICSGP_RESP,
    MODEM_QIMUX_RESP,
    MODEM_QIMODE_RESP,
    MODEM_QITCFG_RESP,
    MODEM_QIDNSIP_RESP,
    MODEM_IMAGE_RESP,
    MODEM_QIREGAPP_RESP,
    MODEM_QIACT_RESP,    
    MODEM_QIOPEN_RESP,
    MODEM_QISEND_RESP,
    MODEM_GREATER_RESP,
    MODEM_PRE_QICLOSE_RESP,
    MODEM_QICLOSE_RESP,
    MODEM_QIDEACT_RESP,
    MODEM_CMGF_RESP,
    MODEM_CMGR_RESP,
    MODEM_DEL_ALL_RESP,
    MODEM_DEL_READ_RESP,
    MODEM_TURNOFF_RESP,
    MODEM_SLEEP_RESP,
    MODEM_FUN_RESP,
    MODEM_TRIPPLE_PLUS_RESP,

    MODEM_PROCESS_DATA,
}_MODEM_STATE;

extern _MODEM_STATE MODEM_STATE;
extern _MODEM_STATE nMODEM_STATE;
extern _MODEM_STATE cMODEM_STATE;

typedef union
{
    unsigned int GPRS_FLAGS;
    
    struct
    {
        unsigned CREG:1;
        unsigned CGREG:1;
        unsigned CGATT:1;
        unsigned CGATT_FAIL:1;
        unsigned COPS:1;
        unsigned CMGR:1;
        unsigned QINISTAT:1;
        unsigned QSPN:1;
        unsigned SIGNAL:1;
        unsigned MSG_RCVD:1;        //Message received, CMGR state to be invoked..
        unsigned MSG_RPT:1;         //Previous message read failed, MSG_RCVD flag to be set in MODEM_QIDEACT_RESP state
        unsigned MSG_PRCD:1;
        unsigned SC_PROVIDER:3;
        unsigned CMS_ERROR:1;
        unsigned IMG_RESP:1;

        char SG_STNTH[2];
        char MsgIndex[2];      
    };
}_GPRS_STATUS;

extern _GPRS_STATUS GPRS_STATUS;

typedef struct
{
    _MODEM_DATA_TYPE Type;
    int Size;
    BYTE pGPRSData[GPRS_TX_BUFFER_SIZE];
}_GPRS_DATA;

extern _GPRS_DATA pGPRSTxFifo;
extern _GPRS_DATA ModemTxBuffer;


int SetDlyTimer(void);
char *PopMsgInedx(void);


void ResetModemTimer(void);
void SetTimeoutTimer(void);
void ConfigModemUART(void);
void CheckEndOfRxBuffer(void);
UINT32 CheckMsgIndexFifo(void);
void Set_NetRTC_Dt_Time(void);
void ReorderBuffer(BYTE* buf);
void SMSReorderBuffer(BYTE* buf);
void StartDataUploadTimer(void);
void PushMsgInedx(char *message);
void ProcessModemResponse(void);
void SetServerCmdTimeoutTimer(void);
void CheckGreaterSymbol(BYTE Rxbyte);
void ReorderBufferForSMS(BYTE* buf);
void ServerRespReorderBuffer(BYTE* buf);
void StartServerResponseTimeoutTimer(void);
UINT8 PharseServerTime(char* buf, UINT8 Format);


extern void PutCharModemUARTBuff(char ch);
void PutModemUART(char *pcString, _MODEM_DATA_TYPE mdType, int size);


#ifdef	__cplusplus
}
#endif

#endif	/* MODEMUART_H */

