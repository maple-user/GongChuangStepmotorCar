#include "App.h"
#include "App_questions.h"

//软件层系统句柄
APP_Handle App = {0};           

/*------------- 循环任务表 -------------*/

//所有需要定时执行的任务都需要在此注册，注册后系统会
//根据任务表中设置的周期来定时执行对应的任务
//注意
APP_TASK_TCB App_LoopTaskTable[] = 
{
    {&App, 1, 1, App_KeyTask, Task_READY},
    {&App, 60, 60,App_IMUMSPM0Task, Task_READY},
   // {&App, 500, 500, App_LEDArmTestTask, Task_READY},
   // {&App, 4000, 4000, App_CarTurnTestTask, Task_READY},
   // {&App,  5,  5, App_UART_DataDeCodeTask,Task_READY},
    // {&App,  500,500,App_LED1TestTask,Task_READY},
    // {&App,  1000,1000,App_LED2TestTask,Task_READY},
    // {&App,  200,200,App_LED3TestTask,Task_READY},
    //{&App,  10,10,App_IMUMSPM0Task,Task_READY},
    {&App,  500,500,App_ErrorTask,Task_READY}
};


//已经注册的任务数量
#define App_LoopTaskTableSize (sizeof(App_LoopTaskTable)/sizeof(APP_TASK_TCB))
/*-------------    END    -------------*/


/*-------------  App任务层调度逻辑功能函数  -------------*/

/**
  * @brief  App 任务时间片调度管理器，必须放在SysTick中断中执行，负责定时唤醒任务
  * @param  无
  * @retval 无
  */
void App_Task_TimeTick(void)
{
    for(uint8_t i = 0; i < App_LoopTaskTableSize; i++)
    {
        //如果任务主动选择停止则从执行队列里面删除
       if(App_LoopTaskTable[i].Task_State == Task_STOP)
       {
            continue;
       }
       if(App_LoopTaskTable[i].Task_Current_ms > 0U)
       {
            App_LoopTaskTable[i].Task_Current_ms--;
       }
       if(App_LoopTaskTable[i].Task_Current_ms == 0U && App_LoopTaskTable[i].Task_State == Task_SLEEP)
       {
            App_LoopTaskTable[i].Task_State = Task_READY;
        
       }
    }
}

/**
  * @brief  App 任务调度器，必须放在主循环中执行，负责执行任务
  * @param  无
  * @retval 无
  */
void App_Task_Scheduler(void)
{
    for(uint8_t i = 0; i < App_LoopTaskTableSize; i++)
    {
        if(App_LoopTaskTable[i].Task_State == Task_READY)
        {
            //执行任务函数
            App_LoopTaskTable[i].Task_Func(App_LoopTaskTable[i].App);
            //重置任务计数器和状态
            App_LoopTaskTable[i].Task_Current_ms = App_LoopTaskTable[i].Task_Period_ms;
            App_LoopTaskTable[i].Task_State = Task_SLEEP;
        }
    }
}


/*--------------------   END  --------------------*/



/*-------------  App任务层逻辑功能函数  -------------*/

/**
  * @brief  App系统初始化任务
  * @param  App APP句柄指针
  * @retval 无
  */
