/*
 * File:   libModemUART.c
 * Author: Krishna
 *
 * Created on July 22, 2013, 7:25 PM
 */

#include <xc.h>
#include <uart.h>
#include <timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apiRTC.h"
#include <apiTick.h>
#include <pSystem.h>
#include "apiTask.h"
#include "apiDelay.h"
#include <apiModem.h>
#include "apiEEPROM.h"
#include "apiHelpers.h"
#include <libModemUART.h>
#include <ctype.h>



static MODEM_TIMER_PERIOD       comTimerPeriod      = MODEM_TIMER_PERIOD_10S;
static MODEM_TIMER_PRESCALER    comTimerPrescaler   = MODEM_TIMER_PRESCALER_256;
static MODEM_TIMER_PRESCALER    ModemTimerPrescaler = MODEM_TIMER_PRESCALER_256;

static MODEM_TIMER_PERIOD       ServerTimerPeriod    = MODEM_TIMER_PERIOD_60S;
static MODEM_TIMER_PRESCALER    ServerTimerPrescaler = SERVER_TIMER_PRESCALER_256;

#define ConfigModemTimer()          ConfigIntTimer23(T3_INT_OFF | T3_INT_PRIOR_2)
#define OpenModemTimer()            OpenTimer23(T2_ON & T2_GATE_OFF & ModemTimerPrescaler  & T2_SOURCE_INT & T2_32BIT_MODE_ON,  (GetPeripheralClock()/256)*comTimerPeriod)
#define OpenServerTimer()           OpenTimer23(T2_ON & T2_GATE_OFF & ServerTimerPrescaler & T2_SOURCE_INT & T2_32BIT_MODE_ON, (GetPeripheralClock()/256)*ServerTimerPeriod)
#define EnModemTimerInterrupt()     EnableIntT3
#define ClrModemTimerInterrupt()    (IFS0bits.T3IF = 0)

extern void ModemCallBack(void);
extern DWORD Timeout_Reg;
DWORD MODEM_TIMEOUT_COUNT;

_MODEM_TASKS mTask;
_MODEM_TASKS MODEM_TASK;
_NETWORK_TIMESTMAP NRTS;
_GPRS_STATUS GPRS_STATUS;
_MODEM_STATUS MODEM_STATUS;
_MODEM_RX_STATUS MODEM_RX_STATUS;
_MODEM_UARTRX_STATUS mUARTRX_STATUS;
_MODEM_CONNECTION_STATUS MODEM_CONNECTION_STATUS;

extern _FLAGS FLAGS;
extern _DATETIME DATE_TIME;
extern _MODEM_STATE cMODEM_STATE;
extern _MODEM_STATE MODEM_STATE;
extern _MODEM_STATE nMODEM_STATE;


BYTE buf[REORDER_BUF_SIZE];
static BYTE vUARTMRXFIFO[uMODEM_RX_BUFFER_SIZE] = {0};
static BYTE vUARTMTXFIFO[uMODEM_TX_BUFFER_SIZE] = {0};

BYTE uMsgRxFifo[uMSG_RX_BUFFER_SIZE];
char pMsgRxFifo[pMSG_RX_FIFO_SIZE][pMSG_RX_WIDTH_SIZE];


static char MsgIndexBuffer[10][10];
static UINT8 MsgIndexHead = 0;
static UINT8 MsgIndexTail = 0;

BYTE *MRXHeadPtrShadow;
BYTE *MRXTailPtrShadow;
static volatile BYTE *MRXHeadPtr = vUARTMRXFIFO, *MRXTailPtr = vUARTMRXFIFO;
static volatile BYTE *MTXHeadPtr = vUARTMTXFIFO, *MTXTailPtr = vUARTMTXFIFO;

const char ComamndSet1[2] = {0x1A};
static const char txBufferPrefix[] = "AT";
static const char txBufferSuffix[] = "\r\n";

volatile unsigned int count = 3;

UINT16 modemRxdbytes = 0;
UINT16 cmodemRxdbytes = 0;

UINT8 Ntime_buf[30];
UINT8 Btime_buf[30];

BYTE ServerRespBuffer[RESP_BUFFER_SIZE];
UINT8 RxCmdCount = 0,CommandRxd = 0,CommandNotRxd = 0;
UINT8 RetransmitCount = 0,AckRxd = 0,AckNotRxd = 0;


/*********************************************************************
 * Function:        void ConfigModemUART(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Sets up the UART peripheral for this application
 *
 * Note:            Uses interrupts
 ********************************************************************/
void ConfigModemUART(void)
{
    UINT64 baud = 0;

    TRISBbits.TRISB12 = 1;              // PIN RB12 as a Rx
    TRISBbits.TRISB10 = 0; 		//PIN RB10 as a Tx

    //Remapping the pins RP42 & RPI44 as
    RPOR4   = 0x0001;           //RP42       //MC_TX
    RPINR18 = 0x002C;           //RPI44      //MC_RX    

    baud = BRG_REG_VALUE_GSM;
    U1BRG  = baud;                      //19200

    U1MODE = 0x0008; 			//turn on module, BRGH = 1 (high speed mode)
    U1STA  = 0x8400; 			//8400	//set interrupts

    IFS0bits.U1TXIF = 0;
    IFS0bits.U1RXIF = 0;

    U1STAbits.UTXISEL0 = 0; // Interrupt after one TX character is transmitted
    U1STAbits.UTXISEL1 = 0;

    _U1RXIF = 0;
    _U1RXIE = 1;
    _U1RXIP = 1;

    _U1EIP = 1;
    _U1TXIF = 0;
    _U1TXIE = 1;
    _U1TXIP = 6;

    U1MODEbits.UARTEN = 1; // Enable UART
    U1STAbits.UTXEN = 1;

    U1STAbits.FERR = 0;
    U1STAbits.PERR = 0;
    U1STAbits.OERR = 0;

    ConfigModemTimer();

    MODEM_STATE = MODEM_POWERDOWN;
    MODEM_STATUS = MODEM_NOT_INITIALIZED;
    InitModemTasks();

}

