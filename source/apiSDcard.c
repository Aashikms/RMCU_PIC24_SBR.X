
#include <pps.h>
#include <string.h>
#include "pSystem.h"
#include "apiModem.h"
#include "FatFs/ff.h"
#include "FatFs/ff.h"
#include <apiSdcard.h>
#include "FatFs/diskio.h"
#include "HardwareProfile.h"



FATFS fatfs[_VOLUMES];

BYTE tFileName[25];
BYTE NEW_LINE = '\n';
BYTE Nline[3] = "\r\n";

const char NULL_CHAR = '\0';
const char MMC_DRIVE_LETTER[]   = "0:";
const char MMC_DRIVE_PATH[]     = "0:\\";
const char MMC_ROOT_PATH[]      = "0:/RMCU";

BYTE SD_READ_BUFFER[100];
UINT8 SD_WRITE_BUFFER[] = "Testing SD card functionality in PIC24";


void WriteDatatoSD(void)
{
    FIL fp;
    FRESULT res;
    UINT bw,Size;


    Size = strlen((const char *)SD_WRITE_BUFFER);

    res = FR_NO_FILE;

    res = f_open(&fp, "0:Test1.txt", FA_WRITE | FA_OPEN_ALWAYS);

    if(res == FR_OK)
    {
        f_lseek(&fp,f_size(&fp));
        f_putc(NEW_LINE,&fp);

        res = f_write(&fp, &SD_WRITE_BUFFER, Size, &bw);

        if(res == FR_OK)
        {
            memset(SD_WRITE_BUFFER,0,sizeof(SD_WRITE_BUFFER));
        }
     }

    f_close(&fp);
    memset(SD_WRITE_BUFFER,0,sizeof(SD_WRITE_BUFFER));

    //for testing
    ReadDataFromSD();

}


void ReadDataFromSD(void)
{
    FIL fpr;
    FRESULT resr;
    unsigned int len = 0,i = 0;
    UINT bc;

    memset(SD_READ_BUFFER,0,sizeof(SD_READ_BUFFER));

    // If Header file LHeader.csv is there in SD_Card read header from that file
        resr = f_open(&fpr,"0:Test1.txt",FA_READ);
        if(resr == FR_OK)
        {
            while(!f_eof(&fpr))
            {
                f_read(&fpr,&SD_READ_BUFFER[i++],1,&bc);
            }

            len = strlen((const char*)SD_READ_BUFFER);
            f_close(&fpr);

       }
}





void DetectSDCard(void)
{
    FRESULT res;

     //Initialize MMC

    if ((res = disk_status(MMC_DRIVE) & STA_NODISK) == FR_OK)
    {
        if ((res = disk_initialize(MMC_DRIVE)) == FR_OK)
        {
            if (f_mount(&fatfs[MMC_DRIVE], "", 1) == FR_OK)
            {
                f_chdir("0:\\");
                f_mkdir((TCHAR*)"RMCU");

                res = FR_NO_FILE;
                tFileName[0] = NULL_CHAR;
                strcat((char *)tFileName, (const char*)MMC_ROOT_PATH);
                f_chdir((const TCHAR*)tFileName);

            }
        }
    }
}