/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */

//控制输出波形的频率
__IO uint16_t period_class = 10;
/* LED亮度等级 PWM表,指数曲线 ，此表使用工程目录下的python脚本index_wave.py生成*/
uint16_t indexWave[] = {
    1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4,
    4, 5, 5, 6, 7, 8, 9, 10, 11, 13,
    15, 17, 19, 22, 25, 28, 32, 36,
    41, 47, 53, 61, 69, 79, 89, 102,
    116, 131, 149, 170, 193, 219, 250,
    284, 323, 367, 417, 474, 539, 613,
    697, 792, 901, 1024, 1024, 901, 792,
    697, 613, 539, 474, 417, 367, 323,
    284, 250, 219, 193, 170, 149, 131,
    116, 102, 89, 79, 69, 61, 53, 47, 41,
    36, 32, 28, 25, 22, 19, 17, 15, 13,
    11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3,
    2, 2, 2, 2, 1, 1, 1, 1

};

//计算PWM表有多少个元素
uint16_t POINT_NUM = sizeof(indexWave) / sizeof(indexWave[0]);
__IO uint32_t rgb_color = 0xFFFFFF;
bool bLed_flash = true;
#define AMPLITUDE_CLASS 256

static uint16_t pwm_index = 0;       //用于PWM查表
static uint16_t period_cnt = 0;      //用于计算周期数
static uint16_t amplitude_cnt = 0;   //用于计算幅值等级
static uint16_t amplitude_cnt_g = 0; //用于计算幅值等级
static uint16_t amplitude_cnt_b = 0; //用于计算幅值等级
/**
  * @brief  Enables or disables the specified TIM interrupts.
  * @param  TIMx: where x can be 1 to 17 to select the TIMx peripheral.
  * @param  TIM_IT: specifies the TIM interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg TIM_IT_Update: TIM update Interrupt source
  *     @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *     @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *     @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *     @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *     @arg TIM_IT_COM: TIM Commutation Interrupt source
  *     @arg TIM_IT_Trigger: TIM Trigger Interrupt source
  *     @arg TIM_IT_Break: TIM Break Interrupt source
  * @note 
  *   - TIM6 and TIM7 can only generate an update interrupt.
  *   - TIM9, TIM12 and TIM15 can have only TIM_IT_Update, TIM_IT_CC1,
  *      TIM_IT_CC2 or TIM_IT_Trigger. 
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have TIM_IT_Update or TIM_IT_CC1.   
  *   - TIM_IT_Break is used only with TIM1, TIM8 and TIM15. 
  *   - TIM_IT_COM is used only with TIM1, TIM8, TIM15, TIM16 and TIM17.    
  * @param  NewState: new state of the TIM interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_ITConfig(TIM_TypeDef *TIMx, uint16_t TIM_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_IT(TIM_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIMx->DIER |= TIM_IT;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIMx->DIER &= (uint16_t)~TIM_IT;
  }
}
/* USER CODE END 0 */

TIM_HandleTypeDef htim3;

