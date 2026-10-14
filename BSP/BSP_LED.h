#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "main.h"

typedef enum
{
    LED_ON  = 1,
    LED_OFF = 2
}LED_State;



typedef struct 
{
    GPIO_TypeDef* GPIO_Port;
    uint16_t      GPIO_Pin;
}LED_GPIO;

typedef struct 
{
    uint8_t   Instance;
    LED_GPIO  GPIO;
    LED_State State;
}LED_Device;

typedef struct 
{ 
    uint8_t       LED_Name;
    GPIO_TypeDef* GPIO_Port;
    uint16_t      GPIO_Pin;
    LED_State     LED_State;
}LED_InitConfig;





#define LED_Green_Pin             GPIO_PIN_0
#define LED_Red_Pin               GPIO_PIN_1
#define LED_Blue_Pin              GPIO_PIN_3
#define StepMotorArm_RedLED_Pin  GPIO_PIN_11


#define LED_Blue_GPIO_Port        GPIOD
#define LED_Red_GPIO_Port         GPIOD
#define LED_Green_GPIO_Port       GPIOD
#define StepMotorArm_RedLED_GPIO_Port  GPIOE

LED_Device* BSP_LED_Init(LED_InitConfig* LED);
void BSP_LED_Toggle(LED_Device* led);
void LED_SetState(LED_Device* led, LED_State state);

#endif
