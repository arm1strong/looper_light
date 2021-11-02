#include "stm32f4xx_hal.h"
#include "hal_spi.h"
#include "hal_gpio.h"

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

/**
 * 初始化SPI
 * 
 */
void hal_initSPI(SPIn spi, POLARITY polarity, PHASE phase)
{
    if(spi==SPI_1)
    {
    	  hspi1.Instance = SPI1;
    	  hspi1.Init.Mode = SPI_MODE_MASTER;
    	  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    	  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    	  hspi1.Init.CLKPolarity = polarity;
    	  hspi1.Init.CLKPhase = phase;
    	  hspi1.Init.NSS = SPI_NSS_SOFT;
    	  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    	  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    	  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    	  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    	  hspi1.Init.CRCPolynomial = 10;
    	  if (HAL_SPI_Init(&hspi1) != HAL_OK)
    	  {
    	    Error_Handler();
    	  }
    }else if(spi==SPI_2)
    {
    	  hspi2.Instance = SPI2;
    	  hspi2.Init.Mode = SPI_MODE_MASTER;
    	  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    	  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    	  hspi2.Init.CLKPolarity = polarity;
    	  hspi2.Init.CLKPhase = phase;
    	  hspi2.Init.NSS = SPI_NSS_SOFT;
    	  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    	  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    	  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    	  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    	  hspi2.Init.CRCPolynomial = 10;
    	  if (HAL_SPI_Init(&hspi2) != HAL_OK)
    	  {
    	    Error_Handler();
    	  }
    }else
    {

    }
}

/**
 * 发送SPI数据
 * 
 */
void hal_sendSPIBytes(SPIn spi, unsigned char *buf, unsigned short size)
{
    SPI_HandleTypeDef * hspi;

    if(spi==SPI_1)
    {
        hspi = &hspi1;
    }else if(spi==SPI_2)
    {
        hspi = &hspi2;
    }else 
    {
        
    }    

    /* Check if the SPI is already enabled */
    if ((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        /* Enable SPI peripheral */
        __HAL_SPI_ENABLE(hspi);
    }
    HAL_SPI_Transmit(hspi, buf, size, 2000);
}

/**
 * 读取SPI数据
 * 
 */
void hal_readSPIBytes(SPIn spi, unsigned char *buf, unsigned short size)
{
    SPI_HandleTypeDef * hspi;

    if(spi==SPI_1)
    {
        hspi = &hspi1;
    }else if(spi==SPI_2)
    {
        hspi = &hspi2;
    }else 
    {
        
    } 

    /* Check if the SPI is already enabled */
    if ((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        /* Enable SPI peripheral */
        __HAL_SPI_ENABLE(hspi);
    }
    HAL_SPI_Receive(hspi, buf, size, 2000);
}

