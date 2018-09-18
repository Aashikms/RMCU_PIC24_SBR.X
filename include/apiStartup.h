/* 
 * File:   startup.h
 * Author: Phoenix
 *
 * Created on July 24, 2013, 10:49 PM
 */

#ifndef STARTUP_H
#define	STARTUP_H

#ifdef	__cplusplus
extern "C" {
#endif

void SysInit(void);
void PLCINTInit(void);
void ConfigOSC(void);
void ConfigSYSTEM(void);




#define  SAMP_BUFF_SIZE	 		4		// Size of the input buffer per analog input
#define  NUM_CHS2SCAN			2		// Number of channels enabled for channel scan

#ifdef	__cplusplus
}
#endif

#endif	/* STARTUP_H */

