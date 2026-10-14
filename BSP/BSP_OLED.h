#ifndef __BSP_OLED_H
#define __BSP_OLED_H


#include "main.h"


#define OLED_SCK_GPIO_Port   GPIOB
#define OLED_SDA_GPIO_Port   GPIOB

#define OLED_SCK_Pin         GPIO_PIN_0
#define OLED_SDA_Pin         GPIO_PIN_1

void OLED_W_SCL(uint8_t BitValue);
void OLED_W_SDA(uint8_t BitValue);
void OLED_GPIO_Init(void);
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
void OLED_I2C_SendByte(uint8_t Byte);



#endif


