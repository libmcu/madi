# SPDX-License-Identifier: MIT

include projects/arch/arm/common.mk

CFLAGS += \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16

LDFLAGS += \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16
