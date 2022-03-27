OUTPUT = $(OUTDIR)/$(DEVICE).elf

CROSS_COMPILE ?= xtensa-esp32-elf

IDF_PATH ?= external/esp-idf
SDK_ROOT := $(IDF_PATH)
PORT_ROOT := ports/esp32

.PHONY: $(OUTPUT)
$(OUTPUT): $(OUTDIR)/esp-idf
	$(info building    $@)
	$(Q)cmake --build $(OUTDIR)
	$(Q)python $(SDK_ROOT)/tools/idf_size.py $(OUTDIR)/$(DEVICE).map
$(OUTDIR)/esp-idf: $(PORT_ROOT)/esp-idf
	$(info preparing   $@)
	$(Q)cmake $< -B$(OUTDIR) \
		-DCMAKE_TOOLCHAIN_FILE=$(IDF_PATH)/tools/cmake/toolchain-esp32.cmake \
		-DBASEDIR=$(BASEDIR) \
		-DOUTPUT=$(DEVICE) \
		-GNinja

.PHONY: flash erase monitor
flash: all
	$(Q)python $(SDK_ROOT)/components/esptool_py/esptool/esptool.py \
		--chip esp32 \
		--port $(PORT) \
		--baud 921600 \
		--before default_reset \
		--after hard_reset write_flash -z \
		--flash_mode dio \
		--flash_freq 40m \
		--flash_size detect \
		0x1000 $(OUTDIR)/bootloader/bootloader.bin \
		0x8000 $(OUTDIR)/partition_table/partition-table.bin \
		0xd000 $(OUTDIR)/ota_data_initial.bin \
		0x10000 $(OUTDIR)/$(DEVICE).bin
erase:
	$(Q)python $(SDK_ROOT)/components/esptool_py/esptool/esptool.py \
		--chip esp32 \
		--port $(PORT) \
		--baud 921600 \
		--before default_reset \
		$@
monitor:
	$(Q)$(SDK_ROOT)/tools/idf_monitor.py $(OUTPUT) \
		--port $(PORT)

PORT ?= /dev/tty.usbserial-0001
