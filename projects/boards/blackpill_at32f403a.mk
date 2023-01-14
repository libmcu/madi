# SPDX-License-Identifier: Apache-2.0

PORT_ROOT := ports/at32
PLATFORM_SPECIFIC_MAKE := $(PORT_ROOT)/f403a/$(BOARD).mk
INCS += $(PORT_ROOT)

include projects/arch/cm4f.mk

.PHONY: flash erase gdbserver
flash: $(OUTBIN)
	$(Q)dfu-util \
		--device 2e3c:df11 \
		--alt 0 \
		--download $< \
		--dfuse-address 0x08000000 \
		--reset

erase:
	$(Q)dfu-util \
		--device 2e3c:df11 \
		--alt 0 \
		--dfuse-address 0x08000000:force:mass-erase:leave \
		--reset
