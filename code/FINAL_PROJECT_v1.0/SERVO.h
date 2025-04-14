#ifndef _SERVO_H
#define	_SERVO_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations
// Servo position definitions (in timer ticks)
#define SERVO_LEFT 4600      // 1ms pulse (0°)
#define SERVO_MIDDLE 3150   // 1.5ms pulse (90°) 3150
#define SERVO_RIGHT 1400      // 2ms pulse (180°)


extern uint8_t position;

void outputCompareConfig(void);
void moveLeft(void);
void moveRight(void);
void moveMiddle(void);




#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

