#include "drv_ble.h"
#include "hal_uart.h"
#include "global_config.h"

/**
 * 发送BLE 数据
 */
static void drv_sendBLEData(char * bytes, uint16_t len)
{
	hal_sendUARTBytes(UART_6, bytes, len);
}

/**
 * 初始化BLE串口
 */
void drv_initBLE(BLE_Descriptor * pdescriptor)
{
	hal_initUART(UART_6, pdescriptor->baudrate);
	pdescriptor->write = drv_sendBLEData;

	pdescriptor->t_buffer = (char *)pvPortMalloc(BLE_MAX_SENDSIZE);
	pdescriptor->r_buffer = (char *)pvPortMalloc(BLE_MAX_REVSIZE);
}

/**
 * 轮询BLE是否接收到数据
 */
void drv_pollBLEEvents(BLE_Descriptor * pdescriptor)
{
	if(pdescriptor->r_flag==true)
	{
		pdescriptor->read(pdescriptor->r_buffer, pdescriptor->r_tail);
		
		memset(pdescriptor->r_buffer, 0, BLE_MAX_REVSIZE);
		pdescriptor->r_flag = false; 
		pdescriptor->r_tail = 0;
	}
}
