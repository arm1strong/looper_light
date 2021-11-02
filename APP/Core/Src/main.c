/********************************************************************************
  * 文件 : main.c
  * 
  * 描述 : 程序入口
  * 
  * 版本 : 
  *       v1
  *          + 实现最初功能  CPU占用率≤1%
  * 
  * 说明 : 
  *       项目方案及通讯协议文档:https://kdocs.cn/l/ca62JvijLn0E
  *       
  *       项目对接文档:https://kdocs.cn/l/cfXPHSzidPcC
  * 
  * 
  * 功能实现进度([√]已完成 [×]未开发 [-]开发未调试):
  *     
  *       [√] RS485 *2 / 内部FLASH / LoRa / Cat1 / LED / PWM转0~10V / DEBUG串口 / 看门狗
  *       [√] BootLoader
  *       [-] 跳传算法的验证
  *       [-] 与平台通讯(获取配置, 上报状态, 下发控制, 在线升级)
  *       [-] 蓝牙串口
  *       [-] 外挂FLASH (暂时用不到) 
  *       [×] APP的对接
  *       [×] 按键 / ETH (暂时用不到) 
  *       [×] 蓝牙的开发
  *       
  * 
  * 硬件版本说明:
  *       V1.0  以下问题已和硬件反馈
  *       [1] PWM转0~10V 并非线性对齐，比如30%占空比对应3.5V，已建议硬件在输出端增加电压反馈，到时可通过ADC进行线性校准
  *       [2] 两路485的GND和A丝印画反
  *       [3] 0~10V生成电路的PWM+应串接一个电阻 建议120R~470R
  *       [4] 光耦1K上拉应降低阻值，建议120R~470R
  *       [5] DC输入部分应修改
  *       [6] 状态指示灯限流电阻由1K->4.7K
  *       [7] 两路485电平转换芯片RO、DI反接
  *       [8] 从可制造性角度分析，建议SIM卡槽移至模组边缘，便于换卡
  *       [9] 以太网电源增强
  */      
#include "main.h"
#include "cmsis_os.h"
#include "app_task.h"
#include "app_device.h"
#include "app_data.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void StartDefaultTask(void *argument);

extern Device device;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  app_initDevice(&device);

  /* Init scheduler */
  osKernelInitialize();

  app_createAllTask();

  /* Start scheduler */
  osKernelStart();

  while (1)
  {
     //不会执行到这里
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

//  SystemCoreClockUpdate();
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
  }
    osDelay(1);
  /* USER CODE END 5 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
