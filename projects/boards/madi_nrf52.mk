# SPDX-License-Identifier: Apache-2.0

TARGET_PLATFORM := nrf52
PORT_ROOT := ports/nrf5-sdk
PLATFORM_SPECIFIC_MAKE := $(PORT_ROOT)/sdk.mk
INCS += $(PORT_ROOT)

include projects/arch/cm4f.mk

SDK_ROOT ?= external/nRF5_SDK_17.1.0_ddde560
.PHONY: flash flash_jlink flash_softdevice erase gdbserver
flash: $(OUTBIN)
	dfu-util --device 1209:e001 --alt 0 --download $<
flash_jlink: $(OUTHEX)
	nrfjprog -f nrf52 --program $< --sectorerase --verify
	nrfjprog -f nrf52 --reset
flash_softdevice: $(SDK_ROOT)/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex
	nrfjprog -f nrf52 --program $< --sectorerase --verify
erase:
	nrfjprog -f nrf52 --eraseall
gdbserver:
	$(Q)pyocd $@ -t nrf52840
