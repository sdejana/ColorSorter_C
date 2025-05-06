/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    SERVO.c
 * @brief   Servo motor control implementation.
 *
 * @details
 *   Provides functions to configure PWM output for servo control and
 *   to move the servo to left, right, and middle positions. Uses Timer2
 *   and Output Compare 1 modules to generate 50Hz PWM signal.
 *
 ******************************************************************************/

#include "xc.h"
#include "SERVO.h"
#include "WIFI.h"
#include <libpic30.h>

/**
 * @brief   Configures Output Compare and Timer2 for servo PWM.
 *
 * @details
 *   Resets Timer2, sets prescaler to 1:8, configures PR2 for 50Hz PWM,
 *   initializes OC1R and OC1RS to middle position, sets OC1 mode to
 *   edge-aligned PWM, selects Timer2 as clock source, and starts Timer2.
 */
void outputCompareConfig(void)
{
    TMR2 = 0;
    T2CONbits.TCKPS = 0b01;     // 1:8 prescaler
    PR2 = 39999;                // 50Hz PWM (16MHz/8/40000)
    OC1R = SERVO_MIDDLE;        // Initial position (neutral)
    OC1RS = SERVO_MIDDLE;
    OC1CONbits.OCM = 0b110;
    OC1CONbits.OCTSEL = 0;
    T2CONbits.TON = 1;
}

/**
 * @brief   Reconfigures system oscillator for servo routines.
 *
 * @details
 *   Sets the frequency divider to zero, selects FRC with PLL, confirms
 *   change, and waits for oscillator switch completion. Ensures consistent
 *   clock for servo PWM generation.
 */
void configureOscillatorServo(void)
{
    CLKDIV = 0;
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    while (OSCCONbits.OSWEN);
}

/**
 * @brief   Moves servo to leftmost position.
 */
void moveLeft(void)
{
    OC1RS = SERVO_LEFT;
}

/**
 * @brief   Moves servo to rightmost position.
 */
void moveRight(void)
{
    OC1RS = SERVO_RIGHT;
}

/**
 * @brief   Moves servo to middle position.
 */
void moveMiddle(void)
{
    OC1RS = SERVO_MIDDLE;
}
