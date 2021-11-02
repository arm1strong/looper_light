/*
 * COPYRIGHT (c) 2010-2012 MACRONIX INTERNATIONAL CO., LTD
 * SPI Flash Low Level Driver (LLD) Sample Code
 *
 * SPI and QPI interface command set
 *
 * $Id: MX25_CMD.c,v 1.18 2012/08/27 08:03:08 mxclldb1 Exp $
 */

#include "drv_mx25.h"
#include "hal_spi.h"
#include "hal_gpio.h"

GPIOs MX25_SPI_NSS = {OUTPUT, GPIOB, GPIO_PIN_12, 1, 0, 0, 0, 0};

/*********************************基础函数 实现初始化 读/写**************************************/
/**
 * 写数据 addr必须为4096整数倍
 * ret : FLASH_OK / FLASH_FAIL
 */
static char drv_writeData(unsigned int addr, char * data, unsigned short len)
{
    unsigned short timeout = 100;

    if(len > 4096)
    {
        return FLASH_FAIL;
    } 

    if(addr%SECTOR_SIZE==0)  //只有在对新段编程时才擦除
    {
        if(CMD_SE(addr)!=FlashOperationSuccess)
        {
            return FLASH_FAIL;
        }
    }

    //整数
    for(unsigned char i=0;i<len/256;++i)
    {
        if(CMD_PP(addr+i*256, data+i*256, 256)!=FlashOperationSuccess)
        {
            return FLASH_FAIL;
        }
    }
    
    if(CMD_PP(addr+len-len%256, data+len-len%256, len%256)!=FlashOperationSuccess)
    {
        return FLASH_FAIL;
    }
      
    return FLASH_OK;
}

/**
 *读数据 addr必须为4096整数倍
 */
static char drv_readData(unsigned int addr, char * rbuffer, unsigned short len)
{
    unsigned char ret = 0;

    if(len > 4096)
    {
        return FALSE;
    }
    
    for(unsigned char i=0;i<len/256;++i)
    {
        if(CMD_READ(addr+i*256, rbuffer+i*256, 256)!=FlashOperationSuccess)
        {
            return FLASH_FAIL;
        }
    }
    
    if(CMD_READ(addr+len-len%256, rbuffer+len-len%256, len%256)!=FlashOperationSuccess)
    {
        return FLASH_FAIL;
    }

    return FLASH_OK;
}


/**
 * 初始化flash 页组成扇区 扇区组成块 块组成最终的flash 数据读写最大单元为一个页
 */
void drv_initFlash(FLASH_Descriptor * pdescriptor)
{
    Initial_Spi();

    CMD_RDID(&pdescriptor->id); //读取ID
    pdescriptor->write = drv_writeData;
    pdescriptor->read = drv_readData;
}


/*
 * Function:       Wait_Flash_WarmUp
 * Arguments:      None.
 * Description:    Wait some time until flash read / write enable.
 * Return Message: None.
 */
void Wait_Flash_WarmUp(void)
{
    unsigned int time_cnt = FlashFullAccessTime;
    while( time_cnt > 0 )
    {
        time_cnt--;
    }
}

/*
 * Function:       Initial_Spi
 * Arguments:      None
 * Description:    Initial spi flash state and wait flash warm-up
 *                 (enable read/write).
 * Return Message: None
 */
void Initial_Spi(void)
{
    hal_initGPIO(MX25_SPI_NSS);
    hal_initSPI(SPI_1, POLARITY_LOW, PHASE_1EGE);
    // Wait flash warm-up
    Wait_Flash_WarmUp();
}

/*
 * Function:       CS_Low, CS_High
 * Arguments:      None.
 * Description:    Chip select go low / high.
 * Return Message: None.
 */
void CS_Low(void)
{
#ifdef GPIO_SPI
    CSn = 0;
#else
    hal_setGPIOLevel(MX25_SPI_NSS, 0);
#endif
}

