# SPDX-License-Identifier: MIT

CROSS_COMPILE ?= arm-none-eabi

CFLAGS += \
	-mthumb \
	-mabi=aapcs \

LDFLAGS += \
	-mthumb \
	-mabi=aapcs \
	--specs=nano.specs \
	--specs=nosys.specs \

STDLIBS += -lnosys -lc -lm
