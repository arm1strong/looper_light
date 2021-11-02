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
 * \file       sx1276-LoRaMisc.c
 * \brief      SX1276 RF chip high level functions driver
 *
 * \remark     Optional support functions.
 *             These functions are defined only to easy the change of the
 *             parameters.
 *             For a final firmware the radio parameters will be known so
 *             there is no need to support all possible parameters.
 *             Removing these functions will greatly reduce the final firmware
 *             size.
 *
 * \version    2.0.0 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
//USE_SX1276_RADIO

#include "sx1276-Hal.h"
#include "sx1276.h"

#include "sx1276-LoRa.h"
#include "sx1276-LoRaMisc.h"

#include "stdbool.h"

/*!
 * SX1276 definitions
 */
#define XTAL_FREQ                                   32000000
#define FREQ_STEP                                   61.03515625

//extern tLoRaSettings LoRaSettings;
extern SX1278Class * sx1278;
extern SX1278Class sx1278_m;
extern SX1278Class sx1278_s;

void SX1276LoRaSetRFFrequency( uint32_t freq )
{
	sx1278->LoRaSettings.RFFrequency = freq;

    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
    sx1278->SX1276LR->RegFrfMsb = ( uint8_t )( ( freq >> 16 ) & 0xFF );
    sx1278->SX1276LR->RegFrfMid = ( uint8_t )( ( freq >> 8 ) & 0xFF );
    sx1278->SX1276LR->RegFrfLsb = ( uint8_t )( freq & 0xFF );
    SX1276WriteBuffer( REG_LR_FRFMSB, &sx1278->SX1276LR->RegFrfMsb, 3 );
}

uint32_t SX1276LoRaGetRFFrequency( void )
{
    SX1276ReadBuffer( REG_LR_FRFMSB, &sx1278->SX1276LR->RegFrfMsb, 3 );
    sx1278->LoRaSettings.RFFrequency = ( ( uint32_t )sx1278->SX1276LR->RegFrfMsb << 16 ) | ( ( uint32_t )sx1278->SX1276LR->RegFrfMid << 8 ) | ( ( uint32_t )sx1278->SX1276LR->RegFrfLsb );
    sx1278->LoRaSettings.RFFrequency = ( uint32_t )( ( double )sx1278->LoRaSettings.RFFrequency * ( double )FREQ_STEP );

    return sx1278->LoRaSettings.RFFrequency;
}