void CS_High(void)
{
#ifdef GPIO_SPI
    CSn = 1;
    WPn = 1;
#else
    hal_setGPIOLevel(MX25_SPI_NSS, 1);
#endif
}

/*
 * Function:       InsertDummyCycle
 * Arguments:      dummy_cycle, number of dummy clock cycle
 * Description:    Insert dummy cycle of SCLK
 * Return Message: None.
 */
void InsertDummyCycle( unsigned char dummy_cycle )
{
#ifdef GPIO_SPI
    unsigned char i;
    for( i=0; i < dummy_cycle; i=i+1 )
    {
        SCLK = 0;
        SCLK = 1;
    }
#else
    for(char i=0; i < dummy_cycle/8; i=i+1 )
    {
        char dummy = 0xFF;
        hal_sendSPIBytes(SPI_1, &dummy, 1);
    }
#endif
}

/*
 * Function:       SendByte
 * Arguments:      byte_value, data transfer to flash
 *                 transfer_type, select different type of I/O mode.
 *                 Seven mode:
 *                 SIO, single IO
 *                 DIO, dual IO
 *                 QIO, quad IO
 *                 PIO, parallel
 *                 DTSIO, double transfer rate SIO
 *                 DTDIO, double transfer rate DIO
 *                 DTQIO, double transfer rate QIO
 * Description:    Send one byte data to flash
 * Return Message: None.
 */
void SendByte( unsigned char byte_value, unsigned char transfer_type )
{
    hal_sendSPIBytes(SPI_1, &byte_value, 1);
}
/*
 * Function:       GetByte
 * Arguments:      byte_value, data receive from flash
 *                 transfer_type, select different type of I/O mode.
 *                 Seven mode:
 *                 SIO, single IO
 *                 DIO, dual IO
 *                 QIO, quad IO
 *                 PIO, parallel IO
 *                 DTSIO, double transfer rate SIO
 *                 DTDIO, double transfer rate DIO
 *                 DTQIO, double transfer rate QIO
 * Description:    Get one byte data to flash
 * Return Message: 8 bit data
 */
unsigned char GetByte( unsigned char transfer_type )
{
    char ret;

    hal_readSPIBytes(SPI_1, &ret, 1);

    return ret;
}

/*
 * Function:       WaitFlashReady
 * Arguments:      ExpectTime, expected time-out value of flash operations.
 *                 No use at non-synchronous IO mode.
 * Description:    Synchronous IO:
 *                 If flash is ready return TRUE.
 *                 If flash is time-out return FALSE.
 *                 Non-synchronous IO:
 *                 Always return TRUE
 * Return Message: TRUE, FALSE
 */
unsigned char WaitFlashReady( unsigned int ExpectTime )
{
#ifndef NON_SYNCHRONOUS_IO
    unsigned int temp = 0;
    while( IsFlashBusy() )
    {
        if( temp > ExpectTime )
        {
            return FALSE;
        }
        temp = temp + 1;
    }
       return TRUE;
#else
    return TRUE;
#endif
}

/*
 * Function:       WaitRYBYReady
 * Arguments:      ExpectTime, expected time-out value of flash operations.
 *                 No use at non-synchronous IO mode.
 * Description:    Synchronous IO:
 *                 If flash is ready return TRUE.
 *                 If flash is time-out return FALSE.
 *                 Non-synchronous IO:
 *                 Always return TRUE
 * Return Message: TRUE, FALSE
 */
unsigned char WaitRYBYReady( unsigned int ExpectTime )
{
#ifndef NON_SYNCHRONOUS_IO
    unsigned int temp = 0;
#ifdef GPIO_SPI
    while( SO == 0 )
#else
    // Insert your code for waiting RYBY (SO) pin ready
#endif
    {
        if( temp > ExpectTime )
        {
            return FALSE;
        }
        temp = temp + 1;
    }
    return TRUE;

#else
    return TRUE;
#endif
}

