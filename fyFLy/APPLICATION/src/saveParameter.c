#include "saveParameter.h"
#include "fyAttitudeCtr.h"
#include "w25qxx.h"
#include "fyCalibration.h"

void saveParameterHandle(void)
{
	/* 内环 */
	saveParameter.innerRingPitchPidKp = angularVelocityPitchPid.kp;
	saveParameter.innerRingPitchPidKi = angularVelocityPitchPid.ki;
	saveParameter.innerRingPitchPidKd = angularVelocityPitchPid.kd;
	
	saveParameter.innerRingRollPidKp = angularVelocityRollPid.kp;
	saveParameter.innerRingRollPidKi = angularVelocityRollPid.ki;
	saveParameter.innerRingRollPidKd = angularVelocityRollPid.kd;
	
	saveParameter.innerRingYawPidKp = angularVelocityYawPid.kp;
	saveParameter.innerRingYawPidKi = angularVelocityYawPid.ki;
	saveParameter.innerRingYawPidKd = angularVelocityYawPid.kd;
	/* 外环 */
	saveParameter.outerRingPitchPidKp = anglePitchPid.kp;
	saveParameter.outerRingPitchPidKi = anglePitchPid.ki;
	saveParameter.outerRingPitchPidKd = anglePitchPid.kd;
	
	saveParameter.outerRingRollPidKp = angleRollPid.kp;
	saveParameter.outerRingRollPidKi = angleRollPid.ki;
	saveParameter.outerRingRollPidKd = angleRollPid.kd;
	
	saveParameter.outerRingYawPidKp = angleYawPid.kp;
	saveParameter.outerRingYawPidKi = angleYawPid.ki;
	saveParameter.outerRingYawPidKd = angleYawPid.kd;
	/* 校正参数 */
	for(uint8_t i=0;i<3;i++) saveParameter.gyroOffset[i] = gyroOffset[i];
	
	for(uint8_t i=0;i<3;i++) saveParameter.accelOffset[i] = accelOffset[i];
	for(uint8_t i=0;i<9;i++) saveParameter.accelCalibrationMatrix3x3[i] = accelCalibrationMatrix3x3[i];
	
	for(uint8_t i=0;i<3;i++) saveParameter.magOffset[i] = magOffset[i];
	for(uint8_t i=0;i<9;i++) saveParameter.magCalibrationMatrix3x3[i] = magCalibrationMatrix3x3[i];
	
	W25QXX_Write((uint8_t *)&saveParameter,0,sizeof(_saveParameter));			/* 写入FLASH */
	W25QXX_Read((uint8_t *)&saveParameter,0x00,sizeof(_saveParameter));		/* 读取数据 */
}
