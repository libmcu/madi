#include "pusb/usbd_cdc.h"

#include <semaphore.h>

#include "tusb.h"

#include "nrf_sdm.h"
#include "nrf_sdh.h"
#include "nrfx_power.h"

#include "evtloop.h"

#include "libmcu/ringbuf.h"

extern void tusb_hal_nrf_power_event(uint32_t event);

enum status {
	STATUS_UNKNOWN,
	STATUS_UNPLUGGED,
	STATUS_PLUGGED,
};
static volatile int status;

static struct ringbuf rxbuf_handle;
static uint8_t rxbuf[512];
static sem_t rx_event;

static void process_usb_event(void *ctx);
static struct ao_event usb_event = {
	.handler = process_usb_event,
};

static void poll_power_event(void *ctx)
{
	uint32_t soc_evt;
	uint32_t err = sd_evt_get(&soc_evt);

	if (err == NRF_SUCCESS) {
		int32_t usbevt = (soc_evt == NRF_EVT_POWER_USB_DETECTED) ? NRFX_POWER_USB_EVT_DETECTED :
			(soc_evt == NRF_EVT_POWER_USB_POWER_READY) ? NRFX_POWER_USB_EVT_READY :
			(soc_evt == NRF_EVT_POWER_USB_REMOVED) ? NRFX_POWER_USB_EVT_REMOVED : -1;

		if (usbevt >= 0) {
			tusb_hal_nrf_power_event(usbevt);
		}
	}
}

NRF_SDH_STACK_OBSERVER(usb_power_event, NRF_SDH_SOC_STACK_OBSERVER_PRIO) =
{
	.handler   = poll_power_event,
	.p_context = NULL,
};

static void process_usb_event(void *ctx)
{
	(void)ctx;

	if (!status) {
		return;
	}

	tud_task_ext(0, false);

	if (tud_cdc_available()) {
		uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];
		size_t len = (size_t)tud_cdc_read(buf, sizeof(buf));
		ringbuf_write(&rxbuf_handle, buf, len);
		sem_post(&rx_event);
	}

	tud_cdc_write_flush();
}

void USBD_IRQHandler(void)
{
	tud_int_handler(BOARD_TUD_RHPORT);
	evtloop_post(&usb_event);
}

void tud_cdc_rx_cb(uint8_t itf)
{
}

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
}

void tud_resume_cb(void)
{
}

void tud_suspend_cb(bool remote_wakeup_en)
{
}

void tud_umount_cb(void)
{
	status = STATUS_UNPLUGGED;
}

void tud_mount_cb(void)
{
	status = STATUS_PLUGGED;
}

int usbd_cdc_write(const void *data, size_t datasize)
{
	if (!tud_cdc_connected()) {
		return 0;
	}

	const uint8_t *p = (uint8_t *)data;
	size_t offset = 0;

	while (offset < datasize) {
		uint32_t cap;

		while ((cap = tud_cdc_write_available()) == 0) {
			tud_cdc_write_flush();
			tud_task_ext(2, false);
		}

		offset += tud_cdc_write(&p[offset], MIN(cap, datasize - offset));
	}

	return (int)offset;
}

int usbd_cdc_read(void *buf, size_t bufsize)
{
	evtloop_post(&usb_event);

	size_t len = MIN(ringbuf_length(&rxbuf_handle), bufsize);

	if (len == 0) {
		sem_wait(&rx_event);
	}

	return (int)ringbuf_read(&rxbuf_handle, 0, buf, len);
}

int usbd_cdc_enable(void)
{
	return 0;
}

int usbd_cdc_disable(void)
{
	return 0;
}

int usbd_cdc_init(void)
{
	sem_init(&rx_event, 0, 0);
	ringbuf_create_static(&rxbuf_handle, rxbuf, sizeof(rxbuf));

	NVIC_SetPriority(USBD_IRQn, 2);
	uint32_t usb_reg;

	sd_power_usbdetected_enable(true);
	sd_power_usbpwrrdy_enable(true);
	sd_power_usbremoved_enable(true);

	sd_power_usbregstatus_get(&usb_reg);

	if (usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk) {
		tusb_hal_nrf_power_event(0);
	}
	if (usb_reg & POWER_USBREGSTATUS_OUTPUTRDY_Msk) {
		tusb_hal_nrf_power_event(2);
	}

	tud_init(BOARD_TUD_RHPORT);
	status = STATUS_UNPLUGGED;

	return 0;
}
