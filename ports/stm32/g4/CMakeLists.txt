# SPDX-License-Identifier: Apache-2.0

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_LIST_DIR} PORT_SRCS)

set(SDK_ROOT ${CMAKE_SOURCE_DIR}/external/STM32CubeG4)
set(LD_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/STM32G473CEUx_FLASH.ld)
set(ST_SRCS
	${SDK_ROOT}/Drivers/CMSIS/Device/ST/STM32G4xx/Source/Templates/gcc/startup_stm32g473xx.s
	${SDK_ROOT}/Drivers/CMSIS/Device/ST/STM32G4xx/Source/Templates/system_stm32g4xx.c

	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pcd.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pcd_ex.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_usb.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc_ex.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_adc.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_fdcan.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_i2c.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_i2c_ex.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_qspi.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c

	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/croutine.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/list.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/queue.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/tasks.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/timers.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c

	${PORT_SRCS}
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/main.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/usart.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/gpio.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/i2c.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/adc.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/quadspi.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32g4xx_hal_msp.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32g4xx_hal_timebase_tim.c
	${CMAKE_CURRENT_LIST_DIR}/Core/Src/stm32g4xx_it.c

	${CMAKE_SOURCE_DIR}/external/libmcu/ports/freertos/board.c
	${CMAKE_SOURCE_DIR}/external/libmcu/ports/freertos/pthread.c
	${CMAKE_SOURCE_DIR}/external/libmcu/ports/freertos/pthread_mutex.c
	${CMAKE_SOURCE_DIR}/external/libmcu/ports/freertos/ao.c
	${CMAKE_SOURCE_DIR}/external/libmcu/ports/freertos/semaphore.c
	${CMAKE_SOURCE_DIR}/external/libmcu/ports/freertos/timext.c
	${CMAKE_SOURCE_DIR}/external/libmcu/ports/stubs/syscall.c
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
)
set(ST_INCS
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Inc
	${SDK_ROOT}/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy
	${SDK_ROOT}/Drivers/CMSIS/Device/ST/STM32G4xx/Include
	${SDK_ROOT}/Drivers/CMSIS/Include
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/include
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
	${SDK_ROOT}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2

	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/Core/Inc
)
set(ST_DEFS
	USE_HAL_DRIVER
	STM32G473xx

	CFG_TUSB_MCU=OPT_MCU_STM32G4
	asm=__asm__
)

if (NOT EXISTS ${SDK_ROOT})
	message(INFO "Downloading STM32CubeG4")
	set(FETCHCONTENT_QUIET FALSE)
	include(FetchContent)
	FetchContent_Declare(stm32_sdk
		GIT_REPOSITORY "https://github.com/STMicroelectronics/STM32CubeG4.git"
		GIT_TAG "master"
		GIT_PROGRESS TRUE
		SOURCE_DIR "${SDK_ROOT}"
	)
	FetchContent_MakeAvailable(stm32_sdk)
endif()

target_include_directories(libmcu PUBLIC
	${CMAKE_SOURCE_DIR}/external/libmcu/modules/common/include/libmcu/posix)

add_library(stm32 OBJECT ${ST_SRCS})
target_include_directories(stm32 PUBLIC ${ST_INCS} ${APP_INCS})
target_compile_definitions(stm32 PUBLIC ${ST_DEFS})
target_link_libraries(stm32 PUBLIC libmcu bq25180)

target_link_libraries(tinyusb PRIVATE stm32)

target_link_libraries(${PROJECT_NAME} PRIVATE stm32)
target_link_options(${PROJECT_NAME} PRIVATE -T${LD_SCRIPT})
