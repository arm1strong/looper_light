#include "app_data.h"
#include "app_protocol.h"
#include "app_device.h"

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
void app_sendMsg(DRV_TYPE devtype, char * buffer, uint16_t size)
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
			if(cat1.nwkstate!=COMPLETED)
			{
				return ;
			}

			cat1.write(buffer, size);
		}	break;

		case DRV_BLE:
		{
			ble.write(buffer, size);
		}	break;

		case DRV_DEBUG:
		{
			debug.write(buffer, size);
		}	break;

		default : break;
	}
}

/*************************************************接收事件回调********************************************/
/**
 * debug 接收
 */
void app_revDebugMsg(char * msg, uint16_t size)
{

}

/**
 * LoRa1 接收
 */
void app_revLoRa1Msg(char * msg, uint16_t size)
{
	led_data.times = 6;
	led_data.period = 100;

	app_procProtocolMsg(M2M, msg, size);
}

/**
 * RS485 1接收 [目前暂未使用]
 */
void app_revRS4851Msg(char * msg, uint16_t size)
{
	led_data.times = 6;
	led_data.period = 100;
}

/**
 * RS485 2接收 [目前暂未使用]
 */
void app_revRS4852Msg(char * msg, uint16_t size)
{
	led_data.times = 6;
	led_data.period = 100;
}

/**
 * Cat1接收
 */
void app_revCat1Msg(char * msg, uint16_t size)
{
	app_procProtocolMsg(M2S, msg, size);
}

/**
 * Cat1连接事件 你可以在里边设置灯光闪烁频率 或者发送注册包
 */
void app_connectServer(void)
{
	led_net.period = 1000;
	app_sendProtocolMsg(SERVER_UPLOAD_STATUS, device.macaddr, 0, 0); //连接后上报一次状态
}

/**
 * Cat1重启事件 当重启Cat1或者断开连接后执行该函数 
 */
void app_restartCat1(void)
{
	led_net.period = 100;
}

/**
 * Cat1组网事件 模组连接到基站执行该函数 你可以在里边设置灯光闪烁频率
 */
void app_registCat1(void)
{
	led_net.period = 500;
}


/**
 * BLE接收事件 诱导标预留
 */
void app_revBLEMsg(char * msg, uint16_t size)
{

}

/**
 * 功能: 发送协议打包数据
 * 参数: 
 *       cmd 指令 
 *       id设备ID 设备间通讯为2字节 平台通讯为8字节
 * 		 buffer 协议数据体内容
 *       size 数据体长度
 * 
 * 返回:None 
 */
