
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "cTimer.h"
#include <apiModbus.h>
#include <libModbus.h>
#include <apiHelpers.h>


//#define SEND_RESP_MANUALLY

extern _FLAGS FLAGS;
MDB_REQ_ADU mdbReqAdu;
MDB_REQ_ADU MbReqAscii;
MDB_REQ_ADU MdbReqBuffer;
MDB_RSP_ADU mdbRspAdu;
MDB_RSP_ADU MdbRespBuffer;

extern volatile int mBufCount;
extern unsigned char mdbUARTRxBuffer[512];


char TimeErrorFlag;
extern void WriteCharToUART(unsigned char CharValue);

/***************************************************************************************************************************************
****************************************************************************************************************************************
	Description:
	This method is the main function that drives Modbus communication on a network.
	Parameters:
	a) networkNo (IN): A number identifying the ?port? on which Modbus communication has to happen.
	b) slaveNo (IN): A single byte value containing the slave ID of the device from which data is being requested.
	c) functionCode (IN): A single byte value of the Modbus function code that defines the Modbus service request.
	d) startAddress (IN): A two-byte value that is the first address in the range of data being requested for.
	e) numItems (IN): A two-byte value that is the number of data items starting from startAddress that are being requested for.
	f) dataBuffer -> (OUT): Pointer to an array of bytes into which the requested data must be copied into in the correct format for 'Read' FCs.
				  -> (IN): Pointer to an array of bytes containing the data that has to be 'written' to slave.
	g) numRetries (IN): The number of times to retry communication with slave.

	Returns:
	Status Code.

	For Ex:

	ModbusOP(0x02, 0x03, 0x0F49, 0x0002, (unsigned char *)HoldingReg, 15);

	NUMITEMS  SHOWS THAT NUMBER OF REGISTER ARE TOBE READ FROM THAT ADDRESS FROM 1 REGISTER TWO BYTES ARE STORED

**************************************************************************************************************************************
***************************************************************************************************************************************/

volatile UCHAR8 statusCode;

UCHAR8 ModbusOP(UCHAR8 slaveNo, UCHAR8 functionCode, UINT16 startAddress, UCHAR8 numItems, UCHAR8 *dataBuffer, UCHAR8 numRetries)
{
    UINT16 noOfBytesToTx, noOfBytesToRx;
    UCHAR8 retryNo = 0;
    statusCode = MMPL_NO_ERROR;

    memset(&MdbReqBuffer.TxBuffer,0,sizeof(MdbReqBuffer.TxBuffer));
    memset(&MdbRespBuffer.RxBuffer,0,sizeof(MdbRespBuffer.RxBuffer));

    statusCode = ConstructRequest(slaveNo, functionCode, startAddress, numItems,dataBuffer, &mdbReqAdu, &mdbRspAdu);

    if(statusCode == MMPL_NO_ERROR)
    {
        /* Calculate size of request frame to be transmitted to the slave */
        noOfBytesToTx = (mdbReqAdu.pduSize + 2 + 2)  /* LRC 2 byte */  /* 2 bytes \r and \n */;
        
        do
        {
            /**********************************************************************************************************************************
                                    Now the Transmitter Buffer is completed which contains the

                                    1. Slave Number				It depends upon the Slave Every Slave has unique number
                                    2. Function Code			It depends upon user which register wants to read & write
                                    3. Data (0 to 256 Bytes)    How many Register user to read or write Minimum value is 2 ( 0x02 indicates that user want to access only 2 register it can be 0xff
                                    4. CRC Value				This values already generated in code on salve side it use to be verified

            **************************************************************************************************************************************/
            mBufCount = 0;

            noOfBytesToRx = (MdbRespBuffer.pduSize+1+2+2+2); /* Header+Slave address+LRC+CRLF */

            WritePacket(noOfBytesToTx);

            /***************************************************************************************************************
                    Proceed to read response only for non-broadcast address & Read the response ADU from the slave
            ************************************************************************ ***************************************/
            ReadPacket((UCHAR8*)mdbRspAdu.RxBuffer, noOfBytesToRx);
            statusCode = DecodeResponse(&mdbReqAdu, &mdbRspAdu, dataBuffer, numItems);

            retryNo++;
         }
         while((retryNo<=numRetries) && (statusCode!=MMPL_NO_ERROR) && (statusCode!=EXCEPTION_RESPONSE));

    }
    return statusCode;
}

