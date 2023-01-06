# SPDX-License-Identifier: Apache-2.0

OUTPUT = $(OUTDIR)/$(BOARD).elf

CROSS_COMPILE ?= xtensa-esp32-elf

IDF_PATH ?= external/esp-idf
SDK_ROOT := $(IDF_PATH)
PORT_ROOT := ports/esp-idf

.PHONY: $(OUTPUT)
$(OUTPUT): $(OUTDIR)/esp-idf
	$(info building    $@)
	$(Q)cmake --build $(OUTDIR)
	$(Q)python $(SDK_ROOT)/tools/idf_size.py $(OUTDIR)/$(BOARD).map
$(OUTDIR)/esp-idf: $(PORT_ROOT)
	$(info preparing   $@)
	$(Q)cmake $(BASEDIR) -B$(OUTDIR) \
		-DESP_PLATFORM=1 \
		-DBOARD=$(BOARD) \
		-GNinja

.PHONY: flash erase monitor
flash: all
	$(Q)python $(SDK_ROOT)/components/esptool_py/esptool/esptool.py \
		--chip $(BOARD) \
		--port $(PORT) \
		--baud 921600 \
		--before default_reset \
		--after hard_reset write_flash -z \
		--flash_mode dio \
		--flash_freq 80m \
		--flash_size detect \
		0x0 $(OUTDIR)/bootloader/bootloader.bin \
		0x8000 $(OUTDIR)/partition_table/partition-table.bin \
		0xd000 $(OUTDIR)/ota_data_initial.bin \
		0x10000 $(OUTDIR)/$(BOARD).bin
erase:
	$(Q)python $(SDK_ROOT)/components/esptool_py/esptool/esptool.py \
		--chip $(BOARD) \
		--port $(PORT) \
		--baud 921600 \
		--before default_reset \
		$@_flash
monitor:
	$(Q)$(SDK_ROOT)/tools/idf_monitor.py $(OUTPUT) \
		--port $(PORT)

PORT ?= /dev/tty.usbmodem1101
