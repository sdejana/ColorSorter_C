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
#include <libpic30.h> // Biblioteka za kasnjenja (__delay_ms i __delay_us)
#include "WIFI.h"
#include "SERVO.h"
volatile uint8_t flag = 0;

void toggleLED()
{
    LATBbits.LATB6 = ~LATBbits.LATB6;
}

/*void __attribute__((interrupt, auto_psv)) _CNInterrupt(void){
    IEC1bits.CNIE = 0; // disable interrupt for little amount of time
    LATBbits.LATB6 = ~LATBbits.LATB6;
    flag = 1;
   // __delay_ms(50); // software debouncing 
    IEC1bits.CNIE = 1;
    IFS1bits.CNIF = 0;
}*/

//sa int0
/*
void __attribute__((interrupt, auto_psv)) _INT0Interrupt(void) {
    
    LATBbits.LATB6 = ~LATBbits.LATB6; // Just for debugging
    flag = ~flag;  // program flag
    IFS0bits.INT0IF = 0;  // Clear INT0 flag
}*/

int main() 
{
    configureOscillator();
    
    //configureOscillatorServo();
    configureIO();
    configurePPS();
    configureU1();
    setupConnection();
    //configurePins();
  
    output_compare_config();
    __delay_ms(1000);           // Wait for stable PWM
    // timer1_config();
    
    //__builtin_enable_interrupts();
    
   
    
    while (1) 
    {
        moveMiddle();
        
        __delay_ms(500); // simulation of reading from color sensor
        
        sendData(GREEN_R); // send through wifi reading
        __delay_ms(1000);
        
        moveLeft();
        __delay_ms(500);
        
        moveMiddle();
        __delay_ms(500);
        
         sendData(PURPLE_R); // send through wifi reading
        __delay_ms(1000);
        
        moveRight();
        __delay_ms(500);
        
    }
    return 0;
}