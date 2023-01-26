# SPDX-License-Identifier: Apache-2.0

SDK_ROOT ?= external/at32sdk/AT32F403A_407_Firmware_Library/libraries
LD_SCRIPT ?= $(SDK_ROOT)/cmsis/cm4/device_support/startup/gcc/linker/AT32F403AxG_FLASH.ld

AT_SRCS = \
	$(SDK_ROOT)/cmsis/cm4/device_support/startup/gcc/startup_at32f403a_407.S \
	$(SDK_ROOT)/cmsis/cm4/device_support/system_at32f403a_407.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_acc.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_adc.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_bpr.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_can.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_crc.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_crm.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_dac.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_debug.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_dma.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_emac.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_exint.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_flash.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_gpio.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_i2c.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_misc.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_pwc.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_rtc.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_sdio.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_spi.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_tmr.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_usart.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_usb.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_wdt.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_wwdt.c \
	$(SDK_ROOT)/drivers/src/at32f403a_407_xmc.c \
	\
	$(wildcard $(PORT_ROOT)/f403a/*.c) \
	$(wildcard $(PORT_ROOT)/f403a/*.cpp) \
	\
	$(LIBMCU_ROOT)/ports/stubs/semaphore.c

AT_INCS = \
	$(SDK_ROOT)/cmsis/cm4/core_support \
	$(SDK_ROOT)/cmsis/cm4/device_support \
	$(SDK_ROOT)/drivers/inc \
	\
	$(PORT_ROOT)/f403a \

AT_DEFS = \
	AT32F403ACGU7 \

$(addprefix $(OUTDIR)/, $(AT_SRCS:%=%.o)): CFLAGS+=-Wno-error

INCS += $(AT_INCS) $(LIBMCU_ROOT)/modules/common/include/libmcu/posix
DEFS += $(AT_DEFS)

AT_OUTPUT := $(OUTDIR)/libat32.a
AT_OBJS := $(addprefix $(OUTDIR)/, $(AT_SRCS:%=%.o))
$(eval $(call generate_lib, $(AT_OUTPUT), $(AT_OBJS)))
