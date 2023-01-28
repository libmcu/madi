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
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c \
	\
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/list.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/queue.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/timers.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
	\
	$(wildcard $(PORT_ROOT)/f1/*.c) \
	$(wildcard $(PORT_ROOT)/f1/*.cpp) \
	$(PORT_ROOT)/f1/Core/Src/main.c \
	$(PORT_ROOT)/f1/Core/Src/usart.c \
	$(PORT_ROOT)/f1/Core/Src/gpio.c \
	$(PORT_ROOT)/f1/Core/Src/stm32f1xx_hal_msp.c \
	$(PORT_ROOT)/f1/Core/Src/stm32f1xx_hal_timebase_tim.c \
	$(PORT_ROOT)/f1/Core/Src/stm32f1xx_it.c \
	\
	$(PORT_ROOT)/f1/USB_DEVICE/App/usb_device.c \
	$(PORT_ROOT)/f1/USB_DEVICE/App/usbd_cdc_if.c \
	$(PORT_ROOT)/f1/USB_DEVICE/App/usbd_desc.c \
	$(PORT_ROOT)/f1/USB_DEVICE/Target/usbd_conf.c \
	\
	$(LIBMCU_ROOT)/ports/freertos/board.c \
	$(LIBMCU_ROOT)/ports/freertos/pthread.c \
	$(LIBMCU_ROOT)/ports/freertos/ao.c \
	$(LIBMCU_ROOT)/ports/freertos/semaphore.c

ST_INCS = \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Inc \
	$(SDK_ROOT)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
	$(SDK_ROOT)/Drivers/CMSIS/Include \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/include \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
	\
	$(PORT_ROOT)/f1 \
	$(PORT_ROOT)/f1/Core/Inc \
	$(PORT_ROOT)/f1/USB_DEVICE/App \
	$(PORT_ROOT)/f1/USB_DEVICE/Target \

ST_DEFS = \
	USE_HAL_DRIVER \
	STM32F103xB \

$(addprefix $(OUTDIR)/, $(ST_SRCS:%=%.o)): CFLAGS+=-Wno-error

INCS += $(ST_INCS) $(LIBMCU_ROOT)/modules/common/include/libmcu/posix
DEFS += $(ST_DEFS)

ST_OUTPUT := $(OUTDIR)/libstm32.a
ST_OBJS := $(addprefix $(OUTDIR)/, $(ST_SRCS:%=%.o))
$(eval $(call generate_lib, $(ST_OUTPUT), $(ST_OBJS)))
