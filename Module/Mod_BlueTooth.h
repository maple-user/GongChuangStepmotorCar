#ifndef __MOD_BLUETOOTH_H
#define __MOD_BLUETOOTH_H

#include "main.h"
#include "usart.h"
#include "BSP_BlueTooth.h"

//蓝牙型号及其注册名
#define   BT_HC_40D_Name    0
//AT指令集
#define   AT_BAUD           0

//蓝牙指令接收缓存区大小
#define BT_CMD_MAXLEN        60

extern uint8_t BT_Command_Buf[BT_CMD_MAXLEN];


void BT_Init(void);
BT_Device* BT_GetDevice(uint8_t BT_Name);
void BT_RefreshConnectState(BT_Device* BT);
void BT_SetParam(BT_Device* BT, uint8_t AT_CMD);

void BT_GetDataCallBack(BT_Device* BT, uint8_t* DataBuf, uint16_t Size);
#endif
