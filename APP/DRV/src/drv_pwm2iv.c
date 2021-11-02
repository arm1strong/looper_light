#include "drv_pwm2iv.h"

/**
 * 设置电流输出通道占空比
 */
static void drv_setPWM2IV(PWM2IVn ch, uint8_t per)
{
	if(per>100)
	{
		return ;
	}
	
	hal_setPulse(ch, per);
}


/**
 * 初始化
 */
void drv_initPWM2IV(PWM2IV_Descriptor * pdescriptor)
{
	hal_initTimer();

	pdescriptor->write = drv_setPWM2IV;

	for(uint8_t i=0;i<sizeof(pdescriptor->ch);++i)
	{
		pdescriptor->write(i, pdescriptor->ch[i]);
	}
}
