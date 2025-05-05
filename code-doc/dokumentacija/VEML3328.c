 /******************************************************************************
 * Faculty of Electrical Engineering
 * MKS 2025
 * https://github.com/lukavidic/ColorSorter_C.git
 ******************************************************************************
 *
 * @file    VEML3328.c
 * @brief   Driver and utility functions for VEML3328 color sensor and WS2812 
 * LED control.
 *
 * @details
 *   Provides routines to configure the VEML3328 sensor over I2C,
 *   read and normalize RGB values, detect colors against a calibrated
 *   color table, and send data to WS2812 LEDs. Also includes I2C bus
 *   error recovery and interrupt handling.
 *
 * @author  Korisnik
 * @date    10 April 2025
 *******************************************************************************/

#include "xc.h"
#include "VEML3328.h"
#include "system.h"

/**
 * @brief   Latest raw color vector from sensor for each channel.
 */
volatile uint16_t red_vector  = 0;
volatile uint16_t green_vector  = 0;
volatile uint16_t blue_vector  = 0;
volatile uint8_t interrupt_detected = 0;

/**
 * @brief   Stored normalized color values.
 */
uint16_t red_value = 0;
uint16_t green_value = 0;
uint16_t blue_value = 0;

/**
 * @struct  Color
 * @brief   Calibration entry for known colors.
 *
 * @details
 *   Holds the human-readable name and calibrated RGB shade values
 *   for color detection.
 */
typedef struct {
    const char* name;        /**< Color name string */
    uint16_t red_shade;      /**< Calibrated red component */
    uint16_t green_shade;    /**< Calibrated green component */
    uint16_t blue_shade;     /**< Calibrated blue component */
} Color;

/**
 * @brief   List of calibrated colors for detection algorithm.
 */
const Color colors[] = {
    {"GREEN", 103, 100,  80},
    {"BLUE",   7,  70, 100},
    {"RED",   90,  60, 100},
    {"YELLOW", 20,  50,  30},
    {"ORANGE",105,  52,  50},
    {"PINK",   20,  50,  70},
    {"BLACK", 143, 170,  80},
    {"WHITE",  30,  50, 100},    
    {"NO",    192, 120, 170}
};

/**
 * @brief   Configure sensor registers for slave mode.
 *
 * @details
 *   Sends I2C commands to set up the VEML3328 for measurements.
 */
void set_slave() {
    I2C_start();    
    I2C_write((0x10 << 1) | 0);
    I2C_write(0x00);
    I2C_write(0x08);
    I2C_stop();
    __delay_us(10);
}

/**
 * @brief   Compute Euclidean distance between two RGB vectors.
 */
float calculate_distance(int r1, int g1, int b1, int r2, int g2, int b2) {
    return sqrtf(powf(r1 - r2, 2) + powf(g1 - g2, 2) + powf(b1 - b2, 2));
}

/**
 * @brief   Determine closest matching color name.
 */
char* detect_color(uint16_t* r, uint16_t* g, uint16_t* b) {
    float min_distance = 1e9;
    char* detected_color = "NN";
    float distance;
    for (int i = 0; i < COLOR_QUANTITY; i++) {
        distance = calculate_distance(*r, *g, *b,
            colors[i].red_shade,
            colors[i].green_shade,
            colors[i].blue_shade);
        if (distance < min_distance) {
            min_distance = distance;
            detected_color = colors[i].name;
        }
    }
    __delay_ms(100);
    return detected_color;
}

/**
 * @brief   Send one byte to WS2812 LED via SPI pattern.
 */
void WS2812_Send_Byte(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        uint8_t spi_data = (byte & (1 << i)) ? 0b11110000 : 0b11000000;
        SPI1_Exchange8bit(spi_data);
    }
}

/**
 * @brief   Recover I2C bus if a slave is stuck driving SDA.
 *
 * @details
 *   Toggles SCL manually to clock out stuck data bits and
 *   generates a STOP condition then re-enables I2C.
 */
