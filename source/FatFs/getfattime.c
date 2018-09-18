#include "Fatfs/diskio.h"
#include "Fatfs/ff.h"
#include "GenericTypeDefs.h"
#include "apiRTC.h"
#include "pSystem.h"


// local variables
volatile BYTE rtcYear = 15, rtcMon = 11, rtcMday = 17;	// RTC starting values
volatile BYTE rtcHour = 12, rtcMin, rtcSec;
volatile BYTE tick;			

volatile UINT Timer;		/* 1kHz increment timer */

/*********************************************************************
 * Function:  		DWORD get_fattime(void)
 *
 * PreCondition:    
 *
 * Input:           None
 *
 * Output:          Time
 *
 * Side Effects:    
 *
 * Overview:        when writing fatfs requires a time stamp
 *					in this exmaple we are going to use a counter
 *					If the starter kit has the 32kHz crystal
 *					installed then the RTCC could be used instead
 *
 * Note:           
 ********************************************************************/
DWORD get_fattime(void)
{
	DWORD tmr = 0;

        INTCON2bits.GIE = 0;
        tmr =	  (((DWORD)RTS.Year - 1980) << 25)
			| ((DWORD)RTS.Month << 21)
			| ((DWORD)RTS.Date << 16)
			| (WORD)(RTS.Hour << 11)
			| (WORD)(RTS.Minute << 5)
			| (WORD)(RTS.Second >> 1);

        INTCON2bits.GIE = 1;
	return tmr;
}