/*
 * Function:       IsFlashBusy
 * Arguments:      None.
 * Description:    Check status register WIP bit.
 *                 If  WIP bit = 1: return TRUE ( Busy )
 *                             = 0: return FALSE ( Ready ).
 * Return Message: TRUE, FALSE
 */
unsigned char IsFlashBusy( void )
{
    unsigned char  gDataBuffer;

    CMD_RDSR( &gDataBuffer );
    if( (gDataBuffer & FLASH_WIP_MASK)  == FLASH_WIP_MASK )
        return TRUE;
    else
        return FALSE;
}

/*
 * Function:       IsFlashQIO
 * Arguments:      None.
 * Description:    If flash QE bit = 1: return TRUE
 *                                 = 0: return FALSE.
 * Return Message: TRUE, FALSE
 */
unsigned char IsFlashQIO( void )
{
#ifdef FLASH_NO_QE_BIT
    return TRUE;
#else
    unsigned char  gDataBuffer;
    CMD_RDSR( &gDataBuffer );
    if( (gDataBuffer & FLASH_QE_MASK) == FLASH_QE_MASK )
        return TRUE;
    else
        return FALSE;
#endif
}
/*
 * Function:       IsFlash4Byte
 * Arguments:      None
 * Description:    Check flash address is 3-byte or 4-byte.
 *                 If flash 4BYTE bit = 1: return TRUE
 *                                    = 0: return FALSE.
 * Return Message: TRUE, FALSE
 */
unsigned char IsFlash4Byte( void )
{
#ifdef FLASH_CMD_RDSCUR
    #ifdef FLASH_4BYTE_ONLY
        return TRUE;
    #elif FLASH_3BYTE_ONLY
        return FALSE;
    #else
        unsigned char  gDataBuffer;
        CMD_RDSCUR( &gDataBuffer );
        if( (gDataBuffer & FLASH_4BYTE_MASK) == FLASH_4BYTE_MASK )
            return TRUE;
        else
            return FALSE;
    #endif
#else
    return FALSE;
#endif
}
/*
 * Function:       SendFlashAddr
 * Arguments:      flash_address, 32 bit flash memory address
 *                 io_mode, I/O mode to transfer address
 *                 addr_4byte_mode,
 * Description:    Send flash address with 3-byte or 4-byte mode.
 * Return Message: None
 */
void SendFlashAddr( unsigned int flash_address, unsigned char io_mode, unsigned char addr_4byte_mode )
{
    /* Check flash is 3-byte or 4-byte mode.
       4-byte mode: Send 4-byte address (A31-A0)
       3-byte mode: Send 3-byte address (A23-A0) */
    if( addr_4byte_mode == TRUE ){
        SendByte( (flash_address >> 24), io_mode ); // A31-A24
    }
    /* A23-A0 */
    SendByte( (flash_address >> 16), io_mode );
    SendByte( (flash_address >> 8), io_mode );
    SendByte( (flash_address), io_mode );
}

/*
 * Function:       GetDummyCycle
 * Arguments:      default_cycle, default dummy cycle
 *                 fsptr, pointer of flash status structure
 * Description:    Get dummy cycle for different condition
 *                 default_cycle: Byte3 | Byte2 | Byte1 | Byte0
 *                      DC 1 bit:   x       1       x       0
 *                      DC 2 bit:   11      10      01      00 
 *                 Note: the variable dummy cycle only support
                         in some product. 
 * Return Message: Dummy cycle value
 */
