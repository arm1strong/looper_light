/******************************************************************
 * 文件：SX1278.c
 * 功能：定义SPI功能函数
 * 日期：2018-03-17
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 声明：移植自Semtech
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "stm32f4xx_hal.h"
#include "hal_spi.h"
#include "drv_sx1278.h"
#include "hal_gpio.h"
#include "sx1276.h"
#include "sx1276-LoRa.h"
#include "sx1276-FskMisc.h"
#include "stdbool.h"
#include "stdint.h"
#include "radio.h"
#include "global_config.h"

GPIOs LoRa_1_NSS = {OUTPUT, GPIOB, GPIO_PIN_9, 1, 0, 0, 0, 0};
GPIOs LoRa_1_DIO0 = {INPUT, GPIOA, GPIO_PIN_11, 1, 0, 0, 0, 0};
GPIOs LoRa_1_RST = {OUTPUT, GPIOA, GPIO_PIN_12, 1, 0, 0, 0, 0};
GPIOs LoRa_1_PWR = {OUTPUT, GPIOE, GPIO_PIN_0, 1, 0, 0, 0, 0};  //高电平供电

extern SX1278Class * sx1278;

/**
 * 切换到接收模式 当发送完成 调用该函数切换至接收
 * 
 */
static void drv_switchReceiveMode(void)
{
	SX1276LoRaSetRFState(RFLR_STATE_RX_INIT);
}

/**
 * 切换到发送模式
 *
 */
static void drv_switchTransmitMode(void)
{
	SX1276LoRaSetRFState(RFLR_STATE_TX_INIT);
}

/**
 * 发送SX1278数据
 */
static void drv_sendLoRaData(char * buffer, uint16_t size)
{
	SX1276LoRaSetTxPacket(buffer, size);
}

/**
 * LoRa 复位
 */
static void  drv_setLoRaResetLevel(LoRan lora, char level)
{
	if(lora==LoRa_1)
	{
		hal_setGPIOLevel(LoRa_1_RST, level);
	}else  
	{

	}
}

/**
 * LoRa 获取DIO0状态
 */
static char drv_getDIO0Level(LoRan lora)
{
	char level;

	if(lora==LoRa_1)
	{
		level = hal_getGPIOLevel(LoRa_1_DIO0);
	}else  
	{

	}
}

/**
 * 设置NSS
 */
static void  drv_setLoRaNSSLevel(LoRan lora, char level)
{
	if(lora==LoRa_1)
	{
		hal_setGPIOLevel(LoRa_1_NSS, level);
	}else  
	{

	}
}


/**
 * 初始化SX1278
 * 
 * 
 */
void drv_initSX1278(LoRa_Descriptor * pdescriptor)
{
	/*初始化相关IO*/
	if(pdescriptor->loraNmb==LoRa_1)
	{
		hal_initGPIO(LoRa_1_NSS);
		hal_initGPIO(LoRa_1_RST);
		hal_initGPIO(LoRa_1_DIO0);
		hal_initGPIO(LoRa_1_PWR);

		pdescriptor->write = drv_sendLoRaData;

		hal_initSPI(LoRa_1, POLARITY_LOW, PHASE_1EGE);

		pdescriptor->t_buffer = (char *)pvPortMalloc(LORA_1_MAX_SENDSIZE);
		pdescriptor->r_buffer = (char *)pvPortMalloc(LORA_1_MAX_REVSIZE);
	}else 
	{

	}

	/*初始化SX1278*/
	pdescriptor->loraparam.spi_nmb = pdescriptor->loraNmb;
	pdescriptor->loraparam.writeSPIData = hal_sendSPIBytes;
	pdescriptor->loraparam.readSPIData = hal_readSPIBytes;
	pdescriptor->loraparam.setReset = drv_setLoRaResetLevel;
	pdescriptor->loraparam.readDIO0 = drv_getDIO0Level;
	pdescriptor->loraparam.setSPINSS = drv_setLoRaNSSLevel;

	sx1278 = &pdescriptor->loraparam;

	HAL_Delay(1000);
	SX1276Init();
	drv_switchReceiveMode();
//	SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );
}

/**
 * 驱动事件轮询处理函数
 */
uint32_t drv_pollLoRaEvents(LoRa_Descriptor * pdescriptor)
{
	uint32_t ret;
	uint16_t size = 0;

	sx1278 = &pdescriptor->loraparam; //切换为自身LoRa

	ret = SX1276LoRaProcess();

	switch (ret)
	{
		case RF_RX_DONE:
		{
			SX1276GetRxPacket(pdescriptor->r_buffer, &pdescriptor->r_tail);

			pdescriptor->read(pdescriptor->r_buffer, pdescriptor->r_tail);
		}                       break;

		case RF_TX_DONE:
		{
			drv_switchReceiveMode();
		}                       break;

		default:                break;
	}

	return ret;
}
