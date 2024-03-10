#include "bsp_sbus.h"

SBUS_CH_Struct sbusCh;
uint8_t sbusBuffer[25];

uint8_t sbusDataAnalysis(uint8_t *buf)
{
    int i;
    if (buf[0] == 0x0F && buf[24] == 0)
    {
        sbusCh.ConnectState = 1;
			
				sbusCh.CH1 = (int16_t)(buf[2] & 0x07) << 8 | buf[1];
				sbusCh.CH2 = (int16_t)(buf[3] & 0x3f) << 5 | (buf[2] >> 3);
				sbusCh.CH3 = (int16_t)(buf[5] & 0x01) << 10 | ((int16_t)buf[4] << 2) | (buf[3] >> 6);
				sbusCh.CH4 = (int16_t)(buf[6] & 0x0F) << 7 | (buf[5] >> 1);
				sbusCh.CH5 = (int16_t)(buf[7] & 0x7F) << 4 | (buf[6] >> 4);
				sbusCh.CH6 = (int16_t)(buf[9] & 0x03) << 9 | ((int16_t)buf[8] << 1) | (buf[7] >> 7);
				sbusCh.CH7 = (int16_t)(buf[10] & 0x1F) << 6 | (buf[9] >> 2);
				sbusCh.CH8 = (int16_t)buf[11] << 3 | (buf[10] >> 5);
				sbusCh.CH9 = (int16_t)(buf[13] & 0x07) << 8 | buf[12];
				sbusCh.CH10 = (int16_t)(buf[14] & 0x3f) << 5 | (buf[13] >> 3);
				sbusCh.CH11 = (int16_t)(buf[16] & 0x01) << 10 | ((int16_t)buf[15] << 2) | (buf[14] >> 6);
				sbusCh.CH12 = (int16_t)(buf[17] & 0x0F) << 7 | (buf[16] >> 1);
				sbusCh.CH13 = (int16_t)(buf[18] & 0x7F) << 4 | (buf[17] >> 4);
				sbusCh.CH14 = (int16_t)(buf[20] & 0x03) << 9 | ((int16_t)buf[19] << 1) | (buf[18] >> 7);
				sbusCh.CH15 = (int16_t)(buf[21] & 0x1F) << 6 | (buf[20] >> 2);
				sbusCh.CH16 = (int16_t)buf[22] << 3 | (buf[21] >> 5);
				
        return 1;
    }
    else 
    {
        sbusCh.ConnectState = 0;
        return 0;
    }
}




