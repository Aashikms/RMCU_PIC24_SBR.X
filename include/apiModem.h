/* 
 * File:   ModemStack.h
 * Author: Phoenix
 *
 * Created on August 25, 2013, 1:04 PM
 */

#ifndef MODEMSTACK_H
#define	MODEMSTACK_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "apiModem.h"
#include "libModemUART.h"

//for ERROR response states
#define INVALID_PARAMETER_ERR            0x01
#define INVALID_COMMAND_ERR              0x02
#define INCOMPLETE_PARAMETERS_ERR        0x03
#define CONNECTION_FAILED_ERR            0x04
#define FAILEDTOSEND_DATA_ERR            0x05
#define PLC_COMM_FAILURE_ERR             0x06
#define FAILEDTO_SAVEDATAIN_PLC_ERR      0x07
#define FAILEDTO_SAVEDATAIN_RMCU_ERR     0x08


    
typedef enum
{
    NO_CMD = 0,
    SERVER_ADDR_CMD,
    NET_APN_CMD,
    APP_ID_CMD,
    DEV_TYPE_CMD,
    RMCU_ID_CMD,
    MN_CMD,
    MN_CMD_RESP_CONFIG_NUM,
    INVALID_CMD,
    RTC_CMD,
    TASK_DURATION_CMD,
    GET_RMCU_SETTINGS,
    GET_PLC_SETTINGS,
    RESTART_RMCU,
    RESTART_PLC,
    

    SET_TIMERS,
    /*SET_TIMER1,
    SET_TIMER2,
    SET_TIMER3,
    SET_TIMER4,
    SET_P1CURRENT,
    SET_P2CURRENT,
    SET_P3CURRENT,*/
    
    SET_SBR_MAINTENANCE,
    SET_PLC_RTC_CMD,
    SERVER_TIME_TO_PLC,
    
}RXSMS_CMDS;

extern RXSMS_CMDS RxdSMSCmd;

typedef struct
{
    RXSMS_CMDS SMS_TASK_STATE;
    BYTE Parameters[12];
}_SMS_TASKS;

extern _SMS_TASKS SMS_TASKS;




typedef struct  __PACKED
{
    UINT8 Data[1024];
}_ACE_SBR_MSG_PCKT;

extern _ACE_SBR_MSG_PCKT ACE_SBR_SUM_PCKT;
extern _ACE_SBR_MSG_PCKT ACE_SBR_STAT_PCKT;
extern _ACE_SBR_MSG_PCKT ACE_SBR_EM_PCKT;
extern _ACE_SBR_MSG_PCKT ACE_SBR_SET_PCKT;
extern _ACE_SBR_MSG_PCKT ACE_SBR_ALM_PCKT;
extern _ACE_SBR_MSG_PCKT ACE_DEV_ID_PCKT;

void ResetModem(void);
void ProcessSMS(void);
void Modem_Stack(void);
void ModemPowerOn(void);
void ModemPowerOff(void);
void ModemCallBack(void);
void ModemTurnOnState(void);

void ConfigModem(void);
void GetImageData(void);
void GetHelloData(void);
void GetImpSensorData(void);


void CheckDataType(void);
BYTE* CheckSMSCmd(void);

_MODEM_STATE CheckACKfromServer(void);
_MODEM_STATE CheckDataTerminate(void);

_MODEM_STATE CheckConClose(void);
_MODEM_STATE CheckCMGFTask(void);
_MODEM_STATE CheckQINISTATTask(void);


BYTE PopSMSstatus(void);
UINT32 CheckSMSstatusFifo(void);
void PushSMSstatus(BYTE Status);

void InitSMSTasks(void);
char *pMsgRxFifoPop(void);
void SetRTC_Dt_Time(void);
_SMS_TASKS GetSMSTask(void);
UINT32 pCheckMsgRxFifo(void);
UINT32 pCheckGPRSTxFifo(void);
void ConstructGPRSPacket(void);
void ReloadRestartCounter(void);
_GPRS_DATA *pGPRSTxFifoPop(void);
void pMsgRxFifoPush(BYTE *message);


void InsertSMSTask(_SMS_TASKS SmsTask);
void pGPRSTxFifoPush(_GPRS_DATA *message);
void CopyPacket2ModemBuffer(_ACE_SBR_MSG_PCKT *message);
void CopyPacket2ModemBufferAll(_ACE_SBR_MSG_PCKT *message);
BYTE *ConstructErrorResponseString (BYTE Errcode,const BYTE *RespString);
void RTCErrorResponse(void);



extern char *ModemTxFifoPop(void);
extern UINT32 MsgGetTxFifoIndex(void);
extern void ModemTxFifoPush(const char *message);




#ifdef	__cplusplus
}
#endif

#endif	/* MODEMSTACK_H */

