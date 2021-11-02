#ifndef __CFG_H
#define __CFG_H

/******************系统参数***************************/
#define VERSION             1   //软件版本号

#define PRODUCT_ID          0x1E000000 //产品ID

#define DRV_POLL_PERIOD     10  //驱动轮询的周期 默认10ms
#define COMMU_POLL_PERIOD   10  //跳传事件轮询周期 
#define APP_POLL_PERIOD     100 //主任务状态轮询周期

#define BOOT_PART           0x08000000UL //引导区地址
#define BOOT_PART_SIZE      (32*1024)    //引导区大小 32KB SECTOR*2
#define BOOT_PART_SECNUM    2            //所占扇区数量

#define CFG_PART            (BOOT_PART+BOOT_PART_SIZE)//配置区地址
#define CFG_PART_SIZE       (32*1024)    //配置区大小 32KB SECTOR*2
#define CFG_PART_SECNUM     2

#define RESERVE_PART        (BOOT_PART+BOOT_PART_SIZE+CFG_PART_SIZE)//保留 可用作给BOOT升级或者存储数据
#define RESERVE_PART_SIZE   (64*1024)
#define RESERVE_PART_SECNUM 1

#define APP_PART            (BOOT_PART+BOOT_PART_SIZE+CFG_PART_SIZE+RESERVE_PART_SIZE)//APP区地址
#define APP_PART_SIZE       (128*1024)   //APP区大小 128KB 用于存储固件 SECTOR*1
#define APP_PART_SECNUM     1 

#define IAP_PART            (BOOT_PART+BOOT_PART_SIZE+CFG_PART_SIZE+RESERVE_PART_SIZE+APP_PART_SIZE)//IAP升级时APP新固件存储地址
#define IAP_PART_SIZE       (128*1024)  //IAP区大小  128KB SECTOR*1
#define IAP_PART_SECNUM     1

#define DATA_PART           (BOOT_PART+BOOT_PART_SIZE+CFG_PART_SIZE+RESERVE_PART_SIZE+APP_PART_SIZE+IAP_PART_SIZE)//数据存储区 该区域可存储用户数据
#define DATA_PART_SIZE      (128*1024) 
#define DATA_PART_SECNUM    1

#define IAP_PKTSIZE         512         //在线升级固件包大小

/*****************调试串口配置区**********************/
#define DEBUG_MAX_SENDSIZE 30
#define DEBUG_MAX_REVSIZE  30

/******************RS485配置区************************/
#define RS485_1_MAX_SENDSIZE 30 //485 1发送缓冲区深度
#define RS485_1_MAX_REVSIZE  30 //485 1接收缓冲区深度

#define RS485_2_MAX_SENDSIZE 30 //485 2发送缓冲区深度
#define RS485_2_MAX_REVSIZE  30 //485 2接收缓冲区深度

/*****************LoRa配置区*************************/
#define LORA_1_MAX_SENDSIZE  256 //LoRa 1接收区深度
#define LORA_1_MAX_REVSIZE   256 //LoRa 1接收区深度

/******************Cat1配置区************************/
#define CAT1_EN_SAVEPOWER   0    //默认不使用低功耗 1表示使能低功耗
#define CAT1_MAX_SENDSIZE   100  //Cat1发送缓冲区深度
#define CAT1_MAX_REVSIZE    (IAP_PKTSIZE+30) //Cat1接收缓冲区深度
#define CAT1_DEFAULT_IP     (103<<24 | 46<<16 | 128<<8 | 21)
#define CAT1_DEFAULT_PORT   25436

/*****************BLE配置区*************************/
#define BLE_MAX_SENDSIZE     256 //BLE接收区深度
#define BLE_MAX_REVSIZE      256 //BLE接收区深度

/******************PWM配置**************************/
#define PWM_FREQ            1000    //方波周期 Hz

/**************跳传消息长度配置*********************/
#define COMMU_MSG_SIZE      16      //8字节通道亮度 + 8字节配置

#endif
