/*
 * File:   interrupt_manager.c
 * Author: Korisnik
 *
 * Created on 10. april 2025., 13.06
 */


#include <xc.h>

/**
    void INTERRUPT_Initialize (void)
*/
void INTERRUPT_Initialize (void)
{
    //    SPII: SPI1 - SPI1 Transfer Done
    //    Priority: 1
        IPC2bits.SPI1IP = 1;
    //    UERI: U1E - UART1 Error
    //    Priority: 1
        IPC16bits.U1ERIP = 1;
    //    UTXI: U1TX - UART1 Transmitter
    //    Priority: 1
        IPC3bits.U1TXIP = 1;
    //    URXI: U1RX - UART1 Receiver
    //    Priority: 1
        IPC2bits.U1RXIP = 1;

}





