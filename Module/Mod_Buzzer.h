#ifndef __MOD_BUZZER_H
#define __MOD_BUZZER_H


#include "BSP_Buzzer.h"

#define Buzzer_1         0U



void Buzzer_Init(void);
Buzzer_Device* Buzzer_GetDevice(uint8_t Buzzer_Name);
 void Buzzer_On(Buzzer_Device* Buzzer,float ms);
 


#endif