void app_sendProtocolMsg(CMD_TYPE cmd, uint64_t id, char * buffer, uint16_t size)
{
	static uint16_t sn = 0;

	uint16_t len; //打包后数据长度

	switch(cmd)
	{
		//周期广播自身MAC 用于APP发现
		case DEVICE_BIND:
		{
			char content[9];
			content[0] = 2;

			memcpy(content+1, (char *)&device.macaddr, 8);

            //填充数据包
            app_packProtocolMsg(M2M, 0xFFFF, content, sizeof(content), cmd, 0, 0, lora1.t_buffer, &len);

            //发送数据包
			app_sendMsg(DRV_LORA_1, lora1.t_buffer, len);		
		}	break;

		//发送跳传数据
		case DEVICE_JUMP:
		{
			char content[23];

			content[0] = device.communication.sn;
			*(uint16_t *)(content+1) = device.communication.self_id;
			*(uint16_t *)(content+3) = device.communication.start_id;
			*(uint16_t *)(content+5) = device.communication.start_id;
			memcpy(content+7, device.communication.msg, 16);

            //填充数据包
            app_packProtocolMsg(M2M, id, 0, 0, cmd, content, sizeof(content), lora1.t_buffer, &len);

            //发送数据包
			app_sendMsg(DRV_LORA_1, lora1.t_buffer, len);
		}	break;

		//回复上级跳传
		case DEVICE_JUMP_ACK:
		{            
            //填充数据包
            app_packProtocolMsg(M2M, id, 0, 0, cmd, 0, 0, lora1.t_buffer, &len);

            //发送数据包
			app_sendMsg(DRV_LORA_1, lora1.t_buffer, len);
		} break;
		
		//回复本地控制器
		case DEVICE_CONTROL_ACK:
		{
            //填充数据包
            app_packProtocolMsg(M2M, id, 0, 0, cmd, 0, 0, lora1.t_buffer, &len);

            //发送数据包
			app_sendMsg(DRV_LORA_1, lora1.t_buffer, len);
		} break;

		//请求配置
		case SERVER_REQ_CFG:
		{
            //填充数据包
            app_packProtocolMsg(M2S, device.macaddr, sn++, 0, cmd, cat1.ccid, sizeof(cat1.ccid), cat1.t_buffer, &len);

            //发送数据包
			app_sendMsg(DRV_CAT1, cat1.t_buffer, len);
		} break;

		//上报自身状态
		case SERVER_UPLOAD_STATUS:
		{
			char content[19];

			//封装当前版本
			content[0] = VERSION;
			//封装8通道灯光亮度
			memcpy(content+1, device.communication.msg, sizeof(device.communication.msg));
			//封装自身短ID
			*(uint16_t *)(content+9) =  app_changeEndian(device.communication.self_id, 2);
			//封装当前信道
			*(uint32_t *)(content+11) = app_changeEndian(device.loraparam.RFFrequency, 4);
			//封装容灾设备数量
			content[15] = device.communication.max_broadcast_dev_cnts;
			//封装广播次数
			content[16] = device.communication.max_broadcast_times;
			//封装广播间隔
			*(uint16_t *)(content+17) = app_changeEndian(device.communication.max_broadcast_period, 2);

            //填充数据包
            app_packProtocolMsg(M2S, device.macaddr, sn++, 0, cmd, content, sizeof(content), cat1.t_buffer, &len);

            //发送数据包
			app_sendMsg(DRV_CAT1, cat1.t_buffer, len);		
		} break;

		//请求固件
		case SERVER_REQ_FW:
		{
            //定义数据包
            char content[4];  
            
            content[0] = 0;  //固件类型
            content[1] = device.fwver.version;  //固件版本 预留
            *(uint16_t *)(content+2) = app_changeEndian(device.fwver.index, 2);//固件索引
    
            //填充数据包
            app_packProtocolMsg(M2S, device.macaddr, sn++, 0, SERVER_REQ_FW, content,sizeof(content), cat1.t_buffer, &len);

            //发送数据包
			app_sendMsg(DRV_CAT1, cat1.t_buffer, len);          
		} break;

		//回复平台控制
		case SERVER_DOWNLOAD_CTRL_ACK:
		{
            //填充数据包
            app_packProtocolMsg(M2S, device.macaddr, *(uint16_t *)buffer, 0, cmd, 0, 0, cat1.t_buffer, &len);

            //发送数据包
			app_sendMsg(DRV_CAT1, cat1.t_buffer, len);
		} break;

		//反馈升级结果
		case SERVER_IAP_RET:
		{
            //填充数据包
            app_packProtocolMsg(M2S, device.macaddr, sn++, 0, cmd, buffer, size, cat1.t_buffer, &len);

            //发送数据包
			app_sendMsg(DRV_CAT1, cat1.t_buffer, len);
		} break;

		default:	break;
	}
}

/**
 * 功能: 拆分并处理协议数据
 * 参数: 
 * 		 msgtype 消息来源
 * 		 buffer 接收的协议内容
 *       size 接收内容长度
 * 
 * 返回:None 
 */
