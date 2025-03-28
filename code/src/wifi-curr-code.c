/*
 * @author: Dejana S.
 */

#pragma config FNOSC = FRCPLL    // FRC sa PLL omogucen
#pragma config FCKSM = CSECMD // Clock switching enabled, fail-safe clock monitor disabled
#pragma config OSCIOFNC = OFF  // OSC2/CLKO is not a clock output
#pragma config POSCMOD = NONE  // Primary oscillator disabled
#pragma config IESO = OFF      // Two-speed oscillator start-up disabled
#pragma config JTAGEN = OFF    // JTAG disabled
#pragma config FWDTEN = OFF    // Watchdog timer disabled

#include <xc.h>      // Ukljucivanje osnovne biblioteke za mikrokontroler
#include <string.h>  // Biblioteka za rad sa stringovima
#include <stdlib.h>  // Standardna biblioteka 
#define FCY 16000000UL // Definisanje frekvencije CPU-a
#include <libpic30.h> // Biblioteka za kasnjenja (__delay_ms i __delay_us)

// Definisanje brzine prenosa podataka za WiFi/UART
#define BRGH_VAL 1
#define BRG_VAL 34
#define BUF_SIZE 80  // Velicina bafera za prijem podataka

// Definisanje komandi za ESP modul
const char* CMD_AT = "AT\r\n";
const char* CMD_RST = "AT+RST\r\n";
const char* CMD_MODE = "AT+CWMODE=1\r\n";
const char* CMD_LIST_AP = "AT+CWLAP\r\n";
const char* CMD_WIFI_CONN = "AT+CWJAP=\"etfbl.net\",\"\"\r\n";
const char* CMD_CONN_TYPE = "AT+CIPMUX=0\r\n";
const char* CMD_START_TCP = "AT+CIPSTART=\"TCP\",\"10.99.131.223\",8084\r\n";
const char* CMD_SEND = "AT+CIPSEND=3\r\n";
const char* CMD_IP = "AT+CIFSR\r\n";

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
// Definisanje globalnih promjenljivih
volatile char buffer[BUF_SIZE] = {0};  // Bafer za prijem podataka
volatile unsigned head = 0, tail = 0; // Pokazivaci na pocetak i kraj bafera
volatile unsigned head_TX = 0, tail_TX = 0;
int flag = 0; // Flag za zaustavljanej programa

