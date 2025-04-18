/*
 * File:   BUTTON.c
 * Author: 23dej
 *
 * Created on April 10, 2025, 3:33 PM
 */


#include "xc.h"
#include "BUTTON.h"
#include "WIFI.h"
#include "CONFIG.h"

/*void __attribute__((interrupt, auto_psv)) _INT0Interrupt(void) 
{
    IEC0bits.INT0IE = 0;
    flag = ~flag;
    if(flag)
    {
        stopAll();
    }
    
    IEC0bits.INT0IE = 1;
    IFS0bits.INT0IF = 0;  // Clear INT0 flag
}*/

void handleReset()
{
     flag = 1;
     sendData("R\r\n");
     stopAll();
     colorBuffer[0] = 'A'; //red
     colorBuffer[1] = 'c'; //green
     colorBuffer[2] = 'b'; //blue
     colorBuffer[3] = 'D'; //yellow
     colorBuffer[4] = 'e';   //pink
     colorBuffer[5] = 'F'; // orange
     colorBuffer[6] = 'G'; // black
     colorBuffer[7] = 'h';    //white
}