void App_SystemInitTask(APP_Handle* App)
{
    if(App == NULL)
    {
        App ->App_State = App_Error;
        return;
    }

   
    App->LED_BLUE  = LED_GetDevice(LED_Blue);
    App->LED_GREEN = LED_GetDevice(LED_Green);
    App->LED_RED   = LED_GetDevice(LED_Red);
    App->LED_ARM   = LED_GetDevice(LED_StepMotorArm);
	App->BT        = BT_GetDevice(BT_HC_40D_Name);
    App->IMUMSPM0  = IMUMSPM0_GetDevice();
    App->StepMotor = StepMotor_GetMaster();
    App->StepMotorArm = StepMotorArm_GetMaster();
    App->MAIXCAM   = MAIXCAM_GetDevice();
    App->Buzzer    = Buzzer_GetDevice(Buzzer_1);
    App->Servo_Plate = Servo_GetDevice(Servo_Plate_ID);
    App->Servo_Yaw   = Servo_GetDevice(Servo_Yaw_ID);
    App->Servo_Claw  = Servo_GetDevice(Servo_Claw_ID);
    //注册表自检
    if(App->LED_BLUE == NULL||App->LED_GREEN == NULL||App->LED_RED == NULL||App->BT == NULL||App->IMUMSPM0 == NULL||App->StepMotor == NULL||App->StepMotorArm == NULL||App->MAIXCAM == NULL||App->Buzzer == NULL||App->Servo_Plate == NULL||App->Servo_Yaw == NULL||App->Servo_Claw == NULL) 
    {
        App ->App_State = App_Error;
        return;
    }
  

	App->BT_CMD_Buf = BT_Command_Buf;
    App->KeyNum     = KEY_NONE; 
    //默认上电先不使用PID
    App->App_PIDENABLE = false;
    
    //上电默认做第二题
    App->App_Mode   = App_Mode_Qst_2;

    App->App_State = App_Init_Wait;
	
//    LED_SetState(App->LED_BLUE,LED_ON);
//	LED_SetState(App->LED_RED,LED_ON);
//	LED_SetState(App->LED_GREEN,LED_ON);
}


/**
  * @brief  按键处理任务
  * @param  App APP句柄指针
  * @retval 无
  */
void App_KeyTask(APP_Handle* App)
{
   if(App == NULL)
   {
       return;
   }

    Key_Tick();
   
    App->KeyNum = Key_GetNum();

   if(App->KeyNum == KEY_START)
   {
        if(App->App_State == App_Init_OK || App->App_State == App_Standby)
       {
           App->App_State = App_Running;
           
           LED_SetState(App->LED_BLUE,LED_OFF);
		   LED_SetState(App->LED_RED,LED_OFF);
           LED_SetState(App->LED_GREEN,LED_ON);

           StepMotor_Move_cm(App->StepMotor, 120.0f, 0.0f);
           LED_SetState(App->LED_BLUE,LED_ON);
           App->App_State = App_Standby;
       }
    //    else if(App->App_State == App_Running)
    //    {
           
          
    //        DWT_Delay(App_SysLatency_s);

    //        PID_AvrControl.Target = 0.0f;
    //        DWT_Delay_ms(500);
    //        App->App_State = App_Standby;
    //        Motor_Stop(App->L_Motor);
    //        Motor_Stop(App->R_Motor);
           
	// 		LED_SetState(App->LED_RED,LED_ON);
    //        LED_SetState(App->LED_BLUE,LED_OFF);
    //    }
   }

   if(App->KeyNum == KEY_CONTROL)
   {
       if(App->App_State == App_Init_OK || App->App_State == App_Standby)
       {
           App->App_State = App_Running;
           
           LED_SetState(App->LED_BLUE,LED_OFF);
		   LED_SetState(App->LED_RED,LED_OFF);
           LED_SetState(App->LED_GREEN,LED_ON);

           StepMotor_Move_cm(App->StepMotor, 120.0f, -90.0f);
           LED_SetState(App->LED_BLUE,LED_ON);
           App->App_State = App_Standby;
       }
   }
       /* Laps_Num += direction; 

        if(Laps_Num >= 4)          
        {
            Laps_Num  =  4;
            direction = -1;
        }
        else if(Laps_Num <= 2)
        {
            Laps_Num  =  2;
            direction = -1;
        }
         */
    //}

    static uint8_t direction = 1;
  
    if(App->KeyNum == KEY_MODE)
    {

       switch(direction)
       {
           case 0:
           direction   = 1;
           App->App_PIDENABLE = false;
           PID_Stop();
          
           break;
			case 1:
			App->App_PIDENABLE = true;
            PID_Start();
            PID_AglControl.Target  = (PID_AglControl.Target + 45.0f);

            if(PID_AglControl.Target >= 180.0f)
            {
                PID_AglControl.Target = 0.0f;
                direction  = 0;
            }
            
			break;
       }
       

     // OLED_ShowFloatNum(0, 0, PID_AglControl.Target, 3,3 OLED_8X16);
      OLED_ShowFloatNum(0,0,PID_AglControl.Target,3,3,OLED_8X16);
      OLED_Update();
    }

}