/*****************************************************************************************************************************************
******************************************************************************************************************************************
	Description:
	This method constructs the Modbus request ADU that has to be sent to the slave.
	Parameters:
	a) slaveNo (IN): A single byte value containing the slave ID of the device from which data is being requested.
	b) functionCode (IN): A single byte value of the Modbus function code that defines the Modbus service request.
	c) startAddress (IN): A two-byte value that is the first address in the range of data being requested for.
	d) numItems (IN): A two-byte value that is the number of data items starting from startAddress that are being requested for.
	e) dataBuffer -> (OUT): Pointer to an array of bytes into which the requested data must be copied into in the correct format for 'Read' FCs.
				  -> (IN): Pointer to an array of bytes containing the data that has to be 'written' to slave.
	f) pMbReqAscii (OUT): Pointer to a structure where the request ADU has to be stored.
	g) pMbRspAscii (OUT): Pointer to a structure of the response ADU (response PDU size is estimated in some cases).

	Returns:
	Status Code.

	PDU Size = FUNCTION CODE + START ADDRESS + NUMITEMS
	PDU SIZE  = 1 + 2 + 2	SO Always Fixed Size.......

	Role of this Function:

	1. This function is used to fill the transmit packet
	2. Transmit packet contains the Slave number, Function Code, Num of Items
	3. MbReqAscii->TxBuffer Fill the transmit buffer
	4. From the Num of items to be read it decides the response Pdu size

					byteCount  = (UCHAR8)(numItems * 2);	// byte count
					MbRspAscii->pduSize = byteCount + 2; 		// Slave number + FC + ByteCnt  1+1+4

*****************************************************************************************************************************************
*******************************************************************************************************************************************/

