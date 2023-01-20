# SPDX-License-Identifier: Apache-2.0

set(SDK_ROOT ${CMAKE_SOURCE_DIR}/external/nRF5_SDK_17.1.0_ddde560)
set(LD_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/nrf52832.ld)
set(NRF_SRCS
	${SDK_ROOT}/modules/nrfx/mdk/gcc_startup_nrf52.S

	${SDK_ROOT}/components/ble/ble_advertising/ble_advertising.c
	${SDK_ROOT}/components/ble/common/ble_advdata.c
	${SDK_ROOT}/components/ble/common/ble_conn_params.c
	${SDK_ROOT}/components/ble/common/ble_conn_state.c
	${SDK_ROOT}/components/ble/common/ble_srv_common.c
	${SDK_ROOT}/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c
	${SDK_ROOT}/components/ble/nrf_ble_gatt/nrf_ble_gatt.c
	${SDK_ROOT}/components/ble/nrf_ble_qwr/nrf_ble_qwr.c
	${SDK_ROOT}/components/ble/nrf_ble_scan/nrf_ble_scan.c
	${SDK_ROOT}/components/ble/ble_db_discovery/ble_db_discovery.c
	${SDK_ROOT}/components/ble/peer_manager/peer_manager.c
	${SDK_ROOT}/components/ble/peer_manager/peer_manager_handler.c
	${SDK_ROOT}/components/ble/peer_manager/security_manager.c
	${SDK_ROOT}/components/ble/peer_manager/gatt_cache_manager.c
	${SDK_ROOT}/components/ble/peer_manager/gatts_cache_manager.c
	${SDK_ROOT}/components/ble/peer_manager/id_manager.c
	${SDK_ROOT}/components/ble/peer_manager/nrf_ble_lesc.c
	${SDK_ROOT}/components/ble/peer_manager/peer_data_storage.c
	${SDK_ROOT}/components/ble/peer_manager/peer_database.c
	${SDK_ROOT}/components/ble/peer_manager/peer_id.c
	${SDK_ROOT}/components/ble/peer_manager/auth_status_tracker.c
	${SDK_ROOT}/components/ble/peer_manager/pm_buffer.c
	${SDK_ROOT}/components/ble/peer_manager/security_dispatcher.c
	${SDK_ROOT}/components/softdevice/common/nrf_sdh.c
	${SDK_ROOT}/components/softdevice/common/nrf_sdh_ble.c
	${SDK_ROOT}/components/softdevice/common/nrf_sdh_soc.c
	${SDK_ROOT}/components/boards/boards.c
	${SDK_ROOT}/components/libraries/log/src/nrf_log_frontend.c
	${SDK_ROOT}/components/libraries/log/src/nrf_log_str_formatter.c
	${SDK_ROOT}/components/libraries/util/app_error.c
	${SDK_ROOT}/components/libraries/util/app_error_handler_gcc.c
	${SDK_ROOT}/components/libraries/util/app_error_weak.c
	${SDK_ROOT}/components/libraries/util/app_util_platform.c
	${SDK_ROOT}/components/libraries/util/nrf_assert.c
	${SDK_ROOT}/components/libraries/atomic/nrf_atomic.c
	${SDK_ROOT}/components/libraries/atomic_fifo/nrf_atfifo.c
	${SDK_ROOT}/components/libraries/atomic_flags/nrf_atflags.c
	${SDK_ROOT}/components/libraries/balloc/nrf_balloc.c
	${SDK_ROOT}/components/libraries/memobj/nrf_memobj.c
	${SDK_ROOT}/components/libraries/ringbuf/nrf_ringbuf.c
	${SDK_ROOT}/components/libraries/strerror/nrf_strerror.c
	${SDK_ROOT}/components/libraries/uart/app_uart_fifo.c
	${SDK_ROOT}/components/libraries/fifo/app_fifo.c
	${SDK_ROOT}/components/libraries/uart/retarget.c
	${SDK_ROOT}/components/libraries/sortlist/nrf_sortlist.c
	${SDK_ROOT}/components/libraries/timer/drv_rtc.c
	${SDK_ROOT}/components/libraries/scheduler/app_scheduler.c
	${SDK_ROOT}/components/libraries/timer/app_timer2.c
	${SDK_ROOT}/components/libraries/experimental_section_vars/nrf_section_iter.c
	${SDK_ROOT}/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c
	${SDK_ROOT}/components/libraries/fds/fds.c
	${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage.c
	${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage_nvmc.c
	${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage_sd.c
	${SDK_ROOT}/external/fprintf/nrf_fprintf.c
	${SDK_ROOT}/external/fprintf/nrf_fprintf_format.c
	${SDK_ROOT}/modules/nrfx/soc/nrfx_atomic.c
	${SDK_ROOT}/modules/nrfx/mdk/system_nrf52.c
	${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uart.c
	${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uarte.c
	${SDK_ROOT}/modules/nrfx/drivers/src/prs/nrfx_prs.c
	${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_clock.c
	${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_gpiote.c
	${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_rtc.c
	${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_uart.c
	${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_clock.c

	${SDK_ROOT}/components/ble/ble_services/ble_nus/ble_nus.c

	${CMAKE_CURRENT_SOURCE_DIR}/timext.c

	${CMAKE_SOURCE_DIR}/external/libmcu/ports/stubs/semaphore.c
)
set(NRF_INCS
	${SDK_ROOT}/components/softdevice/s132/headers/nrf52
	${SDK_ROOT}/components/softdevice/s132/headers
	${SDK_ROOT}/components/ble/common
	${SDK_ROOT}/components/ble/ble_advertising
	${SDK_ROOT}/components/ble/ble_link_ctx_manager
	${SDK_ROOT}/components/ble/nrf_ble_gatt
	${SDK_ROOT}/components/ble/nrf_ble_qwr
	${SDK_ROOT}/components/ble/nrf_ble_scan
	${SDK_ROOT}/components/ble/ble_db_discovery
	${SDK_ROOT}/components/ble/peer_manager
	${SDK_ROOT}/components/softdevice/common
	${SDK_ROOT}/components
	${SDK_ROOT}/components/libraries/strerror
	${SDK_ROOT}/components/toolchain/cmsis/include
	${SDK_ROOT}/components/libraries/sortlist
	${SDK_ROOT}/components/libraries/util
	${SDK_ROOT}/components/libraries/balloc
	${SDK_ROOT}/components/libraries/ringbuf
	${SDK_ROOT}/components/libraries/bsp
	${SDK_ROOT}/components/libraries/log
	${SDK_ROOT}/components/libraries/experimental_section_vars
	${SDK_ROOT}/components/libraries/delay
	${SDK_ROOT}/components/libraries/uart
	${SDK_ROOT}/components/libraries/fifo
	${SDK_ROOT}/components/libraries/atomic_fifo
	${SDK_ROOT}/components/libraries/atomic_flags
	${SDK_ROOT}/components/libraries/timer
	${SDK_ROOT}/components/libraries/scheduler
	${SDK_ROOT}/components/libraries/experimental_section_vars
	${SDK_ROOT}/components/libraries/atomic
	${SDK_ROOT}/components/libraries/memobj
	${SDK_ROOT}/components/libraries/log/src
	${SDK_ROOT}/components/libraries/mutex
	${SDK_ROOT}/components/libraries/pwr_mgmt
	${SDK_ROOT}/components/libraries/fds
	${SDK_ROOT}/components/libraries/fstorage
	${SDK_ROOT}/components/boards
	${SDK_ROOT}/components/drivers_nrf/nrf_soc_nosd
	${SDK_ROOT}/external/fprintf
	${SDK_ROOT}/integration/nrfx
	${SDK_ROOT}/integration/nrfx/legacy
	${SDK_ROOT}/modules/nrfx/mdk
	${SDK_ROOT}/modules/nrfx/hal
	${SDK_ROOT}/modules/nrfx
	${SDK_ROOT}/modules/nrfx/drivers/include
	${SDK_ROOT}/modules/nrfx/drivers/src/prs
	${SDK_ROOT}/modules/nrfx/drivers/src

	${SDK_ROOT}/components/ble/ble_services/ble_nus
	${SDK_ROOT}/components/ble/ble_services/ble_nus_c

	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)
set(NRF_DEFS
	SOFTDEVICE_PRESENT
	S132
	NRF_SD_BLE_API_VERSION=7

	CUSTOM_BOARD_INC=redbear
	BSP_DEFINES_ONLY
	CONFIG_GPIO_AS_PINRESET
	FLOAT_ABI_HARD
	NRF52_PAN_74
	NRF52832_XXAA
	NRF52
	__HEAP_SIZE=8192
	__STACK_SIZE=8192
	APP_TIMER_V2
	APP_TIMER_V2_RTC1_ENABLED
	_POSIX_C_SOURCE=200809L
)

add_library(nrf52 OBJECT ${NRF_SRCS})
target_include_directories(nrf52 PRIVATE ${APP_INCS} PUBLIC ${NRF_INCS})
target_compile_definitions(nrf52 PUBLIC ${NRF_DEFS})
target_link_libraries(nrf52 libmcu)

target_link_libraries(${PROJECT_NAME} PRIVATE nrf52)
target_link_options(${PROJECT_NAME} PRIVATE -T${LD_SCRIPT})
target_link_directories(${PROJECT_NAME} PRIVATE ${SDK_ROOT}/modules/nrfx/mdk)

target_link_libraries(pble nrf52)
