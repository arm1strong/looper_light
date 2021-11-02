#include "app_data.h"
#include "app_protocol.h"
#include "app_device.h"

//系统状态指示灯
extern LED_Descriptor led_sys;
//网络状态灯
extern LED_Descriptor led_net;
//通讯指示灯
extern LED_Descriptor led_data;
//RS485 1
extern RS485_Descriptor rs4851;
//RS485 2
extern RS485_Descriptor rs4852;
//0~10V
extern PWM2IV_Descriptor pwm2iv;
//LoRa 1
extern LoRa_Descriptor lora1;
//Cat1
extern Cat1_Descriptor cat1;
//FLASH 
extern FLASH_Descriptor flash;
//BLE
extern BLE_Descriptor ble;

/**************************************工具函数*****************************************/
/**
 * 大小端转换
 * val为待转换值 len为转换长度 可以为奇数
 * 返回转换后的值
 */
static uint64_t app_changeEndian(uint64_t val, char len)
{
	uint64_t ret_val = 0;

	ret_val = val;

	for(char i=0;i<len;++i)
	{
		*((char *)&ret_val+i) = *((char *)&val+len-i-1);
	}

	return ret_val;
}

/*************************************************发送数据************************************************/
void app_sendMsg(DRVTYPE devtype, char * buffer, uint16_t size)
{
	switch(devtype)
	{
		case DRV_LORA_1:
		{
			lora1.write(buffer, size);
		}	break;

		case DRV_RS485_1:
		{
			rs4851.write(buffer, size);
		}	break;

		case DRV_RS485_2:
		{
			rs4852.write(buffer, size);
		}	break;

		case DRV_CAT1:
		{
			cat1.write(buffer, size);
		}	break;

		case DRV_BLE:
		{
			ble.write(buffer, size);
		}	break;

		default : break;
	}
}

/*************************************************事件回调********************************************/
/**
 * LoRa1 接收
 */
void app_revLoRa1Msg(char * msg, uint16_t size)
{

}

/**
 * RS485 1接收
 */
void app_revRS4851Msg(char * msg, uint16_t size)
{

}

/**
 * RS485 2接收
 */
void app_revRS4852Msg(char * msg, uint16_t size)
{

}

/**
 * Cat1接收
 */
void app_revCat1Msg(char * msg, uint16_t size)
{

}

/**
 * Cat1连接事件 你可以在里边设置灯光闪烁频率 或者发送注册包
 */
void app_connectServer(void)
{

}

/**
 * Cat1重启事件 当重启Cat1或者断开连接后执行该函数 
 */
void app_restartCat1(void)
{

}

/**
 * Cat1组网事件 模组连接到基站执行该函数 你可以在里边设置灯光闪烁频率
 */
void app_registCat1(void)
{

}


/**
 * BLE接收事件
 */
void app_revBLEMsg(char * msg, uint16_t size)
{

}
