CROSS_COMPILE ?= arm-none-eabi

PORT_ROOT := ports/stm32/f1

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

include $(PORT_ROOT)/sdk.mk

.PHONY: flash erase gdbserver
flash: $(OUTHEX)
	pyocd flash -t $(PROJECT) $<
erase:
	pyocd erase -t $(PROJECT) --chip
gdbserver:
	$(Q)pyocd $@ -t $(PROJECT)
