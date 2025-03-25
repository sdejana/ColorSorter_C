/*
 * File:   main.c
 * Author: Korisnik
 *
 * Created on 13. mart 2025., 12.00
 */


#include "xc.h"
#include <libpic30.h>
#include <stdlib.h>

uint32_t button_flag = 1;

void __attribute__((interrupt, auto_psv)) _CNInterrupt(void){
    LATBbits.LATB6 = ~LATBbits.LATB6;
    button_flag = !button_flag;
    IFS1bits.CNIF = 0;
}



int main(void) {
    
        
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 1;
    LATBbits.LATB6 = 1;
    CNPU2bits.CN23PUE = 1;
    CNEN2bits.CN23IE = 1;
    IEC1bits.CNIE = 1;
    IPC4bits.CNIP = 0x7;
    while(1);
    return 0;
}
