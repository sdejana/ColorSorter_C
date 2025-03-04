#include <xc.h>

/*
 * @author Dejana
 */
/*
 * PIN 15 mikrokontrolera se koristi kao signalni.
 * PIN 15 = RB7.
 * Funkcija: treba da signalizuje pocetak rada sklopa.
 * Period: T = 250ms zasad.
 * Pin na plocici: desni header 6, odozgo.
 */

// Configuration Bits
#pragma config FNOSC = FRC   // 8 MHz Fosc
// #pragma config POSCMD = NONE    // Primary Oscillator disabled
// #pragma config FWDTEN = OFF     // Watchdog Timer disabled
// #pragma config JTAGEN = OFF     // JTAG disabled

#define LED_PIN LATBbits.LATB7


void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void) 
{
    LED_PIN ^= 1;  
    IFS0bits.T1IF = 0; /* timer interrupt flag clear*/
}

void Timer1_Init(void) 
{
    T1CON = 0;            /* Reset Timer1 settings */
    T1CONbits.TCKPS = 3;  /* 1:256 prescaler */
    PR1 = 3906;  /* Timer period od ~250ms sa Fosc/2, 4MHz */
    
    /*
     * PR1 = (T*Fcy)/prescaler
     */
    
    TMR1 = 0;             /* Reset timer count */
    IEC0bits.T1IE = 1;    /* Enable Timer1 interrupt */
    IFS0bits.T1IF = 0;    /* Clear interrupt flag */
    T1CONbits.TON = 1;    /* Start Timer1 */
}

void GPIO_Init(void) 
{
    AD1PCFGbits.PCFG15 = 1; /* podesavam samo pin koji koristim na D*/
    TRISBbits.TRISB7 = 0; 
    LED_PIN = 0;          
}

int main(void) 
{
    GPIO_Init();  
    Timer1_Init(); 

    while(1) 
    {
    }
    
    return 0;
}
