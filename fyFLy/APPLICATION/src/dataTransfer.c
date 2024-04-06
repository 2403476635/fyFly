#include "dataTransfer.h"
#include "usbd_cdc_if.h"
#include "usart.h"
#include "bsp_sbus.h"
#include "fyAttitudeCtr.h"
#include "fyCalibration.h"
#include "saveParameter.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stream_buffer.h"

static QueueHandle_t uart1TxQueue =NULL;
static QueueHandle_t uart2TxQueue =NULL;
static QueueHandle_t uart3TxQueue =NULL;
static QueueHandle_t uart4TxQueue =NULL;
static QueueHandle_t uart6TxQueue =NULL;

uint8_t uart1DmaSendIdle = 0;
uint8_t uart2DmaSendIdle = 0;
uint8_t uart3DmaSendIdle = 0;
uint8_t uart4DmaSendIdle = 0;
uint8_t uart6DmaSendIdle = 0;

static uint8_t uart5RxDmaBuffer[50];

static uint8_t uart1RxDmaBuffer[1024];
static uint8_t uart2RxDmaBuffer[1024];
static uint8_t uart3RxDmaBuffer[1024];
static uint8_t uart4RxDmaBuffer[1024];
static uint8_t uart6RxDmaBuffer[1024];

static uint8_t uart1TxDmaBuffer[1024];
static uint8_t uart2TxDmaBuffer[1024];
static uint8_t uart3TxDmaBuffer[1024];
static uint8_t uart4TxDmaBuffer[1024];
static uint8_t uart6RxDmaBuffer[1024];

StreamBufferHandle_t  uart2RxStreamBuffer;

static void uartDataAnalysis(uint8_t *dataBuffer, const uint16_t dataLen);
static unsigned char FrameDataCheck(unsigned char* OneFrameData);
/* 信息发送 */
/* 发送log信息，输入字符 */
uint8_t sendLogMessage(unsigned char frameHead, unsigned char frameAddress, unsigned char frameID, char* data)
{
    unsigned char frameDataLen = 0;
		uint32_t cnt = 0;
    char frameData[260];
    frameDataLen = strlen(data);
    frameData[cnt++] = frameHead;                               /* 帧头 */
    frameData[cnt++] = frameAddress;                            /* 帧地址 */
    frameData[cnt++] = frameID;                                 /* 帧ID */
    frameData[cnt++] = frameDataLen;                            /* 帧数据长度 */

		memcpy(&frameData[4],data,frameDataLen);
	
    unsigned char sumcheck = 0;
    unsigned char addcheck = 0;
    for(unsigned char i=0; i < frameData[3] + 4 ; i++)
    {
        sumcheck += frameData[i]; //从帧头开始，对每一字节进行求和，直到DATA区结束
        addcheck += sumcheck; //每一字节的求和操作，进行一次sumcheck的累加
    }
    frameData[frameDataLen + 4] = sumcheck;
    frameData[frameDataLen + 5] = addcheck;
		
#if defined(USB_DEBUG)
		while(USBD_OK != CDC_Transmit_FS((uint8_t *)frameData,frameDataLen + 4 + 2)){};
#endif
    return frameDataLen + 4 + 2;
}

