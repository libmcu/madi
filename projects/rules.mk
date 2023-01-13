# SPDX-License-Identifier: Apache-2.0

.DEFAULT_GOAL :=
all: $(OUTPUT)
	$(info done $(PROJECT)_$(VERSION)-$(BOARD))

$(OUTELF): $(OBJS) $(EXTRA_OBJS) $(LD_SCRIPT) $(GENERATED_LIBS)
	$(info linking     $@)
	$(Q)$(SZ) -t --common $(sort $(OBJS))
	$(Q)$(CC) -o $@ $(OBJS) $(EXTRA_OBJS) \
		-Wl,-Map,$(OUTELF).map \
		$(addprefix -T, $(LD_SCRIPT)) \
		$(LDFLAGS) \
		$(addprefix -L, $(LIBDIRS)) \
		-Wl,--whole-archive \
		$(GENERATED_LIBS) \
		$(LIBS) \
		-Wl,--no-whole-archive \
		$(STDLIBS)

$(OUTDIR)/%.c.o: %.c Makefile $(MAKEFILE_LIST) | $(PREREQUISITES)
	$(info compiling   $<)
	@mkdir -p $(@D)
	$(Q)$(CC) -o $@ -c $< -MMD \
		$(addprefix -D, $(DEFS)) \
		$(addprefix -I, $(INCS)) \
		$(CFLAGS)
$(OUTDIR)/%.cpp.o: %.cpp Makefile $(MAKEFILE_LIST) | $(PREREQUISITES)
	$(info compiling   $<)
	@mkdir -p $(@D)
	$(Q)$(CC) -o $@ -c $< -MMD \
		$(addprefix -D, $(DEFS)) \
		$(addprefix -I, $(INCS)) \
		$(CXXFLAGS)
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
DEPS += $(OBJS:.o=.d)
-include $(DEPS)
endif
endif
