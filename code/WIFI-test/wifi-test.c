/*
 * @author: Dejana S.
 */
 #pragma config FNOSC = FRCPLL   // Oscillator Source Selection (FRC with PLL module)

 #include <xc.h>
 #include <string.h>
 #include <stdlib.h>
 #define FCY 16000000UL
 #include <libpic30.h>
 
 #define BAUD_RATE_WIFI 115200
 #define BAUD_RATE 115200
 #define BUF_SIZE 32
 
 const char* CMD_AT = "AT\r\n";
 const char* CMD_RST = "AT+RST\r\n";
 const char* CMD_MODE = "AT+CWMODE=1\r\n";
 const char* CMD_LIST_AP = "AT+CWLAP\r\n";
 const char* CMD_WIFI_CONN = "AT+CWJAP=\"etfbl.net\",\"\"\r\n";
 const char* CMD_CONN_TYPE = "AT+CIPMUX=0\r\n";
 const char* CMD_START_TCP = "AT+CIPSTART=\"TCP\",\"10.99.130.47\",8084\r\n";
 const char* CMD_SEND = "AT+CIPSEND=7\r\n";
 const char* CMD_UART_MOD = "AT+UART_DEF\r\n";
 
 volatile char buffer[BUF_SIZE];
 volatile unsigned head = 0, tail = 0;
 int flag = 0;
 
 void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) 
 {
     IFS0bits.U1RXIF = 0;
     if (U1STAbits.URXDA) 
     {
         buffer[head] = U1RXREG;
         head = (head + 1) % BUF_SIZE;
     }
 }
 
 void __attribute__((interrupt, no_auto_psv)) _U1ErrInterrupt(void) 
 {
     IFS4bits.U1ERIF = 0;
     U1STAbits.OERR = 0;
 }
 
 void sendCommand(const char* command) 
 {
     while (*command) 
     {
         while (U1STAbits.UTXBF);
         U1TXREG = *command++;
     }
     __delay_ms(5000);
 }
 
 void UART_Write_Char(char c) 
 {
     while (U1STAbits.UTXBF);  
     U1TXREG = c;
 }
 
 void UART_Write_Text(const char* text) 
 {
     while (*text) 
     {
         UART_Write_Char(*text++);
     }
 }
 
 void configureOscillator()
 {
     CLKDIV = 2; 
     __builtin_write_OSCCONH(0x01);  
     __builtin_write_OSCCONL(OSCCON | 0x01);  
     while (OSCCONbits.OSWEN);  
 }
 
 
 void checkFrequency() 
 {
     if (OSCCONbits.NOSC == 0b001) 
     {  // Fast RC sa PLL
         while(1) {
             LATBbits.LATB6 = 1;
             __delay_ms(1000);
             LATBbits.LATB6 = 0;
             __delay_ms(1000);
         }
     }
 }
 
 void configurePPS()
 {
     __builtin_write_OSCCONL(OSCCON & ~(1 << 6)); 
 
     RPINR18bits.U1RXR = 11; // UART1 RX na RP11 (RB11)
     RPOR6bits.RP12R = 3;    // UART1 TX na RP12 (RB12)
 
     __builtin_write_OSCCONL(OSCCON | (1 << 6));  
 }
 void configureUART() 
 {
     U1MODEbits.BRGH = 1;
     U1BRG = 34;
     // u1brg = fcy/[(16*baud rate) - 1]
     U1MODEbits.UARTEN = 1;
     U1STAbits.UTXEN = 1;
 
     IPC2bits.U1RXIP = 5;
     IFS0bits.U1RXIF = 0;
     IEC0bits.U1RXIE = 1;
     
     IPC16bits.U1ERIP = 5;
     IFS4bits.U1ERIF = 0;
     IEC4bits.U1ERIE = 1;
 }
 
 void configureIO() 
 {
     TRISB = 0;
     TRISBbits.TRISB6 = 0;
     TRISBbits.TRISB11 = 1;
     LATBbits.LATB6 = 0;
 }
 
 int main() 
 {
     configureOscillator();
     configureIO();
     configurePPS();
     configureUART();
     // checkFrequency();
     while (1) 
     {
         char msg[BUF_SIZE] = {0};
         unsigned counter = 0;
         while (tail != head && counter < BUF_SIZE - 1) 
         {
             msg[counter++] = buffer[tail];
             tail = (tail + 1) % BUF_SIZE;
         }
         msg[counter] = '\0';
         
         // sendCommand(CMD_AT);
         /* if(strstr(msg, "OK") != NULL) 
         {
             LATBbits.LATB6 = 1;
             __delay_ms(500);
             LATBbits.LATB6 = 0; 
         } */
         // __delay_ms(1000);
         //sendCommand(CMD_MODE);// radi
         // sendCommand(CMD_WIFI_CONN); radi i connected je
          if(strstr(msg, "") != NULL) 
         {
             LATBbits.LATB6 = 1;
             __delay_ms(500);
             LATBbits.LATB6 = 0; 
         }
         __delay_ms(1000);
         
     }
     return 0;
 }