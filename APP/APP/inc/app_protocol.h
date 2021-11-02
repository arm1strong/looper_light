/**********************************************************
 * 文件: protocol.h
 * 功能: 根据协议格式提供对发送数据的打包以及对接收数据的解析
 * 作者: zx  
 * 日期: 2020-03-04    
 * 版本: 
 *          Ver.1.0  实现打包/解包功能
 * 
 * 说明: 开发者只需关心  1. 使用打包/解包函数  2. 根据各自平台差异 将发送函数填入protocol.c内的发送回调函数中  3.根据情况修改发送和接收数据缓冲区的尺寸 以及设备参数
 * 
 *  Copyright (C) 2018 JIUTONG. All rights reserved.
 *********************************************************/
#ifndef __PROTOCOL_H
#define __PROTOCOL_H


/*************************功能码******************************/
/**************产品线定义***************/
#define ROADNAIL            0x01000000      //道钉
#define WEATHERSTATION      0x02000000      //小气象
/* custom define */

/***************传输方向定义************/
#define SERVERDOWNLOAD 0x00010000           //平台下发数据
#define DEVICEACK      0x00020000           //设备返回数据到平台
#define DEVICEUPLOAD   0x00030000           //设备主动上传


/***************错误码定义**************/
#define CODE_OK         0x00000000   
#define CODE_ERR_CRC    0x00000001
#define CODE_ERR_FUN    0x00000002
#define CODE_ERR_LEN    0x00000003
#define CODE_ERR_DATA   0x00000004
#define CODE_ERR_HEAD   0x00000005
#define CODE_ERR_ID     0x00000006

/**************************协议结构***********************************/
#define HEAD_SIZE               2       //协议头尺寸
#define DEVICEID_SIZE           2       //设备ID  设备指代中继和道钉这类网关下设备
#define COMMAND_SIZE            4       //协议指令码尺寸
#define CRC_SIZE                2       //CRC尺寸
#define DATALEN_SIZE            2       //数据长度尺寸
#define NETWAYMAC_SIZE          8       //网关ID尺寸
#define SN_SIZE                 2       //SN尺寸
#define CRYPTION_SIZE           1       //加密尺寸
#define M2MPROTOCOL_SIZE  HEAD_SIZE+DEVICEID_SIZE+COMMAND_SIZE+CRC_SIZE+DATALEN_SIZE                            //设备间的通讯协议框架尺寸
#define M2SPROTOCOL_SIZE  HEAD_SIZE+NETWAYMAC_SIZE+COMMAND_SIZE+CRC_SIZE+DATALEN_SIZE+SN_SIZE+CRYPTION_SIZE     //设备服务器间的通讯框架尺寸

#define HEAD                    0x4A54   //头部固定标识



/*********使用配置**********/
#define TBUF_SIZE 256                       //协议发送缓冲区大小
#define RBUF_SIZE 1024                      //协议接收有效数据部分的缓冲区大小

#define PROTOCOL_TYPE M2M                   //不同类型对应不同协议标准

#define CRYPTOGRAPHY_TYPE  NONE             //加密类型

#define PROJECT_TYPE ROADNAIL               //产品线定义


/******协议类型********/
typedef enum
{
    M2M = 0x01,     //设备间通信协议
    M2S = 0x02      //设备到服务器通讯协议
}PROTOCOLTYPE;

/******加密方式********/
typedef enum 
{
    NONE   = 0x00,
    DES    = 0x01,
    AES    = 0x02,
    BASE64 = 0x03,
    CUSTOM = 0x04
}CRYPTOGRAPHYTYPE;

/*返回值*/
typedef enum  
{
    NO = 0,
    OK = 1
}BOOL;

/*协议参数 存储协议的功能码 数据长度 数据类型*/
typedef struct protocol
{
    unsigned long long id;                  //ID
    unsigned int command;                   //指令功能码

    unsigned int errcode;                   //错误码

    unsigned short len;                     //有效数据长度

    unsigned short sn;                      //SN 
}ProtocolParam;

BOOL app_packProtocolMsg(PROTOCOLTYPE protocoltype, unsigned long long id, unsigned short sn, unsigned char cryption, unsigned int cmd, char * data, unsigned short len, char * tbuffer, unsigned short * size);
      
BOOL app_unpackProtocolMsg(PROTOCOLTYPE protocoltype, ProtocolParam * pstruct, char * revmsg, unsigned short len, unsigned long long id, char * rbuffer, unsigned short * size);

#endif // ! 
