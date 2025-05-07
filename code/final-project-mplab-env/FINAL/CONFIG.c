/*
 * File:   CONFIG.c
 * Author: 23dej
 *
 * Created on April 2, 2025, 6:12 PM
 */


#include "xc.h"
#include "CONFIG.h"
#include "VEML3328.h"
#include "WIFI.h"
#include "SERVO.h"


void configureOscillator()
{
    CLKDIV = 0;  // Set frequency divider to zero
    __builtin_write_OSCCONH(0x01);  // Set FRC with PLL 
    __builtin_write_OSCCONL(OSCCON | 0x01);  // Confirm change 
    while (OSCCONbits.OSWEN);  // Wait until changes are done
}

void configurePPS()
{
    __builtin_write_OSCCONL(OSCCON & ~(1 << 6));  // PPS unlock

    RPINR18bits.U1RXR = 14; // 14 RX connected to TX of ESP8266EX
    RPOR5bits.RP10R = 3;    // 9 TX connected to RX of ESP8266EX
    RPOR2bits.RP5R = 18;        // Map OC1 to RB5
    RPOR5bits.RP11R = 0x0007;    //RB11->SPI1:SDO1
    
    __builtin_write_OSCCONL(OSCCON | (1 << 6));  // PPS lock
}


void configureIO() 
{
    AD1PCFG = 0xFFFF; 
    TRISB = 0x0000;
    TRISBbits.TRISB14 = 1; // Set RX pin as input pin
    TRISBbits.TRISB10 = 0; // Set TX pin as output pin 
    
    TRISAbits.TRISA4 = 0; // Set enable pin as output pin
    LATAbits.LATA4 = 1; // Set enable pin to high voltage
    
    TRISBbits.TRISB6 = 0; // set LED as output
    TRISBbits.TRISB7 = 1; // set BUTTON as input
    
    LATBbits.LATB6 = 0;    
    
    INTCON2bits.INT0EP = 1; // falling edge
    IEC0bits.INT0IE = 1; // interrupt enable 
    IFS0bits.INT0IF = 0; // interrupt flag clear
    IPC0bits.INT0IP = 7; // highest priority
    
    
}

void stopAll()
{
    flag = 1;
    LATBbits.LATB6 = 0;
    moveMiddle();
    __delay_ms(100);
    /* this updated: rgb led off */
    /*WS2812_Set_Color(0,0,0);
    WS2812_Set_Color(0,0,0);
    WS2812_Set_Color(0,0,0);*/
    sendData("R\r\n");
}

void configureAllComponents()
{
    configureOscillator();    
    configureIO();
    configurePPS();
    colorClickConfiguration();
    outputCompareConfig();
    __delay_ms(1000);
}

void __attribute__((interrupt, auto_psv)) _INT0Interrupt(void) 
{
    IEC0bits.INT0IE = 0;
    
    flag = !flag;
    if(flag)
    {
        stopAll();        
    }   
    else
    {
        sendData("S\r\n");
    }
    IEC0bits.INT0IE = 1;
    IFS0bits.INT0IF = 0;  // Clear INT0 flag
}
