#include "BSP_Buzzer.h"

#define Buzzer_Device_MaxNum   3

static Buzzer_Device Buzzer_Device_Pool[Buzzer_Device_MaxNum] = {0};

static uint8_t Device_Num = 0;

/**
  * @brief  Buzzer BSP层初始化 默认把蜂鸣器关闭
  * @param  无
  * @retval 无
  */
Buzzer_Device* BSP_Buzzer_Init(Buzzer_InitConfig* Buzzer)
{
    if(Buzzer == NULL)
    {
        return NULL;
    }
    if(Device_Num >= Buzzer_Device_MaxNum)
    {
        return NULL;
    }

    Buzzer_Device* Dev = NULL;

    Buzzer_Device_Pool[Buzzer->Buzzer_Name].Instance        = Buzzer->Buzzer_Name;
    Buzzer_Device_Pool[Buzzer->Buzzer_Name].GPIO.GPIO_Port  = Buzzer->GPIO_Port;
    Buzzer_Device_Pool[Buzzer->Buzzer_Name].GPIO.GPIO_Pin   = Buzzer->GPIO_Pin;
    Buzzer_Device_Pool[Buzzer->Buzzer_Name].State           = BUZZER_OFF;

    Dev = &Buzzer_Device_Pool[Device_Num];
    Device_Num++;

    HAL_GPIO_WritePin(Buzzer->GPIO_Port, Buzzer->GPIO_Pin, GPIO_PIN_SET);

    return Dev;
}



/**
  * @brief  设置蜂鸣器状态
  * @param  无
  * @retval 无
  */
void BSP_Buzzer_SetState(Buzzer_Device* buzzer, Buzzer_State state)
{
    if(buzzer == NULL)
    {
        return;
    }
    buzzer->State = state;

    switch (buzzer->State)
    {
    case BUZZER_ON:
        HAL_GPIO_WritePin(buzzer->GPIO.GPIO_Port, buzzer->GPIO.GPIO_Pin, GPIO_PIN_RESET);
        break;
    
    case BUZZER_OFF:
        HAL_GPIO_WritePin(buzzer->GPIO.GPIO_Port, buzzer->GPIO.GPIO_Pin, GPIO_PIN_SET);
        break;
    
     default:
        break;
    }
}


