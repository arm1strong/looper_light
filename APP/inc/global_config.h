#ifndef __CFG_H
#define __CFG_H

/******************系统参数***************************/
#define VERSION             1 //软件版本号
#define DRV_POLL_PERIOD     1 //操作系统轮询的周期 默认1ms

#define BOOT_PART           0x08000000   //引导区地址
#define BOOT_PART_SIZE      (32*1024)    //引导区大小 32KB SECTOR*2
#define BOOT_PART_SECNUM    2            //所占扇区数量

#define CFG_PART            (BOOT_PART+BOOT_PART_SIZE)//配置区地址
#define CFG_PART_SIZE       (32*1024)    //配置区大小 32KB SECTOR*2
#define CFG_PART_SECNUM     2

#define RESERVE_PART        (BOOT_PART+BOOT_PART_SIZE+CFG_PART_SIZE)//保留 可用作给BOOT升级或者存储数据
#define RESERVE_PART_SIZE   (64*1024)
#define RESERVE_PART_SECNUM 1

#define APP_PART            (BOOT_PART+BOOT_PART_SIZE+CFG_PART_SIZE+RESERVE_PART)//APP区地址
#define APP_PART_SIZE       (128*1024)   //APP区大小 128KB 用于存储固件 SECTOR*1
#define APP_PART_SECNUM     1

#define IAP_PART            (BOOT_PART+BOOT_PART_SIZE+CFG_PART_SIZE+RESERVE_PART+APP_PART_SIZE)//IAP升级时APP新固件存储地址
#define IAP_PART_SIZE       (128*1024)  //IAP区大小  128KB SECTOR*1
#define IAP_PART_SECNUM     1

#define DATA_PART           (BOOT_PART+BOOT_PART_SIZE+CFG_PART_SIZE+RESERVE_PART+APP_PART_SIZE+IAP_PART_SIZE)//数据存储区 该区域可存储用户数据
#define DATA_PART_SIZE      (128*1024) 
#define DATA_PART_SECNUM    1

/******************RS485配置区************************/
#define RS485_1_MAX_REVSIZE 256 //485 1接收缓冲区深度

#define RS485_2_MAX_REVSIZE 256 //485 2接收缓冲区深度

/*****************LoRa配置区*************************/
#define LORA_1_MAX_REVSIZE  256 //LoRa 1接收区深度

/******************Cat1配置区************************/
#define CAT1_EN_SAVEPOWER   0    //默认不使用低功耗 1表示使能低功耗
#define CAT1_MAX_REVSIZE    1024 //Cat1接收缓冲区深度
#define CAT1_DEFAULT_IP     (192<<24 | 168<<16 | 1<<8 | 128)
#define CAT1_DEFAULT_PORT   60780

/*****************BLE配置区*************************/
#define BLE_MAX_REVSIZE     256 //BLE接收区深度

/******************PWM配置**************************/
#define PWM_FREQ            1000    //方波周期 Hz

/**************跳传消息长度配置*********************/
#define COMMU_MSG_SIZE      8      //8字节通道亮度    

#endif