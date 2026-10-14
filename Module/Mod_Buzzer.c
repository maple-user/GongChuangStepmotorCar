#include "Mod_Buzzer.h"
#include "BSP_DWT.h"


#define Buzzer_Device_MAXNUM   1

static Buzzer_Device* Buzzer_Device_Pool[Buzzer_Device_MAXNUM] = {0};

/**
  * @brief  Buzzer模块层初始化
  * @param  无
  * @retval 无
  * @note   根据 Buzzer_InitConfig 配置Buzzer设备结构体指针池
  */
void Buzzer_Init(void)
{
    Buzzer_InitConfig Buzzer_InitStruct = {0};

    Buzzer_InitStruct.Buzzer_Name   = Buzzer_1;
    Buzzer_InitStruct.GPIO_Port  = Buzzer_GPIO_Port;
    Buzzer_InitStruct.GPIO_Pin   = Buzzer_Pin;
    Buzzer_InitStruct.Buzzer_State  = BUZZER_OFF;
    Buzzer_Device_Pool[0] = BSP_Buzzer_Init(&Buzzer_InitStruct);
}


/**
  * @brief  Buzzer模块层获取Buzzer设备结构体指针
  * @param  Buzzer_Name Buzzer设备结构体名字
  * @retval Buzzer_Device* Buzzer设备结构体指针
  * @note   必须在Buzzer_Init()后调用
  */
Buzzer_Device* Buzzer_GetDevice(uint8_t Buzzer_Name)
{
    if(Buzzer_Name >= Buzzer_Device_MAXNUM)
    {
        return NULL;
    }
    return Buzzer_Device_Pool[Buzzer_Name];
}

/**
  * @brief  Buzzer模块层设置Buzzer鸣响
  * @param  Buzzer  Buzzer设备结构体指针
  * @note   必须先初始化DWT以实现延时功能
  * @retval 无
  */
 void Buzzer_On(Buzzer_Device* Buzzer,float ms)
 {
    if(Buzzer == NULL)
    {
        return;
    }
    BSP_Buzzer_SetState(Buzzer, BUZZER_ON);
    DWT_Delay_ms(ms);
    BSP_Buzzer_SetState(Buzzer, BUZZER_OFF);
 }

