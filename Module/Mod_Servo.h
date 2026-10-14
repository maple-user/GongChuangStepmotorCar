#ifndef __MOD_SERVO_H
#define __MOD_SERVO_H

#include "BSP_Servo.h"

#define Servo_Plate_ID    0
//#define Servo_Limit_ID    1
#define Servo_Yaw_ID      1
#define Servo_Claw_ID     2


#define Servo_Plate_Stuff1_Angle  13.5f
#define Servo_Plate_Stuff2_Angle  132.5f
#define Servo_Plate_Stuff3_Angle  248.5f

#define Servo_CLAW_OPEN_ANGLE     120.0f
#define Servo_CLAW_HOME_ANGLE     31.5f

void Servo_Init(void);
Servo_Device* Servo_GetDevice(uint8_t Servo_ID);
void Servo_SetAngle(Servo_Device* Servo, float Angle);

	
#endif

