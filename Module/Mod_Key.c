#include "Mod_Key.h"


static KeyState Key_Num = KEY_NONE;



/**
  * @brief  获取按键来源
  * @param  无
  * @retval Keystate按键来源号或未检测状态（0）
  */
KeyState Key_GetNum(void)
{
	KeyState Temp = Key_Num;

	if (Key_Num == KEY_START)         //启停键按下
	{
		Key_Num = KEY_NONE;
		return Temp;
	}
    else if(Key_Num == KEY_CONTROL)   //控制键按下
    {
		Key_Num = KEY_NONE;
		return Temp;
	}
	else if(Key_Num == KEY_MODE)      //模式键按下
	{
		Key_Num = KEY_NONE;
		return Temp;
	}
	else if(Key_Num == KEY_ARM)      //ARM键按下
	{
		Key_Num = KEY_NONE;
		return Temp;
	}

	return KEY_NONE;                  //按键未按下
}

/**
  * @brief  定时中断中获取按键状态计时
  * @note   非阻塞式按键
  * @param  无
  * @retval 无
  */
void Key_Tick(void)
{
	static uint8_t Count      = 0;
	static KeyState CurrState = KEY_NONE;
	static KeyState PrevState = KEY_NONE;
	static KeyState StateBuf  = KEY_NONE;
	
	Count ++;

    /* 每10次定时中断更新按键状态 */
	if (Count >= 4)
	{
		
		Count = 0;
		
		PrevState = CurrState;
		CurrState = Key_GetState();
		
		if(CurrState != KEY_NONE)
		{
			StateBuf = CurrState;
		}
		
	    /* 松手检测并获取按键号 */
		if (CurrState == KEY_NONE && PrevState != KEY_NONE)
		{
			Key_Num = StateBuf;
			StateBuf = KEY_NONE;
		}
	}
}
