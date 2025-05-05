/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    SERVO.h
 * @brief   Servo control interface.
 *
 * @details
 *   Declares functions for configuring PWM output and moving the servo
 *   to predefined positions. Pulse widths are defined for left, middle,
 *   and right positions using Timer ticks for 50Hz operation.
 *
 ******************************************************************************/

#ifndef _SERVO_H
#define _SERVO_H

#include <xc.h> // Include processor files

/**
 * @brief Servo left position pulse width.
 *
 * Pulse width for leftmost position: 1ms (OC1RS = 4600).
 */
#define SERVO_LEFT   4600

/**
 * @brief Servo middle position pulse width.
 *
 * Pulse width for neutral position: 1.5ms (OC1RS = 3150).
 */
#define SERVO_MIDDLE 3150

/**
 * @brief Servo right position pulse width.
 *
 * Pulse width for rightmost position: 2ms (OC1RS = 1400).
 */
#define SERVO_RIGHT  1400

/**
 * @brief Configure PWM output compare and Timer2 for servo control.
 *
 * @details
 *   Sets up Timer2 prescaler and period for 50Hz, initializes output compare
 *   registers, and starts the timer to generate PWM signal on OC1.
 */
void outputCompareConfig(void);

/**
 * @brief Reconfigure system oscillator for servo routines.
 *
 * @details
 *   Selects FRC with PLL, resets clock divider, and waits for oscillator
 *   switch completion to ensure stable clock for PWM generation.
 */
void configureOscillatorServo(void);

/**
 * @brief Move servo to leftmost position.
 *
 * @details
 *   Updates OC1RS value to SERVO_LEFT pulse width.
 */
void moveLeft(void);

/**
 * @brief Move servo to rightmost position.
 *
 * @details
 *   Updates OC1RS value to SERVO_RIGHT pulse width.
 */
void moveRight(void);

/**
 * @brief Move servo to middle (neutral) position.
 *
 * @details
 *   Updates OC1RS value to SERVO_MIDDLE pulse width.
 */
void moveMiddle(void);
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _SERVO_H */
