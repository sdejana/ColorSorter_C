/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    CONFIG.c
 * 
 * @details
 *   Implements configuration of oscillator, peripheral pin select (PPS),
 *   I/O ports, and overall component setup. Also includes the external
 *   interrupt service routine for button presses.
 *
 ******************************************************************************/

#include "xc.h"
#include "CONFIG.h"
#include "VEML3328.h"
#include "WIFI.h"
#include "SERVO.h"

/**
 * @brief   Configures the system oscillator.
 *
 * @details
 *   Sets the frequency divider to zero, selects Fast RC oscillator with PLL,
 *   confirms the change, and waits until the oscillator switch completes.
 */
void configureOscillator()
{
    CLKDIV = 0;  // Set frequency divider to zero
    __builtin_write_OSCCONH(0x01);  // Set FRC with PLL
    __builtin_write_OSCCONL(OSCCON | 0x01);  // Confirm change
    while (OSCCONbits.OSWEN);  // Wait until changes are done
}

/**
 * @brief   Configures peripheral pin select (PPS).
 *
 * @details
 *   Unlocks PPS, maps UART RX/TX pins for ESP8266 communication, maps
 *   OC1 output compare to RB5, and sets SPI1 SDO on RB11, then locks PPS.
 */
void configurePPS()
{
    __builtin_write_OSCCONL(OSCCON & ~(1 << 6));  // PPS unlock

    RPINR18bits.U1RXR = 14; // 14 RX connected to TX of ESP8266EX
    RPOR5bits.RP10R = 3;    // 9 TX connected to RX of ESP8266EX
    RPOR2bits.RP5R = 18;    // Map OC1 to RB5
    RPOR5bits.RP11R = 0x0007;    // RB11 -> SPI1:SDO1
    
    __builtin_write_OSCCONL(OSCCON | (1 << 6));  // PPS lock
}

/**
 * @brief   Configures I/O ports and interrupt settings.
 *
 * @details
 *   Disables analog functions on Port B, sets data direction for RX,
 *   TX, enable, LED, and button pins. Initializes LAT registers and
 *   configures INT0 external interrupt on falling edge with highest priority.
 */
void configureIO()
{
    AD1PCFG = 0xFFFF;
    TRISB = 0x0000;
    TRISBbits.TRISB14 = 1; // Set RX pin as input pin
    TRISBbits.TRISB10 = 0; // Set TX pin as output pin
    
    TRISAbits.TRISA4 = 0; // Set enable pin as output pin
    LATAbits.LATA4 = 1;   // Set enable pin to high voltage
    
    TRISBbits.TRISB6 = 0; // Set LED as output
    TRISBbits.TRISB7 = 1; // Set BUTTON as input
    
    LATBbits.LATB6 = 0;
    
    INTCON2bits.INT0EP = 1; // Falling edge
    IEC0bits.INT0IE = 1;    // Interrupt enable
    IFS0bits.INT0IF = 0;    // Interrupt flag clear
    IPC0bits.INT0IP = 7;    // Highest priority
}

/**
 * @brief   Stops all operations and resets actuators.
 *
 * @details
 *   Sets the global flag, turns off LED, moves servo to middle position,
 *   turns off RGB LED, and sends stop command over Wi-Fi.
 */
void stopAll()
{
    flag = 1;
    LATBbits.LATB6 = 0;
    moveMiddle();
    __delay_ms(100);
    /* Turn off RGB LED */
    WS2812_Set_Color(0,0,0);
    WS2812_Set_Color(0,0,0);
    WS2812_Set_Color(0,0,0);
    sendData("R\r\n");
}

/**
 * @brief   Configures all components on startup.
 *
 * @details
 *   Calls oscillator, I/O, PPS configurations, sets up color click,
 *   output compare, and delays for stabilization.
 */
void configureAllComponents()
{
    configureOscillator();    
    configureIO();
    configurePPS();
    colorClickConfiguration();
    outputCompareConfig();
    __delay_ms(1000);
}

/**
 * @brief   External interrupt 0 service routine.
 *
 * @details
 *   Toggles the global flag, disables button interrupt, calls stop or start
 *   commands, then clears and re-enables the interrupt.
 */
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
