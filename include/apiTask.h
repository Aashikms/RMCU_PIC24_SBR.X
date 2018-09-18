/*
 * File:   apiTask.h
 * Author: Admin
 *
 * Created on October 11, 2014, 2:45 PM
 */
//
#ifndef APITASK_H
#define	APITASK_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "pSystem.h"
#include "libModemUART.h"


typedef enum
{
    APP_INIT = 0,
    APP_INIT_MODEM,
    APP_STANDBY,
    APP_SYS_IDLE,
    APP_READY_FOR_SLEEP,
    APP_SLEEP,

    APP_GET_ALARMS,
    APP_GET_EM_PARS,
    APP_GET_CLTSYS_PARS,
    APP_GET_PLC_SETTINGS,

    APP_SET_TIMERS,
    /*APP_SET_TIMER1,
    APP_SET_TIMER2,
    APP_SET_TIMER3,
    APP_SET_TIMER4,
    APP_SET_P1CURRENT,
    APP_SET_P2CURRENT,
    APP_SET_P3CURRENT,*/
    
    APP_RESTART_PLC,
    
    APP_SET_SBR_MAINTENANCE,
    APP_FRAME_BYTE_STREAM,
    APP_PLC_RTC_TASK,
    
    APP_PUT_DEV_INFO,
    APP_PUT_CLT_SYS_PARS,
    APP_PUT_RST_SMS_ACK,
    APP_PUT_ALARMS,
      
}_APPLICATION_STATE;

extern _APPLICATION_STATE APP_STATE;

void InitModemTasks(void);
void InitSYSTasks(void);
void Application_Tasks(void);
void InsertSYSTask(_SYS_TASKS SysTask);
void InsertMODEMTask(_MODEM_TASKS ModemTask);

_SYS_TASKS GetSYSTask(void);
_MODEM_TASKS GetMODEMTask(void);



#ifdef	__cplusplus
}
#endif

#endif	/* APITASK_H */

