/*
 * @author: Dejana S.
 */

#pragma config FNOSC = FRCPLL    // FRC sa PLL omogucen
#pragma config FCKSM = CSECMD // Clock switching enabled, fail-safe clock monitor disabled
#pragma config OSCIOFNC = OFF  // OSC2/CLKO is not a clock output
#pragma config POSCMOD = NONE  // Primary oscillator disabled
#pragma config IESO = OFF      // Two-speed oscillator start-up disabled
#pragma config JTAGEN = OFF    // JTAG disabled
#pragma config FWDTEN = OFF    // Watchdog timer disabled

#include <xc.h>      // Ukljucivanje osnovne biblioteke za mikrokontroler
#include <string.h>  // Biblioteka za rad sa stringovima
#include <stdlib.h>  // Standardna biblioteka 
#define FCY 16000000UL // Definisanje frekvencije CPU-a
#include "CONFIG.h"
#include <libpic30.h> // Biblioteka za kasnjenja (__delay_ms i __delay_us)
#include "WIFI.h"
#include "SERVO.h"

void toggleLED()
{
    LATBbits.LATB6 = ~LATBbits.LATB6;
}

void turnOnLED()
{
    LATBbits.LATB6 = 1;
}
void turnOffLED()
{
    LATBbits.LATB6 = 0;
}

void pollWiFi()
{
     if(strstr((const char*)buffer, "SSS"))
        {
            flag = 0;
            sendData("S\r\n");
        }
        if(strstr((const char*)buffer, "EEE"))
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
        if(strstr((const char*)buffer, "0"))
        {
            LATBbits.LATB6 = 1;
            parseDataFSM();
            LATBbits.LATB6 = 0;            
        }
}

int main() 
{
    int i = 0;
    configureAllComponents();
    
    TRISBbits.TRISB6 = 0;
    LATBbits.LATB6 = 1;
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
            
            // do color sensor job - func call 
            turnOnLED();
            pollWiFi();
            
            __delay_ms(200);
            char* color = readColor();
            //char* color = "RED";
            pollWiFi();
            processDataFromColorSensor(color);
            pollWiFi();
            turnOffLED();
            moveMiddle();
            pollWiFi();
            __delay_ms(500);
        }
       
        
    }
    return 0;
}