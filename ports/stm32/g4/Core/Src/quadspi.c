/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.c
  * @brief   This file provides code for the configuration
  *          of the QUADSPI instances.
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
#include "quadspi.h"

/* USER CODE BEGIN 0 */
#include <errno.h>

/* USER CODE END 0 */

QSPI_HandleTypeDef hqspi1;

/* QUADSPI1 init function */
void MX_QUADSPI1_Init(void)
{

  /* USER CODE BEGIN QUADSPI1_Init 0 */

  /* USER CODE END QUADSPI1_Init 0 */

  /* USER CODE BEGIN QUADSPI1_Init 1 */

  /* USER CODE END QUADSPI1_Init 1 */
  hqspi1.Instance = QUADSPI;
  hqspi1.Init.ClockPrescaler = 3;
  hqspi1.Init.FifoThreshold = 1;
  hqspi1.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi1.Init.FlashSize = 20;
  hqspi1.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi1.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi1.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi1.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI1_Init 2 */

  /* USER CODE END QUADSPI1_Init 2 */

}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* qspiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_QSPI;
    PeriphClkInit.QspiClockSelection = RCC_QSPICLKSOURCE_SYSCLK;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**QUADSPI1 GPIO Configuration
    PA6     ------> QUADSPI1_BK1_IO3
    PA7     ------> QUADSPI1_BK1_IO2
    PB0     ------> QUADSPI1_BK1_IO1
    PB1     ------> QUADSPI1_BK1_IO0
    PB10     ------> QUADSPI1_CLK
    PB11     ------> QUADSPI1_BK1_NCS
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* QUADSPI interrupt Init */
    HAL_NVIC_SetPriority(QUADSPI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }
}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* qspiHandle)
{

  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI1 GPIO Configuration
    PA6     ------> QUADSPI1_BK1_IO3
    PA7     ------> QUADSPI1_BK1_IO2
    PB0     ------> QUADSPI1_BK1_IO1
    PB1     ------> QUADSPI1_BK1_IO0
    PB10     ------> QUADSPI1_CLK
    PB11     ------> QUADSPI1_BK1_NCS
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6|GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11);

    /* QUADSPI interrupt Deinit */
    HAL_NVIC_DisableIRQ(QUADSPI_IRQn);
  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

static int read_reg(uint32_t cmd, void *buf, size_t bytes_to_read)
{
	QSPI_CommandTypeDef sCommand = {
		.InstructionMode = QSPI_INSTRUCTION_1_LINE,
		.Instruction = cmd,
		.AddressMode = QSPI_ADDRESS_NONE,
		.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
		.DataMode = QSPI_DATA_1_LINE,
		.DdrMode = QSPI_DDR_MODE_DISABLE,
		.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY,
		.SIOOMode = QSPI_SIOO_INST_EVERY_CMD,
		.NbData = bytes_to_read,
	};

	if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return -EIO;
	}

	if (HAL_QSPI_Receive(&hqspi1, buf, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return -EIO;
	}

	return 0;
}

static int write_reg(uint32_t cmd, bool addr,
		const void *data, size_t bytes_to_write)
{
	QSPI_CommandTypeDef sCommand = {
		.InstructionMode = QSPI_INSTRUCTION_1_LINE,
		.Instruction = cmd,
		.AddressMode = QSPI_ADDRESS_NONE,
		.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
		.DataMode = QSPI_DATA_NONE,
		.DdrMode = QSPI_DDR_MODE_DISABLE,
		.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY,
		.SIOOMode = QSPI_SIOO_INST_EVERY_CMD,
	};

	if (addr) {
		sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
		sCommand.AddressSize = QSPI_ADDRESS_24_BITS,
		sCommand.Address = *(uint32_t *)data;
	} else if (data && bytes_to_write != 0) {
		sCommand.DataMode = QSPI_DATA_1_LINE;
		sCommand.NbData = bytes_to_write;
	}

	if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return -EIO;
	}

	if (!addr && data && bytes_to_write != 0) {
		if (HAL_QSPI_Transmit(&hqspi1, data,
				HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
			return -EIO;
		}
	}

	return 0;
}

