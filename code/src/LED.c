/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    LED.c
 * @brief   LED control functions.
 *
 * @details
 *   Implements simple routines to turn the on-board LED on and off.
 *
 *****************************************************************************/

#include "xc.h"
#include "LED.h"

/**
 * @brief   Turns the LED on.
 *
 * @details
 *   Sets the LATB6 bit to drive the LED pin high, turning the LED on.
 */
void turnOnLED()
{
    LATBbits.LATB6 = 1;
}

/**
 * @brief   Turns the LED off.
 *
 * @details
 *   Clears the LATB6 bit to drive the LED pin low, turning the LED off.
 */
void turnOffLED()
{
    LATBbits.LATB6 = 0;
}
