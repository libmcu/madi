CROSS_COMPILE ?= arm-none-eabi

SDK_ROOT := external/stm32f1
PORT_ROOT := ports/stm32/f1

LD_SCRIPT := $(PORT_ROOT)/STM32F103C8Tx_FLASH.ld
LIBS += -lc -lnosys -lm

CFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb \
	-mabi=aapcs

LDFLAGS += \
	-mcpu=cortex-m3 \
	-mthumb \
	-mabi=aapcs \
	--specs=nano.specs

INCS += \
	$(PORT_ROOT) \
	$(PORT_ROOT)/sdk

DEFS += \
	USE_HAL_DRIVER \
	STM32F103xB \

include $(PORT_ROOT)/sdk.mk

.PHONY: flash erase gdbserver
flash: $(OUTHEX)
	pyocd flash -t $(PROJECT) $<
erase:
	pyocd erase -t $(PROJECT) --chip
gdbserver:
	$(Q)pyocd $@ -t $(PROJECT)
	#pyocd pack install stm32f103c8
