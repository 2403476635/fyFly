#include "fyCalibration.h"
#include "saveParameter.h"

float gyroOffset[3] = {0, 0, 0};

float32_t accelOffset[3] = {0, 0, 0};
float32_t accelCalibrationMatrix3x3[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};


void calibrationInit(void)
{
	for(uint8_t i=0;i<3;i++) gyroOffset[i] = saveParameter.gyroOffset[i];
	
	for(uint8_t i=0;i<3;i++) accelOffset[i] = saveParameter.accelOffset[i];
	for(uint8_t i=0;i<9;i++) accelCalibrationMatrix3x3[i] = saveParameter.accelCalibrationMatrix3x3[i];
	
	for(uint8_t i=0;i<3;i++) magOffset[i] = saveParameter.magOffset[i];
	for(uint8_t i=0;i<9;i++) magCalibrationMatrix3x3[i] = saveParameter.magCalibrationMatrix3x3[i];
}

float32_t magOffset[3] = {0, 0, 0};
float32_t magCalibrationMatrix3x3[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

arm_matrix_instance_f32 mat3x1;
arm_matrix_instance_f32 mat3x3;
arm_matrix_instance_f32 resultMat3x1;
/* 磁力计矫正的矩阵计算 */
void magMatrixCalculate(float32_t *magOriginalData, float32_t *magCalibrationData)
{
	magOriginalData[0] -= magOffset[0];
	magOriginalData[1] -= magOffset[1];
	magOriginalData[2] -= magOffset[2];
	
	arm_mat_init_f32(&mat3x1,3,1,magOriginalData);
	arm_mat_init_f32(&mat3x3,3,3,magCalibrationMatrix3x3);
	arm_mat_init_f32(&resultMat3x1,3,1,magCalibrationData);
	arm_mat_mult_f32(&mat3x3, &mat3x1, &resultMat3x1);
	
}




