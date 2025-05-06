/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 ******************************************************************************
 *
 * @file    system.h
 * @brief   System initialization interface and core frequency definition.
 *
 * @details
 *   Defines the system crystal frequency macro and declares the
 *   SYSTEM_Initialize function for initializing system-wide modules
 *   such as pin manager, interrupt manager, and other peripherals.
 *
 * @note    _XTAL_FREQ must match the actual oscillator frequency.
 *
 * @date    10 April 2025
 *******************************************************************************/

#ifndef _XTAL_FREQ
/**
 * @def     _XTAL_FREQ
 * @brief   CPU clock frequency in Hertz (32 MHz).
 */
#define _XTAL_FREQ  32000000UL
#endif

#include "xc.h"
#include "stdint.h"
#include "pin_manager.h"
#include "interrupt_manager.h"

#ifndef SYSTEM_H
#define SYSTEM_H

/**
 * @brief   Perform system-level initialization.
 *
 * @details
 *   Sets up core peripherals by calling initialization routines for
 *   pin manager, interrupt manager, and other modules as needed.
 */
void SYSTEM_Initialize(void);

#endif  /* SYSTEM_H */

/**
 * @brief   End of File marker.
 */
/** End of File */