#include "fyAttitudeCtr.h"
#include "saveParameter.h"

_pid angularVelocityPitchPid;
_pid angularVelocityRollPid;
_pid angularVelocityYawPid;

_pid anglePitchPid;
_pid angleRollPid;
_pid angleYawPid;

static _attitudeControlParameterStruct angularVelocityParameter;
static _attitudeControlParameterStruct angleParameter;

/* PID参数初始化 */
void pidInit(void)
{
	/* 角速度环PID初始化 */
	angularVelocityPitchPid.kp = saveParameter.innerRingPitchPidKp;
	angularVelocityPitchPid.kp_feedforward = 0;
	angularVelocityPitchPid.ki = saveParameter.innerRingPitchPidKi;
	angularVelocityPitchPid.kd = saveParameter.innerRingPitchPidKd; 
	
	angularVelocityRollPid.kp = saveParameter.innerRingRollPidKp;
	angularVelocityRollPid.kp_feedforward = 0;
	angularVelocityRollPid.ki = saveParameter.innerRingRollPidKi;
	angularVelocityRollPid.kd = saveParameter.innerRingRollPidKd; 
	
	angularVelocityYawPid.kp = saveParameter.innerRingYawPidKp;
	angularVelocityYawPid.kp_feedforward = 0;
	angularVelocityYawPid.ki = saveParameter.innerRingYawPidKi;
	angularVelocityYawPid.kd = saveParameter.innerRingYawPidKd; 
	/* 角度环PID初始化 */
	anglePitchPid.kp = saveParameter.outerRingPitchPidKp;
	anglePitchPid.ki = saveParameter.outerRingPitchPidKi;
	anglePitchPid.kd = saveParameter.outerRingPitchPidKd;
	
	angleRollPid.kp = saveParameter.outerRingPitchPidKp;
	angleRollPid.ki = saveParameter.outerRingPitchPidKi;
	angleRollPid.kd = saveParameter.outerRingPitchPidKd;
	
	angleYawPid.kp = saveParameter.outerRingPitchPidKp;
	angleYawPid.ki = saveParameter.outerRingPitchPidKi;
	angleYawPid.kd = saveParameter.outerRingPitchPidKd;
}

/* 角速度环控制 */
void angularVelocityControl(const _imuDataStruct tempImuData, float dtMs)
{
	/* 设定目标角速度 */
	angularVelocityParameter.targetPitchAngularVelocity = anglePitchPid.pidOutValue;
	angularVelocityParameter.targetRollAngularVelocity  = angleRollPid.pidOutValue;
	angularVelocityParameter.targetYawAngularVelocity   = angleYawPid.pidOutValue;
	/* 目标限幅 */
	angularVelocityParameter.targetPitchAngularVelocity = LIMT(angularVelocityParameter.targetPitchAngularVelocity,-MAX_ANGULAR_VELOCITY,MAX_ANGULAR_VELOCITY);
	angularVelocityParameter.targetRollAngularVelocity  = LIMT(angularVelocityParameter.targetRollAngularVelocity,-MAX_ANGULAR_VELOCITY,MAX_ANGULAR_VELOCITY);
	/* 当前角速度反馈 */
	angularVelocityParameter.feedbackPitchAngularVelocity = tempImuData.gyro_x_filter;
	angularVelocityParameter.feedbackRollAngularVelocity  = tempImuData.gyro_y_filter;
	angularVelocityParameter.feedbackYawAngularVelocity   = tempImuData.gyro_z_filter;
	/* PID计算 */
	pidCalculate(	&angularVelocityPitchPid,
								angularVelocityParameter.targetPitchAngularVelocity,
								0,
								angularVelocityParameter.feedbackPitchAngularVelocity,
								5,
								200,
								500,
								dtMs/1000.0f
								);
	pidCalculate(&angularVelocityRollPid,
								angularVelocityParameter.targetRollAngularVelocity,
								0,
								angularVelocityParameter.feedbackRollAngularVelocity,
								5,
								200,
								500,
								dtMs/1000.0f
								);
	pidCalculate(&angularVelocityYawPid,
								angularVelocityParameter.targetYawAngularVelocity,
								0,
								angularVelocityParameter.feedbackYawAngularVelocity,
								5,
								200,
								2000,
								dtMs/1000.0f
								);
	/* PID输出限幅 */
	angularVelocityPitchPid.pidOutValue = LIMT(angularVelocityPitchPid.pidOutValue, -MAX_ANGULAR_VELOCITY_PID_OUT, MAX_ANGULAR_VELOCITY_PID_OUT);
	angularVelocityRollPid.pidOutValue  = LIMT(angularVelocityRollPid.pidOutValue, -MAX_ANGULAR_VELOCITY_PID_OUT, MAX_ANGULAR_VELOCITY_PID_OUT);
	angularVelocityYawPid.pidOutValue   = LIMT(angularVelocityYawPid.pidOutValue, -MAX_ANGULAR_VELOCITY_PID_OUT, MAX_ANGULAR_VELOCITY_PID_OUT);
}
/* 角度环控制 */
void angleControl(const int16_t *channel,const _imuDataStruct tempImuData, float dtMs)
{
	angleParameter.targetPitchAngle = -(float)channel[CH_PITCH] / 500.0f * 30.0f;	/* 遥控器摇杆值转换到当前0~10° */
	angleParameter.feedbackPitchAngle = tempImuData.pitch;
	
	angleParameter.targetRollAngle = (float)channel[CH_ROLL] / 500.0f * 30.0f;	/* 遥控器摇杆值转换到当前0~10° */
	angleParameter.feedbackRollAngle = tempImuData.roll;
	
	angleParameter.targetYawAngle = -(float)channel[CH_YAW] / 500.0f * 30.0f;	/* 遥控器摇杆值转换到当前0~10° */
	angleParameter.feedbackYawAngle = tempImuData.yaw;
	
	/* PID计算 */
	pidCalculate(	&anglePitchPid,
								angleParameter.targetPitchAngle,
								0,
								angleParameter.feedbackPitchAngle,
								0,
								200,
								500,
								dtMs/1000.0f
								);
	
	pidCalculate(	&angleRollPid,
								angleParameter.targetRollAngle,
								0,
								angleParameter.feedbackRollAngle,
								0,
								200,
								500,
								dtMs/1000.0f
								);
								
	pidCalculate(	&angleYawPid,
								angleParameter.targetYawAngle,
								0,
								angleParameter.feedbackYawAngle,
								0,
								200,
								500,
								dtMs/1000.0f
								);							
}

