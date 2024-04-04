#ifndef COMMON_H
#define COMMON_H

/* 飞机状态 */
#include <cstdint>

typedef struct
{
    uint8_t flyTakeOffState;
    uint8_t flyLockState;
    uint8_t rcState;
}_flyStateStruct;

enum ENUM_FLY_STATE
{
    FLY_LOCK = 0,								/* 飞控上锁 */
    FLY_UNLOCK,									/* 飞控解锁 */
    RC_ONLINE,									/* 遥控器连接正常 */
    RC_LOST,                                    /* 遥控器掉线 */
    MOTO_STOP,									/* 停止 */
    MOTO_IDING,									/* 怠速 */
    LANDING_ING,								/* 正在降落 */
    LANDING_DONE,								/* 降落完成 */
    WAIT_TAKE_OFF,                              /* 等待起飞 */
    TAKE_OFF_ING,								/* 正在起飞 */
    TAKE_OFF_DONE,                              /* 起飞完成 */
    ELECTRICAL_CAIBRATION,                      /* 电调校准 */
    EMERGENCY_STOP                              /* 急停 */
};

typedef struct
{
    uint32_t systemClockFrequency; /* 记录系统的运行频率 */
    uint8_t accGyroSensorInitState;
    uint8_t baroSensorInitState;
    uint8_t magSensorInitState;
    float idleThrottlePercent;
    uint16_t idleThrottleValue;
    _flyStateStruct *flyState;
}_systemInfoStruct;

enum ENUM_SYS_STATE
{
    ACC_GYRO_SENSOR_INIT_SUCCESS = 1,			/* 加速度计陀螺仪初始化成功 */
    ACC_GYRO_SENSOR_INIT_ERROR,						/* 加速度计陀螺仪初始化失败 */
    BARO_SENSOR_INIT_SUCCESS,							/* 气压计初始化成功 */
    BARO_SENSOR_INIT_ERROR,								/* 气压计初始化失败 */
    MAG_SENSOR_INIT_SUCCESS,							/* 磁力计初始化成功 */
    MAG_SENSOR_INIT_ERROR									/* 磁力计初始化失败 */
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

enum frameAddress
{
    PC = 0xA5,
    MCU = 0xA6
};

enum frameCmd
{
    CMD_ORIGINAL_IMU_DATA = 0,                          /* 发送IMU的原始数据 */
    CMD_IMU_ACCEL_DATA,									/* 发送加速度转换后的值 */
    CMD_IMU_GYRO_DATA,									/* 发送角速度转换后的值 */
    CMD_READ_CORRECT_PARAMETER,                         /* 读取传感器的校正数据 */
    CMD_SEND_CORRECT_PARAMETER,                         /* 发送传感器的校正数据 */
    CMD_READ_PID_PARAMETER,                             /* 读取PID参数信息 */
    CMD_SET_PID_PARAMETER,                              /* 设置PID参数信息 */
    CMD_SET_CALIBRATION_MODE,                           /* 设置校准模式 */
    CMD_SEND_SENSOR_DATA,								/* 发送用于校准的传感器数据 */
    CMD_FLY_INFO,										/* 飞行信息 */
    CMD_RESET_DEVICE,                                   /* 设备复位 */
    CMD_SEND_BIN_FILE_SIZE,                             /* 发送的固件文件大小 */
    CMD_SEND_BIN_FILE_PACK,                             /* 固件数据包 */
    CMD_SEND_BIN_FILE_END,                              /* 固件数据发送完成 */
    CMD_RUN_IAP,                                        /* 运行IAP程序 */
    CMD_NOW_IAP_STATE,									/* 当前运行IAP程序 */
    CMD_NOW_APP_STATE,                                  /* 当前运行APP程序 */
    CMD_LOG_MESSAGE,
    CMD_SHOW_DEVICE_INFO,
    CMD_ACK                                             /* 响应 */
};
/* 校准模式 */
enum calibrationMode
{
    ACCEL_CALIBRATION_MODE = 1,					/* 加速度计校准模式 */
    GYRO_CALIBRATION_MODE,                      /* 陀螺仪校准模式 */
    MAG_CALIBRATION_MODE						/* 磁力计校准模式 */
};
/* PID参数结构体 */
typedef struct
{
    float kp;								/* 比例系数 */
    float ki;								/* 积分项系数 */
    float kd;                               /* 微分先行 反馈量的系数 */
}_pid;

typedef struct
{
    double dataX;
    double dataY;
    double dataZ;
}_originalSensorForCalibrationData;
#endif // COMMON_H
