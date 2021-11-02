#include "hal_flash.h"

/**
 * 向内部flash编程 size必须为4的整数倍
 * opt ERASE / xERASE
 */
char hal_programFlash(uint32_t addr, char *buffer, uint16_t size)
{
    char ret = FLASH_FAIL;

	HAL_FLASH_Unlock();

    uint16_t i;

    for(i=0;i<size/4;++i)
    {
    	int val = *(uint32_t *)(buffer+i*4);
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr+i*4, *(uint32_t *)(buffer+i*4))!=HAL_OK)
        {
            break;
        }
    }

    if(i==size/4)
    {
        ret = FLASH_SUCCESS;
    }

	HAL_FLASH_Lock();

    return ret;
}

/**
 * 读取内部flash数据 size必须为4字节整数倍
 */
void hal_readFlash(uint32_t addr, char *buffer, uint16_t size)
{
	for(uint16_t i=0;i<size/4;++i)
	{
		*((uint32_t *)buffer+i) = *((__IO uint32_t *)addr+i);
	}
}

/**
 * 擦除指定flash
 *
 */
HAL_StatusTypeDef hal_eraseFlash(uint8_t sector, uint8_t sectornum)
{
	uint32_t flag;

	FLASH_EraseInitTypeDef erase =	{
										  .TypeErase = FLASH_TYPEERASE_SECTORS,
										  .Sector = sector,
										  .NbSectors = sectornum,
										  .VoltageRange = FLASH_VOLTAGE_RANGE_3
									  };

	HAL_FLASH_Unlock();

	HAL_StatusTypeDef ret = HAL_FLASHEx_Erase(&erase, &flag);

	HAL_FLASH_Lock();

	return ret;
}
