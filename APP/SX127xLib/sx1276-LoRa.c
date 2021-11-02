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
 * \file       sx1276-LoRa.c
 * \brief      SX1276 RF chip driver mode LoRa
 *
 * \version    2.0.0 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include <string.h>
#include "stdbool.h"

//USE_SX1276_RADIO

#include "radio.h"

#include "sx1276-Hal.h"
#include "sx1276.h"

#include "sx1276-LoRaMisc.h"
#include "sx1276-LoRa.h"

#include "hal_spi.h"

/*!
 * Frequency hopping frequencies table
 */
const int32_t HoppingFrequencies[] =
{
    916500000,
    923500000,
    906500000,
    917500000,
    917500000,
    909000000,
    903000000,
    916000000,
    912500000,
    926000000,
    925000000,
    909500000,
    913000000,
    918500000,
    918500000,
    902500000,
    911500000,
    926500000,
    902500000,
    922000000,
    924000000,
    903500000,
    913000000,
    922000000,
    926000000,
    910000000,
    920000000,
    922500000,
    911000000,
    922000000,
    909500000,
    926000000,
    922000000,
    918000000,
    925500000,
    908000000,
    917500000,
    926500000,
    908500000,
    916000000,
    905500000,
    916000000,
    903000000,
    905000000,
    915000000,
    913000000,
    907000000,
    910000000,
    926500000,
    925500000,
    911000000,
};

//将全局变量封装在结构体中 可实现多个LoRa的扩展
SX1278Class *sx1278;

void SX1276LoRaInit( void )
{
    sx1278->RFLRState = RFLR_STATE_IDLE;

    SX1276LoRaSetDefaults( );

    SX1276ReadBuffer( REG_LR_OPMODE, sx1278->SX1276Regs + 1, 0x70 - 1 );
    
    sx1278->SX1276LR->RegLna = RFLR_LNA_GAIN_G1;

    SX1276WriteBuffer( REG_LR_OPMODE, sx1278->SX1276Regs + 1, 0x70 - 1 );

    // set the RF settings 
    SX1276LoRaSetRFFrequency( sx1278->LoRaSettings.RFFrequency );
    SX1276LoRaSetSpreadingFactor( sx1278->LoRaSettings.SpreadingFactor ); // SF6 only operates in implicit header mode.
    SX1276LoRaSetErrorCoding( sx1278->LoRaSettings.ErrorCoding );
    SX1276LoRaSetPacketCrcOn( sx1278->LoRaSettings.CrcOn );
    SX1276LoRaSetSignalBandwidth( sx1278->LoRaSettings.SignalBw );
    SX1276LoRaSetPreambleLength(8);
    SX1276LoRaSetImplicitHeaderOn( sx1278->LoRaSettings.ImplicitHeaderOn );
    SX1276LoRaSetSymbTimeout( 0x3FF );
    SX1276LoRaSetPayloadLength( sx1278->LoRaSettings.PayloadLength );
    SX1276LoRaSetLowDatarateOptimize( false );

    if( sx1278->LoRaSettings.RFFrequency > 860000000 )
    {
        SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_RFO );
        SX1276LoRaSetPa20dBm( false );
        sx1278->LoRaSettings.Power = 14;
        SX1276LoRaSetRFPower( sx1278->LoRaSettings.Power );
    }else
    {
        SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_PABOOST );
        SX1276LoRaSetPa20dBm( true );
        SX1276LoRaSetRFPower( sx1278->LoRaSettings.Power );
    }

    SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER );
}

void SX1276LoRaSetDefaults( void )
{
    // REMARK: See SX1276 datasheet for modified default values.

    SX1276Read( REG_LR_VERSION, &sx1278->SX1276LR->RegVersion );
}

void SX1276LoRaReset( void )
{
    SX1276SetReset( RADIO_RESET_ON );
    
    // Wait 1ms
    uint32_t startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 1 ) );    

    SX1276SetReset( RADIO_RESET_OFF );
    
    // Wait 6ms
    startTick = GET_TICK_COUNT( );
    while( ( GET_TICK_COUNT( ) - startTick ) < TICK_RATE_MS( 6 ) );    
}

