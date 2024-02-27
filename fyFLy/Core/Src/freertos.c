/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "spi.h"
#include "icm42670p.h"
#include "spl06.h"
#include "ringBuffer.h"
#include "common.h"
#include "usbd_cdc_if.h"
#include "dataTransfer.h"
#include "lis3mdl.h"
#include "MahonyAHRS.h"
#include "kalman_filter.h"
#include "QuaternionEKF.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint8_t spl06Id = 0;
uint8_t magId = 0;

_imuDataStruct imuData;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
	HAL_GPIO_WritePin(LIS3MD_CS_GPIO_Port, LIS3MD_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ICM42670P_CS_GPIO_Port, ICM42670P_CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin, GPIO_PIN_SET);
	
	Drv_Spl0601Init();
	
	icm42670p_Init();
	lis3mdl_Init();
//	
	startAccel(1600,16);
	startGyro(1600,2000);
	inv_imu_sensor_event_t imu_event;
	
	Mahony_Init(1000);
	IMU_QuaternionEKF_Init(10, 0.001, 10000000, 1, 0.001f,0); //ekf��ʼ��
	
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(1);  // �������������Ƶ��

	// ��ʼ��xLastWakeTimeΪ��ǰ��ϵͳ���ļ���
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
//		HAL_GPIO_TogglePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin);
//		inv_imu_sleep_us(10);
			
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
			getDataFromRegisters(&imu_event);
			
			spl06ReadData(&imuData.spl06Temperature,&imuData.barPressure);	/* ����ѹ�Ƶ�ԭʼ���� */
		
			imuData.icm42670pTemperature = (imu_event.temperature / 128.0f) + 25.0f;
			imuData.accel_x = imu_event.accel[0];
			imuData.accel_y = imu_event.accel[1];
			imuData.accel_z = imu_event.accel[2];
			
			imuData.gyro_x = imu_event.gyro[0];
			imuData.gyro_y = imu_event.gyro[1]; 
			imuData.gyro_z = imu_event.gyro[2];
		
			lis3mdlRead(&imuData.lis3mdlTemperature,&imuData.mag_x,&imuData.mag_y,&imuData.mag_z);
		
			MahonyAHRSupdateIMU(imuData.gyro_x / 16.4f /57.3f,imuData.gyro_y / 16.4f/57.3f,imuData.gyro_z / 16.4f/57.3f,imuData.accel_x/2048.0f,imuData.accel_y/2048.0f,imuData.accel_z/2048.0f);
		
			IMU_QuaternionEKF_Update(imuData.gyro_x / 16.4f /57.3f,imuData.gyro_y / 16.4f/57.3f,imuData.gyro_z / 16.4f/57.3f,imuData.accel_x/2048.0f,imuData.accel_y/2048.0f,imuData.accel_z/2048.0f);
//		
			Mahony_computeAngles();
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
//			computeAngles();
		
//			sendImuData(imuData);																						/* ����λ���������� */
		
			vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void mainApp(void)
{
	
}
#if defined(USB_DEBUG)
/*
	vcpDataAnalysis(_RingBuffer *ringbuffer)
	����:����USB���⴮�ڽ��յ�����
*/
static void vcpDataAnalysis(_RingBuffer *ringbuffer)
{
	char logBuffer[100];
	unsigned char *RxFrameData;/*�洢����֡�Ŀռ�*/
	unsigned short FrameDataLen = ringbuffer->Lenght;
	if(xPortGetFreeHeapSize() >= FrameDataLen)
	{
		RxFrameData = (unsigned char *)pvPortMalloc(FrameDataLen);	/*Ϊ��ȡ�������е��������������ڴ�*/
		if(ReadBytes(ringbuffer,RxFrameData,FrameDataLen) == 1)		/*��ȡ�������е���������*/
		{
			/*��ȡ��������������*/
			for(int i=0;i<FrameDataLen;i++)	/*����*/
			{
				if(RxFrameData[i] == 0xAA)			/*��֡ͷ*/
				{
					if(FrameDataLen - i <= 3)		/*֡ͷ���������ݳ���*/
					{
						WriteBytes(ringbuffer,&RxFrameData[i],FrameDataLen - i);/*δ���������д�ػ�����*/
						vPortFree(RxFrameData);  //�ͷ��ڴ�
						return ;/*���ؿ�����*/
					}
					
					unsigned short FrameLen = RxFrameData[i+3];			/*�õ�֡�������ݳ���*/
					
					if((FrameDataLen - i) >= (4 + FrameLen + 2))	/*ʣ�������Ƿ���һ֡�ĳ���*/
					{
						if(FrameDataCheck( &RxFrameData[i] ) == 1)		/*У��֡����*/
						{
							unsigned short OneFrameDataLen = 4 + FrameLen + 2;			/*����֡�ܳ�*/
							if(RxFrameData[i+1] == PC)														/*����λ�����͵�����*/
							{
								switch (RxFrameData[i+2])
								{
										case CMD_SHOW_DEVICE_INFO:
										{
//												showDeviceInfo();
												break;
										}
										case CMD_RESET_DEVICE:
										{
												sendLogMessage(0xAA,MCU,CMD_LOG_MESSAGE,"\xe8\xae\xbe\xe5\xa4\x87\xe9\x87\x8d\xe5\x90\xaf");
												HAL_Delay(100);
												/* ϵͳ��λ */
												NVIC_SystemReset(); 
												break;
										}
//										case CMD_RUN_IAP:               /* ����IAP���� */
//										{
//											sendLogMessage(0xAA,MCU,CMD_LOG_MESSAGE,"\xe8\xae\xbe\xe5\xa4\x87\xe9\x87\x8d\xe5\x90\xaf");
//											
//											bsp_EraseCpuFlash((uint32_t)ADDR_FLASH_SECTOR_7_BANK2);
//		
//											deviceAttributeInfo.iapState = IAP_RUN_STATE;
//											
//											bsp_WriteCpuFlash(ADDR_FLASH_SECTOR_7_BANK2,(uint8_t *)deviceAttributeInfo.wifiName,sizeof(deviceAttributeInfo));
//											
//											/* ϵͳ��λ */
//											 NVIC_SystemReset(); //��λ
//											break;
//										}


										default:
												break;
								}
							}
							i += OneFrameDataLen-1;										/*ƫ��*/
						}
					}
					else
					{
						WriteBytes(ringbuffer,&RxFrameData[i],FrameDataLen - i);/*δ���������д�ػ�����*/
						vPortFree(RxFrameData);  //�ͷ��ڴ�
						return ;
					}
				}
			}
		}
		vPortFree(RxFrameData);  //�ͷ��ڴ�
	}
}
#endif

/* USER CODE END Application */

