/*
 * File:   main.c
 * Author: Korisnik
 *
 * Created on 14. april 2025., 10.49
 */

#pragma config FNOSC = FRCPLL    // FRC sa PLL omogucen
#pragma config FCKSM = CSECMD // Clock switching enabled, fail-safe clock monitor disabled
#pragma config OSCIOFNC = OFF  // OSC2/CLKO is not a clock output
#pragma config POSCMOD = NONE  // Primary oscillator disabled
#pragma config IESO = OFF      // Two-speed oscillator start-up disabled
#pragma config JTAGEN = OFF    // JTAG disabled
#pragma config FWDTEN = OFF    // Watchdog timer disabled

<<<<<<< HEAD
#include <xc.h>      // Ukljucivanje osnovne biblioteke za mikrokontroler
#include <string.h>  // Biblioteka za rad sa stringovima
#include <stdlib.h>  // Standardna biblioteka 
#define FCY 16000000UL // Definisanje frekvencije CPU-a
#include "CONFIG.h"
#include <libpic30.h> // Biblioteka za kasnjenja (__delay_ms i __delay_us)
#include "WIFI.h"
#include "SERVO.h"
=======
#include "xc.h"
#define FCY 16000000UL
#include <libpic30.h>
>>>>>>> d0e666fc1c7724add0847dafa29d1f83833eea80
#include "CONFIG.h"

#pragma config FNOSC = FRCPLL     // Use Internal Fast RC Oscillator (FRC) without PLL
#pragma config FCKSM = CSECMD // Clock switching enabled, fail-safe clock monitor disabled
#pragma config OSCIOFNC = OFF  // OSC2/CLKO is not a clock output
#pragma config POSCMOD = NONE  // Primary oscillator disabled
#pragma config IESO = OFF      // Two-speed oscillator start-up disabled
#pragma config JTAGEN = OFF    // JTAG disabled
#pragma config FWDTEN = OFF    // Watchdog timer disabled

#define SERVO_LEFT 4600      // 1ms pulse (0°)
#define SERVO_MIDDLE 3150   // 1.5ms pulse (90°) 3150
#define SERVO_RIGHT 1400      // 2ms pulse (180°)

void processDataFromColorSensor(const char* data) 
{
    //if (flag == 1) return; 
    
    // Check for each color and corresponding movement (direction)
       
    if(strcmp(data, "RED") == 0) 
    {
        if(colorBuffer[0] == 'A') {
            moveRight();
            //sendData(RED_R); 
        } else if(colorBuffer[0] == 'a') {
            moveLeft();
            //sendData(RED_L);
        }
    }
    else if(strcmp(data, "GREEN") == 0) 
    {
        if(colorBuffer[1] == 'C') {
            moveRight();
            //sendData(GREEN_R);
        } else if(colorBuffer[1] == 'c') {
            moveLeft();
            //sendData(GREEN_L);
        }
    }
    else if(strcmp(data, "BLUE") == 0) 
    {
        if(colorBuffer[2] == 'B') {
            moveRight();
            //sendData(BLUE_R);
        } else if(colorBuffer[2] == 'b') {
            moveLeft();
            //sendData(BLUE_L);
        }
    }
    else if(strcmp(data, "YELLOW") == 0) 
    {
        if(colorBuffer[3] == 'D') {
            moveRight();
            //sendData(YELLOW_R);
        } else if(colorBuffer[3] == 'd') {
            moveLeft();
            //sendData(YELLOW_L);
        }
    }
    else if(strcmp(data, "PINK") == 0) 
    {
        if(colorBuffer[4] == 'E') {
            moveRight();
            //sendData(PURPLE_R);
        } else if(colorBuffer[4] == 'e') {
            moveLeft();
            //sendData(PURPLE_L);
        }
    }
    else if(strcmp(data, "ORANGE") == 0) 
    {
        if(colorBuffer[5] == 'F') {
            moveRight();
            //sendData(ORANGE_R);
        } else if(colorBuffer[5] == 'f') {
            moveLeft();
            //sendData(ORANGE_L);
        }
    }
    else if(strcmp(data, "BLACK") == 0) 
    {
        if(colorBuffer[6] == 'G') {
            moveRight();
            //sendData(BLACK_R);
        } else if(colorBuffer[6] == 'g') {
            moveLeft();
            //sendData(BLACK_L);
        }
    }
    else if(strcmp(data, "WHITE") == 0) 
    {
        if(colorBuffer[7] == 'H') {
            moveRight();
            //sendData(WHITE_R);
        } else if(colorBuffer[7] == 'h') {
            moveLeft();
            //sendData(WHITE_L);
        }
    }
    else {
        moveMiddle();
    }
    __delay_ms(100);
    moveMiddle();
}

int main(void)
{
<<<<<<< HEAD
    configureOscillator();
    configureAllComponents();
  
    while(1)
    {
        
        /*char* color = readColor();
=======
    
    configureAllComponents();
    TRISBbits.TRISB6 = 0;
    LATBbits.LATB6 = 1;    
    colorBuffer[0] = 'A'; //red
    colorBuffer[1] = 'c'; //green
    colorBuffer[2] = 'b'; //blue
    colorBuffer[3] = 'D'; //yellow
    colorBuffer[4] = 'e';   //pink
    colorBuffer[5] = 'F'; // orange
    colorBuffer[6] = 'G'; // black
    colorBuffer[7] = 'h';    //white
    
    
    
    
    while(1)
    {
        
         __delay_ms(200);
        char* color = readColor();
        
>>>>>>> d0e666fc1c7724add0847dafa29d1f83833eea80
        if(strcmp(color,"NO")!=0)
        {
            __delay_ms(100);
            processDataFromColorSensor(color);
<<<<<<< HEAD
        }*/
        
        LATBbits.LATB6 = 1;
        __delay_ms(500);
        LATBbits.LATB6 = 0;
        __delay_ms(500);
        
        moveLeft();
        __delay_ms(500);
        moveMiddle();
        __delay_ms(500);
=======
            LATBbits.LATB6 = ~LATBbits.LATB6;
        }
        
>>>>>>> d0e666fc1c7724add0847dafa29d1f83833eea80
        
    }
    return 0;
}
