#include "app_data.h"
#include "app_device.h"
#include "app_task.h"
#include "cmsis_os.h"
#include "stdlib.h"

extern Device device;

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
//Debug
extern Debug_Descriptor debug;

//任务句柄
osThreadId_t mainTaskHandle;
osThreadId_t commuTaskHandle;
osThreadId_t driverTaskHandle;

//个任务堆栈剩余空间
UBaseType_t mainTask_size;
UBaseType_t commuTask_size;
UBaseType_t driverTask_size;

//CPU占用率
uint16_t cpu_usage = 0; //小数点后两位用整数表示 如:523 表示占用率为5.23%
uint32_t idle_tick = 0; //空闲任务++

const osThreadAttr_t mainTask_attributes = {
  .name = "mainTask",
  .priority = (osPriority_t) 4,
  .stack_size = 128 * 4
};

const osThreadAttr_t commuTask_attributes = {
  .name = "commuTask",
  .priority = (osPriority_t) 3,
  .stack_size = 128 * 4
};

const osThreadAttr_t driverTask_attributes = {
  .name = "driverTask",
  .priority = (osPriority_t) 2,
  .stack_size = 200 * 4
};



/**
 * 空闲钩子函数 用于统计CPU占用率
 */
void vApplicationIdleHook( void )
{
	static uint32_t tick = 0;

	if(tick != uwTick)
	{
	    ++idle_tick;
	    tick  = uwTick;
	}

}
/**
 * 主任务
 */