void PutModemUART(char *pcString, _MODEM_DATA_TYPE mdType, int size)
{
    UINT32 commandIsSms;
    BYTE *MTXHeadPtrShadow, *MTXTailPtrShadow;
    
    // Reset TX buffer
    memset(vUARTMTXFIFO, 0, sizeof(vUARTMTXFIFO));

    if(mdType == MODEM_COMMAND)
    {
        // check if command is an SMS
        commandIsSms = (strncmp((const char*)pcString, "+CMGS", strlen("+CMGS")) == 0)? 1: 0;

        SetTimeoutTimer();

        // if command begins with '+" then insert heading string (prefix)
        if (pcString[0] == '+' && pcString[1] != '+')
        {
            strcat((char*)vUARTMTXFIFO, (const char*)txBufferPrefix);
            size += strlen((const char*)txBufferPrefix);
        }

        if (!commandIsSms) //added later
            strcat((char*)vUARTMTXFIFO, (const char*)pcString);

        //  add trailing <CR><LF> unless we are writing an SMS
        if(cMODEM_STATE == MODEM_CMGS_TEXT)
            strcat((char*)vUARTMTXFIFO, (const char*)ComamndSet1);

        if (!commandIsSms)
        {
            if(cMODEM_STATE != MODEM_TRIPPLE_PLUS)
            {
                strcat((char*)vUARTMTXFIFO, (const char*)txBufferSuffix);
                size += strlen((const char*)txBufferSuffix);
            }
        }
    }
    else
    {
        memcpy(vUARTMTXFIFO, pcString, size);
        commandIsSms = 0;
    }
    
    // open command timer...
    if(cMODEM_STATE == MODEM_GREATER)
    {
        StartServerResponseTimeoutTimer();
    }
    
    else
    {
        ResetModemTimer();
    }

    if (commandIsSms)
    {
        // Read FIFO pointers into a local shadow copy.  Some pointers are volatile
        // (modified in the ISR), so we must do this safely by disabling interrupts
        IEC0bits.U1TXIE = 0;
        MTXHeadPtrShadow = (BYTE*)MTXHeadPtr;
        MTXTailPtrShadow = (BYTE*)MTXTailPtr;

        if(MTXHeadPtrShadow != MTXTailPtrShadow)
        {
            IEC0bits.U1TXIE = 1;
        }

        *MTXHeadPtrShadow++ = 'A';  //Added later for SMS
        *MTXHeadPtrShadow++ = 'T';

        while(*pcString)
        {
            *MTXHeadPtrShadow++ = *pcString;            // Write character
            pcString++;

            if(MTXHeadPtrShadow >= vUARTMTXFIFO + sizeof(vUARTMTXFIFO))
            MTXHeadPtrShadow = vUARTMTXFIFO;
        }

       *MTXHeadPtrShadow++ = '\r';          //Added

        // Write local shadowed FIFO pointers into the volatile FIFO pointers.
        IEC0bits.U1TXIE = 0;
        MTXHeadPtr = (volatile BYTE*)MTXHeadPtrShadow;

        if(MTXHeadPtrShadow != MTXTailPtrShadow)
        {
            IEC0bits.U1TXIE = 1;
        }

        /* Write out each character with appropriate delay between each. */
    }
    else
    {
        IEC0bits.U1TXIE = 0;
        //Reset Head & Tail
        MTXHeadPtr = vUARTMTXFIFO;
        MTXTailPtr = vUARTMTXFIFO;
        
        MTXHeadPtr +=size; 
        IEC0bits.U1TXIE = 1;
    }
    return;
}

/*********************************************************************
 * Function:        void _ModemRXInterrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies bytes to and from the local UART TX and
 *					RX FIFOs
 *
 * Note:            None
 ********************************************************************/
void _ModemRXInterrupt(void)
{
    BYTE RxByte;
    static int firstbyte = 0;

    while(U1STAbits.URXDA)
    {
        RxByte = U1RXREG;

        if(MRXHeadPtr != MRXTailPtr - 1)
        {
            if((MRXHeadPtr != vUARTMRXFIFO + sizeof(vUARTMRXFIFO)) || (MRXTailPtr != vUARTMRXFIFO))
            {
                if(firstbyte)
                {
                    firstbyte = 0;
                    *MRXHeadPtr++ = RxByte;
                    if(RxByte =='\n')
                    {
                        MRXHeadPtrShadow = (BYTE *)MRXHeadPtr;
                        mUARTRX_STATUS = EOS_RXD;
                    }
                    CheckGreaterSymbol(RxByte);
                }
                firstbyte = 1;
                if(MRXHeadPtr >= vUARTMRXFIFO + sizeof(vUARTMRXFIFO))
                    MRXHeadPtr = vUARTMRXFIFO;
            }
        }
    }

    if ((U1STAbits.PERR | U1STAbits.FERR | U1STAbits.OERR))
    {
        U1STAbits.FERR = 0;
        U1STAbits.PERR = 0;
        U1STAbits.OERR = 0;
    }

    // Clear the interrupt flag so we don't keep entering this ISR
    IFS0bits.U1RXIF = 0;
}

void CheckGreaterSymbol(BYTE Rxbyte)
{
    if (Rxbyte == '>')
    {
        if(cMODEM_STATE == MODEM_QISEND)
        {
            MODEM_STATE = MODEM_QISEND_RESP;
            DsModemTimerInterrupt();
            ModemCallBack();
            MODEM_CONNECTION_STATUS = SERVER_CONNECTED;
        }
        else if(cMODEM_STATE == MODEM_CMGS)
        {
            MODEM_STATE = MODEM_CMGS_RESP;
            DsModemTimerInterrupt();
            ModemCallBack();
        }
    }
}

void PushMsgInedx(char *message)
{

    MsgIndexBuffer[MsgIndexHead][0] = 0;
    strcpy((char*)MsgIndexBuffer[MsgIndexHead], message);

    if(MsgIndexHead++ >= MSG_INDEX_BUFFER_SIZE)
    {
        MsgIndexHead = 0;
    }
    
}

char *PopMsgInedx(void)
{
    char *ptr;
    static BYTE PopCount = 0;
    
    if(MsgIndexTail >= MSG_INDEX_BUFFER_SIZE)
        MsgIndexTail = 0;

    if (CheckMsgIndexFifo())
    {
        ptr = MsgIndexBuffer[MsgIndexTail];
        PopCount++;

        //need to pop 2 times once for read SMS and one more time for Delete SMS
        if(PopCount >=2)
        {
            PopCount = 0;
            MsgIndexTail++;
        }
        return ptr;
    }
    else
        return 0;
}

UINT32 CheckMsgIndexFifo(void)
{
    if(MsgIndexHead != MsgIndexTail)
        return 1;
    else
        return 0;
}



void ReorderBuffer(BYTE* pbuf)
{
    UINT size;
    memset(buf, 0, REORDER_BUF_SIZE);

    while(MRXHeadPtrShadow != MRXTailPtr)
    {
        *pbuf++ = *MRXTailPtr++;
        CheckEndOfRxBuffer();

        size = strlen((const char *)buf);
        if(size >= REORDER_BUF_SIZE)
        {
            //To avoid overflow of destination buffer
            memset(buf, 0, REORDER_BUF_SIZE);
            pbuf = (BYTE *)buf;
        }
    }
}

