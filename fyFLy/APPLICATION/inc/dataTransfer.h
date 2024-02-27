#ifndef __DATA_TRANSFER_H__
#define __DATA_TRANSFER_H__

#include "common.h"
#include "main.h"

uint8_t sendLogMessage(unsigned char frameHead, unsigned char frameAddress, unsigned char frameID, char* data);
	
void sendImuData(const _imuDataStruct data);

#endif /* __DATA_TRANSFER_H__ */