void SX1276LoRaSetRFPower( int8_t power )
{
    SX1276Read( REG_LR_PACONFIG, &sx1278->SX1276LR->RegPaConfig );
    SX1276Read( REG_LR_PADAC, &sx1278->SX1276LR->RegPaDac );
    
    if( ( sx1278->SX1276LR->RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
    {
        if( ( sx1278->SX1276LR->RegPaDac & 0x87 ) == 0x87 )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            sx1278->SX1276LR->RegPaConfig = ( sx1278->SX1276LR->RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
            sx1278->SX1276LR->RegPaConfig = ( sx1278->SX1276LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if( power < 2 )
            {
                power = 2;
            }
            if( power > 17 )
            {
                power = 17;
            }
            sx1278->SX1276LR->RegPaConfig = ( sx1278->SX1276LR->RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
            sx1278->SX1276LR->RegPaConfig = ( sx1278->SX1276LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }else
    {
        if( power < -1 )
        {
            power = -1;
        }
        if( power > 14 )
        {
            power = 14;
        }
        sx1278->SX1276LR->RegPaConfig = ( sx1278->SX1276LR->RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
        sx1278->SX1276LR->RegPaConfig = ( sx1278->SX1276LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    SX1276Write( REG_LR_PACONFIG, sx1278->SX1276LR->RegPaConfig );
    sx1278->LoRaSettings.Power = power;
}

int8_t SX1276LoRaGetRFPower( void )
{
    SX1276Read( REG_LR_PACONFIG, &sx1278->SX1276LR->RegPaConfig );
    SX1276Read( REG_LR_PADAC, &sx1278->SX1276LR->RegPaDac );

    if( ( sx1278->SX1276LR->RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
    {
        if( ( sx1278->SX1276LR->RegPaDac & 0x07 ) == 0x07 )
        {
        	sx1278->LoRaSettings.Power = 5 + ( sx1278->SX1276LR->RegPaConfig & ~RFLR_PACONFIG_OUTPUTPOWER_MASK );
        }
        else
        {
        	sx1278->LoRaSettings.Power = 2 + ( sx1278->SX1276LR->RegPaConfig & ~RFLR_PACONFIG_OUTPUTPOWER_MASK );
        }
    }
    else
    {
    	sx1278->LoRaSettings.Power = -1 + ( sx1278->SX1276LR->RegPaConfig & ~RFLR_PACONFIG_OUTPUTPOWER_MASK );
    }
    return sx1278->LoRaSettings.Power;
}

void SX1276LoRaSetSignalBandwidth( uint8_t bw )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &sx1278->SX1276LR->RegModemConfig1 );
    sx1278->SX1276LR->RegModemConfig1 = ( sx1278->SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_BW_MASK ) | ( bw << 4 );
    SX1276Write( REG_LR_MODEMCONFIG1, sx1278->SX1276LR->RegModemConfig1 );
    sx1278->LoRaSettings.SignalBw = bw;
}

uint8_t SX1276LoRaGetSignalBandwidth( void )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &sx1278->SX1276LR->RegModemConfig1 );
    sx1278->LoRaSettings.SignalBw = ( sx1278->SX1276LR->RegModemConfig1 & ~RFLR_MODEMCONFIG1_BW_MASK ) >> 4;
    return sx1278->LoRaSettings.SignalBw;
}

void SX1276LoRaSetSpreadingFactor( uint8_t factor )
{

    if( factor > 12 )
    {
        factor = 12;
    }
    else if( factor < 6 )
    {
        factor = 6;
    }

    if( factor == 6 )
    {
        SX1276LoRaSetNbTrigPeaks( 5 );
    }
    else
    {
        SX1276LoRaSetNbTrigPeaks( 3 );
    }

    SX1276Read( REG_LR_MODEMCONFIG2, &sx1278->SX1276LR->RegModemConfig2 );
    sx1278->SX1276LR->RegModemConfig2 = ( sx1278->SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_SF_MASK ) | ( factor << 4 );
    SX1276Write( REG_LR_MODEMCONFIG2, sx1278->SX1276LR->RegModemConfig2 );
    sx1278->LoRaSettings.SpreadingFactor = factor;
}

uint8_t SX1276LoRaGetSpreadingFactor( void )
{
    SX1276Read( REG_LR_MODEMCONFIG2, &sx1278->SX1276LR->RegModemConfig2 );
    sx1278->LoRaSettings.SpreadingFactor = ( sx1278->SX1276LR->RegModemConfig2 & ~RFLR_MODEMCONFIG2_SF_MASK ) >> 4;
    return sx1278->LoRaSettings.SpreadingFactor;
}

void SX1276LoRaSetErrorCoding( uint8_t value )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &sx1278->SX1276LR->RegModemConfig1 );
    sx1278->SX1276LR->RegModemConfig1 = ( sx1278->SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | ( value << 1 );
    SX1276Write( REG_LR_MODEMCONFIG1, sx1278->SX1276LR->RegModemConfig1 );
    sx1278->LoRaSettings.ErrorCoding = value;
}

uint8_t SX1276LoRaGetErrorCoding( void )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &sx1278->SX1276LR->RegModemConfig1 );
    sx1278->LoRaSettings.ErrorCoding = ( sx1278->SX1276LR->RegModemConfig1 & ~RFLR_MODEMCONFIG1_CODINGRATE_MASK ) >> 1;
    return sx1278->LoRaSettings.ErrorCoding;
}

void SX1276LoRaSetPacketCrcOn( bool enable )
{
    SX1276Read( REG_LR_MODEMCONFIG2, &sx1278->SX1276LR->RegModemConfig2 );
    sx1278->SX1276LR->RegModemConfig2 = ( sx1278->SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) | ( enable << 2 );
    SX1276Write( REG_LR_MODEMCONFIG2, sx1278->SX1276LR->RegModemConfig2 );
    sx1278->LoRaSettings.CrcOn = enable;
}

void SX1276LoRaSetPreambleLength( uint16_t value )
{
    SX1276ReadBuffer( REG_LR_PREAMBLEMSB, &sx1278->SX1276LR->RegPreambleMsb, 2 );

    sx1278->SX1276LR->RegPreambleMsb = ( value >> 8 ) & 0x00FF;
    sx1278->SX1276LR->RegPreambleLsb = value & 0xFF;
    SX1276WriteBuffer( REG_LR_PREAMBLEMSB, &sx1278->SX1276LR->RegPreambleMsb, 2 );
}

uint16_t SX1276LoRaGetPreambleLength( void )
{
    SX1276ReadBuffer( REG_LR_PREAMBLEMSB, &sx1278->SX1276LR->RegPreambleMsb, 2 );
    return ( ( sx1278->SX1276LR->RegPreambleMsb & 0x00FF ) << 8 ) | sx1278->SX1276LR->RegPreambleLsb;
}

bool SX1276LoRaGetPacketCrcOn( void )
{
    SX1276Read( REG_LR_MODEMCONFIG2, &sx1278->SX1276LR->RegModemConfig2 );
    sx1278->LoRaSettings.CrcOn = ( sx1278->SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON ) >> 1;
    return sx1278->LoRaSettings.CrcOn;
}

void SX1276LoRaSetImplicitHeaderOn( bool enable )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &sx1278->SX1276LR->RegModemConfig1 );
    sx1278->SX1276LR->RegModemConfig1 = ( sx1278->SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable );
    SX1276Write( REG_LR_MODEMCONFIG1, sx1278->SX1276LR->RegModemConfig1 );
    sx1278->LoRaSettings.ImplicitHeaderOn = enable;
}

bool SX1276LoRaGetImplicitHeaderOn( void )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &sx1278->SX1276LR->RegModemConfig1 );
    sx1278->LoRaSettings.ImplicitHeaderOn = ( sx1278->SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_IMPLICITHEADER_ON );
    return sx1278->LoRaSettings.ImplicitHeaderOn;
}