UCHAR8 ConstructRequest(UCHAR8 slaveNo, UCHAR8 functionCode, UINT16 startAddress, UCHAR8 numItems, UCHAR8 *dataBuffer,MDB_REQ_ADU *MbReqAscii, MDB_RSP_ADU *MbRspAscii)
{
    UINT16_VAL cRes;
    UCHAR8 byteCount,cbyteCount = 0;
    UINT8 TxbufIndex = 0,cTxbufIndex = 0;

    
    UINT16 NumItems  = numItems;
    statusCode = MMPL_NO_ERROR;
    /********************** Check the Slave number It should not be grater than 247 & less than 0 *******************/

    if(! ((slaveNo >= 0)&&(slaveNo <= 247)) )
    {
        statusCode = INVALID_SLAVE_ADDR;
    }

    /*********************** Check the What type of Function Code used in Modbus transaction************************
    ***********************	 Fill the buffer which will be transmitted over MOdbus	********************************
    ***********************  TxBuffer Contains the AddressField + Function Code + Data + CRC************************/

    if( statusCode == MMPL_NO_ERROR )
    {
        switch(functionCode)
        {
            case FC_READ_HOLD_REGS:

                if( statusCode == MMPL_NO_ERROR )
                {
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  ':';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = ':';

                    cRes = Hex2Ascii_Frame(slaveNo);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = slaveNo;

                    cRes = Hex2Ascii_Frame(functionCode);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = functionCode;

                    cRes = Hex2Ascii_Frame(((startAddress) & 0xFF00)>>8);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = ((startAddress) & 0xFF00)>>8;


                    cRes = Hex2Ascii_Frame((startAddress) & 0x00FF);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = (startAddress) & 0x00FF;

                    cRes = Hex2Ascii_Frame((NumItems & 0xFF00)>>8);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = (NumItems & 0xFF00)>>8;

                    cRes = Hex2Ascii_Frame((NumItems & 0x00FF));
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = (NumItems & 0x00FF);

                    MbReqAscii->pduSize = TxbufIndex;                     // Fill the PDU Size
                    MdbReqBuffer.pduSize = cTxbufIndex;
                    AppendLrc(MbReqAscii,MbReqAscii->pduSize);		// LRC Check
                    cTxbufIndex++;
                    TxbufIndex++;
                    TxbufIndex++; //2 bytes for LRC

                    
                    MbReqAscii->TxBuffer[TxbufIndex++]  =  '\r'; //added by krishna
                    MbReqAscii->TxBuffer[TxbufIndex++]  =  '\n'; //added by krishna
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = '\r';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = '\n';

                    /************ for FC_READ_HOLD_REGS, FC_READ_IP_REGS **************/
                    byteCount  = (UCHAR8)(NumItems * 2);                // byte count
                    cbyteCount = (UCHAR8)(NumItems * 2);
                    MbRspAscii->pduSize = ((byteCount + 1 + 1) * 2); 	// Bytecount + FC + ByteCnt(Each byte is 2  ASCII chars)
                    MdbRespBuffer.pduSize = (cbyteCount + 1 + 1); 	// Bytecount + FC + ByteCnt
                }
            break;

            case FC_WRITE_SINGLE_REG:

                if( statusCode == MMPL_NO_ERROR )
                {
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  ':';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = ':';

                    cRes = Hex2Ascii_Frame(slaveNo);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = slaveNo;

                    cRes = Hex2Ascii_Frame(functionCode);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = functionCode;

                    cRes = Hex2Ascii_Frame(((startAddress) & 0xFF00)>>8);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = ((startAddress) & 0xFF00)>>8;


                    cRes = Hex2Ascii_Frame((startAddress) & 0x00FF);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = (startAddress) & 0x00FF;

                    MdbReqBuffer.pduSize = cTxbufIndex;// get index to copy data in below function
                    HexToBuffer((UCHAR8 *)dataBuffer, &(MbReqAscii->TxBuffer[TxbufIndex]), 1);
                    
                    TxbufIndex++;   //for Data bytes
                    TxbufIndex++;
                    TxbufIndex++;
                    TxbufIndex++;
                    cTxbufIndex++;
                    cTxbufIndex++;

                    MbReqAscii->pduSize = TxbufIndex;                             // Append LRC byte request frame
                    MdbReqBuffer.pduSize = cTxbufIndex;
                    
                    AppendLrc(MbReqAscii,MbReqAscii->pduSize);			// Estimate the response PDU size

                    cTxbufIndex++;
                    TxbufIndex++;
                    TxbufIndex++; //2 bytes for LRC

                    MbReqAscii->TxBuffer[TxbufIndex++]  =  '\r';
                    MbReqAscii->TxBuffer[TxbufIndex++]  =  '\n';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = '\r';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = '\n';

                    /*Compute Response bytes*/
                    MbRspAscii->pduSize = (2+1+2)*2; 		//2 bytes  written + FC + Reg Address(Each byte is 2 ASCII chars)
                    MdbRespBuffer.pduSize = 2+1+2;             //2 bytes  written  + FC + Reg Address
                }
            break;
            
            case FC_WRITE_SINGLE_COIL:

                if( statusCode == MMPL_NO_ERROR )
                {
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  ':';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = ':';

                    cRes = Hex2Ascii_Frame(slaveNo);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = slaveNo;

                    cRes = Hex2Ascii_Frame(functionCode);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = functionCode;

                    cRes = Hex2Ascii_Frame(((startAddress) & 0xFF00)>>8);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = ((startAddress) & 0xFF00)>>8;


                    cRes = Hex2Ascii_Frame((startAddress) & 0x00FF);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = (startAddress) & 0x00FF;

                    MdbReqBuffer.pduSize = cTxbufIndex;// get index to copy data in below function
                    HexToBuffer((UCHAR8 *)dataBuffer, &(MbReqAscii->TxBuffer[TxbufIndex]), 1);
                    
                    TxbufIndex++;   //for Data bytes
                    TxbufIndex++;
                    TxbufIndex++;
                    TxbufIndex++;
                    cTxbufIndex++;
                    cTxbufIndex++;

                    MbReqAscii->pduSize = TxbufIndex;                             // Append LRC byte request frame
                    MdbReqBuffer.pduSize = cTxbufIndex;
                    
                    AppendLrc(MbReqAscii,MbReqAscii->pduSize);			// Estimate the response PDU size

                    cTxbufIndex++;
                    TxbufIndex++;
                    TxbufIndex++; //2 bytes for LRC

                    MbReqAscii->TxBuffer[TxbufIndex++]  =  '\r';
                    MbReqAscii->TxBuffer[TxbufIndex++]  =  '\n';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = '\r';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = '\n';

                    /*Compute Response bytes*/
                    MbRspAscii->pduSize = (2+1+2)*2; 		//2 bytes  written + FC + Reg Address(Each byte is 2 ASCII chars)
                    MdbRespBuffer.pduSize = 2+1+2;             //2 bytes  written  + FC + Reg Address
                
                }
            break;

            case FC_WRITE_MULTIPLE_REGS:
																	// for Write Multiple Regs FC
                if( !((numItems >= 1) && (numItems <= WR_BLK_SIZE_REGINFO)) )
                {
                    statusCode = INVALID_NUM_ITEMS;
                }

                if( statusCode == MMPL_NO_ERROR )			
                {
        						
                    byteCount  = (UCHAR8)(numItems * 2);		// byte count

                    MbReqAscii->TxBuffer[TxbufIndex++]   =  ':';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = ':';

                    cRes = Hex2Ascii_Frame(slaveNo);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = slaveNo;

                    cRes = Hex2Ascii_Frame(functionCode);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = functionCode;

                    cRes = Hex2Ascii_Frame(((startAddress) & 0xFF00)>>8);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = ((startAddress) & 0xFF00)>>8;

                    cRes = Hex2Ascii_Frame((startAddress) & 0x00FF);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = (startAddress) & 0x00FF;

                    cRes = Hex2Ascii_Frame((NumItems & 0xFF00)>>8);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = (NumItems & 0xFF00)>>8;

                    cRes = Hex2Ascii_Frame((NumItems & 0x00FF));
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = (NumItems & 0x00FF);

                    cRes = Hex2Ascii_Frame(byteCount);
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.HB;
                    MbReqAscii->TxBuffer[TxbufIndex++]   =  cRes.byte.LB;

                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = byteCount; //these two inedxs shouls be same
                    //MdbReqBuffer.TxBuffer[INDEX_WRITE_BYTE_CNT]    = byteCount; //need to check this index

                    MdbReqBuffer.pduSize = cTxbufIndex;                            // get index to copy data in below function
                    HexToBuffer((UCHAR8 *)dataBuffer, &(MbReqAscii->TxBuffer[TxbufIndex]), numItems);

                    TxbufIndex = TxbufIndex + (numItems*2*2); //each REG will have 2 bytes(16bit wide)& each byte will have 2 ASCII chars
                    cTxbufIndex = cTxbufIndex + (numItems*2);

                    MbReqAscii->pduSize = TxbufIndex;
                    MdbReqBuffer.pduSize = cTxbufIndex;

                    AppendLrc(MbReqAscii,MbReqAscii->pduSize);

                    cTxbufIndex++;
                    TxbufIndex++;
                    TxbufIndex++; //2 bytes for LRC

                    MbReqAscii->TxBuffer[TxbufIndex++]  =  '\r';
                    MbReqAscii->TxBuffer[TxbufIndex++]  =  '\n';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = '\r';
                    MdbReqBuffer.TxBuffer[cTxbufIndex++] = '\n';

                    /*Compute Response bytes*/
                    MbRspAscii->pduSize = (1+2+2)*2; 	// FC + start Reg Address + Quantity of regs written(Each byte is 2 c ASCII chars)
                    MdbRespBuffer.pduSize = (1+2+2);     // FC + start Reg Address + Quantity of regs written
                }
            break;

            default:
                statusCode = INVALID_FC;
            break;
        }
    }
    return statusCode;
}