static int write_data(uint32_t cmd, uint32_t addr,
		const void *data, size_t bytes_to_write)
{
	QSPI_CommandTypeDef sCommand = {
		.InstructionMode = QSPI_INSTRUCTION_1_LINE,
		.Instruction = cmd,
		.AddressMode = QSPI_ADDRESS_4_LINES,
		.DataMode = QSPI_DATA_4_LINES,
		.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
		.DdrMode = QSPI_DDR_MODE_DISABLE,
		.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY,
		.SIOOMode = QSPI_SIOO_INST_EVERY_CMD,
		.AddressSize = QSPI_ADDRESS_24_BITS,
		.Address = addr,
		.NbData = bytes_to_write,
	};

	if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return -EIO;
	}
	if (HAL_QSPI_Transmit(&hqspi1, data, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return -EIO;
	}

	return 0;
}

static int read_data(uint32_t cmd, uint32_t addr,
		void *buf, size_t bytes_to_read,
		uint32_t dummy_cycles)
{
	QSPI_CommandTypeDef sCommand = {
		.InstructionMode = QSPI_INSTRUCTION_1_LINE,
		.Instruction = cmd,
		.AddressMode = QSPI_ADDRESS_4_LINES,
		.DataMode = QSPI_DATA_4_LINES,
		.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
		.DdrMode = QSPI_DDR_MODE_DISABLE,
		.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY,
		.SIOOMode = QSPI_SIOO_INST_EVERY_CMD,
		.AddressSize = QSPI_ADDRESS_24_BITS,
		.Address = addr,
		.NbData = bytes_to_read,
		.DummyCycles = dummy_cycles,
	};

	if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return -EIO;
	}
	if (HAL_QSPI_Receive(&hqspi1, buf, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return -EIO;
	}

	return 0;
}

static int set_mode(enum qspi_access_mode mode,
		uint32_t cmd, uint32_t dummy_cycles)
{
	QSPI_CommandTypeDef sCommand = {
		.InstructionMode = QSPI_INSTRUCTION_1_LINE,
		.Instruction = cmd,
		.AddressMode = QSPI_ADDRESS_4_LINES,
		.DataMode = QSPI_DATA_4_LINES,
		.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
		.DdrMode = QSPI_DDR_MODE_DISABLE,
		.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY,
		.SIOOMode = QSPI_SIOO_INST_EVERY_CMD,
		.AddressSize = QSPI_ADDRESS_24_BITS,
		.DummyCycles = dummy_cycles,
	};

	if (mode == QSPI_MEMORY_MAPPED) {
		QSPI_MemoryMappedTypeDef cfg = { 0, };
		if (HAL_QSPI_MemoryMapped(&hqspi1, &sCommand, &cfg) != HAL_OK) {
			return -EIO;
		}
	}

	return 0;
}

static int enable_qspi(bool enable)
{
	if (enable) {
		MX_QUADSPI1_Init();
		return 0;
	}

	/* disable */
	if (HAL_QSPI_DeInit(&hqspi1) != HAL_OK) {
		return -EFAULT;
	}

	return 0;
}

void HAL_QSPI_CmdCpltCallback(QSPI_HandleTypeDef *hqspi)
{
}

void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
}

void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
}

void HAL_QSPI_StatusMatchCallback(QSPI_HandleTypeDef *hqspi)
{
}

struct qspi *stm32_qspi_create(void)
{
	static struct qspi io = {
		.read_reg = read_reg,
		.write_reg = write_reg,
		.read = read_data,
		.write = write_data,
		.set_mode = set_mode,
		.enable = enable_qspi,
	};

	return &io;
}
/* USER CODE END 1 */
