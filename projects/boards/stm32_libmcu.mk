# SPDX-License-Identifier: Apache-2.0

PORT_ROOT := ports/stm32
PLATFORM_SPECIFIC_MAKE := $(PORT_ROOT)/g4/$(BOARD).mk
INCS += $(PORT_ROOT)

include projects/arch/cm4f.mk

.PHONY: flash erase gdbserver
flash: $(OUTHEX)
	pyocd flash -t stm32g473ce $<
erase:
	pyocd erase -t stm32g473ce --chip
gdbserver:
	$(Q)pyocd $@ -t stm32g473ce
