/* 
 * File:   libModbusUART.h
 * Author: Srivardhan
 *
 * Created on 14 September, 2013, 10:51 AM
 */

#ifndef LIBMODBUSUART_H
#define	LIBMODBUSUART_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <psystem.h>
#include <apiTick.h>
#include "apiModbus.h"

#define MODBUS_UART_BAUD_RATE   9600

#define BRG_REG_VALUE_MODBUS   (((GetPeripheralClock()/MODBUS_UART_BAUD_RATE)/4)-1)

 void ConfigModbusUART(void);
 void WriteCharToUART(unsigned char CharValue);

#ifdef	__cplusplus
}
#endif

#endif	/* LIBMODBUSUART_H */

