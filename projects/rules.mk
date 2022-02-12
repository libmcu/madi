target-device := $(firstword $(MAKECMDGOALS))

ifneq ($(target-device),)
ifneq ($(strip $(wildcard projects/devices/$(target-device).mk)),)
$(MAKECMDGOALS): build-target
	@:
.PHONY: build-target
build-target:
	$(Q)DEVICE=$(target-device) $(MAKE) -f projects/rules.mk \
		$(filter-out $(target-device), $(MAKECMDGOALS))
done := 1
endif
endif

ifeq ($(done),)

ifeq ($(DEVICE),)
DEVICE := $(DEFAULT_DEVICE)
endif

DEFS += \
	$(DEVICE) \
	BUILD_DATE=$(BUILD_DATE) \
	VERSION_TAG=$(VERSION_TAG) \
	VERSION=$(VERSION)
LIBS +=
LIBDIRS +=

OUTDIR := $(BUILDIR)/$(DEVICE)
OUTELF := $(OUTDIR)/$(DEVICE)
OUTLIB := $(OUTDIR)/lib$(DEVICE).a
OUTBIN := $(OUTDIR)/$(DEVICE).bin
OUTHEX := $(OUTDIR)/$(DEVICE).hex
OUTDEF := $(OUTDIR)/defines.txt
OUTSRC := $(OUTDIR)/sources.txt
OUTINC := $(OUTDIR)/includes.txt

OUTPUT ?= $(OUTELF) $(OUTLIB) $(OUTHEX) $(OUTBIN) $(OUTDEF) $(OUTSRC) $(OUTINC) \
	  $(OUTELF).size $(OUTELF).dump $(OUTELF).lst $(OUTELF).sym

ASMS += $(foreach dir, $(SRCDIRS), $(shell find $(dir) -type f -regex ".*\.s"))
SRCS += $(foreach dir, $(SRCDIRS), $(shell find $(dir) -type f -regex ".*\.c"))
OBJS += $(addprefix $(OUTDIR)/, $(ASMS:%=%.o)) \
	$(addprefix $(OUTDIR)/, $(SRCS:%=%.o))
DEPS += $(OBJS:.o=.d)

-include projects/app.mk
-include projects/devices/$(DEVICE).mk
include projects/toolchain.mk

.DEFAULT_GOAL :=
all:: $(OUTPUT)
	$(info done $(PROJECT)_$(VERSION)-$(DEVICE))

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

.PHONY: clean
clean:
	$(Q)rm -fr $(OUTDIR)

endif # $(done)