/* TIM3 init function */
void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1024;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim3);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle)
{

  if (tim_baseHandle->Instance == TIM3)
  {
    /* USER CODE BEGIN TIM3_MspInit 0 */

    /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
    /* USER CODE BEGIN TIM3_MspInit 1 */

    /* USER CODE END TIM3_MspInit 1 */
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (timHandle->Instance == TIM3)
  {
    /* USER CODE BEGIN TIM3_MspPostInit 0 */

    /* USER CODE END TIM3_MspPostInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration    
    PB0     ------> TIM3_CH3
    PB1     ------> TIM3_CH4
    PB5     ------> TIM3_CH2 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_TIM3_PARTIAL();

    /* USER CODE BEGIN TIM3_MspPostInit 1 */

    TIM_ITConfig(htim3.Instance, TIM_IT_UPDATE, ENABLE);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
    /* USER CODE END TIM3_MspPostInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_baseHandle)
{

  if (tim_baseHandle->Instance == TIM3)
  {
    /* USER CODE BEGIN TIM3_MspDeInit 0 */

    /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
    /* USER CODE BEGIN TIM3_MspDeInit 1 */

    /* USER CODE END TIM3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

static void SetRedPwm(uint16_t pwm)
{
  if (pwm)
    htim3.Instance->CCR2 = pwm;
  else
    htim3.Instance->CCR2 = 0;
}

static void SetGreenPwm(uint16_t pwm)
{
  if (pwm)
    htim3.Instance->CCR3 = pwm;
  else
    htim3.Instance->CCR3 = 0;
}

static void SetBluePwm(uint16_t pwm)
{
  if (pwm)
    htim3.Instance->CCR4 = pwm;
  else
    htim3.Instance->CCR4 = 0;
}

void setLedFlash(uint8_t iLed, bool bOpen)
{
  SetRedPwm(0);
  SetGreenPwm(0);
  SetBluePwm(0);

  if (bOpen)
  {
    if (iLed & 0x01) //Red
      HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    if (iLed & 0x02) //Green
      HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    if (iLed & 0x04) //Blue
      HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

    pwm_index = 0;     //用于PWM查表
    period_cnt = 0;    //用于计算周期数
    amplitude_cnt = 0; //用于计算幅值等级
  }
  else
  {
    if (iLed & 0x01) //Red
    {
      HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
    }
    if (iLed & 0x02) //Green
    {
      HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
    }
    if (iLed & 0x04) //Blue
    {
      HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
    }
  }
}

void setLedColor(uint8_t iled, uint16_t ired, uint16_t igreen, uint16_t iblue)
{
  if (iled & 0x7)
  {
    if (iled & 0x01)
    {
      SetRedPwm(ired);
    }
    else
    {
      SetRedPwm(0);
    }

    if (iled & 0x02)
    {
      SetGreenPwm(igreen);
    }
    else
    {
      SetGreenPwm(0);
    }
    if (iled & 0x04)
    {
      SetBluePwm(iblue);
    }
    else
    {
      SetBluePwm(0);
    }
  }
}

void LED_Timer_Handle(void)
{

  if (__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_UPDATE) != RESET)
  {
    amplitude_cnt++;
    amplitude_cnt_g++;
    amplitude_cnt_b++;
    //每个PWM表中的每个元素有AMPLITUDE_CLASS个等级，
    //每增加一级多输出一次脉冲，即PWM表中的元素多使用一次
    //使用256次，根据RGB颜色分量设置通道输出
    if (amplitude_cnt_b > (AMPLITUDE_CLASS - 1))
    {
      period_cnt++;
      //每个PWM表中的每个元素使用period_class次
      if (period_cnt > period_class)
      {
        pwm_index++; //标志PWM表指向下一个元素

        if (pwm_index >= POINT_NUM) //若PWM表已到达结尾，重新指向表头
        {
          pwm_index = 0;
        }
        period_cnt = 0; //重置周期计数标志
      }
      amplitude_cnt_b = 0; //重置幅值计数标志
    }
    else
    {
      //蓝
//      if ((rgb_color & 0x0000FF) >= amplitude_cnt_b)
//        htim3.Instance->CCR4 = indexWave[pwm_index]; //根据PWM表修改定时器的比较寄存器值
//      else
//        htim3.Instance->CCR4 = 0; //比较寄存器值为0，通道输出高电平，该通道LED灯灭
      //绿
//      if ((rgb_color & 0x00FF00) >= amplitude_cnt_b)
//        htim3.Instance->CCR3 = indexWave[pwm_index]; //根据PWM表修改定时器的比较寄存器值
//      else
//        htim3.Instance->CCR3 = 0; //比较寄存器值为0，通道输出高电平，该通道LED灯灭
      //红
//      if ((rgb_color & 0xFF0000) >= amplitude_cnt_b)
//        htim3.Instance->CCR2 = indexWave[pwm_index]; //根据PWM表修改定时器的比较寄存器值
//      else
//        htim3.Instance->CCR2 = 0; //比较寄存器值为0，通道输出高电平，该通道LED灯灭
    }

    if (__HAL_TIM_GET_IT_SOURCE(&htim3, TIM_IT_UPDATE) != RESET)
    {
      __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
      HAL_TIM_PeriodElapsedCallback(&htim3);
    }
  }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
