
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apiRTC.h"
#include <libI2C.h>
#include "pSystem.h"
#include "apiModem.h"
#include <apiDelay.h>
#include <apiEEPROM.h>


extern _FLAGS FLAGS;

extern const BYTE dAPN[30];
extern const BYTE dAppId[10];
extern const BYTE dDevType[6];
extern const BYTE dRMCUId[20];
extern const BYTE dTaskDuration[6];
extern const BYTE dMobile_Number[20];
extern const BYTE dServerAddress[50];

volatile unsigned long WebPeriod = 5;

/**************************************************************
                Variable delaration
**************************************************************/

_SYS_PARS SYS_PARS;
_SYS_PARS tSYS_PARS;
extern _SMS_TASKS SMS_TASKS;


BYTE APN[30];
BYTE APP_ID[10];
BYTE RMCU_ID[20];
BYTE DEV_TYPE[10];
BYTE WEBPERIOD[10];
BYTE MOBILENUMBER[20];
BYTE SERVER_ADDRESS[50];


UINT8 TempReadAddress = 0;
UINT8 TempWriteAddress = 0;
unsigned int Task_Duration_H = 2;
unsigned int Task_Duration_M = 2;
unsigned char tTask_Duration[5];

extern BYTE SMS_Rxd_MobileNumber[14];
/******************************************************************/

#ifdef RTC

void Write_ServerAddress(void)
{
    UINT16 i,Len = 0;

    TempWriteAddress = EE_SERVERADDR_ADDR;

    for(i = 0; i<(EE_SERVERADDR_ADDR_SIZE-2); i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress,0x00);
        DelayMs(5);
        TempWriteAddress++;
    }

    TempWriteAddress = EE_SERVERADDR_ADDR;
    Len = strlen((const char*)SYS_PARS.Server_Address);

    for(i = 0; i<Len; i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress, SYS_PARS.Server_Address[i]);
        DelayMs(5);
        TempWriteAddress++;
    }

    //To be implemnted in pic24 I2C
    //SMS_TASKS.Parameters[0] = 0x08;

    SMS_TASKS.Parameters[0] = 0xFF;
}


void Write_APN(void)
{
    UINT16 i,Len = 0;

    TempWriteAddress = EE_APN_ADDR;

    for(i = 0; i<(EE_APN_ADDR_SIZE-2); i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress,0x00);
        DelayMs(5);
        TempWriteAddress++;
    }

    TempWriteAddress = EE_APN_ADDR;
    Len = strlen((const char*)SYS_PARS.APN);

    for(i = 0; i<Len; i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress, SYS_PARS.APN[i]);
        DelayMs(5);
        TempWriteAddress++;
    }

    //To be implemnted in pic24 I2C
    //SMS_TASKS.Parameters[0] = 0x08;

    SMS_TASKS.Parameters[0] = 0xFF;
}

void Write_AppId(void)
{
    UINT16 i,Len = 0;

    TempWriteAddress = EE_APPID_ADDR;

    for(i = 0; i<(EE_APPID_ADDR_SIZE-2); i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress,0x00);
        DelayMs(5);
        TempWriteAddress++;
    }

    TempWriteAddress = EE_APPID_ADDR;
    Len = strlen((const char*)SYS_PARS.App_Id);

    for(i = 0; i<Len; i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress, SYS_PARS.App_Id[i]);
        DelayMs(5);
        TempWriteAddress++;
    }

    //To be implemnted in pic24 I2C
    //SMS_TASKS.Parameters[0] = 0x08;

    SMS_TASKS.Parameters[0] = 0xFF;
}


void Write_DevType(void)
{
    UINT16 i,Len = 0;

    TempWriteAddress = EE_DEVICEYYPE_ADDR;

    for(i = 0; i<(EE_DEVICEYYPE_ADDR_SIZE-2); i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress,0x00);
        DelayMs(5);
        TempWriteAddress++;
    }

    TempWriteAddress = EE_DEVICEYYPE_ADDR;
    Len = strlen((const char*)SYS_PARS.Dev_Type);

    for(i = 0; i<Len; i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress, SYS_PARS.Dev_Type[i]);
        DelayMs(5);
        TempWriteAddress++;
    }

    //To be implemnted in pic24 I2C
    //SMS_TASKS.Parameters[0] = 0x08;

    SMS_TASKS.Parameters[0] = 0xFF;
}


void Write_RMCUId(void)
{
    UINT16 i,Len = 0;

    TempWriteAddress = EE_RMCUID_ADDR;

    //Clear EEPROM contents before doing write
    for(i = 0; i<(EE_RMCUID_ADDR_SIZE-2); i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress,0x00);
        DelayMs(5);
        TempWriteAddress++;
    }

    TempWriteAddress = EE_RMCUID_ADDR;
    Len = strlen((const char*)SYS_PARS.RMCU_Id);

    for(i = 0; i<Len; i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress, SYS_PARS.RMCU_Id[i]);
        DelayMs(5);
        TempWriteAddress++;
    }

    //To be implemnted in pic24 I2C
    //SMS_TASKS.Parameters[0] = 0x08;

    SMS_TASKS.Parameters[0] = 0xFF;
}