unsigned char GetDummyCycle( unsigned int default_cycle )
{
#ifdef FLASH_CMD_RDCR
    unsigned char gDataBuffer;
    unsigned char dummy_cycle = default_cycle;
    CMD_RDCR( &gDataBuffer );
    #ifdef SUPPORT_CR_DC
        // product support 1-bit dummy cycle configuration
        if( (gDataBuffer & FLASH_DC_MASK) == FLASH_DC_MASK )
            dummy_cycle = default_cycle >> 16;
        else
            dummy_cycle = default_cycle;
    #elif SUPPORT_CR_DC_2bit
        // product support 2-bit dummy cycle configuration
        switch( gDataBuffer & FLASH_DC_2BIT_MASK ){
            case 0x00:
                dummy_cycle = default_cycle;
            break;
            case 0x40:
                dummy_cycle = default_cycle >> 8;
            break;
            case 0x80:
                dummy_cycle = default_cycle >> 16;
            break;
            case 0xC0:
                dummy_cycle = default_cycle >> 24;
            break;
        }
    #else
         // configuration register not support dummy configuration
         dummy_cycle = default_cycle;
    #endif
    return dummy_cycle;
#else
    // default case: return default dummy cycle
    return default_cycle; 
#endif
}



/*
 * ID Command
 */

/*
 * Function:       CMD_RDID
 * Arguments:      Identification, 32 bit buffer to store id
 * Description:    The RDID instruction is to read the manufacturer ID
 *                 of 1-byte and followed by Device ID of 2-byte.
 * Return Message: FlashOperationSuccess
 */
ReturnMsg CMD_RDID( unsigned int *Identification )
{
    unsigned int temp;
    unsigned char  gDataBuffer[3];

    // Chip select go low to start a flash command
    CS_Low();

    // Send command
    SendByte( FLASH_CMD_RDID, SIO );

    // Get manufacturer identification, device identification
    gDataBuffer[0] = GetByte( SIO );
    gDataBuffer[1] = GetByte( SIO );
    gDataBuffer[2] = GetByte( SIO );

    // Chip select go high to end a command
    CS_High();

    // Store identification
    temp =  gDataBuffer[0];
    temp =  (temp << 8) | gDataBuffer[1];
    *Identification =  (temp << 8) | gDataBuffer[2];

    return FlashOperationSuccess;
}

/*
 * Function:       CMD_RES
 * Arguments:      ElectricIdentification, 8 bit buffer to store electric id
 * Description:    The RES instruction is to read the Device
 *                 electric identification of 1-byte.
 * Return Message: FlashOperationSuccess
 */
ReturnMsg CMD_RES( unsigned char *ElectricIdentification )
{

    // Chip select go low to start a flash command
    CS_Low();

    // Send flash command and insert dummy cycle
    SendByte( FLASH_CMD_RES, SIO );
    InsertDummyCycle( 24 );

    // Get electric identification
    *ElectricIdentification = GetByte( SIO );

    // Chip select go high to end a flash command
    CS_High();

    return FlashOperationSuccess;
}

/*
 * Function:       CMD_REMS
 * Arguments:      REMS_Identification, 16 bit buffer to store id
 *                 fsptr, pointer of flash status structure
 * Description:    The REMS instruction is to read the Device
 *                 manufacturer ID and electric ID of 1-byte.
 * Return Message: FlashOperationSuccess
 */
ReturnMsg CMD_REMS( unsigned short *REMS_Identification, FlashStatus *fsptr )
{
    unsigned char  gDataBuffer[2];

    // Chip select go low to start a flash command
    CS_Low();

    // Send flash command and insert dummy cycle ( if need )
    // ArrangeOpt = 0x00 will output the manufacturer's ID first
    //            = 0x01 will output electric ID first
    SendByte( FLASH_CMD_REMS, SIO );
    InsertDummyCycle( 16 );
    SendByte( fsptr->ArrangeOpt, SIO );

    // Get ID
    gDataBuffer[0] = GetByte( SIO );
    gDataBuffer[1] = GetByte( SIO );

    // Store identification informaion
    *REMS_Identification = (gDataBuffer[0] << 8) | gDataBuffer[1];

    // Chip select go high to end a flash command
    CS_High();

    return FlashOperationSuccess;
}


/*
 * Register  Command
 */

