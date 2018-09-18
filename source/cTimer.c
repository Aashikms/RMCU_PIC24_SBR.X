
#include <xc.h>
#include <stdio.h>
#include "config.h"
#include <cTimer.h>
#include "apiTask.h"
#include <stdlib.h>
#include <apiTick.h>
#include <pSystem.h>
#include "FatFs/diskio.h"
#include "libModemUART.h"



volatile unsigned long long MdbWaitctr = 0;
volatile unsigned long long I2cTimeout = 0;
volatile unsigned long long MdbRespTimeout = 0;
volatile unsigned long long RestartCounter = 0;




void ConfigFreeRunningTimer(void)
{
    T4CON = 0x00;               //Stops any 16/32-bit Timer4 operation
    T5CON = 0x00;               //Stops any 16/32-bit Timer5 operation

    TMR4 = 0x00;
    TMR5 = 0x00;

    T4CONbits.TON   = 0;
    T4CONbits.TCKPS = 0x3;
    T4CONbits.T32   = 1;        //ENABLE 32 BIT TIMER MODE
    T4CONbits.TGATE = 0;        //Gate time accumulation is disabled
    T4CONbits.TCS   = 0;        //Internal Clock (FOSC/2)

    #ifdef EN_1MS_TIMER
    PR5 = 0x0000;
    PR4 = 156;
    #endif

    #ifdef EN_1SEC_TIMER
    PR5 = 0x0002;
    PR4 = 0x625A;
    #endif

    IPC7bits.T5IP   = 7;    //Set Priority for timer 45 (controls 32bit timer 4 and 5)
    IFS1bits.T5IF   = 0;    //Reset interrupt flag for timer 45 (controls 32bit timer 4 and 5)
    IEC1bits.T5IE   = 1;    //Enable timer 45 interrupts (controls 32bit timer 4 and 5)

    T4CONbits.TON   = 1;    //Turn Timer4 ON

    INTCON2bits.GIE = 1;
}


void __attribute__((__interrupt__,no_auto_psv)) _T5Interrupt(void)
{

    // Clear the interrupt flag
    IFS1bits.T5IF = 0;

    if(MdbRespTimeout)
    {
        --MdbRespTimeout;
        if(!MdbRespTimeout)
        {
            
        }
    }

    if(MdbWaitctr)
    {
        --MdbWaitctr;
        if(!MdbWaitctr)
        {

        }
    }

    if(I2cTimeout)
    {
        --I2cTimeout;
    }

    if(RestartCounter)
    {
        --RestartCounter;
        if(!RestartCounter)
        {
            SOFTRESET();
        }
        
    }
}