void Write_MobileNumber(void)
{
    UINT16 i,Len = 0;

    TempWriteAddress = EE_MN_ADDR;

    //Clear EEPROM contents before doing write
    for(i = 0; i<(EE_MN_ADDR_SIZE-2); i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress,0x00);
        DelayMs(5);
        TempWriteAddress++;
    }

    TempWriteAddress = EE_MN_ADDR;
    Len = strlen((const char*)SYS_PARS.MobileNum);

    for(i = 0; i<Len; i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress, SYS_PARS.MobileNum[i]);
        DelayMs(5);
        TempWriteAddress++;
    }

    //To be implemnted in pic24 I2C
    //SMS_TASKS.Parameters[0] = 0x08;
    
    SMS_TASKS.Parameters[0] = 0xFF;
}

void Write_ReceivedMobileNumber(void)
{
    UINT16 i,Len = 0;

    TempWriteAddress = EE_Received_MN_ADDR;

    //Clear EEPROM contents before doing write
    for(i = 0; i<(EE_ReceivedMN_ADDR_SIZE-2); i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress,0x00);
        DelayMs(5);
        TempWriteAddress++;
    }

    TempWriteAddress = EE_Received_MN_ADDR;
    Len = strlen((const char*)SYS_PARS.ReceivedMobileNum);

    for(i = 0; i<Len; i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress, SYS_PARS.ReceivedMobileNum[i]);
        DelayMs(5);
        TempWriteAddress++;
    }

    //To be implemnted in pic24 I2C
    //SMS_TASKS.Parameters[0] = 0x08;
    
    SMS_TASKS.Parameters[0] = 0xFF;
}


void Write_WebPeriod(void)
{
    UINT16 i,Len = 0;

    TempWriteAddress = EE_TD_ADDRR;

    //Clear EEPROM contents before doing write
    for(i = 0; i<(EE_TD_ADDRR_SIZE); i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress,0x00);
        DelayMs(5);
        TempWriteAddress++;
    }

    TempWriteAddress = EE_TD_ADDRR;
    Len = strlen((const char*)SYS_PARS.TaskDuration);

    for(i = 0; i<Len; i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress, SYS_PARS.TaskDuration[i]);
        DelayMs(5);
        TempWriteAddress++;
    }

    //To be implemnted in pic24 I2C
    //SMS_TASKS.Parameters[0] = 0x08;

    SMS_TASKS.Parameters[0] = 0xFF;
}

void Write_RestartByte(void)
{

    TempWriteAddress = EE_RESTART_ADDRR;

    WriteOnEEPROMI2C(TempWriteAddress, 0xA);
    DelayMs(5);
}



void ReadEEPROM(void)
{
     DelayMs(2000);

     GetServerAddress();
     GetAPN();
     GetMobileNumber();
     GetReceivedMobileNumber();
     GetAppId();
     GetDeviceType();
     GetRMCUId();
     GetWebPeriod();

     CheckSystemRestartByte();
}

void EraseEEPROM(void)
{
    UINT16 i;

    TempWriteAddress = EE_ERASEST_ADDR;

    for(i = 0; i<EE_ERASEEND_ADDR; i++)
    {
        WriteOnEEPROMI2C(TempWriteAddress,0x00);
        DelayMs(10);
        TempWriteAddress++;
    }

}

#endif

void GetServerAddress(void)
{
    UINT i = 0;
    TempReadAddress = EE_SERVERADDR_ADDR;

    for(i = 0; i<30; i++)
    {
        SYS_PARS.Server_Address[i] = ReadOnEEPROMI2C(TempReadAddress);
        DelayMs(10);
        TempReadAddress++;
    }

    //No server address in eeprom, Load default server address
    if((SYS_PARS.Server_Address[0] == '\0')|| (SYS_PARS.Server_Address[0] == '0')|| (SYS_PARS.Server_Address[0] == 0xFF))
    {
        strcpy((char*)SYS_PARS.Server_Address,(const char*)dServerAddress);
    }

    else
    {
        //Address found
    }
}

void GetDeviceType(void)
{
    UINT i = 0;
    
    TempReadAddress = EE_DEVICEYYPE_ADDR;

    for(i = 0; i<5; i++)
    {
        SYS_PARS.Dev_Type[i] = ReadOnEEPROMI2C(TempReadAddress);
        DelayMs(10);
        TempReadAddress++;
    }

    //No Device Type in eeprom, Load default Device Type
    if((SYS_PARS.Dev_Type[0] == '\0') || (SYS_PARS.Dev_Type[0] == 0xFF))
    {
        strcpy((char*)SYS_PARS.Dev_Type,(const char*)dDevType);
    }

    else
    {
        //Device Type  found
    }
}

