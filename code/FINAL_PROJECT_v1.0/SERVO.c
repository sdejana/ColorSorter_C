/*
 * File:   SERVO.c
 * Author: 23dej
 *
 * Created on April 2, 2025, 6:14 PM
 */


#include "xc.h"
#include "SERVO.h"
#include <libpic30.h>

uint8_t position = 0;


void outputCompareConfig() {
    TMR2 = 0;
    T2CONbits.TCKPS = 0b01;     // 1:8 prescaler
    PR2 = 39999;                // 50Hz PWM (16MHz/8/40000) 
    OC1R = SERVO_MIDDLE;        // Initial position (neutral)
    OC1RS = SERVO_MIDDLE;
    OC1CONbits.OCM = 0b110;
    OC1CONbits.OCTSEL = 0;
    T2CONbits.TON = 1;
}

void moveLeft()
{
    OC1RS = SERVO_LEFT;
}

void moveRight()
{
    OC1RS = SERVO_RIGHT; 
}

void moveMiddle()
{
    OC1RS = SERVO_MIDDLE;
}

