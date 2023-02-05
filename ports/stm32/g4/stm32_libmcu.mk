# SPDX-License-Identifier: Apache-2.0

SDK_ROOT ?= external/STM32CubeG4
LD_SCRIPT ?= $(PORT_ROOT)/g4/STM32G473CEUx_FLASH.ld

ST_SRCS = \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32G4xx/Source/Templates/gcc/startup_stm32g473xx.s \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32G4xx/Source/Templates/system_stm32g4xx.c \
	\
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pcd.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pcd_ex.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_usb.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc_ex.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_adc.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_fdcan.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_i2c.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_i2c_ex.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_qspi.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c \
	\
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/list.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/queue.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/timers.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
	\
	$(wildcard $(PORT_ROOT)/g4/*.c) \
	$(wildcard $(PORT_ROOT)/g4/*.cpp) \
	$(PORT_ROOT)/g4/Core/Src/main.c \
	$(PORT_ROOT)/g4/Core/Src/usart.c \
	$(PORT_ROOT)/g4/Core/Src/gpio.c \
	$(PORT_ROOT)/g4/Core/Src/i2c.c \
	$(PORT_ROOT)/g4/Core/Src/adc.c \
	$(PORT_ROOT)/g4/Core/Src/stm32g4xx_hal_msp.c \
	$(PORT_ROOT)/g4/Core/Src/stm32g4xx_hal_timebase_tim.c \
	$(PORT_ROOT)/g4/Core/Src/stm32g4xx_it.c \
	\
	$(PORT_ROOT)/g4/USB_Device/App/usb_device.c \
	$(PORT_ROOT)/g4/USB_Device/App/usbd_cdc_if.c \
	$(PORT_ROOT)/g4/USB_Device/App/usbd_desc.c \
	$(PORT_ROOT)/g4/USB_Device/Target/usbd_conf.c \
	\
	$(LIBMCU_ROOT)/ports/freertos/board.c \
	$(LIBMCU_ROOT)/ports/freertos/pthread.c \
	$(LIBMCU_ROOT)/ports/freertos/ao.c \
	$(LIBMCU_ROOT)/ports/freertos/semaphore.c \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c \

ST_INCS = \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Inc \
	$(SDK_ROOT)/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy \
	$(SDK_ROOT)/Drivers/CMSIS/Device/ST/STM32G4xx/Include \
	$(SDK_ROOT)/Drivers/CMSIS/Include \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/include \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F \
	$(SDK_ROOT)/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
	$(SDK_ROOT)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
	\
	$(PORT_ROOT)/g4 \
	$(PORT_ROOT)/g4/Core/Inc \
	$(PORT_ROOT)/g4/USB_Device/App \
	$(PORT_ROOT)/g4/USB_Device/Target \

ST_DEFS = \
	USE_HAL_DRIVER \
	STM32G473xx \

$(addprefix $(OUTDIR)/, $(ST_SRCS:%=%.o)): CFLAGS+=-Wno-error

INCS += $(ST_INCS) $(LIBMCU_ROOT)/modules/common/include/libmcu/posix
DEFS += $(ST_DEFS)

ST_OUTPUT := $(OUTDIR)/libstm32.a
ST_OBJS := $(addprefix $(OUTDIR)/, $(ST_SRCS:%=%.o))
$(eval $(call generate_lib, $(ST_OUTPUT), $(ST_OBJS)))
