# SPDX-License-Identifier: MIT

include projects/arch/arm/common.mk

CFLAGS += -mcpu=cortex-m3
LDFLAGS += -mcpu=cortex-m3
