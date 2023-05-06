#include "pusb/usbd.h"

#include "tusb.h"

#include "nrf_sdm.h"
#include "nrf_sdh.h"
#include "nrfx_power.h"

static void poll_power_event(void *ctx)
{
	uint32_t soc_evt;
	uint32_t err = sd_evt_get(&soc_evt);

	if (err == NRF_SUCCESS) {
		int32_t usbevt = (soc_evt == NRF_EVT_POWER_USB_DETECTED) ? NRFX_POWER_USB_EVT_DETECTED :
			(soc_evt == NRF_EVT_POWER_USB_POWER_READY) ? NRFX_POWER_USB_EVT_READY :
			(soc_evt == NRF_EVT_POWER_USB_REMOVED) ? NRFX_POWER_USB_EVT_REMOVED : -1;

		if (usbevt >= 0) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
			tusb_hal_nrf_power_event(usbevt);
#pragma GCC diagnostic pop
		}
	}
}

NRF_SDH_STACK_OBSERVER(usb_power_event, NRF_SDH_SOC_STACK_OBSERVER_PRIO) =
{
	.handler   = poll_power_event,
	.p_context = NULL,
};

void USBD_IRQHandler(void)
{
	tud_int_handler(BOARD_TUD_RHPORT);
}

int usbd_port_init(void)
{
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
}
