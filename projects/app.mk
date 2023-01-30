# SPDX-License-Identifier: Apache-2.0

LIBMCU_ROOT ?= $(BASEDIR)/external/libmcu
LIBMCU_MODULES := ao button cli logging metrics
include $(LIBMCU_ROOT)/projects/modules.mk

PBLE_ROOT := $(BASEDIR)/external/pble
include $(PBLE_ROOT)/sources.mk
PWIFI_ROOT := $(BASEDIR)/external/pwifi
include $(PWIFI_ROOT)/sources.mk
PL4_ROOT := $(BASEDIR)/external/pl4
include $(PL4_ROOT)/sources.mk
PMQTT_ROOT := $(BASEDIR)/external/pmqtt
include $(PMQTT_ROOT)/sources.mk

SRCS += $(LIBMCU_MODULES_SRCS) \
	$(PBLE_SRCS) \
	$(PWIFI_SRCS) \
	$(PL4_SRCS) \
	$(PMQTT_SRCS) \

INCS += $(LIBMCU_MODULES_INCS) \
	$(PBLE_INCS) \
	$(PWIFI_INCS) \
	$(PL4_INCS) \
	$(PMQTT_INCS) \

DEFS += METRICS_USER_DEFINES=\"$(BASEDIR)/include/metrics.def\"
