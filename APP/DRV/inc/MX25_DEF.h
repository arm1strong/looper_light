/*
 * COPYRIGHT (c) 2010-2014 MACRONIX INTERNATIONAL CO., LTD
 * SPI Flash Low Level Driver (LLD) Sample Code
 *
 * Flash device information define
 *
 * $Id: MX25_DEF.h,v 1.345 2014/09/18 04:15:16 mxclldb1 Exp $
 */

#ifndef    __MX25_DEF_H__
#define    __MX25_DEF_H__
/*
  Compiler Option
*/

#define    xMCU8051      // use MCU 8051
#define    xGPIO_SPI     // use GPIO connect SPI flash
//#define  Timer_8051 1


/* Select your flash device type */
#define MX25L1006E

#ifdef    MCU8051
#include    <8051.h>
#endif

/* Note:
   Synchronous IO     : MCU will polling WIP bit after
                        sending prgram/erase command
   Non-synchronous IO : MCU can do other operation after
                        sending prgram/erase command
   Default is synchronous IO
*/
//#define    NON_SYNCHRONOUS_IO

/*
  Type and Constant Define
*/

// define type


// variable
#define    TRUE     1
#define    FALSE    0
#define    BYTE_LEN          8
#define    IO_MASK           0x80
#define    HALF_WORD_MASK    0x0000ffff

/*
  Flash Related Parameter Define
*/

#define    Block_Offset       0x10000     // 64K Block size
#define    Block32K_Offset    0x8000      // 32K Block size
#define    Sector_Offset      0x1000      // 4K Sector size
#define    Page_Offset        0x0100      // 256 Byte Page size
#define    Page32_Offset      0x0020      // 32 Byte Page size (some products have smaller page size)
#define    Block_Num          (FlashSize / Block_Offset)

// Flash control register mask define
// status register
#define    FLASH_WIP_MASK         0x01
#define    FLASH_LDSO_MASK        0x02
#define    FLASH_QE_MASK          0x40
// security register
#define    FLASH_OTPLOCK_MASK     0x03
#define    FLASH_4BYTE_MASK       0x04
#define    FLASH_WPSEL_MASK       0x80
// configuration reigster
#define    FLASH_DC_MASK          0x80
#define    FLASH_DC_2BIT_MASK     0xC0
// other
#define    BLOCK_PROTECT_MASK     0xff
#define    BLOCK_LOCK_MASK        0x01



/* Timer Parameter */
//#define  TIMEOUT    1
//#define  TIMENOTOUT 0
//#define  TPERIOD    240             // ns, The time period of timer count one
//#define  COUNT_ONE_MICROSECOND  16  // us, The loop count value within one micro-second


/*
  System Information Define
*/
#ifdef MCU8051

#define    CLK_PERIOD             20     // unit: ns
#define    Min_Cycle_Per_Inst     12     // use 12T 8051
#define    One_Loop_Inst          8      // instruction count of one loop (estimate)

/*GPIO to SPI port mapping ( PORT1 )
 *
 *  GPIO1 bit 7        6     5       4       3       2       1         0
 *  1xI/O     NC     WP#     SO      SI      SCLK    CS#     ErrFlag  OutValid
 *  2xI/O                    SIO1    SIO0
 *  4xI/O     SIO3   SIO2    SIO1    SIO0
 *  8xI/O                    PO7                             PO6
 *
 *(parallel mode)
 *  GPIO3 bit 7        6     5       4       3       2       1         0
 *  8xI/O     nRD    nWR     PO5     PO4     PO3     PO2     PO1      PO0
 */
    #ifdef GPIO_SPI
    #define    SIO3    P1_7
    #define    WPn     P1_6
    #define    SIO2    P1_6
    #define    SO      P1_5
    #define    SIO1    P1_5
    #define    SI      P1_4
    #define    SIO0    P1_4
    #define    SCLK    P1_3
    #define    CSn     P1_2

    #define    SIO7    P3_3
    #define    SIO6    P3_2
    #define    SIO5    P3_1
    #define    SIO4    P3_0


    #define    PO7     P1_5
    #define    PO6     P1_1
    #define    PO5     P3_5
    #define    PO4     P3_4
    #define    PO3     P3_3
    #define    PO2     P3_2
    #define    PO1     P3_1
    #define    PO0     P3_0
    #endif  //end GPIO_SPI

#else
//--- insert your MCU information ---//
#define    CLK_PERIOD                13// unit: ns
#define    Min_Cycle_Per_Inst        1// cycle count of one instruction
#define    One_Loop_Inst             1// instruction count of one loop (estimate)

#endif  //end MCU8051

/*
  Flash ID, Timing Information Define
  (The following information could get from device specification)
*/

#ifdef MX25L1006E
#define    FlashID          0xc22011
#define    ElectronicID     0x10
#define    RESID0           0xc210
#define    RESID1           0x10c2
#define    FlashSize        0x40000        // 128 KB
#define    CE_period        1041668        // tCE /  ( CLK_PERIOD * Min_Cycle_Per_Inst *One_Loop_Inst)
#define    tW               40000000       // 40ms
#define    tDP              10000          // 10us
#define    tPP              3000000        // 3ms
#define    tSE              200000000      // 200ms
#define    tBE              2000000000     // 2s
#define    tPUW             10000000       // 10ms
// Support I/O mode
#define    SIO              0
#define    DIO              1
#endif

// dummy cycle configration
#ifdef DUMMY_CONF_2

#define    DUMMY_CONF_2READ       0x08040804
#define    DUMMY_CONF_4READ       0x0A080406
#define    DUMMY_CONF_FASTREAD    0x08080808
#define    DUMMY_CONF_DREAD       0x08080808
#define    DUMMY_CONF_QREAD       0x08080808

#else

#define    DUMMY_CONF_2READ       0x0A080604
#define    DUMMY_CONF_4READ       0x0A080406
#define    DUMMY_CONF_FASTREAD    0x0A080608
#define    DUMMY_CONF_DREAD       0x0A080608
#define    DUMMY_CONF_QREAD       0x0A080608

#endif

// Flash information define
#define    WriteStatusRegCycleTime     tW / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#define    PageProgramCycleTime        tPP / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#define    SectorEraseCycleTime        tSE / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#define    BlockEraseCycleTime         tBE / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#define    ChipEraseCycleTime          CE_period
#define    FlashFullAccessTime         tPUW / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)

#ifdef tBP
#define    ByteProgramCycleTime        tBP / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#endif
#ifdef tWSR
#define    WriteSecuRegCycleTime       tWSR / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#endif
#ifdef tBE32
#define    BlockErase32KCycleTime      tBE32 / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#endif
#ifdef tWREAW
#define    WriteExtRegCycleTime        tWREAW / (CLK_PERIOD * Min_Cycle_Per_Inst * One_Loop_Inst)
#endif
#endif    /* end of __MX25_DEF_H__  */

