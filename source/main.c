/* 
 * File:   main.c
 * Author: Admin
 *
 * Created on June 30, 2015, 2:23 PM
 */
#include <xc.h>
#include "apiRTC.h"
#include <stdio.h>
#include <stdlib.h>
#include <apiTick.h>
#include <apiTask.h>
#include <pSystem.h>
#include <apiModem.h>
#include <apiModbus.h>
#include <apiEEPROM.h>
#include <apiStartup.h>
#include <apiSDcard.h>


// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = OFF            // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)


// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Mode Select bits (XT Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECME           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Source Selection (Primary Oscillator with PLL module (XT + PLL, HS + PLL, EC + PLL))
#pragma config IESO = ON               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)

#ifdef __DEBUG
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)
#pragma config FWDTEN = ON             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)
#else
#pragma config GCP = ON                // General Segment Code-Protect bit (General Segment Code protect is Enabled)
#pragma config FWDTEN = ON             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)
#endif

int main(int argc, char** argv)
{
    static DWORD Blink_TimeReg = 0,Td = 0;

    SysInit();

    while(1)
    {
        ClrWdt();

        if(TickGet() - Blink_TimeReg >= TICK_SECOND/10ul)
        {
            Blink_TimeReg = TickGet();
            BEAT_LED ^= 1;
        }

        if(TickGet() - Td >= (TICK_MINUTE*WebPeriod))
        {
            Td = TickGet();
            InsertSYSTask(CLT_SYS_READ_TASK);
        }

        Application_Tasks();

        ModbusStack();
        Modem_Stack();

        ProcessModemResponse();
        
    }


    return (EXIT_SUCCESS);
}

