/*
 * File:   main.c
 * Author: Korisnik
 *
 * Created on 14. april 2025., 10.49
 */


#include "xc.h"
#include "CONFIG.h"


void processDataFromColorSensor(const char* data) 
{
    if (flag == 1) return; 
    
    // Check for each color and corresponding movement (direction)
       
    if(strcmp(data, "RED") == 0) 
    {
        if(colorBuffer[0] == 'A') {
            moveRight();
            sendData(RED_R); 
        } else if(colorBuffer[0] == 'a') {
            moveLeft();
            sendData(RED_L);
        }
    }
    else if(strcmp(data, "GREEN") == 0) 
    {
        if(colorBuffer[1] == 'C') {
            moveRight();
            sendData(GREEN_R);
        } else if(colorBuffer[1] == 'c') {
            moveLeft();
            sendData(GREEN_L);
        }
    }
    else if(strcmp(data, "BLUE") == 0) 
    {
        if(colorBuffer[2] == 'B') {
            moveRight();
            sendData(BLUE_R);
        } else if(colorBuffer[2] == 'b') {
            moveLeft();
            sendData(BLUE_L);
        }
    }
    else if(strcmp(data, "YELLOW") == 0) 
    {
        if(colorBuffer[3] == 'D') {
            moveRight();
            sendData(YELLOW_R);
        } else if(colorBuffer[3] == 'd') {
            moveLeft();
            sendData(YELLOW_L);
        }
    }
    else if(strcmp(data, "PINK") == 0) 
    {
        if(colorBuffer[4] == 'E') {
            moveRight();
            sendData(PURPLE_R);
        } else if(colorBuffer[4] == 'e') {
            moveLeft();
            sendData(PURPLE_L);
        }
    }
    else if(strcmp(data, "ORANGE") == 0) 
    {
        if(colorBuffer[5] == 'F') {
            moveRight();
            sendData(ORANGE_R);
        } else if(colorBuffer[5] == 'f') {
            moveLeft();
            sendData(ORANGE_L);
        }
    }
    else if(strcmp(data, "BLACK") == 0) 
    {
        if(colorBuffer[6] == 'G') {
            moveRight();
            sendData(BLACK_R);
        } else if(colorBuffer[6] == 'g') {
            moveLeft();
            sendData(BLACK_L);
        }
    }
    else if(strcmp(data, "WHITE") == 0) 
    {
        if(colorBuffer[7] == 'H') {
            moveRight();
            sendData(WHITE_R);
        } else if(colorBuffer[7] == 'h') {
            moveLeft();
            sendData(WHITE_L);
        }
    }
    else {
        moveMiddle();
    }
}

int main(void)
{
    configureAllComponents();
    
    while(1)
    {
        
        char* color = readColor();
        if(strcmp(color,"NO")!=0)
        {
            processDataFromColorSensor(color);
        }
    }
    return 0;
}
