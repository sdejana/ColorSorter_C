/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    CONFIG.h
 * @brief   System configuration interface.
 *
 * @details
 *   Declares functions for configuring the system oscillator, peripheral
 *   pin select (PPS), I/O ports, and controlling system-wide operations
 *   including the external interrupt service routine.
 *
 ******************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h> // include processor files - each processor file is guarded.
#include "VEML3328.h"
#include "WIFI.h"
#include "SERVO.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   External interrupt 0 service routine.
 *
 * @details
 *   Handles button presses on INT0, toggles system flag, and invokes
 *   stop or start routines accordingly. Clears and re-enables the interrupt.
 */
void __attribute__((interrupt, auto_psv)) _INT0Interrupt(void);

/**
 * @brief   Configures the system oscillator.
 *
 * @details
 *   Sets up the frequency divider, selects the Fast RC oscillator with PLL,
 *   and waits for the oscillator switch to complete.
 */
void configureOscillator(void);

/**
 * @brief   Configures peripheral pin select (PPS).
 *
 * @details
 *   Unlocks PPS, maps UART and SPI pins for Wi‑Fi and servo control,
 *   and then locks PPS configuration.
 */
void configurePPS(void);

/**
 * @brief   Configures I/O ports and interrupt settings.
 *
 * @details
 *   Disables analog functions, sets pin directions for UART, enable pin,
 *   LED, and button. Initializes LAT registers and configures INT0.
 */
void configureIO(void);

/**
 * @brief   Stops all operations and resets actuators.
 *
 * @details
 *   Sets global flag, turns off LED and RGB LED, moves servo to middle,
 *   and sends stop command over Wi‑Fi.
 */
void stopAll(void);

/**
 * @brief   Initializes and configures all components.
 *
 * @details
 *   Calls oscillator, I/O, and PPS configuration functions, then configures
 *   color sensor and output compare modules, and delays for stabilization.
 */
void configureAllComponents(void);

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
