#ifndef __FY_ATTITUDE_CTR_H__
#define __FY_ATTITUDE_CTR_H__

#include "main.h"
#include "fyPid.h"
#include "common.h"


typedef struct  
{
	/* 目标角速度 */
	float targetPitchAngularVelocity;
	float targetRollAngularVelocity;
	float targetYawAngularVelocity;
	/* 当前角速度反馈*/
	float feedbackPitchAngularVelocity;
	float feedbackRollAngularVelocity;
	float feedbackYawAngularVelocity;
	/* 目标角度 */
	float targetPitchAngle;	
	float targetRollAngle;
	float targetYawAngle;
	/* 当前角度反馈*/
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




