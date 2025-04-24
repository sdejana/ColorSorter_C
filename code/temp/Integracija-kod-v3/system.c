/*
 * File:   system.c
 * Author: Korisnik
 *
 * Created on 10. april 2025., 13.07
 */


#include "pin_manager.h"
#include "system.h"
#include "spi1.h"
#include "interrupt_manager.h"

void SYSTEM_Initialize(void)
{
    //PIN_MANAGER_Initialize();    
    INTERRUPT_Initialize();
    SPI1_Initialize();    
}

/**
 End of File
*/