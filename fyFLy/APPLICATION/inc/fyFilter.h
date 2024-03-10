#ifndef __FY_FILTER_H__
#define __FY_FILTER_H__

#include "arm_math.h"

void armIirFilterInit(void);
void armIirFilter(float32_t *inputF32, float32_t *outputF32);


#endif /* __FY_FILTER_H__ */




