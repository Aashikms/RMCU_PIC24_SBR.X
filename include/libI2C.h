/* 
 * File:   libI2C.h
 * Author: Admin
 *
 * Created on October 10, 2014, 11:07 AM
 */

#ifndef LIBI2C_H
#define	LIBI2C_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "psystem.h"

#define I2C_CLK 100000  //100Khz
#define DELAY   (0.00000012)    //120ns
    
#define  I2C_BRG_FORMULA   ((((1/I2C_CLK)-DELAY)*GetPeripheralClock())-2)

#define  I2C_BRG_VALUE   (393)

#define RTC_WRITE_ADDR 		0xDE
#define RTC_READ_ADDR 		0xDF

#define  EEPROM_WRITE_ADDR      0xAE
#define  EEPROM_READ_ADDR       0xAF

void RTCI2C_Config(void);
void GetTimeString(char* pcString);
unsigned char ReadOnI2C(UINT8 RegAddress);
void WriteOnEEPROMI2C(UINT8 RegAddr ,UINT8 Value);
unsigned char ReadOnEEPROMI2C(UINT8 RegAddress);
void WriteOnI2C(UINT8 RegAddr , UINT8 Value);

#ifdef	__cplusplus
}
#endif

#endif	/* LIBI2C_H */

