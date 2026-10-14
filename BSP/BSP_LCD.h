#ifndef __BSP_LCD_H
#define __BSP_LCD_H


#include "main.h"


#include "BSP_DWT.h"

//#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;

#define LCD_SPI_HANDLE     &hspi1

//#define LCD_DC_Pin         GPIO_PIN_4
//#define LCD_DC_GPIO_Port   GPIOA
//#define LCD_SCL_Pin        GPIO_PIN_5
//#define LCD_SCL_GPIO_Port  GPIOA
//#define LCD_RES_Pin        GPIO_PIN_6
//#define LCD_RES_GPIO_Port  GPIOA
//#define LCD_SDA_Pin        GPIO_PIN_7
//#define LCD_SDA_GPIO_Port  GPIOA
//#define LCD_BLK_Pin        GPIO_PIN_4
//#define LCD_BLK_GPIO_Port  GPIOC
//#define LCD_CS_Pin         GPIO_PIN_5
//#define LCD_CS_GPIO_Port   GPIOC

#define LCD_DC_Pin          GPIO_PIN_4
#define LCD_DC_GPIO_Port    GPIOA
#define LCD_SCL_Pin         GPIO_PIN_5
#define LCD_SCL_GPIO_Port   GPIOA
#define LCD_RES_Pin         GPIO_PIN_6
#define LCD_RES_GPIO_Port   GPIOA
#define LCD_SDA_Pin         GPIO_PIN_7
#define LCD_SDA_GPIO_Port   GPIOA
#define LCD_BLK_Pin         GPIO_PIN_4
#define LCD_BLK_GPIO_Port   GPIOC
#define LCD_CS_Pin          GPIO_PIN_5
#define LCD_CS_GPIO_Port    GPIOC

#define USE_HORIZONTAL 2  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 320
#define LCD_H 480

#else
#define LCD_W 480
#define LCD_H 320
#endif



//-----------------LCD�˿ڶ���---------------- 

#define LCD_SCLK_Clr() LCD_SCL_GPIO_Port->BSRR = ((uint32_t)LCD_SCL_Pin << 16U)///HAL_GPIO_WritePin()//GPIO_ResetBits(GPIOG,GPIO_Pin_12)//SCL=SCLK
#define LCD_SCLK_Set() LCD_SCL_GPIO_Port->BSRR = LCD_SCL_Pin

#define LCD_MOSI_Clr() LCD_SDA_GPIO_Port->BSRR = ((uint32_t)LCD_SDA_Pin << 16U)//GPIO_ResetBits(GPIOD,GPIO_Pin_5)//SDA=MOSI
#define LCD_MOSI_Set() LCD_SDA_GPIO_Port->BSRR = LCD_SDA_Pin//GPIO_SetBits(GPIOD,GPIO_Pin_5)

#define LCD_RES_Clr()  HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_RESET)//GPIO_ResetBits(GPIOD,GPIO_Pin_4)//RES
#define LCD_RES_Set()  HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_SET)//GPIO_SetBits(GPIOD,GPIO_Pin_4)

#define LCD_DC_Clr()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_RESET)//GPIO_ResetBits(GPIOD,GPIO_Pin_15)//DC
#define LCD_DC_Set()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_SET)//GPIO_SetBits(GPIOD,GPIO_Pin_15)
 		     
#define LCD_CS_Clr()   HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET)//GPIO_ResetBits(GPIOD,GPIO_Pin_1)//CS
#define LCD_CS_Set()   HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET)//GPIO_SetBits(GPIOD,GPIO_Pin_1)

#define LCD_BLK_Clr()  HAL_GPIO_WritePin(LCD_BLK_GPIO_Port,LCD_BLK_Pin,GPIO_PIN_RESET)//GPIO_ResetBits(GPIOE,GPIO_Pin_8)//BLK
#define LCD_BLK_Set()  HAL_GPIO_WritePin(LCD_BLK_GPIO_Port,LCD_BLK_Pin,GPIO_PIN_SET)//GPIO_SetBits(GPIOE,GPIO_Pin_8)





void LCD_Writ_Bus(u8 dat);//ģ��SPIʱ��
void LCD_WR_DATA8(u8 dat);//д��һ���ֽ�
void LCD_WR_DATA(u32 dat);//д�������ֽ�
void LCD_WR_REG(u8 dat);//д��һ��ָ��
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//�������꺯��
//void LCD_Init(void);//LCD��ʼ��


#endif



