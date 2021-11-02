#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f4xx_hal.h"

typedef enum
{
    PWM_CH1,
    PWM_CH2,
    PWM_CH3,
    PWM_CH4,
    PWM_CH5,
    PWM_CH6,
    PWM_CH7,
    PWM_CH8
}PWMn;

void hal_initTimer(void);
void hal_setPulse(PWMn pwm, uint8_t per);

#endif
