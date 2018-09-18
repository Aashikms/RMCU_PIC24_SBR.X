
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pSystem.h>
#include <apiTick.h>
#include <apiHelpers.h>


float result = 0;

UINT16 Uint2Bcd(UINT16 ival)
{
    return ((ival / 10) << 4) | (ival % 10);
}

UINT16 BcdtoInt(UINT16 bcd_val)
{
    return (((bcd_val >> 4) * 10) + (bcd_val & 0x0F));
}

unsigned int Bcd_to_Uint(unsigned char bcd_num)
{
    bcd_num = ((bcd_num * 100) + ((bcd_num >> 4) * 10)+((bcd_num) * 1));
    return bcd_num;

}

float CovCharToFloat(char D0, char D1, char D2, char D3,char *ff)
{
	*ff=D1;
	 ff++;
	*ff=D0;
	 ff++;
	*ff=D3;
	 ff++;
	*ff=D2;
	return *ff;
}

char *IntToString(int value)
 {
     static char buffer[12];        // 12 bytes is big enough for an INT32
     int original = value;        // save original value

     int c = sizeof(buffer)-1;

     buffer[c] = 0;                // write trailing null in last byte of buffer

     if (value < 0)                 // if it's negative, note that and take the absolute value
         value = -value;

     do                             // write least significant digit of value that's left
     {
         buffer[--c] = (value % 10) + '0';
         value /= 10;
     } while (value);

     if (original < 0)
         buffer[--c] = '-';

     return &buffer[c];
 }

float CovStr2Float(UINT8 *str,float *result)
{

    UINT8 len = 0,n = 0;
    UINT8 dotpos = 0;

    len = strlen((const char *)str);

    for (n = 0; n < len; n++)
    {
        if (str[n] == '.')
        {
            dotpos = len - n  - 1;
        }
        else
        {
            *result = *result * 10.0f + (str[n]-'0');
        }
    }
    while ( dotpos--)
    {
        *result /= 10.0f;
    }
    return *result;
}


unsigned char ASCII_TO_HEX(  UINT8 ascii_text_Lb, UINT8 ascii_text_Hb)
{
    unsigned char bcd_value;

    // left side
    if(ascii_text_Lb >= '0' && ascii_text_Lb <= '9')  // 0-9 range
    {
        bcd_value = ( ascii_text_Lb - 48)  << 4 ; // 48 for '0' ASCII offset
    }
    else if (ascii_text_Lb >= 'A' && ascii_text_Lb <= 'F') // A-F range
    {
        bcd_value = ( 10 + ascii_text_Lb - 65 )  << 4 ; // 65 for 'A' ASCII offset
    }
    else if (ascii_text_Lb >= 'a' && ascii_text_Lb <= 'f') // a-f range
    {
        bcd_value = ( 10 + ascii_text_Lb - 97)  << 4 ; // 97 for 'a'  ASCII offset
    }

    // right side
    if(ascii_text_Hb >= '0' && ascii_text_Hb <= '9')  // 0-9 range
    {
        bcd_value |= ( ascii_text_Hb - 48); // 48 for '0' ASCII offset
    }
    else if (ascii_text_Hb >= 'A' && ascii_text_Hb <= 'F') // A-F range
    {
        bcd_value |= ( 10 + ascii_text_Hb - 65)   ; // 65 for 'A' ASCII offset
    }
    else if (ascii_text_Hb >= 'a' && ascii_text_Hb <= 'f') // a-f range
    {
        bcd_value |= ( 10 + ascii_text_Hb - 97 ) ; // 97 for 'a' ASCII offset
    }

    return bcd_value;
}