void ServerRespReorderBuffer(BYTE* pbuf)
{
    UINT size;
    BYTE *StAddr = buf;

    while(MRXHeadPtrShadow != MRXTailPtr)
    {
        *pbuf++ = *MRXTailPtr++;
        CheckEndOfRxBuffer();

        size = strlen((const char *)buf);
        if(size >= SERVER_REORDER_BUF_SIZE)
        {
            //To avoid overflow of destination buffer
            pbuf = (BYTE *)StAddr;
        }
    }
}

void SMSReorderBuffer(BYTE* pbuf)
{
    UINT size;
    BYTE *StAddr = pbuf;
    
    while(MRXHeadPtr != MRXTailPtr)
    {
        *pbuf++ = *MRXTailPtr++;
        CheckEndOfRxBuffer();

        size = strlen((const char *)pbuf);
        if(size >= SMS_REORDER_BUF_SIZE)
        {
            //To avoid overflow of destination buffer
            pbuf = (BYTE *)StAddr;
        }
    }
}



void StartServerResponseTimeoutTimer(void)
{
    ClrModemTimerInterrupt();
    OpenServerTimer();
    EnModemTimerInterrupt();
}


void SetTimeoutTimer(void)
{
    switch(cMODEM_STATE)
    {
        case MODEM_AT:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_ATE:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;
            
        case MODEM_TURNOFF:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_DCGATT:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_CMGF:
            comTimerPeriod = MODEM_TIMER_PERIOD_5S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_CMGR:
            comTimerPeriod = MODEM_TIMER_PERIOD_5S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_DEL_ALL:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_DEL_READ:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_CSQ:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_SET_CCLK:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_GET_CCLK:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QNITZ_EN:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;
        case MODEM_QLTS_EN:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_CFUN:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QSPN:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_CREG:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_CGREG:
            comTimerPeriod = MODEM_TIMER_PERIOD_20S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_CGATT:
            comTimerPeriod = MODEM_TIMER_PERIOD_20S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_CGATT_MATT:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QINISTAT:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_CMGS:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_CMGS_TEXT:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QIFGCNT:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QICSGP:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QIMUX:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;
            
        case MODEM_QIMODE:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;
            
        case MODEM_QITCFG:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QIDNSIP:
            comTimerPeriod = MODEM_TIMER_PERIOD_10S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;
            
        case MODEM_QIREGAPP:
            comTimerPeriod = MODEM_TIMER_PERIOD_2S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QIACT:
            comTimerPeriod = MODEM_TIMER_PERIOD_60S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;        

        case MODEM_QIOPEN:
            comTimerPeriod = MODEM_TIMER_PERIOD_60S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QISEND:
            comTimerPeriod = MODEM_TIMER_PERIOD_20S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_GREATER:
            comTimerPeriod = MODEM_TIMER_PERIOD_45S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QICLOSE:
            comTimerPeriod = MODEM_TIMER_PERIOD_5S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_QIDEACT:
            comTimerPeriod = MODEM_TIMER_PERIOD_45S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_2;//prescaler;
            break;

        case MODEM_TRIPPLE_PLUS:
            comTimerPeriod = MODEM_TIMER_PERIOD_5S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

        case MODEM_HW_TURNOFF:
            comTimerPeriod = MODEM_TIMER_PERIOD_5S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            break;

            /*Not used just to avoid warnings*/
        case MODEM_POWERDOWN:
            break;

        case MODEM_STANDBY:
            break;

        case MODEM_TURNOFF_DELAY:
            break;

        case MODEM_WAIT:
            break;

        case MODEM_POWERUP:
            break;

        case MODEM_VERSION:
            break;

        case MODEM_GET_GPRS_DATA:
            break;

        case MODEM_BASE_CSQ:
            break;

        case MODEM_CMGS_MODE:
            break;

        case MODEM_GET_IP:
            break;

        case MODEM_GET_IP_RESP:
            break;

        case MODEM_ACK_SERVER:
            break;

        case MODEM_SEND_READY:
            break;

        case MODEM_SEND_DATA:
            break;

        case MODEM_CHECK_ACK:
            break;

        case MODEM_SLEEP:
            break;

        case MODEM_FUN:
            break;

        case MODEM_RESP_WAIT:
            break;

        case MODEM_ACK_WAIT:
            break;

        case MODEM_DLY_WAIT:
            break;

        case MODEM_SET_DLY:
            break;

        case MODEM_TIMEOUT:
            break;

        case MODEM_STIMEOUT:
            break;

        case MODEM_POWERDOWN_RESP:
            break;

        case MODEM_AT_RESP:
            break;

        case MODEM_ATE_RESP:
            break;

        case MODEM_DCGATT_RESP:
            break;

        case MODEM_CSQ_RESP:
            break;

        case MODEM_SERVER_RESP:
            break;

        case MODEM_QNITZ_EN_RESP:
            break;
        case MODEM_QLTS_EN_RESP:
            break;
        case MODEM_SET_CCLK_RESP:
            break;

        case MODEM_GET_CCLK_RESP:
            break;

        case MODEM_BUILD_STRING_RESP:
            break;

        case MODEM_BASE_CSQ_RESP:
            break;

        case MODEM_CFUN_RESP:
            break;

        case MODEM_QSPN_RESP:
            break;

        case MODEM_CREG_RESP:
            break;

        case MODEM_CGREG_RESP:
            break;

        case MODEM_CGATT_RESP:
            break;

        case MODEM_QINISTAT_RESP:
            break;

        case MODEM_CGATT_MATT_RESP:
            break;

        case MODEM_CMGS_MODE_RESP:
            break;

        case MODEM_CMGS_RESP:
            break;

        case MODEM_CMGS_TEXT_RESP:
            break;

        case MODEM_QIFGCNT_RESP:
            break;

        case MODEM_QICSGP_RESP:
            break;

        case MODEM_QIMUX_RESP:
            break;

        case MODEM_QIMODE_RESP:
            break;

        case MODEM_QITCFG_RESP:
            break;

        case MODEM_QIDNSIP_RESP:
            break;

        case MODEM_IMAGE_RESP:
            break;

        case MODEM_QIREGAPP_RESP:
            break;

        case MODEM_QIACT_RESP:
            break;

        case MODEM_QIOPEN_RESP:
            break;

        case MODEM_QISEND_RESP:
            break;

        case MODEM_GREATER_RESP:
            break;

        case MODEM_PRE_QICLOSE_RESP:
            break;

        case MODEM_QICLOSE_RESP:
            break;

        case MODEM_QIDEACT_RESP:
            break;

        case MODEM_CMGF_RESP:
            break;

        case MODEM_CMGR_RESP:
            break;

        case MODEM_DEL_ALL_RESP:
            break;

        case MODEM_DEL_READ_RESP:
            break;

        case MODEM_TURNOFF_RESP:
            break;

        case MODEM_SLEEP_RESP:
            break;

        case MODEM_FUN_RESP:
            break;

        case MODEM_TRIPPLE_PLUS_RESP:
            break;

        case MODEM_PROCESS_DATA:
            break;

            /****************/
    }
}

