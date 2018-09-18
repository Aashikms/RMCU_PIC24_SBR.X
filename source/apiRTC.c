
#include <time.h>
#include <ports.h>
#include <stdio.h>
#include <libI2C.h>
#include <apiRTC.h>
#include "apiTask.h"
#include <pSystem.h>
#include <apiDelay.h>
#include "apiEEPROM.h"




#if defined RTC

char RtcFrame[20];
char TimeFrame[10];

TIMESTAMP RTS, WTS;
_DATETIME Date_Time;

unsigned char RHour, RMinute, RSecond, RDate, RDay, RMonth, RYear;

/*******************************************************************************************************************************************
********************************************************************************************************************************************
												Set & Read RTC Time
********************************************************************************************************************************************
********************************************************************************************************************************************/

void SetRtcTime(unsigned char Seconds,unsigned char Minutes,unsigned char Hours)
{
    WTS.Second = (Seconds);
    WTS.Minute = Minutes;
    WTS.Hour   = Hours;
    
    WriteSeconds();
    DelayMs(2);
    WriteMinutes();
    DelayMs(2);
    WriteHours();
    DelayMs(2);
}

void SetRtcDate(unsigned char Date,unsigned char Month,unsigned char Year)
{
    WTS.Date  = Date;
    WTS.Month = Month;
    WTS.Year  = Year;

    WriteDate();
    DelayMs(2);
    WriteDay();
    DelayMs(2);
    WriteMonth();
    DelayMs(2);
    WriteYear();
    DelayMs(2);
}

void WriteSeconds(void)
{
    WriteOnI2C(0x00,WTS.Second);
}

void WriteMinutes(void)
{
    WriteOnI2C(0x01,WTS.Minute);
}

void WriteHours(void)
{
    WriteOnI2C(0x02,WTS.Hour);
}

void WriteDay(void)
{
    WriteOnI2C(0x03,0x09);
}

void WriteDate(void)
{
    WriteOnI2C(0x04,WTS.Date);
}

void WriteMonth(void)
{
    WriteOnI2C(0x05,WTS.Month);
}

void WriteYear(void)
{
    WriteOnI2C(0x06,WTS.Year);
}

/*******************************************************************************************************************************************
********************************************************************************************************************************************/

void ReadRtcTime(unsigned char Seconds,unsigned char Minutes,unsigned char Hours)
{
  
    ReadSeconds();
    ReadMinutes();
    ReadHours();
    RTS.Second  =  ((RSecond & 0x70) >> 4)*10  + (RSecond & 0x0F);
    RTS.Minute  =  ((RMinute & 0xF0) >> 4)*10  + (RMinute & 0x0F);
    RTS.Hour    =  ((RHour & 0xF0)   >> 4)*10  + (RHour & 0x0F);
    Seconds = RTS.Second;
    Minutes = RTS.Minute;
    Hours   = RTS.Hour;
}

void ReadRtcDate(unsigned char Date,unsigned char Month,unsigned char Year)
{
    ReadDay();
    ReadDate();
    ReadMonth();
    ReadYear();
    RTS.Year    =  ((RYear & 0xF0)   >> 4)*10  + (RYear & 0x0F);
    RTS.Date    =  ((RDate & 0x30)   >> 4)*10  + (RDate & 0x0F);
    RTS.Month   =  ((RMonth & 0x10)  >> 4)*10  + (RMonth & 0x0F);
    Date  = RTS.Date;
    Month = RTS.Month;
    Year  = RTS.Year;
}

void ReadSeconds(void)
{
    RSecond = ReadOnI2C(0x00);
    DelayMs(2);
    if((RSecond&0x80) == 0)
    {
        WriteSeconds();
    }
    DelayMs(2);
}

void ReadMinutes(void)
{
    RMinute = ReadOnI2C(0x01);
    DelayMs(2);
}

void ReadHours(void)
{
    RHour = ReadOnI2C(0x02);
    DelayMs(2);
}

void ReadDay(void)
{
    RDay = ReadOnI2C(0x03);
    if((RDay&0x08) == 0)
    {
        WriteDay();
    }
    DelayMs(2);
}

void ReadDate(void)
{
    RDate = ReadOnI2C(0x04);
    DelayMs(2);
}

void ReadMonth(void)
{
    RMonth = ReadOnI2C(0x05);
    DelayMs(2);
}

void ReadYear(void)
{
    RYear = ReadOnI2C(0x06);
    DelayMs(2);
}

unsigned char ReadSecond(void)    
{
    unsigned char sec;

    sec = ReadOnI2C(0x00);
    return sec;
}

