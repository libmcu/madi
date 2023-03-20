BOARD := usb-dfu-bootloader
PROJECT := $(BOARD)
BASEDIR := $(shell pwd)
ROOTDIR := $(BASEDIR)/../..
BUILDIR := build

CROSS_COMPILE ?= arm-none-eabi

include $(ROOTDIR)/projects/defines.mk
include $(ROOTDIR)/projects/arch/common.mk

TINYUSB_ROOT := $(OUTDIR)/tinyusb
NRFX_ROOT := $(OUTDIR)/nrfx
CMSIS_ROOT := $(OUTDIR)/cmsis

LD_SCRIPT = pca10059.ld
LIBDIRS += $(NRFX_ROOT)/mdk

CFLAGS += \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16
LDFLAGS += \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16

SRCS += \
	gcc_startup_nrf52840.S \
	\
	main.c \
	usb_descriptors.c \
	\
	$(TINYUSB_ROOT)/src/tusb.c \
	$(TINYUSB_ROOT)/src/common/tusb_fifo.c \
	$(TINYUSB_ROOT)/src/device/usbd.c \
	$(TINYUSB_ROOT)/src/device/usbd_control.c \
	$(TINYUSB_ROOT)/src/class/dfu/dfu_device.c \
	$(TINYUSB_ROOT)/src/class/dfu/dfu_rt_device.c \
	$(TINYUSB_ROOT)/src/portable/nordic/nrf5x/dcd_nrf5x.c \
	\
	$(NRFX_ROOT)/drivers/src/nrfx_power.c \
	$(NRFX_ROOT)/drivers/src/nrfx_nvmc.c \
	$(NRFX_ROOT)/mdk/system_nrf52840.c \

INCS += $(BASEDIR) \
	$(OUTDIR) \
	\
	$(TINYUSB_ROOT)/src \
	$(TINYUSB_ROOT)/hw/mcu/nordic \
	$(TINYUSB_ROOT)/hw/mcu/nordic/nrf5x/s140_nrf52_6.1.1_API/include \
	\
	$(NRFX_ROOT) \
	$(NRFX_ROOT)/mdk \
	$(NRFX_ROOT)/hal \
	$(NRFX_ROOT)/drivers/include \
	$(NRFX_ROOT)/drivers/src \
	\
	$(CMSIS_ROOT)/CMSIS/Core/Include \

DEFS += NRF52840_XXAA \
	CFG_TUSB_MCU=OPT_MCU_NRF5X \
	CONFIG_GPIO_AS_PINRESET \

OBJS += $(addprefix $(OUTDIR)/, $(SRCS:%=%.o))

include $(BASEDIR)/../../projects/toolchain.mk
# suppress warning caused by tinyusb and vendor mcu driver
CFLAGS += -Wno-error
include $(BASEDIR)/../../projects/rules.mk

$(SRCS): $(NRFX_ROOT) $(TINYUSB_ROOT) $(CMSIS_ROOT)
$(NRFX_ROOT):
	git clone https://github.com/NordicSemiconductor/nrfx.git $@
$(TINYUSB_ROOT):
	git clone https://github.com/hathach/tinyusb.git $@
$(CMSIS_ROOT):
	git clone https://github.com/ARM-software/CMSIS_5.git $@


.PHONY: clean flash flash_all
flash: $(OUTHEX)
	nrfjprog -f nrf52 --program $< --sectorerase --verify
flash_all: $(BASEDIR)/bootloader.hex
	nrfjprog -f nrf52 --program $< --sectorerase --verify

clean:
	rm -fr $(BUILDIR)
