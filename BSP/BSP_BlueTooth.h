#ifndef __BSP_BLUETOOTH_H
#define __BSP_BLUETOOTH_H

#include "main.h"

typedef enum
{
    BT_Init_Wait     = 0,
    BT_Connect_Wait  = 1,
    BT_Connect_OK    = 2,
    BT_ERROR         = 3
}BT_State;

typedef enum
{
    BT_RX_OK   = 1,
    BT_RX_WAIT = 2
}BT_Flag;

typedef enum
{
    BT_MODE_BLOCK = 1,
    BT_MODE_IT    = 2,
    BT_MODE_DMA   = 3
}BT_Mode;

typedef enum
{
    BT_AT_ENABLE  = 0,
    BT_AT_DISABLE = 1,
}BT_AT;

typedef struct 
{
    GPIO_TypeDef* GPIO_Port;
    uint16_t      GPIO_KEY_Pin;
    uint16_t      GPIO_STATE_Pin;
}BT_GPIO;


typedef struct 
{
    UART_HandleTypeDef* UART;
    uint16_t            UART_TX;
    uint16_t            UART_RX;
}BT_UART;


typedef struct
{
   uint8_t* TxBuf;
   uint8_t* RxBuf;
   uint8_t* ATBuf;
   uint16_t TxBuf_Size;
   uint16_t RxBuf_Size;
}BT_Base;


typedef struct 
{
    HAL_StatusTypeDef (*BT_Transmit)(char* String);
    
    HAL_StatusTypeDef (*BT_Recieve)(UART_HandleTypeDef *huart, uint8_t *pData, 
                                     uint16_t Size, uint32_t Timeout);
    HAL_StatusTypeDef (*BT_Recieve_Idle)(UART_HandleTypeDef *huart, uint8_t *pData, 
                                         uint16_t Size);
}BT_FunctionTable;



typedef struct
{
    uint8_t             Instance;
    BT_GPIO             BT_GPIO;
    BT_UART             BT_UART;
    BT_Base             BT_Base;
    BT_State            BT_State;
    BT_Mode             BT_Mode;
    BT_Flag             BT_Flag;
    BT_FunctionTable    Table;
}BT_Device;

typedef struct
{
    uint8_t             BT_type;
    BT_Mode             BT_Mode;
    BT_AT               BT_AT;
    GPIO_TypeDef      * GPIO_Port;
    uint16_t            GPIO_KEY_Pin;
    uint16_t            GPIO_STATE_Pin;
    UART_HandleTypeDef* UART;
    uint16_t            UART_TX;
    uint16_t            UART_RX;
    uint16_t            TxBuf_Size;
    uint16_t            RxBuf_Size;

}BT_InitConfig;


//蓝牙模块型号定义
#define BT_HC_40D            0xAA

//蓝牙模块引脚定义
#define BT_KEY_Pin           GPIO_PIN_14
#define BT_STATE_Pin         GPIO_PIN_15
#define BT_STATE_GPIO_Port   GPIOB
#define BT_KEY_GPIO_Port     GPIOB

//蓝牙模块UART定义
#define BT_RX_Pin            GPIO_PIN_10
#define BT_TX_Pin            GPIO_PIN_11
#define BT_TX_GPIO_Port      GPIOC
#define BT_RX_GPIO_Port      GPIOC

#define BT_UART_HANDLE        &huart4
#define BT_DMA_UART_RX_HANDLE &hdma_uart4_rx



BT_Device* BSP_BT_Init(BT_InitConfig* BT);
void BSP_BT_GetConnectState(BT_Device* BT);
void BSP_BT_StartConfigParam(BT_Device* BT);
void BSP_BT_StopConfigParam(BT_Device* BT);
void BSP_BT_RecieveATCMD(BT_Device* BT);
void BSP_BT_StartReceive_DMA(BT_Device* BT);

#endif

