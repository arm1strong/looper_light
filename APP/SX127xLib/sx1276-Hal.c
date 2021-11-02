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
 * \file       sx1276-Hal.c
 * \brief      SX1276 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include <stdint.h>
#include <stdbool.h> 
#include "sx1276-Hal.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "sx1276-LoRa.h"

extern SX1278Class * sx1278;

void SX1276InitIo( void )
{
    // sx1278->initSPI(sx1278->spi_nmb);
}

void SX1276SetReset( uint8_t state )
{
    sx1278->setReset(sx1278->spi_nmb, state);
}

void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}

void SX1276Read( uint8_t addr, uint8_t *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}

void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    sx1278->setSPINSS(sx1278->spi_nmb, 0);

    addr |= 0x80;

    sx1278->writeSPIData(sx1278->spi_nmb, &addr, 1);
    sx1278->writeSPIData(sx1278->spi_nmb, buffer, size);

    sx1278->setSPINSS(sx1278->spi_nmb, 1);
}


void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    sx1278->setSPINSS(sx1278->spi_nmb, 0);

    addr &= 0x7F;

    sx1278->writeSPIData(sx1278->spi_nmb, &addr, 1);
    sx1278->readSPIData(sx1278->spi_nmb, buffer, size);

    sx1278->setSPINSS(sx1278->spi_nmb, 1);
}

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}

uint8_t SX1276ReadDio0( void )
{
    uint8_t ret;

    ret = sx1278->readDIO0(sx1278->spi_nmb);

    return ret;
}

uint8_t SX1276ReadDio1( void )
{
    return 0;
}

uint8_t SX1276ReadDio2( void )
{
    return 0;
}

uint8_t SX1276ReadDio3( void )
{
    return 0;
}

uint8_t SX1276ReadDio4( void )
{
    return 0;
}

uint8_t SX1276ReadDio5( void )
{
    return 0;
}

void SX1276WriteRxTx( uint8_t txEnable )
{

}

