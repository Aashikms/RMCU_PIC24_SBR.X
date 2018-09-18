/* 
 * File:   libModbus.h
 * Author: Srivardhan
 *
 * Created on 14 September, 2013, 10:19 AM
 */

#ifndef LIBMODBUS_H
#define	LIBMODBUS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define RD_BLK_SIZE_REGINFO     32
#define  MODBUS_RTU		0				/* Networks that run off Modbus RTU protocol are usually on RS232 or RS485 media interface */


/**********************************************************************************************************************************************************
***********************************************************************************************************************************************************
	The CRC table is an array of 256 bytes used in computing the CRC of Modbus APDUs. By default, for improving efficiency,
	this array is populated once in the beginning and stored in RAM to be used every time the CRC is to be generated. However,
	this uses up 256 bytes of data memory which can cause a resource crunch in small foot print devices. So the library
	provides for two ways of addressing this issue. One is: Since the CRC table is static, it provides an option of placing
	it in code memory (i.e. flash or EPROM). Other is: It provides an option of dynamically generating the CRC table values
	as and when the CRC is being computed.
**********************************************************************************************************************************************************
**********************************************************************************************************************************************************/
#define  IN_RAM				0			/* The CRC table is placed in RAM (data memory) */
#define  IN_ROM				1			/* The CRC table is placed in ROM (code memory) */

/*********************************************************************************************************************************************************
												lIST OF THE ERROR CODES IN MODBUS
**********************************************************************************************************************************************************/

#define  MMPL_NO_ERROR			0x00
#define  UNKNOWN_ERROR			0x01	/* An unknown error occurred reading / writing to port */
#define  INVALID_HANDLE			0x02	/* An invalid handle or path ID was used to read from / write to the port */
#define  INVALID_NETWORKNUM		0x03	/* An uninitialized network number was passed as a parameter */
#define  READ_WRITE_FAIL		0x04	/* Device failure reading / writing to port */
#define  READ_WRITE_TIMEOUT		0x05	/* Timeout occurred reading / writing bytes */
#define  ID_MISMATCH			0x06	/* The slave ID found in the Modbus request does not match this device */
#define  CRC_ERR                        0x07	/* The message contained incorrect CRC Bytes */
#define  BUFFER_TOO_SMALL		0x08	/* The request message has more bytes than the available size of buffer */
#define  PORT_CLOSED			0x09	/* The communication port was closed when trying to read or write on it */
#define  INVALID_FC                     0x0A	/* An invalid/unsupported function code is requested to be serviced */
#define  TXID_MISMATCH			0x0B	/* The Transaction ID of the Modbus request does not match the response's Transaction ID */
#define  INVALID_PROTCODE		0x0C	/* Invalid Protocol code in the response */
#define  EXCEPTION_RESPONSE		0x0D	/* Exception response from slave */
#define  FC_MISMATCH			0x0E	/* The function code of the Modbus request does not match the response's function code */
#define  INVALID_BYTECNT		0x0F	/* Invalid Byte count in the response */
#define  INVALID_DATA_VALUE		0x10	/* Invalid Data Value */
#define  INVALID_PKTLEN			0x11	/* Invalid Packet Length in the response */
#define  INVALID_SLAVE_ADDR		0x12	/* Invalid Slave ID */
#define  INVALID_NUM_ITEMS		0x13	/* Invalid number of items */
#define  READ_ERROR			0x14
/*********************************************************************************************************************************************************
												lIST OF THE IILEGAL FUNCTIONS IN MODBUS
**********************************************************************************************************************************************************/

#define ILLEGAL_FUNCTION		0x01
#define ILLEGAL_DATA_ADDRESS            0x02
#define ILLEGAL_DATA_VALUE		0x03
#define SLAVE_DEVICE_FAILURE            0x04
#define ACKNOWLEDGE                     0x05
#define SLAVE_DEVICE_BUSY		0x06
#define MEMORY_PARITY_ERROR		0x08
#define GW_PATH_UNAVAIL			0x0A
#define GW_TARGET_DEV_TIMEOUT           0x0B


