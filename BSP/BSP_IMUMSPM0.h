#ifndef __BSP_IMUMSPM0_H
#define __BSP_IMUMSPM0_H



#include "stm32f4xx_hal.h"
#include "usart.h"



//可供修改的配置
#define IMUMSPM0_UART_HANDLE                       &huart3


//IMUMSPM0数据帧定义
#define IMUMSPM0_FRAME_READ_HEAD                   0x5A

/*数据帧类型定义*/
#define IMUMSPM0_FRAME_TYPE_SPEED                  0xAA
#define IMUMSPM0_FRAME_TYPE_ANGLE                  0xBB
#define IMUMSPM0_FRAME_TYPE_STATE                  0xCC

//IMUMSPM0操作帧定义
#define IMUMSPM0_FRAME_WRITE_HEAD1                 0x55
#define IMUMSPM0_FRAME_WRITE_HEAD2                 0xAA

/*默认操作*/
#define IMUMSPM0_FRAME_FUNCTION_DEFUALT            0xFEFE

/*KEY操作，所有设置进行前需要先解锁，设置完成后必须保存，延时100ms*/
#define IMUMSPM0_FRAME_KEY_REGADDRESS              0x13
#define IMUMSPM0_FRAME_KEY_FUNCTION_UNLOCK         0x5F8E
#define IMUMSPM0_FRAME_KEY_FUNCTION_RELOCK         0x0000

/*SAVE操作,延时100ms*/
#define IMUMSPM0_FRAME_SAVE_REGADDRESS             0x00
#define IMUMSPM0_FRAME_SAVE_FUNCTION_RESTORE       0x0000
#define IMUMSPM0_FRAME_SAVE_FUNCTION_RESTART       0x00FF
#define IMUMSPM0_FRAME_SAVE_FUNCTION_DEFAULT       0x0001

/*YAW_ZERO操作，延时100ms*/
#define IMUMSPM0_FRAME_YAW_ZERO_REGADDRESS         0x15
#define IMUMSPM0_FRAME_YAW_ZERO_FUNCTION_SET       0x0000

/*BIAS_CAL操作，延时20s*/
#define IMUMSPM0_FRAME_BIAS_CAL_REGADDRESS         0x0A
#define IMUMSPM0_FRAME_BIAS_CAL_STATE_REGADDRESS   0x04
#define IMUMSPM0_FRAME_BIAS_CAL_STATE_SUCCESSCODE  0x1BC      
#define IMUMSPM0_FRAME_BIAS_CAL_FUNCTION_AUTOBIAS  0x0001
#define IMUMSPM0_FRAME_BIAS_CAL_FUNCTION_GETSTATE  0x000A

/*SCALE_FACTOR操作，延时100ms*/
#define IMUMSPM0_FRAME_SCALE_FACTOR_REGADDRESS     0x0A
#define IMUMSPM0_FRAME_SCALE_FACTOR_FUNCTION_MANU  0x0003

/*BUAT操作，延时100ms,在20s内完成SAVE结束帧的发送（默认9600bps）*/
#define IMUMSPM0_FRAME_BUAD_REGADDRESS             0x03
#define IMUMSPM0_FRAME_BUAD_FUNCTION_2400bps       0x0000
#define IMUMSPM0_FRAME_BUAD_FUNCTION_4800bps       0x0001
#define IMUMSPM0_FRAME_BUAD_FUNCTION_9600bps       0x0002
#define IMUMSPM0_FRAME_BUAD_FUNCTION_19200bps      0x0003
#define IMUMSPM0_FRAME_BUAD_FUNCTION_38400bps      0x0004
#define IMUMSPM0_FRAME_BUAD_FUNCTION_57600bps      0x0005
#define IMUMSPM0_FRAME_BUAD_FUNCTION_115200bps     0x0006
#define IMUMSPM0_FRAME_BUAD_FUNCTION_230400bps     0x0007

