#include "BSP_IMUMSPM0.h"


HAL_StatusTypeDef BSP_IMUMSPM0_SetBuadrate(uint32_t newBuadrate)
{
    HAL_StatusTypeDef status = HAL_OK;
    //等待串口空闲
    while(__HAL_UART_GET_FLAG((IMUMSPM0_UART_HANDLE), UART_FLAG_TC) != SET);
    //失能串口外设
    __HAL_UART_DISABLE(IMUMSPM0_UART_HANDLE);
    //修改对应波特率参数
    (IMUMSPM0_UART_HANDLE)->Init.BaudRate = newBuadrate;
    //重新初始化串口
    status = HAL_UART_Init(IMUMSPM0_UART_HANDLE);

    return status;

}

void BSP_IMUMSPM0_Init(IMUMSPM0_Device* Device)
{
    HAL_UART_Receive_IT(IMUMSPM0_UART_HANDLE,(uint8_t*)(&Device->Data.DataByte),1);
}





