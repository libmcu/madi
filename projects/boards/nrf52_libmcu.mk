# SPDX-License-Identifier: Apache-2.0

TARGET_PLATFORM := nrf52
PORT_ROOT := ports/$(TARGET_PLATFORM)
PLATFORM_SPECIFIC_MAKE := $(PORT_ROOT)/nRF5_SDK/sdk.mk
INCS += \
	$(PORT_ROOT) \
	$(PORT_ROOT)/nRF5_SDK
DEFS += TARGET_PLATFORM=$(TARGET_PLATFORM)

include projects/arch/cm4f.mk

.PHONY: flash flash_softdevice erase gdbserver
flash: $(OUTHEX)
	pyocd flash -t nrf52840 $<
flash_softdevice: $(SDK_ROOT)/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex
	pyocd flash -t nrf52840 $<
erase:
	pyocd erase -t nrf52840 --chip
gdbserver:
	$(Q)pyocd $@ -t nrf52840
