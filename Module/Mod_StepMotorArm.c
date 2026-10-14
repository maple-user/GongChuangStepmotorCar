#include "Mod_StepMotorArm.h"
#include "BSP_DWT.h"
#include "usart.h"
#include <math.h>


//X轴电机顺时针转向前，Y轴向上是逆时针，ZDT电机逆时针转给0方向


//总线电机使用的串口相关句柄
#define STEPMOTOR_UART_HANDLE            &huart1
#define STEPMOTOR_DMA_UART_RX_HANDLE     &hdma_usart1_rx
#define STEPMOTOR_DMA_UART_TX_HANDLE     &hdma_usart1_tx

//总线电机注册表ID定义
#define X_StepMotor_ID       0U      //X轴电机ID
#define Y_StepMotor_ID       1U      //Y轴电机ID
#define StepMotor_NUM        2U      //总线电机数量

//总线电机注册表地址定义
#define X_StepMotor_ADDR      0x05  //X电机地址
#define Y_StepMotor_ADDR      0x06  //Y电机地址


//底部皮带旋转轴 （T型曲线）
#define ARM_ROTATE_SPD_RPM      85.0f   // 峰值转速 (RPM)
#define ARM_ROTATE_ACC_RPMPS    42.0f   // 加速度 (RPM/s)
#define ARM_ROTATE_DEC_RPMPS    42.0f   // 减速度 (RPM/s)

// 前后伸缩齿轮齿条轴 （T型曲线）
#define ARM_EXTEND_SPD_RPM      65.0f   // 峰值转速 (RPM)
#define ARM_EXTEND_ACC_RPMPS    40.0f   // 加速度 (RPM/s)
#define ARM_EXTEND_DEC_RPMPS    40.0f   // 减速度 (RPM/s)

//状态帧位掩码定义目前只使用了电机到位掩码
#define STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE   0x02U  //电机到位掩码


//一些固定参数，减少计算性能开销
#define PI                   3.1415927f //圆周率
#define WHEEL_X_D_CM         1.5f       //轮子直径 (cm)
#define WHEEL_Y_D_CM         1.0f       //轮子直径 (cm)

static const float Slip_Compensate_X = 0.616f;  //X轮子打滑补偿系数默认轮子无打滑0.625f
static const float Slip_Compensate_Y = 0.6206f;  //Y轮子打滑补偿系数默认轮子无打滑

//总线电机注册表池
#define StepMotorArm_Master_MAXNUM 1
static StepMotorArm_Master Motor_Master_Pool[StepMotorArm_Master_MAXNUM] = {0};


void StepMotorArm_Init(void)
{
    for(uint8_t i = 0; i < StepMotorArm_Master_MAXNUM; i++)
    {
        //初始化总线电机注册表
        Motor_Master_Pool[i].MotorNum = StepMotor_NUM;
        Motor_Master_Pool[i].StepMotor_Addr[X_StepMotor_ID] = X_StepMotor_ADDR;   //X电机地址
        Motor_Master_Pool[i].StepMotor_Addr[Y_StepMotor_ID] = Y_StepMotor_ADDR;   //Y电机地址


        //默认使能所有电机
        Motor_Master_Pool[i].EnableFlag[X_StepMotor_ID] = true;       //X电机使能
        Motor_Master_Pool[i].EnableFlag[Y_StepMotor_ID] = true;       //Y电机使能
       

        //默认任务标志位为true，表示任务已完成
        Motor_Master_Pool[i].TaskFlag[X_StepMotor_ID]   = true;       //X电机任务标志位
        Motor_Master_Pool[i].TaskFlag[Y_StepMotor_ID]   = true;       //Y电机任务标志位
       

        //初始化所有目标参数
        Motor_Master_Pool[i].wSpeed    = 0.0f;      //旋转角速度
        Motor_Master_Pool[i].vSpeed    = 0.0f;      //移动速度
        Motor_Master_Pool[i].Angle     = 0.0f;      //行进角度
        Motor_Master_Pool[i].Position  = 0.0f;      //移动位移
    }
}

StepMotorArm_Master* StepMotorArm_GetMaster(void)
{
    
    return &Motor_Master_Pool[0];
}

void StepMotorArm_StartReceive(StepMotorArm_Master* Master)
{
    if(Master == NULL)
    {
        return;
    }
    HAL_UART_Receive(STEPMOTOR_UART_HANDLE, Master->RxBuffer, sizeof(Master->RxBuffer), 20);
}


