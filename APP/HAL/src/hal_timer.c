#include "hal_timer.h"
#include "global_config.h"

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

TIM_OC_InitTypeDef sConfigOC = {0};

/**
 * 初始化TIM-CH3/4
 */
void hal_initTimer(void)
{
    static char cfg = 0; //是否初始化过了 0 未 1 是

    if(cfg==1)
    {
        return ;
    }

    /*定时器1 初始化*/
    /* USER CODE END TIM1_Init 1 */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 168;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000000/PWM_FREQ;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if(HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
        Error_Handler();
    }

    /* Start channel 1 */
    if(HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    /* Start channel 2 */
    if(HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    /* Start channel 3 */
    if(HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    /* Start channel 4 */
    if(HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    /*定时器2 初始化*/
    /* USER CODE END TIM1_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 84;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000000/PWM_FREQ;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.RepetitionCounter = 0;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if(HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;


    if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    /* Start channel 2 */
    if(HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    /*定时器4 初始化*/
    /* USER CODE END TIM1_Init 1 */
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 84;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 1000000/PWM_FREQ;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.RepetitionCounter = 0;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if(HAL_TIM_PWM_Init(&htim4) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_OC_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_OC_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_OC_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }

    /* Start channel 1 */
    if(HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    /* Start channel 2 */
    if(HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    /* Start channel 3 */
    if(HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    HAL_TIM_PWM_IO_MspInit(&htim1);
    HAL_TIM_PWM_IO_MspInit(&htim2);
    HAL_TIM_PWM_IO_MspInit(&htim4);

    cfg = 1;
}

/**
 * 设置占空比 per 0~100
 */
void hal_setPulse(PWMn pwm, uint8_t per)
{
    if(per>100)
    {
        per = 100;
    }

    if(pwm==PWM_CH1)
    {
        htim1.Instance->CCR1 = 1000000/PWM_FREQ*per/100;
    }else if(pwm==PWM_CH2)
    {
        htim1.Instance->CCR2 = 1000000/PWM_FREQ*per/100;
    }else if(pwm==PWM_CH3)
    {
        htim1.Instance->CCR3 = 1000000/PWM_FREQ*per/100;
    }else if(pwm==PWM_CH4)
    {
        htim1.Instance->CCR4 = 1000000/PWM_FREQ*per/100;
    }else if(pwm==PWM_CH5)
    {
        htim2.Instance->CCR2 = 1000000/PWM_FREQ*per/100;
    }else if(pwm==PWM_CH6)
    {
        htim4.Instance->CCR1 = 1000000/PWM_FREQ*per/100;
    }else if(pwm==PWM_CH7)
    {
        htim4.Instance->CCR2 = 1000000/PWM_FREQ*per/100;
    }else if(pwm==PWM_CH8)
    {
        htim4.Instance->CCR3 = 1000000/PWM_FREQ*per/100;
    }else  
    {

    }
}
