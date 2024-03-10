#ifndef __FLY_RC_CTR_H__
#define __FLY_RC_CTR_H__

#include "main.h"
#include "bsp_sbus.h"
#include "common.h"
#include "motoCtr.h"

extern int16_t ch[CH_NUM];

void updateRcValue(int16_t *updateCh,const SBUS_CH_Struct getCh);
void updateRcState(_flyStateStruct *state,const uint16_t dtMs);

#endif /* __FLY_RC_CTR_H__ */




