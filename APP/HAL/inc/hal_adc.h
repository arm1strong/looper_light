#ifndef __HAL_ADC_H
#define __HAL_ADC_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "stdint.h"

void hal_initADC(void);
uint16_t hal_getADC(uint32_t ch);

#endif
