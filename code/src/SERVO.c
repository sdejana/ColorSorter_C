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

void timer1_config() 
{
    TMR1 = 0;
    T1CONbits.TCKPS = 0b11;     // 1:256 prescaler
    PR1 = 31250;                // 0.5s interrupts at 16MHz
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    IPC0bits.T1IP = 7;
    T1CONbits.TON = 1;
}

void __attribute__((interrupt, auto_psv)) _T1Interrupt(void) 
{
    IFS0bits.T1IF = 0;
    
    // Change position every 4 interrupts (every 2 seconds)
    static uint8_t counter = 0;
    if (++counter >= 4) 
    {
        counter = 0;
        
        // Pattern: left, middle, right, middle, left, middle, right, etc.
        switch(position) 
        {
            case 0: OC1RS = SERVO_LEFT; position = 1; break;    // Left -> next Middle
            case 1: OC1RS = SERVO_MIDDLE; position = 2; break;  // Middle -> next Right
            case 2: OC1RS = SERVO_RIGHT; position = 3; break;   // Right -> next Middle
            case 3: OC1RS = SERVO_MIDDLE; position = 0; break; // Middle -> next Left
        }
    }
}

void output_compare_config() 
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

