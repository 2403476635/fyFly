#ifndef __SAVE_PARAMETER_H__
#define __SAVE_PARAMETER_H__

#include "common.h"
#include "fyPid.h"

typedef struct 
{
	_pid saveAngularVelocityPitchPid;
	_pid saveAngularVelocityRollPid;
  _pid saveAngularVelocityYawPid;
}__attribute__((packed)) _saveParameter;

extern _saveParameter saveParameter;

void saveParameterHandle(void);

#endif /* __SAVE_PARAMETER_H__ */




