# SPDX-License-Identifier: Apache-2.0

PORT_ROOT := ports/nrf52

INCS += \
	$(PORT_ROOT) \
	$(PORT_ROOT)/nRF5_SDK

include projects/arch/cm4f.mk
include $(PORT_ROOT)/nRF5_SDK/sdk.mk

.PHONY: flash erase gdbserver
flash: $(OUTHEX)
	pyocd flash -t $(PROJECT) $<
erase:
	pyocd erase -t $(PROJECT) --chip
gdbserver:
	$(Q)pyocd $@ -t $(PROJECT)
