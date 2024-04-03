#ifndef __DATA_TRANSFER_H__
#define __DATA_TRANSFER_H__

#include "main.h"
#include "common.h"

typedef struct
{
	uint8_t *dataPtr;
	uint16_t dataLen;
}__attribute__((packed)) _queueDataType;

uint8_t sendLogMessage(unsigned char frameHead, unsigned char frameAddress, unsigned char frameID, char* data);

void uartMessageBufferInit(void);
void uartMessageBufferReceiveAndAnalysis(void);

void uartQueueInit(void);
void uartDmaReceiveInit(void);

void uartDataDmaSend(void);
void uartDataWriteToQueue(UART_HandleTypeDef *huart, uint8_t *uartData, uint16_t dataLen);

void sendFlyInfo(const _systemInfoStruct info);	
void sendImuData(const _imuDataStruct data);
void sendGyroData(const _imuDataStruct data);
#endif /* __DATA_TRANSFER_H__ */




