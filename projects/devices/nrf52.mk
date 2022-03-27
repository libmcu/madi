CROSS_COMPILE ?= arm-none-eabi

SDK_ROOT := external/nRF5_SDK_17.0.2_d674dde
PORT_ROOT := ports/nrf52

LD_SCRIPT := $(PORT_ROOT)/sdk/nrf52832.ld
LIBS += -lc -lnosys -lm
LIBDIRS += -L$(SDK_ROOT)/modules/nrfx/mdk

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

INCS += \
	$(PORT_ROOT) \
	$(PORT_ROOT)/sdk

DEFS += \
	SOFTDEVICE_PRESENT \
	S132 \
	NRF_SD_BLE_API_VERSION=7 \
	\
	CUSTOM_BOARD_INC=redbear \
	BSP_DEFINES_ONLY \
	CONFIG_GPIO_AS_PINRESET \
	FLOAT_ABI_HARD \
	NRF52_PAN_74 \
	NRF52832_XXAA \
	NRF52 \
	__HEAP_SIZE=8192 \
	__STACK_SIZE=8192 \
	APP_TIMER_V2 \
	APP_TIMER_V2_RTC1_ENABLED \

include $(PORT_ROOT)/sdk/sdk.mk

.PHONY: flash erase gdbserver
flash: $(OUTHEX)
	pyocd flash -t $(PROJECT) $<
erase:
	pyocd erase -t $(PROJECT) --chip
gdbserver:
	$(Q)pyocd $@ -t $(PROJECT)
	#pyocd pack install stm32f103c8
