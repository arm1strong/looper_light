#ifndef __LED_H
#define __LED_H
#include "hal_gpio.h"
#include "stm32f4xx_hal.h"

typedef enum
{
    LED_SYS = 0,
    LED_NET,
    LED_DATA
}LEDn;

typedef struct
{
	char toggle;	  //翻转标志
    LEDn ledNmb;      //LED编号
    uint32_t tick;    //闪烁时钟
    uint16_t period;  //周期闪烁时间 0代表不闪烁
    int16_t times;   //闪烁次数 0xFFFF 表示一直 闪烁
    void (*write)(char level);
    char (*read)(void);
}LED_Descriptor;

void drv_initLED(LED_Descriptor * pdescriptor);
void drv_pollLEDEvent(LED_Descriptor * pdescriptor);

#endif
