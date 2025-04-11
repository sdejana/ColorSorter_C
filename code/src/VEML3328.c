/*
 * File:   VEML3328.c
 * Author: Korisnik
 *
 * Created on 10. april 2025., 13.07
 */


#include "xc.h"
#include "VEML3328.h"
#include "system.h"


volatile uint16_t red_vector  = 0;
volatile uint16_t green_vector  = 0;
volatile uint16_t blue_vector  = 0;
volatile uint8_t interrupt_detected = 0;


uint16_t red_value = 0;
uint16_t green_value = 0;
uint16_t blue_value = 0;


typedef struct {
    const char* name;
    uint16_t red_shade, green_shade, blue_shade;
} Color;

const Color colors[] = { // YET TO BE CALIBRATED
    {"GREEN",     6500, 29000, 28000},
    {"BLUE",      8800, 45100, 65500},
    {"RED",    40500, 21800, 24400},
    {"YELLOW",      45000, 65535, 40500},
    {"ORANGE",40700, 18800, 14500},
    {"PINK",      34100, 20200, 29800},
    {"BLACK",       6000, 12300, 16500},
    {"WHITE",    48900, 65535, 65535},
    {"BROWN",     8000, 11800, 14700},
    {"NO",         1200,  2000,  2000}
};

// local functions

void set_slave()
{
    I2C_start();    
    I2C_write((0x10 << 1) | 0); // Slave address + Write     
    I2C_write(0x00); // command code    
    I2C_write(0x00); // DBL
    I2C_write(0x08); // DBH             
    I2C_stop();
    __delay_us(10);
}

float calculate_distance(uint16_t r1, uint16_t g1, uint16_t b1, uint16_t r2, uint16_t g2, uint16_t b2)
{    
    return sqrtf(powf(r1 - r2, 2) + powf(g1 - g2, 2) + powf(b1 - b2, 2));    
}

char* detect_color(void)
{    
    uint16_t red_shade = red_vector;
    uint16_t green_shade = green_vector;
    uint16_t blue_shade = blue_vector;

    float min_distance = 1e9; 
    const char* detected_color = "NN";

    for (int i = 0; i < COLOR_QUANTITY; i++) {
        float distance = calculate_distance(red_shade, green_shade, blue_shade, colors[i].red_shade, colors[i].green_shade, colors[i].blue_shade);
        if (distance < min_distance) {
            min_distance = distance;
            detected_color = colors[i].name;
        }
    }
    
    return detected_color;  
}

void WS2812_Send_Byte(uint8_t byte) // YET TO BE CALIBRATED
{
    for (int i = 7; i >= 0; i --) 
    {        
        uint8_t spi_data = (byte & (1 << i)) ? 0b11110000 : 0b11000000;        
        SPI1_Exchange8bit(spi_data);  
        __delay_ms(100);
        LATBbits.LATB6 = 1;
        __delay_ms(100);
        LATBbits.LATB6 = 0;
    }
}

void I2C_free_stuck_slave()
{    
    I2C1CONbits.I2CEN = 0;
    __delay_us(10);    
    TRISBbits.TRISB9 = 0;  // SDA as output
    TRISBbits.TRISB8 = 0;  // SCL as output
    LATBbits.LATB9 = 1;  // Keep SDA HIGH    
    for (int i = 0; i < 9; i++) {
        LATBbits.LATB8 = 1;  // SCL HIGH
        __delay_us(5);
        LATBbits.LATB8 = 0;  // SCL LOW
        __delay_us(5);
    }
    // Force a STOP condition to tell the slave that communication has ended
    TRISBbits.TRISB9 = 0;  // SDA as output
    TRISBbits.TRISB8 = 0;  // SCL as output
    LATBbits.LATB9 = 0;  // SDA LOW
    __delay_us(5);
    LATBbits.LATB8 = 1;  // SCL HIGH
    __delay_us(5);
    LATBbits.LATB9 = 1;  // SDA HIGH (STOP condition)
    __delay_us(5);
    // Restore SDA and SCL to input mode
    TRISBbits.TRISB9 = 1;  // SDA as input
    TRISBbits.TRISB8 = 1;  // SCL as input
    // Re-enable I2C module
    I2C1CONbits.I2CEN = 1;
    __delay_us(10);
}

void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void)
{
    interrupt_detected = 1;
    IFS1bits.MI2C1IF = 0;  // Clear the interrupt flag         
}

