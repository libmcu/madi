# SPDX-License-Identifier: Apache-2.0

HOST_SRCS = $(PORT_ROOT)/board.c
HOST_INCS = $(PORT_ROOT)
HOST_DEFS = _POSIX_C_SOURCE=200809L

INCS += $(HOST_INCS)
DEFS += $(HOST_DEFS)

HOST_OUTPUT := $(OUTDIR)/libhost.a
HOST_OBJS := $(addprefix $(OUTDIR)/, $(HOST_SRCS:%=%.o))
DEPS += $(HOST_OBJS:.o=.d)
LIBS += -Wl,--whole-archive -lhost -Wl,--no-whole-archive

$(OUTELF):: $(HOST_OUTPUT)
$(eval $(call generate_lib, $(HOST_OUTPUT), $(HOST_OBJS)))
