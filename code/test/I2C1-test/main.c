/*
 * File:   main_2.c
 * Author: Korisnik
 *
 * Created on 18. mart 2025., 13.28
 */

#include "xc.h"
#define FCY 16000000UL
#define COLOR_QUANTITY 9
#define NUM_OF_SAMPLES 10
#include <p24FJ64GA002.h>
#include <libpic30.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "mcc_generated_files/system.h"


unsigned int i;
#define DELAY_105uS asm volatile ("REPEAT, #4201"); Nop(); // 105uS delay


#pragma config FNOSC = FRCPLL     // Use Internal Fast RC Oscillator (FRC) without PLL
#pragma config FCKSM = CSECMD // Clock switching enabled, fail-safe clock monitor disabled
#pragma config OSCIOFNC = OFF  // OSC2/CLKO is not a clock output
#pragma config POSCMOD = NONE  // Primary oscillator disabled
#pragma config IESO = OFF      // Two-speed oscillator start-up disabled
#pragma config JTAGEN = OFF    // JTAG disabled
#pragma config FWDTEN = OFF    // Watchdog timer disabled

volatile uint16_t red_vector  = 0;
volatile uint16_t green_vector  = 0;
volatile uint16_t blue_vector  = 0;
volatile uint8_t interrupt_detected = 0;


volatile uint16_t red_value = 0;
volatile uint16_t green_value = 0;
volatile uint16_t blue_value = 0;


typedef struct {
    const char* name;
    uint16_t red_shade, green_shade, blue_shade;
} Color;

const Color colors[] = { // YET TO BE CALIBRATED
    {"GREEN",     103, 100, 80},
    {"BLUE",      7, 70, 100},
    {"RED",    90, 60, 100},
    {"YELLOW",      20, 50, 30},
    {"ORANGE",      105, 52, 50},
    {"PINK",      20, 50, 70},
    {"BLACK",       143, 170, 80},
    {"WHITE",    30, 50, 100},    
    {"NO",         192,  120,  170}
};


void configure_oscillator()
{
    CLKDIV = 2; 
    __builtin_write_OSCCONH(0x01);  
    __builtin_write_OSCCONL(OSCCON | 0x01);  
    while (OSCCONbits.OSWEN);  
}


void I2C_config() {  
    I2C1CONbits.I2CEN = 0;  // Disable I2C module    
    IFS1bits.MI2C1IF = 0;  // Clear I2C interrupt flag
    IEC1bits.MI2C1IE = 1;  // Enable I2C master interrupt
    IPC4bits.MI2C1IP = 6;  // Set high priority
    //I2C1BRG = 0x27;
    I2C1BRG = 0x9D; // baud rate for 16 MHz frequency
    I2C1CONbits.I2CEN = 1;  // Enable I2C module  
    __delay_us(10);
}

void I2C_free_stuck_slave() {
    // Disable I2C to reset all registers
    I2C1CONbits.I2CEN = 0;
    __delay_us(10);
    // Set SDA & SCL as outputs to manually generate pulses
    TRISBbits.TRISB9 = 0;  // SDA as output
    TRISBbits.TRISB8 = 0;  // SCL as output
    LATBbits.LATB9 = 1;  // Keep SDA HIGH
    // Generate 9 clock pulses on SCL to force slave to release SDA
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


void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void) {
    interrupt_detected = 1;
    IFS1bits.MI2C1IF = 0;  // Clear the interrupt flag         
}

uint8_t color_to_display = 'n';

void I2C_stop() {
    interrupt_detected = 0;
    I2C1CONbits.PEN = 1;  // Stop condition
    int count = 10000;
    while (!interrupt_detected && --count > 0);
    if (count == 0) {
        LATBbits.LATB6 = 1;
        while(1);
    }  
}

void I2C_start() {     
    interrupt_detected = 0;
    I2C1CONbits.SEN = 1;  // Start condition                           
    int count = 10000;
    while (!interrupt_detected && --count > 0);
    if (count == 0) {
        LATBbits.LATB6 = 1;
        while(1);
    }  
          
}
    