int SetDlyTimer(void)
{
    switch(nMODEM_STATE)
    {
        case MODEM_POWERDOWN:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_TURNOFF:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_AT:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_ATE:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_CMGF:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_CMGR:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_DEL_ALL:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_DEL_READ:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_DCGATT:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_CSQ:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_SET_CCLK:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_GET_CCLK:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QNITZ_EN:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;
        
        case MODEM_QLTS_EN:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;
            
       case MODEM_CREG:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_CGREG:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_CGATT:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_CGATT_MATT:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QINISTAT:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_CMGS:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_CMGS_TEXT:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_CFUN:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QSPN:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QIFGCNT:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QICSGP:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QIMUX:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QIMODE:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QITCFG:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;
            
        case MODEM_QIDNSIP:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;
            
        case MODEM_QIREGAPP:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QIACT:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QIOPEN:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_SEND_READY:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_SEND_DATA:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_CHECK_ACK:
            comTimerPeriod = MODEM_TIMER_PERIOD_2S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_ACK_WAIT:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QISEND:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_GREATER:
            comTimerPeriod = 0;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QICLOSE:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_QIDEACT:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_TRIPPLE_PLUS:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;
            
        case MODEM_HW_TURNOFF:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

        case MODEM_STANDBY:
            comTimerPeriod = MODEM_TIMER_PERIOD_1S;//period;
            comTimerPrescaler = MODEM_TIMER_PRESCALER_256;//prescaler;
            return comTimerPeriod;
            break;

            /*Not used just to avoid warnings*/

        case MODEM_POWERUP:
            break;

        case MODEM_TRIPPLE_PLUS_RESP:
            break;

        case MODEM_PROCESS_DATA:
            break;
        case MODEM_TURNOFF_DELAY:
            break;
        case MODEM_WAIT:
            break;
        case MODEM_VERSION:
            break;
        case MODEM_GET_GPRS_DATA:
            break;
        case MODEM_BASE_CSQ:
            break;
        case MODEM_CMGS_MODE:
            break;
        case MODEM_GET_IP:
            break;
        case MODEM_GET_IP_RESP:
            break;
        case MODEM_ACK_SERVER:
            break;
        case MODEM_SLEEP:
            break;
        case MODEM_FUN:
            break;
        case MODEM_RESP_WAIT:
            break;
        case MODEM_DLY_WAIT:
            break;
        case MODEM_SET_DLY:
            break;
        case MODEM_TIMEOUT:
            break;
        case MODEM_STIMEOUT:
            break;
        case MODEM_POWERDOWN_RESP:
            break;
        case MODEM_AT_RESP:
            break;
        case MODEM_DCGATT_RESP:
            break;
        case MODEM_CSQ_RESP:
            break;
        case MODEM_SERVER_RESP:
            break;
        case MODEM_QNITZ_EN_RESP:
            break;
        case MODEM_QLTS_EN_RESP:
            break;
        case MODEM_SET_CCLK_RESP:
            break;
        case MODEM_GET_CCLK_RESP:
        break;
        case MODEM_BUILD_STRING_RESP:
            break;
        case MODEM_BASE_CSQ_RESP:
            break;
        case MODEM_CFUN_RESP:
            break;
        case MODEM_QSPN_RESP:
            break;
        case MODEM_CREG_RESP:
            break;
        case MODEM_CGREG_RESP:
            break;
        case MODEM_CGATT_RESP:
            break;
        case MODEM_QINISTAT_RESP:
            break;
        case MODEM_CGATT_MATT_RESP:
            break;
        case MODEM_CMGS_MODE_RESP:
            break;
        case MODEM_CMGS_RESP:
            break;
        case MODEM_CMGS_TEXT_RESP:
            break;
        case MODEM_QIFGCNT_RESP:
            break;
        case MODEM_QICSGP_RESP:
            break;
        case MODEM_QIMUX_RESP:
            break;
        case MODEM_QIMODE_RESP:
            break;
        case MODEM_QITCFG_RESP:
            break;
        case MODEM_QIDNSIP_RESP:
            break;
        case MODEM_IMAGE_RESP:
            break;
        case MODEM_QIREGAPP_RESP:
            break;
        case MODEM_QIACT_RESP:
            break;
        case MODEM_QIOPEN_RESP:
            break;
        case MODEM_QISEND_RESP:
            break;
        case MODEM_GREATER_RESP:
            break;
        case MODEM_PRE_QICLOSE_RESP:
            break;
        case MODEM_QICLOSE_RESP:
            break;
        case MODEM_QIDEACT_RESP:
            break;
        case MODEM_CMGF_RESP:
            break;
        case MODEM_CMGR_RESP:
            break;
        case MODEM_DEL_ALL_RESP:
            break;
        case MODEM_DEL_READ_RESP:
            break;
        case MODEM_TURNOFF_RESP:
            break;
        case MODEM_SLEEP_RESP:
            break;
        case MODEM_FUN_RESP:
            break;

        default:
            return 0;
            break;
        /***************/

    }
return comTimerPeriod;
}

