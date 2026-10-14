#include "BSP_MAIXCAM.h"
#include <string.h>



//总线电机注册表池,一般就使用一个
#define MAIXCAM_Device_MAXNUM           1
static MAIXCAM_Device MAIXCAM_Device_Pool[MAIXCAM_Device_MAXNUM] = {0};

MAIXCAM_Device* BSP_MAIXCAM_Init(MAIXCAM_InitConfig*MAIXCAM)
{
    if(MAIXCAM == NULL)
    {
        return NULL;
    }
   MAIXCAM_Device_Pool[0].UART = MAIXCAM ->UART;
   MAIXCAM_Device_Pool[0].DMA  = MAIXCAM ->DMA;
   MAIXCAM_Device_Pool[0].ActualAngle    = 0.0f;
   MAIXCAM_Device_Pool[0].ActualPosition = 0.0f;

   MAIXCAM_Device_Pool[0].USE_QR = false;
   MAIXCAM_Device_Pool[0].QR_GetData = false;
   memset(MAIXCAM_Device_Pool[0].MAIXCAM_RxBuffer,0,sizeof(MAIXCAM_Device_Pool[0].MAIXCAM_RxBuffer));
   
   //启动首次DMA接收
   BSP_MAIXCAM_StartReceive_DMA(&MAIXCAM_Device_Pool[0],MAIXCAM_Device_Pool[0].MAIXCAM_RxBuffer);
  
   return &MAIXCAM_Device_Pool[0];
}



void BSP_MAIXCAM_StartReceive_DMA(MAIXCAM_Device* Device, uint8_t* DataBuf)
{
    if(Device == NULL)
    {
        return;
    }
    HAL_UARTEx_ReceiveToIdle_DMA(Device->UART, DataBuf, sizeof(DataBuf));
    __HAL_DMA_DISABLE_IT(Device->DMA,DMA_IT_HT);
}



