#ifndef _WIFI_H
#define	_WIFI_H

/**
 * @file wifi.h
 * @brief Header file for WiFi communication and control using ESP8266.
 */

#define FCY 16000000UL /**< CPU frequency */

#include <xc.h>     /**< Include processor-specific definitions */
#include <stdlib.h> /**< Standard library for general utilities */
#include <string.h> /**< Standard library for string operations */
#include <libpic30.h> /**< Library for dsPIC30/dsPIC33 */

#define BRGH_VAL 1          /**< High-speed baud rate mode */
#define BRG_VAL 34          /**< Baud rate configuration value for 115200 bps */
#define BUF_SIZE 80         /**< Size of the receive buffer */
#define COLOR_NUM  8        /**< Number of color codes supported */

/**
 * @brief Buffer used to store received UART data.
 */
extern volatile char buffer[BUF_SIZE];

/**
 * @brief Head index of the buffer.
 */
extern volatile unsigned head;

/**
 * @brief Tail index of the buffer.
 */
extern volatile unsigned tail;

/**
 * @brief General-purpose status flag.
 */
extern volatile uint8_t flag;

/**
 * @brief Flag indicating that a complete message is ready.
 */
extern volatile uint8_t messageReady;

/**
 * @brief Buffer for storing color-related data.
 */
extern volatile char colorBuffer[COLOR_NUM+1];

/**
 * @brief AT command: Basic attention command.
 */
extern const char* CMD_AT;

/**
 * @brief AT command: Reset the module.
 */
extern const char* CMD_RST;

/**
 * @brief AT command: Set operation mode.
 */
extern const char* CMD_MODE;

/**
 * @brief AT command: List available access points.
 */
extern const char* CMD_LIST_AP;

/**
 * @brief AT command: Connect to WiFi.
 */
extern const char* CMD_WIFI_CONN;

/**
 * @brief AT command: Set connection type.
 */
extern const char* CMD_CONN_TYPE;

/**
 * @brief AT command: Start a TCP connection.
 */
extern const char* CMD_START_TCP;

/**
 * @brief AT command: Send data command.
 */
extern const char* CMD_SEND;

/**
 * @brief AT command: Get IP address.
 */
extern const char* CMD_IP;

/**
 * @brief Color mapping for RED (right).
 */
extern const char* RED_R;
/** @brief Color mapping for BLUE (right). */
extern const char* BLUE_R;
/** @brief Color mapping for YELLOW (right). */
extern const char* YELLOW_R;
/** @brief Color mapping for GREEN (right). */
extern const char* GREEN_R;
/** @brief Color mapping for PURPLE (right). */
extern const char* PURPLE_R;
/** @brief Color mapping for ORANGE (right). */
extern const char* ORANGE_R;
/** @brief Color mapping for WHITE (right). */
extern const char* WHITE_R;
/** @brief Color mapping for BLACK (right). */
extern const char* BLACK_R;

/** @brief Color mapping for RED (left). */
extern const char* RED_L;
/** @brief Color mapping for BLUE (left). */
extern const char* BLUE_L;
/** @brief Color mapping for YELLOW (left). */
extern const char* YELLOW_L;
/** @brief Color mapping for GREEN (left). */
extern const char* GREEN_L;
/** @brief Color mapping for PURPLE (left). */
extern const char* PURPLE_L;
/** @brief Color mapping for ORANGE (left). */
extern const char* ORANGE_L;
/** @brief Color mapping for WHITE (left). */
extern const char* WHITE_L;
/** @brief Color mapping for BLACK (left). */
extern const char* BLACK_L;

/**
 * @brief Bit array representing the current direction state.
 */
extern volatile uint8_t DIRECTION_BIT_ARRAY;

// TODO Insert appropriate #include <>
// TODO Insert C++ class definitions if appropriate
// TODO Insert declarations

/**
 * @brief RX interrupt handler, used to process incoming data and 
 * handle interrupt calls.
 */
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void);

/**
 * @brief Clears buffer overrun and error interrupt flags.
 */
void __attribute__((interrupt, no_auto_psv)) _U1ErrInterrupt(void); 

/**
 * @brief Sends an AT command to the WiFi module and waits for a response.
 * @param command The AT command to be sent.
 */
void sendCommand(const char* command);

/**
 * @brief Sends a single character via UART1.
 * @param ch Character to be sent.
 */
void U1_Write_Char(char ch);

/**
 * @brief Sends a null-terminated string via UART1.
 * @param text String to be sent.
 */
void U1_Write_Text(const char* text);

/**
 * @brief Initializes UART1 with predefined settings.
 */
void configureU1(void);

/**
 * @brief Clears the buffer by initializing all characters to '0'.
 */
void cleanBuffer(void);

/**
 * @brief Configures Peripheral Pin Select (PPS) settings.
 */
void configurePPS(void);

/**
 * @brief Configures GPIO input/output pins.
 */
void configureIO(void);

/**
 * @brief Performs initial setup for WiFi connection.
 */
void setupConnection(void);

/**
 * @brief Sends data to a PC application via WiFi.
 * @param data Null-terminated string of data to be sent.
 */
void sendData(const char* data);

/**
 * @brief Processes received data from the color sensor.
 * @param input The received color sensor data.
 */
void processDataFromColorSensor(const char* input);

/**
 * @brief Parses the received data using a finite state machine.
 */
void parseDataFSM(void); 

/**
 * @brief Polls the WiFi module for new data or status.
 */
void pollWiFi(void);

/**
 * @brief Configures the system oscillator settings.
 */
void configureOscillator(void);

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

// TODO If C++ is being used, regular C code needs function names to have C 
// linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */
