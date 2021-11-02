#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f4xx_hal.h"
#include "stdint.h"


#define FLASH_FAIL    	1
#define FLASH_SUCCESS 	0

char hal_programFlash(uint32_t addr, char *buffer, uint16_t size);
void hal_readFlash(uint32_t addr, char *buffer, uint16_t size);
HAL_StatusTypeDef hal_eraseFlash(uint8_t sector, uint8_t sectornum);

#endif
