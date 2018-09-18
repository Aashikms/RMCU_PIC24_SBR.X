#include <xc.h>
#include <adc.h>
#include "cTimer.h"
#include <libI2C.h>
#include <apiRTC.h>
#include <pSystem.h>
#include <apiDelay.h>
#include <apiTick.h>
#include <apiModem.h>
#include <apiEEPROM.h>
#include <apiSdcard.h>
#include <apiStartup.h>
#include <libModemUART.h>
#include <libModbusUART.h>



void SysInit(void)
{
    DelayMs(10);
    ConfigOSC();
    DelayMs(10);
    ConfigSYSTEM();
    ConfigTick();
    ConfigModemUART();
    DelayMs(5);
    ConfigModbusUART();
    DelayMs(5);
    ConfigModem();
    ConfigModbus();
    ConfigFreeRunningTimer();

    #ifdef RTC
    RTCI2C_Config();

    #ifdef ERASE_EEPROM
    EraseEEPROM();
    #endif

    #ifdef RAED_EEPROM
    ReadEEPROM();
    #endif
    ConfigRTC();
    #endif

    PLCINTInit();

}



void ConfigSYSTEM(void)
{
    //SPI
    TRISCbits.TRISC3 = 0;
    TRISBbits.TRISB0 = 0;

    //LEDS
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC9 = 0;
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB14 = 0;

    //RS485 Direction
    TRISBbits.TRISB9 = 0;
    
    //MODEM_PWRKEY
    TRISBbits.TRISB11 = 0;

    //GSM_Enable
    TRISAbits.TRISA7 = 0;

    //Enable Global interrupt
    INTCON2bits.GIE = 1;

    // Make all the ANx pins as digital pins
    ANSELC = 0;
}

void PLCINTInit(void)
{

    TRISBbits.TRISB7 = 1;
    /* Interrupt nesting enabled here */
    INTCON1bits.NSTDIS = 0;

    INTCON2bits.INT0EP = 1;     //External Interrupt 0 on negative edge

    /* Set INT0(External interrupt) interrupt priority to 5 (level 7 is highest) */
    IPC0bits.INT0IP = 7;
    /* Reset external interrupt interrupt flag */
    IFS0bits.INT0IF = 0;
    /* Enable External intterupt interrupt */
    IEC0bits.INT0IE = 1;

    INTCON2bits.GIE = 1;
    
}

void ConfigOSC(void)
{
    // Configure oscillator to operate the device at 120 MHz
    // Fosc = Fin * M/(N1 * N2), FCY = Fosc/2
    // Fosc = 8M*40(2*2) = 80 MHz for 8M input clock

    CLKDIVbits.PLLPRE = 0;      // N2 = 2
    
    #ifdef SYS_CLK_80_MHZ
    PLLFBD = 38;                // M = 40
    CLKDIVbits.PLLPOST = 0;     // N1 = 2
    #endif

    CLKDIVbits.DOZE = 0;
   
    while(OSCCONbits.LOCK != 1);    //pll lock

}


#ifdef TEST_ADC
void InitAdc1(void)
{
    /* Set port configuration */

    ANSELAbits.ANSA0 = 1;           // Ensure AN0/RA0 is analog

    ANSELCbits.ANSC2 = 1;           // Ensure AN8/RC2 is analog
    
    AD1CON1bits.FORM   = 3;		// Data Output Format: Signed Fraction (Q15 format)
    AD1CON1bits.SSRC   = 7;		// Internal counter ends sampling and starts conversion (auto-convert)
    AD1CON1bits.ASAM   = 1;		// ADC Sample Control: Sampling begins immediately after conversion
    AD1CON1bits.AD12B  = 0;		// 10-bit ADC operation

    AD1CON2bits.CSCNA = 1;		// Scan Input Selections for CH0+ during Sample A bit
    AD1CON2bits.CHPS  = 0;		// Converts CH0

    AD1CON3bits.ADRC = 0;		// ADC Clock is derived from Systems Clock
    AD1CON3bits.ADCS = 63;		// ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*64 = 1.6us (625Khz)
                                        // ADC Conversion Time for 10-bit Tc=12*Tab = 19.2us

    AD1CON2bits.SMPI    = (NUM_CHS2SCAN-1);	// 2 ADC Channel is scanned

    //AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
    AD1CSSH = 0x0000;
    AD1CSSLbits.CSS0=1;			// Enable AN0 for channel scan
    AD1CSSLbits.CSS8=1;			// Enable AN8 for channel scan

    IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
    IEC0bits.AD1IE = 1;			// Enable A/D interrupt
    AD1CON1bits.ADON = 1;		// Turn on the A/D converter

    Delay10us(20);
}

/*=============================================================================
ADC INTERRUPT SERVICE ROUTINE
=============================================================================*/
int  ain0Buff[SAMP_BUFF_SIZE];
int  ain8Buff[SAMP_BUFF_SIZE];


int  scanCounter=0;
int  sampleCounter=0;

void __attribute__((__interrupt__, no_auto_psv)) _AD1Interrupt(void)
{

    switch (scanCounter)
    {
        case 0:
                ain0Buff[sampleCounter]=ADC1BUF0;
                break;

        case 1:
                ain8Buff[sampleCounter]=ADC1BUF0;
                break;

        default:
                break;

    }

    scanCounter++;
    if(scanCounter==NUM_CHS2SCAN)
    {
            scanCounter=0;
            sampleCounter++;
    }

    if(sampleCounter==SAMP_BUFF_SIZE)
            sampleCounter=0;

    IFS0bits.AD1IF = 0;		// Clear the ADC1 Interrupt Flag

}
#endif