/**
  * @brief  视觉处理任务
  * @param  App APP句柄指针
  * @retval 无
  */
 /*
void App_VideoTask(APP_Handle* App)
{
    if(App->Video == NULL)
    {
        App ->App_State = App_Error;
        return;
    }

    //在此处添加视频处理代码，获取视频数据并进行处理以提取小车与赛道的相对位置等信息以辅助寻线控制算法的运行

}
*/


/**
  * @brief  App错误处理任务
  * @param  App APP句柄指针
  * @retval 无
  */
void App_ErrorTask(APP_Handle* App)
{
    if( App ->App_State == App_Error)
    {
        HAL_TIM_Base_Stop_IT(&htim1);

//        Motor_Stop(App->L_Motor);
//        Motor_Stop(App->R_Motor);

//        OLED_ShowString(34, 32, "       ",OLED_8X16);
//        OLED_ShowString(34, 32, "Error",OLED_8X16);
//		OLED_Update();

    while(1)
    {
        // LED_On();
        // HAL_Delay(50);
        // LED_Off();
        // HAL_Delay(50);
        LED_Blink(App->LED_RED,500.0f);
       // HAL_IWDG_Refresh(&hiwdg);
    } 
    }
   
}


///**
//  * @brief  App数码管处理任务
//  * @param  App APP句柄指针
//  * @retval 无
//  */
//void App_TM1637Task(APP_Handle* App)
//{
//    static uint32_t last_update_time = 0;
//    static float  nowtime = 0.0f;
//    uint32_t current_time = HAL_GetTick();
//    
//    if(App == NULL)
//    {
//        return;
//    }

//	uint8_t flag = TM1637_GetState();
//	if(flag == TM1637_ZeroEnable)
//	{
//		nowtime = 0.0f;
//	}
//	
//	if(App ->App_State == App_Running)
//	{
//		if(current_time - last_update_time >= 200) // 每200ms更新一次
//		{
//			last_update_time = current_time;

//			nowtime += 0.21f; // 转换为秒，保留一位小数
//			// 显示速度到TM1637
//			TM1637_DisplayFloat(nowtime);
//		}
//	}
//	else
//	{
//		//防止卡死造成的时间误差，只记录有效运行时间
//		last_update_time = current_time;
//	}
//	
//   
//   
//}


/**
  * @brief  App调试处理任务
  * @param  App APP句柄指针
  * @retval 无
  */
void App_DEBUGTask(APP_Handle* App)
{
	printf("[plot,%d,%d]",(int32_t)PID_PosControl.Target,(int32_t)PID_PosControl.Actualv);
}

/**
  * @brief  App蓝牙掉线检测任务
  * @param  App APP句柄指针
  * @retval 无
  */
void App_BlueToothTask(APP_Handle* App)
{
    if(App->BT == NULL|| App->LED_BLUE == NULL || App->BT->BT_State == BT_ERROR)
    {
        App ->App_State = App_Error;
        return;
    }

    BT_RefreshConnectState(App->BT);

    switch (App->BT->BT_State)
    {
    case BT_Connect_Wait: 
        App->App_State = App_Standby;
//        Motor_Stop(App->R_Motor);
//        Motor_Stop(App->L_Motor);
       
         while (App->BT->BT_State == BT_Connect_Wait)
         {
            
            BT_RefreshConnectState(App->BT);
            LED_Blink(App->LED_BLUE,400);
           // HAL_IWDG_Refresh(&hiwdg);
         }
         App->App_State = App_Standby;
        // PID_AvrControl.Target = Motor_AveSpeed;
//         Motor_Start(App->R_Motor);
//         Motor_Start(App->L_Motor);
       
		 
         LED_SetState(App->LED_BLUE,LED_ON);
        
         break;
    case BT_Connect_OK:
         break;
    default:
         App->App_State = App_Error;
         break;
    }
}

