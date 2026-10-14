#ifndef __TEST_H_
#define __TEST_H_

#include "App.h"

typedef struct
{
    
    Motor_Device  * L_Motor;
    Motor_Device  * R_Motor;
    Sensor_Base   * Sensor;
    //Vidio_Device * Video; 
    KeyState  KeyNum;
    PID_TypeDef   * PID_L;
    PID_TypeDef   * PID_R;

}Test_Handle;

HAL_StatusTypeDef Test_Init(Test_Handle* Test);
void MPU6050_Test(void);
HAL_StatusTypeDef Sensor_Test(Test_Handle* Test);
HAL_StatusTypeDef Motor_Test(Test_Handle* Test);
void Buzzer_Test(void);
void PID_Test(Test_Handle* Test);

#endif

