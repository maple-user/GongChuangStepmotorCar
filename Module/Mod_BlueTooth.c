#include "Mod_BlueTooth.h"
#include "BSP_DWT.h"
#include <string.h>

#define BT_Device_MAXNUM     1
#define BT_AT_CMD_MAXNUM     15
#define BT_AT_CMD_MAXLEN     25


static BT_Device* BT_Device_Pool[BT_Device_MAXNUM] = {0};

static char BT_AT_CMD_Table[BT_AT_CMD_MAXNUM][BT_AT_CMD_MAXLEN] = 
{
    
    "AT+BAUT=115200,N\r\n",     
    //..........
};

uint8_t BT_Command_Buf[BT_CMD_MAXLEN] = {0};


void BT_Init(void)
{
    //DWT_Init();这里在App层会做初始化
    BT_InitConfig BT_InitStruct = {0};

    BT_InitStruct.BT_type        = BT_HC_40D;
    BT_InitStruct.BT_Mode        = BT_MODE_DMA;
    BT_InitStruct.BT_AT          = BT_AT_DISABLE;
    BT_InitStruct.GPIO_Port      = BT_KEY_GPIO_Port;
    BT_InitStruct.GPIO_KEY_Pin   = BT_KEY_Pin;
    BT_InitStruct.GPIO_STATE_Pin = BT_STATE_Pin;
    BT_InitStruct.UART           = &huart4;
    BT_InitStruct.UART_RX        = BT_RX_Pin;
    BT_InitStruct.UART_TX        = BT_TX_Pin;
    BT_InitStruct.RxBuf_Size     = 60;
    BT_InitStruct.TxBuf_Size     = 30;

    BT_Device_Pool[0] = BSP_BT_Init(&BT_InitStruct);


    //确保蓝牙模块完成上电初始化
    DWT_Delay_ms(300);
    //开启首次接收
    BSP_BT_StartReceive_DMA(BT_Device_Pool[0]);

//    while (BT_Device_Pool[0]->BT_State != BT_Connect_OK)
//    {
//       BT_RefreshConnectState(BT_Device_Pool[0]);
//       //HAL_IWDG_Refresh(&hiwdg);
//    }
    
}



BT_Device* BT_GetDevice(uint8_t BT_Name)
{
    if(BT_Name >= BT_Device_MAXNUM)
    {
        return NULL;
    }
    return BT_Device_Pool[BT_Name];
}


void BT_RefreshConnectState(BT_Device* BT)
{
    if(BT == NULL||BT->BT_State == BT_ERROR)
    {
        return;
    }
    BSP_BT_GetConnectState(BT);
}


void BT_SetParam(BT_Device* BT, uint8_t AT_CMD)
{


    if(BT == NULL||BT->BT_State == BT_ERROR)
    {
        return;
    }
    if(BT->BT_State == BT_Connect_OK)
    {
        BSP_BT_StartConfigParam(BT);

        BT->Table.BT_Transmit("AT\r\n");

        BSP_BT_RecieveATCMD(BT);

        if(strcmp((char*)(BT->BT_Base.ATBuf), "OK") == 0)
        {
            switch (AT_CMD)
            {
            case AT_BAUD:
                BT->Table.BT_Transmit((char*)(&BT_AT_CMD_Table[AT_BAUD]));
                BSP_BT_RecieveATCMD(BT);
                if(strcmp((char*)(BT->BT_Base.ATBuf), "OK+BAUD=115200,NONE") != 0)
                {
                    BT->BT_State = BT_ERROR;
                    return;
                }
                break;
            
            default:
                break;
            }

        }
        else
        {   //AT指令通讯失败
            BT->BT_State = BT_ERROR;
            return;
        }
     }

}


void BT_GetDataCallBack(BT_Device* BT, uint8_t* DataBuf, uint16_t Size)
{
    if(BT == NULL||BT->BT_State == BT_ERROR)
    {
        return;
    }
    if(Size >2 && Size < BT_CMD_MAXLEN)
    {
        if(BT->BT_Base.RxBuf[0] == '[' && BT->BT_Base.RxBuf[Size-1] == ']')
        {
            uint8_t commandlen = Size - 2;
            memcpy(DataBuf, &(BT->BT_Base.RxBuf[1]), commandlen);
            DataBuf[commandlen] = '\0';
            BT->BT_Flag = BT_RX_OK;
        }
        
    }
    BSP_BT_StartReceive_DMA(BT);
}
