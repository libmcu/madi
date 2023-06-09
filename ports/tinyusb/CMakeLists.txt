# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.16)

project(tinyusb LANGUAGES C CXX)

set(TINYUSB_ROOT ${CMAKE_SOURCE_DIR}/external/tinyusb)

if (${BOARD} STREQUAL "esp32s3")
	set(TUSB_MCU OPT_MCU_ESP32S3)
	set(TINYUSB_SRCS
		${TINYUSB_ROOT}/src/portable/synopsys/dwc2/dcd_dwc2.c
		${CMAKE_CURRENT_LIST_DIR}/usbd_esp32.c
	)
elseif (${BOARD} STREQUAL "madi_nrf52")
	set(TUSB_MCU OPT_MCU_NRF5X)
	set(TINYUSB_SRCS
		${CMAKE_CURRENT_LIST_DIR}/dcd_nrf5x.c
		${CMAKE_CURRENT_LIST_DIR}/usbd_nrf5x.c
		${CMAKE_CURRENT_LIST_DIR}/netif_usb.c
	)
elseif (${BOARD} STREQUAL "madi_stm32")
	set(TUSB_MCU OPT_MCU_STM32G4)
	set(TINYUSB_SRCS
		${TINYUSB_ROOT}/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c
		${CMAKE_CURRENT_LIST_DIR}/usbd_stm32g4.c
		${CMAKE_CURRENT_LIST_DIR}/netif_usb.c
	)
else()
	message(FATAL_ERROR "BOARD not specified")
endif()

add_library(${PROJECT_NAME} OBJECT
	${TINYUSB_ROOT}/src/tusb.c
	${TINYUSB_ROOT}/src/common/tusb_fifo.c
	${TINYUSB_ROOT}/src/device/usbd.c
	${TINYUSB_ROOT}/src/device/usbd_control.c
	${TINYUSB_ROOT}/src/class/cdc/cdc_device.c
	${TINYUSB_ROOT}/src/class/net/ecm_rndis_device.c
	${TINYUSB_ROOT}/lib/networking/dhserver.c
	${TINYUSB_ROOT}/lib/networking/dnserver.c
	${TINYUSB_ROOT}/lib/networking/rndis_reports.c
	${CMAKE_CURRENT_LIST_DIR}/descriptors.c
	${CMAKE_CURRENT_LIST_DIR}/usbd.c
	${CMAKE_CURRENT_LIST_DIR}/usbd_cdc_acm.c
	${CMAKE_CURRENT_LIST_DIR}/usbd_cdc_net.c
	${TINYUSB_SRCS}
)
target_include_directories(${PROJECT_NAME} PUBLIC
	${TINYUSB_ROOT}/src
	${TINYUSB_ROOT}/lib/networking
	${CMAKE_CURRENT_LIST_DIR}
)
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CFG_TUSB_MCU=${TUSB_MCU}
)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_99)
