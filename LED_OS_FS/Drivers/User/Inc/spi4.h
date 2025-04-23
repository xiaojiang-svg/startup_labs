#ifndef __SPI4_H
#define __SPI4_H

#include "stdint.h"


// SPI总线速度设置 
#define SPI_SPEED_2   		0
#define SPI_SPEED_4   		1
#define SPI_SPEED_8   		2
#define SPI_SPEED_16  		3
#define SPI_SPEED_32 			4
#define SPI_SPEED_64 			5
#define SPI_SPEED_128 		6
#define SPI_SPEED_256 		7
						  	    					
void MAG0_SPI4_Configuration(void);
void SPI4_SetSpeed(uint8_t SpeedSet);
uint8_t SPI4_ReadWriteByte(uint8_t TxData);

#endif