void ReadRTC(_DATETIME *ptrDateTime)
{
    int temp,RTCYear = 0;

    temp = ReadOnI2C(ADDR_SEC);
    (*ptrDateTime).Seconds = 0;
    (*ptrDateTime).Seconds = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    DelayMs(2);

    temp = ReadOnI2C(ADDR_MIN);
    (*ptrDateTime).Mins = 0;
    (*ptrDateTime).Mins = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    DelayMs(2);

    temp = ReadOnI2C(ADDR_HOUR);
    (*ptrDateTime).Hour = 0;
    (*ptrDateTime).Hour = (((temp >> 4) & 0x03) * 10) + (temp & 0x0F);
    (*ptrDateTime).AM_PM = 0;

    if((*ptrDateTime).Hour >= 12)
            (*ptrDateTime).AM_PM = 1;

    DelayMs(2);

    temp = ReadOnI2C(ADDR_DATE);
    (*ptrDateTime).Date = 0;
    (*ptrDateTime).Date = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    DelayMs(2);

    temp = ReadOnI2C(ADDR_MNTH);
    (*ptrDateTime).Month = 0;
    (*ptrDateTime).Month = (((temp >> 4) & 0x01) * 10) + (temp & 0x0F);

    DelayMs(2);

    temp = ReadOnI2C(ADDR_YEAR);
    (*ptrDateTime).Year.byte.LB = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    RTCYear = (*ptrDateTime).Year.byte.LB;
    temp = (RTCYear - CURRENT_YEAR_LB);
    (*ptrDateTime).Year.Val = CURRENT_YEAR;
    (*ptrDateTime).Year.Val += temp;
}

void ReadLastPowerUpTime(_DATETIME *ptrDateTime)
{
    int temp;

    temp = ReadOnI2C(ADDR_SAVtoVDD_MIN);
    (*ptrDateTime).Mins = 0;
    (*ptrDateTime).Mins = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    DelayMs(2);

    temp = ReadOnI2C(ADDR_SAVtoVDD_HR);
    (*ptrDateTime).Hour = 0;
    (*ptrDateTime).Hour = (((temp >> 4) & 0x03) * 10) + (temp & 0x0F);
    (*ptrDateTime).AM_PM = 0;

    if((*ptrDateTime).Hour >= 12)
            (*ptrDateTime).AM_PM = 1;

    DelayMs(2);

    temp = ReadOnI2C(ADDR_SAVtoVDD_DAT);
    (*ptrDateTime).Date = 0;
    (*ptrDateTime).Date = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    DelayMs(2);

    temp = ReadOnI2C(ADDR_SAVtoVDD_MTH);
    (*ptrDateTime).Month = 0;
    (*ptrDateTime).Month = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    DelayMs(2);
}


void VBATInit(void)
{
    UINT8 temp;

    temp = ReadOnI2C(ADDR_STAT);
    DelayMs(2);

    //Set 3rd bit to enable backup battery mode
    temp = (temp | 0x08);
    
    WriteOnI2C(ADDR_STAT, temp);
    DelayMs(2);
}

void RTCINTInit(void)
{
    ANSELBbits.ANSB1 = 0;
    TRISBbits.TRISB1 = 1;
    CNENBbits.CNIEB1 = 1;
    
    IEC1bits.CNIE = 1;
    IFS1bits.CNIF = 0;
    INTCON2bits.GIE = 1;

}


void ConfigRTC(void)
{
    #ifdef SET_RTC_TIME
    //Code has to be clean and built to set time.
    RTCInit();
    #endif

    VBATInit();
    DelayMs(2);
    RTC_Post_Config();
        
    #ifdef RTC_ALM
    RTCINTInit();
    DelayMs(2);
    SetALMTime();
    #endif
}

void RTCInit(void)
{
    char day;
    char month;
    char year;
    char hour;
    char min;
    char sec;

    sec =  (((__TIME__[6]) & 0x0F) << 4) | ((__TIME__[7]) & 0x0F);
    min =  (((__TIME__[3]) & 0x0F) << 4) | ((__TIME__[4]) & 0x0F);
    hour = (((__TIME__[0]) & 0x0F) << 4) | ((__TIME__[1]) & 0x0F);
    day =  (((__DATE__[4]) & 0x0F) << 4) | ((__DATE__[5]) & 0x0F);
    year = (((__DATE__[9]) & 0x0F) << 4) | ((__DATE__[10]) & 0x0F);

    switch(__DATE__[0])
    {
        case 'J':
            //January, June, or July
            switch(__DATE__[1])
            {
                case 'a':
                    //January
                    month = 0x01;
                    break;
                case 'u':
                    switch(__DATE__[2])
                    {
                        case 'n':
                            //June
                            month = 0x06;
                            break;
                        case 'l':
                            //July
                            month = 0x07;
                            break;
                    }
                    break;
            }
            break;
        case 'F':
            month = 0x02;
            break;
        case 'M':
            //March,May
            switch(__DATE__[2])
            {
                case 'r':
                    //March
                    month = 0x03;
                    break;
                case 'y':
                    //May
                    month = 0x05;
                    break;
            }
            break;
        case 'A':
            //April, August
            switch(__DATE__[1])
            {
                case 'p':
                    //April
                    month = 0x04;
                    break;
                case 'u':
                    //August
                    month = 0x08;
                    break;
            }
            break;
        case 'S':
            month = 0x09;
            break;
        case 'O':
            month = 0x10;
            break;
        case 'N':
            month = 0x11;
            break;
        case 'D':
            month = 0x12;
            break;
    }

    RTC_Pre_Config();
    SetRtcTime(sec, min, hour);
    SetRtcDate(day, month, year);
    RTC_Post_Config();

}

