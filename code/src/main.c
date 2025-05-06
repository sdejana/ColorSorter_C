/******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 *****************************************************************************
 *
 * @file    main.c
 * @brief   Application entry point and main processing loop.
 *
 * @details
 *   Initializes system components, sets up Wi-Fi connection, and enters
 *   an infinite loop where color sensor data is read and processed. LED
 *   indicates active sensing, and servo is centered between measurements.
 *
 *****************************************************************************/

#pragma config FNOSC = FRCPLL    // FRC with PLL enabled
#pragma config FCKSM = CSECMD    // Clock switching enabled, fail-safe disabled
#pragma config OSCIOFNC = OFF    // OSC2/CLKO is not a clock output
#pragma config POSCMOD = NONE    // Primary oscillator disabled
#pragma config IESO = OFF        // Two-speed start-up disabled
#pragma config JTAGEN = OFF      // JTAG disabled
#pragma config FWDTEN = OFF      // Watchdog timer disabled

#include <xc.h>
#include <string.h>
#include <stdlib.h>
#define FCY 16000000UL
#include <libpic30.h>
#include "CONFIG.h"
#include "WIFI.h"
#include "SERVO.h"

/**
 * @brief   Main application function.
 *
 * @details
 *   Configures all hardware components, establishes UART connection,
 *   and sets initial color buffer values. Then continuously polls
 *   Wi-Fi for commands, toggles LED, reads color data, processes it,
 *   and returns servo to middle position.
 *
 * @return  int Always returns 0 (never reached).
 */
int main(void)
{
    int i = 0;

    /* Initialize all system components */
    configureAllComponents();
    configureU1();
    setupConnection();

    /* Preload color buffer codes */
    colorBuffer[0] = 'A'; // red
    colorBuffer[1] = 'c'; // green
    colorBuffer[2] = 'b'; // blue
    colorBuffer[3] = 'D'; // yellow
    colorBuffer[4] = 'e'; // pink
    colorBuffer[5] = 'F'; // orange
    colorBuffer[6] = 'G'; // black
    colorBuffer[7] = 'h'; // white

    /* Main processing loop */
    while (1)
    {
        pollWiFi();

        if (flag == 0)
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
