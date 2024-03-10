#include "flyRcCtr.h"

int16_t ch[CH_NUM] = {0};

/* 更新遥控器接收的值 */
void updateRcValue(int16_t *updateCh,const SBUS_CH_Struct getCh)
{
	/* 最小为240，最大为1800 中值1024  (+/-)500 */
	
	/* 遥控器设置美国手 */
	updateCh[CH_PITCH] = LIMT(0.7f * (getCh.CH2 - 1024),-500,500);
	updateCh[CH_ROLL]  = LIMT(0.7f * (getCh.CH1 - 1024),-500,500);
	updateCh[CH_YAW]   = LIMT(0.7f * (getCh.CH4 - 1024),-500,500);
	updateCh[CH_THR]   = LIMT(0.7f * (getCh.CH3 - 1024),-500,500);	
	/* 辅助通道 */
	updateCh[CH_AUX1] = LIMT(0.7f * (getCh.CH5 - 1024),-500,500);
	updateCh[CH_AUX2] = LIMT(0.7f * (getCh.CH6 - 1024),-500,500);
	updateCh[CH_AUX3] = LIMT(0.7f * (getCh.CH7 - 1024),-500,500);
	updateCh[CH_AUX4] = LIMT(0.7f * (getCh.CH8 - 1024),-500,500);
	
}
/* 根据遥控器的值改变飞机的状态 */
void updateRcState(_flyStateStruct *state,const uint16_t dtMs)
{
	static uint16_t lockTimeCnt = 0;		
	static uint16_t unlockTimeCnt = 0;
	static uint16_t idingTimeCnt = 0;
	
	if(ch[CH_AUX1] > 400)
	{
		state->flyTakeOffState = EMERGENCY_STOP; 		/* 急停通道，全局赋值一次，重新上电清除 */
		state->flyLockState = FLY_LOCK;
		return ;
	}
	
	if(state->flyLockState == FLY_LOCK) 					/* 飞控当前为上锁状态 */
	{
		if(state->flyTakeOffState == MOTO_STOP)			/* 未起飞 */
		{
			/* 内八解锁 解锁！ */
			if(ch[CH_THR] < -400 && ch[CH_YAW] > 400 && ch[CH_ROLL] < -400 && ch[CH_PITCH] < -400)
			{
				unlockTimeCnt += dtMs;
				if(unlockTimeCnt > 2000)								/* 检测两秒 */
				{
					state->flyLockState = FLY_UNLOCK;			/* 解锁标志置位 */
					state->flyTakeOffState = MOTO_IDING;	/* 电机怠速 */
					unlockTimeCnt = lockTimeCnt = idingTimeCnt = 0;
				}
			}
	
			/* 校准模式  */
			if(ch[CH_THR] > 400 && ch[CH_YAW] < -400 && ch[CH_AUX2] > 400)
			{
				state->flyTakeOffState = ELECTRICAL_CAIBRATION;
			}
		}
	}
	else if(state->flyLockState == FLY_UNLOCK)		/* 飞控当前为解锁状态 */
	{
		if(MOTO_IDING == state->flyTakeOffState)		/* 怠速电机从M1~M4 间隔转动 */
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
				/* 外八上锁 上锁！ */
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

