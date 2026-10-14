#ifndef __MOD_MAIXCAM_H
#define __MOD_MAIXCAM_H

#include "BSP_MAIXCAM.h"


void MAIXCAM_Init(void);
MAIXCAM_Device* MAIXCAM_GetDevice(void);
void MAIXCAM_GetDataCallBack(MAIXCAM_Device* Device, uint16_t Size);

#endif