/*****************************************************************************************************************************************
******************************************************************************************************************************************************
	Description:
	This method should open communication port and initialise it so as to get it ready for receiving Modbus packets and sending responses.
	This is the place to set all communication parameters like baud rate, parity, port timeouts etc. This function is not internally
	called by the library but must be called by the user during start up of his application, once for each port that will support
	Modbus communication.
	Parameters:
	a) networkNo (IN): A number identifying the ?port? used for Modbus communication that is to be initialised.
	Returns:
	A value indicating if the specified port was opened and initialised successfully or not.
	TRUE ? The specified port was opened and initialised successfully.
	FALSE ? The specified port could not be opened or initialised.
******************************************************************************************************************************************************
*****************************************************************************************************************************************************/


/******************************************************************************************************************************************************
*****************************************************************************************************************************************************
	Description:
	This method is called by the library when it requires writing data bytes to a communication port. If less than the requested number of bytes
	could be written before timeout occurs, then the function should return TRUE and set pNoOfBytesWritten to the actual number of bytes
	written.
	Parameters:
	a) networkNo (IN): 				A number identifying the ?port? to be written to.
	b) noOfBytesToWrite(IN): 		The number of bytes to write on this port.
	c) pNoOfBytesWritten(OUT): 		A pointer to the variable that receives the actual number of bytes written.
	d) pBuffer (IN): 				A pointer to the buffer containing the data to be written to the port.
	e) pErrorCode (OUT): 			A pointer to the variable that receives an error code in case of failure of this function.
	Returns:
	TRUE if the function succeeds, else FALSE. If the return value is FALSE, then an error code indicating
	the reason for failure should be stored in the pErrorCode parameter.
******************************************************************************************************************************************************
*****************************************************************************************************************************************************/

