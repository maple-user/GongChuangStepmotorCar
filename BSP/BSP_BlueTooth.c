#include "BSP_BlueTooth.h"
#include "BSP_UART.h"

#define BT_TXBUF_MAX     60
#define BT_RXBUF_MAX     60
#define BT_AT_CMD_MAXLEN 25

//蓝牙指令接收缓存区
#define BT_CMD_MAXLEN    60

static BT_Device Device = {0};

static uint8_t BT_TxBuf[BT_TXBUF_MAX]     = {0};
static uint8_t BT_RxBuf[BT_RXBUF_MAX]     = {0};
static uint8_t BT_ATBuf[BT_AT_CMD_MAXLEN] = {0};

/**
  * @brief  蓝牙初始化
  * @param  BT 蓝牙配置结构体指针
  * @retval BT_Device* 蓝牙数据结构体指针
  */
BT_Device* BSP_BT_Init(BT_InitConfig* BT)
{
    if(BT == NULL)
    {
        return NULL;
    }
    Device.BT_State                = BT_Init_Wait;
    Device.Instance                = BT -> BT_type;
    Device.BT_Mode                 = BT -> BT_Mode;
    Device.BT_GPIO.GPIO_Port       = BT -> GPIO_Port;
    Device.BT_GPIO.GPIO_KEY_Pin    = BT -> GPIO_KEY_Pin;
    Device.BT_GPIO.GPIO_STATE_Pin  = BT -> GPIO_STATE_Pin;
    Device.BT_UART.UART            = BT -> UART;
    Device.BT_UART.UART_RX         = BT -> UART_RX;
    Device.BT_UART.UART_TX         = BT -> UART_TX;
    Device.BT_Base.RxBuf           = BT_RxBuf;
    Device.BT_Base.TxBuf           = BT_TxBuf;

    if(BT -> BT_AT == BT_AT_ENABLE)
    {
        Device.BT_Base.ATBuf = BT_ATBuf;
    }
    else
    {
        Device.BT_Base.ATBuf = NULL;
    }
    //默认阻塞式发送
    Device.Table.BT_Transmit       = UART_SendString_Block;
    Device.BT_Base.TxBuf_Size      = (BT -> TxBuf_Size >= BT_TXBUF_MAX ? BT_TXBUF_MAX : BT -> TxBuf_Size);

    if(Device.Instance == BT_HC_40D)
    {
        switch (Device.BT_Mode)
        {
        case BT_MODE_BLOCK:
            
            Device.BT_Base.RxBuf_Size    = BT -> RxBuf_Size;
         
            Device.Table.BT_Recieve      = HAL_UART_Receive;
            Device.Table.BT_Recieve_Idle = NULL;
            break;
        case BT_MODE_IT:
            Device.BT_Base.RxBuf_Size    = BT_RXBUF_MAX;

            Device.Table.BT_Recieve      = NULL;
            Device.Table.BT_Recieve_Idle = HAL_UARTEx_ReceiveToIdle_IT;
            break;
        case BT_MODE_DMA:
            Device.BT_Base.RxBuf_Size    = BT_RXBUF_MAX;

            Device.Table.BT_Recieve      = NULL;
            Device.Table.BT_Recieve_Idle = HAL_UARTEx_ReceiveToIdle_DMA;
            break;
        default:
            //模式参数配置错误
            Device.BT_State = BT_ERROR;
            return NULL;
        }

        Device.BT_State = BT_Connect_Wait;
        Device.BT_Flag  = BT_RX_WAIT;
        //默认上电后进入透传模式
        HAL_GPIO_WritePin(Device.BT_GPIO.GPIO_Port,Device.BT_GPIO.GPIO_KEY_Pin,GPIO_PIN_RESET);

        return &Device;
    }

    //没有相关设备型号支持
    return NULL;
    
}

/**
  * @brief  BSP层获取蓝牙连接状态
  * @param  BT 蓝牙设备结构体指针
  * @retval 无
  */
void BSP_BT_GetConnectState(BT_Device* BT)
{
    if(BT == NULL||BT->BT_State == BT_ERROR)
    {
        return;
    }
    GPIO_PinState state;
    state = HAL_GPIO_ReadPin(BT->BT_GPIO.GPIO_Port,BT->BT_GPIO.GPIO_STATE_Pin);

    if(state == GPIO_PIN_RESET)
    {
        BT -> BT_State = BT_Connect_OK;
        return;
    }

    BT -> BT_State = BT_Connect_Wait;
}

/**
  * @brief  BSP层启动蓝牙参数配置
  * @param  BT 蓝牙设备结构体指针
  * @retval 无
  */
void BSP_BT_StartConfigParam(BT_Device* BT)
{
    if(BT == NULL||BT->BT_State == BT_ERROR)
    {
        return;
    }
    
    HAL_GPIO_WritePin(BT->BT_GPIO.GPIO_Port,BT->BT_GPIO.GPIO_KEY_Pin,GPIO_PIN_SET);

}

/**
  * @brief  BSP层启动蓝牙参数配置
  * @param  BT 蓝牙设备结构体指针
  * @retval 无
  */
void BSP_BT_StopConfigParam(BT_Device* BT)
{
    if(BT == NULL||BT->BT_State == BT_ERROR)
    {
        return;
    }
    
    HAL_GPIO_WritePin(BT->BT_GPIO.GPIO_Port,BT->BT_GPIO.GPIO_KEY_Pin,GPIO_PIN_RESET);

}


/**
  * @brief  BSP层获取蓝牙参数配置信息
  * @note   需要在BT_AT_ENABLE模式下使用,同时使用阻塞式接收最大时间不超过4ms
  * @param  BT 蓝牙设备结构体指针
  * @retval 无
  */
void BSP_BT_RecieveATCMD(BT_Device* BT)
{
    if(BT == NULL||BT->BT_State == BT_ERROR||BT->BT_Base.ATBuf == NULL)
    {
        return;
    }
    HAL_UART_Receive(BT->BT_UART.UART, BT->BT_Base.ATBuf, BT_AT_CMD_MAXLEN, 4);

    if(BT->BT_UART.UART->RxXferCount < BT_AT_CMD_MAXLEN)
    {
       BT->BT_Base.ATBuf[(BT_AT_CMD_MAXLEN - BT->BT_UART.UART->RxXferCount)] = '\0';
    }
    else
    {
        //接收数据长度超过AT命令最大长度，强制在末尾添加字符串结束符
        BT->BT_Base.ATBuf[BT_AT_CMD_MAXLEN - 1] = '\0';
        BT->BT_State = BT_ERROR;
    }
}

void BSP_BT_StartReceive_DMA(BT_Device* BT)
{
    if(BT == NULL||BT->BT_State == BT_ERROR)
    {
        return;
    }
    
    if(BT->Table.BT_Recieve_Idle != NULL)
    {
        BT->Table.BT_Recieve_Idle(BT->BT_UART.UART, BT->BT_Base.RxBuf, BT->BT_Base.RxBuf_Size);
      
        __HAL_DMA_DISABLE_IT(BT_DMA_UART_RX_HANDLE,DMA_IT_HT);
    }
}

