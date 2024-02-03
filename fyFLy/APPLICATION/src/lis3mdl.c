/*
 ******************************************************************************
 * @file    read_data_simple.c
 * @author  MEMS Software Solution Team
 * @date    27-September-2017
 * @brief   This file show the simplest way to get data from sensor.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "lis3mdl_reg.h"
#include <string.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"
#include "spi.h"

/* Private macro -------------------------------------------------------------*/

#define TX_BUF_DIM          1000

/* Private variables ---------------------------------------------------------*/
static axis3bit16_t data_raw_magnetic;
static axis1bit16_t data_raw_temperature;
static float magnetic_mG[3];
static float temperature_degC;
static uint8_t whoamI, rst;

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*
 *   Replace the functions "platform_write" and "platform_read" with your
 *   platform specific read and write function.
 *   This example use an STM32 evaluation board and CubeMX tool.
 *   In this case the "*handle" variable is usefull in order to select the
 *   correct interface but the usage uf "*handle" is not mandatory.
 */

static int32_t platform_write(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
	HAL_GPIO_WritePin(LIS3MD_CS_GPIO_Port, LIS3MD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(handle, &Reg, 1, 1000);
	HAL_SPI_Transmit(handle, Bufp, len, 1000);
	HAL_GPIO_WritePin(LIS3MD_CS_GPIO_Port, LIS3MD_CS_Pin, GPIO_PIN_SET);
  return 0;
}

static int32_t platform_read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
	Reg |= 0x80;
	HAL_GPIO_WritePin(LIS3MD_CS_GPIO_Port, LIS3MD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(handle, &Reg, 1, 1000);
	HAL_SPI_Receive(handle, Bufp, len, 1000);
	HAL_GPIO_WritePin(LIS3MD_CS_GPIO_Port, LIS3MD_CS_Pin, GPIO_PIN_SET);
  return 0;
}

/*
 *  Function to print messages
 */
void tx_com( uint8_t *tx_buffer, uint16_t len )
{
  #ifdef NUCLEO_STM32F411RE  
  HAL_UART_Transmit( &huart2, tx_buffer, len, 1000 );
  #endif
  #ifdef MKI109V2  
  CDC_Transmit_FS( tx_buffer, len );
  #endif
}

void lis3mdl_Init(void)
{
  /*
   *  Initialize mems driver interface
   */
  lis3mdl_ctx_t dev_ctx;
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.handle = &hspi1;  
  /*
   *  Check device ID
   */
  whoamI = 0;
  
  while ( whoamI != LIS3MDL_ID )
	{
		lis3mdl_device_id_get(&dev_ctx, &whoamI);
		/*manage here device not found */
	}
 
  /*
   *  Restore default configuration
   */
  lis3mdl_reset_set(&dev_ctx, PROPERTY_ENABLE);
  do {
    lis3mdl_reset_get(&dev_ctx, &rst);
  } while (rst);
	
	lis3mdl_operating_mode_set(&dev_ctx, LIS3MDL_POWER_DOWN);

  /*
   *  Enable Block Data Update
   */
  lis3mdl_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /*
   * Set Output Data Rate
   */
  lis3mdl_data_rate_set(&dev_ctx, LIS3MDL_LP_1kHz);
  /*
   * Set full scale
   */  
  lis3mdl_full_scale_set(&dev_ctx, LIS3MDL_16_GAUSS);
  /*
   * Enable temperature sensor
   */   
  lis3mdl_temperature_meas_set(&dev_ctx, PROPERTY_ENABLE);
  /*
   * Set device in continuos mode
   */   
  lis3mdl_operating_mode_set(&dev_ctx, LIS3MDL_CONTINUOUS_MODE);
  
  /*
   * Read samples in polling mode (no int)
   */
  while(1)
  {
    /*
     * Read output only if new value is available
     */
    lis3mdl_reg_t reg;
    lis3mdl_status_get(&dev_ctx, &reg.status_reg);

    if (reg.status_reg.zyxda)
    {
      /* Read magnetic field data */
      memset(data_raw_magnetic.u8bit, 0x00, 3*sizeof(int16_t));
      lis3mdl_magnetic_raw_get(&dev_ctx, data_raw_magnetic.u8bit);
      magnetic_mG[0] = 1000 * LIS3MDL_FROM_FS_16G_TO_G( data_raw_magnetic.i16bit[0]);
      magnetic_mG[1] = 1000 * LIS3MDL_FROM_FS_16G_TO_G( data_raw_magnetic.i16bit[1]);
      magnetic_mG[2] = 1000 * LIS3MDL_FROM_FS_16G_TO_G( data_raw_magnetic.i16bit[2]);
      
//      sprintf((char*)tx_buffer, "Magnetic field [mG]:%4.2f\t%4.2f\t%4.2f\r\n",
//              magnetic_mG[0], magnetic_mG[1], magnetic_mG[2]);
//      tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
//      
//      /* Read temperature data */
      memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
      lis3mdl_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
      temperature_degC = LIS3MDL_FROM_LSB_TO_degC( data_raw_temperature.i16bit );
//       
//      sprintf((char*)tx_buffer, "Temperature [degC]:%6.2f\r\n", temperature_degC );
//      tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    }
  }
}
