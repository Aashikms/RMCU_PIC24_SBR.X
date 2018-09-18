/* 
 * File:   apiHelpers.h
 * Author: Admin
 *
 * Created on May 26, 2015, 12:11 PM
 */

#ifndef APIHELPERS_H
#define	APIHELPERS_H

#ifdef	__cplusplus
extern "C" {
#endif



char *strrev(char *string);
char *IntToString(int value);
UINT16 Uint2Bcd(UINT16 ival);
UINT16 BcdtoInt(UINT16 bcd_val);
char* ConvtFloatToAsci(double x);
unsigned char ASCII_TO_HEX(UINT8,UINT8);
unsigned int Bcd_to_Uint(unsigned char);
float CovStr2Float(UINT8 *str,float *result);
float CovCharToFloat(char D0, char D1, char D2, char D3,char *ff);

#ifdef	__cplusplus
}
#endif

#endif	/* APIHELPERS_H */

