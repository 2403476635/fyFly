#ifndef __COMMON_H__
#define __COMMON_H__

/* 调试的方式 */
/* 默认USB发送的数据都是做了阻塞发送检测的，所以在USB没插入时，USB数据是检测不到发送成功的，会导致卡死 */
//#define WIRELESS_DEBUG 					
#define USB_DEBUG 

/* 用于串口通信 */
enum frameAddress
{
	PC = 0xA5,
	MCU = 0xA6
};
enum frameCmd
{
		CMD_ORIGINAL_IMU_DATA = 0,                          /* 发送IMU的原始数据 */
		CMD_RESET_DEVICE,                                   /* 设备复位 */
		CMD_SEND_BIN_FILE_SIZE,                             /* 发送的固件文件大小 */
		CMD_SEND_BIN_FILE_PACK,                             /* 固件数据包 */
		CMD_SEND_BIN_FILE_END,                              /* 固件数据发送完成 */
		CMD_RUN_IAP,                                        /* 运行IAP程序 */
		CMD_NOW_IAP_STATE,																	/* 当前运行IAP程序 */
		CMD_NOW_APP_STATE,                                  /* 当前运行APP程序 */
		CMD_LOG_MESSAGE,
		CMD_SHOW_DEVICE_INFO
};

/* 传感器数据 */
typedef struct
{
	float lis3mdlTemperature;
	float mag_x;
	float mag_y;
	float mag_z;
	
	float icm42670pTemperature; 	/* Temperature = （res/128）+ 25 */
	short accel_x;
	short accel_y;
	short accel_z;
	
	short gyro_x;
	short gyro_y;
	short gyro_z;
	
	float spl06Temperature;
	float barPressure;
	
}_imuDataStruct;



#endif /* __RINGBUFFER_H__ */




