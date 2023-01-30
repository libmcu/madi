# SPDX-License-Identifier: Apache-2.0

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_LIST_DIR} PORT_SRCS)

set(SDK_ROOT ${CMAKE_SOURCE_DIR}/external/STM32CubeF1)
set(LD_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/STM32F103C8Tx_FLASH.ld)
set(ST_SRCS
	${SDK_ROOT}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s
	${SDK_ROOT}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c

	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cec.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_crc.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac_ex.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_eth.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_hcd.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2s.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_irda.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_iwdg.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_mmc.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nand.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nor.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pccard.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc_ex.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sd.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_smartcard.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sram.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_usart.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_wwdg.c
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c

	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/croutine.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/list.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/timers.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
	${SDK_ROOT}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
	${SDK_ROOT}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
	${SDK_ROOT}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
	${SDK_ROOT}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c

	${PORT_SRCS}
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/main.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/usart.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/gpio.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32f1xx_hal_msp.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32f1xx_hal_timebase_tim.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32f1xx_it.c

	${CMAKE_CURRENT_LIST_DIR}/USB_DEVICE/App/usb_device.c
	${CMAKE_CURRENT_LIST_DIR}/USB_DEVICE/App/usbd_cdc_if.c
	${CMAKE_CURRENT_LIST_DIR}/USB_DEVICE/App/usbd_desc.c
	${CMAKE_CURRENT_LIST_DIR}/USB_DEVICE/Target/usbd_conf.c

	${CMAKE_SOURCE_DIR}/external/libmcu/ports/freertos/board.c
	${CMAKE_SOURCE_DIR}/external/libmcu/ports/freertos/pthread.c
	${CMAKE_SOURCE_DIR}/external/libmcu/ports/freertos/ao.c
	${CMAKE_SOURCE_DIR}/external/libmcu/ports/freertos/semaphore.c
)
set(ST_INCS
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Inc
	${SDK_ROOT}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
	${SDK_ROOT}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
	${SDK_ROOT}/Drivers/CMSIS/Include
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/include
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
	${SDK_ROOT}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
	${SDK_ROOT}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc

	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/Core/Inc
	${CMAKE_CURRENT_LIST_DIR}/USB_DEVICE/App
	${CMAKE_CURRENT_LIST_DIR}/USB_DEVICE/Target
)
set(ST_DEFS
	USE_HAL_DRIVER
	STM32F103xB
)

target_include_directories(libmcu PUBLIC
	${CMAKE_SOURCE_DIR}/external/libmcu/modules/common/include/libmcu/posix)

add_library(stm32 OBJECT ${ST_SRCS})
target_include_directories(stm32 PUBLIC ${ST_INCS} ${APP_INCS})
target_compile_definitions(stm32 PUBLIC ${ST_DEFS})
target_link_libraries(stm32 PUBLIC libmcu)

target_link_libraries(${PROJECT_NAME} PRIVATE stm32)
target_link_options(${PROJECT_NAME} PRIVATE -T${LD_SCRIPT})