/**
  * @brief  App串口数据包解析任务
  * @note   解析[Target,Name,Action/Value]格式的指令数据包，
  *         Target成员变量表示指令类型，Name成员变量表示指令名称
  *         Action/Value成员变量表示指令动作或数值
  * @param  App APP句柄指针
  * @retval 无
  */
void App_UART_DataDeCodeTask(APP_Handle* App)
{
    if(App == NULL || App->BT_CMD_Buf == NULL || App->BT->BT_Flag != BT_RX_OK)
    {
        return;
    }
    
    char* Target = strtok((char*)App->BT_CMD_Buf, ",");
    char* Name   = NULL;
    char* Action = NULL;
    char* Value  = NULL;

    if(strcmp(Target, "key") == 0)
    {
        Name   = strtok(NULL, ",");
        Action = strtok(NULL, ",");

        if(strcmp(Name, "Start") == 0 && strcmp(Action, "down") == 0)
        {
            App->KeyNum = KEY_START;
            App->BT->BT_Flag = BT_RX_WAIT;
            return;
        }
//        else if(strcmp(Name, "2") == 0 && strcmp(Action, "down") == 0)
//        {
//            App->KeyNum = KEY_CONTROL;
//            App->BT->BT_Flag = BT_RX_WAIT;
//            return;
//        }
//        else if(strcmp(Name, "Mode") == 0 && strcmp(Action, "down") == 0)
//        {
//            App->KeyNum = KEY_MODE;
//            App->BT->BT_Flag = BT_RX_WAIT;
//            return;
//        }
    }
   else if(strcmp(Target, "slider") == 0)
    {
        Name   = strtok(NULL, ",");
        Value  = strtok(NULL, ",");

        if(strcmp(Name, "kp") == 0)
        {
            //PID_PosControl.Kp = atof(Value);
            PID_PosControl.Kp = atof(Value);
            App->BT->BT_Flag = BT_RX_WAIT;
            return;
        }
        else if(strcmp(Name, "ki") == 0)
        {
            PID_PosControl.Ki = atof(Value);
            //PID_DifControl.Ki = atof(Value);
            App->BT->BT_Flag = BT_RX_WAIT;
            return;
        }
        else if(strcmp(Name, "kd") == 0)
        {
            PID_PosControl.Kd = atof(Value);
            //PID_DifControl.Kd = atof(Value);
            App->BT->BT_Flag = BT_RX_WAIT;
            return;
        }
        else if(strcmp(Name, "target") == 0)
        {
          //  PID_AvrControl.Target = atof(Value);
            App->BT->BT_Flag = BT_RX_WAIT;
            return;
        }
		
    }
}


/**
  * @brief  App单轴陀螺仪角度信息显示任务
  * @note   目前仅用于调试
  * @param  App APP句柄指针
  * @retval 无
  */
void App_IMUMSPM0Task(APP_Handle* App)
{
    if(App->IMUMSPM0 == NULL)
    {
        App->App_State = App_Error;
    }
    IMUMSPM0_GetData(App->IMUMSPM0);
    OLED_ShowFloatNum(45,16,App->IMUMSPM0->Data.Angle,3,3,OLED_8X16);
    OLED_ShowFloatNum(45,32,App->IMUMSPM0->Data.Speed,3,3,OLED_8X16);
    OLED_Update();

}



/*-------------------------------------------------------------------------App任务测试区--------------------------------------------------------------------*/

void App_LED1TestTask(APP_Handle* App)
{
    if(App->LED_BLUE == NULL || App->LED_RED == NULL || App->LED_GREEN == NULL)
    {
        App->App_State = App_Error;
    }
    BSP_LED_Toggle(App->LED_BLUE);
}

void App_LED2TestTask(APP_Handle* App)
{
    if(App->LED_BLUE == NULL || App->LED_RED == NULL || App->LED_GREEN == NULL)
    {
        App->App_State = App_Error;
    }
    BSP_LED_Toggle(App->LED_RED);
}

