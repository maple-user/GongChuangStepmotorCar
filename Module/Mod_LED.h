#ifndef __MOD_LED_H
#define __MOD_LED_H

#include "BSP_LED.h"

#define LED_Red       0
#define LED_Green     1
#define LED_Blue      2
#define LED_StepMotorArm 3

void LED_Init(void);
void LED_Blink(LED_Device* LED,float ms);
LED_Device* LED_GetDevice(uint8_t LED_Name);


#endif
