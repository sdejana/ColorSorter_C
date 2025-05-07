/*
 * File:   main.c
 * Author: Korisnik
 *
 * Created on 10. mart 2025., 14.34
 */

#pragma config FNOSC = FRC     // Use Internal Fast RC Oscillator (FRC) without PLL
#pragma config FCKSM = CSECMD // Clock switching enabled, fail-safe clock monitor disabled
#pragma config OSCIOFNC = OFF  // OSC2/CLKO is not a clock output
#pragma config POSCMOD = NONE  // Primary oscillator disabled
#pragma config IESO = OFF      // Two-speed oscillator start-up disabled
#pragma config JTAGEN = OFF    // JTAG disabled
#pragma config FWDTEN = OFF    // Watchdog timer disabled


#include "xc.h"
#include <stdio.h>
#include <stdlib.h>
#define FCY 3685000UL  // Define instruction cycle frequency as Fosc/2
#include <libpic30.h>

uint8_t position_changed = 0;
int prev_value=0;

void timer1_config(){
    TMR1 = 0; // clear the timer    
    T1CONbits.TCKPS = 0x03; // 1:256 prescaler
    T1CONbits.TGATE = 0x00; // gate is useless
    T1CONbits.TCS = 0x00; //tcs is useless also 
    PR1 = 57870;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    IPC0bits.T1IP = 7;    
    T1CONbits.TON = 0x01; // set it to working state    
}

void __attribute__((interrupt, auto_psv)) _T1Interrupt(void){
    LATBbits.LATB6 = ~LATBbits.LATB6;
    IFS0bits.T1IF = 0;   
    position_changed++;    

    if (position_changed % 2 == 0) {
        OC1RS = 800;
    } else {
        OC1RS = (prev_value == 300) ? 1300 : 300;
        prev_value = OC1RS;
    }

}


void output_compare_config(){
    TMR2 = 0;
    T2CONbits.TCKPS = 0x01; //prescaler is 1:8
    PR2 = 9300;
    OC1RS = 800; // duty cycle of my pwm - 90 degrees is 9300 value
    OC1R = 800;    
    OC1CONbits.OCM = 0x06; // continual pwm mode
    T2CONbits.TON = 1;
}



int main(void) {   
            
    AD1PCFG = 0xFFFF;
    TRISB = 0;
    
    
    __builtin_enable_interrupts();
    __builtin_write_OSCCONL(OSCCON & 0xBF); // Unlock PPS   
    RPOR2bits.RP5R = 18;  // Map OC1 to RP5 (RB5)
    __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS
    
    output_compare_config();
    __delay_ms(1000);
    timer1_config();
    
    
    
    while(1);
    
    return 0;
}
