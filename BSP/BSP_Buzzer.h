#ifndef __BSP_BUZZER_H
#define __BSP_BUZZER_H

#include "main.h"

typedef enum
{
    BUZZER_ON  = 1,
    BUZZER_OFF = 2
}Buzzer_State;



typedef struct 
{
    GPIO_TypeDef* GPIO_Port;
    uint16_t      GPIO_Pin;
}Buzzer_GPIO;

typedef struct 
{
    uint8_t   Instance;
    Buzzer_GPIO  GPIO;
    Buzzer_State State;
}Buzzer_Device;

typedef struct 
{ 
    uint8_t       Buzzer_Name;
    GPIO_TypeDef* GPIO_Port;
    uint16_t      GPIO_Pin;
    Buzzer_State     Buzzer_State;
}Buzzer_InitConfig;

//#define Buzzer_Name              0U

#define Buzzer_Pin               GPIO_PIN_4
#define Buzzer_GPIO_Port         GPIOD

Buzzer_Device* BSP_Buzzer_Init(Buzzer_InitConfig* Buzzer);
void BSP_Buzzer_SetState(Buzzer_Device* buzzer, Buzzer_State state);

#endif

