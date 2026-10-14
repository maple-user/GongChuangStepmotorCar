#ifndef __MOD_STEPMOTORARM_H
#define __MOD_STEPMOTORARM_H

#include "BSP_X_V2.h"

typedef struct
{
    uint8_t  StepMotor_Addr[2];   //总线上挂载的所有步进电机的地址，默认最多4个
    bool     EnableFlag[2];       //总线上挂载的所有步进电机的使能标志，默认最多4个
   __IO bool TaskFlag[2];         //总线电机任务标志位，true任务已完成，false任务未执行结束
    uint8_t  RxBuffer[20];        //接收缓冲区,用于接收步进电机返回的状态信息
    uint32_t MotorNum;            //总线上挂载的步进电机数量
    float    wSpeed;              //旋转角速度
    float    vSpeed;              //移动速度
    float    Angle;               //旋转角度
    float    Position;            //位置
}StepMotorArm_Master; 

void StepMotorArm_Init(void);
StepMotorArm_Master* StepMotorArm_GetMaster(void);
void StepMotorArm_Move_cm(StepMotorArm_Master* Master, float X_cm, float Y_cm);

#endif


