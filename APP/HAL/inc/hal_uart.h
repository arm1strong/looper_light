#ifndef __HAL_UART_H
#define __HAL_UART_H

typedef enum
{
    UART_1,
    UART_2,
    UART_3,
	UART_4,
	UART_5,
	UART_6
}UARTn;

void hal_initUART(UARTn uart, unsigned int baudrate);
void hal_sendUARTBytes(UARTn uart, char *buf, unsigned short size);

#endif
