#ifndef _WIFI_H
#define	_WIFI_H

#define FCY 16000000UL // CPU frequency

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdlib.h>
#include <string.h>
#include <libpic30.h>

#define BRGH_VAL 1
#define BRG_VAL 34 // Baud rate of 115200
#define BUF_SIZE 80  // Buffer size
#define COLOR_NUM  8 


extern volatile char buffer[BUF_SIZE];  // Buffer used to store recieved data
extern volatile unsigned head, tail;  // Buffer head and tail
extern volatile uint8_t flag; 
extern volatile uint8_t messageReady;
extern volatile char colorBuffer[COLOR_NUM+1];

// AT commands for ESP8266EX
extern const char* CMD_AT;
extern const char* CMD_RST;
extern const char* CMD_MODE;
extern const char* CMD_LIST_AP;
extern const char* CMD_WIFI_CONN;
extern const char* CMD_CONN_TYPE;
extern const char* CMD_START_TCP;
extern const char* CMD_SEND;
extern const char* CMD_IP;

// Mapping colors to codes
extern const char* RED_R;
extern const char* BLUE_R;
extern const char* YELLOW_R;
extern const char* GREEN_R;
extern const char* PURPLE_R;
extern const char* ORANGE_R;
extern const char* WHITE_R;
extern const char* BLACK_R;

extern const char* RED_L;
extern const char* BLUE_L;
extern const char* YELLOW_L;
extern const char* GREEN_L;
extern const char* PURPLE_L;
extern const char* ORANGE_L;
extern const char* WHITE_L;
extern const char* BLACK_L;

extern volatile uint8_t DIRECTION_BIT_ARRAY;


// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

/*
 * @brief RX interrupt handler, used to process incoming data and 
 * handle interrupt calls. 
 */
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void);

/*
 * @brief Function used for clearing buffer overrun flag and 
 * error interrupt flag.
 * 
 */
void __attribute__((interrupt, no_auto_psv)) _U1ErrInterrupt(void); 

/*
 * @brief Function used for sending AT commands to WIFI and waiting for 
 * predefined amount of time. 
 */
void sendCommand(const char*);

/*
 * @brief Function used for writing a character via UART.
 */
void U1_Write_Char(char);

/*
 * @brief Function used for writing an array of characters via UART.
 */
void U1_Write_Text(const char*);

/*
 * @brief Function used for initializing UART1.
 */
void configureU1(void);

/*
 * @brief Function used for initializing a buffer of characters with '0'.
 */
void cleanBuffer(void);

/*
 * Setup function for connection establishment 
 */
void setupConnection(void);

/*
 * Function used to send data to PC app via WIFI 
 */
void sendData(const char*);

/*
 * Function used to parse data recieved from app
 */
void parseData(void);

/***/

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

