#include "bsp_callBack.h"
#include "bsp_sbus.h"
#include "usart.h"

void HAL_UART_IdleCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == UART5) 
	{
		sbusDataAnalysis(sbusBuffer);
		HAL_UART_Receive_IT(&huart5, (uint8_t *)sbusBuffer, 25);
  }
}








