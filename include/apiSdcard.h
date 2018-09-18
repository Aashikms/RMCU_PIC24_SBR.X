/* 
 * File:   apiSdcard.h
 * Author: Admin
 *
 * Created on July 20, 2015, 4:26 PM
 */

#ifndef APISDCARD_H
#define	APISDCARD_H

#ifdef	__cplusplus
extern "C" {
#endif

void WriteDatatoSD(void);
void DetectSDCard(void);
void ReadDataFromSD(void);


#ifdef	__cplusplus
}
#endif

#endif	/* APISDCARD_H */

