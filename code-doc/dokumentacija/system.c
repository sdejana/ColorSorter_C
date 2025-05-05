/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 ******************************************************************************
 *
 * @file    system.c
 * @brief   System initialization routines.
 *
 * @details
 *   Provides central system setup including interrupt manager and SPI1 module initialization.
 *   Should be called once at startup to configure core peripherals and pins.
 *
 * @author  Korisnik
 * @date    10 April 2025
 ******************************************************************************/

#include "pin_manager.h"
#include "system.h"
#include "spi1.h"
#include "interrupt_manager.h"

/**
 * @brief   Initialize system-level modules and peripherals.
 *
 * @details
 *   Calls the generated interrupt manager initialization and
 *   the SPI1 driver initialization. Pin manager init is commented out
 *   and can be enabled if pin setup via MCC is required.
 */
void SYSTEM_Initialize(void)
{
    //PIN_MANAGER_Initialize();    
    INTERRUPT_Initialize();
    SPI1_Initialize();    
}

/**
 * @brief   End of File marker.
 */
/** End of File */