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
	sysstemInfo.sysstemClockFrequency = HAL_RCC_GetSysClockFreq();	/* 记录系统的时钟 */
	
	HAL_TIM_Base_Start(&htim5);
	
	/* SBUS 的串口接收 */
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);										/* 使用空闲中断进行处理接收数据 */
	HAL_UART_Receive_IT(&huart5, (uint8_t *)sbusBuffer, 25);				/* 接收一帧的SBUS的数据 */	
	
	/* 拉高接在SPI总线上的所有CS线 */
	HAL_GPIO_WritePin(LIS3MD_CS_GPIO_Port, LIS3MD_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ICM42670P_CS_GPIO_Port, ICM42670P_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin, GPIO_PIN_SET);
	
	spl06_Init();						/* 气压计初始化 */
	icm42670p_Init();				/* 加速度计 陀螺仪初始化 */
	lis3mdl_Init();					/* 磁力计初始化 */
	
	startAccel(800,16);		
	startGyro(800,2000);
}