void App_LED3TestTask(APP_Handle* App)
{
    if(App->LED_BLUE == NULL || App->LED_RED == NULL || App->LED_GREEN == NULL)
    {
        App->App_State = App_Error;
    }
    BSP_LED_Toggle(App->LED_GREEN);
}

void App_LEDArmTestTask(APP_Handle* App)
{
    if(App->LED_BLUE == NULL || App->LED_RED == NULL || App->LED_GREEN == NULL)
    {
        App->App_State = App_Error;
    }
    BSP_LED_Toggle(App->LED_ARM);
}


void App_CarTurnTestTask(APP_Handle* App)
{
    if(App->StepMotor == NULL)
    {
        App->App_State = App_Error;
    }

    static uint8_t direction = 1;
    static float angle = 0;
    if(direction == 1)
    {
        angle += 90.0f;
        if(angle >= 180.0f)
        {
            angle = 0;
            direction = 0;
        }
    }
    if(direction == 0)
    {
        angle -= 90.0f;
        if(angle <= -180.0f)
        {
            angle = 0;
            direction = 1;
        }
    }
    LED_SetState(App->LED_BLUE,LED_OFF);
    App_AnglePID_Turn(App, angle);
    LED_SetState(App->LED_BLUE,LED_ON);
   
    
}

void App_ServoTestTask(APP_Handle* App)
{
    if(App->Servo_Plate == NULL)
    {
        App->App_State = App_Error;
    }

    static uint8_t direction = 0;
    
    switch(direction)
    {
        case 0:
            Servo_SetAngle(App->Servo_Plate, Servo_Plate_Stuff1_Angle);
            break;
        case 1:
            Servo_SetAngle(App->Servo_Plate, Servo_Plate_Stuff2_Angle);
            break;
        case 2:
            Servo_SetAngle(App->Servo_Plate, Servo_Plate_Stuff3_Angle);
            break;
        default:
            Servo_SetAngle(App->Servo_Plate, 0.0f);
            break;
    }
    direction  = (direction + 1) % 3;
    
}

void App_ServoYawTestTask(APP_Handle* App)
{
    if(App->Servo_Plate == NULL)
    {
        App->App_State = App_Error;
    }

    Servo_SetAngle(App->Servo_Plate,31.5f);
    
}

void App_StepMotorArmTestTask(APP_Handle* App)
{
    if(App->StepMotorArm == NULL)
    {
        App->App_State = App_Error;
    }

    Servo_SetAngle(App->Servo_Plate,120.0f);
    StepMotorArm_Move_cm(App->StepMotorArm, 3.0f, 8.0f);
   // Servo_SetAngle(App->Servo_Plate,Servo_CLAW_HOME_ANGLE);
    DWT_Delay_ms(300);
    Servo_SetAngle(App->Servo_Plate,31.5f);
    
}
///**
//  * @brief  App蜂鸣器使能检测任务
//  * @param  App APP句柄指针
//  * @retval 无
//  */
//void App_BuzzerTask(APP_Handle* App)
//{
//     if(App == NULL)
//    {
//        return;
//    }
//    if(App->Beep_Flag == true)
//    {
//        Buzzer_On();
//        DWT_Delay_ms(500);
//        Buzzer_Off();
//        App->Beep_Flag = false;
//    }
//}


/*-------------------------------------------------------------------------PID控制算法位置环+角度环--------------------------------------------------------------------*/

#define PID_TIME_MS     20U     //PID控制算法执行周期，单位ms
#define PID_DEADBAND    0.08f   //PID位实际值死区，小于该值后将累计到位
#define PID_ARRIVE_NUM  10     //PID位实际值连续小于死区的次数，累计到位

#define PID_IMU_ZERO    0.001f  //陀螺仪零漂系数，每转1°就漂移这么多


/**
  * @brief  App车体角度PID控制
  * @note   只需要直接调用就行
  * @param  App   APP句柄指针
  * @param  angle 角度范围 -180~180deg
  * @retval 无
  */
