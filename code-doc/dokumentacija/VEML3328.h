 /******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 ******************************************************************************
 *
 * @file    VEML3328.h
 * @brief   Interface for VEML3328 color sensor driver and WS2812 LED control.
 *
 * @details
 *   Declares macros, external variables, and functions for initializing and
 *   communicating with the VEML3328 I2C color sensor, handling I2C bus recovery,
 *   interrupt management, RGB data normalization, color detection, and sending
 *   output to WS2812 LEDs via SPI. Supports reading multiple samples and
 *   matching against calibrated color values.
 *******************************************************************************/
#ifndef _VEML3328_H
#define _VEML3328_H

#include <xc.h>
#define FCY 16000000UL
#include <libpic30.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "system.h"

/** Number of calibrated colors in the detection table. */
#define COLOR_QUANTITY    9
/** Number of samples to average for a single color reading. */
#define NUM_OF_SAMPLES    10

/** Flag set by I2C interrupt when transaction completes. */
extern volatile uint8_t interrupt_detected;
/** Latest raw red channel reading from sensor. */
extern volatile uint16_t red_vector;
/** Latest raw green channel reading from sensor. */
extern volatile uint16_t green_vector;
/** Latest raw blue channel reading from sensor. */
extern volatile uint16_t blue_vector;

/**
 * @brief   Latest normalized red value (scaled). 
 */
extern uint16_t red_value;
/**
 * @brief   Latest normalized green value (scaled). 
 */
extern uint16_t green_value;
/**
 * @brief   Latest normalized blue value (scaled). 
 */
extern uint16_t blue_value;

/**
 * @brief   Initialize sensor registers for measurement.
 */
void set_slave(void);
/**
 * @brief   Detect color name from normalized RGB triple.
 */
char* detect_color(uint16_t *r, uint16_t *g, uint16_t *b);
/**
 * @brief   Compute Euclidean distance between two RGB triples.
 */
float calculate_distance(int r1, int g1, int b1,
                         int r2, int g2, int b2);
/**
 * @brief   Transmit one byte to WS2812 LED via SPI timing patterns.
 */
void WS2812_Send_Byte(uint8_t byte);
/**
 * @brief   Force I2C bus release when stuck by slave.
 */
void I2C_free_stuck_slave(void);
/**
 * @brief   I2C master interrupt service routine.
 */
void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void);
_MI2C1Interrupt(void);
/**
 * @brief   Issue I2C stop condition and wait for completion.
 */
void I2C_stop(void);
/**
 * @brief   Issue I2C start condition and wait for completion.
 */
void I2C_start(void);
/**
 * @brief   Enable I2C receive mode and wait for data.
 */
void I2C_receive_enable(void);
/**
 * @brief   Write one byte to I2C bus and wait for ACK.
 * @param   data Byte to transmit.
 */
void I2C_write(uint8_t data);
/**
 * @brief   Receive 16-bit package (LSB then MSB) over I2C.
 * @param   receive_vector Pointer to store the combined result.
 */
void package_receive(uint16_t*);
/**
 * @brief   Send a command and read back one 16-bit register.
 * @param   command_code Register address/command.
 * @param   color_vector Pointer to store returned data.
 */
void request_data(uint8_t, uint16_t*);
/**
 * @brief   Configure I2C module parameters and prepare sensor.
 */
void I2C_config(void);
/**
 * @brief   Set WS2812 LED strip color with GRB order.
 */
void WS2812_Set_Color(uint8_t, uint8_t, uint8_t);

/* Global API functions */
/**
 * @brief   Perform system and sensor setup then indicate ready.
 *
 * @details
 *   Calls system initialization, configures I2C and sensor, then
 *   lights up WS2812 LED to show readiness.
 */
void colorClickConfiguration(void);
/**
 * @brief   Read averaged normalized color values and detect color name.
 * @return  String name of detected color.
 */
char* readColor(void);

#endif /* _VEML3328_H */
/** End of File */