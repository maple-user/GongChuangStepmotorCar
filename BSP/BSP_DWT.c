#include "BSP_DWT.h"


static DWT_Time_t SysTime;
static uint32_t   CPU_FREQ_Hz, CPU_FREQ_Hz_ms, CPU_FREQ_Hz_us;
static uint32_t   CYCCNT_RountCount;
static uint32_t   CYCCNT_LAST;
static uint64_t   CYCCNT64;

/**
 * @brief 私有函数,用于检查DWT CYCCNT寄存器是否溢出,并更新CYCCNT_RountCount
 * @attention 此函数假设两次调用之间的时间间隔不超过一次溢出
 */
static void DWT_CNT_Update(void)
{
    static volatile uint8_t bit_locker = 0;
    if (!bit_locker)
    {
        bit_locker                = 1;
        volatile uint32_t cnt_now = DWT->CYCCNT;
        if (cnt_now < CYCCNT_LAST)
            CYCCNT_RountCount++;

        CYCCNT_LAST = DWT->CYCCNT;
        bit_locker  = 0;
    }
}


/**
 * @brief 初始化DWT,传入参数为CPU频率,单位MHz
 *
 * @param CPU_Freq_mHz STM32F407主频为168MHz
 */
void DWT_Init(uint32_t CPU_Freq_mHz)
{
    /* 使能DWT外设 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* DWT CYCCNT寄存器计数清0 */
    DWT->CYCCNT = (uint32_t)0u;

    /* 使能Cortex-M DWT CYCCNT寄存器 */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    CPU_FREQ_Hz       = CPU_Freq_mHz * 1000000;
    CPU_FREQ_Hz_ms    = CPU_FREQ_Hz / 1000;
    CPU_FREQ_Hz_us    = CPU_FREQ_Hz / 1000000;
    CYCCNT_RountCount = 0;

    CYCCNT_LAST = DWT->CYCCNT;
}

/**
 * @brief 获取两次调用之间的时间间隔,单位为秒/s
 *
 * @param cnt_last 上一次调用的时间戳
 * @return float 时间间隔,单位为秒/s
 */
float DWT_GetDeltaT(uint32_t *cnt_last)
{
    volatile uint32_t cnt_now = DWT->CYCCNT;
    float             dt      = ((uint32_t)(cnt_now - *cnt_last)) / ((float)(CPU_FREQ_Hz));
    *cnt_last                 = cnt_now;

    DWT_CNT_Update();

    return dt;
}

/**
 * @brief 获取两次调用之间的时间间隔,单位为秒/s,高精度
 *
 * @param cnt_last 上一次调用的时间戳
 * @return double 时间间隔,单位为秒/s
 */
double DWT_GetDeltaT64(uint32_t *cnt_last)
{
    volatile uint32_t cnt_now = DWT->CYCCNT;
    double            dt      = ((uint32_t)(cnt_now - *cnt_last)) / ((double)(CPU_FREQ_Hz));
    *cnt_last                 = cnt_now;

    DWT_CNT_Update();

    return dt;
}


/**
 * @brief DWT更新时间轴函数,会被三个timeline函数调用
 * @attention
 * 如果长时间不调用timeline函数,则需要手动调用该函数更新时间轴,否则CYCCNT溢出后定时和时间轴不准确
 */
void DWT_SysTimeUpdate(void)
{
    volatile uint32_t cnt_now = DWT->CYCCNT;
    static uint64_t   CNT_TEMP1, CNT_TEMP2, CNT_TEMP3;

    DWT_CNT_Update();

    CYCCNT64   = (uint64_t)CYCCNT_RountCount * (uint64_t)UINT32_MAX + (uint64_t)cnt_now;
    CNT_TEMP1  = CYCCNT64 / CPU_FREQ_Hz;
    CNT_TEMP2  = CYCCNT64 - CNT_TEMP1 * CPU_FREQ_Hz;
    SysTime.s  = CNT_TEMP1;
    SysTime.ms = CNT_TEMP2 / CPU_FREQ_Hz_ms;
    CNT_TEMP3  = CNT_TEMP2 - SysTime.ms * CPU_FREQ_Hz_ms;
    SysTime.us = CNT_TEMP3 / CPU_FREQ_Hz_us;
}


/**
 * @brief 获取当前时间,单位为秒/s,即初始化后的时间
 *
 * @return float 时间轴
 */
float DWT_GetTimeline_s(void)
{
    DWT_SysTimeUpdate();
    return SysTime.s + SysTime.ms * 0.001f + SysTime.us * 0.000001f;
}


/**
 * @brief 获取当前时间,单位为毫秒/ms,即初始化后的时间
 *
 * @return float
 */
float DWT_GetTimeline_ms(void)
{
    DWT_SysTimeUpdate();
    return SysTime.s * 1000.0f + SysTime.ms + SysTime.us * 0.001f;
}

/**
 * @brief 获取当前时间,单位为微秒/us,即初始化后的时间
 *
 * @return uint64_t
 */
uint64_t DWT_GetTimeline_us(void)
{
    DWT_SysTimeUpdate();
    return (uint64_t)SysTime.s * 1000000ULL + (uint64_t)SysTime.ms * 1000ULL + SysTime.us;
}


/**
 * @brief DWT延时函数,单位为秒/s
 * @attention 该函数不受中断是否开启的影响,可以在临界区和关闭中断时使用
 * @param Delay 延时时间,单位为秒/s
 */
void DWT_Delay(float Delay)
{
    uint32_t tickstart = DWT->CYCCNT;
    uint64_t wait      = (uint64_t)(Delay * (float)CPU_FREQ_Hz);

    // 避免溢出导致的死循环
    while (((uint64_t)DWT->CYCCNT - tickstart) < wait)
    {
        // 添加编译器屏障防止循环被优化掉
       __memory_changed();
    }
}

/**
 * @brief DWT延时函数,单位为毫秒/ms
 * @attention 该函数不受中断是否开启的影响,可以在临界区和关闭中断时使用
 * @param Delay 延时时间,单位为毫秒/ms
 */
void DWT_Delay_ms(float Delay)
{
    DWT_Delay((Delay/1000.0f));
}

/**
 * @brief DWT延时函数,单位为微秒/us
 * @attention 该函数不受中断是否开启的影响,可以在临界区和关闭中断时使用
 * @param Delay 延时时间,单位为微秒/us
 */
void DWT_Delay_us(float Delay)
{
    DWT_Delay((Delay/1000000.0f));
}

/**
 * @brief DWT延时函数,单位为6ns
* @note  STM32F407 168Mhz,颗粒度为6ns
 * @attention 该函数不受中断是否开启的影响,可以在临界区和关闭中断时使用
 * @param Delay 延时时间,单位为6ns
 */
void DWT_Delay_6xns(float Delay)
{
	DWT_Delay((Delay/1000000000.0f));
}

