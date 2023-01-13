# SPDX-License-Identifier: Apache-2.0

OUTDIR := $(BUILDIR)/$(BOARD)
OUTELF := $(OUTDIR)/$(BOARD)
OUTBIN := $(OUTDIR)/$(BOARD).bin
OUTHEX := $(OUTDIR)/$(BOARD).hex
OUTDEF := $(OUTDIR)/defines.txt
OUTSRC := $(OUTDIR)/sources.txt
OUTINC := $(OUTDIR)/includes.txt

OUTPUT ?= $(OUTELF) $(OUTHEX) $(OUTBIN) $(OUTDEF) $(OUTSRC) $(OUTINC) \
	  $(OUTELF).size $(OUTELF).dump $(OUTELF).lst $(OUTELF).sym

define generate_lib
GENERATED_LIBS += $(1)
DEPS += $(2:.o=.d)
$(1): $(2)
	$(info archiving   $(1))
	$(Q)rm -f $(1)
	$(Q)$(AR) $(ARFLAGS) $(1) $(2) 1> /dev/null 2>&1
endef
