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
	$(TINYUSB_PORT_ROOT)/usbd_nrf5x.c \
	$(TINYUSB_PORT_ROOT)/dcd_nrf5x.c

TINYUSB_INCS := \
	$(TINYUSB_ROOT)/src \
	$(TINYUSB_ROOT)/lib/networking \
	$(TINYUSB_PORT_ROOT)

$(addprefix $(OUTDIR)/, $(TINYUSB_SRCS:%=%.o)): CFLAGS+=-Wno-error
