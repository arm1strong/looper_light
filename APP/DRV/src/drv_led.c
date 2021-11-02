#include "drv_led.h"
#include "app_device.h"
#include "stm32f4xx_hal.h"

GPIOs LED_Sys_State = {OUTPUT, GPIOE, GPIO_PIN_4, 1, 0, 0, 0, 0};
GPIOs LED_Net_State = {OUTPUT, GPIOE, GPIO_PIN_3, 1, 0, 0, 0, 0};
GPIOs LED_Data_State = {OUTPUT, GPIOE, GPIO_PIN_2, 1, 0, 0, 0, 0}; //跳传数据指示灯
/**
 * 写Sys LED状态
 */
static void drv_writeSysLED(char level)
{
    hal_setGPIOLevel(LED_Sys_State, level);
}

/**
 * 写Net LED状态
 */
static void drv_writeNetLED(char level)
{
    hal_setGPIOLevel(LED_Net_State, level);
}

/**
 * 写跳传 LED状态
 */
static void drv_writeDataLED(char level)
{
    hal_setGPIOLevel(LED_Data_State, level);
}

/**
 * 读Sys LED状态
 */
static char drv_readSysLED(void)
{
    return LED_Sys_State.gpio_port->ODR & LED_Sys_State.gpio_pin ? 1 : 0;
}

/**
 * 写Net LED状态
 */
static char drv_readNetLED(void)
{
    return LED_Net_State.gpio_port->ODR & LED_Sys_State.gpio_pin ? 1 : 0;
}

/**
 * 写跳传 LED状态
 */
static char drv_readDataLED(void)
{
    return LED_Data_State.gpio_port->ODR & LED_Sys_State.gpio_pin ? 1 : 0;
}

/**
 * 初始化LED
 * 
 */
void drv_initLED(LED_Descriptor * pdescriptor)
{
    if(pdescriptor->ledNmb==LED_SYS)
    {
        hal_initGPIO(LED_Sys_State);
        pdescriptor->write = drv_writeSysLED;
        pdescriptor->read = drv_readSysLED;
    }else if(pdescriptor->ledNmb==LED_NET)
    {
        hal_initGPIO(LED_Net_State);
        pdescriptor->write = drv_writeNetLED;
        pdescriptor->read = drv_readNetLED;
    }else if(pdescriptor->ledNmb==LED_DATA)
    {
        hal_initGPIO(LED_Data_State);
        pdescriptor->write = drv_writeDataLED;
        pdescriptor->read = drv_readDataLED;
    }
}

/**
 * LED事件查询 1ms调用一次
 * 
 * 
 */
void drv_pollLEDEvent(LED_Descriptor * pdescriptor)
{
	++pdescriptor->tick;

    if(pdescriptor->period!=0 && pdescriptor->times)
    {
        if((pdescriptor->tick*DRV_POLL_PERIOD)%pdescriptor->period==0)
        {
            pdescriptor->write(pdescriptor->toggle);
            pdescriptor->toggle = !pdescriptor->toggle;

            if(pdescriptor->times!=0xFFFF && (pdescriptor->tick*DRV_POLL_PERIOD)==pdescriptor->period)
            {
                if(--pdescriptor->times==0)
                {
    				pdescriptor->write(1);
                }
            }

            pdescriptor->tick = 0;
        }
    }
}
