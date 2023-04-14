# SPDX-License-Identifier: Apache-2.0

LIBMCU_ROOT ?= $(BASEDIR)/external/libmcu
LIBMCU_MODULES := ao button cli logging metrics
LIBMCU_INTERFACES := i2c l4 uart
include $(LIBMCU_ROOT)/projects/modules.mk
include $(LIBMCU_ROOT)/projects/interfaces.mk

PBLE_ROOT := $(BASEDIR)/external/pble
include $(PBLE_ROOT)/sources.mk
PWIFI_ROOT := $(BASEDIR)/external/pwifi
include $(PWIFI_ROOT)/sources.mk
PL4_ROOT := $(BASEDIR)/external/pl4
include $(PL4_ROOT)/sources.mk
PMQTT_ROOT := $(BASEDIR)/external/pmqtt
include $(PMQTT_ROOT)/sources.mk
BQ25180_ROOT := $(BASEDIR)/external/bq25180
include $(BQ25180_ROOT)/sources.mk

SRCS += $(LIBMCU_MODULES_SRCS) \
	$(LIBMCU_INTERFACES_SRCS) \
	$(PBLE_SRCS) \
	$(PWIFI_SRCS) \
	$(PL4_SRCS) \
	$(PMQTT_SRCS) \
	$(BQ25180_SRCS) \

INCS += $(LIBMCU_MODULES_INCS) \
	$(LIBMCU_INTERFACES_INCS) \
	$(PBLE_INCS) \
	$(PWIFI_INCS) \
	$(PL4_INCS) \
	$(PMQTT_INCS) \
	$(BQ25180_INCS) \

DEFS += METRICS_USER_DEFINES=\"$(BASEDIR)/include/metrics.def\" \
	TARGET_PLATFORM=$(TARGET_PLATFORM) \

CFLAGS += -include libmcu/assert.h
