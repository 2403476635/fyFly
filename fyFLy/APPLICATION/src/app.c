#include "tim.h"
#include "gpio.h"
#include "main.h"
#include "w25qxx.h"
#include "usart.h"
#include "common.h"
#include "motoCtr.h"
#include "fyFilter.h"
#include "FreeRTOS.h"
#include "bsp_sbus.h"
#include "bsp_spl06.h"
#include "MahonyAHRS.h"
#include "bsp_lis3mdl.h"
#include "dataTransfer.h"
#include "fyAttitudeCtr.h"
#include "bsp_icm42670p.h"
#include "QuaternionEKF.h"
#include "fyCalibration.h"
#include "saveParameter.h"

_systemInfoStruct systemInfo;				/* 存储系统信息 */
_flyStateStruct flyState;						/* 飞机状态信息 */
_saveParameter saveParameter;				/* 保存在flash中的信息 */

void sensorInit(_systemInfoStruct *info);
void infoConfig(void);
void readInitParameter(void);

void mainApp(void)
{
	w25qxxInit();																										/* 板载FLASH初始化 */	
	readInitParameter();																						/* 读取FLASH中的参数 */
	infoConfig();																										/* 初始化系统状态信息 */	
	systemInfo.systemClockFrequency = HAL_RCC_GetSysClockFreq();		/* 记录系统的时钟 */
	
	HAL_TIM_Base_Start(&htim5);																			/* 启动定时器 */
	uartMessageBufferInit();																				/* 缓冲区初始化 */
	uartQueueInit();																								/* 串口队列初始化 */
	uartDmaReceiveInit();																						/* 串口DMA接收 */
	sensorInit(&systemInfo);																				/* 初始化板子上的传感器 */
	calibrationInit();																							/* 传感器校准参数初始化 */
	pidInit();																											/* PID参数初始化 */
	motoInit();																											/* PWM输出初始化 */
}

void infoConfig(void)
{
	systemInfo.systemClockFrequency 	= 0; 
	systemInfo.accGyroSensorInitState = ACC_GYRO_SENSOR_INIT_ERROR;
	systemInfo.baroSensorInitState 		= BARO_SENSOR_INIT_ERROR;
	systemInfo.magSensorInitState 		= MAG_SENSOR_INIT_ERROR;
	systemInfo.idleThrottlePercent    = 0.10f;
	systemInfo.idleThrottleValue      = systemInfo.idleThrottlePercent * PWM_MAX_RANGE;
	systemInfo.flyState = NULL;
	
	flyState.flyTakeOffState 						= MOTO_STOP;
	flyState.flyLockState 							= FLY_LOCK;
	flyState.rcState 										= RC_LOST;
	
	systemInfo.flyState = &flyState;
}

void sensorInit(_systemInfoStruct *info)
{
	/* 拉高接在SPI总线上的所有CS线 */
	HAL_GPIO_WritePin(LIS3MD_CS_GPIO_Port, LIS3MD_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ICM42670P_CS_GPIO_Port, ICM42670P_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin, GPIO_PIN_SET);
	
	/* 气压计初始化 */
	uint8_t tryCnt = 0;
	while(0 != spl06_Init())			
	{
		tryCnt++;
		if(tryCnt > 10)
		{
			info->baroSensorInitState = BARO_SENSOR_INIT_ERROR;
			break;
		}
		HAL_Delay(1);
	}
	info->baroSensorInitState = BARO_SENSOR_INIT_SUCCESS;
	/* 加速度计 陀螺仪初始化 */
	tryCnt = 0;
	while(0 != icm42670p_Init())				
	{
		tryCnt++;
		if(tryCnt > 10)
		{
			info->baroSensorInitState = ACC_GYRO_SENSOR_INIT_ERROR;
			break;
		}
		HAL_Delay(1);
	}
	info->baroSensorInitState = ACC_GYRO_SENSOR_INIT_SUCCESS;
	/* 磁力计初始化 */			
	tryCnt = 0;
	while(0 != lis3mdl_Init())				
	{
		tryCnt++;
		if(tryCnt > 10)
		{
			info->baroSensorInitState = MAG_SENSOR_INIT_ERROR;
			break;
		}
		HAL_Delay(1);
	}
	info->baroSensorInitState = MAG_SENSOR_INIT_SUCCESS;
	
	startAccel(800,8);		
	startGyro(800,1000);
}

