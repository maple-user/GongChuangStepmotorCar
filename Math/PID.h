#ifndef __PID_H
#define __PID_H

#include "main.h"
#include <math.h>
#include "tim.h"
#include <stdbool.h>


typedef struct 
{
	float Kp;
	float Ki;
	float Kd;
	float Kw;
	float Target;
	float Out;
	float Actualv;
	float Actualw;
	float Error0;
	float Error1;
	float ErrorInt;
	int16_t PID_Limit;
__IO bool IS_ENABLE;
__IO bool IS_ARRIVE;
	
}PID_TypeDef;        


extern PID_TypeDef PID_AglControl;     //PID角度环句柄声明
extern PID_TypeDef PID_PosControl;     //PID位置环句柄声明

#define PID_TIM_HANDLE                     &htim1 //PID算法定时器依赖
#define PID_TIME_MS                        20U    //20ms一次PID
 

#define AGL_KP_NUM                         0.95f   
#define AGL_KI_NUM                         0.0f
#define AGL_KD_NUM                         0.00006f
#define AGL_KW_NUM                         0.00015f

#define POS_KP_NUM                         0.0f   
#define POS_KI_NUM                         0.0f
#define POS_KD_NUM                         0.0f
#define POS_KW_NUM                         0.0f

void PID_Init(PID_TypeDef *PID, float Kp, float Ki, float Kd, float Kw ,float Target, float PID_Limit);
void PID_Start(void);
void PID_Stop(void);
void PID_Angle_Calculate (PID_TypeDef *PID);
void PID_Calculate (PID_TypeDef *PID);



#endif