BOOL WritePacket(UINT16 noOfBytesToWrite)
{
    UINT8 i;

    IEC1bits.U2RXIE = 0;

    for(i=0; i < noOfBytesToWrite; i++)
    {
        WriteCharToUART(mdbReqAdu.TxBuffer[i]);
    }
    IEC1bits.U2RXIE = 1;

    return 1;
}

/*****************************************************************************************************************************************************
*****************************************************************************************************************************************************
	Description:
	This is a helper function which attempts to read the requested number of bytes from the communication port by
	repeatedly calling ReadPort until all bytes are read or a loopguard expires.
	Parameters:
	a) networkNo (IN)	: A number identifying the ?port? from which the characters are to be read.
	b) TxBuffer (OUT)	: Buffer to store the bytes read from the communication port.
	c) noOfBytesToRead (IN): The number of bytes to read from the port.

	Returns:
	Status Code.
		a) MMPL_NO_ERROR (0x00)		: The read was successful
		b) READ_WRITE_TIMEOUT (0x05): The loop guard expired before the requested number of bytes could be read.
		c) An error code returned by ReadPort if it returned with anything other than MMPL_NO_ERROR
******************************************************************************************************************************************************
*****************************************************************************************************************************************************/

void ReadPacket(UCHAR8 *pktBuffer, UINT16 noOfBytesToRead)
{
    UINT16 noOfBytesReadThisCall;
    UCHAR8 errorCode = MMPL_NO_ERROR;	/* assume the attempt be successful */
    ReadPort(noOfBytesToRead, &noOfBytesReadThisCall, pktBuffer, &errorCode);
}

/*****************************************************************************************************************************************************
*****************************************************************************************************************************************************
	Description:
	This method is called by the library when it requires reading data bytes from a communication port. If less than the requested number of
	bytes could be read before timeout occurs, then the function should return TRUE and set pNoOfBytesRead to the actual number of bytes read.
	If the number of bytes read is zero even after timeout occurs, then the function should return FALSE.
	Parameters:
	a) networkNo (IN): 			A number identifying the ?port? to be read.
	b) noOfBytesToRead (IN): 	The number of bytes to read on this port.
	c) pNoOfBytesRead (OUT): 	A pointer to the variable that receives the actual number of bytes read.
	d) pBuffer (OUT): 			A pointer to the buffer that receives the data read from the port.
	e) pErrorCode (OUT): 		A pointer to the variable that receives an error code in case of failure of this function.
	Returns:
	TRUE if the function succeeds, else FALSE. If the return value is FALSE, then an error code indicating
	the reason for failure should be stored in the pErrorCode parameter.
******************************************************************************************************************************************************
*****************************************************************************************************************************************************/