void SX1276LoRaSetOpMode( uint8_t opMode )
{
    bool antennaSwitchTxOn = false;

    sx1278->opModePrev = sx1278->SX1276LR->RegOpMode & ~RFLR_OPMODE_MASK;

    if( opMode != sx1278->opModePrev )
    {
        if( opMode == RFLR_OPMODE_TRANSMITTER )
        {
            antennaSwitchTxOn = true;
        }
        else
        {
            antennaSwitchTxOn = false;
        }
        if( antennaSwitchTxOn != sx1278->antennaSwitchTxOnPrev )
        {
            sx1278->antennaSwitchTxOnPrev = antennaSwitchTxOn;
            RXTX( antennaSwitchTxOn ); // Antenna switch control
        }
        sx1278->SX1276LR->RegOpMode = ( sx1278->SX1276LR->RegOpMode & RFLR_OPMODE_MASK ) | opMode;

        SX1276Write( REG_LR_OPMODE, sx1278->SX1276LR->RegOpMode );        
    }
}

uint8_t SX1276LoRaGetOpMode( void )
{
    SX1276Read( REG_LR_OPMODE, &sx1278->SX1276LR->RegOpMode );
    
    return sx1278->SX1276LR->RegOpMode & ~RFLR_OPMODE_MASK;
}

uint8_t SX1276LoRaReadRxGain( void )
{
    SX1276Read( REG_LR_LNA, &sx1278->SX1276LR->RegLna );
    return( sx1278->SX1276LR->RegLna >> 5 ) & 0x07;
}

double SX1276LoRaReadRssi( void )
{
    // Reads the RSSI value
    SX1276Read( REG_LR_RSSIVALUE, &sx1278->SX1276LR->RegRssiValue );

    if( sx1278->LoRaSettings.RFFrequency < 860000000 )  // LF
    {
        return RSSI_OFFSET_LF + ( double )sx1278->SX1276LR->RegRssiValue;
    }
    else
    {
        return RSSI_OFFSET_HF + ( double )sx1278->SX1276LR->RegRssiValue;
    }
}

uint8_t SX1276LoRaGetPacketRxGain( void )
{
    return sx1278->RxGain;
}

int8_t SX1276LoRaGetPacketSnr( void )
{
    return sx1278->RxPacketSnrEstimate;
}

double SX1276LoRaGetPacketRssi( void )
{
    return sx1278->RxPacketRssiValue;
}

void SX1276LoRaStartRx( void )
{
    SX1276LoRaSetRFState( RFLR_STATE_RX_INIT );
}

void SX1276LoRaGetRxPacket( char *buffer, uint16_t *size )
{
    *size = sx1278->RxPacketSize;


    if(sx1278->RxPacketSize>=256)
    {
    	return ;
    }

    memcpy(buffer, sx1278->RFBuffer, sx1278->RxPacketSize);

    sx1278->RxPacketSize = 0;
}

void SX1276LoRaSetTxPacket(char *buffer, uint16_t size )
{
    sx1278->TxPacketSize = size;

    memcpy(sx1278->RFBuffer, buffer, size);
    
    sx1278->RFLRState = RFLR_STATE_TX_INIT;
}

uint8_t SX1276LoRaGetRFState( void )
{
    return sx1278->RFLRState;
}

void SX1276LoRaSetRFState( uint8_t state )
{
    sx1278->RFLRState = state;
}

