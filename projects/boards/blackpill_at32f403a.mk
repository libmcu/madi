# SPDX-License-Identifier: Apache-2.0

CROSS_COMPILE ?= arm-none-eabi

PORT_ROOT := ports/at32

CFLAGS += \
	-mcpu=cortex-m4 \
	-mthumb \
	-mabi=aapcs \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16

LDFLAGS += \
	-mcpu=cortex-m4 \
	-mthumb \
	-mabi=aapcs \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	--specs=nano.specs

LIBS += -lc -lnosys -lm
INCS += $(PORT_ROOT)

include $(PORT_ROOT)/f403a/$(BOARD).mk

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
