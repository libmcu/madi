# SPDX-License-Identifier: Apache-2.0

PROJECT := $(BOARD)
BASEDIR := $(shell pwd)
BUILDIR := build

VERBOSE ?= 0
V ?= $(VERBOSE)
ifeq ($(V), 0)
	Q = @
else
	Q =
endif
export PROJECT
export BASEDIR
export BUILDIR
export Q

include projects/version.mk
ifneq ($(BOARD),)
include projects/defines.mk
include projects/boards/$(BOARD).mk
include projects/sources.mk
include projects/app.mk
ifneq ($(PLATFORM_SPECIFIC_MAKE),)
include $(PLATFORM_SPECIFIC_MAKE)
endif
include projects/toolchain.mk
include projects/rules.mk
else
all:
	$(info Specify BOARD or target)
endif

.PHONY: test
test:
	$(Q)$(MAKE) -C tests
.PHONY: coverage
coverage:
	$(Q)$(MAKE) -C tests $@
.PHONY: clean
clean:
	$(Q)rm -fr $(BUILDIR)
	$(Q)$(MAKE) -C tests $@
.PHONY: docs
docs:
	@LD_LIBRARY_PATH=$$(llvm-config --libdir) $(MAKE) -C $@ html
.PHONY: version
version:
	$(info $(VERSION_TAG), $(VERSION))
