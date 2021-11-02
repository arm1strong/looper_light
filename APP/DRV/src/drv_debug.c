#include "drv_debug.h"
#include "hal_uart.h"
#include "global_config.h"

/**
 * 发送debug 数据
 */
static void drv_sendDebugData(char * bytes, uint16_t len)
{
	hal_sendUARTBytes(UART_1, bytes, len);
}

/**
 * 初始化Debug串口
 */
void drv_initDebug(Debug_Descriptor * pdescriptor)
{
	hal_initUART(UART_1, pdescriptor->baudrate);
	pdescriptor->write = drv_sendDebugData;

	pdescriptor->t_buffer = (char *)pvPortMalloc(DEBUG_MAX_SENDSIZE);
	pdescriptor->r_buffer = (char *)pvPortMalloc(DEBUG_MAX_REVSIZE);
}

/**
 * 轮询Debug串口是否接收到数据
 */
void drv_pollDebugEvents(Debug_Descriptor * pdescriptor)
{
	if(pdescriptor->r_flag==true)
	{
		pdescriptor->read(pdescriptor->r_buffer, pdescriptor->r_tail);
		
		memset(pdescriptor->r_buffer, 0, BLE_MAX_REVSIZE);
		pdescriptor->r_flag = false; 
		pdescriptor->r_tail = 0;
	}
}
