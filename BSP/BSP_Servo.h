#ifndef __BSP_SERVO_H_
#define __BSP_SERVO_H_

#include "main.h"
#include "tim.h"



//舵机对应板级外设定义
#define SERVO_TIM1                    &htim2
#define SERVO_TIM2                    &htim4


#define SERVO_PLATE_PWM_CHANNEL      TIM_CHANNEL_2  //物料盘舵机
#define SERVO_YAW_PWM_CHANNEL        TIM_CHANNEL_1  //机械臂YAW轴舵机
#define SERVO_CLAW_PWM_CHANNEL       TIM_CHANNEL_1  //机械爪舵机


typedef enum
{
	SERVO_TYPE_180 = 1,            
	SERVO_TYPE_270 = 2,            
	SERVO_TYPE_360 = 3         
}Servo_Type;

typedef struct
{
	TIM_HandleTypeDef*  TIM_HANDLE;
	uint32_t            PWM_CHANNEL;
}Servo_TIM;

typedef struct
{
	uint8_t         Instance;
	Servo_Type      Type;
	float           Angle;
	Servo_TIM       TIM;
}Servo_Device;

typedef struct
{
	uint8_t            Servo_Name;
	Servo_Type         Type;
	TIM_HandleTypeDef* TIM_HANDLE;
	uint32_t           PWM_CHANNEL;
}Servo_InitConfig;




Servo_Device* BSP_Servo_Init(Servo_InitConfig* Servo);
void BSP_Servo_SetAngle(Servo_Device* Servo, float CCR);



#endif
