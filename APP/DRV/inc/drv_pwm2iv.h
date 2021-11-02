#ifndef __PWM2IV_H
#define __PWM2IV_H

#include "hal_gpio.h"
#include "hal_timer.h"
#include "stm32f4xx_hal.h"

typedef enum
{
	PWM2IV_CH1,
	PWM2IV_CH2,
	PWM2IV_CH3,
	PWM2IV_CH4,
	PWM2IV_CH5,
	PWM2IV_CH6,
	PWM2IV_CH7,
	PWM2IV_CH8
}PWM2IVn;

typedef struct
{
	uint8_t ch[8]; //8通道电压 0~100
	void (*write)(PWM2IVn ch, uint8_t per);//设置占空比
}PWM2IV_Descriptor;

void drv_initPWM2IV(PWM2IV_Descriptor * pdescriptor);

#endif