#if defined(USB_DEBUG)
/* 虚拟串口发送数据,提供了dataLen数据长度，所以传输可以是任意数据，最大长度:255 */
uint8_t vcpSendData(uint8_t frameHead, uint8_t frameAddress, uint8_t frameID, uint8_t* data,uint8_t dataLen)
{
		uint32_t cnt = 0;
    uint8_t frameData[260];
	
    frameData[cnt++] = frameHead;                               /* 帧头 */
    frameData[cnt++] = frameAddress;                            /* 帧地址 */
    frameData[cnt++] = frameID;                                 /* 帧ID */
    frameData[cnt++] = dataLen;                            			/* 帧数据长度 */

		memcpy(&frameData[4],data,dataLen);
	
    unsigned char sumcheck = 0;
    unsigned char addcheck = 0;
    for(unsigned char i=0; i < frameData[3] + 4 ; i++)
    {
        sumcheck += frameData[i]; 	//从帧头开始，对每一字节进行求和，直到DATA区结束
        addcheck += sumcheck; 			//每一字节的求和操作，进行一次sumcheck的累加
    }
    frameData[dataLen + 4] = sumcheck;
    frameData[dataLen + 5] = addcheck;

//		while(USBD_OK != CDC_Transmit_FS((uint8_t *)frameData,dataLen + 4 + 2)){};
    CDC_Transmit_FS((uint8_t *)frameData,dataLen + 4 + 2);
		return dataLen + 4 + 2;
}
#endif
#if defined(WIRELESS_DEBUG)
uint8_t uartSendData(uint8_t frameHead, uint8_t frameAddress, uint8_t frameID, uint8_t* data,uint8_t dataLen)
{
		uint32_t cnt = 0;
    uint8_t frameData[260];
	
    frameData[cnt++] = frameHead;                               /* 帧头 */
    frameData[cnt++] = frameAddress;                            /* 帧地址 */
    frameData[cnt++] = frameID;                                 /* 帧ID */
    frameData[cnt++] = dataLen;                            			/* 帧数据长度 */

		memcpy(&frameData[4],data,dataLen);
	
    unsigned char sumcheck = 0;
    unsigned char addcheck = 0;
    for(unsigned char i=0; i < frameData[3] + 4 ; i++)
    {
        sumcheck += frameData[i]; 	//从帧头开始，对每一字节进行求和，直到DATA区结束
        addcheck += sumcheck; 			//每一字节的求和操作，进行一次sumcheck的累加
    }
    frameData[dataLen + 4] = sumcheck;
    frameData[dataLen + 5] = addcheck;
		uartDataWriteToQueue(&huart2, (uint8_t *)frameData, dataLen + 4 + 2);
		return dataLen + 4 + 2;
}
#endif
/* 发送传感器的原始数据 */
void sendImuData(const _imuDataStruct data)
{
	uint8_t sendDataBuffer[100];
	int32_t temp  = 0;	/* 临时存放的数据 */
	uint8_t cnt = 0;
	
	/* float 数据会被放大1000倍 */
	temp = data.spl06Temperature*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.barPressure*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.icm42670pTemperature*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	sendDataBuffer[cnt++] = data.accel_x;
	sendDataBuffer[cnt++] = data.accel_x>>8;
	sendDataBuffer[cnt++] = data.accel_y;
	sendDataBuffer[cnt++] = data.accel_y>>8;
	sendDataBuffer[cnt++] = data.accel_z;
	sendDataBuffer[cnt++] = data.accel_z>>8;
	
	sendDataBuffer[cnt++] = data.gyro_x;
	sendDataBuffer[cnt++] = data.gyro_x>>8;
	sendDataBuffer[cnt++] = data.gyro_y;
	sendDataBuffer[cnt++] = data.gyro_y>>8;
	sendDataBuffer[cnt++] = data.gyro_z;
	sendDataBuffer[cnt++] = data.gyro_z>>8;
	
	temp = data.lis3mdlTemperature*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.mag_x*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.mag_y*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;

	temp = data.mag_z*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.pitch*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.roll*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.yaw*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
/**********************************************************/
	temp = data.accel_x_filter*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.accel_y_filter*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.accel_z_filter*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.gyro_x_filter*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.gyro_y_filter *1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.gyro_z_filter*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
#if defined(USB_DEBUG)
	vcpSendData(0xAA,MCU,CMD_ORIGINAL_IMU_DATA,sendDataBuffer,cnt);
#endif
#if defined(WIRELESS_DEBUG)
	uartSendData(0xAA,MCU,CMD_ORIGINAL_IMU_DATA,sendDataBuffer,cnt);
#endif
}
void sendSensorGyroData(const _imuDataStruct data)
{
	uint8_t sendDataBuffer[100];
	int32_t temp  = 0;	/* 临时存放的数据 */
	uint8_t cnt = 0;
	
	/* float 数据会被放大1000倍 */
	temp = data.gyro_x_filter*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	/* float 数据会被放大1000倍 */
	temp = data.gyro_y_filter*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	/* float 数据会被放大1000倍 */
	temp = data.gyro_z_filter*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
#if defined(USB_DEBUG)
	vcpSendData(0xAA,MCU,CMD_IMU_GYRO_DATA,sendDataBuffer,cnt);
#endif
#if defined(WIRELESS_DEBUG)
	uartSendData(0xAA,MCU,CMD_SEND_SENSOR_DATA,sendDataBuffer,cnt);
#endif
}
void sendAccelData(const _imuDataStruct data)
{

}
void sendGyroData(const _imuDataStruct data)
{
	uint8_t sendDataBuffer[100];
	int32_t temp  = 0;	/* 临时存放的数据 */
	uint8_t cnt = 0;
	
	/* float 数据会被放大1000倍 */
	temp = angleRollPid.pidOutValue*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	/* float 数据会被放大1000倍 */
	temp = data.gyro_y_filter*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	/* float 数据会被放大1000倍 */
	temp = data.yaw*1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
#if defined(USB_DEBUG)
	vcpSendData(0xAA,MCU,CMD_IMU_GYRO_DATA,sendDataBuffer,cnt);
#endif
#if defined(WIRELESS_DEBUG)
	uartSendData(0xAA,MCU,CMD_IMU_GYRO_DATA,sendDataBuffer,cnt);
#endif
}
/* 发送飞控的系统状态 */
void sendFlyInfo(const _systemInfoStruct info)
{
	uint8_t sendDataBuffer[100];
	int32_t temp  = 0;	/* 临时存放的数据 */
	uint8_t cnt = 0;
	
	/* float 数据会被放大1000倍 */
	temp = info.systemClockFrequency;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	sendDataBuffer[cnt++] = info.accGyroSensorInitState;	
	sendDataBuffer[cnt++] = info.baroSensorInitState;
	sendDataBuffer[cnt++] = info.magSensorInitState;
	
	sendDataBuffer[cnt++] = info.flyState->flyTakeOffState;
	sendDataBuffer[cnt++] = info.flyState->flyLockState;
	sendDataBuffer[cnt++] = info.flyState->rcState;
	
#if defined(USB_DEBUG)
	vcpSendData(0xAA,MCU,CMD_FLY_INFO,sendDataBuffer,cnt);
#endif
}
/* 发送原始的数据以及通过滤波器后的数据 */
void sendOriginalAndFilterData(void)
{
		
}
/* 基本收发实现逻辑 */
void uartMessageBufferInit(void)
{
	uart2RxStreamBuffer = xStreamBufferCreate(2048,1);
}
/* 串口接收后解析数据，此段不在中断中执行 */
void uartMessageBufferReceiveAndAnalysis(void)
{
	uint16_t uart2BufferCanReadLen = xStreamBufferBytesAvailable(uart2RxStreamBuffer);
	if(uart2BufferCanReadLen)
	{
		if(xPortGetFreeHeapSize() < uart2BufferCanReadLen)return;
		uint8_t *bufferDataPtr = pvPortMalloc(uart2BufferCanReadLen);
		xStreamBufferReceive(uart2RxStreamBuffer, bufferDataPtr, uart2BufferCanReadLen, 0);
		uartDataAnalysis(bufferDataPtr, uart2BufferCanReadLen);
		vPortFree(bufferDataPtr);
	}
}
/* 队列初始化 */
void uartQueueInit(void)
{
	uart1TxQueue = xQueueCreate(5,sizeof(_queueDataType));
	uart2TxQueue = xQueueCreate(20,sizeof(_queueDataType));
	uart3TxQueue = xQueueCreate(5,sizeof(_queueDataType));
	uart4TxQueue = xQueueCreate(5,sizeof(_queueDataType));
}
/* 串口数据写队列 */
void uartDataWriteToQueue(UART_HandleTypeDef *huart, uint8_t *uartData, uint16_t dataLen)
{
	_queueDataType uartQueueDataType;
	BaseType_t queueSendReturn = pdFALSE;
	if(dataLen>1024)return ;
	/* 任意的串口数据先写入队列中等待发送 */
	if(USART2 == huart->Instance)
	{
		if(xPortGetFreeHeapSize() < dataLen)return;
		uartQueueDataType.dataPtr = pvPortMalloc(dataLen);
		for(uint16_t i=0;i<dataLen;i++)
		{
			uartQueueDataType.dataPtr[i] = uartData[i];
		}
		uartQueueDataType.dataLen = dataLen;
		queueSendReturn = xQueueSend(uart2TxQueue,&uartQueueDataType,0);
		if(pdFALSE == queueSendReturn)
		{
			vPortFree(uartQueueDataType.dataPtr);
		}
	}
}
/* DMA接收初始化 */
void uartDmaReceiveInit(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart5, uart5RxDmaBuffer, 50);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2RxDmaBuffer, 1024);
}
/* DMA接收回调 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t Size)
{
	if(huart->Instance == UART5)/* 处理接收机数据 */
	{
		if(Size <= 50)
		{
			sbusDataAnalysis(uart5RxDmaBuffer);
			HAL_UARTEx_ReceiveToIdle_DMA(&huart5, uart5RxDmaBuffer, 50); 
		}
	}
	else if(huart->Instance == USART2)
	{
		if(Size <= 1024)
		{
//			
			xStreamBufferSendFromISR(uart2RxStreamBuffer, uart2RxDmaBuffer, Size, NULL);
			HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2RxDmaBuffer, 1024); 
		}
	}
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
    if(huart->Instance == UART5)
    {
			HAL_UARTEx_ReceiveToIdle_DMA(&huart5, uart5RxDmaBuffer, 50);
    }
		else if(huart->Instance == USART2)
    {
			HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2RxDmaBuffer, 1024);
    }
}
/* 串口数据MDA发送 */
void uartDataDmaSend(void)
{
	_queueDataType uartQueueDataType;
	BaseType_t queueReceiveReturn = pdFALSE;
	if(0 == uart2DmaSendIdle)	/* 判断DMA发送是否空闲 */
	{
		queueReceiveReturn = xQueueReceive(uart2TxQueue,&uartQueueDataType,0);
		if(pdTRUE == queueReceiveReturn)
		{
			for(uint16_t i=0;i<uartQueueDataType.dataLen;i++)
			{
				uart2TxDmaBuffer[i] = uartQueueDataType.dataPtr[i];
			}
			vPortFree(uartQueueDataType.dataPtr);
			/* DMA发送 */
			HAL_UART_Transmit_DMA(&huart2, (uint8_t *)uart2TxDmaBuffer, uartQueueDataType.dataLen);
			uart2DmaSendIdle = 1;
		}
	}
}
/* DMA发送完成回调函数 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(USART2 == huart->Instance)
	{
		uart2DmaSendIdle = 0;
	}
}

static void CMD_SET_PID_PARAMETER_handle(uint8_t *dataPtr);
static void CMD_SEND_CORRECT_PARAMETER_handle(uint8_t *dataPtr);
static void CMD_SET_CALIBRATION_MODE_handle(uint8_t *dataPtr);
static void CMD_READ_CORRECT_PARAMETER_handle(uint8_t *dataPtr);
static void CMD_READ_PID_PARAMETER_handle(uint8_t *dataPtr);
/* 解析串口接收的数据 */
static void uartDataAnalysis(uint8_t *dataBuffer, const uint16_t dataLen)
{
	unsigned char *RxFrameData;/*存储解析帧的空间*/
	unsigned short FrameDataLen = dataLen;
	RxFrameData = dataBuffer;	
	for(int i=0;i<FrameDataLen;i++)	/*遍历*/
	{
		if(RxFrameData[i] == 0xAA)			/*找帧头*/
		{
			if(FrameDataLen - i <= 3)		/*帧头后面无数据长度*/
			{
				return ;/*返回空数据*/
			}
			unsigned short FrameLen = RxFrameData[i+3];			/*得到帧数据内容长度*/
			if((FrameDataLen - i) >= (4 + FrameLen + 2))		/*剩余数据是否还有一帧的长度*/
			{
				if(FrameDataCheck(&RxFrameData[i]) == 1)		/*校验帧数据*/
				{
					uint8_t *dataPtr = &RxFrameData[4];
					unsigned short OneFrameDataLen = 4 + FrameLen + 2;			/*返回帧总长*/
					if(RxFrameData[i+1] == PC)															/*是上位机发送的数据*/
					{
						switch (RxFrameData[i+2])
						{
							case CMD_READ_PID_PARAMETER:	/* 上位机读取PID参数 */
							{
								CMD_READ_PID_PARAMETER_handle(dataPtr);
								break;
							}
							case CMD_SET_PID_PARAMETER:		/* 上位机写入PID参数 */
							{
								CMD_SET_PID_PARAMETER_handle(dataPtr);
								break;
							}
							case CMD_READ_CORRECT_PARAMETER:	/* 上位机读取校准参数 */
							{
								CMD_READ_CORRECT_PARAMETER_handle(dataPtr);
								break;
							}
							case CMD_SEND_CORRECT_PARAMETER:	/* 接收上位机发送来的校准参数数据 */
							{
								CMD_SEND_CORRECT_PARAMETER_handle(dataPtr);
								break;
							}
							case CMD_SET_CALIBRATION_MODE:	/* 上位机设置校准模式 */
							{
								CMD_SET_CALIBRATION_MODE_handle(dataPtr);
							}
							case CMD_SHOW_DEVICE_INFO:
							{
//								showDeviceInfo();
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
				return ;
			}
		}
	}
}

/*帧校验*/
static unsigned char FrameDataCheck(unsigned char* OneFrameData)
{
	unsigned char sumcheck = 0;
	unsigned char addcheck = 0;
	unsigned char Len = OneFrameData[3];
	
	for(unsigned char i=0; i < Len + 4 ; i++)
	{
			sumcheck += OneFrameData[i]; //从帧头开始，对每一字节进行求和，直到DATA区结束
			addcheck += sumcheck; //每一字节的求和操作，进行一次sumcheck的累加
	}
	
	if(sumcheck == OneFrameData[Len + 4] && addcheck == OneFrameData[Len + 5])
	{
			return 1; //校验通过
	}
	else
	{
			return 0; //校验失败
	}
}

/* 指令解析处理的函数 */
static void CMD_SEND_CORRECT_PARAMETER_handle(uint8_t *dataPtr)
{
	int32_t tempData = 0;
	uint8_t cnt = 0;
	if(GYRO_CALIBRATION_MODE == systemInfo.calibrationMode)					/* 校准的是陀螺仪的数据 */
	{
		for(uint8_t i=0;i<3;i++)
		{
			tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
			gyroOffset[i] = (float)tempData / 1000000.0f;
			cnt += 4;
		}
	}
	else if(ACCEL_CALIBRATION_MODE == systemInfo.calibrationMode)		/* 校准的是加速度的数据 */
	{
		
	}
	else if(MAG_CALIBRATION_MODE == systemInfo.calibrationMode)			/* 校准的是磁力计的数据 */
	{
		for(uint8_t i=0;i<3;i++)
		{
			tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
			magOffset[i] = (float)tempData / 1000000.0f;
			cnt += 4;
		}
		for(uint8_t i=0;i<9;i++)
		{
			tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
			magCalibrationMatrix3x3[i] = (float)tempData / 1000000.0f;
			cnt += 4;
		}
	}
	saveParameterHandle();
}

static void CMD_SET_PID_PARAMETER_handle(uint8_t *dataPtr)
{
	int32_t tempData = 0;
	uint8_t cnt = 0;
	
	/* 角速度环参数 */
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angularVelocityPitchPid.kp = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angularVelocityPitchPid.ki = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angularVelocityPitchPid.kd = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angularVelocityRollPid.kp = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angularVelocityRollPid.ki = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angularVelocityRollPid.kd = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angularVelocityYawPid.kp = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angularVelocityYawPid.ki = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angularVelocityYawPid.kd = (float)tempData/1000.0f;
	cnt += 4;
	/* 角度环参数 */
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	anglePitchPid.kp = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	anglePitchPid.ki = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	anglePitchPid.kd = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angleRollPid.kp = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angleRollPid.ki = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angleRollPid.kd = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angleYawPid.kp = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angleYawPid.ki = (float)tempData/1000.0f;
	cnt += 4;
	
	tempData = dataPtr[cnt] | dataPtr[cnt+1] << 8 | dataPtr[cnt+2] << 16 | dataPtr[cnt+3] << 24;
	angleYawPid.kd = (float)tempData/1000.0f;
	cnt += 4;
	
	saveParameterHandle();
}
/* 发送PID参数 */
static void CMD_READ_PID_PARAMETER_handle(uint8_t *dataPtr)
{
	uint8_t sendDataBuffer[100];
	int32_t temp  = 0;	/* 临时存放的数据 */
	uint8_t cnt = 0;
	
	/* float 数据会被放大1000倍 */
	/* 内环 */
	temp = angularVelocityPitchPid.kp*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angularVelocityPitchPid.ki*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angularVelocityPitchPid.kd*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angularVelocityRollPid.kp*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angularVelocityRollPid.ki*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angularVelocityRollPid.kd*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angularVelocityYawPid.kp*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angularVelocityYawPid.ki*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angularVelocityYawPid.kd*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	/* 外环 */
	temp = anglePitchPid.kp*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = anglePitchPid.ki*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = anglePitchPid.kd*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angleRollPid.kp*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angleRollPid.ki*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angleRollPid.kd*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angleYawPid.kp*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angleYawPid.ki*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = angleYawPid.kd*1000;
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
#if defined(USB_DEBUG)
	vcpSendData(0xAA,MCU,CMD_READ_PID_PARAMETER,sendDataBuffer,cnt);
#endif
#if defined(WIRELESS_DEBUG)
	uartSendData(0xAA,MCU,CMD_READ_PID_PARAMETER,sendDataBuffer,cnt);
#endif
}
static void CMD_SET_CALIBRATION_MODE_handle(uint8_t *dataPtr)
{
	systemInfo.calibrationMode = dataPtr[0];
}

static void CMD_READ_CORRECT_PARAMETER_handle(uint8_t *dataPtr)
{
	uint8_t sendDataBuffer[100];
	int32_t temp  = 0;	/* 临时存放的数据 */
	uint8_t cnt = 0;
	
	sendDataBuffer[cnt++] = systemInfo.calibrationMode;
	
	if(GYRO_CALIBRATION_MODE == systemInfo.calibrationMode)
	{
		for(uint8_t i=0;i<3;i++)
		{
			temp = gyroOffset[i]*1000000;
			sendDataBuffer[cnt++] = temp;
			sendDataBuffer[cnt++] = temp>>8;
			sendDataBuffer[cnt++] = temp>>16;
			sendDataBuffer[cnt++] = temp>>24;
		}
	}
	else if(MAG_CALIBRATION_MODE == systemInfo.calibrationMode)
	{
		for(uint8_t i=0;i<3;i++)
		{
			temp = magOffset[i]*1000000;
			sendDataBuffer[cnt++] = temp;
			sendDataBuffer[cnt++] = temp>>8;
			sendDataBuffer[cnt++] = temp>>16;
			sendDataBuffer[cnt++] = temp>>24;
		}
		for(uint8_t i=0;i<9;i++)
		{
			temp = magCalibrationMatrix3x3[i]*1000000;
			sendDataBuffer[cnt++] = temp;
			sendDataBuffer[cnt++] = temp>>8;
			sendDataBuffer[cnt++] = temp>>16;
			sendDataBuffer[cnt++] = temp>>24;
		}
	}
#if defined(USB_DEBUG)
	vcpSendData(0xAA,MCU,CMD_READ_CORRECT_PARAMETER,sendDataBuffer,cnt);
#endif
#if defined(WIRELESS_DEBUG)
	uartSendData(0xAA,MCU,CMD_READ_CORRECT_PARAMETER,sendDataBuffer,cnt);
#endif
}

