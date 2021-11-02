#include "app_protocol.h"
#include <string.h>
#include "drv_sx1278.h"
#include "hal_uart.h"

// unsigned char tbuffer[TBUF_SIZE] = {0};

/**
 * 功能：计算CRC
 * 参数: data 数据 len 长度
 * 返回: 计算结果
 */
static unsigned short getCrc16(unsigned char * data, unsigned short len)
{
    unsigned short ret = 0;

    for(unsigned short i=0;i<len;++i)
    {
        ret += data[i];
    }

    return ret;
}


/**
 * 功能：协议打包发送函数
 * 参数: protocoltype 协议类型  id M2M取低2字节 cmd 指令码 data 待发送数据 len 发送数据长度 tbuffer 打包完成后存入的缓冲区  size buffer长度
 * 返回: 是否发送成功 OK or NO
 *
 * 最近修改:
 *
 *          [20210111] MAC地址改为小端上传
 */
BOOL app_packProtocolMsg(PROTOCOLTYPE protocoltype, unsigned long long id, unsigned short sn, unsigned char cryption, unsigned int cmd, char * data, unsigned short len, char * tbuffer, unsigned short * size)
{
    BOOL ret = OK;

    /*判断协议类型*/
    if(protocoltype==M2M)
    {
        /*避免内存溢出*/
        if(M2MPROTOCOL_SIZE+len>TBUF_SIZE)
        {
            return NO;
        }

        /*封装头*/
        *(unsigned short *)tbuffer = HEAD;

        /*封装设备ID*/
        *(unsigned short *)(tbuffer + HEAD_SIZE) = id;

        /*封装指令功能码*/
        *(unsigned int *)(tbuffer + HEAD_SIZE + DEVICEID_SIZE) = cmd;

        /*封装数据长度*/
        *(unsigned short *)(tbuffer + HEAD_SIZE + DEVICEID_SIZE + COMMAND_SIZE) = len;

        /*封装数据体*/
        memcpy(tbuffer+HEAD_SIZE+DEVICEID_SIZE+COMMAND_SIZE+DATALEN_SIZE, data, len);

        /*封装CRC*/
        *(unsigned short *)(tbuffer + M2MPROTOCOL_SIZE - CRC_SIZE + len) = getCrc16(tbuffer, M2MPROTOCOL_SIZE - CRC_SIZE + len);

        *size = M2MPROTOCOL_SIZE + len;
        /*发送数据*/
        // callback_sendMsg(tbuffer, M2MPROTOCOL_SIZE+len);
    }else  
    {
        /*避免内存溢出*/
        if(M2SPROTOCOL_SIZE+len>TBUF_SIZE)
        {
            return NO;
        }

        /*封装头*/
        *(unsigned short *)tbuffer = HEAD>>8 | HEAD<<8;

        /*封装设备ID*/
//        for(unsigned char i=0;i<8;++i)
//        {
//            *(unsigned char *)(tbuffer + HEAD_SIZE+i) = id>>(7-i)*8;
//        }
        memcpy(tbuffer+HEAD_SIZE, (char *)&id, 8);
        /*封装SN号*/
        *(unsigned short *)(tbuffer + HEAD_SIZE+NETWAYMAC_SIZE) = sn>>8 | sn<<8;

        /*封装加密方式*/
        tbuffer[HEAD_SIZE+NETWAYMAC_SIZE+SN_SIZE] = cryption;

        /*封装指令功能码*/
        *(unsigned int *)(tbuffer + HEAD_SIZE+NETWAYMAC_SIZE+SN_SIZE+CRYPTION_SIZE) = (cmd&0xFF000000)>>24 | (cmd&0x00FF0000)>>8 | (cmd&0x0000FF00)<<8 | (cmd&0x000000FF)<<24;

        /*封装数据长度*/
        *(unsigned short *)(tbuffer + HEAD_SIZE+NETWAYMAC_SIZE+SN_SIZE+CRYPTION_SIZE+COMMAND_SIZE) = len>>8 | len<<8;

        /*封装数据体*/
        memcpy(tbuffer+HEAD_SIZE+NETWAYMAC_SIZE+SN_SIZE+CRYPTION_SIZE+COMMAND_SIZE+DATALEN_SIZE, data, len);

        /*封装CRC*/
        unsigned short crc = getCrc16(tbuffer, M2SPROTOCOL_SIZE - CRC_SIZE + len);
        *(unsigned short *)(tbuffer + M2SPROTOCOL_SIZE - CRC_SIZE + len) = crc>>8 | crc<<8;

        /*发送数据*/
        // callback_sendMsg(tbuffer, M2SPROTOCOL_SIZE+len);
        *size = M2SPROTOCOL_SIZE + len;
    }

    return OK;
}



/**
 * 功能：协议解包发送函数
 * 参数: protocoltype 协议类型 id M2M取低2字节 cmd 指令码 data 待发送数据 len 发送数据长度 id 自身ID 用于和接收ID比较 rbuffer 解包后有效数据部分缓冲区 size 有效数据长度[暂时没用]
 * 返回: 是否发送成功 OK or NO
 */