void I2C_receive_enable() {    
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

void I2C_write(uint8_t data) {          
    interrupt_detected = 0;
    I2C1TRN = data;                 
    int count = 10000;
    while (!interrupt_detected && --count > 0);
    if (count == 0) {
        LATBbits.LATB6 = 1;
        while(1);
    }  

}

void set_slave(){
    I2C_start();    
    I2C_write((0x10 << 1) | 0); // Slave address + Write     
    I2C_write(0x00); // command code    
    I2C_write(0x00); // DBL
    I2C_write(0x08); // DBH             
    I2C_stop();
    __delay_us(10);
}

void package_transmitting(uint8_t register_address) {          
    I2C_start();                
    I2C_write((0x10 << 1) | 0); // Slave address + Write    
    I2C_write(register_address); // Register address
    I2C_start(); // Repeated Start
    I2C_write((0x10 << 1) | 1); // Slave address + Read    
}

void package_receive(uint16_t* receive_vector) {
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

void request_data(uint8_t command_code, uint16_t* color_vector){
    __delay_us(10);
    I2C_start();
    I2C_write((0x10 << 1) | 0); // slave address + write
    I2C_write(command_code);
    I2C_start();
    I2C_write((0x10 << 1) | 1); // slave address + read
    package_receive(color_vector);
    I2C_stop();
    __delay_us(10);
}

void WS2812_Send_Byte(uint8_t byte)
{
    for (int i = 7; i >= 0; i --)
    {        
        uint8_t spi_data = (byte & (1 << i)) ? 0b11110000 : 0b11000000;  
        
        SPI1_Exchange8bit(spi_data);          
    }
}

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

float calculate_distance(int r1, int g1, int b1, int r2, int g2, int b2)
{        
    return sqrtf(powf(r1 - r2, 2) + powf(g1 - g2, 2) + powf(b1 - b2, 2));    
}

char* detect_color(uint16_t* r,uint16_t* g,uint16_t* b)
{    
    /*uint16_t red_shade = red_vector;
    uint16_t green_shade = green_vector;
    uint16_t blue_shade = blue_vector;*/

    float min_distance = 1e9; 
    char* detected_color = "NN";
    float distance = 0;
    
    for (int i = 0; i < COLOR_QUANTITY; i++) {
        distance = calculate_distance(*r, *g, *b, colors[i].red_shade, colors[i].green_shade, colors[i].blue_shade);        
        
        if (distance < min_distance) {
            min_distance = distance;
            detected_color = colors[i].name;            
        }
    }   
    __delay_ms(100);
    char buffer[32];
    sprintf(buffer, "D: %.2f\r\n", min_distance);
    U1_Write_Text(buffer);
    U1_Write_Text(detected_color);
    U1_Write_Text("\n");
    return detected_color;  
}

char* read_all_shades(void)
{   
    
    
    uint16_t sum_red = 0, sum_green = 0, sum_blue = 0;
    uint16_t avg_red, avg_green, avg_blue;               
    uint16_t clear_vector;
    uint16_t ir_vector;
    uint16_t clear_vector_without_ir;
    
    for (int i = 0; i < NUM_OF_SAMPLES; i++) {                         
    
        red_vector = 0;
        green_vector = 0;
        blue_vector = 0;                        
        
        request_data(0x05,&red_vector);                
        
        request_data(0x06,&green_vector);                 
        
        request_data(0x07,&blue_vector);                 
        
        /*char buffer[16];
        sprintf(buffer, "Clear:%d\r\n", clear_vector);
        U1_Write_Text(buffer);
        
        sprintf(buffer, "IR vector:%d\r\n", ir_vector);
        U1_Write_Text(buffer);*/
        
        sum_red += red_vector;

        sum_green += green_vector;

        sum_blue += blue_vector;

        __delay_ms(200);     
    }
            
    
    avg_red = sum_red / NUM_OF_SAMPLES;
    avg_green = sum_green / NUM_OF_SAMPLES;
    avg_blue = sum_blue / NUM_OF_SAMPLES;
    
    request_data(0x04,&clear_vector);
    __delay_ms(200);
    request_data(0x08,&ir_vector);
    __delay_ms(200);
    
    clear_vector_without_ir = clear_vector-ir_vector;    
    
    uint16_t normalized_red = (avg_red/(float)clear_vector_without_ir) * 1000;
    uint16_t normalized_green = (avg_green/(float)clear_vector_without_ir) * 1000;
    uint16_t normalized_blue = (avg_blue/(float)clear_vector_without_ir) * 1000;
    
    char buffer[16];

    /*sprintf(buffer, "R:%d\r\n", avg_red);
    U1_Write_Text(buffer);
    __delay_ms(10);

    sprintf(buffer, "G:%d\r\n", avg_green);
    U1_Write_Text(buffer);
    __delay_ms(10);

    sprintf(buffer, "B:%d\r\n", avg_blue);
    U1_Write_Text(buffer);
    __delay_ms(10);*/        
    
    red_value = normalized_red;
    blue_value = normalized_blue;
    green_value = normalized_green;  
    
    return detect_color(&normalized_red,&normalized_green,&normalized_blue);    
} 


int main(void) {
    
    configure_oscillator();
    SYSTEM_Initialize();
    __builtin_enable_interrupts();
    
    TRISBbits.TRISB6 = 0;  // LED as output       
    LATBbits.LATB6 = 0;
      
    I2C_free_stuck_slave();
       
    __delay_us(100);
    
    //WS2812_Set_Color(255,255,255);
    
    
    I2C_config();
    
    set_slave();
    
    
    
    while(1){                   
        __delay_us(10); 
        WS2812_Set_Color(255,255,255);
        LATBbits.LATB6 = 1;
        char* color = read_all_shades();        
        __delay_ms(100);
        LATBbits.LATB6 = 0;
        __delay_ms(100); 
        char buffer[16];
        sprintf(buffer, "R:%d\r\n", red_value);
        U1_Write_Text(buffer);
        __delay_ms(10);

        sprintf(buffer, "G:%d\r\n", green_value);
        U1_Write_Text(buffer);
        __delay_ms(10);

        sprintf(buffer, "B:%d\r\n", blue_value);
        U1_Write_Text(buffer);
        __delay_ms(10);
        U1_Write_Text("Color:\n");
        U1_Write_Text(color);            
    }
}