#ifndef _VEML3328_H
#define _VEML3328_H

#include <xc.h> // include processor files - each processor file is guarded. 
#define FCY 16000000UL
#include <libpic30.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "system.h"

#define COLOR_QUANTITY 10
#define NUM_OF_SAMPLES 10


extern volatile uint8_t interrupt_detected;
extern volatile uint16_t red_vector;
extern volatile uint16_t green_vector;
extern volatile uint16_t blue_vector;

extern uint16_t red_value;
extern uint16_t green_value;
extern uint16_t blue_value;


// local functions here
// local functions are being called as a part of global functions

void set_slave(void);
char* detect_color(void);
float calcuate_distance(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
void WS2812_Send_Byte(uint8_t);
void I2C_free_stuck_slave(void);
void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void);
void I2C_stop(void);
void I2C_start(void);
void I2C_receive_enable(void);
void I2C_write(uint8_t);
void package_receive(uint16_t*);
void request_data(uint8_t, uint16_t*);
void I2C_config(void);


// global functions here

void color_click_configuration(void);
void WS2812_Set_Color(uint8_t, uint8_t, uint8_t); // YET TO BE CALIBRATED
char* read_color(void); // YET TO BE CALIBRATED

// ------------

// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

