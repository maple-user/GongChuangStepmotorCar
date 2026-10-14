#include "Mod_StepMotor.h"
#include "BSP_DWT.h"
#include "usart.h"
#include <math.h>

//总线电机使用的串口相关句柄
#define STEPMOTOR_UART_HANDLE            &huart1
#define STEPMOTOR_DMA_UART_RX_HANDLE     &hdma_usart1_rx
#define STEPMOTOR_DMA_UART_TX_HANDLE     &hdma_usart1_tx

//总线电机注册表ID定义
#define LF_StepMotor_ID      0U      //左前电机ID
#define RF_StepMotor_ID      1U      //右前电机ID
#define LR_StepMotor_ID      2U      //左后电机ID
#define RR_StepMotor_ID      3U      //右后电机ID
#define StepMotor_NUM        4U      //总线电机数量

//总线电机注册表地址定义
#define LF_StepMotor_ADDR      0x01  //左前电机地址
#define RF_StepMotor_ADDR      0x02  //右前电机地址
#define LR_StepMotor_ADDR      0x03  //左后电机地址
#define RR_StepMotor_ADDR      0x04  //右后电机地址

//总线电机粗跑参数定义（T型曲线厘米级精度）
#define ROUGH_SPD_RPM           210.0f     // 峰值转速 (RPM)
#define ROUGH_ACC_RPMPS         300.0f     // 加速度   (RPM/s)
#define ROUGH_DEC_RPMPS         145.0f     // 减速度   (RPM/s)

//总线电机精调参数定义（T型曲线毫米级精度）
#define FINE_SPD_RPM            300.0f    // 峰值转速 (RPM)
#define FINE_ACC_RPMPS          600.0f    // 加速度   (RPM/s)
#define FINE_DEC_RPMPS          700.0f    // 减速度   (RPM/s)

//总线电机精调参数定义（T型曲线毫米级精度）
#define FINE_PID_SPD_RPM        300.0f      // 峰值转速 (RPM)
#define FINE_PID_ACC_RPMPS      200.0f      // 加速度   (RPM/s)

//总线电机姿态校正（IMU原地旋转）参数
#define YAW_PID_ACC_RPMPS       450.0f   // 转速加速度 (RPM/s)
#define YAW_PID_MAX_OMEGA       200.0f   // 最大角速度 (°/s)
#define YAW_PID_DEADBAND        1.0f     // 角度死区 (°)

//状态帧位掩码定义目前只使用了电机到位掩码
#define STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE   0x02U  //电机到位掩码

//总线控制器的最大接收数据长度
#define STEPMOTOR_RX_BUFFER_SIZE  10U

//一些固定参数，减少计算性能开销
#define PI                  3.1415927f //圆周率
#define WHEEL_D_CM          8.0f       //轮子直径 (cm)
#define L_mm                88.5f      //车体半宽轮间距 (mm)
#define W_mm                103.0f     //车体半长轮间距 (mm)

static const float Slip_Compensate = 1.0f;    //轮子打滑补偿系数默认轮子无打滑
static const float wSpeed_Compensate = 2.0f;  //麦轮角速度补偿系数默认无补偿
//总线电机接收缓冲区,用于视觉闭环
uint8_t StepMotor_RxBuffer[STEPMOTOR_RX_BUFFER_SIZE] = {0};

//总线电机注册表池
#define StepMotor_Master_MAXNUM 1
static StepMotor_Master Motor_Master_Pool[StepMotor_Master_MAXNUM] = {0};


//私有辅助函数，定义了阻塞式或DMA式的串口接收数据方式
static void StepMotor_StartReceive(StepMotor_Master* Master)
{
    if(Master == NULL)
    {
        return;
    }
    HAL_UART_Receive(STEPMOTOR_UART_HANDLE, Master->RxBuffer, sizeof(Master->RxBuffer), 5);
}

//暂时不用
// static void StepMotor_StartReceive_DMA(StepMotor_Master* Master, uint8_t* DataBuf)
// {
//     if(Master == NULL)
//     {
//         return;
//     }
//     HAL_UARTEx_ReceiveToIdle_DMA(STEPMOTOR_UART_HANDLE, DataBuf, sizeof(DataBuf));
//     __HAL_DMA_DISABLE_IT(STEPMOTOR_DMA_UART_RX_HANDLE,DMA_IT_HT);
// }