/* 紧急制动 */
void emergencyBraking(_systemInfoStruct *sysInfo)
{
	if(sysInfo->flyState->flyTakeOffState == EMERGENCY_STOP)
	{
		/* 电机停止，复位系统 */
	}
}
/* 读取外部的传感器数据 */
void readImuData(inv_imu_sensor_event_t *imuReadHandle,_imuDataStruct *tempImuData)
{
	static uint8_t magSensorReadCnt = 0;
	static uint8_t barSensorReadCnt = 0;
	
	/* 读取陀螺仪和加速度计的数据 */
	getDataFromRegisters(imuReadHandle);
	
	tempImuData->accel_x = imuReadHandle->accel[0];
	tempImuData->accel_y = imuReadHandle->accel[1];
	tempImuData->accel_z = imuReadHandle->accel[2];
	
	tempImuData->gyro_x = imuReadHandle->gyro[0];
	tempImuData->gyro_y = imuReadHandle->gyro[1]; 
	tempImuData->gyro_z = imuReadHandle->gyro[2];
	tempImuData->icm42670pTemperature = (imuReadHandle->temperature / 128.0f) + 25.0f;
			
	/* 读气压计的数据 */
	if(barSensorReadCnt == 5)
	{
		spl06ReadData(&tempImuData->spl06Temperature,&tempImuData->barPressure);	
		barSensorReadCnt = 0;
	}
	/* 读磁力计计的数据 */
//	if(magSensorReadCnt == 5)
//	{
		lis3mdlRead(&tempImuData->lis3mdlTemperature,&tempImuData->mag_x,&tempImuData->mag_y,&tempImuData->mag_z);
		magSensorReadCnt = 0;
//	}
//	magSensorReadCnt++;
	barSensorReadCnt++;
}
/* 对传感器的数据进行滤波 */
void imuDataFilter(_imuDataStruct *tempImuData)
{
	accelAndGyroDataInBuffer[0] = tempImuData->accel_x;
	accelAndGyroDataInBuffer[1] = tempImuData->accel_y;
	accelAndGyroDataInBuffer[2] = tempImuData->accel_z;
	
	accelAndGyroDataInBuffer[3] = tempImuData->gyro_x;
	accelAndGyroDataInBuffer[4] = tempImuData->gyro_y;
	accelAndGyroDataInBuffer[5] = tempImuData->gyro_z;

	accelAndGyroFilter();			/* 对加速度和陀螺仪数据滤波 */
	
	tempImuData->accel_x_filter = accelAndGyroOutBuffer[0];
	tempImuData->accel_y_filter = accelAndGyroOutBuffer[1];
	tempImuData->accel_z_filter = accelAndGyroOutBuffer[2];
	
	tempImuData->gyro_x_filter = accelAndGyroOutBuffer[3];
	tempImuData->gyro_y_filter = accelAndGyroOutBuffer[4];
	tempImuData->gyro_z_filter = accelAndGyroOutBuffer[5];
}
/* 姿态角解算 */
void imuUpdate(_imuDataStruct *tempImuData)
{
	float magData[3];
	float outMagData[3] = {0,0,0};
	/* 数据校正 */
	tempImuData->gyro_x_filter -= gyroOffset[0];	/* 陀螺仪执行的是零偏校正 */
	tempImuData->gyro_y_filter -= gyroOffset[1];
	tempImuData->gyro_z_filter -= gyroOffset[2];
	
	/* 单位转换 */
#if(16 == ACCELRANGE)	
	tempImuData->accel_x_gss = tempImuData->accel_x_filter / 2048.0f;
	tempImuData->accel_y_gss = tempImuData->accel_y_filter / 2048.0f;
	tempImuData->accel_z_gss = tempImuData->accel_z_filter / 2048.0f;
#elif(8 == ACCELRANGE)
	tempImuData->accel_x_gss = tempImuData->accel_x_filter / 4096.0f;
	tempImuData->accel_y_gss = tempImuData->accel_y_filter / 4096.0f;
	tempImuData->accel_z_gss = tempImuData->accel_z_filter / 4096.0f;
#endif
	
#if(2000 == GYRORANGE)	
	tempImuData->gyro_x_deg = tempImuData->gyro_x_filter /16.4f;
	tempImuData->gyro_y_deg = tempImuData->gyro_y_filter /16.4f;
	tempImuData->gyro_z_deg = tempImuData->gyro_z_filter /16.4f;
#elif(1000 == GYRORANGE)
	tempImuData->gyro_x_deg = tempImuData->gyro_x_filter /32.8f;
	tempImuData->gyro_y_deg = tempImuData->gyro_y_filter /32.8f;
	tempImuData->gyro_z_deg = tempImuData->gyro_z_filter /32.8f;
#endif
	tempImuData->gyro_x_rad = tempImuData->gyro_x_deg /(180.0f/M_PI);
	tempImuData->gyro_y_rad = tempImuData->gyro_y_deg /(180.0f/M_PI);
	tempImuData->gyro_z_rad = tempImuData->gyro_z_deg /(180.0f/M_PI);
	
	
	
#ifdef USE_MAHONY_AHRS
	MahonyAHRSupdateIMU(tempImuData->gyro_x_rad,tempImuData->gyro_y_rad,tempImuData->gyro_z_rad,
											tempImuData->accel_x_gss,tempImuData->accel_y_gss,tempImuData->accel_z_gss);
	Mahony_computeAngles();
	tempImuData->pitch = getRoll();
	tempImuData->roll  = getPitch();
#endif
	
#ifdef USE_EKF
			accelData[0] = -tempImuData->accel_y_filter;
			accelData[1] = -tempImuData->accel_x_filter;
			accelData[2] = tempImuData->accel_z_filter;
			
			gyroData[0] = tempImuData->accel_y_filter;
			gyroData[1] = tempImuData->accel_x_filter; 
			gyroData[2] = -tempImuData->accel_z_filter;
		
			IMU_QuaternionEKF_Update(	gyroData[0] / 16.4f /57.3f,gyroData[1] / 16.4f/57.3f,gyroData[2] / 16.4f/57.3f,
																accelData[0]/2048.0f,accelData[1]/2048.0f,accelData[2]/2048.0f);									
			
			tempImuData->pitch = Get_Pitch();
			tempImuData->roll = Get_Roll();
			tempImuData->yaw = Get_Yaw();
#endif

	magData[0] = tempImuData->mag_x;
	magData[1] = tempImuData->mag_y;
	magData[2] = tempImuData->mag_z;

	magMatrixCalculate(magData,outMagData);
	tempImuData->mag_x_filter = outMagData[0];
	tempImuData->mag_y_filter = outMagData[1];
	tempImuData->mag_z_filter = outMagData[2];
	
	outMagData[0] = -outMagData[0];
	outMagData[1] = -outMagData[1];
	
	float degPitch = getDegRoll();
	float degRoll  = getDegPitch();

	float XH = outMagData[0]*cos(degPitch)+outMagData[1]*sin(degRoll)*sin(degPitch)-outMagData[2]*cos(degRoll)*sin(degPitch);
	float YH = outMagData[1]*cos(degRoll)+outMagData[2]*sin(degRoll);

//	tempImuData->yaw =  atan2(YH,XH) * 57.3f;
	tempImuData->yaw =  0;
}

void sendOriginalDataProcess(const _imuDataStruct tempImuData)
{
	if(ACCEL_CALIBRATION_MODE == systemInfo.calibrationMode)
	{
		
	}
	else if(GYRO_CALIBRATION_MODE == systemInfo.calibrationMode)
	{
		sendSensorGyroData(tempImuData);
	}
	else if(MAG_CALIBRATION_MODE == systemInfo.calibrationMode)
	{
		
	}
}

void readInitParameter(void)
{
	if(0xEF16 == W25QXX_ReadID())/* 读取参数 */
	{
		W25QXX_Read((uint8_t *)&saveParameter,0x00,sizeof(_saveParameter));
	}
	else
	{
		memset(&saveParameter,0,sizeof(_saveParameter));
	}
}


