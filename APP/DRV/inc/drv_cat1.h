#ifndef __DRV_CAT1_H
#define __DRV_CAT1_H
#include "stdint.h"
#include "stdbool.h"
#include "global_config.h"

/*****************模块初始化流程枚举*******************************/
typedef enum 
{
    RESET_1         = 0,  //复位 等待15S 模块就绪后会返回RDY
    INIT            = 1,  //初始化状态 
    AT_CHECK        = 2,  //ATE0指令 检测模块是否正常并关闭模块回显
    AT_BAUDRATE     = 3,  //设置波特率为9600 [暂时不用]
    AT_ACTIVEPDP    = 4,  //激活PDP上下文
    AT_CONNECT      = 5,  //连接服务器
    AT_QUITSEND     = 7,  //退出透传 +++
    AT_DISCONNECT   = 6,  //断开与服务器的连接
    COMPLETED       = 8,  //初始化成功

    POWERSWITCH     = 9,  //重启模块
        
    NONEOP          = 10, //空操作 当断开连接时处于此状态

	AT_SLEEP		= 11, //低功耗操作

	AT_ID			= 12, //读取卡号

    SEND            = 100 //发送状态
}NETSTATE;

typedef struct 
{
    uint8_t protocoltype;             //协议类型 目前使用TCP [预留]

    NETSTATE nwkstate;                //当前操作

    bool r_flag;				      //是否有来自Cat-1模块的新消息 
    char * t_buffer;                  //发送数据缓冲区
    char * r_buffer;                  //接收缓冲区
    uint16_t r_tail;                  //接收数据索引 可作为接收数据字节数

    char ccid[20];                    //SIM卡号

    void (*write)(char * data, uint16_t size);//发送函数
    void (*read)(char * data, uint16_t size); //接收回调函数
    void (*connectServerEvent)(void); //当连接到服务器后调用该函数 可上电后向平台索要信息或者报告自身状态
    void (*restart)(void);            //断开事件
    void (*regist)(void);             //网络注册事件
}Cat1_Descriptor;

void drv_initCat1(Cat1_Descriptor * pdescriptor);
NETSTATE drv_pollSCat1Events(Cat1_Descriptor * pdescriptor);

#endif
