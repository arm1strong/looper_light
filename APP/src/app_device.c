#include "app_device.h"

Device device;

void app_revLoRa1Msg(char * msg, uint16_t size);
void app_revRS4851Msg(char * msg, uint16_t size);
void app_revRS4852Msg(char * msg, uint16_t size);
void app_revCat1Msg(char * msg, uint16_t size);
void app_connectServer(void);
void app_restartCat1(void);
void app_registCat1(void);
void app_revBLEMsg(char * msg, uint16_t size);

//驱动初始化列表
//系统状态指示灯
LED_Descriptor led_sys =
{
	.ledNmb = LED_SYS,
	.period = 0, //0表示不闪烁
	.times = 0xFFFF,
};

//网络状态灯
LED_Descriptor led_net = 
{
	.ledNmb = LED_NET,
	.period = 0, //0表示不闪烁
	.times = 0xFFFF,
};

LED_Descriptor led_data =
{
	.ledNmb = LED_DATA,
	.period = 0, //0表示不闪烁
	.times = 0xFFFF,
};
//RS485 1
RS485_Descriptor rs4851 = 
{
	.rs485Nmb = RS485_1,
	.baudrate = 9600,
	.r_tail   = 0,
	.r_flag   = 0,
	.r_buffer = (char *)pvPortMalloc(RS485_1_MAX_REVSIZE),
	.read = app_revRS4851Msg,  //接收回调函数
};

//RS485 2
RS485_Descriptor rs4852 = 
{
	.rs485Nmb = RS485_2,
	.baudrate = 9600,
	.r_tail   = 0,
	.r_flag   = 0,
	.r_buffer = (char *)pvPortMalloc(RS485_2_MAX_REVSIZE),
	.read = app_revRS4852Msg,
};

//0~10V
PWM2IV_Descriptor pwm2iv = 
{
	.ch = {0},
};

//LoRa 1
LoRa_Descriptor lora1 = 
{
	.loraNmb = LORA_1,
	.read = app_revLoRa1Msg,
	.r_buffer = (char *)pvPortMalloc(LORA_1_MAX_REVSIZE),
	.loraparam = 
	{
		.SX1276LR = 0,
		.RFLRState = RFLR_STATE_IDLE,
		.RxPacketSize = 0,
		.RxGain = 1,
		.RxTimeoutTimer = 0,
		.TxPacketSize = 0,
		.LoRaOn = false,
		.LoRaOnState = false,
		.opModePrev = RFLR_OPMODE_STANDBY,
		.antennaSwitchTxOnPrev = true,
		// Default settings
		.LoRaSettings =
		{
			434000000,        // RFFrequency
			20,               // Power
			9,                // SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
							  // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
			7,                // SpreadingFactor [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
			1,                // ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
			true,             // CrcOn [0: OFF, 1: ON]
			false,            // ImplicitHeaderOn [0: OFF, 1: ON]
			0,                // RxSingleOn [0: Continuous, 1 Single]
			0,                // FreqHopOn [0: OFF, 1: ON]
			4,                // HopPeriod Hops every frequency hopping period symbols
			3000,             // TxPacketTimeout
			3000,             // RxPacketTimeout
			128,              // PayloadLength (used for implicit header mode)
		}	
	},
};

Cat1_Descriptor cat1 = 
{
	.nwkstate = POWERSWITCH,
	.r_tail   = 0,
	.r_flag   = 0,
	.r_buffer = (char *)pvPortMalloc(CAT1_MAX_REVSIZE),
	.read = app_revCat1Msg;
	.connectServerEvent = app_connectServer;
	.restart = app_restartCat1;
	.regist = app_registCat1;
};


//FLASH 
FLASH_Descriptor flash;

//BLE
BLE_Descriptor ble = {
	.baudrate = 115200,
	.r_tail   = 0,
	.r_flag   = 0,
	.r_buffer = (char *)pvPortMalloc(BLE_MAX_REVSIZE),
	.read = app_revCat1Msg,  //接收回调函数
};
/**
 * 读取系统配置参数
 */
void app_readSysConfig(Device * dev)
{
	hal_readFlash(CFG_PART, (char *)dev, sizeof(Device));
}

/**
 * 存储系统配置参数
 * 返回 FLASH_SUCCESS / FLASH_FAIL
 */
char app_saveSysConfig(Device * dev)
{
	char ret = FLASH_FAIL;

	if(hal_eraseFlash(2, 2)==0xFFFFFFFF) //擦除32KB空间
	{
		ret = hal_programFlash(CFG_PART, (char *)dev, sizeof(Device));
	}

	return ret;
}



/**
 * 设备初始化  初始化各外设/驱动/设备运行参数
 *  
 */
void app_initDevice(Device *pdev)
{
	uint64_t macaddr = (uint64_t)HAL_GetUIDw1()<<32 | HAL_GetUIDw0();  //获取自身ID

    pdev->systick = 0;   //系统时间戳

    pdev->fwver.version = VERSION;

	app_readSysConfig(pdev);

	if(pdev->macaddr==macaddr) //之前存储过
	{
		sx1278_m.LoRaSettings = pdev->loraparam;
	}else //被恢复出厂或第一次运行
	{
		memset(pdev->channel, 0, sizeof(pdev->channel));

		pdev->uploadperiod = 600; //默认10min上报一次设备状态

	    pdev->loraparam = sx1278_m.LoRaSettings;

	    pdev->fwver.index = 0;
	    pdev->fwver.packsize = 0;
	    pdev->fwver.times = 0;

		app_saveSysConfig(pdev);
	}

	/*初始化驱动*/
	drv_initLED(&led_sys);
	drv_initLED(&led_net);
	drv_initLED(&led_data);
	drv_initFlash(&flash);
	drv_initRS485(&rs4851);
	drv_initRS485(&rs4852);
	drv_initPWM2IV(&pwm2iv);
	drv_initCat1(&cat1);
	drv_initSX1278(&lora1);
	drv_initBLE(&ble);
	hal_initWdg(4095);
}

/**
 * 设备事件轮询 
 * 事件内容为设备各个状态的处理 如:网络 时序指令等
 */
DEVICE_STATE app_pollDeviceEvents(Device *pdev)
{
	static uint32_t tick = 0;






	++tick;

	hal_feedWdg(); //喂狗

	return pdev->state;
}

/**
 * 驱动事件轮询 
 */
DEVICE_STATE app_pollDriverEvents(Device *pdev)
{
	static uint32_t tick = 0;


	++tick;

	hal_feedWdg(); //喂狗

	return pdev->state;
}