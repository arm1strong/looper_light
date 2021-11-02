#include "stm32f4xx_hal.h"
#include "hal_uart.h"
#include "stdio.h"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

/**
 *  初始化串口 8-N-1
 * 
 */
void hal_initUART(UARTn uart, unsigned int baudrate)
{
    if(uart==UART_1) //Debug
    {     
		huart1.Instance = USART1;
		huart1.Init.BaudRate = baudrate;
		huart1.Init.WordLength = UART_WORDLENGTH_8B;
		huart1.Init.StopBits = UART_STOPBITS_1;
		huart1.Init.Parity = UART_PARITY_NONE;
		huart1.Init.Mode = UART_MODE_TX_RX;
		huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart1.Init.OverSampling = UART_OVERSAMPLING_16;
		if (HAL_UART_Init(&huart1) != HAL_OK)
		{
			Error_Handler();
		}

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); //使能接收中断
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); //使能空闲中断
    }else if(uart==UART_2)  //Cat1
    {
		huart2.Instance = USART2;
		huart2.Init.BaudRate = baudrate;
		huart2.Init.WordLength = UART_WORDLENGTH_8B;
		huart2.Init.StopBits = UART_STOPBITS_1;
		huart2.Init.Parity = UART_PARITY_NONE;
		huart2.Init.Mode = UART_MODE_TX_RX;
		huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart2.Init.OverSampling = UART_OVERSAMPLING_16;
		if (HAL_UART_Init(&huart2) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); //使能接收中断
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); //使能空闲中断
    }else if(uart==UART_3) //RS485 1
    {
		huart3.Instance = USART3;
		huart3.Init.BaudRate = baudrate;
		huart3.Init.WordLength = UART_WORDLENGTH_8B;
		huart3.Init.StopBits = UART_STOPBITS_1;
		huart3.Init.Parity = UART_PARITY_NONE;
		huart3.Init.Mode = UART_MODE_TX_RX;
		huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart3.Init.OverSampling = UART_OVERSAMPLING_16;
		if (HAL_UART_Init(&huart3) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE); //使能接收中断
		__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE); //使能空闲中断
    }else if(uart==UART_4) //RS485 2
    {
		huart4.Instance = UART4;
		huart4.Init.BaudRate = baudrate;
		huart4.Init.WordLength = UART_WORDLENGTH_8B;
		huart4.Init.StopBits = UART_STOPBITS_1;
		huart4.Init.Parity = UART_PARITY_NONE;
		huart4.Init.Mode = UART_MODE_TX_RX;
		huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart4.Init.OverSampling = UART_OVERSAMPLING_16;
		if (HAL_UART_Init(&huart4) != HAL_OK)
		{
			Error_Handler();
		}

        __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE); //使能接收中断
        __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE); //使能空闲中断
    }else if(uart==UART_5)	
    {

    }else if(uart==UART_6) //BLE
    {
		huart6.Instance = USART6;
		huart6.Init.BaudRate = baudrate;
		huart6.Init.WordLength = UART_WORDLENGTH_8B;
		huart6.Init.StopBits = UART_STOPBITS_1;
		huart6.Init.Parity = UART_PARITY_NONE;
		huart6.Init.Mode = UART_MODE_TX_RX;
		huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart6.Init.OverSampling = UART_OVERSAMPLING_16;
		if (HAL_UART_Init(&huart6) != HAL_OK)
		{
			Error_Handler();
		}

        __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE); //使能接收中断
        __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE); //使能空闲中断
    }else
    {

    }
}

/**
 *  发送串口数据 
 * 
 */
void hal_sendUARTBytes(UARTn uart, char *buf, unsigned short size)
{
    if(uart==UART_1)
    {     
        HAL_UART_Transmit(&huart1, buf, size, 10000);
    }else if(uart==UART_2)
    {
        HAL_UART_Transmit(&huart2, buf, size, 10000);
    }else if(uart==UART_3)
    {
        HAL_UART_Transmit(&huart3, buf, size, 10000);
    }else if(uart==UART_4)
    {
    	HAL_UART_Transmit(&huart4, buf, size, 10000);
    }else if(uart==UART_5)
    {
    	HAL_UART_Transmit(&huart5, buf, size, 10000);
    }else if(uart==UART_6)
    {
    	HAL_UART_Transmit(&huart6, buf, size, 10000);
    }else 
    {

    }
    
}

#if 0
int _write(int file , char *ptr,int len)
{
    int i = 0;
    for(i = 0;i<len;i++)
        ITM_SendChar((*ptr++));
    return len;
}
#endif

#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

PUTCHAR_PROTOTYPE
{

//  HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
#endif
