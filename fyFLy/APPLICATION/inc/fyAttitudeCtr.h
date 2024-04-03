#ifndef __FY_ATTITUDE_CTR_H__
#define __FY_ATTITUDE_CTR_H__

#include "main.h"
#include "fyPid.h"
#include "common.h"


typedef struct  
{
	/* 目标角度 */
	float targetPitchAngle;	
	float targetRollAngle;
	float targetYawAngle;
	/* 目标角速度 */
	float targetPitchAngularVelocity;
	float targetRollAngularVelocity;
	float targetYawAngularVelocity;
	/* 当前角速度反馈*/
	float feedbackPitchAngularVelocity;
	float feedbackRollAngularVelocity;
	float feedbackYawAngularVelocity;
}_attitudeControlParameterStruct;

extern _pid angularVelocityPitchPid;
extern _pid angularVelocityRollPid;
extern _pid angularVelocityYawPid;

void pidInit(void);
void setPidParameter(_pid *destinationPid,const _pid sourcePid);
void angularVelocityControl(const _imuDataStruct tempImuData, float dtMs);


#endif /* __FY_ATTITUDE_CTR_H__ */