void App_AnglePID_Turn(APP_Handle* App, float Angle)
{
    if(App == NULL)
    {
        return;
    }

    
    static float historical_angle = 0.0f;
    if(Angle < 0.0f)
    {
        historical_angle +=  (-Angle);
    }
    else
    {
        historical_angle += Angle;
    }
    
    //输入限幅，限制角度范围在[-180, 180]之间
    if(Angle < -180.0f)
    {
        Angle = -180.0f;
    }
    else if(Angle > 180.0f)
    {
        Angle = 180.0f;
    }

    //设置PID角度环目标值，考虑陀螺仪零漂的影响
    PID_AglControl.Target = Angle + historical_angle * PID_IMU_ZERO;

    //打开PID之前先把PID配置成只有角度环控制
    PID_AglControl.IS_ENABLE = true;
    PID_PosControl.IS_ENABLE = false;

    //清除角度到位标志
    PID_AglControl.IS_ARRIVE = false;

    //解锁App对PID的控制权，允许App在中断回调中执行PID计算任务
    App->App_PIDENABLE = true;

    //启动PID角度环计算任务
    PID_Start();
    while (PID_AglControl.IS_ARRIVE == false)
    {
        //等待PID角度环控制任务完成
        // 添加编译器屏障防止循环被优化掉
        __memory_changed();
    }
    PID_Stop();

    //关闭App对PID的控制权，禁止App在中断回调中执行PID计算任务
    App->App_PIDENABLE = false;
    //关闭PID角度环计算任务
    PID_AglControl.IS_ENABLE = false;
    
}

/**
  * @brief  App车体位置PID控制
  * @note   只需要直接调用就行
  * @param  App   APP句柄指针
  * @param  TargetPosition  目标位置
  * @retval 无
  */
void App_PositionPID_Set(APP_Handle* App, float TargetPosition)
{
    if(App == NULL)
    {
        return;
    }

    PID_PosControl.Target = TargetPosition;

    //打开PID之前先把PID配置成角度环位置环并行控制
    PID_AglControl.IS_ENABLE = true;
    PID_PosControl.IS_ENABLE = true;

    //清除角度到位标志
    PID_PosControl.IS_ARRIVE = false;

    //解锁App对PID的控制权，允许App在中断回调中执行PID计算任务
    App->App_PIDENABLE = true;

    //启动PID角度环计算任务
    PID_Start();
    while (PID_PosControl.IS_ARRIVE == false)
    {
        //等待PID角度环控制任务完成
        // 添加编译器屏障防止循环被优化掉
        __memory_changed();
    }
    PID_Stop();

    //关闭App对PID的控制权，禁止App在中断回调中执行PID计算任务
    App->App_PIDENABLE = false;
    //关闭PID计算任务
    PID_AglControl.IS_ENABLE = false;
    PID_PosControl.IS_ENABLE = false;
    
}


/**
  * @brief  App车体位置控制PID中断回调任务
  * @note   自身角度姿态和视觉位置环的并环控制
  * @param  App APP句柄指针
  * @param  time_ms PID的执行时间间隔，单位ms
  * @retval 无
  */
