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
#include "drv_debug.h"
#include "hal_flash.h"
#include "hal_wdg.h"

#include "app_communication.h"

#define SWITCH_STATE(STATE) device.old_state =  device.state; \
                            device.state = STATE

/*设备状态*/
typedef enum
{
	IDLE     	= 0, //啥也不干 

    UNCONFIG 	= 1, //开机未配置状态
    CONFIGED 	= 2, //已配置 正常运行状态 通常会处理周期性事物
	DEVIAP  	= 3, //设备IAP模式
	BTLIAP   	= 4, //bootloader IAP升级 [预留]

	RESTART     = 5, //重启设备
	RESTORE     = 6, //恢复出厂

    TIPS        = 7, //隧道灯光闪烁提示

	TEST        = 8  //测试
}DEVICE_STATE;

/*设备角色*/
typedef enum  
{
    ROLE_RADAR, //雷达型 雷达型可以接收 道钉或诱导标的数据
    ROLE_BASIC  //基本型
}ROLE;

/*升级结果*/
typedef enum
{
	IAP_SUCCESS = 0,    //升级成功

	IAP_ERR_VER,		//升级错误 版本相同
	IAP_ERR_OVERFLOW,	//升级错误 多接收固件包 不会出现此场景
	IAP_ERR_CRC,		//升级错误 CRC校验出错
	IAP_ERR_FALSH,		//升级错误 FLASH读写错误
	IAP_ERR_TIMEOUT,	//升级错误 升级超时
}IAP_RET;

/*固件包状态*/
typedef struct
{
    uint8_t version;    //版本号
    
    uint32_t packsize;  //包尺寸 表示有多少个200字节

    uint16_t index;     //当前升级包片段

    uint16_t times;     //要申请多少次 该变量只针对网关

    char updatestate;   //是否处于升级模式 1 是 0 否 bootloader根据该变量判断是否需要升级

    char iap_ret[2];    //升级结果 iap_ret[0] 升级结果 @IAP_RET  iap_ret[1] 当前版本号
}FirmWare;

/*设备自身类 4字节对齐 用于存储在flash*/
typedef struct  __attribute((aligned (8)))
{
    /*IAP升级用*/
    FirmWare        fwver;          //网关自身固件信息
    
    uint64_t        macaddr;        //自身MAC地址
    uint16_t        maccrc;         //MAC地址的CRC 用于防碰撞的随机延时

    DEVICE_STATE    state;          //设备自身状态 
    DEVICE_STATE    old_state;      //上次运行状态

    tLoRaSettings   loraparam;      //主LoRa空中参数

    uint32_t        systick;        //系统时间戳
    uint16_t        uploadperiod;   //设备采集数据周期 单位:秒

    Communication 	communication;    //跳传通讯模块

    uint16_t        rev_timeout;    //服务器接收超时 单位S 如果服务器很长时间没有回复 则重启网络重连
    uint16_t        iap_timeout;    //在线升级超时 天线接触不良 SIM卡欠费 或者网络状况不佳 都会导致升级时间延长 目前配置 升级大约需要2min

    ROLE            role;           //设备角色
}Device;

void app_initDevice(Device *pdev);
DEVICE_STATE app_pollDeviceEvents(Device *pdev);

void app_readSysConfig(Device * dev);
char app_saveSysConfig(Device * dev);

#endif