/*
 * Function:       CMD_RDSR
 * Arguments:      StatusReg, 8 bit buffer to store status register value
 * Description:    The RDSR instruction is for reading Status Register Bits.
 * Return Message: FlashOperationSuccess
 */
ReturnMsg CMD_RDSR( unsigned char *StatusReg )
{
    unsigned char  gDataBuffer;

    // Chip select go low to start a flash command
    CS_Low();

    // Send command
    SendByte( FLASH_CMD_RDSR, SIO );
    gDataBuffer = GetByte( SIO );

    // Chip select go high to end a flash command
    CS_High();

    *StatusReg = gDataBuffer;

    return FlashOperationSuccess;
}

/*
 * Function:       CMD_WRSR
 * Arguments:      UpdateValue, 8/16 bit status register value to updata
 * Description:    The WRSR instruction is for changing the values of
 *                 Status Register Bits (and configuration register)
 * Return Message: FlashIsBusy, FlashTimeOut, FlashOperationSuccess
 */
#ifdef SUPPORT_WRSR_CR
ReturnMsg CMD_WRSR( unsigned short UpdateValue )
#else
ReturnMsg CMD_WRSR( unsigned char UpdateValue )
#endif
{
    // Check flash is busy or not
    if( IsFlashBusy() )    return FlashIsBusy;

    // Setting Write Enable Latch bit
    CMD_WREN();

    // Chip select go low to start a flash command
    CS_Low();

    // Send command and update value
    SendByte( FLASH_CMD_WRSR, SIO );
    SendByte( UpdateValue, SIO );
#ifdef SUPPORT_WRSR_CR
    SendByte( UpdateValue >> 8, SIO );    // write configuration register
#endif

    // Chip select go high to end a flash command
    CS_High();


    if( WaitFlashReady( WriteStatusRegCycleTime ) )
        return FlashOperationSuccess;
    else
        return FlashTimeOut;

}


/*
 * Read Command
 */

/*
 * Function:       CMD_READ
 * Arguments:      flash_address, 32 bit flash memory address
 *                 target_address, buffer address to store returned data
 *                 byte_length, length of returned data in byte unit
 * Description:    The READ instruction is for reading data out.
 * Return Message: FlashAddressInvalid, FlashOperationSuccess
 */
ReturnMsg CMD_READ( unsigned int flash_address, unsigned char *target_address, unsigned int byte_length )
{
    unsigned int index;
    unsigned char  addr_4byte_mode;

    // Check flash address
    if( flash_address > FlashSize ) return FlashAddressInvalid;

    // Check 3-byte or 4-byte mode
    if( IsFlash4Byte() )
        addr_4byte_mode = TRUE;  // 4-byte mode
    else
        addr_4byte_mode = FALSE; // 3-byte mode

    // Chip select go low to start a flash command
    CS_Low();

    // Write READ command and address
    SendByte( FLASH_CMD_READ, SIO );
    SendFlashAddr( flash_address, SIO, addr_4byte_mode );

    // Set a loop to read data into buffer
    for( index=0; index < byte_length; index++ )
    {
        // Read data one byte at a time
        *(target_address + index) = GetByte( SIO );
    }

    // Chip select go high to end a flash command
    CS_High();

    return FlashOperationSuccess;
}


/*
 * Function:       CMD_DREAD
 * Arguments:      flash_address, 32 bit flash memory address
 *                 target_address, buffer address to store returned data
 *                 byte_length, length of returned data in byte unit
 * Description:    The DREAD instruction enable double throughput of Serial
 *                 Flash in read mode
 * Return Message: FlashAddressInvalid, FlashOperationSuccess
 */