void StepMotor_Init(void)
{
    for(uint8_t i = 0; i < StepMotor_Master_MAXNUM; i++)
    {
        //初始化总线电机注册表
        Motor_Master_Pool[i].MotorNum = StepMotor_NUM;
        Motor_Master_Pool[i].StepMotor_Addr[LF_StepMotor_ID] = LF_StepMotor_ADDR;   //左前电机地址
        Motor_Master_Pool[i].StepMotor_Addr[RF_StepMotor_ID] = RF_StepMotor_ADDR;   //右前电机地址
        Motor_Master_Pool[i].StepMotor_Addr[LR_StepMotor_ID] = LR_StepMotor_ADDR;   //左后电机地址
        Motor_Master_Pool[i].StepMotor_Addr[RR_StepMotor_ID] = RR_StepMotor_ADDR;   //右后电机地址

        //默认使能所有电机
        Motor_Master_Pool[i].EnableFlag[LF_StepMotor_ID] = true;       //左前电机使能
        Motor_Master_Pool[i].EnableFlag[RF_StepMotor_ID] = true;       //右前电机使能
        Motor_Master_Pool[i].EnableFlag[LR_StepMotor_ID] = true;       //左后电机使能
        Motor_Master_Pool[i].EnableFlag[RR_StepMotor_ID] = true;       //右后电机使能

        //默认任务标志位为true，表示任务已完成
        Motor_Master_Pool[i].TaskFlag[LF_StepMotor_ID]   = true;       //左前电机任务标志位
        Motor_Master_Pool[i].TaskFlag[RF_StepMotor_ID]   = true;       //右前电机任务标志位
        Motor_Master_Pool[i].TaskFlag[LR_StepMotor_ID]   = true;       //左后电机任务标志位
        Motor_Master_Pool[i].TaskFlag[RR_StepMotor_ID]   = true;       //右后电机任务标志位

        //初始化所有目标参数
        Motor_Master_Pool[i].wSpeed     = 0.0f;      //旋转角速度
        Motor_Master_Pool[i].vSpeed     = 0.0f;      //移动速度
        Motor_Master_Pool[i].Angle      = 0.0f;      //行进角度
        Motor_Master_Pool[i].Position   = 0.0f;      //移动位移
        Motor_Master_Pool[i].SpeedAngle = 0.0f;      //速度方向角度（默认水平运动）
        Motor_Master_Pool[i].ActualPosition = 0.0f;   //管理器实际位置
        Motor_Master_Pool[i].ActualAngle    = 0.0f;   //管理器实际期望角度
    }

    //目前写死，开启首轮数据接收
   //StepMotor_StartReceive_DMA(&Motor_Master_Pool[0], StepMotor_RxBuffer);
}

StepMotor_Master* StepMotor_GetMaster(void)
{
    
    return &Motor_Master_Pool[0];
}



