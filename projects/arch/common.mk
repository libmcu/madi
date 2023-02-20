# SPDX-License-Identifier: Apache-2.0

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
