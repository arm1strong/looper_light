#include "drv_cat1.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "string.h"
#include "stdio.h"

GPIOs Cat1_Power = {OUTPUT, GPIOE, GPIO_PIN_7, 0, 0, 0, 0, 0};
GPIOs Cat1_Rst = {OUTPUT, GPIOD, GPIO_PIN_7, 1, 0, 0, 0, 0};
GPIOs Cat1_Sleep = {OUTPUT, GPIOD, GPIO_PIN_4, 0, 0, 0, 0, 0};

/************************复位引脚操作[可作为快速重启]**********************************/
#define PULLUP_RSTPIN()      hal_setGPIOLevel(Cat1_Rst, 1)   //拉高复位引脚
#define PULLDOWN_RSTPIN()    hal_setGPIOLevel(Cat1_Rst, 0)   //拉低复位移引脚 此时复位

/*******************************掉电重启引脚操作**************************************/
#define POWERON()     hal_setGPIOLevel(Cat1_Power, 1)  //开机
#define SHUTDOWN()    hal_setGPIOLevel(Cat1_Power, 0)  //断电关机

/*********************************低功耗控制*****************************************/
#define ENTER_SLEEP()  hal_setGPIOLevel(Cat1_Sleep, 1)   //进入休眠模式
#define EXIT_SLEEP()   hal_setGPIOLevel(Cat1_Sleep, 0)   //推出休眠

/**
 * 功能：发送AT指令
 * 参数: op 操作指令类型
 * 返回: NONE
 * 说明: NONE
 */
static void drv_sendATCmd(NETSTATE op)
{
    static uint8_t atbuffer[50];             //AT指令buffer 最大为create socket AT指令 47字节
    
    memset(atbuffer, 0, sizeof(atbuffer));
 
    switch(op)
    {
        case AT_CHECK      :sprintf(atbuffer,"ATE0\r\n");break;
        case AT_BAUDRATE   :sprintf(atbuffer,"AT+IPR=9600\r\n");break;
        case AT_ACTIVEPDP  :sprintf(atbuffer,"AT+QIACT=1\r\n");break;
        case AT_CONNECT    :sprintf(atbuffer,"AT+QIOPEN=1,0,\"TCP\",\"%d.%d.%d.%d\",%d,0,2\r\n", (CAT1_DEFAULT_IP&0xFF000000)>>24, (CAT1_DEFAULT_IP&0x00FF0000)>>16, (CAT1_DEFAULT_IP&0x0000FF00)>>8, CAT1_DEFAULT_IP&0xFF, CAT1_DEFAULT_PORT);break;
        case AT_DISCONNECT :sprintf(atbuffer,"AT+QICLOSE=0\r\n");break;
        case AT_QUITSEND   :sprintf(atbuffer, "+++");break;
        case AT_SLEEP	   :sprintf(atbuffer, "AT+QSCLKEX=1,2,120\r\n");break;//使能休眠  1S进入休眠 异常恢复休眠时间120min
        case AT_ID		   :sprintf(atbuffer, "AT+QCCID\r\n\r\n");break;//读取卡号

        default:                            break;
    }
    
    hal_sendUARTBytes(UART_2, atbuffer, strlen(atbuffer));
}

/**
 * 发送数据到服务器
 */
static void drv_sendData(char * buffer, uint16_t size)
{
    if(CAT1_EN_SAVEPOWER)
    {
        EXIT_SLEEP();
        HAL_Delay(10);
    }

    hal_sendUARTBytes(UART_2, buffer, size);
    
    if(CAT1_EN_SAVEPOWER)
    {
        ENTER_SLEEP();
    }
}

/**
 * 初始化串口 引脚 状态等
 */
void drv_initCat1(Cat1_Descriptor * pdescriptor)
{   
    hal_initGPIO(Cat1_Power);
    hal_initGPIO(Cat1_Rst);
    hal_initGPIO(Cat1_Sleep);

    hal_initUART(UART_2, 115200);

    pdescriptor->write = drv_sendData;
    pdescriptor->nwkstate = POWERSWITCH;

    pdescriptor->t_buffer = (char *)pvPortMalloc(CAT1_MAX_SENDSIZE);
    pdescriptor->r_buffer = (char *)pvPortMalloc(CAT1_MAX_REVSIZE);
}

/**
 * 网络状态时间轮询 1ms调用一次
 */
