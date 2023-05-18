#include "pusb/usbd.h"
#include "tusb.h"

static usbd_cdc_net_tx_handler_t call_on_tx;
static usbd_cdc_net_rx_handler_t call_on_rx;

bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
	bool rc = true;

	if (call_on_rx) {
		rc = !!(*call_on_rx)(src, size);
	}
	tud_network_recv_renew();

	return rc;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg)
{
	uint16_t rc = 0;

	if (call_on_tx) {
		rc = (uint16_t)(*call_on_tx)(ref, dst);
	}

	return rc;
}

void tud_network_init_cb(void)
{
}

int usbd_cdc_net_send(const void *data, uint16_t datasize,
		usbd_cdc_net_tx_handler_t cb)
{
	call_on_tx = cb;

	while (1) {
		if (!tud_ready())
			return -8;

		if (tud_network_can_xmit(datasize)) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
			tud_network_xmit(data, 0);
#pragma GCC diagnostic pop
			return 0;
		}

		tud_task();
	}

	return 0;
}

void usbd_cdc_net_register_rx_handler(usbd_cdc_net_rx_handler_t handler)
{
	call_on_rx = handler;
}

void usbd_cdc_net_init(void)
{
}
