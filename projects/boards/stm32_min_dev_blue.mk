# SPDX-License-Identifier: Apache-2.0

CROSS_COMPILE ?= arm-none-eabi

PORT_ROOT := ports/stm32

CFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb \
	-mabi=aapcs

LDFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb \
	-mabi=aapcs \
	--specs=nano.specs

LIBS += -lc -lnosys -lm
INCS += $(PORT_ROOT)

include $(PORT_ROOT)/f1/$(BOARD).mk

.PHONY: flash erase gdbserver
flash: $(OUTHEX)
	pyocd flash -t stm32f103c8 $<
erase:
	pyocd erase -t $(PROJECT) --chip
gdbserver:
	$(Q)pyocd $@ -t stm32f103c8
	#pyocd pack install stm32f103c8
