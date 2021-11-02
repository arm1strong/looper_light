#ifndef __485_H
#define __485_H
#include "hal_gpio.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h"

typedef enum
{
	RS485_1,
	RS485_2
}RS485n;

typedef struct
{
	RS485n rs485Nmb;  	//485编号
	uint32_t baudrate;	//波特率

	bool r_flag;		//接收标志 true为有数据接收 此时可读取r_buffer
	char r_tail;		//接收数据索引 该成员可作为接收数据的长度来使用
	char * t_buffer; //发送数据缓冲区
	char * r_buffer;    //接收缓冲区

	void (*write)(char * bytes, uint16_t len);   //发送函数
	void (*read)(char * bytes, uint16_t len);//接收函数
}RS485_Descriptor;

void drv_initRS485(RS485_Descriptor * pdescriptor);
void drv_pollRS485Events(RS485_Descriptor * pdescriptor);

#endif