ReturnMsg CMD_DREAD( unsigned int flash_address, unsigned char *target_address, unsigned int byte_length )
{
    unsigned int index;
    unsigned char  addr_4byte_mode;
    unsigned char  dc;

    // Check flash address
    if( flash_address > FlashSize ) return FlashAddressInvalid;

    // Check 3-byte or 4-byte mode
    if( IsFlash4Byte() )
        addr_4byte_mode = TRUE;    // 4-byte mode
    else
        addr_4byte_mode = FALSE;   // 3-byte mode

    // get dummy cycle number
    dc = GetDummyCycle( DUMMY_CONF_DREAD );

    // Chip select go low to start a flash command
    CS_Low();

    // Write 2-I/O Read command and address
    SendByte( FLASH_CMD_DREAD, SIO );
    SendFlashAddr( flash_address, SIO, addr_4byte_mode );
    InsertDummyCycle( dc );                    // Wait 8 dummy cycle

    // Set a loop to read data into data buffer
    for( index=0; index < byte_length; index++ )
    {
        *(target_address + index) = GetByte( DIO );
    }

    // Chip select go high to end a flash command
    CS_High();

    return FlashOperationSuccess;
}


/*
 * Function:       CMD_FASTREAD
 * Arguments:      flash_address, 32 bit flash memory address
 *                 target_address, buffer address to store returned data
 *                 byte_length, length of returned data in byte unit
 * Description:    The FASTREAD instruction is for quickly reading data out.
 * Return Message: FlashAddressInvalid, FlashOperationSuccess
 */
ReturnMsg CMD_FASTREAD( unsigned int flash_address, unsigned char *target_address, unsigned int byte_length )
{
    unsigned int index;
    unsigned char  addr_4byte_mode;
    unsigned char  dc;

    // Check flash address
    if( flash_address > FlashSize ) return FlashAddressInvalid;

    // Check 3-byte or 4-byte mode
    if( IsFlash4Byte() )
        addr_4byte_mode = TRUE;  // 4-byte mode
    else
        addr_4byte_mode = FALSE; // 3-byte mode

    dc = GetDummyCycle( DUMMY_CONF_FASTREAD ); 

    // Chip select go low to start a flash command
    CS_Low();

    // Write Fast Read command, address and dummy cycle
    SendByte( FLASH_CMD_FASTREAD, SIO );
    SendFlashAddr( flash_address, SIO, addr_4byte_mode );
    InsertDummyCycle ( dc );          // Wait dummy cycle

    // Set a loop to read data into data buffer
    for( index=0; index < byte_length; index++ )
    {
        *(target_address + index) = GetByte( SIO );
    }

    // Chip select go high to end a flash command
    CS_High();

    return FlashOperationSuccess;
}


/*
 * Function:       CMD_RDSFDP
 * Arguments:      flash_address, 32 bit flash memory address
 *                 target_address, buffer address to store returned data
 *                 byte_length, length of returned data in byte unit
 * Description:    RDSFDP can retrieve the operating characteristics, structure
 *                 and vendor-specified information such as identifying information,
 *                 memory size, operating voltages and timinginformation of device
 * Return Message: FlashAddressInvalid, FlashOperationSuccess
 */
ReturnMsg CMD_RDSFDP( unsigned int flash_address, unsigned char *target_address, unsigned int byte_length )
{
    unsigned int index;
    unsigned char  addr_4byte_mode;

    // Check flash address
    if( flash_address > FlashSize ) return FlashAddressInvalid;

    // Check 3-byte or 4-byte mode
    if( IsFlash4Byte() )
        addr_4byte_mode = TRUE;  // 4-byte mode
    else
        addr_4byte_mode = FALSE; // 3-byte mode

    // Chip select go low to start a flash command
    CS_Low();

    // Write Read SFDP command
    SendByte( FLASH_CMD_RDSFDP, SIO );
    SendFlashAddr( flash_address, SIO, addr_4byte_mode );
    InsertDummyCycle ( 8 );        // Insert dummy cycle

    // Set a loop to read data into data buffer
    for( index=0; index < byte_length; index++ )
    {
        *(target_address + index) = GetByte( SIO );
    }

    // Chip select go high to end a flash command
    CS_High();

    return FlashOperationSuccess;
}
/*
 * Program Command
 */

