# SPDX-License-Identifier: Apache-2.0

LIBMCU_ROOT ?= $(BASEDIR)/external/libmcu
LIBMCU_MODULES := cli logging trace
include $(LIBMCU_ROOT)/projects/modules.mk

app-src-dirs := src stubs
APP_SRCS = $(foreach dir, $(addprefix $(BASEDIR)/, $(app-src-dirs)), \
	$(shell find $(dir) -type f \( -iname \*.c -o -iname \*.cpp \)))

SRCS += \
	$(APP_SRCS) \
	$(LIBMCU_MODULES_SRCS) \

INCS += \
	$(BASEDIR)/include \
	$(BASEDIR)/drivers/include \
	$(LIBMCU_MODULES_INCS) \

DEFS += \
	$(BOARD) \
	BUILD_DATE=$(BUILD_DATE) \
	VERSION_TAG=$(VERSION_TAG) \
	VERSION=$(VERSION)

OBJS += $(addprefix $(OUTDIR)/, $(SRCS:%=%.o))
LIBDIRS += $(OUTDIR)

ifndef NDEBUG
$(addprefix $(OUTDIR)/, $(SRCS:%=%.o)): CFLAGS+=-finstrument-functions
endif
