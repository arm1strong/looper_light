#ifndef __HAL_WDG_H
#define __HAL_WDG_H

#include "stm32f4xx_hal.h"
#include "stdint.h"

void hal_initWdg(void);
void hal_feedWdg(void);

#endif
