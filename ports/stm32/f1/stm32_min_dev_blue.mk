# SPDX-License-Identifier: Apache-2.0

SDK_ROOT ?= external/STM32CubeF1
LD_SCRIPT ?= $(PORT_ROOT)/f1/STM32F103C8Tx_FLASH.ld

ST_SRCS = \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c \
	\
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c  \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cec.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_crc.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_eth.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_hcd.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2s.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_irda.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_iwdg.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_mmc.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nand.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nor.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pccard.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sd.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_smartcard.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sram.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_usart.c \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_wwdg.c \
	\
	$(PORT_ROOT)/f1/board.c \
	$(PORT_ROOT)/f1/uart1.c \
	$(PORT_ROOT)/f1/cli.c \
	\
	$(LIBMCU_ROOT)/ports/stubs/semaphore.c

ST_INCS = \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Inc \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
	$(SDK_ROOT)/Drivers/CMSIS/Include \
	\
	$(PORT_ROOT)/f1

ST_DEFS = \
	USE_HAL_DRIVER \
	STM32F103xB \
	HSE_VALUE=8000000U \

$(addprefix $(OUTDIR)/, $(ST_SRCS:%=%.o)): CFLAGS+=-Wno-error

INCS += $(ST_INCS)
DEFS += $(ST_DEFS)

ST_OUTPUT := $(OUTDIR)/libstm32.a
ST_OBJS := $(addprefix $(OUTDIR)/, $(ST_SRCS:%=%.o))
$(eval $(call generate_lib, $(ST_OUTPUT), $(ST_OBJS)))