void StepMotor_Move_cm(StepMotor_Master* Master, float cm, float Angle)
{
    if(Master == NULL)
    {
        return;
    }

    //考虑轮子打滑补偿系数
    Master->Position = cm * Slip_Compensate; 
    //将角度转换为弧度制，便于计算
    Master->Angle = Angle * PI / 180.0f;       

    //将极坐标系下的移动参数转换为笛卡尔坐标系下的移动参数，便于计算每个电机的目标位置
    float X_cm = Master->Position * cosf(Master->Angle);
    float Y_cm = Master->Position * sinf(Master->Angle);

    //根据笛卡尔坐标系下的移动参数通过麦克纳姆轮运动学解算计算每个电机的目标位置，便于发送给电机控制器
    float LeftFront_cm  = Y_cm - X_cm;           //左前电机目标位置
    float RightFront_cm = Y_cm + X_cm;           //右前电机目标位置
    float LeftRear_cm   = -X_cm - Y_cm;          //左后电机目标位置
    float RightRear_cm  = X_cm - Y_cm;           //右后电机目标位置

    float LeftFront_Angle  = LeftFront_cm / (WHEEL_D_CM * PI) * 360.0f;      //左前电机目标角度
    float RightFront_Angle = RightFront_cm / (WHEEL_D_CM * PI) * 360.0f;     //右前电机目标角度
    float LeftRear_Angle   = LeftRear_cm / (WHEEL_D_CM * PI) * 360.0f;       //左后电机目标角度
    float RightRear_Angle  = RightRear_cm / (WHEEL_D_CM * PI) * 360.0f;      //右后电机目标角度

    //由于这里位置范围非常大，所以不做输出限幅


    //根据计算出的每个电机的目标角度发送控制命令给电机控制器，控制电机运动,多机同步
    if(LeftFront_Angle < 0)
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[LF_StepMotor_ID],1, ROUGH_ACC_RPMPS, ROUGH_DEC_RPMPS, ROUGH_SPD_RPM, -LeftFront_Angle, 0, 1);
      
    }
    else
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[LF_StepMotor_ID],0, ROUGH_ACC_RPMPS, ROUGH_DEC_RPMPS, ROUGH_SPD_RPM, LeftFront_Angle, 0, 1);
        
    }
   
    if(RightFront_Angle < 0)
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[RF_StepMotor_ID],1, ROUGH_ACC_RPMPS, ROUGH_DEC_RPMPS, ROUGH_SPD_RPM, -RightFront_Angle, 0, 1);
       
    }
    else
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[RF_StepMotor_ID],0, ROUGH_ACC_RPMPS, ROUGH_DEC_RPMPS, ROUGH_SPD_RPM, RightFront_Angle, 0, 1);
        
    }

    if(LeftRear_Angle < 0)
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[LR_StepMotor_ID],1, ROUGH_ACC_RPMPS, ROUGH_DEC_RPMPS, ROUGH_SPD_RPM, -LeftRear_Angle, 0, 1);
        
    }
    else
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[LR_StepMotor_ID],0, ROUGH_ACC_RPMPS, ROUGH_DEC_RPMPS, ROUGH_SPD_RPM, LeftRear_Angle, 0, 1);
        
    }

    if(RightRear_Angle < 0)
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[RR_StepMotor_ID],1, ROUGH_ACC_RPMPS, ROUGH_DEC_RPMPS, ROUGH_SPD_RPM, -RightRear_Angle, 0, 1);
        
    }
    else
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[RR_StepMotor_ID],0, ROUGH_ACC_RPMPS, ROUGH_DEC_RPMPS, ROUGH_SPD_RPM, RightRear_Angle, 0, 1);
      
    }

    //发送同步运动命令，所有电机同时开始运动
   // X_V2_Synchronous_motion(0);
    X_V2_Multi_Motor_Cmd(0);
    //将任务标志位设置为false，表示任务正在执行中
    Master->TaskFlag[LF_StepMotor_ID]   = false;       
    Master->TaskFlag[RF_StepMotor_ID]   = false;       
    Master->TaskFlag[LR_StepMotor_ID]   = false;       
    Master->TaskFlag[RR_StepMotor_ID]   = false;      


    //在等待过程中不断轮询电机状态帧，直到所有电机的任务标志位都为true，表示任务已完成
    for(;;)
    {
        if(Master->TaskFlag[LF_StepMotor_ID] != true)
        {
            X_V2_Read_Sys_Params(Master->StepMotor_Addr[LF_StepMotor_ID],S_FLAG);
            StepMotor_StartReceive(Master);
            //校验电机地址和尾部校验字节
            if((Master->RxBuffer[0] == Master->StepMotor_Addr[LF_StepMotor_ID])&&(Master->RxBuffer[1] == 0x3A))
            {
                if((Master->RxBuffer[2] & STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE) == STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE)
                {
                    Master->TaskFlag[LF_StepMotor_ID] = true;
                   
                }
              
            }
        }
        if(Master->TaskFlag[RF_StepMotor_ID] != true)
        {
            X_V2_Read_Sys_Params(Master->StepMotor_Addr[RF_StepMotor_ID],S_FLAG);
            StepMotor_StartReceive(Master);
            //校验电机地址和尾部校验字节
            if((Master->RxBuffer[0] == Master->StepMotor_Addr[RF_StepMotor_ID])&&(Master->RxBuffer[1] == 0x3A))
            {
                if((Master->RxBuffer[2] & STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE) == STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE)
                {
                    Master->TaskFlag[RF_StepMotor_ID] = true;
                   
                }
              
            }
        }
        if(Master->TaskFlag[LR_StepMotor_ID] != true)
        {
            X_V2_Read_Sys_Params(Master->StepMotor_Addr[LR_StepMotor_ID],S_FLAG);
            StepMotor_StartReceive(Master);
            //校验电机地址和尾部校验字节
            if((Master->RxBuffer[0] == Master->StepMotor_Addr[LR_StepMotor_ID])&&(Master->RxBuffer[1] == 0x3A))
            {
                if((Master->RxBuffer[2] & STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE) == STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE)
                {
                    Master->TaskFlag[LR_StepMotor_ID] = true;
                }
              
            }
        }
        if(Master->TaskFlag[RR_StepMotor_ID] != true)
        {
            X_V2_Read_Sys_Params(Master->StepMotor_Addr[RR_StepMotor_ID],S_FLAG);
            StepMotor_StartReceive(Master);
            //校验电机地址和尾部校验字节
            if((Master->RxBuffer[0] == Master->StepMotor_Addr[RR_StepMotor_ID])&&(Master->RxBuffer[1] == 0x3A))
            {
                if((Master->RxBuffer[2] & STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE) == STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE)
                {
                    Master->TaskFlag[RR_StepMotor_ID] = true;
                }
              
            }
        }
        //检查所有电机的任务标志位是否为true，表示任务已完成
        if(Master->TaskFlag[LF_StepMotor_ID] && Master->TaskFlag[RF_StepMotor_ID] && Master->TaskFlag[LR_StepMotor_ID] && Master->TaskFlag[RR_StepMotor_ID])
        {
            break;
        }
    }


}