NETSTATE drv_pollSCat1Events(Cat1_Descriptor * pdescriptor)
{
    static uint16_t waitcnts = 0;                //指令等待时间计数 单位为1ms

    static uint8_t atresult = 0;                 //指令结果 如果返回OK 或者CONNECT则为1 返回ERROR NO则为0

    static uint8_t retrytimes = 0;               //重试次数 如果某个AT操作失败超过N次则强制重启模组 目前次数为5次

    //接收数据分发
    if(pdescriptor->r_flag)
	{
		switch (pdescriptor->nwkstate)
		{
			case INIT          :
								{
									if(strstr(pdescriptor->r_buffer, "RDY")!=0)
									{
										atresult = 1;
									}else
									{
										atresult = 0;
									}
								}break;
			case AT_CHECK      :
								{
									if(strstr(pdescriptor->r_buffer, "OK")!=0)
									{
										atresult = 1;
									}else
									{
										atresult = 0;
									}
								}break;

			case AT_ID      :
								{
									if(strstr(pdescriptor->r_buffer, "OK")!=0)
									{
										atresult = 1;

				                        memcpy(pdescriptor->ccid, pdescriptor->r_buffer+10, 20);
									}else
									{
										atresult = 0;
									}
								}break;

			case AT_BAUDRATE   :
								{
									/* nothing.. */
								}break;
			case AT_ACTIVEPDP  :
								{
									if(strstr(pdescriptor->r_buffer, "OK")!=0)
									{
										atresult = 1;
									}else
									{
										atresult = 0;
									}
								}break;
			case AT_SLEEP  :
								{
									if(strstr(pdescriptor->r_buffer, "OK")!=0)
									{
										atresult = 1;
									}else
									{
										atresult = 0;
									}
								}break;
			case AT_CONNECT    :
								{
									if(strstr(pdescriptor->r_buffer, "CONNECT")!=0)
									{
										atresult = 1;
									}else
									{
										atresult = 0;
									}
								}break;
			case AT_DISCONNECT :
								{
									if(strstr(pdescriptor->r_buffer, "OK")!=0)
									{
										atresult = 1;
									}else
									{
										atresult = 0;
									}
								}break;
			case AT_QUITSEND   :
								{
									/* nothing.. */
								}break;

			case COMPLETED     :
								{
									/*重连机制*/
									if(strstr(pdescriptor->r_buffer, "NO CARRIER")!=0)
									{
										pdescriptor->nwkstate = POWERSWITCH;
									}else
									{
										pdescriptor->read(pdescriptor->r_buffer, pdescriptor->r_tail);
									}
								}

			default:                            break;

		}

		memset(pdescriptor->r_buffer, 0, CAT1_MAX_REVSIZE);
        pdescriptor->r_flag = 0;
        pdescriptor->r_tail = 0;
	}

    /*网络状态处理 */
    if(pdescriptor->nwkstate!=COMPLETED)
    {
        switch (pdescriptor->nwkstate)
        {
            /*掉电重启*/
            case POWERSWITCH:
            {
                if(waitcnts++<20000/DRV_POLL_PERIOD)  //拉低20S 放电完全
                {
                    if(waitcnts==1)
                    {
                        pdescriptor->restart();
                    }
                	SHUTDOWN();
                }else  
                {
                	POWERON();

                    retrytimes = 0;
                    waitcnts = 0;
                    atresult = 0;

                	pdescriptor->nwkstate = INIT;
                }           
            }break;
            
            /*硬件复位*/
            case RESET_1:
            {
                if(waitcnts++<1000/DRV_POLL_PERIOD)
                {
                	PULLDOWN_RSTPIN();
                }else  
                {
                	PULLUP_RSTPIN();
                
                    retrytimes = 0;
                    waitcnts = 0;
                    atresult = 0;

                    pdescriptor->nwkstate = INIT;
                }
            }break;

            /*模组初始化中 等待15S 模块会在该时间内返回RDY*/
            case INIT:
            {
                if(waitcnts++>=15000/DRV_POLL_PERIOD)
                {
                    waitcnts = 0;
                    
                    if(atresult==1)
                    {
                        retrytimes = 0;
                        atresult = 0;

                        pdescriptor->nwkstate = AT_CHECK;
                    }else  
                    {                        
                        pdescriptor->nwkstate = POWERSWITCH;
                    }
                }
            }break;

            case AT_CHECK:
            {
                if(waitcnts++==0)
                {
                    drv_sendATCmd(AT_CHECK);  //第一时刻发送
                }else if(waitcnts>=1000/DRV_POLL_PERIOD)
                {
                    waitcnts = 0;

                    if(atresult==1)
                    {
                        retrytimes = 0;
                        atresult = 0;

                        pdescriptor->nwkstate = AT_ID;
                    }else  
                    {
                        if(++retrytimes>=5)
                        {
                            retrytimes = 0;
                            pdescriptor->nwkstate = POWERSWITCH;
                        }else
                        {
                            pdescriptor->nwkstate = AT_CHECK;
                        }
                    }                     
                }else   
                {

                }
            }break; 

            case AT_BAUDRATE:
            {
                //暂时不用
            }break;  

            case AT_ID:
            {
                if(waitcnts++==0)
                {
                    drv_sendATCmd(AT_ID);  //第一时刻发送
                }else if(waitcnts>=1000/DRV_POLL_PERIOD)
                {
                    waitcnts = 0;

                    if(atresult==1)
                    {
                        retrytimes = 0;
                        atresult = 0;

                        pdescriptor->nwkstate = AT_ACTIVEPDP;
                    }else
                    {
                        if(++retrytimes>=5)
                        {
                            retrytimes = 0;
                            pdescriptor->nwkstate = POWERSWITCH;
                        }else
                        {
                            pdescriptor->nwkstate = AT_ID;
                        }
                    }
                }else
                {

                }
            }break;

            case AT_ACTIVEPDP:
            {
                if(waitcnts++==0)
                {
                    drv_sendATCmd(AT_ACTIVEPDP);  //第一时刻发送
                }else if(waitcnts>=2000/DRV_POLL_PERIOD)
                {
                    waitcnts = 0;

                    if(atresult==1)
                    {
                        atresult = 0;
                        retrytimes = 0;
                        pdescriptor->nwkstate = AT_SLEEP;  //设置进入低功耗
                        pdescriptor->regist();
                    }else  
                    {
                        if(++retrytimes>=90)  //最长等待3分钟
                        {
                            retrytimes = 0;
                            pdescriptor->nwkstate = POWERSWITCH;
                        }else
                        {
                            pdescriptor->nwkstate = AT_ACTIVEPDP;
                        }
                    }                     
                }else   
                {

                }
            }break;   

            case AT_SLEEP:
            {
                if(waitcnts++==0)
                {
                    drv_sendATCmd(AT_SLEEP);  //第一时刻发送 300ms左右回复
                }else if(waitcnts>=1000/DRV_POLL_PERIOD)
                {
                    //对接收数据进行检测
                    waitcnts = 0;

                    if(atresult==1)
                    {
                        atresult = 0;
                        retrytimes = 0;
                        pdescriptor->nwkstate = AT_CONNECT;
                    }else
                    {
                        if(++retrytimes>=5)
                        {
                            retrytimes = 0;
                        }else
                        {

                        }

                        pdescriptor->nwkstate = POWERSWITCH;
                    }
                }else
                {

                }
            }	break;

            case AT_CONNECT:
            {
                if(waitcnts++==0)
                {
                    drv_sendATCmd(AT_CONNECT);  //第一时刻发送
                }else if(waitcnts>=3000/DRV_POLL_PERIOD)
                {
                    //对接收数据进行检测
                    waitcnts = 0;

                    if(atresult==1)
                    {
                        atresult = 0;
                        retrytimes = 0;
                        pdescriptor->nwkstate = COMPLETED;

                        pdescriptor->connectServerEvent();
                    }else
                    {
                        if(++retrytimes>=5)
                        {
                            retrytimes = 0;
                        }else
                        {
 
                        }
                        pdescriptor->nwkstate = POWERSWITCH;
                    }                     
                }else   
                {

                }
            }break;    

            case AT_DISCONNECT:
            {
                if(waitcnts++<1500/DRV_POLL_PERIOD)
                {
                    /*退出透传前1.5S无操作*/
                }else if(waitcnts==1600/DRV_POLL_PERIOD)
                {
                    /*退出透传*/
                    drv_sendATCmd(AT_QUITSEND); 

                }else if(waitcnts>=3000/DRV_POLL_PERIOD) 
                {
                    if(waitcnts==3000/DRV_POLL_PERIOD)
                    {
                        /*退出透传在等1.5S后发送断开连接*/
                        drv_sendATCmd(AT_DISCONNECT); 
                    }else if(waitcnts>=4000/DRV_POLL_PERIOD)
                    {
                        waitcnts = 0;

                        if(atresult==1)
                        {
                            atresult = 0;
                            retrytimes = 0;
                            pdescriptor->nwkstate = NONEOP;
                        }else  
                        {
                            if(++retrytimes>=5)
                            {
                                retrytimes = 0;
                                pdescriptor->nwkstate = POWERSWITCH;
                            }else
                            {
                                pdescriptor->nwkstate = AT_DISCONNECT;
                            }
                            
                        }
                    }else  
                    {

                    }

                }else
                {

                }
            }break; 

            case AT_QUITSEND:
            {
                if(waitcnts++<1500/DRV_POLL_PERIOD)
                {
                    /*退出透传前1.5S无操作*/
                }else if(waitcnts==1500/DRV_POLL_PERIOD)
                {
                    waitcnts = 0;
                    
                    /*退出透传*/
                    drv_sendATCmd(AT_QUITSEND); 

                }else if(waitcnts>=3000/DRV_POLL_PERIOD) 
                {
                    pdescriptor->nwkstate = NONEOP;
                }else
                {

                }
            }break;  
                        
            default:break;
        }
    }

    return pdescriptor->nwkstate;
}
