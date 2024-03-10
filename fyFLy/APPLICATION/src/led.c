#include "main.h"

#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include "bsp_sbus.h"
#include "bsp_spl06.h"
#include "bsp_lis3mdl.h"
#include "bsp_icm42670p.h"

void mainApp(void)
{
	sysstemInfo.sysstemClockFrequency = HAL_RCC_GetSysClockFreq();	/* ��¼ϵͳ��ʱ�� */
	
	HAL_TIM_Base_Start(&htim5);
	
	/* SBUS �Ĵ��ڽ��� */
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);										/* ʹ�ÿ����жϽ��д���������� */
	HAL_UART_Receive_IT(&huart5, (uint8_t *)sbusBuffer, 25);				/* ����һ֡��SBUS������ */	
	
	/* ���߽���SPI�����ϵ�����CS�� */
	HAL_GPIO_WritePin(LIS3MD_CS_GPIO_Port, LIS3MD_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ICM42670P_CS_GPIO_Port, ICM42670P_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin, GPIO_PIN_SET);
	
	spl06_Init();						/* ��ѹ�Ƴ�ʼ�� */
	icm42670p_Init();				/* ���ٶȼ� �����ǳ�ʼ�� */
	lis3mdl_Init();					/* �����Ƴ�ʼ�� */
	
	startAccel(800,16);		
	startGyro(800,2000);
}

