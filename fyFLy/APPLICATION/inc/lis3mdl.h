#ifndef _LIS3MD_H_
#define _LIS3MD_H_

void lis3mdl_Init(void);
void lis3mdlRead(float *temperature, float *mag_x, float *mag_y, float *mag_z);

#endif


