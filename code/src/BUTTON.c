/*
 * File:   BUTTON.c
 * Author: 23dej
 *
 * Created on April 10, 2025, 3:33 PM
 */


#include "xc.h"
#include "BUTTON.h"
#include "WIFI.h"

void __attribute__((interrupt, auto_psv)) _INT0Interrupt(void) 
{
    IEC0bits.INT0IE = 0;
    flag = ~flag;
    if(flag)
    {
        stopAll();
    }
    IEC0bits.INT0IE = 1;
    IFS0bits.INT0IF = 0;  // Clear INT0 flag
}


