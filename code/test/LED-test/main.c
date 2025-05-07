void main() {

    AD1PCFG = 0XFFFF;

    // konfiguracija I/O portova
   // TRISA.F0 = 1;                 // RA0 input pin
    TRISB = 0;                    // RB output pins

    LATB = 0;                     // All RBn pins are set to zero

    while(1){
        LATB = ~LATB;
        Delay_ms(1000);
    }
}