/*********************************************************************************************************************************************************
												lIST OF THE INDEX IN MODBUS
**********************************************************************************************************************************************************/
#define INDEX_HEADER                    0 //changed
#define INDEX_SLAVE_ID			1
#define INDEX_FUNC_CODE			2
#define INDEX_START_ADDR1		3
#define INDEX_START_ADDR2		4
#define INDEX_NUM_ITEMS1		5
#define INDEX_NUM_ITEMS2		6
#define INDEX_VALUE1			5
#define INDEX_VALUE2			6
#define INDEX_WRITE_BYTE_CNT            7
#define INDEX_WRITE_START_DATA          8


#define INDEX_RESP_BYTE_CNT		3 //changed
#define INDEX_RESP_START_DATA           4

/**********************************************************************************************************************************************************
												lIST OF THE FUNCTION CODES IN MODBUS
**********************************************************************************************************************************************************/
#define FC_READ_COILS                   0x01
#define FC_READ_DISC_IPS                0x02
#define FC_READ_HOLD_REGS               0x03
#define FC_READ_IP_REGS                 0x04
#define FC_WRITE_SINGLE_COIL            0x05
#define FC_WRITE_SINGLE_REG             0x06
#define FC_WRITE_MULTIPLE_COILS         0x0F
#define FC_WRITE_MULTIPLE_REGS          0x10

#define SIZE_RESP_PDU_WRITE_BASIC       5
#define WR_BLK_SIZE_REGINFO             64
#define WR_BLK_SIZE_BITINFO             64


    /**********************************************************************************************************************************************************
		lIST OF THE FUNCTION CODES IN MODBUS
**********************************************************************************************************************************************************/
#define FC_READ_COILS                   0x01
#define FC_READ_DISC_IPS                0x02
#define FC_READ_HOLD_REGS               0x03
#define FC_READ_IP_REGS                 0x04
#define FC_WRITE_SINGLE_COIL            0x05
#define FC_WRITE_SINGLE_REG             0x06
#define FC_WRITE_MULTIPLE_COILS         0x0F
#define FC_WRITE_MULTIPLE_REGS          0x10

#define SIZE_RESP_PDU_WRITE_BASIC       5
#define WR_BLK_SIZE_REGINFO             64
#define WR_BLK_SIZE_BITINFO             64


/**********************PLC MODBUS register Addresses********************************************************/
#define DEVICE                          0x01		//PLC Modbus Device ID


//#define TOTALPLCREGSTOREAD              84          // To read CLASS PLC registers in integer
#define ACE_PLC_REGS_TO_READ             34          // To read CLASS PLC registers in integer
#define SETT_PLC_REGS_TO_READ            26          // To read CLASS PLC registers in integer
    
/* RMCU - General Parameters*/
#define RMC_UNIT_ID                     0x0001
#define RMC_LOC                         0x0002
#define RMC_TYPE                        0x0003
#define RMC_PATTERN                     0x0004
#define RMC_BRD_SIG_STA                 0x0005

#define PLC_SYM_MODE_CHANGE             0x1198      // Register to change the SBR system mode
#define PLC_SBR_ISR_RESET               0x0B24      // Register to change the SBR system mode

#define RMC_SYM_SECS                    0x1521
#define RMC_SYM_MINS                    0x1522    
#define RMC_SYM_HOUR                    0x1523
#define RMC_SYM_DATE                    0x1524
#define RMC_SYM_MONTH                   0x1525
#define RMC_SYM_YEAR                    0x1527
    
// PLC REGISTERS TO GET PREV RTC TIME
#define RMC_SYM_PREV_SECS               0x1C1C             //Reg D3100

#define RMC_SYM_SET_RTC                 0xB5B8             //Reg M3000    
#define RMC_SYM_CHANGE_YEARS            0x1C80             //Reg D3200    
/* SBR SYSTEM SETTING PARAMETERS*/

