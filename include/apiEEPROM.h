/*
 * File:   apiEEPROM.h
 * Author: Technosphere
 *
 * Created on March 3, 2014, 2:21 PM
 */

#ifndef APIEEPROM_H
#define	APIEEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif


#define EE_SERVERADDR_ADDR             0
#define EE_APN_ADDR                    40
#define EE_APPID_ADDR                  65
#define EE_DEVICEYYPE_ADDR             75
#define EE_RMCUID_ADDR                 80
#define EE_MN_ADDR                     98
#define EE_TD_ADDRR                    110
#define EE_Received_MN_ADDR            115
#define EE_RESTART_ADDRR               127


#define EE_ERASEST_ADDR                 0
#define EE_ERASEEND_ADDR                127

//Size
#define EE_SERVERADDR_ADDR_SIZE         40
#define EE_APN_ADDR_SIZE                25
#define EE_APPID_ADDR_SIZE              10
#define EE_DEVICEYYPE_ADDR_SIZE         5
#define EE_RMCUID_ADDR_SIZE             18
#define EE_MN_ADDR_SIZE                 12
#define EE_ReceivedMN_ADDR_SIZE         12
#define EE_TD_ADDRR_SIZE                5
#define EE_RESTART_ADDRR_SIZE           2

#define SERVER_ADDRESS_LENGTH           40
#define APN_LENGTH                      30
#define APP_ID_LENGTH                   10
#define DEV_TYPE_LENGTH                 6
#define RMCU_ID_LENGTH                  20
#define MOBILE_NUMBER_LENGTH            15
#define WEB_PERIOD_LENGTH               6
#define RTC_BUF_LENGTH                  25


typedef struct __PACKED
{
    unsigned char Server_Address[SERVER_ADDRESS_LENGTH];
    unsigned char APN[APN_LENGTH];
    unsigned char App_Id[APP_ID_LENGTH];
    unsigned char Dev_Type[DEV_TYPE_LENGTH];
    unsigned char RMCU_Id[RMCU_ID_LENGTH];
    unsigned char MobileNum[MOBILE_NUMBER_LENGTH];
    unsigned char ReceivedMobileNum[MOBILE_NUMBER_LENGTH];
    unsigned char TaskDuration[WEB_PERIOD_LENGTH];

    unsigned char Rtc_Buf[RTC_BUF_LENGTH];
    
}_SYS_PARS;

extern _SYS_PARS SYS_PARS;
extern _SYS_PARS tSYS_PARS;




/**************************************************************
                Prototype declaration
**************************************************************/

void GetAPN(void);
void GetAppId(void);
void GetRMCUId(void);
void GetWebPeriod(void);
void GetDeviceType(void);
void GetMobileNumber(void);
void GetReceivedMobileNumber(void);
void CheckSystemRestartByte(void);



void Write_APN(void);
void ReadEEPROM(void);
void EraseEEPROM(void);
void Write_AppId(void);
void Write_RMCUId(void);
void Write_DevType(void);
void Write_WebPeriod(void);
void Write_RestartByte(void);
void Write_MobileNumber(void);
void Write_ReceivedMobileNumber(void);
void Write_ServerAddress(void);
void GetServerAddress(void);

extern volatile unsigned long WebPeriod;
extern unsigned int Task_Duration_H;
extern unsigned int Task_Duration_M;
extern unsigned char tTask_Duration[5];
#ifdef	__cplusplus
}
#endif

#endif	/* APIEEPROM_H */