/*!
 * \brief Process the LoRa modem Rx and Tx state machines depending on the
 *        SX1276 operating mode.
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY, 
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
uint32_t SX1276LoRaProcess(void)
{
    uint32_t result = RF_BUSY;
    
    switch( sx1278->RFLRState )
    {
    case RFLR_STATE_IDLE:
        break;
    case RFLR_STATE_RX_INIT:
        
        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );

        sx1278->SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                    //RFLR_IRQFLAGS_RXDONE |
                                    //RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                    RFLR_IRQFLAGS_VALIDHEADER |
                                    RFLR_IRQFLAGS_TXDONE |
                                    RFLR_IRQFLAGS_CADDONE |
                                    //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                    RFLR_IRQFLAGS_CADDETECTED;
        SX1276Write( REG_LR_IRQFLAGSMASK, sx1278->SX1276LR->RegIrqFlagsMask );

        if( sx1278->LoRaSettings.FreqHopOn == true )
        {
            sx1278->SX1276LR->RegHopPeriod = sx1278->LoRaSettings.HopPeriod;

            SX1276Read( REG_LR_HOPCHANNEL, &sx1278->SX1276LR->RegHopChannel );
            SX1276LoRaSetRFFrequency( HoppingFrequencies[sx1278->SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
        }
        else
        {
            sx1278->SX1276LR->RegHopPeriod = 255;
        }
        
        SX1276Write( REG_LR_HOPPERIOD, sx1278->SX1276LR->RegHopPeriod );
                
                                    // RxDone                    RxTimeout                   FhssChangeChannel           CadDone
        sx1278->SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                    // CadDetected               ModeReady
        sx1278->SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1276WriteBuffer( REG_LR_DIOMAPPING1, &sx1278->SX1276LR->RegDioMapping1, 2 );
    
        if( sx1278->LoRaSettings.RxSingleOn == true ) // Rx single mode
        {
            SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
        }else // Rx continuous mode
        {
            sx1278->SX1276LR->RegFifoAddrPtr = sx1278->SX1276LR->RegFifoRxBaseAddr;
            SX1276Write( REG_LR_FIFOADDRPTR, 0);
            
            SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER );
        }
        
        memset( sx1278->RFBuffer, 0, ( size_t )RF_BUFFER_SIZE );

        sx1278->PacketTimeout = sx1278->LoRaSettings.RxPacketTimeout;
        sx1278->RxTimeoutTimer = GET_TICK_COUNT( );
        sx1278->RFLRState = RFLR_STATE_RX_RUNNING;

//        SX1276ReadBuffer( REG_LR_OPMODE, sx1278->SX1276Regs + 1, 0x70 - 1 ); //debug
        break;
    case RFLR_STATE_RX_RUNNING:
        
        if( DIO0 != 0 ) // RxDone
        {
            sx1278->RxTimeoutTimer = GET_TICK_COUNT( );
            if( sx1278->LoRaSettings.FreqHopOn == true )
            {
                SX1276Read( REG_LR_HOPCHANNEL, &sx1278->SX1276LR->RegHopChannel );
                SX1276LoRaSetRFFrequency( HoppingFrequencies[sx1278->SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
            }
            // Clear Irq
            SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE  );
            sx1278->RFLRState = RFLR_STATE_RX_DONE;
        }
        if( DIO2 != 0 ) // FHSS Changed Channel
        {
            sx1278->RxTimeoutTimer = GET_TICK_COUNT( );
            if( sx1278->LoRaSettings.FreqHopOn == true )
            {
                SX1276Read( REG_LR_HOPCHANNEL, &sx1278->SX1276LR->RegHopChannel );
                SX1276LoRaSetRFFrequency( HoppingFrequencies[sx1278->SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
            }
            // Clear Irq
            SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
            // Debug
            sx1278->RxGain = SX1276LoRaReadRxGain( );
        }

        if( sx1278->LoRaSettings.RxSingleOn == true ) // Rx single mode
        {
            if( ( GET_TICK_COUNT( ) - sx1278->RxTimeoutTimer ) > sx1278->PacketTimeout )
            {
                sx1278->RFLRState = RFLR_STATE_RX_TIMEOUT;
            }
        }
        break;
    case RFLR_STATE_RX_DONE:
        SX1276Read( REG_LR_IRQFLAGS, &sx1278->SX1276LR->RegIrqFlags );
        if( ( sx1278->SX1276LR->RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR ) == RFLR_IRQFLAGS_PAYLOADCRCERROR )
        {
            // Clear Irq
            SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR  );

            if( sx1278->LoRaSettings.RxSingleOn == true ) // Rx single mode
            {
                sx1278->RFLRState = RFLR_STATE_RX_INIT;
            }
            else
            {
                sx1278->RFLRState = RFLR_STATE_RX_RUNNING;
            }
            break;
        }
        
        {
            uint8_t rxSnrEstimate;
            SX1276Read( REG_LR_PKTSNRVALUE, &rxSnrEstimate );
            if( rxSnrEstimate & 0x80 ) // The SNR sign bit is 1
            {
                // Invert and divide by 4
                sx1278->RxPacketSnrEstimate = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2;
                sx1278->RxPacketSnrEstimate = -sx1278->RxPacketSnrEstimate;
            }
            else
            {
                // Divide by 4
                sx1278->RxPacketSnrEstimate = ( rxSnrEstimate & 0xFF ) >> 2;
            }
        }
        
        SX1276Read( REG_LR_PKTRSSIVALUE, &sx1278->SX1276LR->RegPktRssiValue );
    
        if( sx1278->LoRaSettings.RFFrequency < 860000000 )  // LF
        {    
            if( sx1278->RxPacketSnrEstimate < 0 )
            {
                sx1278->RxPacketRssiValue = RSSI_OFFSET_LF + ( ( double )sx1278->SX1276LR->RegPktRssiValue ) + sx1278->RxPacketSnrEstimate;
            }
            else
            {
                sx1278->RxPacketRssiValue = RSSI_OFFSET_LF + ( 1.0666 * ( ( double )sx1278->SX1276LR->RegPktRssiValue ) );
            }
        }
        else                                        // HF
        {    
            if( sx1278->RxPacketSnrEstimate < 0 )
            {
                sx1278->RxPacketRssiValue = RSSI_OFFSET_HF + ( ( double )sx1278->SX1276LR->RegPktRssiValue ) + sx1278->RxPacketSnrEstimate;
            }
            else
            {    
                sx1278->RxPacketRssiValue = RSSI_OFFSET_HF + ( 1.0666 * ( ( double )sx1278->SX1276LR->RegPktRssiValue ) );
            }
        }

        if( sx1278->LoRaSettings.RxSingleOn == true ) // Rx single mode
        {
            sx1278->SX1276LR->RegFifoAddrPtr = sx1278->SX1276LR->RegFifoRxBaseAddr;
            SX1276Write( REG_LR_FIFOADDRPTR, sx1278->SX1276LR->RegFifoAddrPtr );

            if( sx1278->LoRaSettings.ImplicitHeaderOn == true )
            {
                sx1278->RxPacketSize = sx1278->SX1276LR->RegPayloadLength;
                SX1276ReadFifo( sx1278->RFBuffer, sx1278->SX1276LR->RegPayloadLength );
            }
            else
            {
                SX1276Read( REG_LR_NBRXBYTES, &sx1278->SX1276LR->RegNbRxBytes );
                sx1278->RxPacketSize = sx1278->SX1276LR->RegNbRxBytes;
                SX1276ReadFifo( sx1278->RFBuffer, sx1278->SX1276LR->RegNbRxBytes );
            }
        }else // Rx continuous mode
        {
            SX1276Read( REG_LR_FIFORXCURRENTADDR, &sx1278->SX1276LR->RegFifoRxCurrentAddr );

            if( sx1278->LoRaSettings.ImplicitHeaderOn == true )
            {
                sx1278->RxPacketSize = sx1278->SX1276LR->RegPayloadLength;
                sx1278->SX1276LR->RegFifoAddrPtr = sx1278->SX1276LR->RegFifoRxCurrentAddr;
                SX1276Write( REG_LR_FIFOADDRPTR, sx1278->SX1276LR->RegFifoAddrPtr );
                SX1276ReadFifo( sx1278->RFBuffer, sx1278->SX1276LR->RegPayloadLength );
            }else
            {
                SX1276Read( REG_LR_NBRXBYTES, &sx1278->SX1276LR->RegNbRxBytes );
                sx1278->RxPacketSize = sx1278->SX1276LR->RegNbRxBytes;
                sx1278->SX1276LR->RegFifoAddrPtr = sx1278->SX1276LR->RegFifoRxCurrentAddr;  
                // SX1276LR->RegFifoAddrPtr = 0;  //所有buffer都用作接收

                SX1276Write( REG_LR_FIFOADDRPTR, sx1278->SX1276LR->RegFifoAddrPtr );
                SX1276ReadFifo( sx1278->RFBuffer, sx1278->SX1276LR->RegNbRxBytes );

                // SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );  //清除FIFO
            }
        }
        
        if( sx1278->LoRaSettings.RxSingleOn == true ) // Rx single mode
        {
            sx1278->RFLRState = RFLR_STATE_RX_INIT;
        }
        else // Rx continuous mode
        {
            sx1278->RFLRState = RFLR_STATE_RX_RUNNING;
        }
        result = RF_RX_DONE;
        break;
    case RFLR_STATE_RX_TIMEOUT:
        sx1278->RFLRState = RFLR_STATE_RX_INIT;
        result = RF_RX_TIMEOUT;
        break;
    case RFLR_STATE_TX_INIT:

        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );

        if( sx1278->LoRaSettings.FreqHopOn == true )
        {
            sx1278->SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                        RFLR_IRQFLAGS_RXDONE |
                                        RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                        RFLR_IRQFLAGS_VALIDHEADER |
                                        //RFLR_IRQFLAGS_TXDONE |
                                        RFLR_IRQFLAGS_CADDONE |
                                        //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                        RFLR_IRQFLAGS_CADDETECTED;
            sx1278->SX1276LR->RegHopPeriod = sx1278->LoRaSettings.HopPeriod;

            SX1276Read( REG_LR_HOPCHANNEL, &sx1278->SX1276LR->RegHopChannel );
            SX1276LoRaSetRFFrequency( HoppingFrequencies[sx1278->SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
        }else
        {
            sx1278->SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                        RFLR_IRQFLAGS_RXDONE |
                                        RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                        RFLR_IRQFLAGS_VALIDHEADER |
                                        //RFLR_IRQFLAGS_TXDONE |
                                        RFLR_IRQFLAGS_CADDONE |
                                        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                        RFLR_IRQFLAGS_CADDETECTED;
            sx1278->SX1276LR->RegHopPeriod = 0;
        }
        // SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );
        SX1276Write( REG_LR_IRQFLAGSMASK, sx1278->SX1276LR->RegIrqFlagsMask );

        // Initializes the payload size
        sx1278->SX1276LR->RegPayloadLength = sx1278->TxPacketSize;
        SX1276Write( REG_LR_PAYLOADLENGTH, sx1278->SX1276LR->RegPayloadLength );
        
        sx1278->SX1276LR->RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
        SX1276Write( REG_LR_FIFOTXBASEADDR, sx1278->SX1276LR->RegFifoTxBaseAddr );

        sx1278->SX1276LR->RegFifoAddrPtr = sx1278->SX1276LR->RegFifoTxBaseAddr;
        SX1276Write( REG_LR_FIFOADDRPTR, sx1278->SX1276LR->RegFifoAddrPtr );
        
        // Write payload buffer to LORA modem
        SX1276WriteFifo( sx1278->RFBuffer, sx1278->SX1276LR->RegPayloadLength );
                                        // TxDone               RxTimeout                   FhssChangeChannel          ValidHeader         
        sx1278->SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;
                                        // PllLock              Mode Ready
        sx1278->SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_01 | RFLR_DIOMAPPING2_DIO5_00;
        SX1276WriteBuffer( REG_LR_DIOMAPPING1, &sx1278->SX1276LR->RegDioMapping1, 2 );

        SX1276LoRaSetOpMode( RFLR_OPMODE_TRANSMITTER );
        sx1278->RFLRState = RFLR_STATE_TX_RUNNING;
        break;
    case RFLR_STATE_TX_RUNNING:
        if( DIO0 != 0 ) // TxDone
        {
            // Clear Irq
            SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE  );
            sx1278->RFLRState = RFLR_STATE_TX_DONE;
        }
        if( DIO2 == 1 ) // FHSS Changed Channel
        {
            if( sx1278->LoRaSettings.FreqHopOn == true )
            {
                SX1276Read( REG_LR_HOPCHANNEL, &sx1278->SX1276LR->RegHopChannel );
                SX1276LoRaSetRFFrequency( HoppingFrequencies[sx1278->SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
            }
            // Clear Irq
            SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
        }
        break;
    case RFLR_STATE_TX_DONE:
        // optimize the power consumption by switching off the transmitter as soon as the packet has been sent
        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );

        sx1278->RFLRState = RFLR_STATE_IDLE;
        result = RF_TX_DONE;
        break;
    case RFLR_STATE_CAD_INIT:    
        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
    
        sx1278->SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                    RFLR_IRQFLAGS_RXDONE |
                                    RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                    RFLR_IRQFLAGS_VALIDHEADER |
                                    RFLR_IRQFLAGS_TXDONE |
                                    //RFLR_IRQFLAGS_CADDONE |
                                    RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL; // |
                                    //RFLR_IRQFLAGS_CADDETECTED;
        SX1276Write( REG_LR_IRQFLAGSMASK, sx1278->SX1276LR->RegIrqFlagsMask );
           
                                    // RxDone                   RxTimeout                   FhssChangeChannel           CadDone
        sx1278->SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                    // CAD Detected              ModeReady
        sx1278->SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1276WriteBuffer( REG_LR_DIOMAPPING1, &sx1278->SX1276LR->RegDioMapping1, 2 );
            
        SX1276LoRaSetOpMode( RFLR_OPMODE_CAD );
        sx1278->RFLRState = RFLR_STATE_CAD_RUNNING;
        break;
    case RFLR_STATE_CAD_RUNNING:
        if( DIO3 == 1 ) //CAD Done interrupt
        { 
            // Clear Irq
            SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE  );
            if( DIO4 == 1 ) // CAD Detected interrupt
            {
                // Clear Irq
                SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED  );
                // CAD detected, we have a LoRa preamble
                sx1278->RFLRState = RFLR_STATE_RX_INIT;
                result = RF_CHANNEL_ACTIVITY_DETECTED;
            } 
            else
            {    
                // The device goes in Standby Mode automatically    
                sx1278->RFLRState = RFLR_STATE_IDLE;
                result = RF_CHANNEL_EMPTY;
            }
        }   
        break;
    
    default:
        break;
    } 
    return result;
}
