#ifndef __FY_PID_H__
#define __FY_PID_H__

#include "main.h"
#include "common.h"

typedef struct  
{
	float kp;								/* ����ϵ�� */
	float kp_feedforward;		/* ǰ������ϵ�� */
	float ki;								/* ������ϵ�� */
	float kd;			/* ΢������ ��������ϵ�� */	
	
	float thisError;				/* ��ǰ��� */
	float lastError;				/* �ϴε���� */
	float lastFeedBack;			/* �ϴεķ����� */
	float integralError;		/* ������� */
	
	float kpOutValue;				/* �����΢�� */
	float kiOutValue;				/* ����Ļ��� */
	float kdOutValue;				/* �����΢�� */
	
	float pidOutValue;			/* PID����� */
}__attribute__((packed)) _pid;



void pidCalculate(_pid *tempPid, float target, float feedforwardValue, float feedbackValue, float errorLimt, float integralLimt, float dt);

											
#endif /* __FY_PID_H__ */




