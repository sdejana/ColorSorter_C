#ifndef _PIN_MANAGER_H
#define _PIN_MANAGER_H
/**
    Section: Includes
*/
#include <xc.h>

#define SDO1_SetHigh()          (_LATB11 = 1)

#define SDO1_SetLow()           (_LATB11 = 0)

#define SDO1_Toggle()           (_LATB11 ^= 1)

#define SDO1_GetValue()         _RB11

#define SDO1_SetDigitalInput()  (_TRISB11 = 1)

#define SDO1_SetDigitalOutput() (_TRISB11 = 0)


void PIN_MANAGER_Initialize (void);

#endif