void RTC_Pre_Config(void)
{
     char temp;

    temp = ReadOnI2C(ADDR_SEC);
    DelayMs(2);

    //clear the 7th bit to stop the clock
    temp = (temp & 0x7F);

    //Stop clock
    WriteOnI2C(ADDR_SEC,temp);
    DelayMs(2);

    //Wait for OSCRUN bit to clear
    while(ReadOnI2C(ADDR_STAT) & 0x20);
    DelayMs(2);
}

void RTC_Post_Config(void)
{
    char temp;

    temp = ReadOnI2C(ADDR_SEC);
    DelayMs(2);

    //set the 7th bit to start the clock
    temp = (temp | 0x80);

    //Start clock
    WriteOnI2C(ADDR_SEC,temp);
    DelayMs(2);
}

unsigned int uint2bcd(unsigned int ival)
{
    return ((ival / 10) << 4) | (ival % 10);
}

#ifdef RTC_ALM
void SetALMTime(void)
{
    unsigned int temp;
    unsigned int tempH;

    temp = ReadOnI2C(ADDR_ALM0CTL);
    //Clear Alarm Flag
    temp = temp & 0xF0;
    DelayMs(2);
    WriteOnI2C(ADDR_ALM0CTL, uint2bcd(temp));
    DelayMs(2);

    ReadRTC(&Date_Time);
    DelayMs(2);
    WriteOnI2C(ADDR_ALM0MTH, uint2bcd(Date_Time.Month));
    DelayMs(2);
    WriteOnI2C(ADDR_ALM0DAT, uint2bcd(Date_Time.Date));
    DelayMs(2);

    temp  = Date_Time.Mins + Task_Duration_M;
    tempH = Date_Time.Hour + Task_Duration_H;

    WriteOnI2C(ADDR_ALM0MIN, uint2bcd(temp));
    DelayMs(2);

    if(tempH == 23)
    {
        WriteOnI2C(ADDR_ALM0HR, 0);
    }
    else
    {
        WriteOnI2C(ADDR_ALM0HR, uint2bcd(tempH));
    }

    DelayMs(2);

    WriteOnI2C(ADDR_ALM0SEC, uint2bcd(Date_Time.Seconds));
    DelayMs(2);

    temp = uint2bcd(ALMx_POL|ALMxC_MIN);
    WriteOnI2C(ADDR_ALM0CTL,temp);
    DelayMs(2);
    
    WriteOnI2C(ADDR_CTRL, uint2bcd(ALM_0));
    DelayMs(2);
}


void DisableRTCAlarm(void)
{
    UINT8 temp = 0;

    //Clear Alarm Interrupt flag
    temp = ReadOnI2C(ADDR_ALM0CTL);
    temp = temp & 0xF7;
    DelayMs(2);
    WriteOnI2C(ADDR_ALM0CTL,uint2bcd(temp));
    DelayMs(2);

    WriteOnI2C(ADDR_CTRL, uint2bcd(ALM_NO));
    DelayMs(2);
}

#endif

void GetTimeString(char* pcString)
{
    char tBuf[2];

    ReadRTC(&Date_Time);

    sprintf(tBuf, "%d", Date_Time.Hour);

    if(tBuf[1] == 0)
    {
        TimeFrame[1] = tBuf[0];
        TimeFrame[0] = 0x30;
    }
    else
    {
        TimeFrame[0] = tBuf[0];
        TimeFrame[1] = tBuf[1];
    }

    TimeFrame[2] = ':';

    sprintf(tBuf, "%d", Date_Time.Mins);

    if(tBuf[1] == 0)
    {
        TimeFrame[4] = tBuf[0];
        TimeFrame[3] = 0x30;
    }
    else
    {
        TimeFrame[3] = tBuf[0];
        TimeFrame[4] = tBuf[1];
    }

    TimeFrame[5] = ':';

    sprintf(tBuf, "%d", Date_Time.Seconds);

    if(tBuf[1] == 0)
    {
        TimeFrame[7] = tBuf[0];
        TimeFrame[6] = 0x30;
    }
    else
    {
        TimeFrame[6] = tBuf[0];
        TimeFrame[7] = tBuf[1];
    }

    TimeFrame[8] = '\0';

    strcat((char*)pcString, TimeFrame);
}

#ifdef RTC_ALM
 void __attribute__ (( interrupt, shadow, no_auto_psv )) _CNInterrupt(void)
{
     unsigned int temp;

    //Clear Alarm Interrupt flag
    temp = ReadOnI2C(ADDR_ALM0CTL);
    DelayMs(2);
    temp = (temp & 0xF7);
    
    WriteOnI2C(ADDR_ALM0CTL,uint2bcd(temp));
    DelayMs(2);

    SetALMTime();
    //Read CLT Sysytem parameters
//    InsertSYSTask(CLT_SYS_READ_TASK);
    IFS1bits.CNIF = 0;

}
#endif
#endif
