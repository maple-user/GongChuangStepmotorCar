#include "BSP_Servo.h"
#include "tim.h" // 包含CubeMX生成的定时器初始化代码

#define Servo_Device_MaxNum   4

static Servo_Device Servo_Device_Pool[Servo_Device_MaxNum] = {0};

static uint8_t Device_Num = 0;

/**
  * @brief  Servo BSP层初始化 默认复位舵机
  * @param  无
  * @retval 无
  */
Servo_Device* BSP_Servo_Init(Servo_InitConfig* Servo)
{
	//配置结构体空指针检查
    if(Servo == NULL)
    {
        return NULL;
    }
	//设备池注册数量检查
    if(Device_Num >= Servo_Device_MaxNum)
    {
        return NULL;
    }

    Servo_Device* Dev = NULL;

	//对于一个申请成功的实例初始化
    Servo_Device_Pool[Device_Num].Instance        = Servo->Servo_Name;
    Servo_Device_Pool[Device_Num].TIM.TIM_HANDLE  = Servo->TIM_HANDLE;
    Servo_Device_Pool[Device_Num].TIM.PWM_CHANNEL = Servo->PWM_CHANNEL;
    Servo_Device_Pool[Device_Num].Type            = Servo->Type;
	Servo_Device_Pool[Device_Num].Angle           = 0.0f;
	
    Dev = &Servo_Device_Pool[Device_Num];
    
	//启动舵机对应控制通道PWM输出
	HAL_TIM_PWM_Start(Servo->TIM_HANDLE,Servo->PWM_CHANNEL);
	
    Device_Num++;
    return Dev;
}

/**
  * @brief  BSP层舵机设置角度
  * @param  无
  * @retval 无
  */
void BSP_Servo_SetAngle(Servo_Device* Servo, float CCR)
{
	
	__HAL_TIM_SET_COMPARE(Servo->TIM.TIM_HANDLE,Servo->TIM.PWM_CHANNEL,(uint16_t)CCR);
}
