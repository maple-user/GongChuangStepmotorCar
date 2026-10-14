#include "BSP_MAIXCAM.h"

#define MAIXCAM_RX_BUFFER_SIZE         20

//总线电机注册表池,一般就使用一个
#define MAIXCAM_Device_MAXNUM           1
static MAIXCAM_Device* MAIXCAM_Device_Pool[MAIXCAM_Device_MAXNUM] = {0};

void MAIXCAM_Init(void)
{
    MAIXCAM_InitConfig InitStruct = {0};
    InitStruct.UART = MAIXCAM_UART_HANDLE;
    InitStruct.DMA  = MAIXCAM_DMA_UART_RX_HANDLE;

    MAIXCAM_Device_Pool[0] = BSP_MAIXCAM_Init(&InitStruct);


}

MAIXCAM_Device* MAIXCAM_GetDevice(void)
{
    return MAIXCAM_Device_Pool[0];
}


void MAIXCAM_GetDataCallBack(MAIXCAM_Device* Device, uint16_t Size)
{
    if(Device == NULL)
    {
        return;
    }

    if(Size >2 && Size < MAIXCAM_RX_BUFFER_SIZE)
    {
        if(MAIXCAM_Device_Pool[0]->MAIXCAM_RxBuffer[0] == '[' && MAIXCAM_Device_Pool[0]->MAIXCAM_RxBuffer[5] == ']')
        {
            Device->ActualPosition = (float)(((MAIXCAM_Device_Pool[0]->MAIXCAM_RxBuffer[1]<<8) | MAIXCAM_Device_Pool[0]->MAIXCAM_RxBuffer[2])/2000.0f); //实际位置
            Device->ActualAngle = (float)(((MAIXCAM_Device_Pool[0]->MAIXCAM_RxBuffer[3]<<8) | MAIXCAM_Device_Pool[0]->MAIXCAM_RxBuffer[4])/2000.0f);    //实际角度(角度制)
        }
        
    }
    
    //开启下一帧接收
    BSP_MAIXCAM_StartReceive_DMA(Device, MAIXCAM_Device_Pool[0]->MAIXCAM_RxBuffer);
   
}
