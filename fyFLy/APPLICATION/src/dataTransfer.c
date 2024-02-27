#include "usbd_cdc_if.h"

#include "dataTransfer.h"

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

		while(USBD_OK != CDC_Transmit_FS((uint8_t *)frameData,dataLen + 4 + 2)){};
    return dataLen + 4 + 2;
}
#endif
/* 发送传感器的原始数据 */
void sendImuData(const _imuDataStruct data)
{
	uint8_t sendDataBuffer[100];
	int32_t temp  = 0;	/* 临时存放的数据 */
	uint8_t cnt = 0;
	
	/* float 数据会被方法1000倍 */
	temp = data.spl06Temperature *1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.barPressure *1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.icm42670pTemperature * 1000;
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
	
	temp = data.lis3mdlTemperature *1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.mag_x *1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
	temp = data.mag_y *1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;

	temp = data.mag_z *1000;	
	sendDataBuffer[cnt++] = temp;
	sendDataBuffer[cnt++] = temp>>8;
	sendDataBuffer[cnt++] = temp>>16;
	sendDataBuffer[cnt++] = temp>>24;
	
#if defined(USB_DEBUG)
	vcpSendData(0xAA,MCU,CMD_ORIGINAL_IMU_DATA,sendDataBuffer,cnt);
#endif
}















