PROJECT := fw-project
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

STATIC_GOALS := flash flash_factory test coverage clean
goals := $(filter-out $(STATIC_GOALS), $(MAKECMDGOALS))

.PHONY: build
build:
	$(Q)$(MAKE) -f projects/rules.mk $(MAKECMDGOALS)
$(goals): build
	@:

.PHONY: flash_factory
flash_factory:
.PHONY: test
test:
	$(Q)$(MAKE) -C tests
.PHONY: coverage
coverage:
	$(Q)$(MAKE) -C tests $@
.PHONY: clean
clean:
	$(Q)rm -rf $(BUILDIR)
	$(Q)$(MAKE) -C tests $@
