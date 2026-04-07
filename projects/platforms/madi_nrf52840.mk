# SPDX-License-Identifier: MIT

include projects/arch/arm/cm4f.mk

app-src-dirs := src
APP_SRCS = $(foreach dir, $(addprefix $(BASEDIR)/, $(app-src-dirs)), \
	$(shell find $(dir) -type f \( -iname \*.c -o -iname \*.cpp \)))

# Platfrom
TARGET_PLATFORM := nrf52
PORT_ROOT := ports/$(TARGET_PLATFORM)
PLATFORM_SPECIFIC := $(PORT_ROOT)/platform.mk

# Third Party
LIBMCU_ROOT ?= $(BASEDIR)/external/libmcu
include $(LIBMCU_ROOT)/project/modules.mk
include $(LIBMCU_ROOT)/project/interfaces.mk

include ports/rtt/sources.mk

SRCS += $(APP_SRCS) \
	$(LIBMCU_MODULES_SRCS) \
	$(LIBMCU_INTERFACES_SRCS) \
	$(LIBMCU_ROOT)/ports/armcm/fault.c \
	$(LIBMCU_ROOT)/ports/armcm/assert.c \
	$(RTT_SRCS) \

INCS += $(BASEDIR)/include \
	$(PORT_ROOT) \
	$(LIBMCU_MODULES_INCS) \
	$(LIBMCU_INTERFACES_INCS) \
	$(LIBMCU_ROOT)/ports/armcm/include \
	$(LIBMCU_ROOT)/modules/common/include/libmcu/posix \
	$(RTT_INCS) \

DEFS += $(PROJECT) \
	TARGET_PLATFORM_madi_$(TARGET_PLATFORM)840 \
	\
	BUILD_DATE=$(BUILD_DATE) \
	VERSION_TAG=$(VERSION_TAG) \
	VERSION=$(VERSION) \
	\
	_POSIX_THREADS \
	_POSIX_C_SOURCE=200809L \
	\
	METRICS_USER_DEFINES=\"$(BASEDIR)/include/metrics.def\" \
	LOGGING_MESSAGE_MAXLEN=256 \

OBJS += $(addprefix $(OUTDIR)/, $(SRCS:%=%.o))
LIBDIRS += $(OUTDIR)
CFLAGS += -include libmcu/assert.h

include $(PLATFORM_SPECIFIC)
