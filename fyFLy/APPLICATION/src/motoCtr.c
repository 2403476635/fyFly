#include "motoCtr.h"

uint16_t outPwmValue[4] = {0,0,0,0};

static void setMotoValue(float valueCh1, float valueCh2, float valueCh3, float valueCh4)
{
	valueCh1 += 2000;
	valueCh2 += 2000;
	valueCh3 += 2000;
	valueCh4 += 2000;
	
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,valueCh1);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2,valueCh2);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3,valueCh3);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4,valueCh4);
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
		if(channel[CH_AUX4] > 400)				/* ����������ֵ */
		{
			setMotoValue(0,0,0,0);
			state->flyTakeOffState = MOTO_STOP;	/* У׼��� */
		}
		else
		{
			setMotoValue(PWM_MAX_RANGE,PWM_MAX_RANGE,PWM_MAX_RANGE,PWM_MAX_RANGE);
		}
	}
}

void motoPowerOut(float valueCh1, float valueCh2, float valueCh3, float valueCh4,const _flyStateStruct *state)
{
	if(EMERGENCY_STOP == state->flyTakeOffState)	/* ��ͣ���������һ��ֹͣ */
	{
		setMotoValue(0,0,0,0);
	}
	else if(FLY_UNLOCK == state->flyLockState)		/* �ɿؽ���֮����ܸ����������� */
	{
		setMotoValue(valueCh1,valueCh2,valueCh3,valueCh4);
	}
	else
	{
		setMotoValue(0,0,0,0);
	}
	
}
