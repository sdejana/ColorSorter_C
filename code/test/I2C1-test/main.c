/*
 * File:   main_2.c
 * Author: Korisnik
 *
 * Created on 18. mart 2025., 13.28
 */

#include "xc.h"
#define FCY 16000000UL
#include <p24FJ64GA002.h>
#include <libpic30.h>
#include <stdio.h>
#include <stdlib.h>

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
void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0; // Clear TX Interrupt flag
    U1TXREG = color_to_display; // Transmit one character
}

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

void uart_config(){
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB11 = 1;        
    // DO is RP12 = TX, RST is RP11 = RX
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    // Assign U1RX To Pin RP11
    //RPINR18bits.U1RXR = 11;
    // Assign U1TX To Pin RP12
    RPOR6bits.RP12R = 3;
    __builtin_write_OSCCONL(OSCCON | 0x40);
    
    U1MODEbits.STSEL = 0; // 1-Stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-Data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud disabled
    U1MODEbits.BRGH = 0; // Standard-Speed mode
    //U1BRG = 50; // Baud Rate setting for 4800
    U1BRG = 207; // baud rate for FCY = 16 MHz
    U1STAbits.UTXISEL0 = 1; // Interrupt after the last transmission
    U1STAbits.UTXISEL1 = 0;
    IEC0bits.U1TXIE = 1; // Enable UART TX interrupt
    IPC3bits.U1TXIP = 4;
    U1MODEbits.UARTEN = 1; // Enable UART
    U1STAbits.UTXEN = 1; // Enable UART TX
   
    /* Wait at least 210 microseconds (1/4800) before sending first char */
    __delay_us(210);        
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
    I2C_start();
    I2C_write((0x10 << 1) | 0); // slave address + write
    I2C_write(command_code);
    I2C_start();
    I2C_write((0x10 << 1) | 1); // slave address + read
    package_receive(color_vector);
    I2C_stop();
    __delay_us(10);
}

int main(void) {
    
    configure_oscillator();
    
    __builtin_enable_interrupts();
    
    TRISBbits.TRISB6 = 0;  // LED as output       
    LATBbits.LATB6 = 0;
    
    uart_config();
    
    I2C_free_stuck_slave();
       
    __delay_us(10);
    
    I2C_config();    
    
    set_slave();        
    
    while(1){   
        I2C_free_stuck_slave();
        __delay_us(10);
        request_data(0x05,&red_vector);
        request_data(0x06,&green_vector);
        request_data(0x07,&blue_vector);                      
        
        LATBbits.LATB6 = 1;
        __delay_ms(100);
        LATBbits.LATB6 = 0;
        
        if(red_vector>green_vector && red_vector>blue_vector){
            while(U1STAbits.UTXBF);
            color_to_display = 'R'; 
        }
        else if(red_vector<green_vector && green_vector>blue_vector){
            while (U1STAbits.UTXBF);
            color_to_display = 'G';
        }
        else{
            while(U1STAbits.UTXBF);
            color_to_display = 'B';
        }
        
        __delay_ms(100);
        /*while (U1STAbits.UTXBF);
        
        uint8_t lsb = red_vector & 0x00FF;  // Keep LSB as is
        uint8_t msb = (red_vector >> 8) & 0x00FF;  // Shift MSB correctly

        U1TXREG = lsb; // Send LSB over UART

        __delay_ms(1000);
        while (U1STAbits.UTXBF);
        U1TXREG = msb; // Send MSB over UART
        
        
        __delay_ms(1000);
        while (U1STAbits.UTXBF);
        lsb = green_vector & 0x00FF;  
        msb = (green_vector >> 8) & 0x00FF;  
        U1TXREG = lsb; // send LSB over UART
        __delay_ms(1000);
        
        while (U1STAbits.UTXBF);
        U1TXREG = msb; // Send MSB over UART
        
        __delay_ms(1000);
        while (U1STAbits.UTXBF);
        lsb = blue_vector & 0x00FF;  
        msb = (blue_vector >> 8) & 0x00FF; 
        U1TXREG = lsb; // send LSB over UART

        __delay_ms(1000);
        while (U1STAbits.UTXBF);
        U1TXREG = msb; // Send MSB over UART
        LATBbits.LATB6 = 1;        
        __delay_ms(1000); */       
    }
    
    /*while (1) {        
        package_transmitting(0x05);  // Read RED register
        package_receiving(red_vector);

        while (U1STAbits.UTXBF);
        U1TXREG = red_vector[0]; // Send LSB over UART

        while (U1STAbits.UTXBF);
        U1TXREG = red_vector[1]; // Send MSB over UART

        __delay_ms(1000);
    }*/
}