void StepMotor_Move_mm(StepMotor_Master* Master, float mm, float Angle)
{
    if(Master == NULL)
    {
        return;
    }

    //考虑轮子打滑补偿系数
    Master->Position = mm * Slip_Compensate; 
    //将角度转换为弧度制，便于计算
    Master->Angle = Angle * PI / 180.0f;       

    //将极坐标系下的移动参数转换为笛卡尔坐标系下的移动参数，便于计算每个电机的目标位置
    float X_mm = Master->Position * cosf(Master->Angle);
    float Y_mm = Master->Position * sinf(Master->Angle);

    //根据笛卡尔坐标系下的移动参数通过麦克纳姆轮运动学解算计算每个电机的目标位置，便于发送给电机控制器
    float LeftFront_mm  = Y_mm - X_mm;           //左前电机目标位置
    float RightFront_mm = Y_mm + X_mm;           //右前电机目标位置
    float LeftRear_mm   = -X_mm - Y_mm;          //左后电机目标位置
    float RightRear_mm  = X_mm - Y_mm;           //右后电机目标位置

    float LeftFront_Angle  = LeftFront_mm / (WHEEL_D_CM * PI) * 36.0f;      //左前电机目标角度
    float RightFront_Angle = RightFront_mm / (WHEEL_D_CM * PI) * 36.0f;     //右前电机目标角度
    float LeftRear_Angle   = LeftRear_mm / (WHEEL_D_CM * PI) * 36.0f;       //左后电机目标角度
    float RightRear_Angle  = RightRear_mm / (WHEEL_D_CM * PI) * 36.0f;      //右后电机目标角度

    //由于这里位置范围非常大，所以不做输出限幅

    //根据计算出的每个电机的目标角度发送控制命令给电机控制器，控制电机运动,多机同步
    if(LeftFront_Angle < 0)
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[LF_StepMotor_ID],1, FINE_ACC_RPMPS, FINE_DEC_RPMPS, FINE_SPD_RPM, -LeftFront_Angle, 0, 1);
      
    }
    else
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[LF_StepMotor_ID],0, FINE_ACC_RPMPS, FINE_DEC_RPMPS, FINE_SPD_RPM, LeftFront_Angle, 0, 1);
        
    }
   
    if(RightFront_Angle < 0)
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[RF_StepMotor_ID],1, FINE_ACC_RPMPS, FINE_DEC_RPMPS, FINE_SPD_RPM, -RightFront_Angle, 0, 1);
      
    }
    else
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[RF_StepMotor_ID],0, FINE_ACC_RPMPS, FINE_DEC_RPMPS, FINE_SPD_RPM, RightFront_Angle, 0, 1);
  
    }

    if(LeftRear_Angle < 0)
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[LR_StepMotor_ID],1, FINE_ACC_RPMPS, FINE_DEC_RPMPS, FINE_SPD_RPM, -LeftRear_Angle, 0, 1);
        
    }
    else
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[LR_StepMotor_ID],0, FINE_ACC_RPMPS, FINE_DEC_RPMPS, FINE_SPD_RPM, LeftRear_Angle, 0, 1);
       
    }

    if(RightRear_Angle < 0)
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[RR_StepMotor_ID],1, FINE_ACC_RPMPS, FINE_DEC_RPMPS, FINE_SPD_RPM, -RightRear_Angle, 0, 1);
       
    }
    else
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[RR_StepMotor_ID],0, FINE_ACC_RPMPS, FINE_DEC_RPMPS, FINE_SPD_RPM, RightRear_Angle, 0, 1);
    
    }

    //发送同步运动命令，所有电机同时开始运动
   //X_V2_Synchronous_motion(0);
    X_V2_Multi_Motor_Cmd(0);
    
}


