/*****************************************************************
 *  说明:设备业务逻辑及结构体定义
 * 
 *  日期:2021-8-24 13:52:29
 *****************************************************************/
#ifndef __APP_DEVICE_H
#define __APP_DEVICE_H

#include "stm32f4xx.h"
#include "global_config.h"
#include "string.h"
#include "stdlib.h"

#include "FreeRTOS.h"
#include "drv_485.h"
#include "drv_cat1.h"
#include "drv_led.h"
#include "drv_mx25.h"
#include "drv_pwm2iv.h"
#include "drv_sx1278.h"
#include "drv_ble.h"

#include "app_communication.h"

#define SWITCH_STATE(STATE) device.state = STATE

/*网关状态*/
typedef enum
{
	IDLE     	= 0, //啥也不干 

    UNCONFIG 	= 1, //开机未配置状态
    CONFIGED 	= 2, //已配置 正常运行状态 通常会处理周期性事物
	GATEIAP  	= 3, //网关IAP模式
	BTLIAP   	= 4, //bootloader IAP升级 [预留]

	RESTART     = 5,//重启设备
	RESTORE     = 6,//恢复出厂

    TIPS        = 7, //隧道灯光提示 用于指示工作状态

	TEST        = 100//测试
}DEVICE_STATE;

/*固件包状态*/
typedef struct
{
    uint8_t version;    //版本号
    
    uint32_t packsize;  //包尺寸 表示有多少个200字节

    uint16_t index;     //当前升级包片段

    uint16_t times;     //要申请多少次 该变量只针对网关

    char updatestate;   //是否处于升级模式 1 是 0 否 bootloader根据该变量判断是否需要升级
}FirmWare;

/*网关设备自身类 8字节对齐 用于存储在flash*/
typedef struct  __attribute((aligned (8)))
{
    uint64_t        macaddr;        //自身MAC地址

    DEVICE_STATE    state;          //设备自身状态 

    tLoRaSettings   loraparam;      //主LoRa空中参数

    uint8_t         channel[8];     //8通道电压

    uint32_t        systick;        //系统时间戳
    uint16_t        uploadperiod;   //设备采集数据周期 单位:秒

    Communication communication;    //跳传通讯模块

    /*IAP升级用*/
    FirmWare        fwver;          //网关自身固件信息
}Device;

void app_initDevice(Device *pdev);
DEVICE_STATE app_pollDeviceEvents(Device *pdev);

#endif
