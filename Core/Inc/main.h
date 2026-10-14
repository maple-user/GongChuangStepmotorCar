/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Servo_Claw_PWM_Pin GPIO_PIN_0
#define Servo_Claw_PWM_GPIO_Port GPIOA
#define Servo_Plate_PWM_Pin GPIO_PIN_1
#define Servo_Plate_PWM_GPIO_Port GPIOA
#define MAIXCAM_Control_RX_Pin GPIO_PIN_2
#define MAIXCAM_Control_RX_GPIO_Port GPIOA
#define MAIXCAM_Control_TX_Pin GPIO_PIN_3
#define MAIXCAM_Control_TX_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_4
#define LCD_DC_GPIO_Port GPIOA
#define LCD_SCL_Pin GPIO_PIN_5
#define LCD_SCL_GPIO_Port GPIOA
#define LCD_RES_Pin GPIO_PIN_6
#define LCD_RES_GPIO_Port GPIOA
#define LCD_SDA_Pin GPIO_PIN_7
#define LCD_SDA_GPIO_Port GPIOA
#define LCD_BLK_Pin GPIO_PIN_4
#define LCD_BLK_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_5
#define LCD_CS_GPIO_Port GPIOC
#define OLED_SCL_Pin GPIO_PIN_0
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_1
#define OLED_SDA_GPIO_Port GPIOB
#define Key_Start_Pin GPIO_PIN_7
#define Key_Start_GPIO_Port GPIOE
#define Key_Control_Pin GPIO_PIN_8
#define Key_Control_GPIO_Port GPIOE
#define Key_Mode_Pin GPIO_PIN_10
#define Key_Mode_GPIO_Port GPIOE
#define IMUMSPM0_RX_Pin GPIO_PIN_10
#define IMUMSPM0_RX_GPIO_Port GPIOB
#define IMUMSPM0_TX_Pin GPIO_PIN_11
#define IMUMSPM0_TX_GPIO_Port GPIOB
#define BT_KEY_Pin GPIO_PIN_14
#define BT_KEY_GPIO_Port GPIOB
#define BT_STATE_Pin GPIO_PIN_15
#define BT_STATE_GPIO_Port GPIOB
#define Servo_Yaw_PWM_Pin GPIO_PIN_12
#define Servo_Yaw_PWM_GPIO_Port GPIOD
#define Servo_MAIXCAM_PWM_Pin GPIO_PIN_13
#define Servo_MAIXCAM_PWM_GPIO_Port GPIOD
#define BT_RX_Pin GPIO_PIN_10
#define BT_RX_GPIO_Port GPIOC
#define BT_TX_Pin GPIO_PIN_11
#define BT_TX_GPIO_Port GPIOC
#define LED_Green_Pin GPIO_PIN_0
#define LED_Green_GPIO_Port GPIOD
#define LED_Red_Pin GPIO_PIN_1
#define LED_Red_GPIO_Port GPIOD
#define LED_Blue_Pin GPIO_PIN_3
#define LED_Blue_GPIO_Port GPIOD
#define Buzzer_Pin GPIO_PIN_4
#define Buzzer_GPIO_Port GPIOD
#define StepMotor_TX_Pin GPIO_PIN_6
#define StepMotor_TX_GPIO_Port GPIOB
#define StepMotor_RX_Pin GPIO_PIN_7
#define StepMotor_RX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define StepMotorArm_RedLED_Pin  GPIO_PIN_11
#define StepMotorArm_RedLED_GPIO_Port  GPIOE

#define Key_Arm_Pin  GPIO_PIN_12
#define Key_Arm_GPIO_Port  GPIOE
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
