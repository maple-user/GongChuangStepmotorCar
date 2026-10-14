#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "main.h"
#include "usart.h"

#include <stdio.h>

/**
  * @brief  串口BSP层串口空闲中断DMA接收字符串数据
  * @param  String  字符串指针
  * @retval 无
  */
#define BSP_UART_RecieveToIdle_DMA(Handle,pData,Size)     \
do                                                        \
{                                                         \
    HAL_UARTEx_ReceiveToIdle_DMA((Handle),(pData),(Size));\
}while(0U)


HAL_StatusTypeDef UART_SendString_Block(char* String);


#endif

