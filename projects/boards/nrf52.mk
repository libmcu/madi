# SPDX-License-Identifier: Apache-2.0

CROSS_COMPILE ?= arm-none-eabi

PORT_ROOT := ports/nrf52

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
INCS += \
	$(PORT_ROOT) \
	$(PORT_ROOT)/nRF5_SDK

include $(PORT_ROOT)/nRF5_SDK/sdk.mk

.PHONY: flash erase gdbserver
flash: $(OUTHEX)
	pyocd flash -t $(PROJECT) $<
erase:
	pyocd erase -t $(PROJECT) --chip
gdbserver:
	$(Q)pyocd $@ -t $(PROJECT)
	#pyocd pack install stm32f103c8
