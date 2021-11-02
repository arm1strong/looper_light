#include "hal_gpio.h"

//GPIOs gpios[GPIO_NUM] = {
//                            {OUTPUT, GPIOB, GPIO_PIN_12, 1, 0, 0, 0, 0},  //SPI2 主LoRa NSS
//                            {OUTPUT, GPIOB, GPIO_PIN_1, 1, 0, 0, 0, 0},   //主LoRa RST
//                            {OUTPUT, GPIOA, GPIO_PIN_15, 1, 0, 0, 0, 0},   //SPI3 副LoRa NSS
//                            {OUTPUT, GPIOB, GPIO_PIN_6, 1, 0, 0, 0, 0},   //副LoRa RST
//                            {OUTPUT, GPIOA, GPIO_PIN_4, 1, 0, 0, 0, 0},   //SPI3 FLASH  NSS
//                            {OUTPUT, GPIOA, GPIO_PIN_12, 0, 0, 0, 0, 0},  //Cat-1 POWER_EN
//                            {OUTPUT, GPIOA, GPIO_PIN_8, 1, 0, 0, 0, 0},   //Cat-1 RST
//                            {INPUT, GPIOA, GPIO_PIN_6, 1, 0, 0, 0, 0},    //主LoRa DIO0
//                            {INPUT, GPIOB, GPIO_PIN_9, 1, 0, 0, 0, 0},    //副LoRa DIO0
//                            {INPUT, GPIOA, GPIO_PIN_1, 1, EGE_FALLING, EXTI1_IRQn, 0, 0},//GPS 1PPS
//                            {OUTPUT, GPIOB, GPIO_PIN_2, 1, 0, 0, 0, 0},   //运行指示灯
//                            {OUTPUT, GPIOC, GPIO_PIN_13, 1, 0, 0, 0, 0},  //网络状态指示灯
//							{OUTPUT, GPIOA, GPIO_PIN_2, 1, 0, 0, 0, 0},   //GPS网络使能
//							{OUTPUT, GPIOA, GPIO_PIN_11, 0, 0, 0, 0, 0},  //Cat-1唤醒引脚
//							{INPUT, GPIOC, GPIO_PIN_13, 1, 0, 0, 0, 0},  //网络状态指示灯 作为按键
//                        };
/**
 * 初始化IO
 * gpio 结构体  gpiocnt 要初始化GPIO的数量
 */
void hal_initGPIO(GPIOs gpio)
{
    /*初始化时钟*/
    if(gpio.gpio_port==GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }else if(gpio.gpio_port==GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }else if(gpio.gpio_port==GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }else if(gpio.gpio_port==GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }else if(gpio.gpio_port==GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }

    /*配置GPIO*/
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = gpio.gpio_pin;

    if(gpio.gpio_mode==INPUT)
    {
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;

    }else if(gpio.gpio_mode==OUTPUT)
    {
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;

        /*配置默认电平*/
        HAL_GPIO_WritePin(gpio.gpio_port, gpio.gpio_pin, gpio.gpio_level);  
    }else   //外部中断
    {
        GPIO_InitStruct.Mode = gpio.exti_ege;

        HAL_NVIC_SetPriority(gpio.exti_irqn, gpio.exti_pre, gpio.exti_sub);
        HAL_NVIC_EnableIRQ(gpio.exti_irqn);
    }

    HAL_GPIO_Init(gpio.gpio_port, &GPIO_InitStruct);  
}

/**
 * 设置IO电平
 * 
 */
void hal_setGPIOLevel(GPIOs gpio, uint8_t level)
{
    HAL_GPIO_WritePin(gpio.gpio_port, gpio.gpio_pin, level);  
}

/**
 * 翻转电平状态
 * 
 */
void hal_toggleGPIOLevel(GPIOs gpio)
{
    HAL_GPIO_TogglePin(gpio.gpio_port, gpio.gpio_pin);  
}

/**
 * 读取IO电平
 * 
 * 
 */
uint8_t hal_getGPIOLevel(GPIOs gpio)
{
    return HAL_GPIO_ReadPin(gpio.gpio_port, gpio.gpio_pin);
}

