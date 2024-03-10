#ifndef _BSP_LIS3MD_H_
#define _BSP_LIS3MD_H_

uint8_t lis3mdl_Init(void);
void lis3mdlRead(float *temperature, float *mag_x, float *mag_y, float *mag_z);

#endif


