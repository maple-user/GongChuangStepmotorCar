#ifndef __MOD_IMUMSPM0_H
#define __MOD_IMUMSPM0_H

#include "main.h"
#include "BSP_IMUMSPM0.h"



//IMUMSPM0命令索引定义
#define IMUMSPM0_CMD_YAW_ZERO            1
#define IMUMSPM0_CMD_BIAS_CAL            2
#define IMUMSPM0_CMD_SCALE_FACTOR        3
#define IMUMSPM0_CMD_BUAD                4
#define IMUMSPM0_CMD_RATE                5
#define IMUMSPM0_CMD_SAVE                6

//IMUMSPM0错误码定义
#define IMUMSPM0_ERR_TIMEOUT             1
#define IMUMSPM0_ERR_STATE               2
#define IMUMSPM0_ERR_CRC                 3
#define IMUMSPM0_ERR_NONE                4
#define IMUMSPM0_ERR_FRAME               5
#define IMUMSPM0_ERR_INIT                6


void IMUMSPM0_Init(void);
IMUMSPM0_Device* IMUMSPM0_GetDevice(void);

void IMUMSPM0_GetDataCallback(IMUMSPM0_Device* Device);
void IMUMSPM0_GetData(IMUMSPM0_Device* Device);
void IMUMSPM0_SendCommand(IMUMSPM0_Device* Device, uint8_t IMUMSPM0_CMD_IDX,const uint16_t IMUMSPM0_CMD_FUNCTION);




#endif


