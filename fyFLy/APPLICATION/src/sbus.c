#include "sbus.h"

SBUS_CH_Struct SBUS_CH;
uint16_t CH[18];
//将sbus信号转化为通道值

int16_t aux3 = 0;

uint8_t sbusDataAnalysis(uint8_t *buf)
{
    int i;
    if (buf[0] == 0x0F && buf[24] == 0)
    {
        SBUS_CH.ConnectState = 1;
			
				SBUS_CH.CH1 = (int16_t)(buf[2] & 0x07) << 8 | buf[1];
				SBUS_CH.CH2 = (int16_t)(buf[3] & 0x3f) << 5 | (buf[2] >> 3);
				SBUS_CH.CH3 = (int16_t)(buf[5] & 0x01) << 10 | ((int16_t)buf[4] << 2) | (buf[3] >> 6);
				SBUS_CH.CH4 = (int16_t)(buf[6] & 0x0F) << 7 | (buf[5] >> 1);
				SBUS_CH.CH5 = (int16_t)(buf[7] & 0x7F) << 4 | (buf[6] >> 4);
				SBUS_CH.CH6 = (int16_t)(buf[9] & 0x03) << 9 | ((int16_t)buf[8] << 1) | (buf[7] >> 7);
				SBUS_CH.CH7 = (int16_t)(buf[10] & 0x1F) << 6 | (buf[9] >> 2);
				SBUS_CH.CH8 = (int16_t)buf[11] << 3 | (buf[10] >> 5);
				
				SBUS_CH.CH9 = (int16_t)(buf[13] & 0x07) << 8 | buf[12];
				SBUS_CH.CH10 = (int16_t)(buf[14] & 0x3f) << 5 | (buf[13] >> 3);
				SBUS_CH.CH11 = (int16_t)(buf[16] & 0x01) << 10 | ((int16_t)buf[15] << 2) | (buf[14] >> 6);
				SBUS_CH.CH12 = (int16_t)(buf[17] & 0x0F) << 7 | (buf[16] >> 1);
				SBUS_CH.CH13 = (int16_t)(buf[18] & 0x7F) << 4 | (buf[17] >> 4);
				SBUS_CH.CH14 = (int16_t)(buf[20] & 0x03) << 9 | ((int16_t)buf[19] << 1) | (buf[18] >> 7);
				SBUS_CH.CH15 = (int16_t)(buf[21] & 0x1F) << 6 | (buf[20] >> 2);
				SBUS_CH.CH16 = (int16_t)buf[22] << 3 | (buf[21] >> 5);
				
				aux3 = 0.65f * (SBUS_CH.CH3 - 1024);
				
        return 1;
    }
    else 
    {
        SBUS_CH.ConnectState = 0;
        return 0;
    }
}
void Sbus_Decode(uint8_t data)
{
  static uint8_t i;
  static uint8_t DataCnt  = 0;
  static uint8_t SUBS_RawData[25];
  /*接收数据*/
  SUBS_RawData[DataCnt++] = data;

  /*每帧数据长度为25*/
  if(DataCnt >= 25) {
    /*判断帧头帧尾是否正确 只要有一个不正确就退出函数*/
    if(SUBS_RawData[0] == 0x0F && SUBS_RawData[24] == 0) 
		{
      DataCnt = 0;
      SBUS_CH.CH1 = (int16_t)(SUBS_RawData[2] & 0x07) << 8 | SUBS_RawData[1];
      SBUS_CH.CH2 = (int16_t)(SUBS_RawData[3] & 0x3f) << 5 | (SUBS_RawData[2] >> 3);
      SBUS_CH.CH3 = (int16_t)(SUBS_RawData[5] & 0x01) << 10 | ((int16_t)SUBS_RawData[4] << 2) | (SUBS_RawData[3] >> 6);
      SBUS_CH.CH4 = (int16_t)(SUBS_RawData[6] & 0x0F) << 7 | (SUBS_RawData[5] >> 1);
      SBUS_CH.CH5 = (int16_t)(SUBS_RawData[7] & 0x7F) << 4 | (SUBS_RawData[6] >> 4);
      SBUS_CH.CH6 = (int16_t)(SUBS_RawData[9] & 0x03) << 9 | ((int16_t)SUBS_RawData[8] << 1) | (SUBS_RawData[7] >> 7);
      SBUS_CH.CH7 = (int16_t)(SUBS_RawData[10] & 0x1F) << 6 | (SUBS_RawData[9] >> 2);
      SBUS_CH.CH8 = (int16_t)SUBS_RawData[11] << 3 | (SUBS_RawData[10] >> 5);

      SBUS_CH.CH9 = (int16_t)(SUBS_RawData[13] & 0x07) << 8 | SUBS_RawData[12];
      SBUS_CH.CH10 = (int16_t)(SUBS_RawData[14] & 0x3f) << 5 | (SUBS_RawData[13] >> 3);
      SBUS_CH.CH11 = (int16_t)(SUBS_RawData[16] & 0x01) << 10 | ((int16_t)SUBS_RawData[15] << 2) | (SUBS_RawData[14] >> 6);
      SBUS_CH.CH12 = (int16_t)(SUBS_RawData[17] & 0x0F) << 7 | (SUBS_RawData[16] >> 1);
      SBUS_CH.CH13 = (int16_t)(SUBS_RawData[18] & 0x7F) << 4 | (SUBS_RawData[17] >> 4);
      SBUS_CH.CH14 = (int16_t)(SUBS_RawData[20] & 0x03) << 9 | ((int16_t)SUBS_RawData[19] << 1) | (SUBS_RawData[18] >> 7);
      SBUS_CH.CH15 = (int16_t)(SUBS_RawData[21] & 0x1F) << 6 | (SUBS_RawData[20] >> 2);
      SBUS_CH.CH16 = (int16_t)SUBS_RawData[22] << 3 | (SUBS_RawData[21] >> 5);
//      sbus_flag = SUBS_RawData[23];
      /*一帧数据解析完成*/ 
      //user 
//      if(sbus_flag & 0x04) {
//        //如果有数据且能接收到有失控标记，则不处理，转嫁成无数据失控。
//      } else {
//        //遥控器在线
//        receivedNewData(); 
//      }
			//接收机在线处理
    } 
		else 
		{
      for( i = 0; i < 24; i++)
        SUBS_RawData[i] = SUBS_RawData[i + 1];

      DataCnt = 24;
    }
  }

}
uint16_t sbus_to_pwm(uint16_t sbus_value)
{
//    float pwm;
//    pwm = (float)SBUS_TARGET_MIN + (float)(sbus_value - SBUS_RANGE_MIN) * SBUS_SCALE_FACTOR;
//    //                1000                                   300              1000/1400
//    if (pwm > 2000) pwm = 2000;
//    if (pwm < 1000) pwm = 1000;
//    return (uint16_t)pwm;
}


