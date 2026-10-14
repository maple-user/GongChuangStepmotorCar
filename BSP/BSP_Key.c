#include "BSP_Key.h"


/**
  * @brief  按键状态监测
  * @param  无
  * @retval KEY_NONE无按键按下 KEY_START开始按键按下 KEY_CONTROL控制按键按下
  */
 KeyState Key_GetState(void)
{
	if (HAL_GPIO_ReadPin(Start_Key_GPIO_Port, Start_Key_Pin)     == GPIO_PIN_RESET)
	{
		return KEY_START;
	}
    if (HAL_GPIO_ReadPin(Control_Key_GPIO_Port, Control_Key_Pin) == GPIO_PIN_RESET)
    {
         return KEY_CONTROL;
    }
    if(HAL_GPIO_ReadPin(Mode_Key_GPIO_Port, Mode_Key_Pin)        == GPIO_PIN_RESET)
    {
         return KEY_MODE;
    }
	if(HAL_GPIO_ReadPin(Key_Arm_GPIO_Port, Key_Arm_Pin)        == GPIO_PIN_RESET)
    {
         return KEY_ARM;
    }
	return KEY_NONE;
}
