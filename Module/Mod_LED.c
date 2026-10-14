#include "Mod_LED.h"
#include "BSP_DWT.h"


#define LED_Device_MAXNUM   4

static LED_Device* LED_Device_Pool[LED_Device_MAXNUM] = {0};

/**
  * @brief  LED模块层初始化
  * @param  无
  * @retval 无
  * @note   根据 LED_InitConfig 配置LED设备结构体指针池
  */
void LED_Init(void)
{
    LED_InitConfig LED_InitStruct = {0};

    LED_InitStruct.LED_Name   = LED_Red;
    LED_InitStruct.GPIO_Port  = LED_Red_GPIO_Port;
    LED_InitStruct.GPIO_Pin   = LED_Red_Pin;
    LED_InitStruct.LED_State  = LED_OFF;
    LED_Device_Pool[0] = BSP_LED_Init(&LED_InitStruct);

    LED_InitStruct.LED_Name   = LED_Green;
    LED_InitStruct.GPIO_Port  = LED_Green_GPIO_Port;
    LED_InitStruct.GPIO_Pin   = LED_Green_Pin;
    LED_InitStruct.LED_State  = LED_OFF;
    LED_Device_Pool[1] = BSP_LED_Init(&LED_InitStruct);

    LED_InitStruct.LED_Name   = LED_Blue;
    LED_InitStruct.GPIO_Port  = LED_Blue_GPIO_Port;
    LED_InitStruct.GPIO_Pin   = LED_Blue_Pin;
    LED_InitStruct.LED_State  = LED_OFF;
    LED_Device_Pool[2] = BSP_LED_Init(&LED_InitStruct);

    LED_InitStruct.LED_Name   = LED_StepMotorArm;
    LED_InitStruct.GPIO_Port  = StepMotorArm_RedLED_GPIO_Port;
    LED_InitStruct.GPIO_Pin   = StepMotorArm_RedLED_Pin;
    LED_InitStruct.LED_State  = LED_OFF;
    LED_Device_Pool[3] = BSP_LED_Init(&LED_InitStruct);
}

/**
  * @brief  LED模块层获取LED设备结构体指针
  * @param  LED_Name LED设备结构体名字
  * @retval LED_Device* LED设备结构体指针
  * @note   必须在LED_Init()后调用
  */
LED_Device* LED_GetDevice(uint8_t LED_Name)
{
    if(LED_Name >= LED_Device_MAXNUM)
    {
        return NULL;
    }
    return LED_Device_Pool[LED_Name];
}

/**
  * @brief  LED模块层设置LED灯闪烁
  * @param  LED  LED设备结构体指针
  * @note   必须先初始化DWT以实现延时功能
  * @retval 无
  */
 void LED_Blink(LED_Device* LED,float ms)
 {
    if(LED == NULL)
    {
        return;
    }
    BSP_LED_Toggle(LED);
    DWT_Delay_ms(ms);
 }











