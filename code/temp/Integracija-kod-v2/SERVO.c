/*
 * File:   SERVO.c
 * Author: 23dej
 *
 * Created on April 2, 2025, 6:14 PM
 */


#include "xc.h"
#include "SERVO.h"
#include "WIFI.h"
#include <libpic30.h>

uint8_t position = 0;
volatile uint8_t middlePos;


/*void timer1_config() {
    TMR1 = 0;
    T1CONbits.TCKPS = 0b11;     // 1:256 prescaler
    PR1 = 30860;                // 0.5s interrupts at 16MHz
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    IPC0bits.T1IP = 5;
    T1CONbits.TON = 1;
}*/

/*void __attribute__((interrupt, auto_psv)) _T1Interrupt(void) {
    LATBbits.LATB6 = ~LATBbits.LATB6;  // Toggle LED for debugging
    IFS0bits.T1IF = 0;
    
    // Change position every 4 interrupts (every 2 seconds)
    static uint8_t counter = 0;
    if (++counter >= 2) {
        counter = 0;
        // Pattern: left, middle, right, middle, left, middle, right, etc.
        switch(position) {
            case 0: OC1RS = SERVO_LEFT; position = 1; break;    //-> next Middle
            case 1: OC1RS = SERVO_MIDDLE; position = 2; break;  //-> next Right
            case 2: OC1RS = SERVO_RIGHT; position = 3; break;   //-> next Middle
            case 3: OC1RS = SERVO_MIDDLE; position = 0; break; //-> next Left
        }
    }
}*/

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

void configureOscillatorServo() {
    CLKDIV = 0;
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    while (OSCCONbits.OSWEN);
}

void moveLeft()
{
    OC1RS = SERVO_LEFT;
    middlePos = 1;
}

void moveRight()
{
    OC1RS = SERVO_RIGHT;
    middlePos = 2;
}

void moveMiddle()
{
    OC1RS = SERVO_MIDDLE;
    middlePos = 0;
}