void GetRMCUId(void)
{
    UINT i = 0;
    TempReadAddress = EE_RMCUID_ADDR;

    for(i = 0; i<17; i++)
    {
        SYS_PARS.RMCU_Id[i] = ReadOnEEPROMI2C(TempReadAddress);
        DelayMs(10);
        TempReadAddress++;
    }

    //No  RMCU ID in eeprom, Load default  RMCU ID
    if((SYS_PARS.RMCU_Id[0] == '\0') || (SYS_PARS.RMCU_Id[0] == 0xFF))
    {
        strcpy((char*)SYS_PARS.RMCU_Id,(const char*)dRMCUId);
    }

    else
    {
        //RMCU ID found
    }
    
}

void GetAPN(void)
{
    UINT i;
    TempReadAddress = EE_APN_ADDR;

    for(i = 0; i<25; i++)
    {
        SYS_PARS.APN[i] = ReadOnEEPROMI2C(TempReadAddress);
        DelayMs(10);
        TempReadAddress++;
    }

    //No APN in eeprom, Load default APN
    if((SYS_PARS.APN[0] == '\0')|| (SYS_PARS.APN[0] == 0xFF))
    {
        strcpy((char*)SYS_PARS.APN,(const char*)dAPN);
    }

    else
    {
        //APNfound
    }
}

void GetAppId(void)
{
    UINT i;
    TempReadAddress = EE_APPID_ADDR;

    for(i = 0; i<9; i++)
    {
        SYS_PARS.App_Id[i] = ReadOnEEPROMI2C(TempReadAddress);
        DelayMs(10);
        TempReadAddress++;
    }

    //No AppId in eeprom, Load default AppId
    if((SYS_PARS.App_Id[0] == '\0')|| (SYS_PARS.App_Id[0] == 0xFF))
    {
        strcpy((char*)SYS_PARS.App_Id,(const char*)dAppId);
    }

    else
    {
        //AppId found
    }
}

void GetWebPeriod(void)
{
    UINT i;
    char *ptr;
    TempReadAddress = EE_TD_ADDRR;

    for(i = 0; i<5; i++)
    {
        SYS_PARS.TaskDuration[i] = ReadOnEEPROMI2C(TempReadAddress);
        DelayMs(10);
        TempReadAddress++;
    }

    //No Web reporting period in eeprom, Load default  Web reporting period
    if((SYS_PARS.TaskDuration[0] == '\0')|| (SYS_PARS.MobileNum[0] == 0xFF))
    {
        strcpy((char*)SYS_PARS.TaskDuration,(const char*)dTaskDuration);
    }

    else
    {
        //Web reporting period found
    }

    strncpy((char*)tTask_Duration,(const char*)SYS_PARS.TaskDuration,2);
    Task_Duration_H = atoi((char*)tTask_Duration);

    if((ptr = strstr((char*)SYS_PARS.TaskDuration, ".")) != NULL)
    {
        ptr++; //Skip .(dot)
        tTask_Duration[0] = *ptr++;
        tTask_Duration[1] = *ptr++;
        Task_Duration_M = atoi((char*)tTask_Duration);
    }
    else
    {
        Task_Duration_M = 5;
    }

    WebPeriod = ((Task_Duration_H*60)+Task_Duration_M);
}

void GetMobileNumber(void)
{
    UINT i;
    TempReadAddress = EE_MN_ADDR;

    for(i = 0; i<10; i++)
    {
        SYS_PARS.MobileNum[i] = ReadOnEEPROMI2C(TempReadAddress);
        DelayMs(10);
        TempReadAddress++;
    }

    //No mobile number in eeprom, Load default mobile number
    if((SYS_PARS.MobileNum[0] == '\0') || (SYS_PARS.MobileNum[0] == '0')|| (SYS_PARS.MobileNum[0] == 0xFF))
    {
        strcpy((char*)SYS_PARS.MobileNum,(const char*)dMobile_Number);
        strcpy((char*)SMS_Rxd_MobileNumber,(const char*)dMobile_Number); //Required for 1st MN validation
    }

    else
    {
        //Number found
    }
}

void GetReceivedMobileNumber(void)
{
    UINT i;
    TempReadAddress = EE_Received_MN_ADDR;

    for(i = 0; i<10; i++)
    {
        SYS_PARS.ReceivedMobileNum[i] = ReadOnEEPROMI2C(TempReadAddress);
        DelayMs(10);
        TempReadAddress++;
    }

    //No mobile number in eeprom, Load default mobile number
    if((SYS_PARS.ReceivedMobileNum[0] == '\0') || (SYS_PARS.ReceivedMobileNum[0] == '0')|| (SYS_PARS.ReceivedMobileNum[0] == 0xFF))
    {
        strcpy((char*)SYS_PARS.ReceivedMobileNum,(const char*)dMobile_Number);
        strcpy((char*)SMS_Rxd_MobileNumber,(const char*)dMobile_Number); //Required for 1st MN validation
    }

    else
    {
        //Number found
    }
}

void CheckSystemRestartByte(void)
{
    BYTE Rst = 0;

   /*Read restart byte to check system is restarted by SMS*/
    Rst = ReadOnEEPROMI2C(EE_RESTART_ADDRR);
    DelayMs(5);
    
    if(Rst == 0xA)
    {
        FLAGS.SysIsRestartedBySMS = 1;
        WriteOnEEPROMI2C(EE_RESTART_ADDRR, 0x00);
        DelayMs(10);
    }
}