void SX1276LoRaSetRxSingleOn( bool enable )
{
	sx1278->LoRaSettings.RxSingleOn = enable;
}

bool SX1276LoRaGetRxSingleOn( void )
{
    return sx1278->LoRaSettings.RxSingleOn;
}

void SX1276LoRaSetFreqHopOn( bool enable )
{
	sx1278->LoRaSettings.FreqHopOn = enable;
}

bool SX1276LoRaGetFreqHopOn( void )
{
    return sx1278->LoRaSettings.FreqHopOn;
}

void SX1276LoRaSetHopPeriod( uint8_t value )
{
	sx1278->SX1276LR->RegHopPeriod = value;
    SX1276Write( REG_LR_HOPPERIOD, sx1278->SX1276LR->RegHopPeriod );
    sx1278->LoRaSettings.HopPeriod = value;
}

uint8_t SX1276LoRaGetHopPeriod( void )
{
    SX1276Read( REG_LR_HOPPERIOD, &sx1278->SX1276LR->RegHopPeriod );
    sx1278->LoRaSettings.HopPeriod = sx1278->SX1276LR->RegHopPeriod;
    return sx1278->LoRaSettings.HopPeriod;
}

void SX1276LoRaSetTxPacketTimeout( uint32_t value )
{
	sx1278->LoRaSettings.TxPacketTimeout = value;
}

uint32_t SX1276LoRaGetTxPacketTimeout( void )
{
    return sx1278->LoRaSettings.TxPacketTimeout;
}

void SX1276LoRaSetRxPacketTimeout( uint32_t value )
{
	sx1278->LoRaSettings.RxPacketTimeout = value;
}

uint32_t SX1276LoRaGetRxPacketTimeout( void )
{
    return sx1278->LoRaSettings.RxPacketTimeout;
}

void SX1276LoRaSetPayloadLength( uint8_t value )
{
	sx1278->SX1276LR->RegPayloadLength = value;
    SX1276Write( REG_LR_PAYLOADLENGTH, sx1278->SX1276LR->RegPayloadLength );
    sx1278->LoRaSettings.PayloadLength = value;
}

uint8_t SX1276LoRaGetPayloadLength( void )
{
    SX1276Read( REG_LR_PAYLOADLENGTH, &sx1278->SX1276LR->RegPayloadLength );
    sx1278->LoRaSettings.PayloadLength = sx1278->SX1276LR->RegPayloadLength;
    return sx1278->LoRaSettings.PayloadLength;
}