#define MAX_DELAY 0xEFFFFF
BOOL ReadPort(UINT16 noOfBytesToRead, UINT16 *pNoOfBytesRead, UCHAR8 *pBuffer, UCHAR8 *pErrorCode){

    BYTE tBuff[10];
    UINT16 RxCtr,tRxbytes = 0,Rxbytes = 0;
    
    RxCtr = 0;
    mBufCount = 0;
    
    #ifndef SEND_RESP_MANUALLY
    MdbRespTimeout = MDB_RESPONSE_TIMEOUT;
    while(MdbRespTimeout && !FLAGS.UART_RxDone);
    #else
    while(!FLAGS.UART_RxDone);
    #endif
   
     /* To avoid copying \r and \n */
    if((strstr((char*)mdbUARTRxBuffer, "\r\n")))
    {
        noOfBytesToRead = ((noOfBytesToRead-1)*2);
    }

    else
    {
         /* To avoid copying only \r */
        noOfBytesToRead = ((noOfBytesToRead-1)*2);
    }
    
    if((!MdbRespTimeout)&&(FLAGS.UART_RxDone == 0))
    {
        TimeErrorFlag=1;
        *pErrorCode = READ_WRITE_FAIL;
        return 0;

    }
    else
    {
        FLAGS.UART_RxDone = 0;
        TimeErrorFlag=0;
        MdbRespTimeout = 0;
        // Fill The buffer From UART ......................
        do
        {
            *(pBuffer) = mdbUARTRxBuffer[RxCtr];

            //To avoid conversion for first byte
            if(RxCtr >=1)
            {
                tRxbytes++;
                if(tRxbytes <2 )
                {
                    tBuff[0] = (mdbUARTRxBuffer[RxCtr]);//added
                }

                if(tRxbytes >=2)
                {
                    tBuff[1] = (mdbUARTRxBuffer[RxCtr]);//added

                    MdbRespBuffer.RxBuffer[Rxbytes++] = ASCII_TO_HEX(tBuff[0],tBuff[1]);//added
                    tRxbytes = 0;
                }
            }
            else
            {
                MdbRespBuffer.RxBuffer[Rxbytes++] = mdbUARTRxBuffer[RxCtr];//added
            }
            pBuffer++;
            RxCtr++;
        }while((RxCtr<noOfBytesToRead));
    }

    if(RxCtr == 0 )
    {				// ReadFile returned succes but no data. This means a timeout occured waiting for data.
        return 0;
        *pErrorCode = READ_WRITE_TIMEOUT;
    }
    else
    {
        *pErrorCode = MMPL_NO_ERROR;
    }
   return 	1;
}



UCHAR8 DecodeResponse(MDB_REQ_ADU *pMbReqAscii, MDB_RSP_ADU *pMbRspAscii, UCHAR8 *dataBuffer, UCHAR8 numItems)
{
    UCHAR8 byteCount;
    statusCode = MMPL_NO_ERROR;

    if ((MdbReqBuffer.TxBuffer[INDEX_SLAVE_ID]) != (MdbRespBuffer.RxBuffer[INDEX_SLAVE_ID]))
    {
        statusCode = ID_MISMATCH;
    }
    else if( CheckLrc(&MdbRespBuffer) != TRUE )			/* Check if LRC is valid */
    {
        statusCode = CRC_ERR;
    }
    else if ((MdbRespBuffer.RxBuffer[INDEX_FUNC_CODE]) & 0x80)	/* Exception response? */
    {
        dataBuffer[0] = MdbRespBuffer.RxBuffer[INDEX_FUNC_CODE+1];	/* Retrieve exception code in the Exception response */
        statusCode = EXCEPTION_RESPONSE;
    }
    else if ((MdbReqBuffer.TxBuffer[INDEX_FUNC_CODE]) != (MdbRespBuffer.RxBuffer[INDEX_FUNC_CODE]))
    {
        statusCode = FC_MISMATCH;
    }
    else
    {
        switch(MdbRespBuffer.RxBuffer[INDEX_FUNC_CODE])
        {
            case FC_READ_HOLD_REGS:
            case FC_READ_IP_REGS:
                byteCount  = (UCHAR8)(numItems * 2);	/* byte count computation */

                if( byteCount != (MdbRespBuffer.RxBuffer[INDEX_RESP_BYTE_CNT]) )
                {
                        statusCode = INVALID_BYTECNT;
                }
                else
                {
                    //removed function call since we are not using
                }
            break;

            case FC_WRITE_SINGLE_COIL:
            case FC_WRITE_SINGLE_REG:
            case FC_WRITE_MULTIPLE_COILS:
            case FC_WRITE_MULTIPLE_REGS:

                if((MdbReqBuffer.TxBuffer[INDEX_START_ADDR1] != MdbRespBuffer.RxBuffer[INDEX_START_ADDR1]) ||
                        (MdbReqBuffer.TxBuffer[INDEX_START_ADDR2] != MdbRespBuffer.RxBuffer[INDEX_START_ADDR2]) ||
                        (MdbReqBuffer.TxBuffer[INDEX_VALUE1]      != MdbRespBuffer.RxBuffer[INDEX_VALUE1]) 	   ||
                        (MdbReqBuffer.TxBuffer[INDEX_VALUE2]      != MdbRespBuffer.RxBuffer[INDEX_VALUE2]) )
                {
                        statusCode = INVALID_DATA_VALUE;
                }
            break;
        }
    }

    return statusCode;
}

