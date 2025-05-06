/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    LED.h
 * @brief   LED control interface.
 *
 * @details
 *   Declares functions to turn the on-board LED on and off.Function 
 *   implementations are provided in LED.c.
 *
 *****************************************************************************/
#ifndef _LED_H
#define _LED_H

#include <xc.h> // include processor files

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Turns the LED on.
 *
 * @details
 *   Sets LATB6 to 1, driving the LED pin high to illuminate the LED.
 */
void turnOnLED(void);

/**
 * @brief   Turns the LED off.
 *
 * @details
 *   Sets LATB6 to 0, driving the LED pin low to extinguish the LED.
 */
void turnOffLED(void);

#ifdef __cplusplus
}
#endif

#endif /* _LED_H */
