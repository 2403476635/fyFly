#ifndef __BSP_ICM42670P_H__
#define __BSP_ICM42670P_H__


#include "main.h"
#include "spi.h"
#include "imu/inv_imu_driver.h"
#include "imu/inv_imu_transport.h"
#include "imu/inv_imu_apex.h"

// This defines the handler called when retrieving a sample from the FIFO
typedef void (*ICM42670P_sensor_event_cb)(inv_imu_sensor_event_t *event);
// This defines the handler called when receiving an irq
typedef void (*ICM42670P_irq_handler)(void);


int icm42670p_Init(void);
	
int startAccel(uint16_t odr, uint16_t fsr);
int startGyro(uint16_t odr, uint16_t fsr);
int getDataFromRegisters(inv_imu_sensor_event_t *evt);
int enableFifoInterrupt(uint8_t intpin, ICM42670P_irq_handler handler, uint8_t fifo_watermark);
int getDataFromFifo(ICM42670P_sensor_event_cb event_cb);
bool isAccelDataValid(inv_imu_sensor_event_t *evt);
bool isGyroDataValid(inv_imu_sensor_event_t *evt);
int startTiltDetection(uint8_t intpin, ICM42670P_irq_handler handler);
int startPedometer(uint8_t intpin, ICM42670P_irq_handler handler);
//int getPedometer(uint32_t &step_count, float &step_cadence, char* &activity);
int startWakeOnMotion(uint8_t intpin, ICM42670P_irq_handler handler);
		
ACCEL_CONFIG0_ODR_t accel_freq_to_param(uint16_t accel_freq_hz);
GYRO_CONFIG0_ODR_t gyro_freq_to_param(uint16_t gyro_freq_hz);
ACCEL_CONFIG0_FS_SEL_t accel_fsr_g_to_param(uint16_t accel_fsr_g);
GYRO_CONFIG0_FS_SEL_t gyro_fsr_dps_to_param(uint16_t gyro_fsr_dps);
int initApex(uint8_t intpin, ICM42670P_irq_handler handler);

		
#endif /* __ICM42670P_H__ */




