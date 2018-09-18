/* 
 * File:   config.h
 * Author: Admin
 *
 * Created on May 26, 2015, 6:17 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#include "pSystem.h"
#include "GenericTypeDefs.h"


#ifdef	__cplusplus
extern "C" {
#endif

//#define SoftReset() (RCONbits.SWR = 1)
    /*********************************************************************
* Macro: #define	GetSystemClock()
*
* Overview: This macro returns the system clock frequency in Hertz.
*			* value is 8 MHz x 4 PLL for PIC24
*			* value is 8 MHz/2 x 18 PLL for PIC32
*
********************************************************************/
#if defined(__PIC24F__) || defined(__PIC24E__)


//    #define GetSystemClock()    (160000000ul)
//    #define GetSystemClock()    (120000000ul)
    #define GetSystemClock()    (80000000ul)


#elif defined(__PIC32MX__)
    #define GetSystemClock()    (80000000ul)
#elif defined(__dsPIC33F__) || defined(__PIC24H__)
    #define GetSystemClock()    (80000000ul)
#endif

/*********************************************************************
* Macro: #define	GetPeripheralClock()
*
* Overview: This macro returns the peripheral clock frequency
*			used in Hertz.
*			* value for PIC24 is <PRE>(GetSystemClock()/2) </PRE>
*			* value for PIC32 is <PRE>(GetSystemClock()/(1<<OSCCONbits.PBDIV)) </PRE>
*
********************************************************************/
#if defined(__PIC24F__) || defined(__PIC24H__) || defined(__dsPIC33F__) || defined(__PIC24E__)
    #define GetPeripheralClock()    (GetSystemClock() / 2)
#elif defined(__PIC32MX__)
    #define GetPeripheralClock()    (GetSystemClock()/1)    //(GetSystemClock() / (1 << OSCCONbits.PBDIV))
#endif

/*********************************************************************
* Macro: #define	GetInstructionClock()
*
* Overview: This macro returns instruction clock frequency
*			used in Hertz.
*			* value for PIC24 is <PRE>(GetSystemClock()/2) </PRE>
*			* value for PIC32 is <PRE>(GetSystemClock()) </PRE>
*
********************************************************************/
#if defined(__PIC24F__) || defined(__PIC24H__) || defined(__dsPIC33F__) || defined(__PIC24E__)
    #define GetInstructionClock()   (GetSystemClock() / 2)
#elif defined(__PIC32MX__)
    #define GetInstructionClock()   (GetSystemClock())
#endif

#if defined (USE_MULTIBYTECHAR)
    #define XCHAR   unsigned short
    #define UXCHAR  unsigned short
#elif defined (USE_UNSIGNED_XCHAR)
    #define XCHAR   unsigned char
    #define UXCHAR  unsigned char
#else
    #define XCHAR   char
    #define UXCHAR  unsigned char
#endif

#define ASCII_BASE_VAL  0x30
    
#define SOFTRESET() asm("RESET");

extern char _DelayCnt;

//----------------------------------------------------------------------
#define uMODEM_TX_BUFFER_SIZE       1200
#define uMODEM_RX_BUFFER_SIZE       1200

#define uMSG_RX_BUFFER_SIZE         512
#define RESP_BUFFER_SIZE            2048

#define GPRS_TX_BUFFER_SIZE         1024
#define SMS_BUFF_SIZE               MAX_CHARS_IN_SMS

#define DEVID_BYTES_BEFORE_SIZE     6
#define CLT_BYTES_BFORE_SIZE        13
#define CLT_BYTES_AFTER_SIZE        13
#define RMC_CLT_SUM_DATA_LENGTH     36
#define RMC_CLT_STAT_DATA_LENGTH    46
#define RMC_EM_DATA_LENGTH          28

#define pMSG_RX_FIFO_SIZE           8
#define pMSG_RX_FIFO_INDEX          pMSG_RX_FIFO_SIZE - 1
#define pMSG_RX_WIDTH_SIZE          200

#define MSG_INDEX_BUFFER_SIZE       10

//To avoid blocking
#define MAX_CHARS_IN_SMS            300

#define MOBILE_NO_SIZE              20

#define GSM_ON_OFF              (LATAbits.LATA7)
#define GSM_STATUS              (LATBbits.LATB13)
#define GSM_POWERKEY            (LATBbits.LATB11)


#define TASK_FIFO_SIZE               50


#define ON              1
#define OFF             0
#define PASS            0
#define FAIL            1
#define ASCII_BASE_VAL  0x30

#define CURRENT_YEAR_LB 0
#define CURRENT_YEAR    2000

#define SERVER_RESP_TIMEOUT_COUNT        3


#define PASS    0
#define FAIL    1

#define GOOD    0
#define BAD     1

#define     OFF         0
#define     ON          1

#define     DISABLE     0
#define     ENABLE      1

typedef struct
{

    unsigned int Seconds;
    unsigned int Mins;
    unsigned int Hour;
    unsigned int Date;
    unsigned int Month;
    UINT16_VAL   Year;
    unsigned int AM_PM;
    unsigned int Week_Day;
}_DATETIME;

extern  _DATETIME TIME_STAMP;
extern _DATETIME Date_Time;
extern _DATETIME Prev_Date_Time;

typedef struct
{
    unsigned MBResponse:1;
    unsigned UART_RxDone:1;
    unsigned SMSData:1;
    unsigned SysIsRestartedBySMS:1;
    unsigned MobileNumberChanged:1;
    unsigned GetPLCSettingsSMScmd:1;
    unsigned Server_Status_Success:1;
    unsigned ConnectionTimeOut:1;
    unsigned GetServerTime:1;
    unsigned DefaultMobileNo:1;

}_FLAGS;

 extern _FLAGS FLAGS;

#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

