# SPDX-License-Identifier: Apache-2.0

PORT_ROOT := ports/native
INCS += $(PORT_ROOT)

include $(PORT_ROOT)/$(BOARD).mk