// Interrupt rutina za prijem podataka preko UART1
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) 
{
    IFS0bits.U1RXIF = 0;  // Resetovanje interrupt flag-a
    if (U1STAbits.URXDA)  // Provjera da li postoje podaci u registru prijema
    { 
        buffer[head] = U1RXREG; // Smijestanje primljenog podatka u bafer
        head = (head + 1) % BUF_SIZE;   // Azuriranje pokazivaca
        /*if(head == tail)
        {
            cleanBuffer(); // vidjeti koliko ce ovo usporiti interrupt
        }*/
    }  
    if(strstr(buffer, "R") != NULL)
    {
        flag = 1;
    }
    else if(strstr(buffer, "S") != NULL)
    {
        flag = 0;
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

// Interrupt rutina za upravljanje greskama UART1
void __attribute__((interrupt, no_auto_psv)) _U1ErrInterrupt(void) 
{
    IFS4bits.U1ERIF = 0; // Resetovanje error interrupt flag-a
    U1STAbits.OERR = 0;  // Resetovanje overrun error flag-a
}

/*void __attribute__ (( interrupt, no_auto_psv )) _U1TXInterrupt (void)
{
    if(head_TX == tail_TX)
    {
        head_TX = 0;
    }
    
}*/

// Funkcija za slanje AT komandi preko UART-a
void sendCommand(const char* command) 
{
    while (*command)  // Petlja se izvrsava dok ne posaljemo cijeli string
    {
        while (U1STAbits.UTXBF); // Cekanje dok se UART bafer ne isprazni
        U1TXREG = *command++;  // Slanje jednog karaktera
    }
    __delay_ms(1000);  // Kratko kasnjenje nakon slanja komande, ceka odgovor
}

// Funkcija za slanje jednog karaktera preko UART-a
void UART_Write_Char(char c) 
{
    while (U1STAbits.UTXBF);  // Cekanje dok se bafer ne isprazni
    U1TXREG = c;  // Slanje karaktera
}

// Funkcija za slanje stringa preko UART-a
void UART_Write_Text(const char* text) 
{
    while (*text)  // Petlja koja salje svaki karakter stringa
    {
        UART_Write_Char(*text++);
    }
}

void UART2_Write_Char(char c) 
{
    while (U2STAbits.UTXBF);  // Cekanje dok se bafer ne isprazni
    U2TXREG = c;  // Slanje karaktera
}

// Funkcija za slanje stringa preko UART-a
void UART2_Write_Text(const char* text) 
{
    while (*text)  // Petlja koja salje svaki karakter stringa
    {
        UART2_Write_Char(*text++);
    }
}

// Funkcija za podesavanje oscilatora
void configureOscillator()
{
    CLKDIV = 0;  // Postavljanje djelitelja frekvencije
    __builtin_write_OSCCONH(0x01);  // Odabir FRC sa PLL kao izvor oscilatora
    __builtin_write_OSCCONL(OSCCON | 0x01);  // Potvrda promjene oscilatora
    while (OSCCONbits.OSWEN);  // Cekanje dok se promjene ne primijene
}

// Funkcija za testiranje frekvencije oscilatora
void checkFrequency() 
{
    if (OSCCONbits.NOSC == 0b001) // Ako je FRC sa PLL aktivan
    {
        while(1) {
            LATBbits.LATB6 = 1;  // Ukljucivanje LED diode
            __delay_ms(1000);
            LATBbits.LATB6 = 0;  // Iskljucivanje LED diode
            __delay_ms(1000);
        }
    }
}

// Podesavanje PPS (Peripheral Pin Select) za UART
void configurePPS()
{
    __builtin_write_OSCCONL(OSCCON & ~(1 << 6));  // Otkljucavanje PPS-a

    RPINR18bits.U1RXR = 8; 
    RPOR4bits.RP9R = 3;    
    
    RPINR19bits.U2RXR = 5;
    RPOR3bits.RP7R = 5;
    
    __builtin_write_OSCCONL(OSCCON | (1 << 6));  // Zakljucavanje PPS-a
}

// Podesavanje GPIO pinova
void configureIO() 
{
    TRISB = 0; // Svi pinovi PORTB su izlazni prvobitno
    TRISBbits.TRISB6 = 0;  // RB6 kao izlaz (LED)
    TRISBbits.TRISB8 = 1; // RB8 kao ulaz (UART2 RX)
    TRISBbits.TRISB5 = 1; // RB5 kao ulaz (UART1 RX))
    LATBbits.LATB6 = 1;  // LED ukljucena
}

// Podesavanje UART1
void configureUART() 
{
    U1MODEbits.BRGH = BRGH_VAL; // Postavljanje high-speed moda
    U1BRG = BRG_VAL; // Postavljanje baud rate-a na 115200
    U1MODEbits.UARTEN = 1; // Aktivacija UART1
    U1STAbits.UTXEN = 1; // Aktivacija TX

    IPC2bits.U1RXIP = 5; // Podesavanje prioriteta prijema
    IFS0bits.U1RXIF = 0; // Resetovanje RX flag-a
    IEC0bits.U1RXIE = 1; // Aktivacija RX interrupt-a
}
void configureUART2()
{
    U2MODEbits.BRGH = BRGH_VAL;
    U2BRG = BRG_VAL;  
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
    
    IPC7bits.U2RXIP = 5;
    IFS1bits.U2RXIF = 0;
    IEC1bits.U2RXIE = 1;
}

int checkStopFlag()
{
    if(strstr(buffer, "R"))
            {   
                flag = 1;
                UART2_Write_Text("STOPIRAN!!");
                return 1;
                
            }
    return 0;
}

int checkStartFlag()
{
    if(strstr(buffer, "S"))
            {   
                flag = 0;
                UART2_Write_Text("STARTOVAN!!");
                return 1;
                
            }
    return 0;
}

int main() 
{
    configureOscillator();
    configureIO();
    configurePPS();
    configureUART();
    configureUART2();
    
    // Uspostavljanje konekcije sa WiFi modulom
    cleanBuffer();
    sendCommand(CMD_AT); 
    UART2_Write_Text(buffer);
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_RST);
    UART2_Write_Text(buffer);
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_MODE);
     UART2_Write_Text(buffer);
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_WIFI_CONN);
     UART2_Write_Text(buffer);
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_CONN_TYPE);
     UART2_Write_Text(buffer);
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_IP);
     UART2_Write_Text(buffer);
    __delay_ms(100);
    
    cleanBuffer();
    sendCommand(CMD_START_TCP);
    __delay_ms(1000);
    
    LATBbits.LATB6 = 0;
    // sendCommand("ATE0\r\n");
    __delay_ms(100);

    while (1) 
    {
        /*char msg[BUF_SIZE] = {0};
        unsigned counter = 0;
        while (tail != head && counter < BUF_SIZE - 1) 
        {
            msg[counter++] = buffer[tail];
            tail = (tail + 1) % BUF_SIZE;
        }
        msg[counter] = '\0';*/
        if(flag == 0)
        {
         //   checkStopFlag();
        sendCommand(CMD_START_TCP);
        __delay_ms(1000);
        cleanBuffer();
        //    checkStopFlag();

        sendCommand(CMD_SEND);
        UART2_Write_Text(buffer);
        __delay_ms(100); 
        //    checkStopFlag();
        
        sendCommand(PURPLE_L);
        UART2_Write_Text(buffer);
        __delay_ms(300);
        
         //   checkStopFlag();
        
        
        cleanBuffer();
        __delay_ms(100);
        sendCommand(CMD_SEND);
        UART2_Write_Text(buffer);
        __delay_ms(100); 
        //    checkStopFlag();
       
        sendCommand(ORANGE_R);
        UART2_Write_Text(buffer);
        __delay_ms(300);
        //    checkStopFlag();
        
        
        
        cleanBuffer();
        __delay_ms(100);
        sendCommand(CMD_SEND);
        UART2_Write_Text(buffer);
        __delay_ms(100); 
          //  checkStopFlag();
       
        sendCommand(BLACK_R);
        UART2_Write_Text(buffer);
        __delay_ms(300);
        
         //   checkStopFlag();
        
        
        cleanBuffer();
        __delay_ms(100);
        sendCommand(CMD_SEND);
        UART2_Write_Text(buffer);
        __delay_ms(100); 
            checkStopFlag();
       
        sendCommand(GREEN_L);
        UART2_Write_Text(buffer);
        __delay_ms(300);
         //   checkStopFlag();
        
        
        cleanBuffer();
        __delay_ms(100);
        sendCommand(CMD_SEND);
        UART2_Write_Text(buffer);
        __delay_ms(100); 
          //  checkStopFlag();
       
        sendCommand(PURPLE_R);
        UART2_Write_Text(buffer);
        __delay_ms(300);
          //  checkStopFlag();
        
        cleanBuffer();
        __delay_ms(100);
        }

        //checkStartFlag();   
          
    }
    return 0;
}