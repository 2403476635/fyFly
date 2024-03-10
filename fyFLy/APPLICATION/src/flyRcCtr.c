#include "flyRcCtr.h"

int16_t ch[CH_NUM] = {0};

/* ����ң�������յ�ֵ */
void updateRcValue(int16_t *updateCh,const SBUS_CH_Struct getCh)
{
	/* ��СΪ240�����Ϊ1800 ��ֵ1024  (+/-)500 */
	
	/* ң�������������� */
	updateCh[CH_PITCH] = LIMT(0.7f * (getCh.CH2 - 1024),-500,500);
	updateCh[CH_ROLL]  = LIMT(0.7f * (getCh.CH1 - 1024),-500,500);
	updateCh[CH_YAW]   = LIMT(0.7f * (getCh.CH4 - 1024),-500,500);
	updateCh[CH_THR]   = LIMT(0.7f * (getCh.CH3 - 1024),-500,500);	
	/* ����ͨ�� */
	updateCh[CH_AUX1] = LIMT(0.7f * (getCh.CH5 - 1024),-500,500);
	updateCh[CH_AUX2] = LIMT(0.7f * (getCh.CH6 - 1024),-500,500);
	updateCh[CH_AUX3] = LIMT(0.7f * (getCh.CH7 - 1024),-500,500);
	updateCh[CH_AUX4] = LIMT(0.7f * (getCh.CH8 - 1024),-500,500);
	
}
/* ����ң������ֵ�ı�ɻ���״̬ */
void updateRcState(_flyStateStruct *state,const uint16_t dtMs)
{
	static uint16_t lockTimeCnt = 0;		
	static uint16_t unlockTimeCnt = 0;
	static uint16_t idingTimeCnt = 0;
	
	if(ch[CH_AUX1] > 400)
	{
		state->flyTakeOffState = EMERGENCY_STOP; 		/* ��ͣͨ����ȫ�ָ�ֵһ�Σ������ϵ���� */
		state->flyLockState = FLY_LOCK;
		return ;
	}
	
	if(state->flyLockState == FLY_LOCK) 					/* �ɿص�ǰΪ����״̬ */
	{
		if(state->flyTakeOffState == MOTO_STOP)			/* δ��� */
		{
			/* �ڰ˽��� ������ */
			if(ch[CH_THR] < -400 && ch[CH_YAW] > 400 && ch[CH_ROLL] < -400 && ch[CH_PITCH] < -400)
			{
				unlockTimeCnt += dtMs;
				if(unlockTimeCnt > 2000)								/* ������� */
				{
					state->flyLockState = FLY_UNLOCK;			/* ������־��λ */
					state->flyTakeOffState = MOTO_IDING;	/* ������� */
					unlockTimeCnt = lockTimeCnt = idingTimeCnt = 0;
				}
			}
	
			/* У׼ģʽ  */
			if(ch[CH_THR] > 400 && ch[CH_YAW] < -400 && ch[CH_AUX2] > 400)
			{
				state->flyTakeOffState = ELECTRICAL_CAIBRATION;
			}
		}
	}
	else if(state->flyLockState == FLY_UNLOCK)		/* �ɿص�ǰΪ����״̬ */
	{
		if(MOTO_IDING == state->flyTakeOffState)		/* ���ٵ����M1~M4 ���ת�� */
		{
			idingTimeCnt += dtMs;
			if(idingTimeCnt < 400)
			{
				outPwmValue[0] = systemInfo.idleThrottleValue;
			}
			else if(idingTimeCnt < 800)
			{
				outPwmValue[1] = systemInfo.idleThrottleValue;
			}
			else if(idingTimeCnt < 1200)
			{
				outPwmValue[2] = systemInfo.idleThrottleValue;
			}
			else if(idingTimeCnt < 1600)
			{
				outPwmValue[3] = systemInfo.idleThrottleValue;
			}
			else if(idingTimeCnt > 2000)
			{
				state->flyTakeOffState = WAIT_TAKE_OFF;
				unlockTimeCnt = lockTimeCnt = idingTimeCnt = 0;
			}
		}
		else if(MOTO_STOP == state->flyTakeOffState || WAIT_TAKE_OFF == state->flyTakeOffState)
		{
				/* ������� ������ */
			if(ch[CH_THR] > 400 && ch[CH_YAW] < -400 && ch[CH_ROLL] > 400 && ch[CH_PITCH] > 400)
			{
				lockTimeCnt += dtMs;
				if(lockTimeCnt > 5000)
				{
					state->flyLockState = FLY_LOCK;
					state->flyTakeOffState = MOTO_STOP;
					outPwmValue[0] = outPwmValue[1] = outPwmValue[2] = outPwmValue[3] = 0;
					unlockTimeCnt = lockTimeCnt = idingTimeCnt = 0;
				}
			}
		}
	}
	
	/*  */
}

