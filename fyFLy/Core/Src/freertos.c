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
#include "fyCalibration.h"
#include "fyPid.h"
#include "fyAttitudeCtr.h"
#include "usart.h"
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
/* Definitions for taskFor250Hz */
osThreadId_t taskFor250HzHandle;
const osThreadAttr_t taskFor250Hz_attributes = {
  .name = "taskFor250Hz",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal5,
};
/* Definitions for taskFor500Hz */
osThreadId_t taskFor500HzHandle;
const osThreadAttr_t taskFor500Hz_attributes = {
  .name = "taskFor500Hz",
  .stack_size = 256 * 4,
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
void readImuData(inv_imu_sensor_event_t *imuReadHandle,_imuDataStruct *imuData);
void imuDataFilter(_imuDataStruct *tempImuData);
void imuUpdate(_imuDataStruct *tempImuData);
void sendOriginalDataProcess(const _imuDataStruct tempImuData);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskFor1000Hz(void *argument);
void StartTaskFor250Hz(void *argument);
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

  /* creation of taskFor250Hz */
  taskFor250HzHandle = osThreadNew(StartTaskFor250Hz, NULL, &taskFor250Hz_attributes);

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
	vTaskDelay(100);
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		uartDataDmaSend();		/* 串口数据发送 */
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
	const TickType_t xFrequency = pdMS_TO_TICKS(1);  // 定义任务的运行频率 1000Hz
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  /* USER CODE END StartTaskFor1000Hz */
}

/* USER CODE BEGIN Header_StartTaskFor250Hz */
/**
* @brief Function implementing the taskFor250Hz thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskFor250Hz */
void StartTaskFor250Hz(void *argument)
{
  /* USER CODE BEGIN StartTaskFor250Hz */
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(4);  // 定义任务的运行频率 250Hz
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		angleControl(ch, imuData, 4);
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  /* USER CODE END StartTaskFor250Hz */
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
	const TickType_t xFrequency = pdMS_TO_TICKS(2);  // 定义任务的运行频率 500Hz
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
	
	inv_imu_sensor_event_t imu_event;
	memset(&imu_event,0,sizeof(inv_imu_sensor_event_t));
	
#ifdef USE_MAHONY_AHRS
	Mahony_Init(500);
#endif
	
#ifdef USE_EKF
	float init_quaternion[4] = {0};
	for(int i=0;i<100;i++)
	{
		getDataFromRegisters(&imu_event);
		HAL_Delay(2);
	}
  InitQuaternion(init_quaternion, &imu_event);
	IMU_QuaternionEKF_Init(init_quaternion, 10, 0.001, 10000000, 1, 0.002f,0);
#endif
	accelAndGyroFilterInit();/* 滤波器 启动！ */
  for(;;)
  {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		/* 从传感器读取数据 */
		readImuData(&imu_event,&imuData);
		/* 数据滤波 */
		imuDataFilter(&imuData);
		/* 校准模式下发送传感器的原始数据 */
		sendOriginalDataProcess(imuData);
		/* 姿态解算 */
		imuUpdate(&imuData);
		/* 角速度环 */
		if(WAIT_TAKE_OFF == flyState.flyTakeOffState)
		{	
			angularVelocityControl(imuData,2);
			outPwmValue[0] = ch[CH_THR]*3 + angularVelocityPitchPid.pidOutValue - angularVelocityRollPid.pidOutValue + angularVelocityYawPid.pidOutValue + systemInfo.idleThrottleValue;
			outPwmValue[1] = ch[CH_THR]*3 + angularVelocityPitchPid.pidOutValue + angularVelocityRollPid.pidOutValue - angularVelocityYawPid.pidOutValue + systemInfo.idleThrottleValue;
			outPwmValue[2] = ch[CH_THR]*3 - angularVelocityPitchPid.pidOutValue + angularVelocityRollPid.pidOutValue + angularVelocityYawPid.pidOutValue + systemInfo.idleThrottleValue;
			outPwmValue[3] = ch[CH_THR]*3 - angularVelocityPitchPid.pidOutValue - angularVelocityRollPid.pidOutValue - angularVelocityYawPid.pidOutValue + systemInfo.idleThrottleValue;

			outPwmValue[0] = LIMT(outPwmValue[0],0,PWM_MAX_RANGE);
			outPwmValue[1] = LIMT(outPwmValue[1],0,PWM_MAX_RANGE);
			outPwmValue[2] = LIMT(outPwmValue[2],0,PWM_MAX_RANGE);
			outPwmValue[3] = LIMT(outPwmValue[3],0,PWM_MAX_RANGE);
		}
		else 
		{	
			angularVelocityPitchPid.integralError = 0;
			angularVelocityRollPid.integralError = 0;
			angularVelocityYawPid.integralError = 0;
		}
		if(0 == systemInfo.calibrationMode)
		{
			sendGyroData(imuData);
		}
//		sendFlyInfo(systemInfo);
//		sendImuData(imuData);
		
		
		
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
	const TickType_t xFrequency = pdMS_TO_TICKS(10);  // 定义任务的运行频率 100Hz
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		/* 更新遥控器接收的值 */
		updateRcValue(ch,sbusCh);
		/* 根据遥控器的值更新系统状态 */
		updateRcState(&flyState,10);
		/* 串口数据接收以及处理 */
		uartMessageBufferReceiveAndAnalysis();
		/* 电调校准 */	
		electricalCalibration(ch,&flyState);		
//	sendGyroData(imuData);
//		if(WAIT_TAKE_OFF == flyState.flyTakeOffState)
//		{
//			outPwmValue[1] = systemInfo.idleThrottleValue +  ch[CH_THR];
//		}
//		sendImuData(imuData);
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  /* USER CODE END StartTaskFor100Hz */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
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
// 使用加速度计的数据初始化Roll和Pitch,而Yaw置0,这样可以避免在初始时候的姿态估计误差
void InitQuaternion(float *init_q4, inv_imu_sensor_event_t *evt)
{
    float acc_init[3] = {0};
    float gravity_norm[3] = {0, 0, 1}; // 导航系重力加速度矢量,归一化后为(0,0,1)
    float axis_rot[3] = {0};           // 旋转轴
    // 读取100次加速度计数据,取平均值作为初始值
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
    // 计算原始加速度矢量和导航系重力加速度矢量的夹角
    float angle = acosf(Dot3d(acc_init, gravity_norm));
    Cross3d(acc_init, gravity_norm, axis_rot);
    Norm3d(axis_rot);
    init_q4[0] = cosf(angle / 2.0f);
    for (uint8_t i = 0; i < 2; ++i)
        init_q4[i + 1] = axis_rot[i] * sinf(angle / 2.0f); // 轴角公式,第三轴为0(没有z轴分量)
}
/* USER CODE END Application */