#define RMC_SBR_ISR_REG                 0x17D0
#define RMC_SBR_BATCH_TIMER             0x17D1
#define RMC_SBR_FILL_START              0x17D2
#define RMC_SBR_FILL_STOP               0x17D3
#define RMC_SBR_AERATE_START            0x17D4
#define RMC_SBR_AERATE_STOP             0x17D5
#define RMC_SBR_PRESETTLE_START         0x17D6
#define RMC_SBR_PRESETTLE_STOP          0x17D7
#define RMC_SBR_DECANT_START            0x17D8
#define RMC_SBR_DECANT_STOP             0x17D9
#define RMC_SBR_IDLE_START              0x17DA
#define RMC_SBR_IDLE_STOP               0x17DB
#define RMC_SBR_INTER_AER_FRQ_TMR       0x17DC
#define RMC_SBR_INTER_AER_RUN_TMR       0x17DD
#define RMC_P1_CUR_MAX_TRLD             0x17DE
#define RMC_P1_CUR_MIN_TRLD             0x17DF
#define RMC_P2_CUR_MAX_TRLD             0x17E0
#define RMC_P2_CUR_MIN_TRLD             0x17E1
#define RMC_P3_CUR_MAX_TRLD             0x17E2
#define RMC_P3_CUR_MIN_TRLD             0x17E3
#define RMC_SBR_HIGH_LEVEL_TRLD         0x17E4
#define RMC_SBR_LOW_LEVEL_TRLD          0x17E5
#define RMC_SBR_HIGH_WINDOW_TRLD        0x17E6
#define RMC_SBR_LOW_WINDOW_TRLD         0x17E7    
#define RMC_SBR_SENSOR_CHECK_TRLD       0x17E8
#define RMC_SBR_OVERFILL_TIMER_TRLD     0x17E9
    
/* RMCU - SBR ACE WEB system SUMMARY Parameters*/

#define RMC_SYM_RUN_HOUR                0x19C4
#define RMC_SBR_BATCH_CNT               0x19C6
#define RMC_SBR_CYCLE_CNT               0x19C8
#define RMC_SBR_FLOW_RATE               0x19CA
#define RMC_SBR_FLOW_TOTALIZER          0x19CB
#define RMC_SBR_TANK_LEVEL              0x19CD    

/*SBR ACE SYSTEM STATUS*/

#define RMC_SBR_SYS_MODE                0x19CE
#define RMC_SBR_SENSOR_STAT             0x19CF
#define RMC_LAST_POWEROFF_DATE          0x19D0
#define RMC_LAST_POWEROFF_MONTH         0x19D1
#define RMC_LAST_POWEROFF_YEAR          0x19D2
#define RMC_LAST_POWEROFF_HOURS         0x19D3
#define RMC_LAST_POWEROFF_MINS          0x19D4
#define RMC_LAST_POWEROFF_SECS          0x19D5
#define RMC_LAST_POWERON_DATE           0x19D6
#define RMC_LAST_POWERON_MONTH          0x19D7
#define RMC_LAST_POWERON_YEAR           0x19D8
#define RMC_LAST_POWERON_HOURS          0x19D9
#define RMC_LAST_POWERON_MINS           0x19DA
#define RMC_LAST_POWERON_SECS           0x19DB

/*SBR ACE SYSTEM STATUS*/
    
#define RMC_TOTAL_SYS_ENERGY            0x19DC
#define RMC_TOTAL_AVG_CURRENT           0x19DE
#define RMC_TOTAL_AVG_PF                0x19DF
#define RMC_P1_CURRENT                  0x19E0
#define RMC_P2_CURRENT                  0x19E1
#define RMC_P3_CURRENT                  0x19E2

/* RMCU - SBR Alarm Parameters */
#define RMC_SBR_FAULT_CODE              0x19E3
#define RMC_FAULT_CODE_VAL              0x19E5

/* Parameter sizes*/    

#define PLC_REG_DEFAULT_SIZE         2
#define PLC_REG_TWO_REG_READ         4

    
/*******************************************************************************************************************
								 Macros that set the receive and transmit buffer sizes
********************************************************************************************************************/
#define RX_BUFFER_SIZE	1024
#define TX_BUFFER_SIZE	64


/*******************************************************************************************************************
 A structure to hold the complete Modbus request ADU including the "addtional address" field as well as the CRC bytes
********************************************************************************************************************/