void SX1276LoRaSetPa20dBm( bool enale )
{
    SX1276Read( REG_LR_PADAC, &sx1278->SX1276LR->RegPaDac );
    SX1276Read( REG_LR_PACONFIG, &sx1278->SX1276LR->RegPaConfig );

    if( ( sx1278->SX1276LR->RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
    {    
        if( enale == true )
        {
        	sx1278->SX1276LR->RegPaDac = 0x87;
        }
    }
    else
    {
    	sx1278->SX1276LR->RegPaDac = 0x84;
    }
    SX1276Write( REG_LR_PADAC, sx1278->SX1276LR->RegPaDac );
}

bool SX1276LoRaGetPa20dBm( void )
{
    SX1276Read( REG_LR_PADAC, &sx1278->SX1276LR->RegPaDac );
    
    return ( ( sx1278->SX1276LR->RegPaDac & 0x07 ) == 0x07 ) ? true : false;
}

void SX1276LoRaSetPAOutput( uint8_t outputPin )
{
    SX1276Read( REG_LR_PACONFIG, &sx1278->SX1276LR->RegPaConfig );
    sx1278->SX1276LR->RegPaConfig = (sx1278->SX1276LR->RegPaConfig & RFLR_PACONFIG_PASELECT_MASK ) | outputPin;
    SX1276Write( REG_LR_PACONFIG, sx1278->SX1276LR->RegPaConfig );
}

uint8_t SX1276LoRaGetPAOutput( void )
{
    SX1276Read( REG_LR_PACONFIG, &sx1278->SX1276LR->RegPaConfig );
    return sx1278->SX1276LR->RegPaConfig & ~RFLR_PACONFIG_PASELECT_MASK;
}

void SX1276LoRaSetPaRamp( uint8_t value )
{
    SX1276Read( REG_LR_PARAMP, &sx1278->SX1276LR->RegPaRamp );
    sx1278->SX1276LR->RegPaRamp = ( sx1278->SX1276LR->RegPaRamp & RFLR_PARAMP_MASK ) | ( value & ~RFLR_PARAMP_MASK );
    SX1276Write( REG_LR_PARAMP, sx1278->SX1276LR->RegPaRamp );
}

uint8_t SX1276LoRaGetPaRamp( void )
{
    SX1276Read( REG_LR_PARAMP, &sx1278->SX1276LR->RegPaRamp );
    return sx1278->SX1276LR->RegPaRamp & ~RFLR_PARAMP_MASK;
}

void SX1276LoRaSetSymbTimeout( uint16_t value )
{
    SX1276ReadBuffer( REG_LR_MODEMCONFIG2, &sx1278->SX1276LR->RegModemConfig2, 2 );

    sx1278->SX1276LR->RegModemConfig2 = ( sx1278->SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) | ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK );
    sx1278->SX1276LR->RegSymbTimeoutLsb = value & 0xFF;
    SX1276WriteBuffer( REG_LR_MODEMCONFIG2, &sx1278->SX1276LR->RegModemConfig2, 2 );
}

uint16_t SX1276LoRaGetSymbTimeout( void )
{
    SX1276ReadBuffer( REG_LR_MODEMCONFIG2, &sx1278->SX1276LR->RegModemConfig2, 2 );
    return ( ( sx1278->SX1276LR->RegModemConfig2 & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) << 8 ) | sx1278->SX1276LR->RegSymbTimeoutLsb;
}

void SX1276LoRaSetLowDatarateOptimize( bool enable )
{
    SX1276Read( REG_LR_MODEMCONFIG3, &sx1278->SX1276LR->RegModemConfig3 );
    sx1278->SX1276LR->RegModemConfig3 = ( sx1278->SX1276LR->RegModemConfig3 & RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) | ( enable << 3 );
    SX1276Write( REG_LR_MODEMCONFIG3, sx1278->SX1276LR->RegModemConfig3 );
}

bool SX1276LoRaGetLowDatarateOptimize( void )
{
    SX1276Read( REG_LR_MODEMCONFIG3, &sx1278->SX1276LR->RegModemConfig3 );
    return ( ( sx1278->SX1276LR->RegModemConfig3 & RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON ) >> 3 );
}

void SX1276LoRaSetNbTrigPeaks( uint8_t value )
{
    SX1276Read( 0x31, &sx1278->SX1276LR->RegDetectOptimize );
    sx1278->SX1276LR->RegDetectOptimize = ( sx1278->SX1276LR->RegDetectOptimize & 0xF8 ) | value;
    SX1276Write( 0x31, sx1278->SX1276LR->RegDetectOptimize );
}

uint8_t SX1276LoRaGetNbTrigPeaks( void )
{
    SX1276Read( 0x31, &sx1278->SX1276LR->RegDetectOptimize );
    return ( sx1278->SX1276LR->RegDetectOptimize & 0x07 );
}