BOOL app_unpackProtocolMsg(PROTOCOLTYPE protocoltype, ProtocolParam * pstruct, char * revmsg, unsigned short len, unsigned long long id, char * rbuffer, unsigned short * size)
{
    BOOL ret = OK;
    
    //小于2会导致 在计算CRC时数组越界
    if(len<2)
    {
        return NO;
    }
    
    if(protocoltype==M2M)
    {
        /*数据头错误*/
        if(*(unsigned short *)revmsg!=HEAD)
        {
            pstruct->errcode = CODE_ERR_HEAD;
            return NO;
        }

        /*CRC错误*/
        unsigned short crc = getCrc16(revmsg, len-sizeof(short));
        if(*(unsigned short *)(revmsg+len-sizeof(short))!=crc)
        {
            pstruct->errcode = CODE_ERR_CRC;
            return NO;
        }

        /*数据长度错误*/
        if(*(unsigned short *)(revmsg+HEAD_SIZE+DEVICEID_SIZE+COMMAND_SIZE)+M2MPROTOCOL_SIZE!=len)
        {
            pstruct->errcode = CODE_ERR_LEN;
            return NO;
        }

        /*无错误 开始解析*/
        /*解析功能码*/
        pstruct->command = *(unsigned int *)(revmsg+HEAD_SIZE+DEVICEID_SIZE);
        /*获取ID*/
        pstruct->id = *(unsigned short *)(revmsg+HEAD_SIZE);
        /*解析数据长度*/
        pstruct->len = *(unsigned short *)(revmsg+HEAD_SIZE+DEVICEID_SIZE+COMMAND_SIZE);
        /*解析数据*/
        memcpy(rbuffer, revmsg+HEAD_SIZE+DEVICEID_SIZE+COMMAND_SIZE+DATALEN_SIZE, pstruct->len);
    }else  
    {
        /*检测ID是否正确*/
        unsigned char i = 0;
        
//        id = (id&0xFF00000000000000)>>56 | (id&0x00FF000000000000)>>40 | (id&0x0000FF0000000000)>>24 \
//                                    | (id&0x000000FF00000000)>>8 | (id&0x00000000FF000000)<<8 | (id&0x0000000000FF0000)<<24 \
//                                    | (id&0x000000000000FF00)<<40 | (id&0x00000000000000FF)<<56;
        unsigned long long tmpmacaddr;
        
        memcpy((char *)&tmpmacaddr, revmsg + HEAD_SIZE, 8);
        
        if(tmpmacaddr!=id)
        {
            pstruct->errcode = CODE_ERR_ID;
            return NO;
        }

        /*数据头错误*/
        if(*(unsigned short *)revmsg!=((HEAD&0xFF00)>>8 | (HEAD&0x00FF)<<8))
        {
            pstruct->errcode = CODE_ERR_HEAD;
            return NO;
        }

        /*CRC错误*/
        if(getCrc16((unsigned char *)revmsg, len-sizeof(short))!=((*(unsigned short *)(revmsg+len-sizeof(short))&0xFF00)>>8 | (*(unsigned short *)(revmsg+len-sizeof(short))&0x00FF)<<8))
        {
            pstruct->errcode = CODE_ERR_CRC;
            return NO;
        }

        /*数据长度错误*/
        if((((*(unsigned short *)(revmsg+HEAD_SIZE+NETWAYMAC_SIZE+COMMAND_SIZE+SN_SIZE+CRYPTION_SIZE)&0xFF00)>>8)|((*(unsigned short *)(revmsg+HEAD_SIZE+NETWAYMAC_SIZE+COMMAND_SIZE+SN_SIZE+CRYPTION_SIZE))&0x00FF)<<8)+M2SPROTOCOL_SIZE!=len)
        {
            pstruct->errcode = CODE_ERR_LEN;
            return NO;
        }

        pstruct->errcode = CODE_OK;
        /*无错误 开始解析*/
        /*解析SN*/
        pstruct->sn = *(unsigned short *)(revmsg+HEAD_SIZE+NETWAYMAC_SIZE);
        pstruct->sn = pstruct->sn>>8 | pstruct->sn<<8;
        /*解析ID*/
        memcpy(&pstruct->id, revmsg+HEAD_SIZE, 8);
        pstruct->id = (pstruct->id&0xFF00000000000000)>>56 | (pstruct->id&0x00FF000000000000)>>40 | (pstruct->id&0x0000FF0000000000)>>24 \
                                    | (pstruct->id&0x000000FF00000000)>>8 | (pstruct->id&0x00000000FF000000)<<8 | (pstruct->id&0x0000000000FF0000)<<24 \
                                    | (pstruct->id&0x000000000000FF00)<<40 | (pstruct->id&0x00000000000000FF)<<56;  

        /*解析功能码*/
        pstruct->command = *(unsigned int *)(revmsg+HEAD_SIZE+NETWAYMAC_SIZE+SN_SIZE+CRYPTION_SIZE);
        pstruct->command = (pstruct->command&0xFF000000)>>24 | (pstruct->command&0x00FF0000)>>8 | (pstruct->command&0x0000FF00)<<8 | (pstruct->command&0x000000FF)<<24;
        /*解析数据长度*/
        pstruct->len = *(unsigned short *)(revmsg+HEAD_SIZE+NETWAYMAC_SIZE+SN_SIZE+CRYPTION_SIZE+COMMAND_SIZE);
        pstruct->len = (pstruct->len&0xFF00)>>8 | (pstruct->len&0x00FF)<<8;
        
        /*解析数据*/
        memcpy(rbuffer, revmsg+HEAD_SIZE+NETWAYMAC_SIZE+SN_SIZE+CRYPTION_SIZE+COMMAND_SIZE+DATALEN_SIZE, pstruct->len);
    }
    
    return OK;
}