void App_PositionPID_Callback(APP_Handle* App, uint16_t time_ms)
{
    if(App == NULL)
    {
       return;
    }
    static uint16_t time = 0;
    static uint8_t  agl_arrive_num = 0;
    static uint8_t  pos_arrive_num = 0;
    float w_speed = 0.0f;
    float v_speed = 0.0f;
   
   //如果PID使能标志为true，则执行PID控制算法
   if(App->App_PIDENABLE == true)
   {
        //到时自动触发
        time++;
        if(time > time_ms)
        {
            time = 0;

            //触发后选择执行角度环和位置环的PID计算任务，角度环和位置环的PID计算任务是并行执行的
            if(PID_AglControl.IS_ENABLE == true)
            {
                IMUMSPM0_GetData(App->IMUMSPM0);

                PID_AglControl.Actualv = App->IMUMSPM0->Data.Angle;
                PID_AglControl.Actualw = App->IMUMSPM0->Data.Speed;

                PID_Angle_Calculate(&PID_AglControl);

                w_speed = PID_AglControl.Out;

                //判断角度环是否到位，连续5次小于死区则认为到位,如果中途有扰动则会继续跑PID并重置状态
                if(fabs(PID_AglControl.Error0) < PID_DEADBAND)
                {
                    agl_arrive_num++;

                    //连续5次小于死区则认为到位,复位计数器并置完成标志位
                    if(agl_arrive_num >= PID_ARRIVE_NUM)
                    {
                        agl_arrive_num = 0;
                        PID_AglControl.IS_ARRIVE = true;
                    }

                }
                else
                {
                    agl_arrive_num = 0;
                }
            }
            else
            {
                //如果角度环没有使能，则直接置角速度为0，避免速度影响车体位姿
                w_speed = 0.0f;
            }



            if(PID_PosControl.IS_ENABLE == true)
            {
                PID_PosControl.Actualv = App->StepMotor->ActualPosition;

                PID_Calculate(&PID_PosControl);

                v_speed = PID_PosControl.Out;

                //判断位置环是否到位，连续5次小于死区则认为到位,如果中途有扰动则会继续跑PID并重置状态
                if(fabs(PID_PosControl.Error0) < PID_DEADBAND)
                {
                    pos_arrive_num++;

                    //连续5次小于死区则认为到位,复位计数器并置完成标志位
                    if(pos_arrive_num >= PID_ARRIVE_NUM)
                    {
                        pos_arrive_num = 0;
                        PID_PosControl.IS_ARRIVE = true;
                    }

                }
                else
                {
                    pos_arrive_num = 0;
                }
            }
            else
            {
                //如果位置环没有使能，则直接置速度为0，避免速度影响车体位姿
                v_speed = 0.0f;
                App->StepMotor->ActualAngle = 0.0f;
            }

            StepMotor_SetSpeed_PID(App->StepMotor,v_speed,w_speed,App->StepMotor->ActualAngle);
            
           

        }
   }
  else
  {
    //增加鲁棒性，防止PID误触发
    time = 0;
  }
}
/*-------------------------------------------------------------------------机械臂动作--------------------------------------------------------------------*/
//夹爪关闭时间
#define APP_SERVO_CLAW_WAIT_TIME_MS  300U
#define APP_SERVO_YAW_WAIT_TIME_MS   600U
//机械臂XY轴最大位移距离
#define STEPMOTORARM_X_MAX_CM  20.0f
#define STEPMOTORARM_Y_MAX_CM  20.0f
//机械臂标准定义位置HOME

#define STEPMOTORARM_X_HOME_CM       3.0f
#define STEPMOTORARM_Y_HOME_CM       15.0f
#define STEPMOTORARM_YAW_HOME_ANGLE  20.0f

//机械臂物料区XY轴放置物体到物料盘位置
#define STEPMOTORARM_X_PLATE_CM      6.0f
#define STEPMOTORARM_Y_PLATE_CM      10.0f
#define STEPMOTORARM_PLATE_YAWANGLE  50.0f

//机械臂物料区XY轴抓取物体位置
#define STEPMOTORARM_X_PICK_1_CM  6.0f  //如果使用视觉抓取方案就不用该值，该值只作为机械臂抓取物体的固定位置
#define STEPMOTORARM_Y_PICK_1_CM  5.0f

//物料颜色及对应逻辑
#define STUFF_RED          1
#define STUFF_BLUE         2
#define STUFF_GREEN        3

/**
  * @brief  App机械臂扫描二维码动作
  * @param  App APP句柄指针
  * @retval 无
  */
void App_Arm_QR(APP_Handle* App)
{
    if(App == NULL)
    {
        return;
    }

    //复位爪子爪子，准备抓取物体
    Servo_SetAngle(App->Servo_Claw,Servo_CLAW_OPEN_ANGLE);

    //把机械臂扭头复位
    Servo_SetAngle(App->Servo_Yaw,STEPMOTORARM_YAW_HOME_ANGLE);
    // DWT_Delay_ms(APP_SERVO_YAW_WAIT_TIME_MS);

    //复位机械臂
    StepMotorArm_Move_cm(App->StepMotorArm,STEPMOTORARM_X_HOME_CM,STEPMOTORARM_Y_HOME_CM);

}