/*RATE操作，延时100ms,在20s内完成SAVE结束帧的发送（默认10Hz）*/
#define IMUMSPM0_FRAME_RATE_REGADDRESS             0x02
#define IMUMSPM0_FRAME_RATE_FUNCTION_0_1Hz         0x0000
#define IMUMSPM0_FRAME_RATE_FUNCTION_0_2Hz         0x0001
#define IMUMSPM0_FRAME_RATE_FUNCTION_0_5Hz         0x0002
#define IMUMSPM0_FRAME_RATE_FUNCTION_1Hz           0x0003
#define IMUMSPM0_FRAME_RATE_FUNCTION_2Hz           0x0004
#define IMUMSPM0_FRAME_RATE_FUNCTION_5Hz           0x0005
#define IMUMSPM0_FRAME_RATE_FUNCTION_10Hz          0x0006
#define IMUMSPM0_FRAME_RATE_FUNCTION_20Hz          0x0007
#define IMUMSPM0_FRAME_RATE_FUNCTION_50Hz          0x0008
#define IMUMSPM0_FRAME_RATE_FUNCTION_100Hz         0x0009
#define IMUMSPM0_FRAME_RATE_FUNCTION_125Hz         0x000A
#define IMUMSPM0_FRAME_RATE_FUNCTION_200Hz         0x000B
#define IMUMSPM0_FRAME_RATE_FUNCTION_250Hz         0x000C
#define IMUMSPM0_FRAME_RATE_FUNCTION_500Hz         0x000D
#define IMUMSPM0_FRAME_RATE_FUNCTION_1000Hz        0x000E


typedef enum
{
    IMUMSPM0_WAIT  = 0,
    IMUMSPM0_TYPE  = 1,
    IMUMSPM0_DATAL = 2,
    IMUMSPM0_DATAH = 3,
    IMUMSPM0_MCRC  = 4
}IMUMSPM0_State;

typedef enum
{
   IMUMSPM0_TYPE_SPEED = 7,
   IMUMSPM0_TYPE_ANGLE = 8
}IMUMSPM0_DataType;

typedef struct
{
   volatile uint8_t            DataByte;           //寄存器读取的原始值
   volatile int16_t            newDataSpeed;       //正在接收的新角速度数据
   volatile int16_t            oldDataSpeed;       //CPU可以安全读取旧角速度数据
   volatile int16_t            newDataAngle;       //正在接收的新角度数据
   volatile int16_t            oldDataAngle;       //CPU可以安全读取旧角度数据
   volatile float              Speed;              //角速度 rad/s
   volatile float              Angle;              //角度 rad
   volatile IMUMSPM0_DataType  DataType;           //数据帧类型
}IMUMSPM0_Data;


typedef struct 
{
    uint8_t        ErrorCode;
    uint16_t       ID;
    IMUMSPM0_Data  Data;
    IMUMSPM0_State State;
}IMUMSPM0_Device;



/**
  * @brief  IMUMSPM0 BSP层阻塞式发送字节数据
  * @param  ByteBuf 字节数据缓存区数组地址
  * @param  Timeout 超时等待值
  * @retval 无
  */
#define BSP_IMUMSPM0_SendBytes(ByteBuf,Size,Timeout)                                \
do                                                                                  \
{                                                                                   \
     HAL_UART_Transmit((IMUMSPM0_UART_HANDLE),(ByteBuf),(Size),(Timeout));          \
}while (0U)

/**
  * @brief  IMUMSPM0 BSP层阻塞式接收字节数据
  * @param  ByteBuf 字节数据缓存区数组地址
  * @param  Timeout 超时等待值
  * @retval 无
  */
#define BSP_IMUMSPM0_ReadBytes(ByteBuf,Size,Timeout,StatusAdd)                      \
do                                                                                  \
{                                                                                   \
    *(StatusAdd) = HAL_UART_Receive((IMUMSPM0_UART_HANDLE),                         \
                                  (ByteBuf),(Size),(Timeout));                      \
}while (0U)

/**
  * @brief  IMUMSPM0 BSP层中断式接收字节数据
  * @param  ByteBuf 字节数据缓存区数组地址
  * @param  Timeout 超时等待值
  * @retval 无
  */
#define BSP_IMUMSPM0_ReadBytes_IT(ByteBuf,Size,StatusAdd)                           \
do                                                                                  \
{                                                                                   \
    *(StatusAdd) = HAL_UART_Receive_IT((IMUMSPM0_UART_HANDLE),                      \
                                  (ByteBuf),(Size));                                \
}while (0U)


void BSP_IMUMSPM0_Init(IMUMSPM0_Device* Device);
HAL_StatusTypeDef BSP_IMUMSPM0_SetBuadrate(uint32_t newBuadrate);

#endif


