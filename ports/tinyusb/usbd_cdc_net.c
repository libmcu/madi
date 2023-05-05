#include "pusb/usbd.h"
#include "tusb.h"
#include "libmcu/board.h"

uint8_t tud_network_mac_address[6];

static int convert_hexstr_to_number(uint8_t *buf, size_t bufsize,
		const char *hexstr, size_t hexstr_len)
{
	size_t o = 0;

	for (size_t i = 0; i < hexstr_len && o < bufsize; i++) {
		char c = hexstr[i];
		char base = '0';

		if (c >= '0' && c <= '9') {
		} else if (c >= 'A' && c <= 'F') {
			base = 'A' - 0xA;
		} else if (c >= 'a' && c <= 'f') {
			base = 'a' - 0xa;
		} else {
			break;
		}

		if (i & 1) {
			buf[o] <<= 4;
			buf[o] |= c - base;
			o++;
		} else {
			buf[o] = c - base;
		}
	}

	return (int)o;
}

static void set_mac_address(void)
{
	if (tud_network_mac_address[0] || tud_network_mac_address[1] ||
			tud_network_mac_address[2] ||
			tud_network_mac_address[3] ||
			tud_network_mac_address[4] ||
			tud_network_mac_address[5]) {
		return;
	}

	const char *serial = board_get_serial_number_string();
	convert_hexstr_to_number(tud_network_mac_address,
			sizeof(tud_network_mac_address),
			serial, strlen(serial));
	tud_network_mac_address[0] |= 0x02U; /* locally administered */
	tud_network_mac_address[0] &= (uint8_t)~0x01U; /* unicast */
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
	return !usbd_cdc_net_received(src, size);
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg)
{
	return (uint16_t)usbd_cdc_net_send_post(ref, dst);
}

void tud_network_init_cb(void)
{
}

int usbd_cdc_net_send(const void *data, uint16_t datasize)
{
	while (1) {
		if (!tud_ready())
			return -8;

		if (tud_network_can_xmit(datasize)) {
			tud_network_xmit(data, 0);
			return 0;
		}

		tud_task();
	}

	return 0;
}

void usbd_cdc_net_received_post(void)
{
	tud_network_recv_renew();
}

void usbd_cdc_net_get_mac(uint8_t mac[6])
{
	set_mac_address();
	memcpy(mac, tud_network_mac_address, 6);
	mac[5] ^= 0x01; /* to distinguish from the host's mac address. */
}

void usbd_cdc_net_init(void)
{
	set_mac_address();
}
