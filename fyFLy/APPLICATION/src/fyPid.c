#include "fyPid.h"

void pidCalculate(	_pid *tempPid, 
										float target, 					/* ����ֵ */
										float feedforwardValue, /* ǰ���� */
										float feedbackValue,		/* ������ */
										float errorMinLimt,			/* ����޷� */
										float errorMaxLimt,			/* ����޷� */
										float integralLimt,			/* �����޷� */
										float dt_s)							/* ���� s */
{
	float hz = 1.0f/dt_s;
	/* P */
	tempPid->thisError = target - feedbackValue;	/* ������� */
	tempPid->kpOutValue = tempPid->kp * tempPid->thisError;
	/* I */
	if(ABS(tempPid->thisError) > ABS(errorMinLimt))
	{
		tempPid->integralError = tempPid->integralError + LIMT(tempPid->thisError, -errorMaxLimt,errorMaxLimt) * dt_s;	/* �ۼ���ʹ��������޷����൱�ڻ��ַ���Ĳ��ֲ��� */
		tempPid->kiOutValue = tempPid->ki * tempPid->integralError;
		tempPid->kiOutValue = LIMT(tempPid->kiOutValue ,-integralLimt,integralLimt);
	}
	/* D */
	tempPid->kdOutValue = tempPid->kd * (feedbackValue - tempPid->lastFeedBack) * hz;	/* ��������΢�� */
	
	tempPid->pidOutValue = 	tempPid->kp_feedforward * feedforwardValue + 
													tempPid->kpOutValue + 
													tempPid->kiOutValue + 
													tempPid->kdOutValue;
	
	tempPid->lastFeedBack = feedbackValue;
}




