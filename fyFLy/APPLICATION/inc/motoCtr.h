#ifndef __MOTO_CTR_H__
#define __MOTO_CTR_H__

#include "tim.h"
#include "main.h"
#include "common.h"

#define PWM_MAX_RANGE 2000

extern int32_t outPwmValue[4];

void motoInit(void);
void electricalCalibration(const int16_t *channel,_flyStateStruct *state);
void motoPowerOut(float valueCh1, float valueCh2, float valueCh3, float valueCh4,const _flyStateStruct *state);

#endif /* __MOTO_CTR_H__ */




