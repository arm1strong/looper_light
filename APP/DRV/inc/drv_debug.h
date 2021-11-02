#ifndef __DEBUG_H
#define __DEBUG_H
#include "hal_gpio.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h"

typedef struct
{
	uint32_t baudrate;	//波特率

	bool r_flag;		//接收标志 true为有数据接收 此时可读取r_buffer
	char r_tail;		//接收数据索引 该成员可作为接收数据的长度来使用
    char * t_buffer; 	//发送数据缓冲区
	char * r_buffer;    //接收缓冲区

	void (*write)(char * bytes, uint16_t len);//发送函数
	void (*read)(char * bytes, uint16_t len);//接收函数
}Debug_Descriptor;

void drv_initDebug(Debug_Descriptor * pdescriptor);
void drv_pollDebugEvents(Debug_Descriptor * pdescriptor);

#endif
