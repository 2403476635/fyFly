/******************** (C) COPYRIGHT 2017 ANO Tech ********************************
 * 作者    ：匿名科创
 * 官网    ：www.anotc.com
 * 淘宝    ：anotc.taobao.com
 * 技术Q群 ：190169595
 * 描述    ：SPL06气压计驱动
**********************************************************************************/
#include "spl06.h"
#include "spi.h"

static struct spl0601_t spl0601;
static struct spl0601_t *p_spl0601;

void spl0601_get_calib_param ( void );

/*****************************************************************************
 函 数 名  : spl0601_write
 功能描述  : I2C 寄存器写入子函数
 输入参数  : uint8 hwadr   硬件地址
             uint8 regadr  寄存器地址
             uint8 val     值
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
static void spl0601_write ( unsigned char regadr, unsigned char val )
{
	HAL_GPIO_WritePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &regadr, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &val, 1, 1000);
	HAL_GPIO_WritePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin, GPIO_PIN_SET);
}

/*****************************************************************************
 函 数 名  : spl0601_read
 功能描述  : I2C 寄存器读取子函数
 输入参数  : uint8 hwadr   硬件地址
             uint8 regadr  寄存器地址
 输出参数  :
 返 回 值  : uint8 读出值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
static uint8_t spl0601_read ( unsigned char regadr )
{
  uint8_t reg_data;
	regadr = regadr | 0x80;
	HAL_GPIO_WritePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin, GPIO_PIN_RESET);
	regadr = regadr|0x80;
	HAL_SPI_Transmit(&hspi1,&regadr,1,HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi1,&reg_data,1,HAL_MAX_DELAY);
	HAL_GPIO_WritePin(SPL06_CS_GPIO_Port, SPL06_CS_Pin, GPIO_PIN_SET);
  return reg_data;
}
/*****************************************************************************
 函 数 名  : spl0601_rateset
 功能描述  :  设置温度传感器的每秒采样次数以及过采样率
 输入参数  : uint8 u8OverSmpl  过采样率         Maximal = 128
             uint8 u8SmplRate  每秒采样次数(Hz) Maximal = 128
             uint8 iSensor     0: Pressure; 1: Temperature
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月24日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_rateset ( uint8_t iSensor, uint8_t u8SmplRate, uint8_t u8OverSmpl )
{
    uint8_t reg = 0;
    int i32kPkT = 0;
    switch ( u8SmplRate )
    {
    case 2:
        reg |= ( 1 << 4 );
        break;
    case 4:
        reg |= ( 2 << 4 );
        break;
    case 8:
        reg |= ( 3 << 4 );
        break;
    case 16:
        reg |= ( 4 << 4 );
        break;
    case 32:
        reg |= ( 5 << 4 );
        break;
    case 64:
        reg |= ( 6 << 4 );
        break;
    case 128:
        reg |= ( 7 << 4 );
        break;
    case 1:
    default:
        break;
    }
    switch ( u8OverSmpl )
    {
    case 2:
        reg |= 1;
        i32kPkT = 1572864;
        break;
    case 4:
        reg |= 2;
        i32kPkT = 3670016;
        break;
    case 8:
        reg |= 3;
        i32kPkT = 7864320;
        break;
    case 16:
        i32kPkT = 253952;
        reg |= 4;
        break;
    case 32:
        i32kPkT = 516096;
        reg |= 5;
        break;
    case 64:
        i32kPkT = 1040384;
        reg |= 6;
        break;
    case 128:
        i32kPkT = 2088960;
        reg |= 7;
        break;
    case 1:
    default:
        i32kPkT = 524288;
        break;
    }

    if ( iSensor == 0 )
    {
        p_spl0601->i32kP = i32kPkT;
        spl0601_write ( 0x06, reg );
        if ( u8OverSmpl > 8 )
        {
            reg = spl0601_read ( 0x09 );
            spl0601_write ( 0x09, reg | 0x04 );
        }
    }
    if ( iSensor == 1 )
    {
        p_spl0601->i32kT = i32kPkT;
        spl0601_write ( 0x07, reg | 0x80 ); //Using mems temperature
        if ( u8OverSmpl > 8 )
        {
            reg = spl0601_read ( 0x09 );
            spl0601_write ( 0x09, reg | 0x08 );
        }
    }

}
/*****************************************************************************
 函 数 名  : spl0601_get_calib_param
 功能描述  : 获取校准参数
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_get_calib_param ( void )
{
    uint32_t h;
    uint32_t m;
    uint32_t l;
    h =  spl0601_read ( 0x10 );
    l  =  spl0601_read ( 0x11 );
    p_spl0601->calib_param.c0 = ( short ) h << 4 | l >> 4;
    p_spl0601->calib_param.c0 = ( p_spl0601->calib_param.c0 & 0x0800 ) ? ( 0xF000 | p_spl0601->calib_param.c0 ) : p_spl0601->calib_param.c0;
    h =  spl0601_read ( 0x11 );
    l  =  spl0601_read ( 0x12 );
    p_spl0601->calib_param.c1 = ( short ) ( h & 0x0F ) << 8 | l;
    p_spl0601->calib_param.c1 = ( p_spl0601->calib_param.c1 & 0x0800 ) ? ( 0xF000 | p_spl0601->calib_param.c1 ) : p_spl0601->calib_param.c1;
    h =  spl0601_read ( 0x13 );
    m =  spl0601_read ( 0x14 );
    l =  spl0601_read ( 0x15 );
    p_spl0601->calib_param.c00 = ( int ) h << 12 | ( int ) m << 4 | ( int ) l >> 4;
    p_spl0601->calib_param.c00 = ( p_spl0601->calib_param.c00 & 0x080000 ) ? ( 0xFFF00000 | p_spl0601->calib_param.c00 ) : p_spl0601->calib_param.c00;
    h =  spl0601_read ( 0x15 );
    m =  spl0601_read ( 0x16 );
    l =  spl0601_read ( 0x17 );
    p_spl0601->calib_param.c10 = ( int ) h << 16 | ( int ) m << 8 | l;
    p_spl0601->calib_param.c10 = ( p_spl0601->calib_param.c10 & 0x080000 ) ? ( 0xFFF00000 | p_spl0601->calib_param.c10 ) : p_spl0601->calib_param.c10;
    h =  spl0601_read ( 0x18 );
    l  =  spl0601_read ( 0x19 );
    p_spl0601->calib_param.c01 = ( short ) h << 8 | l;
    h =  spl0601_read ( 0x1A );
    l  =  spl0601_read ( 0x1B );
    p_spl0601->calib_param.c11 = ( short ) h << 8 | l;
    h =  spl0601_read ( 0x1C );
    l  =  spl0601_read ( 0x1D );
    p_spl0601->calib_param.c20 = ( short ) h << 8 | l;
    h =  spl0601_read ( 0x1E );
    l  =  spl0601_read ( 0x1F );
    p_spl0601->calib_param.c21 = ( short ) h << 8 | l;
    h =  spl0601_read ( 0x20 );
    l  =  spl0601_read ( 0x21 );
    p_spl0601->calib_param.c30 = ( short ) h << 8 | l;
}
/*****************************************************************************
 函 数 名  : spl0601_start_temperature
 功能描述  : 发起一次温度测量
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_start_temperature ( void )
{
    spl0601_write ( 0x08, 0x02 );
}

/*****************************************************************************
 函 数 名  : spl0601_start_pressure
 功能描述  : 发起一次压力值测量
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_start_pressure ( void )
{
    spl0601_write ( 0x08, 0x01 );
}

/*****************************************************************************
 函 数 名  : spl0601_start_continuous
 功能描述  : Select node for the continuously measurement
 输入参数  : uint8 mode  1: pressure; 2: temperature; 3: pressure and temperature
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月25日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_start_continuous ( uint8_t mode )
{
    spl0601_write ( 0x08, mode + 4 );
}
/*****************************************************************************
 函 数 名  : spl0601_get_raw_temp
 功能描述  : 获取温度的原始值，并转换成32Bits整数
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_get_raw_temp ( void )
{
    uint8_t h[3] = {0};

    h[0] = spl0601_read ( 0x03 );
    h[1] = spl0601_read ( 0x04 );
    h[2] = spl0601_read ( 0x05 );

    p_spl0601->i32rawTemperature = ( int ) h[0] << 16 | ( int ) h[1] << 8 | ( int ) h[2];
    p_spl0601->i32rawTemperature = ( p_spl0601->i32rawTemperature & 0x800000 ) ? ( 0xFF000000 | p_spl0601->i32rawTemperature ) : p_spl0601->i32rawTemperature;
}

/*****************************************************************************
 函 数 名  : spl0601_get_raw_pressure
 功能描述  : 获取压力原始值，并转换成32bits整数
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
void spl0601_get_raw_pressure ( void )
{
    uint8_t h[3];

    h[0] = spl0601_read ( 0x00 );
    h[1] = spl0601_read ( 0x01 );
    h[2] = spl0601_read ( 0x02 );

    p_spl0601->i32rawPressure = ( int ) h[0] << 16 | ( int ) h[1] << 8 | ( int ) h[2];
    p_spl0601->i32rawPressure = ( p_spl0601->i32rawPressure & 0x800000 ) ? ( 0xFF000000 | p_spl0601->i32rawPressure ) : p_spl0601->i32rawPressure;
}
/*****************************************************************************
 函 数 名  : spl0601_init
 功能描述  : SPL06-01 初始化函数
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
//uint8_t test_spi[5];
uint8_t Drv_Spl0601Init ( void )
{
    p_spl0601 = &spl0601; /* read Chip Id */
    p_spl0601->i32rawPressure = 0;
    p_spl0601->i32rawTemperature = 0;
    p_spl0601->chip_id = spl0601_read ( 0x0D );// 0x34  0x10
	
		while(p_spl0601->chip_id != 0x10)
		{
			 p_spl0601->chip_id = spl0601_read ( 0x0D );
		}
	
    spl0601_get_calib_param();

    spl0601_rateset ( PRESSURE_SENSOR, 128, 16 );

    spl0601_rateset ( TEMPERATURE_SENSOR, 8, 8 );

    spl0601_start_continuous ( CONTINUOUS_P_AND_T );
	
	if(p_spl0601->chip_id == 0x10)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*****************************************************************************
 函 数 名  : spl0601_get_temperature
 功能描述  : 在获取原始值的基础上，返回浮点校准后的温度值
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
float spl0601_get_temperature ( void )
{
    float fTCompensate;
    float fTsc;

    fTsc = p_spl0601->i32rawTemperature / ( float ) p_spl0601->i32kT;
    fTCompensate =  p_spl0601->calib_param.c0 * 0.5 + p_spl0601->calib_param.c1 * fTsc;
    return fTCompensate;
}

