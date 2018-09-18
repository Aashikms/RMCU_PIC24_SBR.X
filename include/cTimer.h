/* 
 * File:   cTimer.h
 * Author: Admin
 *
 * Created on June 2, 2015, 12:00 PM
 */

#ifndef CTIMER_H
#define	CTIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define EN_1MS_TIMER
//#define EN_1SEC_TIMER

extern volatile unsigned long long MdbWaitctr;
extern volatile unsigned long long I2cTimeout;
extern volatile unsigned long long RestartCounter;
extern volatile unsigned long long MdbRespTimeout;

void ConfigFreeRunningTimer(void);

#ifdef	__cplusplus
}
#endif

#endif	/* CTIMER_H */

