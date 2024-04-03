#include "saveParameter.h"
#include "fyAttitudeCtr.h"
#include "w25qxx.h"

void saveParameterHandle(void)
{
	memcpy(&saveParameter.saveAngularVelocityPitchPid,&angularVelocityPitchPid,sizeof(_pid));
	memcpy(&saveParameter.saveAngularVelocityRollPid,&angularVelocityRollPid,sizeof(_pid));
	memcpy(&saveParameter.saveAngularVelocityYawPid,&angularVelocityYawPid,sizeof(_pid));
	
	W25QXX_Write((uint8_t *)&saveParameter,0,sizeof(_saveParameter));	/* 写入FLASH */
	W25QXX_Read((uint8_t *)&saveParameter,0x00,sizeof(_saveParameter));		/* 读取数据 */
}
