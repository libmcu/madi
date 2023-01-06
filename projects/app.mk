# SPDX-License-Identifier: Apache-2.0

include projects/sources.mk

LIBMCU_ROOT ?= $(BASEDIR)/external/libmcu
LIBMCU_MODULES := ao button cli logging metrics trace
include $(LIBMCU_ROOT)/projects/modules.mk
SRCS += $(LIBMCU_MODULES_SRCS)
INCS += $(LIBMCU_MODULES_INCS)

PWIFI_ROOT := $(BASEDIR)/external/pwifi
include $(PWIFI_ROOT)/sources.mk
SRCS += $(PWIFI_SRCS)
INCS += $(PWIFI_INCS)

PBLE_ROOT := $(BASEDIR)/external/pble
include $(PBLE_ROOT)/sources.mk
SRCS += $(PBLE_SRCS)
INCS += $(PBLE_INCS)

DEFS += METRICS_USER_DEFINES=\"$(BASEDIR)/include/metrics.def\"
