#include "drv_485.h"
#include "hal_uart.h"
#include "global_config.h"

GPIOs RS485_1_EN = {OUTPUT, GPIOC, GPIO_PIN_12, 0, 0, 0, 0, 0};
GPIOs RS485_2_EN = {OUTPUT, GPIOD, GPIO_PIN_10, 0, 0, 0, 0, 0};

/**
 * 发送RS485 1 数据
 */
static void drv_sendRS485_1_Data(char * bytes, uint16_t len)
{
	hal_setGPIOLevel(RS485_1_EN, 1);
	hal_sendUARTBytes(UART_4, bytes, len);
	hal_setGPIOLevel(RS485_1_EN, 0);
}

/**
 * 发送RS485 2 数据
 */
static void drv_sendRS485_2_Data(char * bytes, uint16_t len)
{
	hal_setGPIOLevel(RS485_2_EN, 1);
	hal_sendUARTBytes(UART_3, bytes, len);
	hal_setGPIOLevel(RS485_2_EN, 0);
}

/**
 * 初始化485
 */
void drv_initRS485(RS485_Descriptor * pdescriptor)
{
	if(pdescriptor->rs485Nmb==RS485_1)
	{
		hal_initGPIO(RS485_1_EN);
		hal_initUART(UART_4, pdescriptor->baudrate);
		pdescriptor->write = drv_sendRS485_1_Data;
		pdescriptor->t_buffer = (char *)pvPortMalloc(RS485_1_MAX_SENDSIZE);
		pdescriptor->r_buffer = (char *)pvPortMalloc(RS485_1_MAX_REVSIZE);
	}else if(pdescriptor->rs485Nmb==RS485_2)
	{
		hal_initGPIO(RS485_2_EN);
		hal_initUART(UART_3, pdescriptor->baudrate);
		pdescriptor->write = drv_sendRS485_2_Data;
		pdescriptor->t_buffer = (char *)pvPortMalloc(RS485_2_MAX_SENDSIZE);
		pdescriptor->r_buffer = (char *)pvPortMalloc(RS485_2_MAX_REVSIZE);
	}else
	{

	}
}

/**
 * 轮询485是否接收到数据
 */
void drv_pollRS485Events(RS485_Descriptor * pdescriptor)
{
	if(pdescriptor->r_flag==true)
	{
		pdescriptor->read(pdescriptor->r_buffer, pdescriptor->r_tail);
		
		memset(pdescriptor->r_buffer, 0, RS485_1_MAX_REVSIZE);
		pdescriptor->r_flag = false; 
		pdescriptor->r_tail = 0;
	}
}
