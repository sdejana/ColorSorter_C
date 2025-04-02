/*
 * File:   WIFI.c
 * Author: 23dej
 *
 * Created on April 2, 2025, 4:28 PM
 */

#include "WIFI.h"
#include <string.h>
#include <stdlib.h>
#include <xc.h>
#include <libpic30.h>

const char* CMD_AT = "AT\r\n";
const char* CMD_RST = "AT+RST\r\n";
const char* CMD_MODE = "AT+CWMODE=1\r\n";
const char* CMD_LIST_AP = "AT+CWLAP\r\n";
const char* CMD_WIFI_CONN = "AT+CWJAP=\"etfbl.net\",\"\"\r\n";
const char* CMD_CONN_TYPE = "AT+CIPMUX=0\r\n";
const char* CMD_START_TCP = "AT+CIPSTART=\"TCP\",\"10.99.131.223\",8084\r\n";
const char* CMD_SEND = "AT+CIPSEND=3\r\n";
const char* CMD_IP = "AT+CIFSR\r\n";

// Mapping colors to codes
const char* RED_R = "A\r\n";
const char* BLUE_R = "B\r\n";
const char* YELLOW_R = "D\r\n";
const char* GREEN_R = "C\r\n";
const char* PURPLE_R = "E\r\n";
const char* ORANGE_R = "F\r\n";
const char* WHITE_R = "H\r\n";
const char* BLACK_R = "G\r\n";

const char* RED_L = "a\r\n";
const char* BLUE_L = "b\r\n";
const char* YELLOW_L = "d\r\n";
const char* GREEN_L = "c\r\n";
const char* PURPLE_L = "e\r\n";
const char* ORANGE_L = "f\r\n";
const char* WHITE_L = "h\r\n";
const char* BLACK_L = "g\r\n";

volatile char buffer[BUF_SIZE] = {0};  // Recieving data buffer
volatile unsigned head = 0, tail = 0; // Pointers to head & tail 
int flag = 0;

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt() 
{
    IFS0bits.U1RXIF = 0;  // Reset interrupt flag
    
    if (U1STAbits.URXDA)  // Check if there are data incoming
    { 
        buffer[head] = U1RXREG; // Put data in buffer
        head = (head + 1) % BUF_SIZE;   // Update pointer
    }  
    
    if(strstr((const char*)buffer, "R") != NULL)
    {
        flag = 1; // Set flag if program is reseted
    }
    else if(strstr((const char*)buffer, "S") != NULL)
    {
        flag = 0; // Set flag if program is started
    }
}

// Interrupt routine for managing errors
void __attribute__((interrupt, no_auto_psv)) _U1ErrInterrupt() 
{
    IFS4bits.U1ERIF = 0; // Reset interrupt flag
    U1STAbits.OERR = 0;  // Reset overrun error flag
}

void sendCommand(const char* command) 
{
    while (*command)  // Loop continues until the whole command isn't sent
    {
        while (U1STAbits.UTXBF); // Wait until buffer isn't empty
        U1TXREG = *command++;  // Send one character
    }
    __delay_ms(1000);  // Small delay - waiting for response
}

void U1_Write_Char(char c) 
{
    while (U1STAbits.UTXBF);  // Wait until buffer isn't empty
    U1TXREG = c;  // Send character
}

void U1_Write_Text(const char* text) 
{
    while (*text)  // Send string
    {
        U1_Write_Char(*text++);
    }
}

void cleanBuffer()
{
    for(int i = 0; i < BUF_SIZE; i++)
    {
        buffer[i] = 0;
    }
    head = 0;
    tail = 0;
}

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
 
    __builtin_write_OSCCONL(OSCCON | (1 << 6));  // PPS lock
}

void configureIO() 
{
    TRISBbits.TRISB14 = 1; // Set RX pin as input pin
    TRISBbits.TRISB10 = 0; // Set TX pin as output pin 
    
    TRISAbits.TRISA4 = 0; // Set enable pin as output pin
    LATAbits.LATA4 = 1; // Set enable pin to high voltage
    
}

void configureU1() 
{
    U1MODEbits.BRGH = BRGH_VAL; // Set high speed mode
    U1BRG = BRG_VAL; // Set baud rate to 115200
    U1MODEbits.UARTEN = 1; // Enable UART1
    U1STAbits.UTXEN = 1; // Enable TX

    IPC2bits.U1RXIP = 5; // Set RX priority
    IFS0bits.U1RXIF = 0; // Reset RX interrupt flag
    IEC0bits.U1RXIE = 1; // Enable RX 
}

void setupConnection()
{
    cleanBuffer();
    sendCommand(CMD_AT); 
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_RST);
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_MODE);
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_WIFI_CONN);
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_CONN_TYPE);
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_IP);
    __delay_ms(100);
}

void sendData(const char* data)
{
    sendCommand(CMD_START_TCP);
    __delay_ms(500);
    
    cleanBuffer();

    sendCommand(CMD_SEND);
    __delay_ms(100); 
    
    sendCommand(data);
    __delay_ms(100);
}
