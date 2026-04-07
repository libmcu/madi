# SPDX-License-Identifier: MIT

SDK_ROOT ?= external/nRF5_SDK_17.1.0_ddde560
LD_SCRIPT ?= $(PORT_ROOT)/nrf52840.ld

NRF_SRCS = \
   	$(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S \
	$(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
	\
	$(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
	$(SDK_ROOT)/components/ble/common/ble_advdata.c \
	$(SDK_ROOT)/components/ble/common/ble_conn_params.c \
	$(SDK_ROOT)/components/ble/common/ble_conn_state.c \
	$(SDK_ROOT)/components/ble/common/ble_srv_common.c \
	$(SDK_ROOT)/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c \
	$(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
	$(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
	$(SDK_ROOT)/components/ble/nrf_ble_scan/nrf_ble_scan.c \
	$(SDK_ROOT)/components/ble/ble_db_discovery/ble_db_discovery.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_manager_handler.c \
	$(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
	$(SDK_ROOT)/components/ble/peer_manager/nrf_ble_lesc.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
	$(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
	$(SDK_ROOT)/components/ble/peer_manager/auth_status_tracker.c \
	$(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
	$(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh_freertos.c \
	$(SDK_ROOT)/components/boards/boards.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
	$(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
	$(SDK_ROOT)/components/libraries/util/app_error.c \
	$(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
	$(SDK_ROOT)/components/libraries/util/app_error_weak.c \
	$(SDK_ROOT)/components/libraries/util/app_util_platform.c \
	$(SDK_ROOT)/components/libraries/util/nrf_assert.c \
	$(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
	$(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
	$(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
	$(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
	$(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
	$(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
	$(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
	$(SDK_ROOT)/components/libraries/uart/app_uart_fifo.c \
	$(SDK_ROOT)/components/libraries/fifo/app_fifo.c \
	$(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c \
	$(SDK_ROOT)/components/libraries/timer/drv_rtc.c \
	$(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
	$(SDK_ROOT)/components/libraries/timer/app_timer_freertos.c \
	$(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
	$(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
	$(SDK_ROOT)/components/libraries/fds/fds.c \
	$(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
	$(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_nvmc.c \
	$(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
	$(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
	$(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
	$(SDK_ROOT)/external/freertos/source/croutine.c \
	$(SDK_ROOT)/external/freertos/source/event_groups.c \
	$(SDK_ROOT)/external/freertos/source/portable/MemMang/heap_4.c \
	$(SDK_ROOT)/external/freertos/source/list.c \
	$(SDK_ROOT)/external/freertos/portable/GCC/nrf52/port.c \
	$(SDK_ROOT)/external/freertos/portable/CMSIS/nrf52/port_cmsis.c \
	$(SDK_ROOT)/external/freertos/portable/CMSIS/nrf52/port_cmsis_systick.c \
	$(SDK_ROOT)/external/freertos/source/queue.c \
	$(SDK_ROOT)/external/freertos/source/stream_buffer.c \
	$(SDK_ROOT)/external/freertos/source/tasks.c \
	$(SDK_ROOT)/external/freertos/source/timers.c \
	$(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rtc.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twim.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twis.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_ppi.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_saadc.c \
	$(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_qspi.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_twi.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
	$(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_ppi.c \
	\
	$(SDK_ROOT)/components/ble/ble_services/ble_nus/ble_nus.c \
	\
	$(wildcard $(PORT_ROOT)/*.c) \
	$(wildcard $(PORT_ROOT)/*.cpp) \
	\
	$(LIBMCU_ROOT)/ports/freertos/board.c \
	$(LIBMCU_ROOT)/ports/freertos/pthread.c \
	$(LIBMCU_ROOT)/ports/freertos/pthread_mutex.c \
	$(LIBMCU_ROOT)/ports/freertos/semaphore.c \
	$(LIBMCU_ROOT)/ports/freertos/timext.c \
	$(LIBMCU_ROOT)/ports/freertos/hooks.c \
	$(LIBMCU_ROOT)/ports/nrf52/freertos.c \
	$(LIBMCU_ROOT)/ports/stubs/syscall.c \

NRF_INCS = \
   	$(SDK_ROOT)/components/softdevice/s140/headers/nrf52 \
	$(SDK_ROOT)/components/softdevice/s140/headers \
	\
	$(SDK_ROOT)/components/ble/common \
	$(SDK_ROOT)/components/ble/ble_advertising \
	$(SDK_ROOT)/components/ble/ble_link_ctx_manager \
	$(SDK_ROOT)/components/ble/nrf_ble_gatt \
	$(SDK_ROOT)/components/ble/nrf_ble_qwr \
	$(SDK_ROOT)/components/ble/nrf_ble_scan \
	$(SDK_ROOT)/components/ble/ble_db_discovery \
	$(SDK_ROOT)/components/ble/peer_manager \
	$(SDK_ROOT)/components/softdevice/common \
	$(SDK_ROOT)/components \
	$(SDK_ROOT)/components/libraries/strerror \
	$(SDK_ROOT)/components/toolchain/cmsis/include \
	$(SDK_ROOT)/components/libraries/sortlist \
	$(SDK_ROOT)/components/libraries/util \
	$(SDK_ROOT)/components/libraries/balloc \
	$(SDK_ROOT)/components/libraries/ringbuf \
	$(SDK_ROOT)/components/libraries/bsp \
	$(SDK_ROOT)/components/libraries/log \
	$(SDK_ROOT)/components/libraries/experimental_section_vars \
	$(SDK_ROOT)/components/libraries/delay \
	$(SDK_ROOT)/components/libraries/uart \
	$(SDK_ROOT)/components/libraries/fifo \
	$(SDK_ROOT)/components/libraries/atomic_fifo \
	$(SDK_ROOT)/components/libraries/atomic_flags \
	$(SDK_ROOT)/components/libraries/timer \
	$(SDK_ROOT)/components/libraries/scheduler \
	$(SDK_ROOT)/components/libraries/experimental_section_vars \
	$(SDK_ROOT)/components/libraries/atomic \
	$(SDK_ROOT)/components/libraries/memobj \
	$(SDK_ROOT)/components/libraries/log/src \
	$(SDK_ROOT)/components/libraries/mutex \
	$(SDK_ROOT)/components/libraries/pwr_mgmt \
	$(SDK_ROOT)/components/libraries/fds \
	$(SDK_ROOT)/components/libraries/fstorage \
	$(SDK_ROOT)/components/boards \
	$(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd \
	$(SDK_ROOT)/external/fprintf \
	$(SDK_ROOT)/external/freertos/portable/GCC/nrf52 \
	$(SDK_ROOT)/external/freertos/portable/CMSIS/nrf52 \
	$(SDK_ROOT)/external/freertos/source/include \
	$(SDK_ROOT)/integration/nrfx \
	$(SDK_ROOT)/integration/nrfx/legacy \
	$(SDK_ROOT)/modules/nrfx/mdk \
	$(SDK_ROOT)/modules/nrfx/hal \
	$(SDK_ROOT)/modules/nrfx \
	$(SDK_ROOT)/modules/nrfx/drivers/include \
	$(SDK_ROOT)/modules/nrfx/drivers/src/prs \
	$(SDK_ROOT)/modules/nrfx/drivers/src \
	\
	$(SDK_ROOT)/components/ble/ble_services/ble_nus \
	$(SDK_ROOT)/components/ble/ble_services/ble_nus_c \
	\
	$(PORT_ROOT) \

NRF_DEFS = \
	S140 \
	NRF52840_XXAA \
	CUSTOM_BOARD_INC=madi \
	\
	SOFTDEVICE_PRESENT \
	NRF_SD_BLE_API_VERSION=7 \
	NRFX_SAADC_API_V2 \
	\
	BSP_DEFINES_ONLY \
	CONFIG_GPIO_AS_PINRESET \
	FLOAT_ABI_HARD \
	FREERTOS \
	__HEAP_SIZE=8192 \
	__STACK_SIZE=8192 \
	_POSIX_C_SOURCE=200809L \

$(addprefix $(OUTDIR)/, $(NRF_SRCS:%=%.o)): CFLAGS+=-Wno-error

INCS += $(NRF_INCS)
DEFS += $(NRF_DEFS)
LIBDIRS += $(SDK_ROOT)/modules/nrfx/mdk

NRF_OUTPUT := $(OUTDIR)/libnrf52.a
NRF_OBJS := $(addprefix $(OUTDIR)/, $(NRF_SRCS:%=%.o))
$(eval $(call generate_lib, $(NRF_OUTPUT), $(NRF_OBJS)))

$(addprefix $(OUTDIR)/, $(SRCS:%=%.o)): $(BASEDIR)/external/nRF5_SDK_17.1.0_ddde560
$(BASEDIR)/external/nRF5_SDK_17.1.0_ddde560:
	$(error download     SDK first)

.PHONY: flash flash_usb erase gdbserver
## flash_usb: flash with dfu-util
flash_usb: $(OUTBIN)
	dfu-util --device 1209:e001 --alt 0 --download $<
## flash: flash
flash: $(OUTHEX)
	nrfjprog -f nrf52 --program $< --sectorerase --verify
	nrfjprog -f nrf52 --reset
## flash_softdevice: flash softdevice
flash_softdevice: $(SDK_ROOT)/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex
	nrfjprog -f nrf52 --program $< --sectorerase --verify
## erase: erase flash with dfu-util
erase:
	nrfjprog -f nrf52 --eraseall
## gdbserver: open gdb server
gdbserver:
	$(Q)pyocd $@ -t nrf52840
