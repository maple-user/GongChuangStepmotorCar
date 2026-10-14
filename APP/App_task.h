#ifndef __APP_TASK_H
#define __APP_TASK_H

#include "App.h"


typedef enum
{
    App_Init_Wait = 1,
    App_Init_OK   = 2,
    App_Running   = 3,
    App_Standby   = 4,
    App_Error     = 5
} App_State;

typedef enum 
{
    App_Mode_Qst_2  = 1,
    App_Mode_Qst_3  = 2,
    App_Loop_Circle = 3,
  
}App_Mode;

typedef enum
{
    Task_READY = 0,
    Task_SLEEP = 1,
    Task_STOP  = 2
}App_TaskState;

typedef struct 
{
   
    IMUMSPM0_Device    * IMUMSPM0;
    LED_Device         * LED_BLUE;
    LED_Device         * LED_RED;
    LED_Device         * LED_GREEN;
    LED_Device         * LED_ARM;
    MAIXCAM_Device     * MAIXCAM;
    StepMotor_Master   * StepMotor;
    StepMotorArm_Master* StepMotorArm;
    Buzzer_Device      * Buzzer;
    Servo_Device       * Servo_Plate;
    Servo_Device       * Servo_Yaw;
    Servo_Device       * Servo_Claw;
	BT_Device          * BT;
    KeyState  KeyNum;
    App_Mode  App_Mode;
    App_State App_State;
	uint8_t*  BT_CMD_Buf;
    
    bool App_PIDENABLE;
}APP_Handle;


typedef struct
{
    APP_Handle* App;                         //软件层系统句柄指针
    const uint16_t Task_Period_ms;           //任务执行周期，单位ms
    uint16_t       Task_Current_ms;          //任务当前计数器，单位ms
    void (*Task_Func)(APP_Handle* App);      //任务功能执行函数指针
    App_TaskState Task_State;                //任务状态，READY表示任务就绪等待，SLEEP表示任务休眠，STOP表示任务终止不再运行

}APP_TASK_TCB;

extern APP_Handle App;

//任务调度器相关函数
void App_Task_TimeTick(void);
void App_Task_Scheduler(void);

//App任务层逻辑功能函数
void App_SystemInitTask(APP_Handle* App);
//void App_BuzzerTask(APP_Handle* App);
void App_KeyTask(APP_Handle* App);
//void App_OLEDTask(APP_Handle*App);
//void App_SensorTask(APP_Handle* App);
void App_IMUMSPM0Task(APP_Handle* App);
//void App_VideoTask(APP_Handle* App);
void App_ErrorTask(APP_Handle* App);
//void App_TM1637Task(APP_Handle* App);
void App_DEBUGTask(APP_Handle* App);
void App_BlueToothTask(APP_Handle* App);
void App_UART_DataDeCodeTask(APP_Handle* App);

//测试任务
void App_LED1TestTask(APP_Handle* App);
void App_LED2TestTask(APP_Handle* App);
void App_LED3TestTask(APP_Handle* App);
void App_LEDArmTestTask(APP_Handle* App);
void App_CarTurnTestTask(APP_Handle* App);
void App_ServoTestTask(APP_Handle* App);
void App_ServoYawTestTask(APP_Handle* App);
void App_StepMotorArmTestTask(APP_Handle* App);

//App任务层PID相关函数
void App_PositionPID_Callback(APP_Handle* App, uint16_t time_ms);
void App_PositionPID_Set(APP_Handle* App, float TargetPosition);
void App_AnglePID_Turn(APP_Handle* App, float Angle);

#endif


