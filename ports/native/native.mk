# SPDX-License-Identifier: Apache-2.0

NATIVE_SRCS = $(PORT_ROOT)/board.c
NATIVE_INCS = $(PORT_ROOT)
NATIVE_DEFS = _POSIX_C_SOURCE=200809L

INCS += $(NATIVE_INCS)
DEFS += $(NATIVE_DEFS)

NATIVE_OUTPUT := $(OUTDIR)/libnative.a
NATIVE_OBJS := $(addprefix $(OUTDIR)/, $(NATIVE_SRCS:%=%.o))
$(eval $(call generate_lib, $(NATIVE_OUTPUT), $(NATIVE_OBJS)))
