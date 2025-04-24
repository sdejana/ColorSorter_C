/*
 * File:   WIFI.c
 * Author: 23dej
 *
 * Created on April 2, 2025, 4:28 PM
 */

#include "WIFI.h"
#include "SERVO.h"
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
const char* PINK_R = "E\r\n";
const char* ORANGE_R = "F\r\n";
const char* WHITE_R = "H\r\n";
const char* BLACK_R = "G\r\n";

const char* RED_L = "a\r\n";
const char* BLUE_L = "b\r\n";
const char* YELLOW_L = "d\r\n";
const char* GREEN_L = "c\r\n";
const char* PINK_L = "e\r\n";
const char* ORANGE_L = "f\r\n";
const char* WHITE_L = "h\r\n";
const char* BLACK_L = "g\r\n";

volatile char buffer[BUF_SIZE];  // Recieving data buffer
volatile char colorBuffer[COLOR_NUM+1];
volatile unsigned head = 0, tail = 0; // Pointers to head & tail 
volatile uint8_t flag = 0;
volatile uint8_t messageReady = 0;

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt() 
{
    IFS0bits.U1RXIF = 0;  // Reset interrupt flag
    LATBbits.LATB6 = 0;
    if (U1STAbits.URXDA)  // Check if there are data incoming
    { 
        buffer[head] = U1RXREG; // Put data in buffer
        head = (head + 1) % BUF_SIZE;   // Update pointer
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
    __delay_ms(500);  // Small delay - waiting for response
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

void U2_Write_Char(char c) 
{
    while (U2STAbits.UTXBF);  // Wait until buffer isn't empty
    U1TXREG = c;  // Send character
}

void U2_Write_Text(const char* text) 
{
    while (*text)  // Send string
    {
        U2_Write_Char(*text++);
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

void configureU1() 
{
    U1MODEbits.BRGH = BRGH_VAL; // Set high speed mode
    U1BRG = BRG_VAL; // Set baud rate to 115200
    U1MODEbits.UARTEN = 1; // Enable UART1
    U1STAbits.UTXEN = 1; // Enable TX
    
    U2MODEbits.BRGH = BRGH_VAL; // Set high speed mode
    U2BRG = BRG_VAL; // Set baud rate to 115200
    U2MODEbits.UARTEN = 1; // Enable UART1
    U2STAbits.UTXEN = 1; // Enable TX

    IPC2bits.U1RXIP = 6; // Set RX priority
    IFS0bits.U1RXIF = 0; // Reset RX interrupt flag
    IEC0bits.U1RXIE = 1; // Enable RX 
}

void setupConnection()
{
    cleanBuffer();
    sendCommand(CMD_AT); 
    __delay_ms(1000);
    
    cleanBuffer();
    sendCommand(CMD_RST);
    __delay_ms(1000);
    
    cleanBuffer();
    sendCommand(CMD_MODE);
    __delay_ms(1000);
    
    cleanBuffer();
    sendCommand(CMD_WIFI_CONN);
    __delay_ms(1000);
    
    cleanBuffer();
    sendCommand(CMD_CONN_TYPE);
    __delay_ms(3000);
    
    cleanBuffer();
    sendCommand(CMD_IP);
    __delay_ms(1000);
    
    cleanBuffer();
    sendCommand(CMD_START_TCP);
    __delay_ms(2000);
    cleanBuffer();
    sendCommand("K\r\n");
    __delay_ms(1000);
}

void sendData(const char* data)
{
    sendCommand(CMD_START_TCP);
    __delay_ms(400);

    sendCommand(CMD_SEND);
    __delay_ms(100); 

    sendCommand(data);
}

void processDataFromColorSensor(const char* data) 
{
    if (flag == 1) return; 
    
    // Check for each color and corresponding movement (direction)
    if(strcmp(data, "RED") == 0) 
    {
        if(colorBuffer[0] == 'A') 
        {
            moveRight();
            sendData(RED_R); 
        } else if(colorBuffer[0] == 'a') 
        {
            moveLeft();
            sendData(RED_L);
        }
    }
    else if(strcmp(data, "GREEN") == 0) 
    {
        if(colorBuffer[1] == 'C') 
        {
            moveRight();
            sendData(GREEN_R);
        } else if(colorBuffer[1] == 'c') 
        {
            moveLeft();
            sendData(GREEN_L);
        }
    }
    else if(strcmp(data, "BLUE") == 0) 
    {
        if(colorBuffer[2] == 'B') 
        {
            moveRight();
            sendData(BLUE_R);
            
        } else if(colorBuffer[2] == 'b') 
        {
            moveLeft();
            sendData(BLUE_L);
        }
    }
    else if(strcmp(data, "YELLOW") == 0) 
    {
        if(colorBuffer[3] == 'D') 
        {
            moveRight();
            sendData(YELLOW_R);
        } else if(colorBuffer[3] == 'd') 
        {
            moveLeft();
            sendData(YELLOW_L);
        }
    }
    else if(strcmp(data, "PINK") == 0) 
    {
        if(colorBuffer[4] == 'E') 
        {
            moveRight();
            sendData(PINK_R);
        } else if(colorBuffer[4] == 'e') 
        {
            moveLeft();
            sendData(PINK_L);
        }
    }
    else if(strcmp(data, "ORANGE") == 0) 
    {
        if(colorBuffer[5] == 'F') 
        {
            moveRight();
            sendData(ORANGE_R);
        } else if(colorBuffer[5] == 'f') 
        {
            moveLeft();
            sendData(ORANGE_L);
        }
    }
    else if(strcmp(data, "BLACK") == 0) 
    {
        if(colorBuffer[6] == 'G') 
        {
            moveRight();
            sendData(BLACK_R);
        } else if(colorBuffer[6] == 'g') 
        {
            moveLeft();
            sendData(BLACK_L);
        }
    }
    else if(strcmp(data, "WHITE") == 0) 
    {
        if(colorBuffer[7] == 'H') 
        {
            moveRight();
            sendData(WHITE_R);
        } else if(colorBuffer[7] == 'h') 
        {
            moveLeft();
            sendData(WHITE_L);
        }
    }
    else 
    {
        sendData("K\r\n");
    }
}

void parseDataFSM() 
{
    uint8_t i = 0;
    uint8_t inData = 0;
    
    for (unsigned pos = 0; pos < BUF_SIZE && buffer[pos] != '\0'; pos++) 
    {
        char c = buffer[pos];

        if (c == '0') 
        {
            inData = 1;
            i = 0;
            continue;
        } 
        else if (c == '1') 
        {
            break;
        } 
        else if (inData && c != '#' && c != '\r' && c != '\n' && i < COLOR_NUM) 
        {
            if ((c >= 'A' && c <= 'H') || (c >= 'a' && c <= 'h'))
            {
                colorBuffer[i++] = c;
            }
        }
    }

    if (i < COLOR_NUM) 
    {
        colorBuffer[i] = '\0';
    }

    cleanBuffer();
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
            parseDataFSM();
        }
}