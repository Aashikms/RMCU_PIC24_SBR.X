/*
 * File:   HardwareProfile.h
 * Author: Phoenix
 *
 * Created on November 2, 2013, 10:57 AM
 */
#include <xc.h>
#include <spi.h>

#ifndef HARDWAREPROFILE_H
#define	HARDWAREPROFILE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define BEAT_LED_TRIS		(TRISCbits.TRISC6)	// Ref LED1
#define BEAT_LED		(LATCbits.LATC6)
#define PLC_TX_LED_TRIS		(TRISCbits.TRISC9)	// Ref LED2
#define PLC_TX_LED		(LATCbits.LATC9)
#define PLC_RX_LED_TRIS         (TRISBbits.TRISB8)	// Ref LED3
#define PLC_RX_LED		(LATBbits.LATB8)
#define BAT_LOW_LED_TRIS        (TRISBbits.TRISB14)	// Ref LED3
#define BAT_LOW_LED		(LATBbits.LATB14)


//      #include "HardwareProfile.h"

    // define the chip select pin function
#define _SetChipSelect(on)          {\
                                        TRISBbits.TRISB0;\
                                        if(on)\
                                            LATBbits.LATB0 = 0;\
                                        else\
                                            LATBbits.LATB0 = 1;\
                                    }

#define SD_CS_ANSEL   (ANSELBbits.ANSB0)
#define SD_SDI_ANSEL  (ANSELAbits.ANSA4)


#define MEDIASD_IF_SPI1

    // SD Card SPI configuration defines
#define FATFS_SPI_START_CFG_1		(PRI_PRESCAL_64_1 & SEC_PRESCAL_8_1 & MASTER_ENABLE_ON & SPI_CKE_ON & SPI_SMP_ON)
#define FATFS_SPI_START_CFG_2		(SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR)


#define _SPI1
#define MDD_USE_SPI_1
#define USE_SD_INTERFACE_WITH_SPI

#define SPI_START_CFG_1     (PRI_PRESCAL_64_1 & SEC_PRESCAL_8_1 & MASTER_ENABLE_ON & SPI_CKE_ON & SPI_SMP_ON)
#define SPI_START_CFG_2     (SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR)
#define SPI_FREQUENCY	(20000000)		// Define the SPI frequency

    

    // Description: SD-SPI Chip Select Output bit
    #define SD_CS               _LATB0
    // Description: SD-SPI Chip Select TRIS bit
    #define SD_CS_TRIS          _TRISB0
  
    #define SPICON1             SPI1CON1
	// Description: The SPI status register
    #define SPISTAT             SPI1STAT
	// Description: The SPI Buffer
    #define SPIBUF              SPI1BUF
	// Description: The receive buffer full bit in the SPI status register
    #define SPISTAT_RBF         SPI1STATbits.SPIRBF
	// Description: The bitwise define for the SPI control register (i.e. _____bits)
    #define SPICON1bits         SPI1CON1bits
    // Description: The bitwise define for the SPI status register (i.e. _____bits)
    #define SPISTATbits         SPI1STATbits
	// Description: The enable bit for the SPI module
    #define SPIENABLE           SPI1STATbits.SPIEN
	// Description: The definition for the SPI baud rate generator register (PIC32)
    #define SPIBRG		  SPI1BRG

    #define SPICLOCK            TRISCbits.TRISC3
    // Description: The TRIS bit for the SDI pin
    #define SPIIN               TRISAbits.TRISA4
    // Description: The TRIS bit for the SDO pin
    #define SPIOUT              TRISAbits.TRISA9

   
#ifdef	__cplusplus
}
#endif

#endif	/* HARDWAREPROFILE_H */

