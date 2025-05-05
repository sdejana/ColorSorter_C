/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    interrupt_manager.c
 * @brief   Interrupt controller initialization.
 *
 * @details
 *   Implements the initialization routine for configuring interrupt
 *   priorities for SPI1 transfer, UART1 error, UART1 transmitter, and
 *   UART1 receiver interrupts.
 *
 *****************************************************************************/

#include <xc.h>

/**
 * @brief   Initializes interrupt priorities for peripherals.
 *
 * @details
 *   Sets the priority level (1) for the following interrupts:
 *     - SPI1 Transfer Done (SPII)
 *     - UART1 Error (UERI)
 *     - UART1 Transmitter (UTXI)
 *     - UART1 Receiver (URXI)
 */
void INTERRUPT_Initialize(void)
{
    // SPI1 Transfer Done interrupt priority
    IPC2bits.SPI1IP = 1;
    // UART1 Error interrupt priority
    IPC16bits.U1ERIP = 1;
    // UART1 Transmitter interrupt priority
    IPC3bits.U1TXIP = 1;
    // UART1 Receiver interrupt priority
    IPC2bits.U1RXIP = 1;
}
