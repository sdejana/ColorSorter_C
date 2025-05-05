/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    pin_manager.h
 * @brief   Pin manager interface definitions.
 *
 * @details
 *   Provides macros for controlling SDO1 pin (RB11) and declares the
 *   PIN_MANAGER_Initialize function for setting up pin configurations.
 *
 *****************************************************************************/
#ifndef _PIN_MANAGER_H
#define _PIN_MANAGER_H

/**
 * @section Includes
 * Include device headers for register definitions.
 */
#include <xc.h>

/** @name SDO1 Pin Control Macros
 *  Macros to control the SDO1 pin (RB11).
 */
/**@{*/
/**
 * @brief   Set SDO1 pin high.
 */
#define SDO1_SetHigh()          (_LATB11 = 1)

/**
 * @brief   Set SDO1 pin low.
 */
#define SDO1_SetLow()           (_LATB11 = 0)

/**
 * @brief   Toggle SDO1 pin state.
 */
#define SDO1_Toggle()           (_LATB11 ^= 1)

/**
 * @brief   Get current SDO1 pin value.
 *
 * @return  Returns the logic level (0 or 1) of RB11.
 */
#define SDO1_GetValue()         _RB11

/**
 * @brief   Configure SDO1 pin as digital input.
 */
#define SDO1_SetDigitalInput()  (_TRISB11 = 1)

/**
 * @brief   Configure SDO1 pin as digital output.
 */
#define SDO1_SetDigitalOutput() (_TRISB11 = 0)
/**@}*/

/**
 * @brief   Initialize pin manager settings.
 *
 * @details
 *   Configures initial pin states and directions as required by the
 *   application. Should be called once at system startup.
 */
void PIN_MANAGER_Initialize(void);

#endif /* _PIN_MANAGER_H */
