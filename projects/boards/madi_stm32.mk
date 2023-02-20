# SPDX-License-Identifier: Apache-2.0

TARGET_PLATFORM := stm32
PORT_ROOT := ports/${TARGET_PLATFORM}
PLATFORM_SPECIFIC_MAKE := $(PORT_ROOT)/g4/$(BOARD).mk
INCS += $(PORT_ROOT)

include projects/arch/cm4f.mk

.PHONY: flash erase gdbserver
flash: $(OUTBIN)
	dfu-util --alt 0 --dfuse-address 0x08000000:leave --download $<
erase:
	dfu-util --alt 0 --dfuse-address 0x08000000:mass-erase:force
gdbserver:
	$(Q)pyocd $@ -t stm32g473ce