void app_mainTask(void * param)
{
    for(;;)
    {
        switch(device.state)
        {
            case IDLE:
            {
	            led_sys.period = 1000;

                mainTask_size = osThreadGetStackSpace(mainTaskHandle );
                commuTask_size = osThreadGetStackSpace(commuTaskHandle );
                driverTask_size = osThreadGetStackSpace(driverTaskHandle );
            }   break;

            case UNCONFIG:
            {
                static uint32_t unconfig_tick = 0; 
                static uint16_t rand_tick = 0;
                static uint16_t rand_period = 7;

                led_sys.period = 100;    

                if((++unconfig_tick*APP_POLL_PERIOD)%1000==0) //1S获取一次配置
                {
                    app_sendProtocolMsg(SERVER_REQ_CFG, device.macaddr, 0, 0);                  
                }

                if(++rand_tick%rand_period==0)
                {
                    rand_tick = 0;

                    srand(osKernelGetTickCount()+device.maccrc);

                    rand_period = rand()%100; //0~10S 单位100ms 随机广播

                    if(rand_period==0)  //避免除0异常
                    {
                        rand_period = 6;
                    }

                    app_sendProtocolMsg(DEVICE_BIND, 0, 0, 0); 
                }

                for(int i=0;i<8;++i)
                {
                	pwm2iv.write(i, (i+3)*10);
                }
            }   break;

            case CONFIGED:
            {
                static uint32_t configed_tick = 0; 
                
	            led_sys.period = 1000;    

                if((++configed_tick*APP_POLL_PERIOD)%device.uploadperiod==0) //周期上报状态
                {

                }
            }   break;

            case DEVIAP:
            {
                static uint32_t deviap_tick = 0; 

	            led_sys.period = 500;    

                if((++deviap_tick*APP_POLL_PERIOD)%1000==0) //升级 1s请求一次
                {
                    if(++device.iap_timeout*APP_POLL_PERIOD > 300000) //如果升级超过5min则认为升级超时
                    {
                        device.iap_timeout = 0;

                        SWITCH_STATE(CONFIGED);
                        app_saveSysConfig(&device);
                    }else 
                    {
                        app_sendProtocolMsg(SERVER_REQ_FW, device.macaddr, 0, 0);
                    }
                }

            }   break;

            case BTLIAP: //BOOT升级[预留]
            {

            }   break;

            case RESTART://复位
            {
                HAL_NVIC_SystemReset();
            }   break;

            case RESTORE://恢复出厂
            {
                device.macaddr = 0;
                app_saveSysConfig(&device);
                NVIC_SystemReset();	
            }   break;

            case TIPS: //用于现场指示 闪烁3次
            {
                static uint32_t tips_tick = 0; 
                static uint8_t light[2] = {0, 50};
                static uint8_t index = 0;

                if((++tips_tick*APP_POLL_PERIOD)%800==0)
                {
                    for(char i=0;i<8;++i)
                    {
                        pwm2iv.write(i, light[index%2]);
                    }

                    if(++index>=6) //闪烁完成还原之前状态
                    {
                        index = 0;

                        for(char i=0;i<8;++i)
                        {
                            pwm2iv.write(i, device.communication.msg[i]);
                        }

                        SWITCH_STATE(device.old_state);                        
                    }
                }
            }   break;

            case TEST: //可作为测试或厂测模式
            {
                static uint16_t period = 100;

                static uint16_t test_tick = 0; 

                if(++test_tick==period)
                {
                    app_sendProtocolMsg(DEVICE_BIND, 0, 0, 0);
                }else if(++test_tick==period)
                {
                    app_sendProtocolMsg(DEVICE_JUMP, 1, 0, 0);
                    
                    period += 100;
                }else if(++test_tick==period)
                {
                    app_sendProtocolMsg(DEVICE_JUMP_ACK, 1, 0, 0);
                    period += 100;
                }else if(++test_tick==period)
                {
                    app_sendProtocolMsg(DEVICE_CONTROL_ACK, 1, 0, 0);

                    period += 100;
                }else if(++test_tick==period)
                {
                    app_sendProtocolMsg(SERVER_REQ_CFG, 0, 0, 0);

                    period += 100;
                }else if(++test_tick==period)
                {
                    app_sendProtocolMsg(SERVER_REQ_FW, 0, 0, 0);

                    period += 100;
                }else if(++test_tick==period)
                {
                    app_sendProtocolMsg(SERVER_DOWNLOAD_CTRL_ACK, 0, 0, 0);

                    period += 100;
                }else if(++test_tick==period)
                {
					//版本相同 不升级
					device.fwver.iap_ret[0] = IAP_ERR_TIMEOUT;
					device.fwver.iap_ret[1] = VERSION;

    				app_sendProtocolMsg(SERVER_IAP_RET, 0, device.fwver.iap_ret, sizeof(device.fwver.iap_ret));  //回复平台表示已配置完成

                    period += 100;
                }else 
                {

                }
                
            }   break;

            default: //下发非法状态默认切换至运行态
            {
                SWITCH_STATE(CONFIGED);
            }  break;
        }

        osDelay(APP_POLL_PERIOD); //100ms执行一次     
    }
}

/**
 * 跳传通讯任务
 */
void app_communicationTask(void *argument)
{
    for(;;)
    {
        app_pollCommunicationEvents(&device.communication); 
        osDelay(COMMU_POLL_PERIOD);  //10ms执行一次     
    }
}


/**
 * 驱动任务
 */
void app_driverTask(void *argument)
{
    for(;;)
    {
        drv_pollSCat1Events(&cat1);
        drv_pollLoRaEvents(&lora1);
        drv_pollRS485Events(&rs4851);
        drv_pollRS485Events(&rs4852);
        drv_pollBLEEvents(&ble);
        drv_pollLEDEvent(&led_sys);
        drv_pollLEDEvent(&led_net);
        drv_pollLEDEvent(&led_data);
        hal_feedWdg();

        osDelay(DRV_POLL_PERIOD);  //10ms执行一次       
    }
    
}

/**
 * 创建所有任务
 */
void app_createAllTask(void)
{
    mainTaskHandle = osThreadNew(app_mainTask, NULL, &mainTask_attributes);             //优先级别高 轮询周期100ms
    commuTaskHandle = osThreadNew(app_communicationTask, NULL, &commuTask_attributes);  //优先级别中 轮询周期10ms
    driverTaskHandle = osThreadNew(app_driverTask, NULL, &driverTask_attributes);       //优先级别低 轮询周期10ms
}
