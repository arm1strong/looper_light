#include "hal_wdg.h"

IWDG_HandleTypeDef hiwdg;

void hal_initWdg(void)
{
	  //默认复位时间32S
	  hiwdg.Instance = IWDG;
	  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
	  hiwdg.Init.Reload = 4095;
	  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void hal_feedWdg(void)
{
	HAL_IWDG_Refresh(&hiwdg);
}
