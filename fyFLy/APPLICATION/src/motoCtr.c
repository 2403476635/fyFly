#include "motoCtr.h"

int32_t outPwmValue[4] = {0,0,0,0};

static void setMotoValue(float valueCh1, float valueCh2, float valueCh3, float valueCh4)
{
	float outValueCh1 = valueCh1 + 2000;
	float outValueCh2 = valueCh2 + 2000;
	float outValueCh3 = valueCh3 + 2000;
	float outValueCh4 = valueCh4 + 2000;
	
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,outValueCh1);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2,outValueCh2);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3,outValueCh3);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4,outValueCh4);
}

void motoInit(void)
{
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
	
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,0);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2,0);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3,0);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4,0);
}

void electricalCalibration(const int16_t *channel,_flyStateStruct *state)
{
	if(ELECTRICAL_CAIBRATION == state->flyTakeOffState)	
	{
		if(channel[CH_AUX4] > 400)				/* 输出油门最低值 */
		{
			outPwmValue[0] = outPwmValue[1] = outPwmValue[2] = outPwmValue[3] = 0;
			state->flyTakeOffState = MOTO_STOP;	/* 校准完成 */
		}
		else
		{
			outPwmValue[0] = outPwmValue[1] = outPwmValue[2] = outPwmValue[3] = PWM_MAX_RANGE;
		}
	}
}

void motoPowerOut(float valueCh1, float valueCh2, float valueCh3, float valueCh4,const _flyStateStruct *state)
{
	if(EMERGENCY_STOP == state->flyTakeOffState)	/* 急停！！！电机一定停止 */
	{
		setMotoValue(0,0,0,0);
	}
	else if(FLY_UNLOCK == state->flyLockState)		/* 飞控解锁之后才能给电机功率输出 */
	{
		setMotoValue(valueCh1,valueCh2,valueCh3,valueCh4);
	}
	else if(FLY_LOCK == state->flyLockState && ELECTRICAL_CAIBRATION == state->flyTakeOffState)
	{
		setMotoValue(valueCh1,valueCh2,valueCh3,valueCh4);
	}
	else
	{
		setMotoValue(0,0,0,0);
	}
	
}