void StepMotorArm_StartReceive_DMA(StepMotorArm_Master* Master)
{
    if(Master == NULL)
    {
        return;
    }
    HAL_UARTEx_ReceiveToIdle_DMA(STEPMOTOR_UART_HANDLE, Master->RxBuffer, sizeof(Master->RxBuffer));
    __HAL_DMA_DISABLE_IT(STEPMOTOR_DMA_UART_RX_HANDLE,DMA_IT_HT);
}


void StepMotorArm_Move_cm(StepMotorArm_Master* Master, float X_cm, float Y_cm)
{
    if(Master == NULL)
    {
        return;
    }

    //考虑轮子打滑补偿系数,转化成角度
    X_cm = X_cm * Slip_Compensate_X/ (WHEEL_X_D_CM * PI) * 360.0f;
    Y_cm = Y_cm * Slip_Compensate_Y/ (WHEEL_Y_D_CM * PI) * 360.0f;
    
    //根据计算出的每个电机的目标角度发送控制命令给电机控制器，控制电机运动,多机同步
    if(X_cm < 0)
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[X_StepMotor_ID],1, ARM_EXTEND_ACC_RPMPS, ARM_EXTEND_DEC_RPMPS, ARM_EXTEND_SPD_RPM, -X_cm, 1, 1);
      
    }
    else
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[X_StepMotor_ID],0, ARM_EXTEND_ACC_RPMPS, ARM_EXTEND_DEC_RPMPS, ARM_EXTEND_SPD_RPM, X_cm, 1, 1);
        
    }

     //根据计算出的每个电机的目标角度发送控制命令给电机控制器，控制电机运动,多机同步
    if(Y_cm < 0)
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[Y_StepMotor_ID],1, ARM_ROTATE_ACC_RPMPS, ARM_ROTATE_DEC_RPMPS, ARM_ROTATE_SPD_RPM, -Y_cm, 1, 1);
      
    }
    else
    {
        X_V2_MMCL_Traj_Pos_Control(Master->StepMotor_Addr[Y_StepMotor_ID],0, ARM_ROTATE_ACC_RPMPS, ARM_ROTATE_DEC_RPMPS, ARM_ROTATE_SPD_RPM, Y_cm, 1, 1);
        
    }
     X_V2_Multi_Motor_Cmd(0);

    Master->TaskFlag[X_StepMotor_ID]   = false;       
    Master->TaskFlag[Y_StepMotor_ID]   = false; 

      //等待电机控制器应答帧
    DWT_Delay_ms(20);

    //在等待过程中不断轮询电机状态帧，直到所有电机的任务标志位都为true，表示任务已完成
    for(;;)
    {
        if(Master->TaskFlag[X_StepMotor_ID] != true)
        {
            X_V2_Read_Sys_Params(Master->StepMotor_Addr[X_StepMotor_ID],S_FLAG);
            StepMotorArm_StartReceive(Master);
            //校验电机地址和尾部校验字节
            if((Master->RxBuffer[0] == Master->StepMotor_Addr[X_StepMotor_ID])&&(Master->RxBuffer[1] == 0x3A))
            {
                if((Master->RxBuffer[2] & STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE) == STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE)
                {
                    Master->TaskFlag[X_StepMotor_ID] = true;
                }
              
            }
        }
        if(Master->TaskFlag[Y_StepMotor_ID] != true)
        {
            X_V2_Read_Sys_Params(Master->StepMotor_Addr[Y_StepMotor_ID],S_FLAG);
            StepMotorArm_StartReceive(Master);
            //校验电机地址和尾部校验字节
            if((Master->RxBuffer[0] == Master->StepMotor_Addr[Y_StepMotor_ID])&&(Master->RxBuffer[1] == 0x3A))
            {
                if((Master->RxBuffer[2] & STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE) == STEPMOTOR_STATUS_MASK_MOTOR_ARRIVE)
                {
                    Master->TaskFlag[Y_StepMotor_ID] = true;
                }
              
            }
        }
        //检查所有电机的任务标志位是否为true，表示任务已完成
        if(Master->TaskFlag[X_StepMotor_ID] && Master->TaskFlag[Y_StepMotor_ID])
        {
            break;
        }
    }
}
