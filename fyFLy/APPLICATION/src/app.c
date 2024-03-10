#include "tim.h"
#include "gpio.h"
#include "main.h"
#include "usart.h"
#include "common.h"
#include "motoCtr.h"
#include "FreeRTOS.h"
#include "bsp_sbus.h"
#include "bsp_spl06.h"
#include "bsp_lis3mdl.h"
#include "bsp_icm42670p.h"

_systemInfoStruct systemInfo;				/* 存储系统信息 */
_flyStateStruct flyState;						/* 飞机状态信息 */

void sensorInit(_systemInfoStruct *info);
void infoConfig(void);

void mainApp(void)
{
	infoConfig();
	systemInfo.systemClockFrequency = HAL_RCC_GetSysClockFreq();		/* 记录系统的时钟 */
	
	HAL_TIM_Base_Start(&htim5);
	
	/* SBUS 的串口接收 */
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);										/* 使用空闲中断进行处理接收数据 */
	HAL_UART_Receive_IT(&huart5, (uint8_t *)sbusBuffer, 25);				/* 接收一帧的SBUS的数据 */	
	
	sensorInit(&systemInfo);																				/* 初始化板子上的传感器 */
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
	
	startAccel(800,16);		
	startGyro(800,2000);
}

/* 紧急制动 */
void emergencyBraking(_systemInfoStruct *sysInfo)
{
	if(sysInfo->flyState->flyTakeOffState == EMERGENCY_STOP)
	{
		/* 电机停止，复位系统 */
	}
}



