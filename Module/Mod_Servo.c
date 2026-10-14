#include "Mod_Servo.h"

//标准舵机的零点CCR值
#define Servo_Zero_CCRNUM        ((uint16_t)500U)

//项目所需舵机对应默认值
#define Servo_Plate_DefaultCCR    Servo_Zero_CCRNUM
//#define Servo_Limit_DefaultCCR    Servo_Zero_CCRNUM
#define Servo_Yaw_DefaultCCR      Servo_Zero_CCRNUM
#define Servo_Claw_DefaultCCR     Servo_Zero_CCRNUM

#define Servo_Device_MAXNUM   4

static Servo_Device* Servo_Device_Pool[Servo_Device_MAXNUM] = {0};

/**
  * @brief  Servo模块层初始化
  * @param  无
  * @retval 无
  * @note   根据 Servo_InitConfig 配置Servo设备结构体指针池
  */
void Servo_Init(void)
{
    Servo_InitConfig Servo_InitStruct = {0};

    Servo_InitStruct.Servo_Name       = Servo_Plate_ID;
    Servo_InitStruct.TIM_HANDLE       = SERVO_TIM1;
    Servo_InitStruct.PWM_CHANNEL      = SERVO_PLATE_PWM_CHANNEL;
    Servo_InitStruct.Type             = SERVO_TYPE_270;
    Servo_Device_Pool[Servo_Plate_ID] = BSP_Servo_Init(&Servo_InitStruct);

	
	Servo_InitStruct.Servo_Name       = Servo_Yaw_ID;
	Servo_InitStruct.TIM_HANDLE       = SERVO_TIM2;
    Servo_InitStruct.PWM_CHANNEL      = SERVO_YAW_PWM_CHANNEL;
    Servo_InitStruct.Type             = SERVO_TYPE_180;
    Servo_Device_Pool[Servo_Yaw_ID]   = BSP_Servo_Init(&Servo_InitStruct);
	
	Servo_InitStruct.Servo_Name       = Servo_Claw_ID;
	Servo_InitStruct.TIM_HANDLE       = SERVO_TIM1;
    Servo_InitStruct.PWM_CHANNEL      = SERVO_CLAW_PWM_CHANNEL;
    Servo_InitStruct.Type             = SERVO_TYPE_180;
    Servo_Device_Pool[Servo_Claw_ID]  = BSP_Servo_Init(&Servo_InitStruct);
	
	//上电给每个舵机设置默认值
	BSP_Servo_SetAngle(Servo_Device_Pool[Servo_Plate_ID],Servo_Plate_DefaultCCR);
	BSP_Servo_SetAngle(Servo_Device_Pool[Servo_Yaw_ID],Servo_Yaw_DefaultCCR);
	BSP_Servo_SetAngle(Servo_Device_Pool[Servo_Claw_ID],Servo_Claw_DefaultCCR);
	
	
}

/**
  * @brief  Servo模块层获取Servo设备结构体指针
  * @param  Servo_ID Servo设备结构体名字
  * @retval Servo_Device* Servo设备结构体指针
  * @note   必须在Servo_Init()后调用
  */
Servo_Device* Servo_GetDevice(uint8_t Servo_ID)
{
    if(Servo_ID >= Servo_Device_MAXNUM)
    {
        return NULL;
    }
    return Servo_Device_Pool[Servo_ID];
}

/**
  * @brief  Servo模块层设置角度
  * @param  Angle               目标角度
  * @retval Servo_Device* Servo 设备结构体指针
  */
void Servo_SetAngle(Servo_Device* Servo, float Angle)
{
	if(Servo == NULL)
	{
		return;
	}
	switch(Servo->Type)
	{
		case SERVO_TYPE_180:
			if (Angle < 0.0f) 
			{
				Angle = 0.0f;
			}
            if (Angle > 180.0f) 
			{
				Angle = 180.0f;
			}
			Servo->Angle = (Angle * (2500.0f - Servo_Zero_CCRNUM) / 180.0f + Servo_Zero_CCRNUM);
			BSP_Servo_SetAngle(Servo,Servo->Angle);
			break;
		case SERVO_TYPE_270:
			if (Angle < 0.0f) 
			{
				Angle = 0.0f;
			}
			if (Angle > 270.0f) 
			{
				Angle = 270.0f;
			}
			Servo->Angle = (Angle * (2500.0f - Servo_Zero_CCRNUM) / 270.0f + Servo_Zero_CCRNUM);
			BSP_Servo_SetAngle(Servo,Servo->Angle);
			break;
		case SERVO_TYPE_360:
			if (Angle < 0.0f) 
			{
				Angle = 0.0f;
			}
            if (Angle > 360.0f) 
			{
				Angle = 360.0f;
			}
			break;
		default:
			break;
		
	}
}

