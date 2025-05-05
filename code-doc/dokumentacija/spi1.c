/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    spi1.c
 * @brief   SPI1 driver implementation.
 *
 * @details
 *   Provides functions to initialize SPI1 module, exchange data in 8-bit
 *   or buffer modes, and retrieve driver status and transfer mode. Supports
 *   both 8-bit and 16-bit transfer modes based on configuration.
 *
 ******************************************************************************/

#include "spi1.h"

/**
 * @brief   Initializes the SPI1 peripheral.
 *
 * @details
 *   Configures SPI1 as master, sets clock polarity, phase, data input sample
 *   phase, buffer mode, and enables the module. Priorities are set by default.
 */
void SPI1_Initialize(void)
{
    // MSTEN Master; DISSDO disabled; PPRE 1:1; SPRE 2:1;
    // MODE16 disabled; SMP Middle; DISSCK disabled;
    // CKP Idle:Low, Active:High; CKE Active to Idle
    SPI1CON1 = 0x13B;
    SPI1CON1bits.CKE = 0;
    SPI1CON1bits.CKP = 1;
    SPI1CON1bits.SMP = 0;

    // SPIBEN enabled; SPIFPOL disabled; SPIFE disabled
    SPI1CON2 = 0x01;

    // Enable SPI1, clear flags
    SPI1STAT = 0x800C;
}

/**
 * @brief   Exchanges one byte over SPI1.
 *
 * @param   pTransmitData Pointer to byte to transmit.
 * @param   pReceiveData  Pointer to variable to store received byte.
 *
 * @details
 *   Waits until transmit buffer is empty, writes data to SPI1BUF,
 *   then waits until receive buffer has data and reads it back.
 */
void SPI1_Exchange(uint8_t *pTransmitData, uint8_t *pReceiveData)
{
    while (SPI1STATbits.SPITBF)
    {
        // wait for transmit buffer to be free
    }
    SPI1BUF = *pTransmitData;
    while (SPI1STATbits.SRXMPT)
    {
        // wait for receive buffer to have data
    }
    *pReceiveData = SPI1BUF;
}

/**
 * @brief   Exchanges multiple bytes over SPI1.
 *
 * @param   pTransmitData Pointer to transmit buffer (or NULL to send dummy).
 * @param   byteCount     Number of bytes to exchange.
 * @param   pReceiveData  Pointer to receive buffer (or NULL to discard).
 * @return  uint16_t      Number of bytes transmitted.
 *
 * @details
 *   Handles simultaneous transmit and receive of up to byteCount bytes.
 *   If transmit or receive buffer pointers are NULL, dummy data is used.
 */
uint16_t SPI1_ExchangeBuffer(uint8_t *pTransmitData, uint16_t byteCount, uint8_t *pReceiveData)
{
    uint16_t dataSentCount = 0;
    uint16_t dataReceivedCount = 0;
    uint16_t dummyDataReceived = 0;
    uint16_t dummyDataTransmit = SPI1_DUMMY_DATA;
    uint8_t  *pSend;
    uint8_t  *pReceived;
    uint16_t sendAddressIncrement;
    uint16_t receiveAddressIncrement;

    // Determine address increments based on pointer validity
    if (pTransmitData == NULL)
    {
        sendAddressIncrement = 0;
        pSend = (uint8_t*)&dummyDataTransmit;
    }
    else
    {
        sendAddressIncrement = 1;
        pSend = pTransmitData;
    }
    if (pReceiveData == NULL)
    {
        receiveAddressIncrement = 0;
        pReceived = (uint8_t*)&dummyDataReceived;
    }
    else
    {
        receiveAddressIncrement = 1;
        pReceived = pReceiveData;
    }

    // Clear transmit buffer
    while (SPI1STATbits.SPITBF) {}

    // Exchange loop
    while (dataSentCount < byteCount)
    {
        if (!SPI1STATbits.SPITBF)
        {
            SPI1BUF = *pSend;
            pSend += sendAddressIncrement;
            dataSentCount++;
        }
        if (!SPI1STATbits.SRXMPT)
        {
            *pReceived = SPI1BUF;
            pReceived += receiveAddressIncrement;
            dataReceivedCount++;
        }
    }
    // Drain remaining data if any
    while (dataReceivedCount < byteCount)
    {
        if (!SPI1STATbits.SRXMPT)
        {
            *pReceived = SPI1BUF;
            pReceived += receiveAddressIncrement;
            dataReceivedCount++;
        }
    }
    return dataSentCount;
}

/**
 * @brief   Exchanges a single byte and returns the received byte.
 *
 * @param   data Byte to send.
 * @return  uint8_t Received byte.
 */
uint8_t SPI1_Exchange8bit(uint8_t data)
{
    uint8_t receiveData;
    SPI1_Exchange(&data, &receiveData);
    return receiveData;
}

/**
 * @brief   Exchanges multiple bytes and returns the count.
 *
 * @param   dataTransmitted Pointer to transmit buffer.
 * @param   byteCount       Number of bytes to exchange.
 * @param   dataReceived    Pointer to receive buffer.
 * @return  uint16_t        Number of bytes transmitted.
 */
uint16_t SPI1_Exchange8bitBuffer(uint8_t *dataTransmitted, uint16_t byteCount, uint8_t *dataReceived)
{
    return SPI1_ExchangeBuffer(dataTransmitted, byteCount, dataReceived);
}
/**

    The module's transfer mode affects the operation
    of the exchange functions. The table below shows
    the effect on data sent or received:
    |=======================================================================|
    | Transfer Mode  |     Exchange Function      |        Comments         |
    |=======================================================================|
    |                | SPIx_Exchange8bitBuffer()  |                         |
    |                |----------------------------|  OK                     |
    |                | SPIx_Exchange8bit()        |                         |
    |     8 bits     |----------------------------|-------------------------|
    |                | SPIx_Exchange16bitBuffer() | Do not use. Only the    |
    |                |----------------------------| lower byte of the 16-bit|
    |                | SPIx_Exchange16bit()       | data will be sent or    |
    |                |                            | received.               |
    |----------------|----------------------------|-------------------------|
    |                | SPIx_Exchange8bitBuffer()  | Do not use. Additional  |
    |                |----------------------------| data byte will be       |
    |                | SPIx_Exchange8bit()        | inserted for each       |
    |                |                            | 8-bit data.             |
    |     16 bits    |----------------------------|-------------------------|
    |                | SPIx_Exchange16bitBuffer() |                         |
    |                |----------------------------|  OK                     |
    |                | SPIx_Exchange16bit()       |                         |
    |----------------|----------------------------|-------------------------|
*/


/**
 * @brief   Gets the current SPI1 transfer mode.
 *
 * @return  SPI1_TRANSFER_MODE Current mode (8-bit or 16-bit).
 */
inline __attribute__((__always_inline__)) SPI1_TRANSFER_MODE SPI1_TransferModeGet(void)
{
    if (SPI1CON1bits.MODE16 == 0)
        return SPI1_DRIVER_TRANSFER_MODE_8BIT;
    else
        return SPI1_DRIVER_TRANSFER_MODE_16BIT;
}

/**
 * @brief   Retrieves the SPI1 status register.
 *
 * @return  SPI1_STATUS Current contents of SPI1STAT.
 */
SPI1_STATUS SPI1_StatusGet(void)
{
    return SPI1STAT;
}

/**
 * @note    End of File
 */
