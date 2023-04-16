/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "adc.h"

/* USER CODE BEGIN 0 */
#include <string.h>
#include <errno.h>

#include "libmcu/port/adc.h"
#include "libmcu/metrics.h"

#define VOLTAGE_DIVIDER_RATIO	134 /* R1: 360K, R2: 56K */

struct adc {
	ADC_HandleTypeDef *handle;
	bool activated;
};

/* USER CODE END 0 */

ADC_HandleTypeDef hadc2;

/* ADC2 init function */
void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.GainCompensation = 0;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspInit 0 */

  /* USER CODE END ADC2_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
    PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* ADC2 clock enable */
    __HAL_RCC_ADC12_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**ADC2 GPIO Configuration
    PA1     ------> ADC2_IN2
    PC4     ------> ADC2_IN5
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BATMON_LEVEL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BATMON_LEVEL_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC2_MspInit 1 */

  /* USER CODE END ADC2_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspDeInit 0 */

  /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC12_CLK_DISABLE();

    /**ADC2 GPIO Configuration
    PA1     ------> ADC2_IN2
    PC4     ------> ADC2_IN5
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

    HAL_GPIO_DeInit(BATMON_LEVEL_GPIO_Port, BATMON_LEVEL_Pin);

  /* USER CODE BEGIN ADC2_MspDeInit 1 */

  /* USER CODE END ADC2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
int adc_port_convert_to_millivolts(struct adc *self, int value)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	int mv = value * 3300/*ref. in millivoltage*/ / (4096-1)/*12bit*/;
	return mv * 1000 / VOLTAGE_DIVIDER_RATIO;
}

int adc_port_read(struct adc *self, adc_channel_t channel)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	(void)channel;
	return (int)HAL_ADC_GetValue(self->handle);
}

int adc_port_measure(struct adc *self)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	HAL_ADC_Start(self->handle);
	HAL_ADC_PollForConversion(self->handle, HAL_MAX_DELAY);

	return 0;
}

int adc_port_channel_init(struct adc *self, adc_channel_t channel)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	(void)channel;
	return 0;
}

int adc_port_calibrate(struct adc *self)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	if (HAL_ADCEx_Calibration_Start(self->handle, ADC_SINGLE_ENDED)
			!= HAL_OK) {
		metrics_increase(ADCErrorCalibration);
		return -ESTALE;
	}

	return 0;
}

int adc_port_enable(struct adc *self)
{
	if (!self) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	} else if (self->activated) {
		metrics_increase(ADCErrorSequence);
		return -EALREADY;
	}

	MX_ADC2_Init();
	self->activated = true;

	return 0;
}

int adc_port_disable(struct adc *self)
{
	if (!self) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	} else if (!self->activated) {
		metrics_increase(ADCErrorSequence);
		return -EALREADY;
	}

	if (HAL_ADC_DeInit(self->handle) != HAL_OK) {
		metrics_increase(ADCFault);
		return -EFAULT;
	}

	self->activated = false;
	return 0;
}

struct adc *adc_port_create(uint8_t adc_num)
{
	static struct adc adc;

	if (adc_num != 2 || adc.activated) {
		return NULL;
	}

	adc.handle = &hadc2;

	return &adc;
}

int adc_port_delete(struct adc *self)
{
	if (!self) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	} else if (self->activated) {
		adc_port_disable(self);
	}

	memset(self, 0, sizeof(*self));
	return 0;
}

/* USER CODE END 1 */
