#ifndef __BSP_KEY_H
#define __BSP_KEY_H


#include "main.h"

typedef enum
{
    KEY_NONE = 0,
    KEY_START,
    KEY_CONTROL,
    KEY_MODE,
    KEY_ARM
} KeyState;


#define Start_Key_Pin                GPIO_PIN_7
#define Start_Key_GPIO_Port          GPIOE
#define Mode_Key_Pin                 GPIO_PIN_10
#define Mode_Key_GPIO_Port           GPIOE
#define Control_Key_Pin              GPIO_PIN_8
#define Control_Key_GPIO_Port        GPIOE
#define Key_Arm_Pin                  GPIO_PIN_12
#define Key_Arm_GPIO_Port            GPIOE

KeyState Key_GetState(void);


#endif