/*
 * Function:       CMD_WREN
 * Arguments:      None.
 * Description:    The WREN instruction is for setting
 *                 Write Enable Latch (WEL) bit.
 * Return Message: FlashOperationSuccess
 */
ReturnMsg CMD_WREN( void )
{
    // Chip select go low to start a flash command
    CS_Low();

    // Write Enable command = 0x06, Setting Write Enable Latch Bit
    SendByte( FLASH_CMD_WREN, SIO );

    // Chip select go high to end a flash command
    CS_High();

    return FlashOperationSuccess;
}

/*
 * Function:       CMD_WRDI
 * Arguments:      None.
 * Description:    The WRDI instruction is to reset
 *                 Write Enable Latch (WEL) bit.
 * Return Message: FlashOperationSuccess
 */
ReturnMsg CMD_WRDI( void )
{
    // Chip select go low to start a flash command
    CS_Low();

    // Write Disable command = 0x04, resets Write Enable Latch Bit
    SendByte( FLASH_CMD_WRDI, SIO );

    CS_High();

    return FlashOperationSuccess;
}


/*
 * Function:       CMD_PP
 * Arguments:      flash_address, 32 bit flash memory address
 *                 source_address, buffer address of source data to program
 *                 byte_length, byte length of data to programm
 * Description:    The PP instruction is for programming
 *                 the memory to be "0".
 *                 The device only accept the last 256 byte ( or 32 byte ) to program.
 *                 If the page address ( flash_address[7:0] ) reach 0xFF, it will
 *                 program next at 0x00 of the same page.
 *                 Some products have smaller page size ( 32 byte )
 * Return Message: FlashAddressInvalid, FlashIsBusy, FlashOperationSuccess,
 *                 FlashTimeOut
 */
ReturnMsg CMD_PP( unsigned int flash_address, unsigned char *source_address, unsigned int byte_length )
{
    unsigned int index;
    unsigned char  addr_4byte_mode;

    // Check flash address
    if( flash_address > FlashSize ) return FlashAddressInvalid;

    // Check flash is busy or not
    if( IsFlashBusy() )    return FlashIsBusy;

    // Check 3-byte or 4-byte mode
    if( IsFlash4Byte() )
        addr_4byte_mode = TRUE;  // 4-byte mode
    else
        addr_4byte_mode = FALSE; // 3-byte mode

    // Setting Write Enable Latch bit
    CMD_WREN();

    // Chip select go low to start a flash command
    CS_Low();

    // Write Page Program command
    SendByte( FLASH_CMD_PP, SIO );
    SendFlashAddr( flash_address, SIO, addr_4byte_mode );

    // Set a loop to down load whole page data into flash's buffer
    // Note: only last 256 byte ( or 32 byte ) will be programmed
    for( index=0; index < byte_length; index++ )
    {
        SendByte( *(source_address + index), SIO );
    }

    // Chip select go high to end a flash command
    CS_High();

    if( WaitFlashReady( PageProgramCycleTime ) )
        return FlashOperationSuccess;
    else
        return FlashTimeOut;
}


/*
 * Erase Command
 */

/*
 * Function:       CMD_SE
 * Arguments:      flash_address, 32 bit flash memory address
 * Description:    The SE instruction is for erasing the data
 *                 of the chosen sector (4KB) to be "1".
 * Return Message: FlashAddressInvalid, FlashIsBusy, FlashOperationSuccess,
 *                 FlashTimeOut
 */
