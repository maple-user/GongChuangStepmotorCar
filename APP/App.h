#ifndef __APP_H
#define __APP_H

//HAL库
#include "stm32f4xx_hal.h"

//C库包含
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
//HAL库外设库路径包含
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"


//BSP层库路径包含
#include "BSP_LED.h"
#include "BSP_DWT.h"
#include "BSP_BlueTooth.h"
#include "BSP_IMUMSPM0.h"
#include "BSP_MAIXCAM.h"
#include "BSP_Buzzer.h"
#include "BSP_OLED.h"
#include "BSP_Servo.h"
#include "BSP_X_V2.h"
#include "BSP_LCD.h"
//模块层库路径包含

#include "Mod_Key.h"
#include "Mod_LED.h"
#include "Mod_BlueTooth.h"
#include "Mod_IMUMSPM0.h"
#include "Mod_StepMotor.h"
#include "Mod_MAIXCAM.h"
#include "Mod_Buzzer.h"
#include "Mod_OLED.h"
#include "Mod_Servo.h"
#include "Mod_StepMotorArm.h"
#include "Mod_LCD.h"
//PID算法库路径包含
#include "PID.h"

//App任务层路径包含
#include "App_task.h"
#include "App_questions.h"


#define APP_ENABLE           4U
#define APP_DISABLE          5U

#define STM32F4xx_SystemFrequency_MHz            168U

void App_Init (void);
void App_Loop (void);
void App_Refresh (void);



#endif
