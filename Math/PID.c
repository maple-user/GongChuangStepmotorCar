#include "PID.h"

#define MINNUM       0.00001f        //定义最小启用积分项的Ki值
#define PID_INT_MAX  450             //定义PID积分项限幅值，防止积分项过大导致积分饱和
#define PID_LIMIT    100.0f          //定义PID输出限幅值

#define PID_TIM_HANDLE        &htim1 //PID算法定时器依赖
#define PID_AGL_DEADBAND      0.6f   //PID角度环死区，单位为度

PID_TypeDef PID_AglControl = {0};    //PID角度环句柄初始化
PID_TypeDef PID_PosControl = {0};    //PID位置环句柄初始化


/**
  * @brief  PID任务句柄初始化
  * @param  PID_TypeDef *PID: PID句柄地址
  * @param  Kp: 比例系数
  * @param  Ki: 积分系数
  * @param  Kd: 微分系数
  * @param  Kw: 前馈系数
  * @retval 无
  */
void PID_Init(PID_TypeDef *PID, float Kp, float Ki, float Kd, float Kw ,float Target, float PID_Limit)
{
	assert_param(PID != NULL);
	
	PID ->Kp = Kp;
	PID ->Ki = Ki;
	PID ->Kd = Kd;
	PID ->Kw = Kw;

	PID ->Target = Target;
	
	PID ->PID_Limit = PID_Limit;
	
	PID ->Out = 0.0f;
	PID ->IS_ENABLE = false;
	PID ->IS_ARRIVE = true;
}



/**
  * @brief  PID角度环计算任务
  * @param  PID_TypeDef *PID: PID句柄地址
  * @retval 无
  */
void PID_Angle_Calculate (PID_TypeDef *PID)
{
	assert_param(PID != NULL);
	
	PID->Error1=PID->Error0;
	PID->Error0=PID->Target - PID->Actualv;
	
	if(PID->Error0 > 180.0f)
	{
		PID->Error0 -= 360.0f;
	}
	if(PID->Error0 < -180.0f)
	{
		PID->Error0 += 360.0f;
	}
    /***** 角度环死区 *****/
	if(fabs(PID->Error0) < PID_AGL_DEADBAND)
	{
		PID->Error0 = 0.0f;
	}

	/***** 防止积分饱和 *****/
	if(fabs(PID->Ki) > MINNUM)
	{
		PID->ErrorInt += PID->Error0;
	}
    /***** 积分限幅 *****/
	PID -> ErrorInt = PID -> ErrorInt > PID_INT_MAX ? PID_INT_MAX : (PID -> ErrorInt < -PID_INT_MAX ? -PID_INT_MAX : PID -> ErrorInt); //积分项限幅
	
	PID->Out=(PID->Kp)*(PID->Error0)+(PID->Ki)*(PID->ErrorInt)+(PID->Kd)*((PID->Error0)-(PID->Error1)) + (PID->Kw)*(PID->Actualw);
    
	
	/***** 输出限幅 *****/
	if(PID->Out > PID->PID_Limit){PID->Out = PID->PID_Limit;}
    if(PID->Out < -(PID->PID_Limit)){PID->Out = -(PID->PID_Limit);}

	
}

/**
  * @brief  PID计算任务
  * @param  PID_TypeDef *PID: PID句柄地址
  * @retval 无
  */
void PID_Calculate (PID_TypeDef *PID)
{
	assert_param(PID != NULL);
	
	PID->Error1=PID->Error0;
	PID->Error0=PID->Target - PID->Actualv;
	

	/***** 防止积分饱和 *****/
	if(fabs(PID->Ki) > MINNUM)
	{
		PID->ErrorInt += PID->Error0;
	}
    /***** 积分限幅 *****/
	PID -> ErrorInt = PID -> ErrorInt > PID_INT_MAX ? PID_INT_MAX : (PID -> ErrorInt < -PID_INT_MAX ? -PID_INT_MAX : PID -> ErrorInt); //积分项限幅
	
	PID->Out=(PID->Kp)*(PID->Error0)+(PID->Ki)*(PID->ErrorInt)+(PID->Kd)*((PID->Error0)-(PID->Error1));
    
	
	/***** 输出限幅 *****/
	if(PID->Out > PID->PID_Limit){PID->Out = PID->PID_Limit;}
    if(PID->Out < -(PID->PID_Limit)){PID->Out = -(PID->PID_Limit);}

	
}

/**
  * @brief  PID算法定时器依赖使能
  * @param  PID_TypeDef *PID: PID句柄地址
  * @retval 无
  */
void PID_Start(void)
{
    HAL_TIM_Base_Start_IT(PID_TIM_HANDLE);
}

/**
  * @brief  PID算法定时器依赖失能
  * @param  PID_TypeDef *PID: PID句柄地址
  * @retval 无
  */
void PID_Stop(void)
{
    HAL_TIM_Base_Stop_IT(PID_TIM_HANDLE);
}


