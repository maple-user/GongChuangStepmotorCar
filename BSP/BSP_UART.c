#include "BSP_UART.h"

#define UART_HANDLE    &huart4
#define UART_TIMOUT    100     //ms
 
/**
  * @brief  串口BSP层阻塞式发送单字节数据
  * @param  Handle  串口句柄指针
  * @param  Byte    单字节数据
  * @param  Timeout 超时等待值
  * @retval 无
  */
#define BSP_UART_SendByte_Block(Handle,Byte,Timeout)    \
do                                                      \
{                                                       \
     uint8_t temp = (uint8_t)(Byte);                    \
     HAL_UART_Transmit((Handle),&temp,1,(Timeout));     \
}while (0U)



/**
  * @brief  串口BSP层Printf()重定向
  * @param  ch  整形数据
  * @param  f   文件日志
  * @retval 无
  */
int fputc(int ch,FILE*f)
{
	BSP_UART_SendByte_Block((UART_HANDLE),ch,UART_TIMOUT);
  //uint8_t temp = (uint8_t)ch;
 // HAL_UART_Transmit(&huart4,&temp,1,100);           
	return ch;
}


/**
  * @brief  串口BSP层阻塞式发送字符串数据
  * @param  String  字符串指针
  * @retval HAL_StatusTypeDef HAL_OK发送成功 HAL_ERROR发送失败 HAL_BUSY串口忙 HAL_TIMEOUT发送超时
  */
HAL_StatusTypeDef UART_SendString_Block(char* String)
{

  for(uint8_t i = 0; String[i] != '\0'; i++)
  {
    HAL_StatusTypeDef status = HAL_UART_Transmit((UART_HANDLE),(uint8_t*)&String[i],1,UART_TIMOUT);
    if(status != HAL_OK)
    {
      return status;
    }
  }
  return HAL_OK;

}

