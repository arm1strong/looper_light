#ifndef __HAL_SPI_H
#define __HAL_SPI_H

typedef enum
{
    SPI_1, 
    SPI_2
}SPIn;

typedef enum 
{
    POLARITY_LOW  = 0,
    POLARITY_HIGH = 2
}POLARITY;

typedef enum 
{
    PHASE_1EGE  = 0,
    PHASE_2EGE = 1
}PHASE;

void hal_initSPI(SPIn spi, POLARITY polarity, PHASE phase);
void hal_sendSPIBytes(SPIn spi, unsigned char *buf, unsigned short size);
void hal_readSPIBytes(SPIn spi, unsigned char *buf, unsigned short size);

#endif
