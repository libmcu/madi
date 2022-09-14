# SPDX-License-Identifier: Apache-2.0

DEFS += $(BOARD)
LIBS +=
LIBDIRS +=

OUTDIR := $(BUILDIR)/$(BOARD)
OUTELF := $(OUTDIR)/$(BOARD)
OUTLIB := $(OUTDIR)/lib$(BOARD).a
OUTBIN := $(OUTDIR)/$(BOARD).bin
OUTHEX := $(OUTDIR)/$(BOARD).hex
OUTDEF := $(OUTDIR)/defines.txt
OUTSRC := $(OUTDIR)/sources.txt
OUTINC := $(OUTDIR)/includes.txt

OUTPUT ?= $(OUTELF) $(OUTLIB) $(OUTHEX) $(OUTBIN) $(OUTDEF) $(OUTSRC) $(OUTINC) \
	  $(OUTELF).size $(OUTELF).dump $(OUTELF).lst $(OUTELF).sym

SRCS += $(foreach dir, $(SRCDIRS), $(shell find $(dir) -type f -regex ".*\.c"))
OBJS += $(addprefix $(OUTDIR)/, $(SRCS:%=%.o))
DEPS += $(OBJS:.o=.d)

include projects/app.mk
include projects/boards/$(BOARD).mk
include projects/toolchain.mk

.DEFAULT_GOAL :=
all:: $(OUTPUT)
	$(info done $(PROJECT)_$(VERSION)-$(BOARD))

$(OUTELF):: $(OBJS) $(EXTRA_OBJS) $(LD_SCRIPT)
	$(info linking     $@)
	$(Q)$(SZ) -t --common $(sort $(OBJS))
	$(Q)$(CC) -o $@ $(OBJS) $(EXTRA_OBJS) \
		-Wl,-Map,$(OUTELF).map \
		$(addprefix -T, $(LD_SCRIPT)) \
		$(LDFLAGS) \
		$(LIBDIRS) \
		$(LIBS)

$(OUTLIB): $(OBJS)
	$(info archiving   $@)
	$(Q)rm -f $@
	$(Q)$(AR) $(ARFLAGS) $@ $^ 1> /dev/null 2>&1

$(OUTDIR)/%.c.o: %.c Makefile $(MAKEFILE_LIST) | $(PREREQUISITES)
	$(info compiling   $<)
	@mkdir -p $(@D)
	$(Q)$(CC) -o $@ -c $< -MMD \
		$(addprefix -D, $(DEFS)) \
		$(addprefix -I, $(INCS)) \
		$(CFLAGS)
$(OUTDIR)/%.s.o: %.s Makefile $(MAKEFILE_LIST) | $(PREREQUISITES)
	$(info assembling  $<)
	@mkdir -p $(@D)
	$(Q)$(CC) -o $@ -c $< -MMD \
		$(addprefix -D, $(DEFS)) \
		$(addprefix -I, $(INCS)) \
		$(CFLAGS)
$(OUTDIR)/%.S.o: %.S Makefile $(MAKEFILE_LIST) | $(PREREQUISITES)
	$(info assembling  $<)
	@mkdir -p $(@D)
	$(Q)$(CC) -x assembler-with-cpp \
		-o $@ -c $< -MMD \
		$(addprefix -D, $(DEFS)) \
		$(addprefix -I, $(INCS)) \
		$(CFLAGS)

$(OUTDEF): $(OUTELF)
	$(info generating  $@)
	$(Q)echo $(sort $(DEFS)) | tr ' ' '\n' > $@
$(OUTSRC): $(OUTELF)
	$(info generating  $@)
	$(Q)echo $(sort $(SRCS)) | tr ' ' '\n' > $@
$(OUTINC): $(OUTELF)
	$(info generating  $@)
	$(Q)echo $(sort $(INCS)) | tr ' ' '\n' > $@

$(OUTELF).size: $(OUTELF)
	$(info generating  $@)
	$(Q)$(NM) -S --size-sort $< > $@
$(OUTELF).dump: $(OUTELF)
	$(info generating  $@)
	$(Q)$(OD) -x $< > $@
$(OUTELF).lst: $(OUTELF)
	$(info generating  $@)
	$(Q)$(OD) -d $< > $@
$(OUTELF).sym: $(OUTELF)
	$(info generating  $@)
	$(Q)$(OD) -t $< | sort > $@
$(OUTBIN): $(OUTELF)
	$(info generating  $@)
	$(Q)$(SZ) $<
	$(Q)$(OC) -O binary $< $@
$(OUTHEX): $(OUTELF)
	$(info generating  $@)
	$(Q)$(OC) -O ihex $< $@

ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), depend)
-include $(DEPS)
endif
endif
