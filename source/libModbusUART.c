#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include <apiTick.h>
#include <psystem.h>
#include <libModbusUART.h>



extern _FLAGS FLAGS;

CHAR8 Dbyte = 0;
const BYTE MASK = 0x7F;
volatile int mBufCount = 0;

unsigned char mdbUARTRxBuffer[512];


void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt(void)
{

    if(IFS1bits.U2RXIF)
    {
        Dbyte = U2RXREG;
        Dbyte = Dbyte & MASK;
        mdbUARTRxBuffer[mBufCount] = Dbyte;

        if(mdbUARTRxBuffer[mBufCount] == '\r')
        {
            FLAGS.UART_RxDone = 1;
        }
        mBufCount++;
        IFS1bits.U2RXIF = 0;
    }
    
    if ((U2STAbits.PERR | U2STAbits.FERR | U2STAbits.OERR))
    {
        U2STAbits.FERR = 0;
        U2STAbits.PERR = 0;
        U2STAbits.OERR = 0;
    }
}

void WriteCharToUART(unsigned char CharValue)
{
    while (U2STAbits.UTXBF);
    U2TXREG = (0x7F & CharValue);
    while(!U2STAbits.TRMT);
}


void ConfigModbusUART(void)
{
    UINT64 baud;

    TRISBbits.TRISB15 = 1;              // PIN RB15 as a Rx
    TRISBbits.TRISB4 = 0; 		//PIN RB4 as a Tx

    //Remapping the pins RP36 & RPI47 as
    RPOR1   = 0x0003;           //RP36       //MC_TX
    RPINR19 = 0x002F;           //RPI47      //MC_RX

   baud = BRG_REG_VALUE_MODBUS;
    U2BRG  = baud;                      //9600
    //U2MODE = 0x0008; 			//turn on module, BRGH = 1 (high speed mode) 9600,8,none,1
    U2MODE = 0x000A; 			//turn on module, BRGH = 1 (high speed mode) and 9600,8,even,1
    U2STA  = 0x0400; 			//8400	//set interrupts

    IFS1bits.U2TXIF = 0;
    IFS1bits.U2RXIF = 0;

    U2STAbits.UTXISEL0 = 0; // Interrupt after one TX character is transmitted
    U2STAbits.UTXISEL1 = 0;

    U2MODEbits.UARTEN = 1; // Enable UART
    U2STAbits.UTXEN = 1;

    IFS1bits.U2TXIF = 0;
    IFS1bits.U2RXIF = 0;

    _U2RXIE = 1;
    _U2RXIP = 2;
}