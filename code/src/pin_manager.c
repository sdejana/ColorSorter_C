/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    pin_manager.c
 * @brief   Pin manager initialization routines.
 *
 * @details
 *   Implements the initialization of output latches, GPIO directions,
 *   open-drain configuration, analog/digital settings, and optional
 *   peripheral pin select (PPS) settings for the device.
 *
 *****************************************************************************/

#include <xc.h>
#include <stdio.h>
#include "pin_manager.h"

/**
 * @brief   Initializes pin manager settings.
 *
 * @details
 *   Configures open-drain control registers, analog/digital pins,
 *   and can include PPS mappings if enabled. Commented-out sections
 *   illustrate latch settings, GPIO direction, and pull-ups/-downs.
 */
void PIN_MANAGER_Initialize(void)
{
    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0000;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    AD1PCFG = 0x100C;
    
    /****************************************************************************
     * Optional: Set the PPS
     *
     * Unlock PPS, configure RP11->SDO1, then lock PPS.
     ***************************************************************************/
    /*
    __builtin_write_OSCCONL(OSCCON & 0xBF); // unlock PPS
    RPOR5bits.RP11R = 0x0007; // RB11 -> SPI1:SDO1
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS
    */
}
