/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim16;

/* TIM2 init function */
void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef   sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef  sMasterConfig      = {0};
  TIM_OC_InitTypeDef       sConfigOC          = {0};

  htim2.Instance               = TIM2;
  htim2.Init.Prescaler         = 0;
  htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim2.Init.Period            = 100000;
  htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    Error_Handler();

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    Error_Handler();

  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    Error_Handler();

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    Error_Handler();

  sConfigOC.OCMode     = TIM_OCMODE_PWM1;
  sConfigOC.Pulse      = 25000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    Error_Handler();

  HAL_TIM_MspPostInit(&htim2);
}

/* TIM16 init function */
void MX_TIM16_Init(void)
{
  TIM_OC_InitTypeDef            sConfigOC          = {0};
  TIM_BreakDeadTimeConfigTypeDef sBDTConfig        = {0};

  htim16.Instance               = TIM16;
  htim16.Init.Prescaler         = 71;                     // ticker = 72MHz/(71+1)=1MHz
  htim16.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim16.Init.Period            = 499;                    // overflow @1MHz/500 = 2kHz toggle
  htim16.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
    Error_Handler();

  if (HAL_TIM_OC_Init(&htim16) != HAL_OK)
    Error_Handler();

  sConfigOC.OCMode      = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse       = 250;                           // toggle at half-period
  sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode  = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    Error_Handler();

  sBDTConfig.OffStateRunMode  = TIM_OSSR_DISABLE;
  sBDTConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBDTConfig.LockLevel        = TIM_LOCKLEVEL_OFF;
  sBDTConfig.DeadTime         = 0;
  sBDTConfig.BreakState       = TIM_BREAK_DISABLE;
  sBDTConfig.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
  sBDTConfig.BreakFilter      = 0;
  sBDTConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBDTConfig) != HAL_OK)
    Error_Handler();

  HAL_TIM_MspPostInit(&htim16);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_baseHandle)
{
  if (htim_baseHandle->Instance == TIM2)
  {
    __HAL_RCC_TIM2_CLK_ENABLE();
  }
  else if (htim_baseHandle->Instance == TIM16)
  {
    __HAL_RCC_TIM16_CLK_ENABLE();
  }
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (timHandle->Instance == TIM2)
  {
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**TIM2 GPIO Configuration
       PC6     ------> TIM2_CH3
    */
    GPIO_InitStruct.Pin       = GPIO_PIN_6;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }
  else if (timHandle->Instance == TIM16)
  {
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**TIM16 GPIO Configuration
       PD0     ------> TIM16_CH1
    */
    GPIO_InitStruct.Pin       = GPIO_PIN_0;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM16;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if (tim_baseHandle->Instance == TIM2)
  {
    __HAL_RCC_TIM2_CLK_DISABLE();
  }
  else if (tim_baseHandle->Instance == TIM16)
  {
    __HAL_RCC_TIM16_CLK_DISABLE();
  }
}

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */
