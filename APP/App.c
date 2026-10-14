#include "App.h"


/**
  * @brief  主程序初始化任务
  * @param  无
  * @retval 无
  */
void App_Init (void)
{
    //模块层初始化
    DWT_Init(STM32F4xx_SystemFrequency_MHz); // 初始化DWT,传入CPU频率,单位MHz
    LED_Init();
    BT_Init();
    OLED_Init();
	LCD_Init();
    IMUMSPM0_Init();
    StepMotor_Init();
    StepMotorArm_Init();
    MAIXCAM_Init();
    Servo_Init();
    Buzzer_Init();
    //算法初始化

    PID_Init(&PID_PosControl,POS_KP_NUM,POS_KI_NUM,POS_KD_NUM,POS_KW_NUM,0.0f,500.0f);
    PID_Init(&PID_AglControl,AGL_KP_NUM,AGL_KI_NUM,AGL_KD_NUM,AGL_KW_NUM,0.0f,300.0f);

    //App显示UI初始化

    OLED_ShowString(0,16,"Agl:",OLED_8X16);
    OLED_ShowString(0,32,"Spd:",OLED_8X16);
    OLED_Update();

    //App层系统初始化
    App_SystemInitTask(&App);

    if(App.App_State == App_Error)
    {
        App_ErrorTask(&App);
    }
    
    if(App.App_PIDENABLE == true)
    {
        //启动PID任务
        PID_Start();
    }
    else
    {
        //关闭PID任务
        PID_Stop();
    }
    
	LCD_ShowString(30,0,"Hello",BLACK,WHITE,16,0);
	LCD_ShowString(30,40,"World",RED,WHITE,16,0);
	LCD_ShowIntNum(128,40,LCD_H,3,RED,WHITE,16);
    LCD_ShowString(0,200,"Hello",BLACK,WHITE,48,0);
	//重启串口空闲中断触发单次DMA转换
//    App.BT->Table.BT_Recieve_Idle(App.BT->BT_UART.UART, App.BT->BT_Base.RxBuf, App.BT->BT_Base.RxBuf_Size);
// 	//关闭DMA传输半完成中断以防打断数据解析
//    __HAL_DMA_DISABLE_IT(DMA_UART_RX_HANDLE,DMA_IT_HT);
//     //左右电机共用一个定时中断
   // Motor_StartPID(App.L_Motor);
//	LED_SetState(App.LED_RED,LED_ON);
//	HAL_Delay(5000);
//	LED_SetState(App.LED_RED,LED_OFF);
//	LED_SetState(App.LED_BLUE,LED_OFF);
//    StepMotor_Move_cm(App.StepMotor,120,45.0f);
//    LED_SetState(App.LED_BLUE,LED_ON);
   App.App_State = App_Init_OK;
}


/**
  * @brief  主程序循环任务
  * @param  无
  * @retval 无
  */
void App_Loop (void)
{
   // App_ErrorTask(&App);
   // App_KeyTask(&App);
   // App_OLEDTask(&App);
    //App_SensorTask(&App);
    //App_VideoTask(&App);
  // App_Questions_Scheduler(&App);
  App_Task_Scheduler();
// App_CarTurnTestTask(&App);
 
}

/**
  * @brief  主程序刷新任务
  * @param  无
  * @retval 无
  */
void App_Refresh (void)
{
   // HAL_IWDG_Refresh(&hiwdg);
}


/**
  * @brief  TIM1更新中断回调函数
  * @note   1ms触发一次
  * @note   每20ms 唤醒PID速度环控制电机任务 
  * @note   每40ms 唤醒PID位置环控制转向任务    
  * @retval 无
  */
 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef*htim)
{
   
	if(htim == PID_TIM_HANDLE)
  {

  
    //按键扫描函数，检测按键状态并更新App.KeyNum
	//Key_Tick();
	
    //根据App.App_State的状态来决定是否执行PID控制任务，只有在App处于运行状态时才执行PID控制任务
    //此处作用是为了确保在App处于非Running状态时，PID控制任务不会被执行，从而避免不必要的计算和资源消耗

	  if(App.App_PIDENABLE == true)
    {
		  App_PositionPID_Callback(&App, PID_TIME_MS);
    }
  }
			
}

/**
  * @brief  UART空闲中断回调函数
  * @note   使用DMA单次转换模式，每次接收后都需要更新计数器并重启DMA 
  * @param  huart UART句柄指针
  * @param  Size  接收数据长度   
  * @retval 无
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart == BT_UART_HANDLE)
    {
       BT_GetDataCallBack(App.BT, App.BT_CMD_Buf, Size);
    }
    if(huart == MAIXCAM_UART_HANDLE)
    {
       MAIXCAM_GetDataCallBack(App.MAIXCAM, Size);
    }
}

/**
  * @brief  USART3接收中断回调函数
  * @note  使用状态机拆包，调用IMUMSPM0回调函数解析 
  * @param  huart UART句柄指针
  * @param  Size  接收数据长度   
  * @retval 无
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == IMUMSPM0_UART_HANDLE)
    {
        IMUMSPM0_GetDataCallback(App.IMUMSPM0);
    }

}


