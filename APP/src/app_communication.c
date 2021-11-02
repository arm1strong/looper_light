#include "app_communication.h"
#include "app_data.h"
#include "global_config.h"

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

/**
 * 跳传事件轮询
 * 
 */
COMMU_STATE app_pollCommunicationEvents(Communication * pcommu)
{
	COMMU_STATE state;

	switch(pcommu->state)
	{
		case COMMU_IDEL:		//空闲态
		{
			/*啥也不干等数据*/
		}	break;

		case COMMU_ALIGNMENT:	//接收对齐态 假设当前接收sn为2 则在该状态等到max_broadcast_times再给下一级控制器跳传
		{
			if(++pcommu->tick >= pcommu->max_broadcast_period/DRV_POLL_PERIOD)
			{
				pcommu->tick = 0;

				if(++pcommu->sn>=pcommu->max_broadcast_times) //对齐时间到
				{
					if(pcommu->self_id<pcommu->start_id) //不应用 跳传
					{
						pcommu->state = COMMU_BROADCAST;
						pcommu->id_index = 1;
					}else if(pcommu->self_id>=pcommu->start_id && pcommu->self_id<pcommu->end_id) //应用 跳传
					{						
						for(char i=0;i<8;++i)
						{
							if(pcommu->msg[i]<=100) //大于100 通道不应用
							{
								pwm2iv.write(i, pcommu->msg[i]);
							}
						}

						pcommu->state = COMMU_BROADCAST;
						pcommu->id_index = 1;
					}else if(pcommu->self_id==pcommu->end_id) //应用不跳传
					{
						for(char i=0;i<8;++i)
						{
							if(pcommu->msg[i]<=100) //大于100 通道不应用
							{
								pwm2iv.write(i, pcommu->msg[i]);
							}
						}

						pcommu->state = COMMU_IDEL;
						pcommu->sn = 0;
						pcommu->tick = 0;
						pcommu->id_index = 1;
					}else //不应用 不跳传
					{
						pcommu->state = COMMU_IDEL;
						pcommu->sn = 0;
						pcommu->tick = 0;
						pcommu->id_index = 1;
					}

					pcommu->sn = 0;
				}
			}				
		}	break;

		case COMMU_BROADCAST:	//广播跳传态
		{
			if(++pcommu->tick >= pcommu->max_broadcast_period/DRV_POLL_PERIOD)
			{
				++pcommu->tick = 0;

				if(++pcommu->sn>=pcommu->max_broadcast_times) //对单个控制器广播最大次数无返回 自动切换到下一个控制器对其进行广播
				{
					pcommu->sn = 0;
					if(++pcommu->id_index>=pcommu->max_broadcast_dev_cnts) //发给自己所有下游控制器均未收到回复 则自动切换IDLE[超过容灾最大限度 无力回天]
					{
						pcommu->state = COMMU_IDEL;
						pcommu->sn = 0;
						pcommu->tick = 0;
						pcommu->id_index = 1;
					}
				}else 
				{
					/*广播跳传包*/
				}
			}
		}	break;

		default : state = -1; break; //不存在的状态
	}

	return state;
}