void I2C_stop()
{
    interrupt_detected = 0;
    I2C1CONbits.PEN = 1;  // Stop condition
    int count = 10000;
    while (!interrupt_detected && --count > 0);
    if (count == 0) {
        LATBbits.LATB6 = 1;
        while(1);
    }  
}

void I2C_start()
{     
    interrupt_detected = 0;
    I2C1CONbits.SEN = 1;  // Start condition                           
    int count = 10000;
    while (!interrupt_detected && --count > 0);
    if (count == 0) {
        LATBbits.LATB6 = 1;
        while(1);
    }           
}

void I2C_receive_enable()
{    
    I2C1CON = I2C1CON & 0xFFE0; // set the last 5 bits of I2C1CON to 0
    interrupt_detected = 0;
    I2C1CONbits.RCEN = 1;  // Enable Receive mode
    int count = 10000;
    while (!interrupt_detected && --count > 0);
    if (count == 0) {
        LATBbits.LATB6 = 1;
        while(1);
    }  
}

void I2C_write(uint8_t data)
{          
    interrupt_detected = 0;
    I2C1TRN = data;                 
    int count = 10000;
    while (!interrupt_detected && --count > 0);
    if (count == 0) {
        LATBbits.LATB6 = 1;
        while(1);
    }  
}

void package_receive(uint16_t* receive_vector)
{
    I2C_receive_enable();  // Enable receiving
    uint16_t lsb = I2C1RCV;  // Read LSB    
    I2C1CONbits.ACKDT = 0;  // Send ACK
    I2C1CONbits.ACKEN = 1;
    while (I2C1CONbits.ACKEN);  // Wait for ACK sequence to finish    
    I2C_receive_enable();  // Enable receiving again
    uint16_t msb = I2C1RCV;  // Read MSB
    I2C1CONbits.ACKDT = 1;  // Send NACK
    I2C1CONbits.ACKEN = 1;
    while (I2C1CONbits.ACKEN);  // Wait for NACK sequence to finish  
    *receive_vector = (msb << 8) | lsb;  // Correctly combine MSB and LSB
}


void request_data(uint8_t command_code, uint16_t* color_vector)
{
    I2C_start();
    I2C_write((0x10 << 1) | 0); // slave address + write
    I2C_write(command_code);
    I2C_start();
    I2C_write((0x10 << 1) | 1); // slave address + read
    package_receive(color_vector);
    I2C_stop();
    __delay_us(10);
}

// global functions


void WS2812_Set_Color(uint8_t red, uint8_t green, uint8_t blue)
{    
    SPI1STATbits.SPIEN = 0;
    __delay_us(100);
    SPI1STATbits.SPIEN = 1;
    
    WS2812_Send_Byte(green);
    WS2812_Send_Byte(red);
    WS2812_Send_Byte(blue);
     
    SPI1STATbits.SPIEN = 0;
    __delay_us(100);
    SPI1STATbits.SPIEN = 1;     
}

void I2C_config()
{  
    I2C_free_stuck_slave();
    __delay_us(100);
    I2C1CONbits.I2CEN = 0;  // Disable I2C module    
    IFS1bits.MI2C1IF = 0;  // Clear I2C interrupt flag
    IEC1bits.MI2C1IE = 1;  // Enable I2C master interrupt
    IPC4bits.MI2C1IP = 6;  // Set high priority
    //I2C1BRG = 0x27;
    I2C1BRG = 0x9D; // baud rate for 16 MHz frequency
    I2C1CONbits.I2CEN = 1;  // Enable I2C module  
    __delay_us(10);
    set_slave();
}


char* read_color(void)
{
    
    uint16_t sum_red = 0, sum_green = 0, sum_blue = 0;
    uint16_t avg_red, avg_green, avg_blue;    
    
    
    __delay_ms(1000);
       
    
    for (int i = 0; i < NUM_OF_SAMPLES; i++) { 
                
        
        request_data(0x05,&red_vector);                
        
        request_data(0x06,&green_vector);                 
               
        request_data(0x07,&blue_vector);                  
        
        sum_red += red_vector;

        sum_green += green_vector;

        sum_blue += blue_vector;

        __delay_ms(100);     
    }
            
    avg_red = sum_red / NUM_OF_SAMPLES;
    avg_green = sum_green / NUM_OF_SAMPLES;
    avg_blue = sum_blue / NUM_OF_SAMPLES;
    
    red_value = avg_red;
    blue_value = avg_blue;
    green_value = avg_green;   
    
    return detect_color();    
}

void color_click_configuration()
{
    SYSTEM_Initialize();
    I2C_config();
}