typedef struct
{
	UCHAR8 TxBuffer[TX_BUFFER_SIZE];
	UCHAR8 pduSize;
}MDB_REQ_ADU;

extern MDB_REQ_ADU MbReqAscii;
extern MDB_REQ_ADU MdbReqBuffer;
extern MDB_REQ_ADU mdbReqAdu;


/*******************************************************************************************************************
 A structure to hold the complete Modbus response ADU including the "addtional address" field as well as the CRC bytes
********************************************************************************************************************/
typedef struct
{
	UCHAR8 RxBuffer[RX_BUFFER_SIZE];
	UCHAR8 pduSize;
}MDB_RSP_ADU;

extern MDB_RSP_ADU mdbRspAdu;
extern MDB_RSP_ADU MdbRespBuffer;

/*******************************************************************************************************************
									lIST OF  Forward declarations
**********************************************************************************************************************/

UCHAR8 ModbusOP(UCHAR8 slaveNo, UCHAR8 functionCode, UINT16 startAddress, UCHAR8 numItems, UCHAR8 *dataBuffer, UCHAR8 numRetries);
UCHAR8 ConstructRequest(UCHAR8 slaveNo, UCHAR8 functionCode, UINT16 startAddress, UCHAR8 numItems, UCHAR8 *dataBuffer,MDB_REQ_ADU *MbReqAscii, MDB_RSP_ADU *MbRspAscii);
UCHAR8 DecodeResponse(MDB_REQ_ADU *pMbReqAscii, MDB_RSP_ADU *pMbRspAscii, UCHAR8 *dataBuffer, UCHAR8 numItems);
BOOL CheckLrc(MDB_RSP_ADU *pMbRspAscii);
void AppendLrc(MDB_REQ_ADU *pMbReqAscii,UINT16 totalBytes);
void HexToBuffer(UCHAR8 *pSrcBuffer, UCHAR8 *pDstBuffer, UINT16 noOfRegs);
void ReadPacket(UCHAR8 *pkBuffer, UINT16 noOfBytesToRead);
void MDBPackBits(UCHAR8 *pSrcBuffer, UCHAR8 *pDstBuffer, UINT16 noOfBits);
UINT16_VAL Hex2Ascii_Frame (unsigned char DataByte);


/**********************************************************************************************************************************************************
**********************************************************************************************************************************************************
	Description:
	This method is called by the library when it requires reading data bytes from a communication port. If less than the requested number of
	bytes could be read before timeout occurs, then the function should return CSPL_TRUE and set pNoOfBytesRead to the actual number of bytes read.
	Parameters:
	a) networkNo (IN): A number identifying the ?port? to be read.
	b) noOfBytesToRead (IN): The number of bytes to read on this port.
	c) pNoOfBytesRead (OUT): A pointer to the variable that receives the actual number of bytes read.
	d) pBuffer (OUT): A pointer to the buffer that receives the data read from the port.
	e) pErrorCode (OUT): A pointer to the variable that receives an error code in case of failure of this function.
	Returns:
	CSPL_TRUE if the function succeeds, else CSPL_FALSE. If the return value is CSPL_FALSE, then an error code indicating
	the reason for failure should be stored in the pErrorCode parameter.
**********************************************************************************************************************************************************
**********************************************************************************************************************************************************/


BOOL ReadPort(UINT16 ,UINT16 *, UCHAR8 *, UCHAR8 *);

/**********************************************************************************************************************************************************
**********************************************************************************************************************************************************
	Description:
	This method is called by the library when it requires writing data bytes to a communication port. If less than the requested number of bytes
	could be written before timeout occurs, then the function should return CSPL_TRUE and set pNoOfBytesWritten to the actual number of bytes
	written.
	Parameters:
	a) noOfBytesToWrite(IN): The number of bytes to write on this port.
**********************************************************************************************************************************************************
***********************************************************************************************************************************************************/


BOOL WritePacket(UINT16);

#define RS_485_EN       LATBbits.LATB9

#ifdef	__cplusplus
}
#endif

#endif	/* LIBMODBUS_H */

