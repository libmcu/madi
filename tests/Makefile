export CPPUTEST_HOME = cpputest
export TEST_BUILDIR ?= build

TESTS := $(shell find runners -type f -regex ".*\.mk")

.PHONY: all test compile gcov debug flags
all: test
test: BUILD_RULE=all
test: $(TESTS)
compile: BUILD_RULE=start
compile: $(TESTS)
gcov: BUILD_RULE=gcov
gcov: $(TESTS)
debug: BUILD_RULE=debug
debug: $(TESTS)
flags: BUILD_RULE=flags
flags: $(TESTS)

.PHONY: $(TESTS)
$(TESTS):
	$(MAKE) -f $@ $(BUILD_RULE)

LCOV_INFO_FILE = $(TEST_BUILDIR)/lcov.info
.PHONY: lcov
$(TEST_BUILDIR)/test_coverage lcov: $(TEST_BUILDIR)
	@lcov \
        --base-directory . \
        --directory . \
        -c -o $(LCOV_INFO_FILE).tmp
	@lcov \
        --remove $(LCOV_INFO_FILE).tmp \
        -o $(LCOV_INFO_FILE) \
        '*cpputest/*' \
        '/usr/*' \
        '*tests/*'
	@genhtml \
        -o test_coverage \
        -t "coverage" \
        --num-spaces 4 \
        $(LCOV_INFO_FILE) \
        -o $@

.PHONY: coverage
coverage: $(TEST_BUILDIR)/test_coverage
	$(Q)open $(TEST_BUILDIR)/test_coverage/index.html
$(TEST_BUILDIR): $(TESTS)

.PHONY: clean
clean:
	$(Q)rm -rf $(TEST_BUILDIR)

.PHONY: install reinstall
install: | $(CPPUTEST_HOME)
	@cd $(CPPUTEST_HOME) && autoreconf -i && ./configure && make
reinstall:
	@cd $(CPPUTEST_HOME) && make
$(CPPUTEST_HOME):
	@git clone -b hack/malloc --single-branch \
		https://github.com/onkwon/cpputest.git $@
