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
#include "common.h"
#include "spi.h"
#include "bsp_icm42670p.h"
#include "bsp_spl06.h"
#include "bsp_sbus.h"
#include "bsp_lis3mdl.h"
#include "ringBuffer.h"
#include "usbd_cdc_if.h"
#include "dataTransfer.h"
#include "MahonyAHRS.h"
#include "kalman_filter.h"
#include "QuaternionEKF.h"
#include "user_lib.h"
#include "flyRcCtr.h"
#include "fyFilter.h"
#include "motoCtr.h"
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
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for taskFor1000Hz */
osThreadId_t taskFor1000HzHandle;
const osThreadAttr_t taskFor1000Hz_attributes = {
  .name = "taskFor1000Hz",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal7,
};
/* Definitions for taskFor200Hz */
osThreadId_t taskFor200HzHandle;
const osThreadAttr_t taskFor200Hz_attributes = {
  .name = "taskFor200Hz",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal5,
};
/* Definitions for taskFor500Hz */
osThreadId_t taskFor500HzHandle;
const osThreadAttr_t taskFor500Hz_attributes = {
  .name = "taskFor500Hz",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal6,
};
/* Definitions for taskFor100Hz */
osThreadId_t taskFor100HzHandle;
const osThreadAttr_t taskFor100Hz_attributes = {
  .name = "taskFor100Hz",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal4,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void InitQuaternion(float *init_q4, inv_imu_sensor_event_t *evt);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskFor1000Hz(void *argument);
void StartTaskFor200Hz(void *argument);
void StartTaskFor500Hz(void *argument);
void StartTaskFor100Hz(void *argument);

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

  /* creation of taskFor1000Hz */
  taskFor1000HzHandle = osThreadNew(StartTaskFor1000Hz, NULL, &taskFor1000Hz_attributes);

  /* creation of taskFor200Hz */
  taskFor200HzHandle = osThreadNew(StartTaskFor200Hz, NULL, &taskFor200Hz_attributes);

  /* creation of taskFor500Hz */
  taskFor500HzHandle = osThreadNew(StartTaskFor500Hz, NULL, &taskFor500Hz_attributes);

  /* creation of taskFor100Hz */
  taskFor100HzHandle = osThreadNew(StartTaskFor100Hz, NULL, &taskFor100Hz_attributes);

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
  /* Infinite loop */
  for(;;)
  {
		HAL_Delay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskFor1000Hz */
/**
* @brief Function implementing the taskFor1000Hz thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskFor1000Hz */
void StartTaskFor1000Hz(void *argument)
{
  /* USER CODE BEGIN StartTaskFor1000Hz */
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(1);  // �������������Ƶ�� 1000Hz
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
	
  for(;;)
  {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  /* USER CODE END StartTaskFor1000Hz */
}

/* USER CODE BEGIN Header_StartTaskFor200Hz */
/**
* @brief Function implementing the taskFor200Hz thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskFor200Hz */
void StartTaskFor200Hz(void *argument)
{
  /* USER CODE BEGIN StartTaskFor200Hz */
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(5);  // �������������Ƶ�� 200Hz
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
	
  for(;;)
  {
//		float tempValue;
//		if(ch[CH_THR] > 0)
//			tempValue = 4 * ch[CH_THR];
//		else
//			tempValue = 0;
		
		
		
		
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  /* USER CODE END StartTaskFor200Hz */
}

/* USER CODE BEGIN Header_StartTaskFor500Hz */
/**
* @brief Function implementing the taskFor500Hz thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskFor500Hz */
void StartTaskFor500Hz(void *argument)
{
  /* USER CODE BEGIN StartTaskFor500Hz */
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(2);  // �������������Ƶ�� 500Hz
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
	inv_imu_sensor_event_t imu_event;
	memset(&imu_event,0,sizeof(inv_imu_sensor_event_t));
	
	Mahony_Init(500);
	
	float init_quaternion[4] = {0};
	for(int i=0;i<100;i++)
	{
		getDataFromRegisters(&imu_event);
		HAL_Delay(2);
	}
	
  InitQuaternion(init_quaternion, &imu_event);
	IMU_QuaternionEKF_Init(init_quaternion, 10, 0.001, 10000000, 1, 0.002f,0);
	
  for(;;)
  {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
			getDataFromRegisters(&imu_event);
			
			spl06ReadData(&imuData.spl06Temperature,&imuData.barPressure);	/* ����ѹ�Ƶ�ԭʼ���� */
		
			imuData.icm42670pTemperature = (imu_event.temperature / 128.0f) + 25.0f;
		
			imuData.accel_x = -imu_event.accel[1];
			imuData.accel_y = -imu_event.accel[0];
			imuData.accel_z = imu_event.accel[2];
			
			imuData.gyro_x = -imu_event.gyro[1];
			imuData.gyro_y = -imu_event.gyro[0]; 
			imuData.gyro_z = -imu_event.gyro[2];
		
			lis3mdlRead(&imuData.lis3mdlTemperature,&imuData.mag_x,&imuData.mag_y,&imuData.mag_z);
			
//			MahonyAHRSupdateIMU(imuData.gyro_x / 16.4f /57.3f,imuData.gyro_y / 16.4f/57.3f,imuData.gyro_z / 16.4f/57.3f,
//													imuData.accel_x/2048.0f,imuData.accel_y/2048.0f,imuData.accel_z/2048.0f);
			
//			imuData.accel_x = -imu_event.accel[1];
//			imuData.accel_y = -imu_event.accel[0];
//			imuData.accel_z = imu_event.accel[2];
//			
//			imuData.gyro_x = imu_event.gyro[1];
//			imuData.gyro_y = imu_event.gyro[0]; 
//			imuData.gyro_z = -imu_event.gyro[2];
//			
//			IMU_QuaternionEKF_Update(	imuData.gyro_x / 16.4f /57.3f,imuData.gyro_y / 16.4f/57.3f,imuData.gyro_z / 16.4f/57.3f,
//																imuData.accel_x/2048.0f,imuData.accel_y/2048.0f,imuData.accel_z/2048.0f);
																
//			Mahony_computeAngles();
//			
//			imuData.pitch = getPitch();
//			imuData.roll = getRoll();
//			imuData.yaw = getYaw();

//			sendImuData(imuData);																						/* ����λ���������� */
		motoPowerOut(outPwmValue[0], outPwmValue[1], outPwmValue[2], outPwmValue[3],&flyState);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  /* USER CODE END StartTaskFor500Hz */
}

/* USER CODE BEGIN Header_StartTaskFor100Hz */
/**
* @brief Function implementing the taskFor100Hz thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskFor100Hz */
void StartTaskFor100Hz(void *argument)
{
  /* USER CODE BEGIN StartTaskFor100Hz */
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(10);  // �������������Ƶ�� 100Hz
	xLastWakeTime = xTaskGetTickCount();
	
	_imuDataStruct testFilterData;
	
  /* Infinite loop */
	armIirFilterInit();
  for(;;)
  {
		
		updateRcValue(ch,sbusCh);
		updateRcState(&flyState,10);
		
		electricalCalibration(ch,&flyState);		/* ���У׼ */
		
		testFilterData.roll = imuData.gyro_x;
		testFilterData.pitch = imuData.pitch;
		
    armIirFilter(&testFilterData.roll, &testFilterData.pitch);
//		sendFlyInfo(systemInfo);
		
		sendImuData(testFilterData);	
		
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  /* USER CODE END StartTaskFor100Hz */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
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
// ʹ�ü��ٶȼƵ����ݳ�ʼ��Roll��Pitch,��Yaw��0,�������Ա����ڳ�ʼʱ�����̬�������
void InitQuaternion(float *init_q4, inv_imu_sensor_event_t *evt)
{
    float acc_init[3] = {0};
    float gravity_norm[3] = {0, 0, 1}; // ����ϵ�������ٶ�ʸ��,��һ����Ϊ(0,0,1)
    float axis_rot[3] = {0};           // ��ת��
    // ��ȡ100�μ��ٶȼ�����,ȡƽ��ֵ��Ϊ��ʼֵ
    for (uint8_t i = 0; i < 100; ++i)
    {
        getDataFromRegisters(evt);
        acc_init[0] += -evt->accel[1]/2024.0f;
        acc_init[1] += -evt->accel[0]/2024.0f;
        acc_init[2] += evt->accel[2]/2024.0f;
        HAL_Delay(1);
    }
    for (uint8_t i = 0; i < 3; ++i)
        acc_init[i] /= 100;
		
		
    Norm3d(acc_init);
    // ����ԭʼ���ٶ�ʸ���͵���ϵ�������ٶ�ʸ���ļн�
    float angle = acosf(Dot3d(acc_init, gravity_norm));
    Cross3d(acc_init, gravity_norm, axis_rot);
    Norm3d(axis_rot);
    init_q4[0] = cosf(angle / 2.0f);
    for (uint8_t i = 0; i < 2; ++i)
        init_q4[i + 1] = axis_rot[i] * sinf(angle / 2.0f); // ��ǹ�ʽ,������Ϊ0(û��z�����)
}
/* USER CODE END Application */