void StepMotor_SetwSpeed(StepMotor_Master* Master, float deg_s)
{
    if(Master == NULL)
    {
        return;
    }

    float rad_s = deg_s * PI / 180.0f;           //将角速度转换为弧度制
    Master->wSpeed = rad_s*6.0f/(PI*WHEEL_D_CM); //将角速度转换为电机转速(RPM)

    //这里默认逆时针旋转为正方向
    float LeftFront_rpm  = -Master->wSpeed*(L_mm+W_mm);     //左前电机目标位置
    float RightFront_rpm = Master-> wSpeed*(L_mm+W_mm);     //右前电机目标位置
    float LeftRear_rpm   = -Master->wSpeed*(L_mm+W_mm);     //左后电机目标位置
    float RightRear_rpm  = Master-> wSpeed*(L_mm+W_mm);     //右后电机目标位置

    if(LeftFront_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LF_StepMotor_ID],1, YAW_PID_ACC_RPMPS, -LeftFront_rpm, 1);
        
    }
    else
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LF_StepMotor_ID],0, YAW_PID_ACC_RPMPS, LeftFront_rpm, 1);
       
    }

    if(RightFront_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RF_StepMotor_ID],1, YAW_PID_ACC_RPMPS, -RightFront_rpm, 1);
       
    }
    else
    {
       X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RF_StepMotor_ID],0, YAW_PID_ACC_RPMPS, RightFront_rpm, 1);
       
    }

    if(LeftRear_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LR_StepMotor_ID],1, YAW_PID_ACC_RPMPS, -LeftRear_rpm, 1);
       
    }
    else
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LR_StepMotor_ID],0, YAW_PID_ACC_RPMPS, LeftRear_rpm, 1);
        
    }

    if(RightRear_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RR_StepMotor_ID],1, YAW_PID_ACC_RPMPS, -RightRear_rpm, 1);
        
    }
    else
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RR_StepMotor_ID],0, YAW_PID_ACC_RPMPS, RightRear_rpm, 1);
       
    }

    //发送同步运动命令，所有电机同时开始运动
   // X_V2_Synchronous_motion(0);
    X_V2_Multi_Motor_Cmd(0);
}


