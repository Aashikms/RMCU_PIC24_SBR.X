/*
 * File:   psystem.h
 * Author: Phoenix
 *
 * Created on July 22, 2013, 7:26 PM
 */

#ifndef PSYSTEM_H
#define	PSYSTEM_H

#ifdef	__cplusplus
extern "C" {
#endif


#if defined(__dsPIC33E__)
#include "p33exxxx.h"
#elif defined(__PIC24E__)
//#include <xc.h>
#include <ports.h>
#include <uart.h>
#include <timer.h>
#include "p24exxxx.h"

#endif


#include <config.h>
#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <GenericTypeDefs.h>
#include <HardwareProfile.h>
    

#define RTC
//#define RTC_ALM
#define RAED_EEPROM
//#define SET_RTC_TIME
//#define ERASE_EEPROM
#define EN_NETWORK_TIME

#define TF_SERVER
#define DOMAIN_NAME
#define SMS_NUMBER_VALIDATION
#define HANDLE_CASE_SENSITIVE_SMS

//#define EN_ECHO_OFF

#define CLT_SYS_PARS_TEST
#define GET_TIMESTAMP_FROM_PLC
    
#ifdef DOMAIN_NAME
#define NON_TRANSPERANT_MODE
#endif


#define SYS_CLK_80_MHZ

#define TASK_DURATION           10

#define SERVER_ACK_LENGTH       500

#define MODEM_FAIL_COUNT        5
#define MODEM_NO_CSQ_COUNT      5
#define MODBUS_RETRY_COUNT      3


#define I2C_TIMEOUT_VALUE       1000
#define MDB_RESPONSE_TIMEOUT    3000



typedef enum
{
    SYS_NO_TASK = 0,

    CLT_SYS_READ_TASK,
    EM_PARS_READ_TASK,
    ALARMS_READ_TASK,

   /* SET_TIMER1_TASK,
    SET_TIMER2_TASK,
    SET_TIMER3_TASK,
    SET_TIMER4_TASK,*/
    SET_TIMERS_TASK,
    /*SET_P1CURRENT_TASK,
    SET_P2CURRENT_TASK,
    SET_P3CURRENT_TASK,*/
    
    RESTART_PLC_TASK,
    GET_PLC_SETTINGS_TASK,
            
    DEV_INFO_UPLOAD_TASK,
    SEND_RESTART_ACK_TASK,
    CLT_SYS_UPLOAD_TASK,
    FRAME_BYTE_STREAM_TASK,
    PLC_RTC_TASK,

    SYSTEM_SLEEP_TASK,
    // Below TASKs are added by Naveen as per change request        
    
    SET_SBR_MAINTENANCE_TASK,
            
}_SYS_TASKS;


typedef struct
{
    _SYS_TASKS SYS_TASK_STATE;
    char Parameters[12];
}_SYSTEM_TASKS;

extern _SYSTEM_TASKS SYSTEM_TASK;

#ifdef	__cplusplus
}
#endif

#endif	/* PSYSTEM_H */

