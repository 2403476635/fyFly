#ifndef __FY_CALIBRATION_H__ 
#define __FY_CALIBRATION_H__
#include "main.h"
#include "arm_math.h"

extern float32_t magOffset[3];
extern float32_t magCalibrationMatrix3x3[9];

void magMatrixCalculate(float32_t *magOriginalData, float32_t *magCalibrationData);

#endif /* __FY_CALIBRATION_H__ */




