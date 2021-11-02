/**********************************************************
 * 说明:该文件用于实现数据的集中处理的接口函数(发送/接收)
 * 
 * 内容:
 *       各个外设接收事件回调函数
 *   	 协议打包/拆包
 *********************************************************/
#ifndef __DATA_H
#define __DATA_H

#include "stdint.h"
#include "app_protocol.h"
#include "app_device.h"

//通讯指令
typedef enum
{
	//手机绑定
	DEVICE_BIND =  0x1E001000,

	//跳传
	DEVICE_JUMP = 0x1E001001,							//设备n->设备n+1 灯光控制跳传
	DEVICE_JUMP_ACK = DEVICE_JUMP | 0x00FF0000,			//设备n+1->设备n 回复

	//本地控制器
	DEVICE_CONTROL = 0x1E001002,						//本地控制器->设备 本地控制器控制灯光
	DEVICE_CONTROL_ACK = DEVICE_CONTROL | 0x00FF0000,   //设备->本地控制器 回复本地控制器

	//诱导标 [预留]

	//平台通讯
	SERVER_REQ_CFG = 0x1E032001,  						//设备->平台 请求配置
	SERVER_REQ_CFG_ACK = 0x01012001,					//平台->设备 下发配置

	SERVER_UPLOAD_STATUS = 0x1E032002,					//设备->平台 上传状态
	SERVER_UPLOAD_STATUS_ACK = 0x1E000000,				//平台->设备 回复 该指令可用于检查是否与平台建立连接

	SERVER_DOWNLOAD_CTRL  = 0x1E012003, 				//平台->设备 平台下发各个控制指令
	SERVER_DOWNLOAD_CTRL_ACK = 0x1E022003,				//设备->平台 设备回复

	SERVER_REQ_FW = 0x1E032004, 						//设备->平台 设备请求固件片段
	SERVER_REQ_FW_ACK = 0x1E012004, 					//平台->设备 平台回复固件片段
	SERVER_IAP_RET = 0x01032005 						//设备->平台 设备汇报升级结果
}CMD_TYPE;

//设备类型
typedef enum
{
	DRV_LORA_1,
	DRV_RS485_1,
	DRV_RS485_2,
	DRV_CAT1,
	DRV_BLE,
	DRV_DEBUG
}DRV_TYPE;

/*基础数据通讯接口*/
void app_sendMsg(DRV_TYPE devtype, char * buffer, uint16_t size);

void app_revLoRa1Msg(char * msg, uint16_t size);
void app_revRS4851Msg(char * msg, uint16_t size);
void app_revRS4852Msg(char * msg, uint16_t size);
void app_revCat1Msg(char * msg, uint16_t size);
void app_connectServer(void);
void app_restartCat1(void);
void app_registCat1(void);
void app_revBLEMsg(char * msg, uint16_t size);

/*协议接口函数*/
void app_sendProtocolMsg(CMD_TYPE cmd, uint64_t id, char * buffer, uint16_t size);
void app_procProtocolMsg(PROTOCOLTYPE msgtype, char * buffer, uint16_t size);

#endif
