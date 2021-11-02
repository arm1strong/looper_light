#ifndef __MX25_H
#define __MX25_H

#include "MX25_DEF.h"
#include <stdint.h>

#define FLASH_OK    0
#define FLASH_FAIL  1

/*** MX25 series command hex code definition ***/
//ID comands
#define    FLASH_CMD_RDID      0x9F    //RDID (Read Identification)
#define    FLASH_CMD_RES       0xAB    //RES (Read Electronic ID)
#define    FLASH_CMD_REMS      0x90    //REMS (Read Electronic & Device ID)

//Register comands
#define    FLASH_CMD_WRSR      0x01    //WRSR (Write Status Register)
#define    FLASH_CMD_RDSR      0x05    //RDSR (Read Status Register)

//READ comands
#define    FLASH_CMD_READ        0x03    //READ (1 x I/O)
#define    FLASH_CMD_FASTREAD    0x0B    //FAST READ (Fast read data)
#define    FLASH_CMD_DREAD       0x3B    //DREAD (1In/2 Out fast read)
#define    FLASH_CMD_RDSFDP      0x5A    //RDSFDP (Read SFDP)

//Program comands
#define    FLASH_CMD_WREN     0x06    //WREN (Write Enable)
#define    FLASH_CMD_WRDI     0x04    //WRDI (Write Disable)
#define    FLASH_CMD_PP       0x02    //PP (page program)

//Erase comands
#define    FLASH_CMD_SE       0x20    //SE (Sector Erase)
#define    FLASH_CMD_BE       0xD8    //BE (Block Erase)
#define    FLASH_CMD_CE       0x60    //CE (Chip Erase) hex code: 60 or C7

//Mode setting comands
#define    FLASH_CMD_DP       0xB9    //DP (Deep Power Down)
#define    FLASH_CMD_RDP      0xAB    //RDP (Release form Deep Power Down)
#ifdef SBL_CMD_0x77
#else
#endif

//Reset comands

//Security comands
#ifdef LCR_CMD_0xDD_0xD5
#else
#endif

//Suspend/Resume comands

// Return Message
typedef enum {
    FlashOperationSuccess,
    FlashWriteRegFailed,
    FlashTimeOut,
    FlashIsBusy,
    FlashQuadNotEnable,
    FlashAddressInvalid
}ReturnMsg;

// Flash status structure define
struct sFlashStatus{
    /* Mode Register:
     * Bit  Description
     * -------------------------
     *  7   RYBY enable
     *  6   Reserved
     *  5   Reserved
     *  4   Reserved
     *  3   Reserved
     *  2   Reserved
     *  1   Parallel mode enable
     *  0   QPI mode enable
    */
    unsigned char  ModeReg;
    char     ArrangeOpt;
};

typedef struct sFlashStatus FlashStatus;

/*******************驱动层函数*********************/
/*扩展多种驱动*/
#define FLASH_MX25R8035F 0
#define FLASH_OTHER      1

/*使用MX25R8035F作为flash*/
#define USEFLASH  FLASH_MX25R8035F

/*扇区尺寸*/
#define SECTOR_SIZE 4096
/*扇区数量*/
#define SECTOR_NUMBER 64

typedef struct
{
    uint32_t id;
    void (*write)(unsigned int addr, char * data, unsigned short len);
    void (*read)(unsigned int addr, char * data, unsigned short len);
}FLASH_Descriptor;

/*基础函数*/
void drv_initFlash(FLASH_Descriptor * pdescriptor);

/*扩展函数*/
//void app_writeSectorData(unsigned char sector, char * data, unsigned short len);
//unsigned char app_readSectorData(unsigned char sector, char * rbuffer, unsigned short len);

/******************基础函数[from:官方SDK]************************/
/* Basic functions */
void CS_High(void);
void CS_Low(void);
void InsertDummyCycle( unsigned char dummy_cycle );
void SendByte( unsigned char byte_value, unsigned char transfer_type );
unsigned char GetByte( unsigned char transfer_type );

/* Utility functions */
void Wait_Flash_WarmUp(void);
void Initial_Spi(void);
unsigned char WaitFlashReady( unsigned int ExpectTime );
unsigned char WaitRYBYReady( unsigned int ExpectTime );
unsigned char IsFlashBusy( void );
unsigned char IsFlashQIO( void );
unsigned char IsFlash4Byte( void );
void SendFlashAddr( unsigned int flash_address, unsigned char io_mode, unsigned char addr_mode );
unsigned char GetDummyCycle( unsigned int default_cycle );

/* Flash commands */
ReturnMsg CMD_RDID( unsigned int *Identification );
ReturnMsg CMD_RES( unsigned char *ElectricIdentification );
ReturnMsg CMD_REMS( unsigned short *REMS_Identification, FlashStatus *fsptr );

ReturnMsg CMD_RDSR( unsigned char *StatusReg );
#ifdef SUPPORT_WRSR_CR
   ReturnMsg CMD_WRSR( unsigned short UpdateValue );
#else
   ReturnMsg CMD_WRSR( unsigned char UpdateValue );
#endif

ReturnMsg CMD_READ( unsigned int flash_address, unsigned char *target_address, unsigned int byte_length  );
ReturnMsg CMD_FASTREAD( unsigned int flash_address, unsigned char *target_address, unsigned int byte_length );
ReturnMsg CMD_DREAD( unsigned int flash_address, unsigned char *target_address, unsigned int byte_length );
ReturnMsg CMD_RDSFDP( unsigned int flash_address, unsigned char *target_address, unsigned int byte_length );

ReturnMsg CMD_WREN( void );
ReturnMsg CMD_WRDI( void );
ReturnMsg CMD_PP( unsigned int flash_address, unsigned char *source_address, unsigned int byte_length );

ReturnMsg CMD_SE( unsigned int flash_address );
ReturnMsg CMD_BE( unsigned int flash_address );
ReturnMsg CMD_CE( void );

ReturnMsg CMD_DP( void );
ReturnMsg CMD_RDP( void );

#endif    /* __MX25_CMD_H__ */