void StepMotor_SetvSpeed(StepMotor_Master* Master,float vSpeed, float SpeedAngle)
{
    if(Master == NULL)
    {
        return;
    }

    float rad_s = vSpeed * PI / 180.0f;              //将角速度转换为弧度制
    Master->vSpeed = rad_s*6.0f/(PI*WHEEL_D_CM);     //将角速度转换为电机转速(RPM)

    //输入限幅防止过冲
    if(Master->vSpeed > FINE_PID_SPD_RPM)
    {
        Master->vSpeed = FINE_PID_SPD_RPM;
    }
    else if(Master->vSpeed < -FINE_PID_SPD_RPM)
    {
        Master->vSpeed = -FINE_PID_SPD_RPM;
    }

    Master->SpeedAngle = SpeedAngle * PI / 180.0f;   //将角度转换为弧度制，便于计算

    //将极坐标系下的移动参数转换为笛卡尔坐标系下的移动参数，便于计算每个电机的目标位置
    float X_v = Master->vSpeed * cosf(Master->SpeedAngle);
    float Y_v = Master->vSpeed * sinf(Master->SpeedAngle);

    //根据笛卡尔坐标系下的移动参数通过麦克纳姆轮运动学解算计算每个电机的目标位置，便于发送给电机控制器
    float LeftFront_rpm  = Y_v - X_v;           //左前电机目标位置
    float RightFront_rpm = Y_v + X_v;           //右前电机目标位置
    float LeftRear_rpm   = -X_v - Y_v;          //左后电机目标位置
    float RightRear_rpm  = X_v - Y_v;           //右后电机目标位置

    //由于这里位置范围非常大，所以不做输出限幅


    //根据计算出的每个电机的目标角度发送控制命令给电机控制器，控制电机运动,多机同步
    if(LeftFront_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LF_StepMotor_ID],1, FINE_PID_ACC_RPMPS, -LeftFront_rpm, 1);
        
    }
    else
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LF_StepMotor_ID],0, FINE_PID_ACC_RPMPS, LeftFront_rpm, 1);
       
    }

    if(RightFront_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RF_StepMotor_ID],1, FINE_PID_ACC_RPMPS, -RightFront_rpm, 1);
       
    }
    else
    {
       X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RF_StepMotor_ID],0, FINE_PID_ACC_RPMPS, RightFront_rpm, 1);
       
    }

    if(LeftRear_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LR_StepMotor_ID],1, FINE_PID_ACC_RPMPS, -LeftRear_rpm, 1);
       
    }
    else
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LR_StepMotor_ID],0, FINE_PID_ACC_RPMPS, LeftRear_rpm, 1);
        
    }

    if(RightRear_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RR_StepMotor_ID],1, FINE_PID_ACC_RPMPS, -RightRear_rpm, 1);
        
    }
    else
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RR_StepMotor_ID],0, FINE_PID_ACC_RPMPS, RightRear_rpm, 1);
       
    }

    //发送同步运动命令，所有电机同时开始运动
   // X_V2_Synchronous_motion(0);
    X_V2_Multi_Motor_Cmd(0);
}



