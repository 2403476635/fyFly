#include "fyPid.h"

void pidCalculate(	_pid *tempPid, 
										float target, 					/* 期望值 */
										float feedforwardValue, /* 前馈量 */
										float feedbackValue,		/* 反馈量 */
										float errorMinLimt,			/* 误差限幅 */
										float errorMaxLimt,			/* 误差限幅 */
										float integralLimt,			/* 积分限幅 */
										float dt_s)							/* 周期 s */
{
	float hz = 1.0f/dt_s;
	/* P */
	tempPid->thisError = target - feedbackValue;	/* 计算误差 */
	tempPid->kpOutValue = tempPid->kp * tempPid->thisError;
	/* I */
	if(ABS(tempPid->thisError) > ABS(errorMinLimt))
	{
		tempPid->integralError = tempPid->integralError + LIMT(tempPid->thisError, -errorMaxLimt,errorMaxLimt) * dt_s;	/* 累计误差，使用了误差限幅，相当于积分分离的部分操作 */
		tempPid->kiOutValue = tempPid->ki * tempPid->integralError;
		tempPid->kiOutValue = LIMT(tempPid->kiOutValue ,-integralLimt,integralLimt);
	}
	/* D */
	tempPid->kdOutValue = tempPid->kd * (feedbackValue - tempPid->lastFeedBack) * hz;	/* 反馈量的微分 */
	
	tempPid->pidOutValue = 	tempPid->kp_feedforward * feedforwardValue + 
													tempPid->kpOutValue + 
													tempPid->kiOutValue + 
													tempPid->kdOutValue;
	
	tempPid->lastFeedBack = feedbackValue;
}




