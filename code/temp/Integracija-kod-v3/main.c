#pragma config FNOSC = FRCPLL    // FRC sa PLL omogucen
#pragma config FCKSM = CSECMD // Clock switching enabled, fail-safe clock monitor disabled
#pragma config OSCIOFNC = OFF  // OSC2/CLKO is not a clock output
#pragma config POSCMOD = NONE  // Primary oscillator disabled
#pragma config IESO = OFF      // Two-speed oscillator start-up disabled
#pragma config JTAGEN = OFF    // JTAG disabled
#pragma config FWDTEN = OFF    // Watchdog timer disabled

#include <xc.h>      
#include <string.h>  
#include <stdlib.h>   
#define FCY 16000000UL 
#include "CONFIG.h"
#include <libpic30.h> 
#include "WIFI.h"
#include "SERVO.h"

int main() 
{
    int i = 0;
    configureAllComponents();
    configureU1();
    setupConnection();
    
    colorBuffer[0] = 'A'; //red
    colorBuffer[1] = 'c'; //green
    colorBuffer[2] = 'b'; //blue
    colorBuffer[3] = 'D'; //yellow
    colorBuffer[4] = 'e';   //pink
    colorBuffer[5] = 'F'; // orange
    colorBuffer[6] = 'G'; // black
    colorBuffer[7] = 'h';    //white
        
    while (1) 
    { 
        pollWiFi();

        if(flag == 0)
        {
            
            turnOnLED();
            pollWiFi();
            __delay_ms(100);
            char* color = readColor();
            pollWiFi();
            processDataFromColorSensor(color);
            pollWiFi();
            turnOffLED();
            moveMiddle();
            pollWiFi();
            __delay_ms(250);
        }  
    }
    return 0;
}