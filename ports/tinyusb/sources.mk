# SPDX-License-Identifier: MIT

TINYUSB_ROOT := $(BASEDIR)/external/tinyusb

TINYUSB_SRCS := \
	$(TINYUSB_ROOT)/src/tusb.c \
	$(TINYUSB_ROOT)/src/common/tusb_fifo.c \
	$(TINYUSB_ROOT)/src/device/usbd.c \
	$(TINYUSB_ROOT)/src/device/usbd_control.c \
	$(TINYUSB_ROOT)/src/class/cdc/cdc_device.c \
	$(TINYUSB_ROOT)/src/class/net/ecm_rndis_device.c \
	$(TINYUSB_ROOT)/lib/networking/dhserver.c \
	$(TINYUSB_ROOT)/lib/networking/dnserver.c \
	$(TINYUSB_ROOT)/lib/networking/rndis_reports.c \
	$(TINYUSB_PORT_ROOT)/descriptors.c \
	$(TINYUSB_PORT_ROOT)/usbd.c \
	$(TINYUSB_PORT_ROOT)/usbd_cdc_acm.c \
	$(TINYUSB_PORT_ROOT)/usbd_cdc_net.c \

ifeq ($(BOARD), madi_esp32)
TINYUSB_SRCS += \
	$(TINYUSB_ROOT)/src/portable/synopsys/dwc2/dcd_dwc2.c \
	$(TINYUSB_PORT_ROOT)/usbd_esp32.c \
	$(TINYUSB_PORT_ROOT)/netif_usb_esp32.c
else ifeq ($(BOARD), madi_nrf52)
TINYUSB_SRCS += \
	$(TINYUSB_PORT_ROOT)/usbd_nrf5x.c \
	$(TINYUSB_PORT_ROOT)/dcd_nrf5x.c \
	$(TINYUSB_PORT_ROOT)/netif_usb.c
else ifeq ($(BOARD), madi_stm32)
TINYUSB_SRCS += \
	$(TINYUSB_ROOT)/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c \
	$(TINYUSB_PORT_ROOT)/usbd_stm32g4.c \
	$(TINYUSB_PORT_ROOT)/netif_usb.c
else
$(error "BOARD not specified")
endif

TINYUSB_INCS := \
	$(TINYUSB_ROOT)/src \
	$(TINYUSB_ROOT)/lib/networking \
	$(TINYUSB_PORT_ROOT)

$(addprefix $(OUTDIR)/, $(TINYUSB_SRCS:%=%.o)): CFLAGS+=-Wno-error
