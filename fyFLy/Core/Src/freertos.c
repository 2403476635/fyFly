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
	IMU_QuaternionEKF_Init(10, 0.001, 10000000, 1, 0.001f,0); //ekf初始化
	
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(1);  // 定义任务的运行频率

	// 初始化xLastWakeTime为当前的系统节拍计数
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
//		HAL_GPIO_TogglePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin);
//		inv_imu_sleep_us(10);
			
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
			getDataFromRegisters(&imu_event);
			
			spl06ReadData(&imuData.spl06Temperature,&imuData.barPressure);	/* 读气压计的原始数据 */
		
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
		
//			sendImuData(imuData);																						/* 向上位机发送数据 */
		
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
	功能:解析USB虚拟串口接收的数据
*/
static void vcpDataAnalysis(_RingBuffer *ringbuffer)
{
	char logBuffer[100];
	unsigned char *RxFrameData;/*存储解析帧的空间*/
	unsigned short FrameDataLen = ringbuffer->Lenght;
	if(xPortGetFreeHeapSize() >= FrameDataLen)
	{
		RxFrameData = (unsigned char *)pvPortMalloc(FrameDataLen);	/*为读取缓冲区中的所有数据申请内存*/
		if(ReadBytes(ringbuffer,RxFrameData,FrameDataLen) == 1)		/*读取缓冲区中的所有数据*/
		{
			/*读取到缓冲区的数据*/
			for(int i=0;i<FrameDataLen;i++)	/*遍历*/
			{
				if(RxFrameData[i] == 0xAA)			/*找帧头*/
				{
					if(FrameDataLen - i <= 3)		/*帧头后面无数据长度*/
					{
						WriteBytes(ringbuffer,&RxFrameData[i],FrameDataLen - i);/*未处理的数据写回缓冲区*/
						vPortFree(RxFrameData);  //释放内存
						return ;/*返回空数据*/
					}
					
					unsigned short FrameLen = RxFrameData[i+3];			/*得到帧数据内容长度*/
					
					if((FrameDataLen - i) >= (4 + FrameLen + 2))	/*剩余数据是否还有一帧的长度*/
					{
						if(FrameDataCheck( &RxFrameData[i] ) == 1)		/*校验帧数据*/
						{
							unsigned short OneFrameDataLen = 4 + FrameLen + 2;			/*返回帧总长*/
							if(RxFrameData[i+1] == PC)														/*是上位机发送的数据*/
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
												/* 系统复位 */
												NVIC_SystemReset(); 
												break;
										}
//										case CMD_RUN_IAP:               /* 运行IAP程序 */
//										{
//											sendLogMessage(0xAA,MCU,CMD_LOG_MESSAGE,"\xe8\xae\xbe\xe5\xa4\x87\xe9\x87\x8d\xe5\x90\xaf");
//											
//											bsp_EraseCpuFlash((uint32_t)ADDR_FLASH_SECTOR_7_BANK2);
//		
//											deviceAttributeInfo.iapState = IAP_RUN_STATE;
//											
//											bsp_WriteCpuFlash(ADDR_FLASH_SECTOR_7_BANK2,(uint8_t *)deviceAttributeInfo.wifiName,sizeof(deviceAttributeInfo));
//											
//											/* 系统复位 */
//											 NVIC_SystemReset(); //复位
//											break;
//										}


										default:
												break;
								}
							}
							i += OneFrameDataLen-1;										/*偏移*/
						}
					}
					else
					{
						WriteBytes(ringbuffer,&RxFrameData[i],FrameDataLen - i);/*未处理的数据写回缓冲区*/
						vPortFree(RxFrameData);  //释放内存
						return ;
					}
				}
			}
		}
		vPortFree(RxFrameData);  //释放内存
	}
}
#endif

/* USER CODE END Application */

