#include "fyCalibration.h"

//float32_t magOffset[3] = { -271.864, -35.8921, 108.058};
float32_t magOffset[3] = { 0, 0, 0};
//float32_t magCalibrationMatrix3x3[9] = 
//{
//	0.918419,		0.0207589, 		0.0464997, 
//	0.0207589,	0.962683, 		-0.000712886,
//	0.0464997,	-0.000712886, 0.969636 
//};
float32_t magCalibrationMatrix3x3[9] = {0,0,0,0,0,0,0,0,0};
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