void StepMotor_SetSpeed_PID(StepMotor_Master* Master,float vSpeed,float deg_s ,float SpeedAngle)
{
    if(Master == NULL)
    {
        return;
    }

    float wrad_s = deg_s * PI / 180.0f;           //将角速度转换为弧度制
    Master->wSpeed = wrad_s*6.0f/(PI*WHEEL_D_CM); //将角速度转换为电机转速(RPM)

    //这里默认逆时针旋转为正方向，可以调节角速度补偿系数
    float LeftFront_wrpm  = ((Master->wSpeed*(L_mm+W_mm))*wSpeed_Compensate);     //左前电机目标角速度
    float RightFront_wrpm = ((Master-> wSpeed*(L_mm+W_mm))*wSpeed_Compensate);     //右前电机目标角速度
    float LeftRear_wrpm   = ((Master->wSpeed*(L_mm+W_mm))*wSpeed_Compensate);     //左后电机目标角速度
    float RightRear_wrpm  = ((Master-> wSpeed*(L_mm+W_mm))*wSpeed_Compensate);     //右后电机目标角速度

    float vrad_s = vSpeed * PI / 180.0f;              //将角速度转换为弧度制
    Master->vSpeed = vrad_s*6.0f/(PI*WHEEL_D_CM);     //将角速度转换为电机转速(RPM)

    //输入限幅防止过冲
    if(Master->vSpeed > YAW_PID_MAX_OMEGA)
    {
        Master->vSpeed = YAW_PID_MAX_OMEGA;
    }
    else if(Master->vSpeed < -YAW_PID_MAX_OMEGA)
    {
        Master->vSpeed = -YAW_PID_MAX_OMEGA;
    }

    Master->SpeedAngle = SpeedAngle * PI / 180.0f;   //将角度转换为弧度制，便于计算

    //将极坐标系下的移动参数转换为笛卡尔坐标系下的移动参数，便于计算每个电机的目标位置
    float X_v = Master->vSpeed * cosf(Master->SpeedAngle);
    float Y_v = Master->vSpeed * sinf(Master->SpeedAngle);

    //根据笛卡尔坐标系下的移动参数通过麦克纳姆轮运动学解算计算每个电机的目标位置，便于发送给电机控制器
    float LeftFront_vrpm  = Y_v - X_v;           //左前电机目标位置
    float RightFront_vrpm = Y_v + X_v;           //右前电机目标位置
    float LeftRear_vrpm   = -X_v - Y_v;          //左后电机目标位置
    float RightRear_vrpm  = X_v - Y_v;           //右后电机目标位置

    //由于这里位置范围非常大，所以不做输出限幅

    float LeftFront_rpm  = LeftFront_vrpm + LeftFront_wrpm;        //左前电机矢量转速
    float RightFront_rpm = RightFront_vrpm + RightFront_wrpm;      //右前电机矢量转速
    float LeftRear_rpm   = LeftRear_vrpm + LeftRear_wrpm;          //左后电机矢量转速
    float RightRear_rpm  = RightRear_vrpm + RightRear_wrpm;        //右后电机矢量转速


    //根据计算出的每个电机的目标角度发送控制命令给电机控制器，控制电机运动,多机同步
    if(LeftFront_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LF_StepMotor_ID],1, YAW_PID_ACC_RPMPS , -LeftFront_rpm, 1);
        
    }
    else
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LF_StepMotor_ID],0, YAW_PID_ACC_RPMPS, LeftFront_rpm, 1);
       
    }

    if(RightFront_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RF_StepMotor_ID],1, YAW_PID_ACC_RPMPS, -RightFront_rpm, 1);
       
    }
    else
    {
       X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RF_StepMotor_ID],0, YAW_PID_ACC_RPMPS, RightFront_rpm, 1);
       
    }

    if(LeftRear_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LR_StepMotor_ID],1, YAW_PID_ACC_RPMPS, -LeftRear_rpm, 1);
       
    }
    else
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[LR_StepMotor_ID],0, YAW_PID_ACC_RPMPS, LeftRear_rpm, 1);
        
    }

    if(RightRear_rpm < 0)
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RR_StepMotor_ID],1, YAW_PID_ACC_RPMPS, -RightRear_rpm, 1);
        
    }
    else
    {
        X_V2_MMCL_Vel_Control(Master->StepMotor_Addr[RR_StepMotor_ID],0, YAW_PID_ACC_RPMPS, RightRear_rpm, 1);
       
    }

    //发送同步运动命令，所有电机同时开始运动
    // X_V2_Synchronous_motion(0);
    X_V2_Multi_Motor_Cmd(0);
}


// void StepMotor_GetDataCallBack(StepMotor_Master* Master, uint16_t Size)
// {
//     if(Master == NULL)
//     {
//         return;
//     }

//     if(Size >2 && Size < STEPMOTOR_RX_BUFFER_SIZE)
//     {
//         if(StepMotor_RxBuffer[0] == '[' && StepMotor_RxBuffer[5] == ']')
//         {
//             Master->ActualPosition = (float)((StepMotor_RxBuffer[1]<<8) | StepMotor_RxBuffer[2])/2000.0f; //实际位置
//             Master->ActualAngle = (float)((StepMotor_RxBuffer[3]<<8) | StepMotor_RxBuffer[4])/2000.0f;    //实际角度(角度制)
//         }
        
//     }
    
//     //开启下一帧接收
//     StepMotor_StartReceive_DMA(Master, StepMotor_RxBuffer);
   
// }