ReturnMsg CMD_SE( unsigned int flash_address )
{
    unsigned char  addr_4byte_mode;

    // Check flash address
    if( flash_address > FlashSize ) return FlashAddressInvalid;

    // Check flash is busy or not
    if(IsFlashBusy())    return FlashIsBusy;

    // Check 3-byte or 4-byte mode
    if( IsFlash4Byte() )
        addr_4byte_mode = TRUE;  // 4-byte mode
    else
        addr_4byte_mode = FALSE; // 3-byte mode

    // Setting Write Enable Latch bit
    CMD_WREN();

    // Chip select go low to start a flash command
    CS_Low();

    //Write Sector Erase command = 0x20;
    SendByte( FLASH_CMD_SE, SIO );
    SendFlashAddr( flash_address, SIO, addr_4byte_mode );

    // Chip select go high to end a flash command
    CS_High();

    if( WaitFlashReady( SectorEraseCycleTime ) )
        return FlashOperationSuccess;
    else
        return FlashTimeOut;
}


/*
 * Function:       CMD_BE
 * Arguments:      flash_address, 32 bit flash memory address
 * Description:    The BE instruction is for erasing the data
 *                 of the chosen sector (64KB) to be "1".
 * Return Message: FlashAddressInvalid, FlashIsBusy, FlashOperationSuccess,
 *                 FlashTimeOut
 */
ReturnMsg CMD_BE( unsigned int flash_address )
{
    unsigned char  addr_4byte_mode;

    // Check flash address
    if( flash_address > FlashSize ) return FlashAddressInvalid;

    // Check flash is busy or not
    if( IsFlashBusy() )    return FlashIsBusy;

    // Check 3-byte or 4-byte mode
    if( IsFlash4Byte() )
        addr_4byte_mode = TRUE;  // 4-byte mode
    else
        addr_4byte_mode = FALSE; // 3-byte mode

    // Setting Write Enable Latch bit
    CMD_WREN();

    // Chip select go low to start a flash command
    CS_Low();

    //Write Block Erase command = 0xD8;
    SendByte( FLASH_CMD_BE, SIO );
    SendFlashAddr( flash_address, SIO, addr_4byte_mode );

    // Chip select go high to end a flash command
    CS_High();

    if( WaitFlashReady( BlockEraseCycleTime ) )
        return FlashOperationSuccess;
    else
        return FlashTimeOut;
}

/*
 * Function:       CMD_CE
 * Arguments:      None.
 * Description:    The CE instruction is for erasing the data
 *                 of the whole chip to be "1".
 * Return Message: FlashIsBusy, FlashOperationSuccess, FlashTimeOut
 */
ReturnMsg CMD_CE( void )
{
    // Check flash is busy or not
    if( IsFlashBusy() )    return FlashIsBusy;

    // Setting Write Enable Latch bit
    CMD_WREN();

    // Chip select go low to start a flash command
    CS_Low();

    //Write Chip Erase command = 0x60;
    SendByte( FLASH_CMD_CE, SIO );

    // Chip select go high to end a flash command
    CS_High();

    if( WaitFlashReady( ChipEraseCycleTime ) )
        return FlashOperationSuccess;
    else
        return FlashTimeOut;
}


/*
 * Mode setting Command
 */

/*
 * Function:       CMD_DP
 * Arguments:      None.
 * Description:    The DP instruction is for setting the
 *                 device on the minimizing the power consumption.
 * Return Message: FlashOperationSuccess
 */
ReturnMsg CMD_DP( void )
{
    // Chip select go low to start a flash command
    CS_Low();

    // Deep Power Down Mode command
    SendByte( FLASH_CMD_DP, SIO );

    // Chip select go high to end a flash command
    CS_High();

    return FlashOperationSuccess;
}

/*
 * Function:       CMD_RDP
 * Arguments:      None.
 * Description:    The Release from RDP instruction is
 *                 putting the device in the Stand-by Power mode.
 * Return Message: FlashOperationSuccess
 */
ReturnMsg CMD_RDP( void )
{
    // Chip select go low to start a flash command
    CS_Low();

    // Deep Power Down Mode command
    SendByte( FLASH_CMD_RDP, SIO );

    // Chip select go high to end a flash command
    CS_High();

    return FlashOperationSuccess;
}

