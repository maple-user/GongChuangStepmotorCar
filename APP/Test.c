#include "Test.h"


#define App_TIM_Debug

//static uint8_t Data_Pool   [8] = {0,0,0,0,0,0,0,0};

//static float yaw = 0.0f;
//static int16_t L_Actual_Speed = 0;
//static int16_t R_Actual_Speed = 0;
static int16_t Target_Speed = 0;
//static Sensor_Base sensor= {0};

static PID_TypeDef PID_R = {0};
static PID_TypeDef PID_L = {0};
    

// static void Sensor_DataTransmit (Sensor_Base* Sensor)
// {
//     Data_Pool[0] = Sensor ->L4;
//     Data_Pool[1] = Sensor ->L3;
//     Data_Pool[2] = Sensor ->L2;
//     Data_Pool[3] = Sensor ->L1;
//     Data_Pool[4] = Sensor ->R1;
//     Data_Pool[5] = Sensor ->R2;
//     Data_Pool[6] = Sensor ->R3;
//     Data_Pool[7] = Sensor ->R4;
   

// }

HAL_StatusTypeDef Test_Init(Test_Handle* Test)
{
    if(Test == NULL)
    {
        return HAL_ERROR;
    }
    DWT_Init(STM32F4xx_SystemFrequency_MHz); // 初始化DWT,传入CPU频率,单位MHz
    Motor_Init();

    Test->L_Motor = Motor_GetDevice(L_Motor_Name);
    Test->R_Motor = Motor_GetDevice(R_Motor_Name);
    Test->Sensor  = NULL;
    //Test->Video = Video_GetDevice();
    Test->PID_L   = &PID_L;
    Test->PID_R   = &PID_R;
    Test->KeyNum  = KEY_NONE;
   // sensor.Sensor_Event = SENSOR_NORMAL;

    OLED_Init();
   // MPU6050_Init();
//    OLED_ShowString(1,1,"Hello");
//    OLED_ShowString(2,1,"Actual:");
//    OLED_ShowString(3,1,"Target:"); 


    PID_Init(Test->PID_L,AVE_KP_NUM,AVE_KI_NUM,AVE_KD_NUM,AVE_KP_NUM,0.0f);
    PID_Init(Test->PID_R,AVE_KP_NUM,AVE_KI_NUM,AVE_KD_NUM,AVE_KP_NUM,0.0f);

    Motor_Start(Test->L_Motor);
    Motor_Start(Test->R_Motor);
  
    //Buzzer_Init();
    return HAL_OK;

}

void MPU6050_Test(void)
{
   // yaw = MPU6050_GetAngleSpeed();
    //OLED_ShowSignedNum(2,1,(int16_t)yaw,5); 
}




HAL_StatusTypeDef Sensor_Test(Test_Handle* Test)
{
    if(Test == NULL || Test->Sensor == NULL)
    {
        return HAL_ERROR;
    }

 //   static uint32_t direction = 1;
   // float error = 0.0f;

    Test->KeyNum = Key_GetNum();
    
    if(Test->KeyNum == KEY_CONTROL)
    {
//         if((direction & (0x00000001U)) == 1U)
//            {
//                Sensor_CalibrateStart();
//            }
//            else
//            {
//                Sensor_CalibrateStop();
//            }
//            direction++;
    }
    
    BSP_Sensor_GetData(Test->Sensor);
   // Sensor_DataTransmit(Test->Sensor);
   // error = Sensor_GetError(Test->Sensor);

//    OLED_ShowNum(2,1,direction,3);
//    OLED_ShowNum(2,6,(uint8_t)HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15),1); 
//    OLED_ShowSignedNum(2,10,(int16_t)error,3);
//    OLED_ShowNum(4,1,Data_Pool[0],1);
//    OLED_ShowNum(4,2,Data_Pool[1],1);
//    OLED_ShowNum(4,3,Data_Pool[2],1);
//    OLED_ShowNum(4,4,Data_Pool[3],1);
//    OLED_ShowNum(4,5,Data_Pool[4],1);
//    OLED_ShowNum(4,6,Data_Pool[5],1);
//    OLED_ShowNum(4,7,Data_Pool[6],1);
//    OLED_ShowNum(4,8,Data_Pool[7],1);
//  

    return HAL_OK;
}


HAL_StatusTypeDef Motor_Test(Test_Handle* Test)
{
    if(Test == NULL || Test->L_Motor == NULL || Test->R_Motor == NULL)
    {
        return HAL_ERROR;
    }

    static uint8_t direction1 = 1;
   // L_Actual_Speed = Motor_GetSpeed(Test->L_Motor);
    //R_Actual_Speed = Motor_GetSpeed(Test->R_Motor);

    Test->KeyNum = Key_GetNum();
     if(Test->KeyNum == KEY_CONTROL)
    {
        if(direction1 == 1)
        {
           Target_Speed += 10;

           if(Target_Speed > 100)
           {
              Target_Speed = 0;
              direction1 = 0;
           }
       }
        else
        {
            Target_Speed -= 10;
    
           if(Target_Speed < -100)
           {
                Target_Speed = 0;
                 direction1 = 1;
            }
        }
    }
    Test->PID_L->Target = Target_Speed;
    Test->PID_R->Target = Target_Speed;

   // Motor_SetSpeed(Test->L_Motor, Target_Speed);
 //   Motor_SetSpeed(Test->R_Motor, Target_Speed);
//    OLED_ShowSignedNum(2,8,(int16_t)(Test->PID_L->Actualv),3);
//    OLED_ShowSignedNum(2,12,(int16_t)(Test->PID_R->Actualv),3);
//    OLED_ShowSignedNum(3,8,(int16_t)(Test->PID_L->Target),3);
//    OLED_ShowSignedNum(3,12,(int16_t)(Test->PID_R->Target),3);
    return HAL_OK; 
}

void Buzzer_Test(void)
{
    HAL_GPIO_TogglePin(Buzzer_GPIO_Port,Buzzer_Pin);
    HAL_Delay(2);
}

void PID_Test(Test_Handle* Test)
{
    if(Test == NULL || Test->PID_L == NULL || Test->PID_R==NULL)
    {
        return;
    }
    Test->PID_L->Actualv = Motor_GetSpeed(Test->L_Motor);
    Test->PID_R->Actualv = Motor_GetSpeed(Test->R_Motor);

    PID_Calculate(Test->PID_L);
    PID_Calculate(Test->PID_R);

    Motor_SetSpeed(Test->L_Motor,(int16_t) (Test->PID_L->Out));
    Motor_SetSpeed(Test->R_Motor,(int16_t) (Test->PID_R->Out));
}

