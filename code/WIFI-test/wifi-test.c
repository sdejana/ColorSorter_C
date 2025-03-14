/*
* @author: Dejana S.
*/
/*#define WIFI_RX_PIN F13
#define WIFI_TX_PIN F14
#define UART_RX_PIN F10
#define UART_TX_PIN F11    */
#define BAUD_RATE_WIFI 19200
#define BAUD_RATE 19200
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

/**
 * @brief Funkcija rukovaoca prekida za prijem UART podataka.
 * Cita podatak dobijen od strane ESP8266-01.
 */
void UART1_Receiver_Interrupt() iv 0x00002A
{
  IEC0.U1RXIE = 0;
  
  if(UART1_data_ready())
  {
     buffer[head] = UART1_Read();
  }
  head = (head + 1) % BUF_SIZE;
  
  IEC0.U1RXIE = 1;
  IFS0.U1RXIF = 0;
}

/**
 * @brief Funkcija koja salje zadanu komandu preko UART-a 
 * i ostavlja 5 sekundi za odgovor.
 */
void sendCommand(const char* command)
{
    UART1_Write_Text(command);
    Delay_ms(5000);
}

/**
 * @brief Funkcija rukovaoca prekida koja se aktivira kada se
 * desi prekoracenje UART bafera za prijem podataka. Rukovaoc
 * cisti bit greske (OERR) cime omogucava nastavak UART prenosa.
 */
void UART1_Error_Interrupt() iv 0x000096
{
  IEC4.U1ERIE = 0;
  U1STA.OERR  = 0;
  IEC4.U1ERIE = 1;
  IFS4.U1ERIF = 0;
}

void main()
{
  char msg[BUF_SIZE];
  unsigned counter = 0;

  CLKDIV = 0;

  // Mijenjamo frekvenciju, tj. oscilator Fast RC sa PLL modulom
  OSCCONH = 0x78;
  OSCCONH = 0x9A;
  OSCCONbits.NOSC = 0x001;
  OSCCONL = 0x46;
  OSCCONL = 0x57;
  OSCCONbits.OSWEN = 1;
  Delay_ms(200);

  AD1PCFG = 0xFFFF;
  TRISB = 0;
  // TRISB.F3 = 1; // RX je input pin
  TRISB.F8 = 1;
  LATB = 0;
  TRISA = 0;
  LATA.F4 = 1;


  IPC2bits.U1RXIP = 5;
  IFS0.U1RXIF = 0;
  IEC0.U1RXIE = 1;

  IPC16bits.U1ERIP = 5;
  IFS4.U1ERIF = 0;
  IEC4.U1ERIE = 1;

  Unlock_IOLOCK();
  // PPS_Mapping_NoLock(2, _INPUT, _U2RX);
  // PPS_Mapping_NoLock(7, _OUTPUT, _U2TX);
  PPS_Mapping_NoLock(8, _INPUT, _U1RX);
  PPS_Mapping_NoLock(9, _OUTPUT, _U1TX);
  Lock_IOLOCK();

  UART1_Init(19200);
  Delay_ms(200);
  UART2_Init(19200);

  Delay_ms(200);
  /*
  if(OSCCONbits.OSWEN == 0)
  {
      LATB.F6 = 1;
      Delay_ms(1000);
      LATB.F6 = 0; // blinka, znaci jeste ukljucen Fast RC sa PLL- om 
  }    */

  while(1)
  {
        memset(msg, 0, BUF_SIZE);
        counter = 0;
        while(tail != head && counter < BUF_SIZE - 1)
        {
            msg[counter] = buffer[tail];
            counter++;
            tail = (tail + 1) % BUF_SIZE;
        }
        msg[counter] = '\0';

        /* 01001111 01001011 00001101 00001010
           ----O--- ----K--- ----\r-- ----\n-- */
           
        sendCommand(CMD_AT);
        
        if(strstr(msg, "OK") != 0)
        {
           // ovo radi ok HVALA BOGU RADI SUPER
           LATB.F6 = 1;
           Delay_ms(500);
           LATB.F6 = 0;
        }
        Delay_ms(1000);
        
       //  sendCommand(CMD_UART_MOD);

        // 20 sekundi otprilike - 5 sekundi delay-a, 2 sekunde delay-a
        // diodu. jeeej
        /* if(strstr(msg, "UART") != 0)  // radi i ovo
        {
           // ovo radi ok
           LATB.F6 = 1;
           Delay_ms(1000);
           LATB.F6 = 0;
        }
        Delay_ms(1000);  */
  }
}