#ifndef __SAVE_PARAMETER_H__
#define __SAVE_PARAMETER_H__

#include "common.h"
#include "fyPid.h"

typedef struct 
{
	/* �ڻ��Ĳ��� */
	float innerRingPitchPidKp_feedForward;
	float innerRingPitchPidKp;
	float innerRingPitchPidKi;
	float innerRingPitchPidKd;
	
	float innerRingRollPidKp_feedForward;
	float innerRingRollPidKp;
	float innerRingRollPidKi;
	float innerRingRollPidKd;
	
	float innerRingYawPidKp_feedForward;
	float innerRingYawPidKp;
	float innerRingYawPidKi;
	float innerRingYawPidKd;
	/* �⻷�Ĳ��� */
	float outerRingPitchPidKp_feedForward;
	float outerRingPitchPidKp;
	float outerRingPitchPidKi;
	float outerRingPitchPidKd;
	
	float outerRingRollPidKp_feedForward;
	float outerRingRollPidKp;
	float outerRingRollPidKi;
	float outerRingRollPidKd;
	
	float outerRingYawPidKp_feedForward;
	float outerRingYawPidKp;
	float outerRingYawPidKi;
	float outerRingYawPidKd;
	/* ����������� */
	float gyroOffset[3];
	
	float accelOffset[3];
	float accelCalibrationMatrix3x3[9];
		
	float magOffset[3];
	float magCalibrationMatrix3x3[9];
	
}__attribute__((packed)) _saveParameter;

extern _saveParameter saveParameter;

void saveParameterHandle(void);

#endif /* __SAVE_PARAMETER_H__ */




