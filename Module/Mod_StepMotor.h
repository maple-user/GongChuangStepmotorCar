#ifndef __MOD_STEPMOTOR_H
#define __MOD_STEPMOTOR_H

#include "BSP_X_V2.h"

typedef struct
{
    uint8_t  StepMotor_Addr[4];   //总线上挂载的所有步进电机的地址，默认最多4个
    bool     EnableFlag[4];       //总线上挂载的所有步进电机的使能标志，默认最多4个
   __IO bool TaskFlag[4];         //总线电机任务标志位，true任务已完成，false任务未执行结束
    uint8_t  RxBuffer[20];        //接收缓冲区,用于接收步进电机返回的状态信息
    uint32_t MotorNum;            //总线上挂载的步进电机数量
    float    wSpeed;              //旋转角速度
    float    vSpeed;              //移动速度
    float    SpeedAngle;          //速度方向角度,标准极坐标
    float    Angle;               //旋转角度
    float    Position;            //位置
    float    ActualPosition;      //管理器实际位置
    float    ActualAngle;         //管理器实际期望角度

}StepMotor_Master; 


/**
  * @brief  ZDT闭环步进电机控制器初始化函数
  * @param  无
  * @retval 无
  */
void StepMotor_Init(void);

/**
  * @brief  ZDT闭环步进电机控制器句柄获取函数
  * @param  无
  * @retval 无
  */
StepMotor_Master* StepMotor_GetMaster(void);

/**
  * @brief  ZDT闭环步进电机控制器麦轮解算位置移动函数（梯形加速）
  * @param  Master  步进电机控制器句柄
  * @param  cm      移动距离（厘米）
  * @param  Angle   移动角度
  * @retval 无
  */
void StepMotor_Move_cm(StepMotor_Master* Master, float cm, float Angle);

/**
  * @brief  ZDT闭环步进电机控制器麦轮解算位置移动函数（梯形加速）
  * @param  Master  步进电机控制器句柄
  * @param  cm      移动距离（毫米）
  * @param  Angle   移动角度
  * @retval 无
  */
void StepMotor_Move_mm(StepMotor_Master* Master, float mm, float Angle);

/**
  * @brief  ZDT闭环步进电机控制器麦轮解算速度移动函数
  * @param  Master      步进电机控制器句柄
  * @param  vSpeed      移动速度
  * @param  SpeedAngle  移动方向角度
  * @retval 无
  */
void StepMotor_SetvSpeed(StepMotor_Master* Master,float vSpeed, float SpeedAngle);

/**
  * @brief  ZDT闭环步进电机控制器麦轮解算角速度控制函数
  * @param  Master      步进电机控制器句柄
  * @param  deg_s       预期角速度
  * @retval 无
  */
void StepMotor_SetwSpeed(StepMotor_Master* Master, float deg_s);

/**
  * @brief  ZDT闭环步进电机控制器麦轮解算速度+角速度PID控制函数
  * @param  Master      步进电机控制器句柄
  * @param  vSpeed      移动速度
  * @param  deg_s       预期角速度
  * @param  SpeedAngle  移动方向角度
  * @retval 无
  */
void StepMotor_SetSpeed_PID(StepMotor_Master* Master,float vSpeed,float deg_s ,float SpeedAngle);


#endif


