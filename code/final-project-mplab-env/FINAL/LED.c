/*
 * File:   LED.c
 * Author: 23dej
 *
 * Created on April 10, 2025, 3:44 PM
 */


#include "xc.h"
#include "LED.h"

void turnOnLED()
{
    LATBbits.LATB6 = 1;
}
void turnOffLED()
{
    LATBbits.LATB6 = 0;
}
