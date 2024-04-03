#ifndef __FY_ATTITUDE_CTR_H__
#define __FY_ATTITUDE_CTR_H__

#include "main.h"
#include "fyPid.h"
#include "common.h"


typedef struct  
{
	/* Ŀ��Ƕ� */
	float targetPitchAngle;	
	float targetRollAngle;
	float targetYawAngle;
	/* Ŀ����ٶ� */
	float targetPitchAngularVelocity;
	float targetRollAngularVelocity;
	float targetYawAngularVelocity;
	/* ��ǰ���ٶȷ���*/
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




