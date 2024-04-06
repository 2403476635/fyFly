#ifndef __FY_ATTITUDE_CTR_H__
#define __FY_ATTITUDE_CTR_H__

#include "main.h"
#include "fyPid.h"
#include "common.h"


typedef struct  
{
	/* Ŀ����ٶ� */
	float targetPitchAngularVelocity;
	float targetRollAngularVelocity;
	float targetYawAngularVelocity;
	/* ��ǰ���ٶȷ���*/
	float feedbackPitchAngularVelocity;
	float feedbackRollAngularVelocity;
	float feedbackYawAngularVelocity;
	/* Ŀ��Ƕ� */
	float targetPitchAngle;	
	float targetRollAngle;
	float targetYawAngle;
	/* ��ǰ�Ƕȷ���*/
	float feedbackPitchAngle;
	float feedbackRollAngle;
	float feedbackYawAngle;
}_attitudeControlParameterStruct;

extern _pid angularVelocityPitchPid;
extern _pid angularVelocityRollPid;
extern _pid angularVelocityYawPid;

extern _pid anglePitchPid;
extern _pid angleRollPid;
extern _pid angleYawPid;

void pidInit(void);
void angularVelocityControl(const _imuDataStruct tempImuData, float dtMs);
void angleControl(const int16_t *channel,const _imuDataStruct tempImuData, float dtMs);

#endif /* __FY_ATTITUDE_CTR_H__ */




