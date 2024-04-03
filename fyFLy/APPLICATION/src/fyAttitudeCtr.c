#include "fyAttitudeCtr.h"
#include "saveParameter.h"

_pid angularVelocityPitchPid;
_pid angularVelocityRollPid;
_pid angularVelocityYawPid;

_attitudeControlParameterStruct angularVelocityParameter;
/* PID������ʼ�� */
void pidInit(void)
{
	/* ���ٶȻ�PID��ʼ�� */
	angularVelocityPitchPid.kp = saveParameter.saveAngularVelocityPitchPid.kp;
	angularVelocityPitchPid.kp_feedforward = 0;
	angularVelocityPitchPid.ki = saveParameter.saveAngularVelocityPitchPid.ki;
	angularVelocityPitchPid.kd = saveParameter.saveAngularVelocityPitchPid.kd; 
	
	angularVelocityRollPid.kp = saveParameter.saveAngularVelocityRollPid.kp;
	angularVelocityRollPid.kp_feedforward = 0;
	angularVelocityRollPid.ki = saveParameter.saveAngularVelocityRollPid.ki;
	angularVelocityRollPid.kd = saveParameter.saveAngularVelocityRollPid.kd; 
	
	angularVelocityYawPid.kp = saveParameter.saveAngularVelocityYawPid.kp;
	angularVelocityYawPid.kp_feedforward = 0;
	angularVelocityYawPid.ki = saveParameter.saveAngularVelocityYawPid.ki;
	angularVelocityYawPid.kd = saveParameter.saveAngularVelocityYawPid.kd; 
}
/* PID�������� */
void setPidParameter(_pid *destinationPid,const _pid sourcePid)
{
	destinationPid->kp = sourcePid.kp;
	destinationPid->ki = sourcePid.ki;
	destinationPid->kd = sourcePid.kd; 
}

/* ���ٶȻ����� */
void angularVelocityControl(const _imuDataStruct tempImuData, float dtMs)
{
	/* �趨Ŀ����ٶ� */
	angularVelocityParameter.targetPitchAngularVelocity = 0;
	angularVelocityParameter.targetRollAngularVelocity  = 0;
	angularVelocityParameter.targetYawAngularVelocity   = 0;
	/* Ŀ���޷� */
	angularVelocityParameter.targetPitchAngularVelocity = LIMT(angularVelocityParameter.targetPitchAngularVelocity,-MAX_ANGULAR_VELOCITY,MAX_ANGULAR_VELOCITY);
	angularVelocityParameter.targetRollAngularVelocity  = LIMT(angularVelocityParameter.targetRollAngularVelocity,-MAX_ANGULAR_VELOCITY,MAX_ANGULAR_VELOCITY);
	/* ��ǰ���ٶȷ��� */
	angularVelocityParameter.feedbackPitchAngularVelocity = tempImuData.gyro_x_filter;
	angularVelocityParameter.feedbackRollAngularVelocity  = tempImuData.gyro_y_deg;
	angularVelocityParameter.feedbackYawAngularVelocity   = tempImuData.gyro_z_deg;
	/* PID���� */
	pidCalculate(	&angularVelocityPitchPid,
								angularVelocityParameter.targetPitchAngularVelocity,
								0,
								angularVelocityParameter.feedbackPitchAngularVelocity,
								200,
								500,
								dtMs/1000.0f
								);
	pidCalculate(&angularVelocityRollPid,
								angularVelocityParameter.targetRollAngularVelocity,
								0,
								angularVelocityParameter.feedbackRollAngularVelocity,
								200,
								500,
								dtMs/1000.0f
								);
	pidCalculate(&angularVelocityYawPid,
								angularVelocityParameter.targetYawAngularVelocity,
								0,
								angularVelocityParameter.feedbackYawAngularVelocity,
								200,
								2000,
								dtMs/1000.0f
								);
	/* PID����޷� */
	angularVelocityPitchPid.pidOutValue = LIMT(angularVelocityPitchPid.pidOutValue, -MAX_ANGULAR_VELOCITY_PID_OUT, MAX_ANGULAR_VELOCITY_PID_OUT);
	angularVelocityRollPid.pidOutValue  = LIMT(angularVelocityRollPid.pidOutValue, -MAX_ANGULAR_VELOCITY_PID_OUT, MAX_ANGULAR_VELOCITY_PID_OUT);
	angularVelocityYawPid.pidOutValue   = LIMT(angularVelocityYawPid.pidOutValue, -MAX_ANGULAR_VELOCITY_PID_OUT, MAX_ANGULAR_VELOCITY_PID_OUT);
	
}


