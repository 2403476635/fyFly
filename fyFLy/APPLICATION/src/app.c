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

_systemInfoStruct systemInfo;				/* �洢ϵͳ��Ϣ */
_flyStateStruct flyState;						/* �ɻ�״̬��Ϣ */

void sensorInit(_systemInfoStruct *info);
void infoConfig(void);

void mainApp(void)
{
	infoConfig();
	systemInfo.systemClockFrequency = HAL_RCC_GetSysClockFreq();		/* ��¼ϵͳ��ʱ�� */
	
	HAL_TIM_Base_Start(&htim5);
	
	/* SBUS �Ĵ��ڽ��� */
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);										/* ʹ�ÿ����жϽ��д���������� */
	HAL_UART_Receive_IT(&huart5, (uint8_t *)sbusBuffer, 25);				/* ����һ֡��SBUS������ */	
	
	sensorInit(&systemInfo);																				/* ��ʼ�������ϵĴ����� */
	motoInit();																											/* PWM�����ʼ�� */
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
	/* ���߽���SPI�����ϵ�����CS�� */
	HAL_GPIO_WritePin(LIS3MD_CS_GPIO_Port, LIS3MD_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ICM42670P_CS_GPIO_Port, ICM42670P_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin, GPIO_PIN_SET);
	
	/* ��ѹ�Ƴ�ʼ�� */
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
	/* ���ٶȼ� �����ǳ�ʼ�� */
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
	/* �����Ƴ�ʼ�� */			
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

/* �����ƶ� */
void emergencyBraking(_systemInfoStruct *sysInfo)
{
	if(sysInfo->flyState->flyTakeOffState == EMERGENCY_STOP)
	{
		/* ���ֹͣ����λϵͳ */
	}
}



