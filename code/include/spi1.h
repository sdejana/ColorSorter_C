/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 ******************************************************************************
 *
 * @file    spi1.h
 * @brief   SPI1 driver API definitions.
 *
 * @details
 *   Provides the interface for initializing SPI1 module, performing
 *   data exchanges in 8-bit and buffer modes, and retrieving module status
 *   and transfer mode. Supports both 8-bit and 16-bit data transfer modes.
 ******************************************************************************/
#ifndef _SPI1_H
#define _SPI1_H

/**
 * Section: Included Files
 */
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus  /**< Provide C++ Compatibility */
extern "C" {
#endif

/**
 * @def      SPI1_DUMMY_DATA
 * @brief    Default dummy data used when no transmit buffer is provided.
 */
#define SPI1_DUMMY_DATA           0x0

/**
 * @def      SPI1_FIFO_FILL_LIMIT
 * @brief    FIFO fill limit for SPI1 transmission (max 8 entries).
 */
#define SPI1_FIFO_FILL_LIMIT      0x8

/* Ensure FIFO limit does not exceed hardware maximum */
#if (SPI1_FIFO_FILL_LIMIT > 8)
    #undef SPI1_FIFO_FILL_LIMIT
    #define SPI1_FIFO_FILL_LIMIT  8
#endif

/**
 * @brief  Bit masks representing various SPI1 status flags.
 */
typedef enum {
    SPI1_SHIFT_REGISTER_EMPTY  = 1 << 7,  /**< Shift register is empty */
    SPI1_RECEIVE_OVERFLOW      = 1 << 6,  /**< Receive overflow occurred */
    SPI1_RECEIVE_FIFO_EMPTY    = 1 << 5,  /**< Receive FIFO is empty */
    SPI1_TRANSMIT_BUFFER_FULL  = 1 << 1,  /**< Transmit buffer is full */
    SPI1_RECEIVE_BUFFER_FULL   = 1 << 0   /**< Receive buffer is full */
} SPI1_STATUS;

/**
 * @brief  SPI1 transfer data size configuration.
 */
typedef enum {
    SPI1_DRIVER_TRANSFER_MODE_8BIT  = 0,  /**< 8-bit transfer mode */
    SPI1_DRIVER_TRANSFER_MODE_16BIT = 1,  /**< 16-bit transfer mode */
    SPI1_DRIVER_TRANSFER_MODE_32BIT = 2   /**< 32-bit transfer mode (if supported) */
} SPI1_TRANSFER_MODE;

/**
 * Section: Interface Routines
 */

/**
 * @brief  Initialize the SPI1 module.
 *
 * @details
 *   Configures SPI1 as master, sets clock polarity/phase,
 *   selects data input sampling, and enables the module.
 *   Must be called once before any other SPI1 APIs.
 */
void SPI1_Initialize(void);

/**
 * @brief  Exchange a single 8-bit data word over SPI1.
 *
 * @param  data  Byte to transmit over SPI1.
 *
 * @return Received byte from SPI1.
 *
 */
uint8_t SPI1_Exchange8bit(uint8_t data);

/**
 * @brief  Exchange multiple 8-bit data words over SPI1.
 *
 * @param  dataTransmitted  Pointer to transmit buffer; if NULL, dummy data sent.
 * @param  byteCount        Number of bytes to exchange.
 * @param  dataReceived     Pointer to receive buffer; if NULL, received data discarded.
 *
 * @return Number of bytes successfully exchanged.
 *
 */
uint16_t SPI1_Exchange8bitBuffer(uint8_t *dataTransmitted,
                                 uint16_t byteCount,
                                 uint8_t *dataReceived);

/**
 * @brief  Get current SPI1 status flags.
 *
 * @return Status register contents as SPI1_STATUS bitmask.
 */
SPI1_STATUS SPI1_StatusGet(void);

#ifdef __cplusplus  /**< End C++ Compatibility */
    }
#endif

#endif /* _SPI1_H */
