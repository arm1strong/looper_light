/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_device.h"

#define DEBUG_IRQ     USART1_IRQHandler
#define CAT1_IRQ      USART2_IRQHandler
#define RS4851_IRQ    USART3_IRQHandler
#define RS4852_IRQ    UART4_IRQHandler
#define BLE_IRQ       USART6_IRQHandler

//系统状态指示灯
extern LED_Descriptor led_sys;
//网络状态灯
extern LED_Descriptor led_net;
//通讯指示灯
extern LED_Descriptor led_data;
//RS485 1
extern RS485_Descriptor rs4851;
//RS485 2
extern RS485_Descriptor rs4852;
//0~10V
extern PWM2IV_Descriptor pwm2iv;
//LoRa 1
extern LoRa_Descriptor lora1;
//Cat1
extern Cat1_Descriptor cat1;
//FLASH 
extern FLASH_Descriptor flash;
//BLE
extern BLE_Descriptor ble;
//Debug
extern Debug_Descriptor debug;


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  NVIC_SystemReset();

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
  NVIC_SystemReset();

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */
  NVIC_SystemReset();

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */
  NVIC_SystemReset();

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */
  
  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

extern uint16_t cpu_usage; //小数点后两位用整数表示 如:523 表示占用率为5.23%
extern uint32_t idle_tick; //空闲任务++
/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();

  if(uwTick%1000==0)
  {
    cpu_usage = (1000-idle_tick) * 100 / 1000;
    idle_tick = 0;
  }
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif /* INCLUDE_xTaskGetSchedulerState */
  xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  }
#endif /* INCLUDE_xTaskGetSchedulerState */
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART1 global interrupt.
  *        用于调试 [接收暂时没用上]
  */
void DEBUG_IRQ(void)
{
    if(__HAL_UART_GET_FLAG((UART_HandleTypeDef *)&huart1, UART_FLAG_RXNE)!=0) //接收单字节
    {

    }else if(__HAL_UART_GET_FLAG((UART_HandleTypeDef *)&huart1, UART_FLAG_IDLE))//接收完成 在此处处理平台数据
    {
    	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
    }else
    {

    }
}

/**
  * @brief This function handles USART2 global interrupt.
  *        用于Cat1接收
  */
void CAT1_IRQ(void)
{
    if(__HAL_UART_GET_FLAG((UART_HandleTypeDef *)&huart2, UART_FLAG_RXNE)!=0) //接收单字节
    {
		cat1.r_buffer[cat1.r_tail++] = huart2.Instance->DR;

		if(cat1.r_tail>=CAT1_MAX_REVSIZE)
		{
			cat1.r_tail = 0;
		}
    }else if(__HAL_UART_GET_FLAG((UART_HandleTypeDef *)&huart2, UART_FLAG_IDLE))//没用上
    {
    	__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		cat1.r_flag = true; //接收完成标志 驱动模块会自行处理
    }else
    {

    }
}

/**
  * @brief This function handles USART2 global interrupt.
  *        用于RS485 1接收
  */
void RS4851_IRQ(void)
{
    if(__HAL_UART_GET_FLAG((UART_HandleTypeDef *)&huart3, UART_FLAG_RXNE)!=0) //接收单字节
    {
		rs4851.r_buffer[rs4851.r_tail++] = huart3.Instance->DR;

		if(rs4851.r_tail>=RS485_1_MAX_REVSIZE)
		{
			rs4851.r_tail = 0;
		}
    }else if(__HAL_UART_GET_FLAG((UART_HandleTypeDef *)&huart3, UART_FLAG_IDLE))//没用上
    {
    	__HAL_UART_CLEAR_IDLEFLAG(&huart3);
		rs4851.r_flag = true;
    }else
    {

    }
}

/**
  * @brief This function handles USART2 global interrupt.
  *        用于RS485 2接收
  */
void RS4852_IRQ(void)
{
    if(__HAL_UART_GET_FLAG((UART_HandleTypeDef *)&huart4, UART_FLAG_RXNE)!=0) //接收单字节
    {
		rs4852.r_buffer[rs4852.r_tail++] = huart4.Instance->DR;

		if(rs4852.r_tail>=RS485_2_MAX_REVSIZE)
		{
			rs4852.r_tail = 0;
		}
    }else if(__HAL_UART_GET_FLAG((UART_HandleTypeDef *)&huart4, UART_FLAG_IDLE))//没用上
    {
    	__HAL_UART_CLEAR_IDLEFLAG(&huart4);
		rs4852.r_flag = true;
    }else
    {

    }
}

/**
  * @brief This function handles USART2 global interrupt.
  *        用于BLE接收
  */
void BLE_IRQ(void)
{
    if(__HAL_UART_GET_FLAG((UART_HandleTypeDef *)&huart6, UART_FLAG_RXNE)!=0) //接收单字节
    {
		ble.r_buffer[ble.r_tail++] = huart6.Instance->DR;

		if(ble.r_tail>=BLE_MAX_REVSIZE)
		{
			ble.r_tail = 0;
		}
    }else if(__HAL_UART_GET_FLAG((UART_HandleTypeDef *)&huart6, UART_FLAG_IDLE))//没用上
    {
    	__HAL_UART_CLEAR_IDLEFLAG(&huart6);
		ble.r_flag = true;
    }else
    {

    }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
