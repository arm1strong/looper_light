#ifndef __DRV_SX1278_H
#define __DRV_SX1278_H
#include "stdint.h"
#include "sx1276-LoRa.h"

typedef enum 
{
    LoRa_1 = SPI_2,
}LoRan;

typedef struct
{
    LoRan loraNmb; //LoRa编号 1 or 2

    SX1278Class loraparam; //LoRa参数

    char * t_buffer; //发送数据缓冲区
    char * r_buffer; //接收数据缓冲区
    uint16_t r_tail; //接收数据长度
    void (*write)(char * data, uint16_t size);
    void (*read)(char * data, uint16_t size);
}LoRa_Descriptor;

void drv_initSX1278(LoRa_Descriptor * pdescriptor);
uint32_t drv_pollLoRaEvents(LoRa_Descriptor * pdescriptor);

#endif
