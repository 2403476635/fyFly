#ifndef __FY_PID_H__
#define __FY_PID_H__

#include "main.h"
#include "common.h"

typedef struct  
{
	float kp;								/* 比例系数 */
	float kp_feedforward;		/* 前馈比例系数 */
	float ki;								/* 积分项系数 */
	float kd;			/* 微分先行 反馈量的系数 */	
	
	float thisError;				/* 当前误差 */
	float lastError;				/* 上次的误差 */
	float lastFeedBack;			/* 上次的反馈量 */
	float integralError;		/* 积分误差 */
	
	float kpOutValue;				/* 计算的微分 */
	float kiOutValue;				/* 计算的积分 */
	float kdOutValue;				/* 计算的微分 */
	
	float pidOutValue;			/* PID总输出 */
}__attribute__((packed)) _pid;



void pidCalculate(_pid *tempPid, float target, float feedforwardValue, float feedbackValue, float errorLimt, float integralLimt, float dt);

											
#endif /* __FY_PID_H__ */




