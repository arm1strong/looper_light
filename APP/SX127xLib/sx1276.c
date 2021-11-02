/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx1276.c
 * \brief      SX1276 RF chip driver
 *
 * \version    2.0.0 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
//USE_SX1276_RADIO
#include "radio.h"
#include "sx1276.h"

#include "sx1276-Hal.h"
#include "sx1276-Fsk.h"
#include "sx1276-LoRa.h"

#include "stdbool.h"

#include "stm32f4xx_hal.h"

extern SX1278Class * sx1278;

/*!
 * SX1276 registers variable
 */
//uint8_t SX1276Regs[0x70];

//static bool LoRaOn = false;
//static bool LoRaOnState = false;
//extern tSX1276* SX1276;

void SX1276Init( void )
{
    // Initialize FSK and LoRa registers structure
	sx1278->SX1276 = ( tSX1276* )sx1278->SX1276Regs;
	sx1278->SX1276LR = ( tSX1276LR* )sx1278->SX1276Regs;

    SX1276Reset();

    // REMARK: After radio reset the default modem is FSK

#if ( LORA == 0 ) 

    LoRaOn = false;
    SX1276SetLoRaOn( LoRaOn );
    // Initialize FSK modem
    SX1276FskInit( );

#else

    sx1278->LoRaOn = true;
    SX1276SetLoRaOn( sx1278->LoRaOn );
    // Initialize LoRa modem
    SX1276LoRaInit();
    
#endif

}

void SX1276Reset( void )
{
    SX1276SetReset( RADIO_RESET_OFF );
    
    HAL_Delay(10);
    
    SX1276SetReset( RADIO_RESET_ON );
}

void SX1276SetLoRaOn( bool enable )
{
    if( sx1278->LoRaOnState == enable )
    {
        return;
    }
    sx1278->LoRaOnState = enable;
    sx1278->LoRaOn = enable;

    if( sx1278->LoRaOn == true )
    {
        SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );
        
        sx1278->SX1276LR->RegOpMode = ( sx1278->SX1276LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON;
        SX1276Write( REG_LR_OPMODE, sx1278->SX1276LR->RegOpMode );
        
        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
                                        // RxDone               RxTimeout                   FhssChangeChannel           CadDone
        sx1278->SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                        // CadDetected          ModeReady
        sx1278->SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1276WriteBuffer( REG_LR_DIOMAPPING1, &sx1278->SX1276LR->RegDioMapping1, 2 );
        
        SX1276ReadBuffer( REG_LR_OPMODE, sx1278->SX1276Regs + 1, 0x70 - 1 );
    }
    else
    {
        SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );
        
        sx1278->SX1276LR->RegOpMode = ( sx1278->SX1276LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_OFF;
        SX1276Write( REG_LR_OPMODE, sx1278->SX1276LR->RegOpMode );
        
        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
        
        SX1276ReadBuffer( REG_OPMODE, sx1278->SX1276Regs + 1, 0x70 - 1 );
    }
}

bool SX1276GetLoRaOn( void )
{
    return sx1278->LoRaOn;
}

void SX1276SetOpMode( uint8_t opMode )
{
    if( sx1278->LoRaOn == false )
    {
        SX1276FskSetOpMode( opMode );
    }
    else
    {
        SX1276LoRaSetOpMode( opMode );
    }
}

uint8_t SX1276GetOpMode( void )
{
    if( sx1278->LoRaOn == false )
    {
        return SX1276FskGetOpMode( );
    }
    else
    {
        return SX1276LoRaGetOpMode( );
    }
}

double SX1276ReadRssi( void )
{
    if( sx1278->LoRaOn == false )
    {
        return SX1276FskReadRssi( );
    }
    else
    {
        return SX1276LoRaReadRssi( );
    }
}

uint8_t SX1276ReadRxGain( void )
{
    if( sx1278->LoRaOn == false )
    {
        return SX1276FskReadRxGain( );
    }
    else
    {
        return SX1276LoRaReadRxGain( );
    }
}

uint8_t SX1276GetPacketRxGain( void )
{
    if( sx1278->LoRaOn == false )
    {
        return SX1276FskGetPacketRxGain(  );
    }
    else
    {
        return SX1276LoRaGetPacketRxGain(  );
    }
}

int8_t SX1276GetPacketSnr( void )
{
    if( sx1278->LoRaOn == false )
    {
         while( 1 )
         {
             // Useless in FSK mode
             // Block program here
         }
    }
    else
    {
        return SX1276LoRaGetPacketSnr(  );
    }
}

double SX1276GetPacketRssi( void )
{
    if( sx1278->LoRaOn == false )
    {
        return SX1276FskGetPacketRssi(  );
    }
    else
    {
        return SX1276LoRaGetPacketRssi( );
    }
}

uint32_t SX1276GetPacketAfc( void )
{
    if( sx1278->LoRaOn == false )
    {
        return SX1276FskGetPacketAfc(  );
    }
    else
    {
         while( 1 )
         {
             // Useless in LoRa mode
             // Block program here
         }
    }
}

void SX1276StartRx( void )
{
    if( sx1278->LoRaOn == false )
    {
        SX1276FskSetRFState( RF_STATE_RX_INIT );
    }
    else
    {
        SX1276LoRaSetRFState( RFLR_STATE_RX_INIT );
    }
}

void SX1276GetRxPacket( void *buffer, uint16_t *size )
{
    if( sx1278->LoRaOn == false )
    {
//        SX1276FskGetRxPacket( buffer, size );
    }
    else
    {
        SX1276LoRaGetRxPacket( buffer, size );
    }
}

void SX1276SetTxPacket(const void *buffer, uint16_t size)
{
    if(sx1278->LoRaOn == false)
    {
        SX1276FskSetTxPacket(buffer, size);
    }
    else
    {
        SX1276LoRaSetTxPacket(buffer, size);
    }
}

uint8_t SX1276GetRFState( void )
{
    if( sx1278->LoRaOn == false )
    {
        return SX1276FskGetRFState( );
    }
    else
    {
        return SX1276LoRaGetRFState( );
    }
}

void SX1276SetRFState( uint8_t state )
{
    if( sx1278->LoRaOn == false )
    {
        SX1276FskSetRFState( state );
    }
    else
    {
        SX1276LoRaSetRFState( state );
    }
}

uint32_t SX1276Process( void )
{
    if( sx1278->LoRaOn == false )
    {
        return SX1276FskProcess( );
    }
    else
    {
        return SX1276LoRaProcess( );
    }
}

