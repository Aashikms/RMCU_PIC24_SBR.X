#include <xc.h>
#include <i2c.h>
#include <stdio.h>
#include <stdlib.h>
#include "libI2C.h"
#include "cTimer.h"
#include "apiTick.h"
#include <apiRTC.h>
#include <pSystem.h>
#include <apiDelay.h>


void RTCI2C_Config(void)
{

    I2C1CON = 0x0000;
    I2C1STAT = 0x00;
    I2C1ADD = 0;
    I2C1MSK = 0;
    I2C1CONbits.I2CEN = 1; //Enable the I2C Module

    I2C1BRG = I2C_BRG_VALUE;
}

/***********************************************************************************************************************
				BASIC I2C CODES FOR WRITE AND READ CYCLE
************************************************************************************************************************/

/*******************************************************************************************************************************************
********************************************************************************************************************************************/
void WriteOnI2C(UINT8 RegAddr ,UINT8 Value)
{
    DelayMs(1);
    
    StartI2C1();
    IdleI2C1();
    
    MasterWriteI2C1(RTC_WRITE_ADDR);
    while( I2C1STATbits.ACKSTAT );
    IdleI2C1();

    MasterWriteI2C1(RegAddr);
    while( I2C1STATbits.ACKSTAT );
    IdleI2C1();
    
    MasterWriteI2C1(Value);
    while( I2C1STATbits.ACKSTAT );
    IdleI2C1();
    
    StopI2C1();	
    IdleI2C1();
}

void WriteOnEEPROMI2C(UINT8 RegAddr ,UINT8 Value)
{
    DelayMs(1);

    StartI2C1();
    IdleI2C1();

    MasterWriteI2C1(EEPROM_WRITE_ADDR);
    while( I2C1STATbits.ACKSTAT );
    IdleI2C1();

    MasterWriteI2C1(RegAddr);
    while( I2C1STATbits.ACKSTAT );
    IdleI2C1();

    MasterWriteI2C1(Value);
    while( I2C1STATbits.ACKSTAT );
    IdleI2C1();

    StopI2C1();
    IdleI2C1();
}


/*******************************************************************************************************************************************
********************************************************************************************************************************************/
unsigned char ReadOnI2C(UINT8 RegAddress)
{

    unsigned char I2C_Result = 0;
    
    /*start*/
    StartI2C1();
    while(I2C1CONbits.SEN);
    IdleI2C1();

    MasterWriteI2C1(RTC_WRITE_ADDR);
    while( I2C1STATbits.ACKSTAT );
    IdleI2C1();

    MasterWriteI2C1(RegAddress);
    while( I2C1STATbits.ACKSTAT );
    IdleI2C1();

    /*repeated start*/
    RestartI2C1();
    while(I2C1CONbits.RSEN);
    IdleI2C1();
    
    MasterWriteI2C1(RTC_READ_ADDR);
    while( I2C1STATbits.TRSTAT );

    /*read first byte data*/
    I2C_Result = MasterReadI2C1();
    while(I2C1CONbits.RCEN);
    NotAckI2C1();
    IdleI2C1();

    /*send stop*/
    StopI2C1();
    while(I2C1CONbits.PEN);
    IdleI2C1();

    return(I2C_Result);
   
}


unsigned char ReadOnEEPROMI2C(UINT8 RegAddress)
{

    unsigned char I2C_Result = 0;

    /*start*/
    StartI2C1();
    while(I2C1CONbits.SEN);
    IdleI2C1();

    MasterWriteI2C1(EEPROM_WRITE_ADDR);
    while( I2C1STATbits.ACKSTAT );
    IdleI2C1();

    MasterWriteI2C1(RegAddress);
    while( I2C1STATbits.ACKSTAT );
    IdleI2C1();

    /*repeated start*/
    RestartI2C1();
    while(I2C1CONbits.RSEN);
    IdleI2C1();

    MasterWriteI2C1(EEPROM_READ_ADDR);
    while( I2C1STATbits.TRSTAT );

    /*read first byte data*/
    I2C_Result = MasterReadI2C1();
    while(I2C1CONbits.RCEN);
    NotAckI2C1();
    IdleI2C1();

    /*send stop*/
    StopI2C1();
    while(I2C1CONbits.PEN);
    IdleI2C1();

    return(I2C_Result);

}
