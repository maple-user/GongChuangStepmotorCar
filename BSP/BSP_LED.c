#include "BSP_LED.h"

#define LED_Device_MaxNum   4

static LED_Device LED_Device_Pool[LED_Device_MaxNum] = {0};

static uint8_t Device_Num = 0;

/**
  * @brief  LED BSP层初始化 默认把LED灯关闭
  * @param  无
  * @retval 无
  */
LED_Device* BSP_LED_Init(LED_InitConfig* LED)
{
    if(LED == NULL)
    {
        return NULL;
    }
    if(Device_Num >= LED_Device_MaxNum)
    {
        return NULL;
    }

    LED_Device* Dev = NULL;

    LED_Device_Pool[LED->LED_Name].Instance        = LED->LED_Name;
    LED_Device_Pool[LED->LED_Name].GPIO.GPIO_Port  = LED->GPIO_Port;
    LED_Device_Pool[LED->LED_Name].GPIO.GPIO_Pin   = LED->GPIO_Pin;
    LED_Device_Pool[LED->LED_Name].State           = LED_OFF;

    Dev = &LED_Device_Pool[Device_Num];
    Device_Num++;

    HAL_GPIO_WritePin(LED->GPIO_Port, LED->GPIO_Pin, GPIO_PIN_RESET);

    return Dev;
}



/**
  * @brief  设置LED灯状态
  * @param  无
  * @retval 无
  */
void LED_SetState(LED_Device* led, LED_State state)
{
    if(led == NULL)
    {
        return;
    }
    led->State = state;

    switch (led->State)
    {
    case LED_ON:
        HAL_GPIO_WritePin(led->GPIO.GPIO_Port, led->GPIO.GPIO_Pin, GPIO_PIN_SET);
        break;
    
    case LED_OFF:
        HAL_GPIO_WritePin(led->GPIO.GPIO_Port, led->GPIO.GPIO_Pin, GPIO_PIN_RESET);
        break;
    
     default:
        break;
    }
}




/**
  * @brief  BSP层翻转LED灯
  * @param  无
  * @retval 无
  */
void BSP_LED_Toggle(LED_Device* led)
{
    if(led == NULL)
    {
        return;
    }
    HAL_GPIO_TogglePin(led->GPIO.GPIO_Port, led->GPIO.GPIO_Pin);
}