void ProcessModemResponse(void)
{

    UINT i;

    if(MRXHeadPtr != MRXTailPtr)
    {

        if(MODEM_CONNECTION_STATUS == SERVER_CONNECTED)
        {
            if(mUARTRX_STATUS == EOS_RXD)
            {
                char *ptr;
                UINT size;
                BYTE tServerRespBuffer[SERVER_REORDER_BUF_SIZE];

                mUARTRX_STATUS = EOS_PROCESS;

                memset(tServerRespBuffer,0,sizeof(tServerRespBuffer));
                ServerRespReorderBuffer((BYTE*)tServerRespBuffer);
                
                size = strlen((const char *)ServerRespBuffer);

                if(size >= RESP_BUFFER_SIZE)
                {
                    memset(ServerRespBuffer,0,sizeof(ServerRespBuffer));
                }

                strcat((char *)ServerRespBuffer,(const char *)tServerRespBuffer);

                if((ptr = strstr((char*)ServerRespBuffer, "Date: ")) != NULL)
                {
                    if(FLAGS.GetServerTime)
                    {
                        ptr += 6;
                        if (!(PharseServerTime(ptr,0)))
                        {
                            RTCErrorResponse();
                        }
                        else
                            InsertSYSTask(FRAME_BYTE_STREAM_TASK);
                    }
                    MODEM_RX_STATUS = RECEIVE_DONE;
                }
                
                if((ptr = strstr((char*)ServerRespBuffer, "HTTP/1.1 200 OK")) != NULL)
                {
                    modemRxdbytes = 0;
                    FLAGS.Server_Status_Success = 1;
                    if(!FLAGS.GetServerTime)
                        MODEM_RX_STATUS = RECEIVE_DONE;
                }

                if((ptr = strstr((char*)ServerRespBuffer, "3CA10000")) != NULL)
                {

                }

                else if((ptr = strstr((char*)ServerRespBuffer, "3CA10001")) != NULL)
                {
                    modemRxdbytes = 0;
                    FLAGS.Server_Status_Success = 0;
                    if(!FLAGS.GetServerTime)
                        MODEM_RX_STATUS = RECEIVE_DONE;
                }

                if ((strstr((const char*)ServerRespBuffer, "ERROR")) != NULL)
                {
                    MODEM_RX_STATUS = RECEIVE_DONE;
                    if(FLAGS.GetServerTime)
                    {
                        RTCErrorResponse();
                    }
                }

                if ((strstr((char*)ServerRespBuffer, "CLOSED")) != NULL)
                {
                    modemRxdbytes = 0;
                    MODEM_CONNECTION_STATUS = SERVER_DISCONNECTED;
                    MODEM_STATE = MODEM_PRE_QICLOSE_RESP;
                    cMODEM_STATE = MODEM_QICLOSE;
                    DsModemTimerInterrupt();
                    ModemCallBack();
                    FLAGS.ConnectionTimeOut = 1;
                    memset(ServerRespBuffer,0,sizeof(ServerRespBuffer));
                }

                if ((ptr = strstr((const char*)ServerRespBuffer, "+CMTI:")) != NULL)
                {
                    ptr = ptr + 12;
                    mTask.Parameters[0] = *ptr;
                    ptr++;
                    mTask.Parameters[1] = *ptr;
                    mTask.Parameters[2] = '\0';
                    PushMsgInedx(mTask.Parameters);
                    mTask.MODEM_TASK_STATE = MODEM_READ_SMS_TASK;
                    InsertMODEMTask(mTask);
                    memset(ServerRespBuffer,0,sizeof(ServerRespBuffer));
                        modemRxdbytes = 0;
                }

                if(MODEM_RX_STATUS == RECEIVE_DONE)
                {
                    /**********************Server ACK************************/

                    memset(ServerRespBuffer, 0, sizeof(ServerRespBuffer));
                    if(FLAGS.Server_Status_Success)
                    {
                        AckRxd = 1;
                        DsModemTimerInterrupt();
                        MODEM_STATE = MODEM_CHECK_ACK;
                        MODEM_CONNECTION_STATUS = SERVER_DISCONNECTED;
                    }

                    else
                    {
                        AckRxd = 1;
                        DsModemTimerInterrupt();
                        MODEM_STATE = MODEM_CHECK_ACK;
                        MODEM_CONNECTION_STATUS = SERVER_DISCONNECTED;
                    }

                    MODEM_RX_STATUS = RECEIVE_WAIT;

                    /*********************** end *************************************/
                }

                if(mUARTRX_STATUS == EOS_PROCESS)
                {
                    mUARTRX_STATUS = EOS_WAIT;
                }
            }
        }

        //For modem response
        else
        {
            if(mUARTRX_STATUS == EOS_RXD)
            {
                char *res;

                mUARTRX_STATUS = EOS_PROCESS;

                ReorderBuffer((BYTE*)buf);

                if ((res = strstr((const char*)buf, "+CMTI:")) != NULL)
                {
                    res = res + 12;
                    mTask.Parameters[0] = *res;
                    res++;
                    mTask.Parameters[1] = *res;
                    mTask.Parameters[2] = '\0';
                    PushMsgInedx(mTask.Parameters);
                    mTask.MODEM_TASK_STATE = MODEM_READ_SMS_TASK;
                    InsertMODEMTask(mTask);
                }

                if ((res = strstr((char*)buf, "+CMGR:")) != NULL)
                {
                    BYTE TempSMSBuf[SMS_REORDER_BUF_SIZE];
                    DelayMs(50);

                    memset(TempSMSBuf, 0, sizeof(TempSMSBuf));
                    SMSReorderBuffer((BYTE*)TempSMSBuf);
                    strcat((char *)buf,(const char *)TempSMSBuf);

                    if ((res = strstr((char*)buf, "+CMGR:")) != NULL)
                    {
                        res += 5;

                        memset(TempSMSBuf, 0, sizeof(TempSMSBuf));
                        strcat((char *)TempSMSBuf,(const char *)res);

                        memset(buf, 0, sizeof(buf));

                        UINT8 CR_Count = 0;

                       for(i = 0; i<MAX_CHARS_IN_SMS; i++)
                       {
                            uMsgRxFifo[i] = TempSMSBuf[i];

                            if(TempSMSBuf[i] == '\r')
                            {
                                CR_Count++;
                                if(CR_Count >=2)
                                {
                                    break;
                                }
                            }
                        }

                        CR_Count = 0;

                        pMsgRxFifoPush(uMsgRxFifo);  //Fill  msg FIFO

                        memset(buf, 0, REORDER_BUF_SIZE);
                        GPRS_STATUS.CMGR = 1;
                        MODEM_STATE = MODEM_CMGR_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }
                }

                else if ((res = strstr((const char*)buf, "+QSPN:")) != NULL)
                {
                    GPRS_STATUS.QSPN = 1;

                    if((strstr((char*)buf, "airtel")) != NULL)
                    {
                        GPRS_STATUS.SC_PROVIDER = AIRTEL;
                    }
                    else if((strstr((char*)buf, "bsnl")) != NULL)
                    {
                        GPRS_STATUS.SC_PROVIDER = BSNL;
                    }
                    else if((strstr((char*)buf, "idea")) != NULL)
                    {
                        GPRS_STATUS.SC_PROVIDER = IDEA;
                    }
                    else if((strstr((char*)buf, "AIRCEL")) != NULL)
                    {
                        GPRS_STATUS.SC_PROVIDER = AIRCEL;
                    }
                    else if((strstr((char*)buf, "vodafone IN")) != NULL)
                    {
                        GPRS_STATUS.SC_PROVIDER = VODAFONE;
                    }
                    else
                        GPRS_STATUS.SC_PROVIDER = AIRTEL;
                }
                else if ((strstr((char*)buf, "+CSQ:")) != NULL)
                {
                    char *res;
                    res = strstr((char*)buf, "+CSQ:");

                    if(res != NULL)
                    {
                        res = res + 6;
                        GPRS_STATUS.SG_STNTH[0] = *res;

                        if(GPRS_STATUS.SG_STNTH[0] < 48 && GPRS_STATUS.SG_STNTH[0] < 57)
                            GPRS_STATUS.SG_STNTH[0] = 0;
                        res++;
                        GPRS_STATUS.SG_STNTH[1] = *res;

                        if(GPRS_STATUS.SG_STNTH[1] < 48 && GPRS_STATUS.SG_STNTH[1] < 57)
                            GPRS_STATUS.SG_STNTH[1] = 0;
                    }
                }

                else if ((strstr((char*)buf, "+CREG: ")) != NULL)
                {
                    GPRS_STATUS.CREG = 1;
                }
                else if ((strstr((char*)buf, "+CGREG: ")) != NULL)
                {
                    GPRS_STATUS.CGREG = 1;
                }
                else if ((strstr((char*)buf, "+CGATT: 1")) != NULL)
                {
                    GPRS_STATUS.CGATT = 1;
                }

                else if ((strstr((char*)buf, "+CGATT: 0")) != NULL)
                {
                    GPRS_STATUS.CGATT_FAIL = 1;
                }

                else if((strstr((char*)vUARTMRXFIFO, "+CCLK:")) != NULL)
                {
                    memset(Ntime_buf,0,sizeof(Ntime_buf));

                    char *ptr;
                    ptr = strstr((char*)vUARTMRXFIFO, "+CCLK:");
                    if(ptr!= NULL)
                    {
                        ptr = ptr + 8;
                        i = 0;
                        while(*ptr)
                        {
                            Ntime_buf[i++] = *ptr++;
                        }
                    }
                    Ntime_buf[i] ='\0';
                }
                else if((strstr((char*)buf, "+QNITZ:")) != NULL)
                {
                    memset(Btime_buf,0,sizeof(Btime_buf));

                    char *ptr;
                    ptr = strstr((char*)buf, "+QNITZ:");
                    if(ptr!= NULL)
                    {
                        ptr = ptr + 9;
                        i = 0;
                        while(*ptr)
                        {
                            Btime_buf[i++] = *ptr++;
                        }
                    
                        Btime_buf[--i] ='\0';
                        Btime_buf[--i] ='\0';
                        Btime_buf[--i] ='\0';
                        Btime_buf[--i] ='\0';
                        Btime_buf[--i] ='\0';
                        Btime_buf[--i] ='\0';
                        //Set_NetRTC_Dt_Time();
                        
                    }
                }
               
               else if((strstr((char*)buf, "+QLTS:")) != NULL)
                {
                    memset(Btime_buf,0,sizeof(Btime_buf));

                    char *ptr;
                    ptr = strstr((char*)buf, "+QLTS:");
                    if(ptr!= NULL)
                    {
                        ptr = ptr + 8;
                        i = 0;
                        while(*ptr)
                        {
                            Btime_buf[i++] = *ptr++;
                        }
                    
                        Btime_buf[--i] ='\0';
                        Btime_buf[--i] ='\0';
                        Btime_buf[--i] ='\0';
                        Btime_buf[--i] ='\0';
                        Btime_buf[--i] ='\0';
                        //Btime_buf[--i] ='\0';
                        
                        //Set_NetRTC_Dt_Time();
                        
                    }
                }

                else if ((strstr((char*)buf, "SEND OK")) != NULL)
                {
                    if(cMODEM_STATE == MODEM_GREATER)
                    {
                        MODEM_STATE = MODEM_GREATER_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }
                }

                else if ((strstr((char*)buf, "CLOSED")) != NULL)
                {
                        MODEM_STATE = MODEM_PRE_QICLOSE_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                }
                else if ((strstr((char*)buf, "+CMS ERROR:")) != NULL)
                {
                        MODEM_STATE = MODEM_STANDBY;
                        GPRS_STATUS.CMS_ERROR = 1;
                        DsModemTimerInterrupt();
                }

                else if ((strstr((char*)buf, "CONNECT FAIL")) != NULL)
                {
                    //Added recently
                    MODEM_STATE = MODEM_QICLOSE_RESP;
                    DsModemTimerInterrupt();
                    ModemCallBack();
                }
                else if ((strstr((char*)buf, "CONNECT")) != NULL)
                {
                    MODEM_STATE = MODEM_QIOPEN_RESP;
                    DsModemTimerInterrupt();
                    ModemCallBack();
                }
                else if ((strstr((char*)buf, "CONNECT OK")) != NULL)
                {
                    MODEM_STATE = MODEM_QIOPEN_RESP;
                    DsModemTimerInterrupt();
                    ModemCallBack();
                }
                else if ((strstr((char*)buf, "ALREADY CONNECT")) != NULL)
                {
                    MODEM_STATE = MODEM_QIOPEN_RESP;
                    DsModemTimerInterrupt();
                    ModemCallBack();
                }
                else if ((strstr((char*)buf, "CONNECT FAIL")) != NULL)
                {
                    MODEM_STATE = MODEM_TIMEOUT;
                    DsModemTimerInterrupt();
                }
                else if ((strstr((char*)buf, "ERROR")) != NULL)
                {
                    if(cMODEM_STATE == MODEM_QIOPEN)
                    {
                        GetServerAddress();
                    }

                    else if(cMODEM_STATE == MODEM_QICLOSE)
                    {
                        MODEM_STATE = MODEM_PRE_QICLOSE_RESP;;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }
                    
                    else
                    MODEM_STATE = MODEM_TIMEOUT;
                    DsModemTimerInterrupt();
                }

                else if ((strstr((char*)buf, "CLOSE OK")) != NULL)
                {
                    if(cMODEM_STATE == MODEM_QICLOSE)
                    {
                        MODEM_STATE = MODEM_QICLOSE_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }
                }
                else if ((strstr((char*)buf, "DEACT OK")) != NULL)
                {
                    if(cMODEM_STATE == MODEM_QIDEACT)
                    {
                        MODEM_STATE = MODEM_QIDEACT_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }
                }
                else if ((strstr((char*)buf, "+QINISTAT: 3")) != NULL)
                {
                    GPRS_STATUS.QINISTAT = 1;
                }
                else if ((strstr((char*)buf, "+CMGS: ")) != NULL)
                {
                    MODEM_STATE = MODEM_CMGS_TEXT_RESP;
                    DsModemTimerInterrupt();
                    ModemCallBack();
                }

                else if((strstr((char*)buf, "NORMAL POWER DOWN")) != NULL)
                {
                    if(cMODEM_STATE == MODEM_TURNOFF)
                    {
                        MODEM_STATE = MODEM_TURNOFF_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }
                }

                if ((strstr((char*)buf, "OK")) != NULL)
                {
                    if(cMODEM_STATE == MODEM_ATE)
                    {
                        MODEM_STATE = MODEM_ATE_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_DCGATT)
                    {
                        MODEM_STATE = MODEM_DCGATT_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                     if(cMODEM_STATE == MODEM_CMGF)
                    {
                        MODEM_STATE = MODEM_CMGF_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_CMGR &&
                            GPRS_STATUS.CMGR == 1)
                    {
                        GPRS_STATUS.CMGR = 0;
                        MODEM_STATE = MODEM_CMGR_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_DEL_ALL)
                    {
                        MODEM_STATE = MODEM_DEL_ALL_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_DEL_READ)
                    {
                        MODEM_STATE = MODEM_DEL_READ_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_CSQ)
                    {
                        MODEM_STATE = MODEM_CSQ_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    #ifdef AT_CSCS
                    if(cMODEM_STATE == MODEM_CSCS)
                    {
                        MODEM_STATE = MODEM_CSCS_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }
                    #endif

                    if(cMODEM_STATE == MODEM_BASE_CSQ)
                    {
                        MODEM_STATE = MODEM_BASE_CSQ_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_QSPN &&
                            GPRS_STATUS.QSPN == 1)
                    {
                        GPRS_STATUS.QSPN = 0;
                        MODEM_STATE = MODEM_QSPN_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_QNITZ_EN)
                    {
                        MODEM_STATE = MODEM_QNITZ_EN_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }
                    if(cMODEM_STATE == MODEM_QLTS_EN)
                    {
                        MODEM_STATE = MODEM_QLTS_EN_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_SET_CCLK)
                    {
                        MODEM_STATE = MODEM_SET_CCLK_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_GET_CCLK)
                    {
                        MODEM_STATE = MODEM_GET_CCLK_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_CFUN)
                    {
                        MODEM_STATE = MODEM_CFUN_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }
                    if(cMODEM_STATE == MODEM_CREG)
                    {
                        if(GPRS_STATUS.CREG)
                        {
                            GPRS_STATUS.CREG = 0;
                            MODEM_STATE = MODEM_CREG_RESP;
                            DsModemTimerInterrupt();
                            ModemCallBack();
                        }
                    }

                    if(cMODEM_STATE == MODEM_CGREG)
                    {
                        if(GPRS_STATUS.CGREG)
                        {
                            GPRS_STATUS.CGREG = 0;
                            MODEM_STATE = MODEM_CGREG_RESP;
                            DsModemTimerInterrupt();
                            ModemCallBack();
                        }
                    }

                    if(cMODEM_STATE == MODEM_CGATT)
                    {
                        if(GPRS_STATUS.CGATT)
                        {
                            GPRS_STATUS.CGATT = 0;
                            MODEM_STATE = MODEM_CGATT_RESP;
                            DsModemTimerInterrupt();
                            ModemCallBack();
                        }
                    }

                    if(cMODEM_STATE == MODEM_QINISTAT)
                    {
                        MODEM_STATE = MODEM_QINISTAT_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_CMGS_TEXT)
                    {
                        MODEM_STATE = MODEM_CMGS_TEXT_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_QIMUX)
                    {
                        MODEM_STATE = MODEM_QIMUX_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_QIMODE)
                    {
                        MODEM_STATE = MODEM_QIMODE_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_QIFGCNT)
                    {
                        MODEM_STATE = MODEM_QIFGCNT_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_QICSGP)
                    {
                        MODEM_STATE = MODEM_QICSGP_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_QIREGAPP)
                    {
                        MODEM_STATE = MODEM_QIREGAPP_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_TRIPPLE_PLUS)
                    {
                        MODEM_STATE = MODEM_TRIPPLE_PLUS_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_QIACT)
                    {
                        MODEM_STATE = MODEM_QIACT_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_QIDNSIP)
                    {
                        MODEM_STATE = MODEM_QIDNSIP_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(cMODEM_STATE == MODEM_QITCFG)
                    {
                        MODEM_STATE = MODEM_QITCFG_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }

                    if(MODEM_STATUS == MODEM_POWER_CYCLE)
                    {
                        MODEM_STATE = MODEM_POWERUP;
                    }
                    else if(MODEM_STATUS == MODEM_NOT_INITIALIZED)
                    {
                        MODEM_STATE = MODEM_AT_RESP;
                        DsModemTimerInterrupt();
                        ModemCallBack();
                    }
                }

                if(mUARTRX_STATUS == EOS_PROCESS)
                {
                    mUARTRX_STATUS = EOS_WAIT;
                }
            }
        }
    }
}


#ifdef  EN_NETWORK_TIME
void Set_NetRTC_Dt_Time(void)
{
    unsigned int hrs[2], mins[2], date[2];
    unsigned int Date, Mins, Hrs;

    // ASCII to Int conversion
    date[1] = Btime_buf[6] - '0';
    date[0] = Btime_buf[7] - '0';

    hrs[1] = Btime_buf[9]  - '0';
    hrs[0] = Btime_buf[10] - '0';

    mins[1] = Btime_buf[12] - '0';
    mins[0] = Btime_buf[13] - '0';

    // BCD to int conversion
    mins[1] = mins[1] * 10;
    Mins = mins[1] + mins[0] ;

    hrs[1] = hrs[1] * 10;
    Hrs = hrs[1] + hrs[0] ;

    date[1] = date[1] * 10;
    Date = date[1] + date[0] ;

    Hrs = Hrs + 05;
    Mins = Mins + 30;

    if(Mins > 59)
    {
        Mins = Mins - 60;
        Hrs  =  Hrs + 1;
    }

    if(Hrs > 23)
    {
        Hrs  = Hrs - 24;
        Date = Date + 1;
    }

    NRTS.Month  = ASCII_TO_HEX(Btime_buf[3],Btime_buf[4]);
    NRTS.Year   = ASCII_TO_HEX(Btime_buf[0],Btime_buf[1]);
    NRTS.Second = ASCII_TO_HEX(Btime_buf[15],Btime_buf[16]);
    
    ReadRTC(&DATE_TIME);

    if((DATE_TIME.Mins != NRTS.Minute) ||(DATE_TIME.Hour != NRTS.Hour)||
       (DATE_TIME.Date != NRTS.Date) ||(DATE_TIME.Month != NRTS.Month) ||(DATE_TIME.Year.Val != NRTS.Year))
    {
        NRTS.Minute = Uint2Bcd(Mins);
        NRTS.Hour   = Uint2Bcd(Hrs);
        NRTS.Date   = Uint2Bcd(Date);

        RTC_Pre_Config();
        SetRtcDate(NRTS.Date, NRTS.Month, NRTS.Year);
        SetRtcTime(NRTS.Second , NRTS.Minute, NRTS.Hour);
        RTC_Post_Config();
    }

     memset(Btime_buf, 0, 30);
}
#endif



void ResetModemTimer(void)
{
    ClrModemTimerInterrupt();
    OpenModemTimer();
    EnModemTimerInterrupt();
}

void CheckEndOfRxBuffer(void)
{
    if(MRXTailPtr >= vUARTMRXFIFO + sizeof(vUARTMRXFIFO))
    {
        MRXTailPtr = vUARTMRXFIFO;
    }
}

UINT8 PharseServerTime(char* buf, UINT8 Format)
{
    //Format is 0 for GMT and 1 for IST
    
    BYTE Months[12][5] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    BYTE Weekday[7][5] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    unsigned int monthDays[12] = {31,28,31,30,31,31,31,31,30,31,30,31};
    char temp[5] = "\0";
    unsigned int i;
    unsigned int week_day = 0, date, month = 0, year, hour, min;
    
    temp[0] = toupper(buf[0]);
    temp[1] = buf[1];
    temp[2] = buf[2];
    temp[3] = '\0';
    for (i = 0 ; i < 7 ; i++)
    {
        if(strstr((char*)Weekday[i],(const char*)temp) != NULL)
        {
            week_day = i+1;
            break;
        }
    }
    
    temp[0] = buf[5];
    temp[1] = buf[6];
    temp[2] = '\0';
    date = atoi(temp);
    
    temp[0] = toupper(buf[8]);
    temp[1] = buf[9];
    temp[2] = buf[10];
    temp[3] = '\0';
    for (i = 0 ; i < 12 ; i++)
    {
        if( strstr((char*)Months[i],(const char*)temp) != NULL)
        {
            month = i+1;
            break;
        }
    }
    
    temp[0] = buf[12];
    temp[1] = buf[13];
    temp[2] = buf[14];
    temp[3] = buf[15];
    temp[4] = '\0';
    year = atoi(temp);
    
    temp[0] = buf[17];
    temp[1] = buf[18];
    temp[2] = '\0';
    hour = atoi(temp);
    
    temp[0] = buf[20];
    temp[1] = buf[21];
    temp[2] = '\0';
    min = atoi(temp);
    
    temp[0] = buf[23];
    temp[1] = buf[24];
    temp[2] = '\0';
    DATE_TIME.Seconds = atoi(temp);
    
    if ((year == 0) || (week_day == 0) || (date == 0) || (month == 0))
    {
        return 0;
    }
    
    /*if (year < 2000)
    {
        year += 2000;
    }*/
    
    if(Format == 0)
    {
        min += 30;
        hour += 5;
    
        if(min > 59)
        {
            min %= 60;
            hour ++;
        }

        if(hour > 23)
        {
            hour %= 24;
            date ++;
            week_day ++;
            if (week_day > 7)
            {
                week_day %= 7;
            }
        }
    
        if(date > monthDays[month-1])
        {
            if(((year%4) == 0) && (((year%100) != 0)||((year%400) == 0)) && (month == 2))
            {
                if(date > 29)
                {
                    date %= 29;
                    month++;
                }
            }
            else
            {
                date %= monthDays[month-1];
                month++;
            }
        }
    
        if(month > 12)
        {
            year ++;
            month %=12;
        }
    }
    
    DATE_TIME.Week_Day = week_day;
    DATE_TIME.Mins = min;
    DATE_TIME.Hour = hour;
    DATE_TIME.Date = date;
    DATE_TIME.Month = month;
    DATE_TIME.Year.Val = year;
    return 1;
}

/*********************************************************************
 * Function:        void _ModemTXInterrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies bytes to and from the local UART TX and
 *					RX FIFOs
 *
 * Note:            None
 ********************************************************************/
void _ModemTXInterrupt(void)
{
    // Transmit a byte, if pending, if possible
    if(MTXHeadPtr != MTXTailPtr)
    {
        // Clear the TX interrupt flag before transmitting again
        IFS0bits.U1TXIF = 0;

        U1TXREG = *MTXTailPtr++;
        if(MTXTailPtr >= vUARTMTXFIFO + sizeof(vUARTMTXFIFO))
            MTXTailPtr = vUARTMTXFIFO;
    }
    else	// Disable the TX interrupt if we are done so that we don't keep entering this ISR
    {
        IEC0bits.U1TXIE = 0;
    }
}


void __attribute__((__interrupt__,no_auto_psv)) _T3Interrupt(void)
{
    // interrupt received from TIMER
    DsModemTimerInterrupt();

    if(cMODEM_STATE == MODEM_GREATER)
        MODEM_STATE = MODEM_STIMEOUT;

    else if(MODEM_STATE == MODEM_DLY_WAIT)
        MODEM_STATE = nMODEM_STATE;
    else
        // oops,timeout... switch to IDLE and notify RX failure
        MODEM_STATE = MODEM_TIMEOUT;

    ClrModemTimerInterrupt();
}

void __attribute__((__interrupt__,no_auto_psv)) _U1TXInterrupt(void)
{

   _ModemTXInterrupt();
}

void __attribute__((__interrupt__,no_auto_psv)) _U1ErrInterrupt(void)
{
    Nop();
    Nop();
}

void __attribute__((__interrupt__,auto_psv)) _U1RXInterrupt(void)
{

    /* Check for receive errors */
    if(U1STAbits.FERR == 1)
    {
    }
    /* Must clear the overrun error to keep UART receiving */
    if(U1STAbits.OERR == 1)
    {
        U1STAbits.OERR = 0;
    }
    _ModemRXInterrupt();
}