/*****************************************************************************
 函 数 名  : spl0601_get_pressure
 功能描述  : 在获取原始值的基础上，返回浮点校准后的压力值
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : WL
    修改内容   : 新生成函数

*****************************************************************************/
float spl0601_get_pressure ( void )
{
    float fTsc, fPsc;
    float qua2, qua3;
    float fPCompensate;

    fTsc = p_spl0601->i32rawTemperature / ( float ) p_spl0601->i32kT;
    fPsc = p_spl0601->i32rawPressure / ( float ) p_spl0601->i32kP;
    qua2 = p_spl0601->calib_param.c10 + fPsc * ( p_spl0601->calib_param.c20 + fPsc * p_spl0601->calib_param.c30 );
    qua3 = fTsc * fPsc * ( p_spl0601->calib_param.c11 + fPsc * p_spl0601->calib_param.c21 );
    //qua3 = 0.9f *fTsc * fPsc * (p_spl0601->calib_param.c11 + fPsc * p_spl0601->calib_param.c21);

    fPCompensate = p_spl0601->calib_param.c00 + fPsc * qua2 + fTsc * p_spl0601->calib_param.c01 + qua3;
    //fPCompensate = p_spl0601->calib_param.c00 + fPsc * qua2 + 0.9f *fTsc  * p_spl0601->calib_param.c01 + qua3;
    return fPCompensate;
}



float baro_Offset, alt_3, height;
unsigned char baro_start;
float temperature, alt_high;
float baro_pressure;

float Drv_Spl0601_Read ( void )
{


    spl0601_get_raw_temp();
    temperature = spl0601_get_temperature();

    spl0601_get_raw_pressure();
    baro_pressure = spl0601_get_pressure();

    //alt_high = powf((temp/101325),1/5.255f);
/////////////////////////////////////////////////////////////

    alt_3 = ( 101400 - baro_pressure ) / 1000.0f;
    height = 0.82f * alt_3 * alt_3 * alt_3 + 0.09f * ( 101400 - baro_pressure ) * 100.0f ;


    alt_high = ( height - baro_Offset ) ; //cm +




/////////////////////////////////////////////////////////////



    return alt_high;
}
/* 读取SPL06的原始数据 */
void spl06ReadData(float *temperature,float *barPressure)
{
    spl0601_get_raw_temp();
    *temperature = spl0601_get_temperature();

    spl0601_get_raw_pressure();
    *barPressure = spl0601_get_pressure();
}




