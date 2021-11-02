/**********************************************************
 * 说明:该文件用于实现数据的集中处理的接口函数(发送/接收)
 * 
 * 
 * 
 * 
 *********************************************************/
#ifndef __DATA_H
#define __DATA_H

#include "stdint.h"
#include "app_protocol.h"
#include "app_device.h"

typedef enum
{
	DRV_LORA_1,
	DRV_RS485_1,
	DRV_RS485_2,
	DRV_CAT1,
	DRV_BLE
}DRVTYPE;

void app_sendMsg(DRVTYPE devtype, char * buffer, uint16_t size);

void app_revLoRa1Msg(char * msg, uint16_t size);
void app_revRS4851Msg(char * msg, uint16_t size);
void app_revRS4852Msg(char * msg, uint16_t size);
void app_revCat1Msg(char * msg, uint16_t size);
void app_connectServer(void);
void app_restartCat1(void);
void app_registCat1(void);
void app_revBLEMsg(char * msg, uint16_t size);

#endif
