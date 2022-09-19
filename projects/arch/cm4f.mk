# SPDX-License-Identifier: Apache-2.0

include projects/arch/common.mk

CFLAGS += \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16

LDFLAGS += \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16
