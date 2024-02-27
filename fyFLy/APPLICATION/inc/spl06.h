#ifndef _DRV_SPL06_H_
#define _DRV_SPL06_H_

#include "main.h"

#define CONTINUOUS_PRESSURE     1
#define CONTINUOUS_TEMPERATURE  2
#define CONTINUOUS_P_AND_T      3
#define PRESSURE_SENSOR     0
#define TEMPERATURE_SENSOR  1

struct spl0601_calib_param_t {	
    short c0;
    short c1;
    int c00;
    int c10;
    short c01;
    short c11;
    short c20;
    short c21;
    short c30;       
};

struct spl0601_t {	
    struct spl0601_calib_param_t calib_param;/**<calibration data*/	
    uint8_t 			chip_id; /**<chip id*/	
    int 	i32rawPressure;
    int 	i32rawTemperature;
    int 	i32kP;    
    int 	i32kT;
};

void Drv_SPL06CSPinInit(void);
uint8_t Drv_Spl0601Init(void);
float Drv_Spl0601_Read(void);
float spl0601_get_temperature(void);
float spl0601_get_pressure(void);
float Drv_Spl0601_Read (void);

void spl06ReadData(float *temperature,float *barPressure);
#endif
