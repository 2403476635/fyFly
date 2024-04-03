#ifndef __FY_FILTER_H__
#define __FY_FILTER_H__

#include "arm_math.h"

extern float accelAndGyroDataInBuffer[6];
extern float accelAndGyroOutBuffer[6];
void armIirFilterInit(void);
void armIirFilter(float32_t *inputF32, float32_t *outputF32);

void accelAndGyroFilterInit(void);
void accelAndGyroFilter(void);

#endif /* __FY_FILTER_H__ */




