# SPDX-License-Identifier: MIT

RTT_ROOT ?= external/SEGGER_RTT_V784e

RTT_SRCS = \
	$(RTT_ROOT)/RTT/SEGGER_RTT.c \
	$(RTT_ROOT)/RTT/SEGGER_RTT_printf.c \

RTT_INCS = \
	$(RTT_ROOT)/RTT \
	$(RTT_ROOT)/Config \

RTT_DEFS =

$(addprefix $(OUTDIR)/, $(RTT_SRCS:%=%.o)): CFLAGS+=-Wno-error
