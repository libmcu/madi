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