void app_procProtocolMsg(PROTOCOLTYPE msgtype, char * buffer, uint16_t size)
{
	static char fwcache[IAP_PKTSIZE];
	static uint32_t crc = 0;    //本地计算下发固件的CRC
	static uint32_t fwcrc = 0;  //固件本身携带的crc

    ProtocolParam protocol;
    uint16_t len = 0;

    //接收的数据体长度
    char content[IAP_PKTSIZE]; //接收的数据体部分 考虑配置字节较多的场景(在线升级)

    //解析数据
    if(app_unpackProtocolMsg(msgtype, &protocol, buffer, size, 0, content, &len)==NO)
    {
        return ;
    }

    //判断产品ID 1E表示回路调光 
    if((protocol.command&0xFF000000) != PRODUCT_ID)
    {
        return ;
    }

	if(msgtype==M2M)
	{
		if(protocol.id!=device.communication.self_id && protocol.id!=0xFFFF)  //非法ID 只对自身ID或者广播ID进行处理
		{
			return ;
		}

		switch(protocol.command)
		{
			//接收到手发来的绑定测试指令
			case DEVICE_BIND:
			{
				if(*(uint64_t *)(content+1)==device.macaddr)//判断指令是否是属于自己
				{
					if(content[0]==1)//1 是灯光测试指令
					{
						SWITCH_STATE(TIPS); //灯光提示
					}
				}
			}	break;

			//接收到上级设备的跳传数据 进入跳传任务
			case DEVICE_JUMP:
			{
				device.communication.state = COMMU_ALIGNMENT;
				device.communication.tick = 0;
				device.communication.id_index = 1;
				device.communication.sn = content;
				device.communication.start_id = *(uint16_t *)(content+3);
				device.communication.end_id = *(uint16_t *)(content+5);
				memcpy(device.communication.msg, content+7, sizeof(device.communication.msg));

				app_sendProtocolMsg(DEVICE_JUMP_ACK, *(uint16_t *)(content+1), 0, 0); //回复数据源设备
			}	break;

			//收到下级设备的回复 表示该设备跳传任务结束
			case DEVICE_JUMP_ACK:
			{
				device.communication.state = COMMU_IDEL;
				device.communication.tick = 0;
				device.communication.id_index = 1;
				device.communication.sn = 0;
				
				//完成跳传后并收到回复后 也对自身应用配置
				if(device.communication.self_id>=device.communication.start_id && device.communication.self_id<=device.communication.end_id)
				{
					switch (device.communication.msg[8])
					{
						case 0x01: //重启
						{
							SWITCH_STATE(RESTART);										
						}	break;

						case 0x02:	//恢复出厂
						{
							SWITCH_STATE(RESTORE);															
						}	break;

						default:
							break;
					}
				}

			}	break;

			//接收到本地控制器的控制数据
			case DEVICE_CONTROL: //[失效]
			{
				device.communication.state = COMMU_ALIGNMENT;
				device.communication.tick = 0;
				device.communication.id_index = 1;
				device.communication.sn = 0;
				device.communication.start_id = *(uint16_t *)(content+3);
				device.communication.end_id = *(uint16_t *)(content+5);
				memcpy(device.communication.msg, content+7, 8);

				app_sendProtocolMsg(DEVICE_JUMP_ACK, device.communication.self_id, 0, 0);
			}	break;

			default:	break;
		}
	}else if(msgtype==M2S)
	{
		if(protocol.id!=device.macaddr)  //非法MAC地址
		{
			return ;
		}

		switch(protocol.command)
		{
			//接收到服务器的配置
			case SERVER_REQ_CFG_ACK:
			{
				device.communication.self_id = app_changeEndian(*(uint16_t *)content, 2);
				device.loraparam.RFFrequency = app_changeEndian(*(uint16_t *)(content+2), 4);

				app_saveSysConfig(&device);

				SWITCH_STATE(RESTART); //重启
			}	break;

			//接收到服务器状态包的回复
			case SERVER_UPLOAD_STATUS_ACK:
			{
				device.rev_timeout = 0;

			}	break;

			//接收服务器回复的固件包
			case SERVER_REQ_FW_ACK:
			{
				//过滤非法固件包
				if(device.fwver.index!=app_changeEndian(*(uint16_t *)content, 2))
				{
					return ;
				}

				if(device.fwver.index>=device.fwver.times)
				{
					//升级失败 固件溢出 理论上不会出现这种情况
					device.fwver.iap_ret[0] = IAP_ERR_OVERFLOW;
					device.fwver.iap_ret[1] = VERSION;

					app_sendProtocolMsg(SERVER_IAP_RET, device.macaddr, device.fwver.iap_ret, sizeof(device.fwver.iap_ret));     //回复平台升级结果
					device.fwver.index = 0;
					app_saveSysConfig(&device);

					return ;
				}

                device.iap_timeout = 0;

				memcpy(fwcache, content+2, IAP_PKTSIZE);  //不进行复制会导致总线错误!!!

				if(hal_programFlash(IAP_PART+device.fwver.index*IAP_PKTSIZE, fwcache, IAP_PKTSIZE)==FLASH_SUCCESS)
				{
					app_saveSysConfig(&device); //用于断点续传
					if(device.fwver.index==device.fwver.times-1) //固件升级完成 设置启动参数并重启
					{
						/*开始验证flash存储的数据对不对*/
						crc = 0;
						fwcrc = 0;
						for(uint8_t fragment=0;fragment<device.fwver.times;++fragment) //读取升级片段 计算固件CRC
						{
							hal_readFlash(IAP_PART + fragment*IAP_PKTSIZE, fwcache, IAP_PKTSIZE);

							if(device.fwver.packsize%IAP_PKTSIZE < 4) //固件CRC在上一包
							{
								for(uint16_t i=0;i<IAP_PKTSIZE;++i)
								{
									if(fragment==device.fwver.times-2 && i==IAP_PKTSIZE-(4-device.fwver.packsize%IAP_PKTSIZE))
									{
										break;
									}

									crc += (uint8_t)fwcache[i];
								}

								if(fragment==device.fwver.times-2)
								{
									for(char i=0;i<4-device.fwver.packsize%IAP_PKTSIZE;++i)
									{
										fwcrc |= fwcache[IAP_PKTSIZE-i-1]<<8*(4-device.fwver.packsize%IAP_PKTSIZE-i-1);
									}
								}else if(fragment==device.fwver.times-1)
								{
									for(char i=0;i<device.fwver.packsize%IAP_PKTSIZE;++i)
									{
										fwcrc |= fwcache[i]<<8*(4-device.fwver.packsize%IAP_PKTSIZE+i);
									}
								}else
								{

								}
							}else
							{
								for(uint16_t i=0;i<IAP_PKTSIZE;++i)
								{
									if(fragment==device.fwver.times-1 && i==device.fwver.packsize%IAP_PKTSIZE-4)
									{
										fwcrc = *(uint32_t *)(fwcache+device.fwver.packsize%IAP_PKTSIZE-4);
										break;
									}
									crc += (uint8_t)fwcache[i];
								}
							}
						}

						if(crc==fwcrc)  //固件传输正常
						{
							//升级成功
							device.fwver.iap_ret[0] = IAP_SUCCESS;
							device.fwver.iap_ret[1] = device.fwver.version;

							app_sendProtocolMsg(SERVER_IAP_RET, device.macaddr, device.fwver.iap_ret, sizeof(device.fwver.iap_ret));  //回复平台表示已配置完成

							device.fwver.updatestate = 1;//升级完成标志
							device.fwver.index = 0;
							app_saveSysConfig(&device);
							NVIC_SystemReset();  //重启 由Bootloader进行APP切换
						}else
						{
							//升级失败 CRC错误
							device.fwver.iap_ret[0] = IAP_ERR_CRC;
							device.fwver.iap_ret[1] = VERSION;

							app_sendProtocolMsg(SERVER_IAP_RET, device.macaddr, device.fwver.iap_ret, sizeof(device.fwver.iap_ret));  //回复平台表示已配置完成

							device.fwver.index = 0;
							SWITCH_STATE(CONFIGED);
							app_saveSysConfig(&device);
						}

						crc = 0;
						fwcrc = 0;

						return ;
					}else //加不加都行 如果存储错误 CRC也不会校验通过
					{
						//升级失败 FLASH错误
						device.fwver.iap_ret[0] = IAP_ERR_FALSH;
						device.fwver.iap_ret[1] = VERSION;

						app_sendProtocolMsg(SERVER_IAP_RET, 0, device.fwver.iap_ret, sizeof(device.fwver.iap_ret));  //回复平台表示已配置完成

						SWITCH_STATE(CONFIGED);
					}

					++device.fwver.index;
				}else //防止网络或者FLASH异常导致的请求固件死循环
				{

				}				
			}	break;

			//接收到服务器下发的控制指令
			case SERVER_DOWNLOAD_CTRL:
			{
				app_sendProtocolMsg(SERVER_DOWNLOAD_CTRL_ACK, device.macaddr, (char *)&protocol.sn, 0); //回复平台响应

				switch (content[0])
				{
					case 0x01: //重启

					case 0x02: //恢复出厂
					{
						device.communication.state = COMMU_ALIGNMENT;
						device.communication.tick = 0;
						device.communication.id_index = 1;
						device.communication.sn = 0;
						device.communication.start_id = *(uint16_t *)(content+1);
						device.communication.end_id = *(uint16_t *)(content+3);
						
						memset(device.communication.msg, 0xFF, sizeof(device.communication.msg)); //调光部分填充失效数据 目的是不调光只重启
						device.communication.msg[8] = content[0];
					}	break;

					case 0x03: //设置亮度
					{
						device.communication.state = COMMU_ALIGNMENT;
						device.communication.tick = 0;
						device.communication.id_index = 1;
						device.communication.sn = 0;
						device.communication.start_id = *(uint16_t *)(content+1);
						device.communication.end_id = *(uint16_t *)(content+3);
						
						memset(device.communication.msg, 0xFF, sizeof(device.communication.msg)); //调光部分填充失效数据 目的是不调光只重启
						memcpy(device.communication.msg, content+5, 8); //调光部分填充失效数据 目的是不调光只重启
					}	break;

					case 0x04: //升级推送
					{
                        if(content[1]==VERSION) //相同版本不升级
                        {
							//版本相同 不升级
							device.fwver.iap_ret[0] = IAP_ERR_VER;
							device.fwver.iap_ret[1] = VERSION;

            				app_sendProtocolMsg(SERVER_IAP_RET, 0, device.fwver.iap_ret, sizeof(device.fwver.iap_ret));  //回复平台表示已配置完成

                        	return ;
                        }

                        device.fwver.version = content[1];

                        device.fwver.packsize = app_changeEndian(*(uint32_t *)(content+2), 4);

                        device.fwver.times = device.fwver.packsize/IAP_PKTSIZE;

                        if(device.fwver.packsize%IAP_PKTSIZE)
                        {
                        	++device.fwver.times;
                        }

                        device.fwver.index = 0;

                        device.iap_timeout = 0;

                        SWITCH_STATE(DEVIAP);

                        app_saveSysConfig(&device); //保存状态 用于断点续传
                        hal_eraseFlash(6, 1);
					}	break;

					case 0x05: //控制设备状态
					{
						SWITCH_STATE(content[1]);
					}	break;

					case 0x06: //控制设备角色 0 普通型 1 雷达型
					{
						device.role = content[1];
                        app_saveSysConfig(&device); //保存状态
					}	break;

					case 0x07: //设置设备ID
					{
						device.communication.self_id = *(uint16_t *)(content+1);
                        app_saveSysConfig(&device); //保存状态
					}	break;	

					case 0x08: //设置蓝牙白名单 [预留]
					{

					}	break;	

					case 0x09: //设置LoRa发射功率
					{
						device.loraparam.Power = content[1];
                        app_saveSysConfig(&device); //保存状态
					}	break;	

					case 0x0A: //设置最大广播次数
					{
						device.communication.max_broadcast_times = content[1];
                        app_saveSysConfig(&device); //保存状态
					}	break;	

					case 0x0B: //设置广播间隔
					{
						device.communication.max_broadcast_period = *(uint16_t *)(content+1);
                        app_saveSysConfig(&device); //保存状态
					}	break;	

					case 0x0C: //设置容灾设备数量
					{
						device.communication.max_broadcast_dev_cnts = content[1];
                        app_saveSysConfig(&device); //保存状态
					}	break;	

					case 0x0D: //设置空中参数 [预留]
					{

					}	break;	

					case 0x0E: //设置上传周期 周期在1~20min范围内设置
					{
						device.uploadperiod = *(uint16_t *)(content+1);

						if(device.uploadperiod<60)
						{
							device.uploadperiod = 60;
						}

						if(device.uploadperiod>1200)
						{
							device.uploadperiod = 1200;
						}

                        app_saveSysConfig(&device); //保存状态
					}	break;	

					default:
						break;
				}
			}	break;

			default:	break;
		}		
	}else  
	{
		
	}
}