/*******************************************************************************************************************************************
*******************************************************************************************************************************************
	Description:
	This function validates the LRC bytes at the end of the Modbus ADU.

	Parameters:
	a) pMbRspAscii (IN):  Pointer to a structure holding the response ADU.

	Returns:
	TRUE if LRC is OK, else FALSE.

	  The total no. of bytes to be used to make the CRC (excludes the CRC bytes themselves)
******************************************************************************************************************************************
********************************************************************************************************************************************/

BOOL CheckLrc(MDB_RSP_ADU *pMbRspAscii)
{
    UCHAR8 LRC = 0x00;
    UINT16 ctr = 0;

    UINT16 totalBytes = MdbRespBuffer.pduSize + 1 + 1/* Header+Slave address*/;

    //Avoid start bytein LRC calculation
    for(ctr=1; ctr<(totalBytes); ctr++)
    {
        LRC += (MdbRespBuffer.RxBuffer[ctr]);
    }

    LRC = ~LRC;
    LRC = LRC+1;
    
    if( (LRC == MdbRespBuffer.RxBuffer[totalBytes]) )
    {
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************************************************************************
*******************************************************************************************************************************************
	Description:
	This function appends the LRC bytes at the end of the Modbus ADU.

	Parameters:
	a) pMbReqAscii (IN):  Pointer to a structure holding the request ADU.

	Returns:
	None.
******************************************************************************************************************************************
********************************************************************************************************************************************/

void AppendLrc(MDB_REQ_ADU *pMbReqAscii,UINT16 totalBytes)
{
   
    UINT16 ctr;
    UINT16_VAL cRes;
    UCHAR8 Result = 0x00,ctotalBytes = 0;

    ctotalBytes = MdbReqBuffer.pduSize;
    
    //Avoid 1st byte(HEADER) It is not used in LRC calcuclation
    for(ctr=1; ctr<(ctotalBytes); ctr++)
    {
        Result += MdbReqBuffer.TxBuffer[ctr];
    }
    Result = ~Result;
    Result = Result+1;

    MdbReqBuffer.TxBuffer[MdbReqBuffer.pduSize] = Result;
    
    cRes = Hex2Ascii_Frame(Result);
    pMbReqAscii->TxBuffer[totalBytes++]   =  cRes.byte.HB;
    pMbReqAscii->TxBuffer[totalBytes++]   =  cRes.byte.LB;

}

void HexToBuffer(UCHAR8 *pSrcBuffer, UCHAR8 *pDstBuffer, UINT16 noOfRegs)
{
    UINT16_VAL Ascii_res;
    UCHAR8 regNo, srcBufCtr, dstBufCtr,cdstBufCtr = 0;

    srcBufCtr = 0;
    dstBufCtr = 0;
    cdstBufCtr = MdbReqBuffer.pduSize;

    //Each byte is represented in 2 ASCII chars
    noOfRegs = noOfRegs *2;

    for(regNo=0; regNo<noOfRegs; regNo++)
    {
        MdbReqBuffer.TxBuffer[cdstBufCtr++] = pSrcBuffer[srcBufCtr];

        Ascii_res = Hex2Ascii_Frame(pSrcBuffer[srcBufCtr]);
        pDstBuffer[dstBufCtr++] = Ascii_res.byte.HB;
        pDstBuffer[dstBufCtr++] = Ascii_res.byte.LB;;
        srcBufCtr++;
    }
}

UINT16_VAL Hex2Ascii_Frame (unsigned char DataByte)
{
    unsigned char Temp_MSB;
    UINT16_VAL AsciBytes;

    Temp_MSB = (DataByte & 0xF0);
    Temp_MSB = Temp_MSB >>4;
    AsciBytes.byte.HB = Hex2Ascii(Temp_MSB);
    AsciBytes.byte.LB = Hex2Ascii(DataByte & 0x0F);

    return AsciBytes;

}
