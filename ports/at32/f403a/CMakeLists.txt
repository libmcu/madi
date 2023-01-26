# SPDX-License-Identifier: Apache-2.0

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_LIST_DIR} PORT_SRCS)

set(SDK_ROOT ${CMAKE_SOURCE_DIR}/external/at32sdk/AT32F403A_407_Firmware_Library/libraries)
set(LD_SCRIPT ${SDK_ROOT}/cmsis/cm4/device_support/startup/gcc/linker/AT32F403AxG_FLASH.ld)
set(AT_SRCS
	${SDK_ROOT}/cmsis/cm4/device_support/startup/gcc/startup_at32f403a_407.S
	${SDK_ROOT}/cmsis/cm4/device_support/system_at32f403a_407.c
	${SDK_ROOT}/drivers/src/at32f403a_407_acc.c
	${SDK_ROOT}/drivers/src/at32f403a_407_adc.c
	${SDK_ROOT}/drivers/src/at32f403a_407_bpr.c
	${SDK_ROOT}/drivers/src/at32f403a_407_can.c
	${SDK_ROOT}/drivers/src/at32f403a_407_crc.c
	${SDK_ROOT}/drivers/src/at32f403a_407_crm.c
	${SDK_ROOT}/drivers/src/at32f403a_407_dac.c
	${SDK_ROOT}/drivers/src/at32f403a_407_debug.c
	${SDK_ROOT}/drivers/src/at32f403a_407_dma.c
	${SDK_ROOT}/drivers/src/at32f403a_407_emac.c
	${SDK_ROOT}/drivers/src/at32f403a_407_exint.c
	${SDK_ROOT}/drivers/src/at32f403a_407_flash.c
	${SDK_ROOT}/drivers/src/at32f403a_407_gpio.c
	${SDK_ROOT}/drivers/src/at32f403a_407_i2c.c
	${SDK_ROOT}/drivers/src/at32f403a_407_misc.c
	${SDK_ROOT}/drivers/src/at32f403a_407_pwc.c
	${SDK_ROOT}/drivers/src/at32f403a_407_rtc.c
	${SDK_ROOT}/drivers/src/at32f403a_407_sdio.c
	${SDK_ROOT}/drivers/src/at32f403a_407_spi.c
	${SDK_ROOT}/drivers/src/at32f403a_407_tmr.c
	${SDK_ROOT}/drivers/src/at32f403a_407_usart.c
	${SDK_ROOT}/drivers/src/at32f403a_407_usb.c
	${SDK_ROOT}/drivers/src/at32f403a_407_wdt.c
	${SDK_ROOT}/drivers/src/at32f403a_407_wwdt.c
	${SDK_ROOT}/drivers/src/at32f403a_407_xmc.c

	${PORT_SRCS}

	${CMAKE_SOURCE_DIR}/external/libmcu/ports/stubs/semaphore.c
)
set(AT_INCS
	${SDK_ROOT}/cmsis/cm4/core_support
	${SDK_ROOT}/cmsis/cm4/device_support
	${SDK_ROOT}/drivers/inc

	${CMAKE_CURRENT_LIST_DIR}
)
set(AT_DEFS
	AT32F403ACGU7
)

target_include_directories(libmcu PUBLIC
	${CMAKE_SOURCE_DIR}/external/libmcu/modules/common/include/libmcu/posix)

add_library(at32 OBJECT ${AT_SRCS})
target_include_directories(at32 PRIVATE ${APP_INCS} PUBLIC ${AT_INCS})
target_compile_definitions(at32 PUBLIC ${AT_DEFS})
target_link_libraries(at32 libmcu)

target_link_libraries(${PROJECT_NAME} PRIVATE at32)
target_link_options(${PROJECT_NAME} PRIVATE -T${LD_SCRIPT})