void I2C_free_stuck_slave()
{
    I2C1CONbits.I2CEN = 0;
    __delay_us(10);
    TRISBbits.TRISB9 = 0;
    TRISBbits.TRISB8 = 0;
    LATBbits.LATB9 = 1;
    for (int i = 0; i < 9; i++) {
        LATBbits.LATB8 = 1; __delay_us(5);
        LATBbits.LATB8 = 0; __delay_us(5);
    }
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

/**
 * @brief   I2C master interrupt service routine.
 * @note    Clears flag and signals transaction complete.
 */
void __attribute__((interrupt, no_auto_psv)) _MI2C1Interrupt(void)
{
    interrupt_detected = 1;
    IFS1bits.MI2C1IF = 0;
}

/**
 * @brief   Initiate I2C STOP condition and wait for completion.
 */
void I2C_stop()
{
    interrupt_detected = 0;
    I2C1CONbits.PEN = 1;
    int count = 10000;
    while (!interrupt_detected && --count);
    if (count == 0) {
        while (1) 
	{
            LATBbits.LATB6 = 1;
            __delay_ms(100);
            LATBbits.LATB6 = 0;
            __delay_ms(100);
        }
    }
}

/**
 * @brief   Initiate I2C START condition and wait for completion.
 */
void I2C_start()
{
    interrupt_detected = 0;
    I2C1CONbits.SEN = 1;
    int count = 10000;
    while (!interrupt_detected && --count > 0);
    if (count == 0) {
        while (1) {
            LATBbits.LATB6 = 1;
            __delay_ms(100);
            LATBbits.LATB6 = 0;
            __delay_ms(100);
        }
    }
}

/**
 * @brief   Enable I2C receive mode and wait for data.
 */
void I2C_receive_enable()
{
    I2C1CON = I2C1CON & 0xFFE0;
    interrupt_detected = 0;
    I2C1CONbits.RCEN = 1;
    int count = 10000;
    while (!interrupt_detected && --count > 0);
    if (count == 0) {
        while (1) {
            LATBbits.LATB6 = 1;
            __delay_ms(100);
            LATBbits.LATB6 = 0;
            __delay_ms(100);
        }
    }
}

/**
 * @brief   Write one byte over I2C and wait for ACK.
 * @param   data Byte to transmit.
 */
void I2C_write(uint8_t data)
{
    interrupt_detected = 0;
    I2C1TRN = data;
    int count = 10000;
    while (!interrupt_detected && --count > 0);
    if (count == 0) {
        while (1) {
           LATBbits.LATB6 = 1;
            __delay_ms(100);
            LATBbits.LATB6 = 0;
            __delay_ms(100);        }
    }
}

/**
 * @brief   Receive a two-byte data package over I2C.
 * @param   receive_vector Pointer to store combined MSB/LSB.
 */
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

/**
 * @brief   Send a command and read one 16-bit register.
 * @param   command_code Register address.
 * @param   color_vector Pointer to store returned value.
 */
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

/**
 * @brief   Set WS2812 LED color (GRB order).
 */
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

/**
 * @brief   Configure I2C module and prepare sensor.
 */
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

/**
 * @brief   Read averaged normalized color and detect name.
 * @return  Detected color string.
 */
char* readColor(void)
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
                
        sum_red += red_vector;

        sum_green += green_vector;

        sum_blue += blue_vector;

        __delay_ms(100);     
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
        
    red_value = normalized_red;
    blue_value = normalized_blue;
    green_value = normalized_green;  
    
    return detect_color(&normalized_red,&normalized_green,&normalized_blue);    

}

/**
 * @brief   Perform initial system and sensor setup, then indicate readiness.
 */
void colorClickConfiguration()
{
    SYSTEM_Initialize();
    I2C_config();
    __delay_us(500);
    WS2812_Set_Color(255,255,255);
}

/** End of File */