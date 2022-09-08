DEFAULT_DEVICE ?= unknown
ifeq ($(DEVICE),)
DEVICE := $(DEFAULT_DEVICE)
endif

PROJECT := $(DEVICE)
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

include projects/rules.mk

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
