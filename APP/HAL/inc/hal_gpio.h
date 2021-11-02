#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f4xx_hal.h"

/*GPIO 模式*/
typedef enum 
{
    INPUT,
    OUTPUT,
    EXTIMODE
}MODE;

/*EXTI触发沿*/
typedef enum 
{
    EGE_RISING  = 0x10110000u,  
    EGE_FALLING = 0x10210000u    
}EXTI_EGE;

/*GPIO结构体*/
typedef struct
{
    MODE            gpio_mode;
    GPIO_TypeDef *  gpio_port;
    uint16_t        gpio_pin;
    uint8_t         gpio_level;

    EXTI_EGE        exti_ege;       //外部中断出发沿
    uint8_t         exti_irqn;      //中断号
    uint8_t         exti_pre;       //主优先级
    uint8_t         exti_sub;       //子优先级
}GPIOs;


void hal_initGPIO(GPIOs gpio);
void hal_setGPIOLevel(GPIOs gpio, uint8_t level);
uint8_t hal_getGPIOLevel(GPIOs gpio);
void hal_toggleGPIOLevel(GPIOs gpio);

#endif
