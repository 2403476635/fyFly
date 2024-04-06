#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

/* 调试的方式 */
/* 默认USB发送的数据都是做了阻塞发送检测的，所以在USB没插入时，USB数据是检测不到发送成功的，会导致卡死 */
#define WIRELESS_DEBUG 					
//#define USB_DEBUG 

#define LIMT(X,MIN,MAX) ( (X) < (MIN) ? (MIN) : ( (X) > (MAX) ? (MAX) : (X) ) )	/* 限幅 */
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define USE_MAHONY_AHRS 
//#define USE_EKF 

/* 飞机状态 */
typedef enum 
{
	FLY_LOCK = 0,								/* 飞控上锁 */
	FLY_UNLOCK,									/* 飞控解锁 */
	RC_ONLINE,									/* 遥控器连接正常 */
	RC_LOST,										/* 遥控器掉线 */
	MOTO_STOP,									/* 停止 */
	MOTO_IDING,									/* 怠速 */
	LANDING_ING,								/* 正在降落 */
	LANDING_DONE,								/* 降落完成 */
	WAIT_TAKE_OFF,							/* 等待起飞 */
	TAKE_OFF_ING,								/* 正在起飞 */
	TAKE_OFF_DONE,							/* 起飞完成 */
	ELECTRICAL_CAIBRATION,			/* 电调校准 */
	EMERGENCY_STOP							/* 急停 */
}ENUM_FLY_STATE;

typedef struct  
{
	ENUM_FLY_STATE flyTakeOffState;
 	ENUM_FLY_STATE flyLockState;
	ENUM_FLY_STATE rcState;
}_flyStateStruct;
extern _flyStateStruct flyState;

typedef struct
{
	uint32_t systemClockFrequency; /* 记录系统的运行频率 */
	uint8_t accGyroSensorInitState;
	uint8_t baroSensorInitState;
	uint8_t magSensorInitState;
	uint8_t calibrationMode;			/* 校准模式 */
	float idleThrottlePercent;
	uint16_t idleThrottleValue;
	_flyStateStruct *flyState;
}_systemInfoStruct;
extern _systemInfoStruct systemInfo;			/* 记录系统信息 */

enum ENUM_SYS_STATE
{
	ACC_GYRO_SENSOR_INIT_SUCCESS = 1,			/* 加速度计陀螺仪初始化成功 */
	ACC_GYRO_SENSOR_INIT_ERROR,						/* 加速度计陀螺仪初始化失败 */
	BARO_SENSOR_INIT_SUCCESS,							/* 气压计初始化成功 */
	BARO_SENSOR_INIT_ERROR,								/* 气压计初始化失败 */
	MAG_SENSOR_INIT_SUCCESS,							/* 磁力计初始化成功 */
	MAG_SENSOR_INIT_ERROR									/* 磁力计初始化失败 */
};

/* 用于串口通信 */
enum ENUM_FRAME_ADDRESS
{
	PC = 0xA5,
	MCU = 0xA6
};
enum frameCmd
{
    CMD_ORIGINAL_IMU_DATA = 0,                          /* 发送IMU的原始数据 */
    CMD_IMU_ACCEL_DATA,																	/* 发送加速度转换后的值 */
    CMD_IMU_GYRO_DATA,																	/* 发送角速度转换后的值 */
    CMD_READ_CORRECT_PARAMETER,                         /* 读取传感器的校正数据 */
    CMD_SEND_CORRECT_PARAMETER,                         /* 发送传感器的校正数据 */
    CMD_READ_PID_PARAMETER,                             /* 读取PID参数信息 */
    CMD_SET_PID_PARAMETER,                              /* 设置PID参数信息 */
    CMD_SET_CALIBRATION_MODE,                     			/* 设置校准模式 */
		CMD_SEND_SENSOR_DATA,																/* 发送用于校准的传感器数据 */
    CMD_FLY_INFO,																				/* 飞行信息 */
    CMD_RESET_DEVICE,                                   /* 设备复位 */
    CMD_SEND_BIN_FILE_SIZE,                             /* 发送的固件文件大小 */
    CMD_SEND_BIN_FILE_PACK,                             /* 固件数据包 */
    CMD_SEND_BIN_FILE_END,                              /* 固件数据发送完成 */
    CMD_RUN_IAP,                                        /* 运行IAP程序 */
    CMD_NOW_IAP_STATE,																	/* 当前运行IAP程序 */
    CMD_NOW_APP_STATE,                                  /* 当前运行APP程序 */
    CMD_LOG_MESSAGE,
    CMD_SHOW_DEVICE_INFO,
    CMD_ACK                                             /* 响应 */
};
/* 校准模式 */
enum calibrationMode
{
	ACCEL_CALIBRATION_MODE = 1,			/* 加速度计校准模式 */
	GYRO_CALIBRATION_MODE,					/* 陀螺仪校准模式 */
	MAG_CALIBRATION_MODE						/* 磁力计校准模式 */
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
	
	float pitch;
	float roll;
	float yaw;
	
	float mag_x_filter;
	float mag_y_filter;
	float mag_z_filter;
	
	float accel_x_filter;
	float accel_y_filter;
	float accel_z_filter;
	
	float gyro_x_filter;
	float gyro_y_filter;
	float gyro_z_filter;
	/* g/s/s */
	float accel_x_gss;
	float accel_y_gss;
	float accel_z_gss;
	/* °/s */
	float gyro_x_deg;
	float gyro_y_deg;
	float gyro_z_deg;
	/* rad/s */
	float gyro_x_rad;
	float gyro_y_rad;
	float gyro_z_rad;
}_imuDataStruct;

/* 遥控的通道 */	
enum ENUM_CH				
{
	CH_THR = 0,	/* 油门 */
	CH_PITCH,		/* 俯仰 */
	CH_ROLL,		/* 横滚 */
	CH_YAW,			/* 偏航 */
	CH_AUX1,		/* 通道1 */
	CH_AUX2,		/* 通道2 */
	CH_AUX3,		/* 通道3 */
	CH_AUX4,		/* 通道4 */
	CH_NUM
}; 

#define M_PI (3.14159265358979f)

#define MAX_ANGULAR_VELOCITY (1500)
#define MAX_ANGULAR_VELOCITY_PID_OUT (1500)

#define GYRORANGE  1000
#define ACCELRANGE 8
#endif /* __RINGBUFFER_H__ */




