#ifndef __BSP_MAIXCAM_H
#define __BSP_MAIXCAM_H

#include "main.h"
#include "usart.h"
#include <stdbool.h>

#define MAIXCAM_UART_HANDLE              &huart2
#define MAIXCAM_DMA_UART_RX_HANDLE     &hdma_usart2_rx



typedef struct 
{
    UART_HandleTypeDef*   UART;   //串口句柄
    DMA_HandleTypeDef*    DMA;    //DMA句柄

    uint8_t MAIXCAM_RxBuffer[20]; //接收缓冲区
  __IO float    ActualPosition;   //管理器实际位置
  __IO float    ActualAngle;      //管理器实际期望角度

    bool  USE_QR;                //是否使用二维码识别功能
    bool  QR_GetData;            //二维码标志位，0表示未接收到数据，
}MAIXCAM_Device;


typedef struct 
{
    UART_HandleTypeDef*   UART;   //串口句柄
    DMA_HandleTypeDef*    DMA;    //DMA句柄

}MAIXCAM_InitConfig;

MAIXCAM_Device* BSP_MAIXCAM_Init(MAIXCAM_InitConfig*MAIXCAM);
void BSP_MAIXCAM_StartReceive_DMA(MAIXCAM_Device* Device, uint8_t* DataBuf);


#endif



