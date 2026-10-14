#ifndef __BSP_DWT_H_
#define __BSP_DWT_H_


#include "main.h"
#include "cmsis_armcc.h"

typedef struct
{
    uint32_t s;
    uint16_t ms;
    uint16_t us;
} DWT_Time_t;





void DWT_Init(uint32_t CPU_Freq_mHz);
void DWT_SysTimeUpdate(void);
void DWT_Delay(float Delay);
void DWT_Delay_ms(float Delay);
void DWT_Delay_us(float Delay);
void DWT_Delay_6xns(float Delay);
float DWT_GetDeltaT(uint32_t *cnt_last);
float DWT_GetTimeline_s(void);
float DWT_GetTimeline_ms(void);
double DWT_GetDeltaT64(uint32_t *cnt_last);

uint64_t DWT_GetTimeline_us(void);







#endif