/**
  * @brief  App机械臂物料盘抓取动作
  * @note   如果使用视觉抓取方案就不用该函数，该函数只作为机械臂抓取物体的固定位置
  * @param  App APP句柄指针
  * @param  StuffAngle 机械臂需要扭头的角度，单位deg
  * @param  StuffPosition 机械臂需要移动的X轴位置，单位cm
  * @param  StuffNum 物料颜色编号，1红色，2蓝色，3绿色
  * @retval 无
  */
void App_Arm_PickAction_1(APP_Handle* App, float StuffAngle, float StuffPosition, uint8_t StuffNum)
{
    if(App == NULL)
    {
        return;
    }

    //输入限幅防止电机堵转
    if(StuffPosition > STEPMOTORARM_X_MAX_CM)
    {
        StuffPosition = STEPMOTORARM_X_MAX_CM;
    }

    //打开爪子，准备抓取物体
    Servo_SetAngle(App->Servo_Claw,Servo_CLAW_OPEN_ANGLE);
    
    //移动机械臂到抓取物体的位置,同时调整物料盘位置节约时间

    switch(StuffNum)
    {
        case STUFF_RED:
            Servo_SetAngle(App->Servo_Plate,Servo_Plate_Stuff1_Angle);
            break;
        case STUFF_BLUE:
            Servo_SetAngle(App->Servo_Plate,Servo_Plate_Stuff2_Angle);
            break;
        case STUFF_GREEN:
            Servo_SetAngle(App->Servo_Plate,Servo_Plate_Stuff3_Angle);
            break;
        default:
            Servo_SetAngle(App->Servo_Plate,Servo_Plate_Stuff1_Angle);
            break;
    }


    Servo_SetAngle(App->Servo_Yaw,StuffAngle);
    StepMotorArm_Move_cm(App->StepMotorArm,StuffPosition,STEPMOTORARM_Y_PICK_1_CM);

    //关闭爪子，抓取物体
    Servo_SetAngle(App->Servo_Claw,Servo_CLAW_HOME_ANGLE);
    DWT_Delay_ms(APP_SERVO_CLAW_WAIT_TIME_MS);

    //把机械臂扭头回去物料盘
    Servo_SetAngle(App->Servo_Yaw,STEPMOTORARM_PLATE_YAWANGLE);
    //DWT_Delay_ms(APP_SERVO_YAW_WAIT_TIME_MS);

    //将机械臂移动到物料盘上方，准备放置物体
    StepMotorArm_Move_cm(App->StepMotorArm,STEPMOTORARM_X_HOME_CM,STEPMOTORARM_Y_HOME_CM); 

    //把爪子下降到规定的物料盘上方
    StepMotorArm_Move_cm(App->StepMotorArm,STEPMOTORARM_X_PLATE_CM,STEPMOTORARM_Y_PLATE_CM); 

    //松爪放置物体
    Servo_SetAngle(App->Servo_Claw,Servo_CLAW_OPEN_ANGLE);
    DWT_Delay_ms(APP_SERVO_CLAW_WAIT_TIME_MS);

  
   
    //复位爪子爪子，准备抓取物体
    Servo_SetAngle(App->Servo_Claw,Servo_CLAW_OPEN_ANGLE);

    //把机械臂扭头复位
    Servo_SetAngle(App->Servo_Yaw,STEPMOTORARM_YAW_HOME_ANGLE);
   // DWT_Delay_ms(APP_SERVO_YAW_WAIT_TIME_MS);

    //复位机械臂
    StepMotorArm_Move_cm(App->StepMotorArm,STEPMOTORARM_X_HOME_CM,STEPMOTORARM_Y_HOME_CM);

}



/*-------------------------------------------------------------------------工创赛主逻辑--------------------------------------------------------------------*/








/** 
  * @brief  App主任务状态机任务
  * @param  App APP句柄指针
  * @note   会阻塞其他任务比如在跑位的时候，但是会在需要任务正常执行的时候非阻塞
  * @retval 无
  */
void App_MainTask(APP_Handle* App)
{

}
