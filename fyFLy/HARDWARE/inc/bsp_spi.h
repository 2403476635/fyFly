#ifndef __BSP_SPI_H
#define __BSP_SPI_H


#include "main.h"

//#ifndef ENABLE
//#define ENABLE              1
//#endif /* ENABLE */

//#ifndef DISABLE
//#define DISABLE             0
//#endif /* DISABLE */

#ifndef HIGH
#define HIGH                1
#endif /* HIGH */

#ifndef LOW
#define LOW                 0
#endif /* LOW */

#define SL_SPI_SCK_CLK		__HAL_RCC_GPIOA_CLK_ENABLE()
#define SL_SPI_SCK_PORT		GPIOA
#define SL_SPI_SCK_PINS		GPIO_PIN_5

#define SL_SPI_MOSI_CLK		__HAL_RCC_GPIOA_CLK_ENABLE()
#define SL_SPI_MOSI_PORT	GPIOA
#define SL_SPI_MOSI_PINS	GPIO_PIN_7

#define SL_SPI_MISO_CLK		__HAL_RCC_GPIOA_CLK_ENABLE()
#define SL_SPI_MISO_PORT	GPIOA
#define SL_SPI_MISO_PINS	GPIO_PIN_6

#define SPI_SCK(x)			HAL_GPIO_WritePin(SL_SPI_SCK_PORT, SL_SPI_SCK_PINS, (GPIO_PinState)x)
#define SPI_MOSI(x)			HAL_GPIO_WritePin(SL_SPI_MOSI_PORT, SL_SPI_MOSI_PINS,(GPIO_PinState)x)
#define SPI_MISO				HAL_GPIO_ReadPin(SL_SPI_MISO_PORT, SL_SPI_MISO_PINS)

uint8_t Write_SPI_Byte( uint8_t Byte );
uint8_t Read_SPI_Byte(void);
void Simulate_SPI_Config(void);


#endif /* __BSP_SPI_H */


/*---------------------------- END OF FILE ----------------------------*/


