# SPDX-License-Identifier: Apache-2.0

app-src-dirs := src
APP_SRCS = $(foreach dir, $(addprefix $(BASEDIR)/, $(app-src-dirs)), \
	$(shell find $(dir) -type f \( -iname \*.c -o -iname \*.cpp \)))

SRCS += $(APP_SRCS)
INCS += $(BASEDIR)/include
DEFS += \
	$(BOARD) \
	BUILD_DATE=$(BUILD_DATE) \
	VERSION_TAG=$(VERSION_TAG) \
	VERSION=$(VERSION) \
	\
	_POSIX_THREADS \
	_POSIX_C_SOURCE=200809L \

OBJS += $(addprefix $(OUTDIR)/, $(SRCS:%=%.o))
LIBDIRS += $(OUTDIR)
