#include "pusb/usbd.h"
#include <semaphore.h>
#include "tusb.h"
#include "libmcu/ringbuf.h"
#include "libmcu/board.h"

enum status {
	STATUS_UNKNOWN,
	STATUS_UNPLUGGED,
	STATUS_PLUGGED,
};
static volatile int status;

static struct ringbuf cdc_acm_rxhandle;
static uint8_t cdc_acm_rxbuf[512];
static sem_t cdc_acm_rxevt;

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

static void process_cdc_acm(void)
{
	if (tud_cdc_available()) {
		uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];
		size_t len = (size_t)tud_cdc_read(buf, sizeof(buf));
		ringbuf_write(&cdc_acm_rxhandle, buf, len);
		sem_post(&cdc_acm_rxevt);
	}

	tud_cdc_write_flush();
}

static void process_cdc_net(void)
{
	netif_usb_step();
}

static void process_usb_event(void)
{
	if (!status) {
		return;
	}

	tud_task_ext(0, false);

	process_cdc_acm();
	process_cdc_net();
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
	size_t len = MIN(ringbuf_length(&cdc_acm_rxhandle), bufsize);

	if (len == 0) {
		sem_wait(&cdc_acm_rxevt);
	}

	return (int)ringbuf_read(&cdc_acm_rxhandle, 0, buf, len);
}

int usbd_step(void)
{
	process_usb_event();
	return 0;
}

int usbd_enable(void)
{
	return 0;
}

int usbd_disable(void)
{
	return 0;
}

int usbd_init(void)
{
	sem_init(&cdc_acm_rxevt, 0, 0);
	ringbuf_create_static(&cdc_acm_rxhandle,
			cdc_acm_rxbuf, sizeof(cdc_acm_rxbuf));

	if (!tud_network_mac_address[0] && !tud_network_mac_address[1] &&
			!tud_network_mac_address[2] &&
			!tud_network_mac_address[3] &&
			!tud_network_mac_address[4] &&
			!tud_network_mac_address[5]) {
		const char *serial = board_get_serial_number_string();
		convert_hexstr_to_number(tud_network_mac_address,
				sizeof(tud_network_mac_address),
				serial, strlen(serial));
		tud_network_mac_address[0] |= 0x02; /* locally administered */
		tud_network_mac_address[0] &= ~0x01; /* unicast */
		tud_network_mac_address[5] ^= 0x01; /* to distinguish from the serial number */
	}

	usbd_port_init();

	tud_init(BOARD_TUD_RHPORT);
	status = STATUS_UNPLUGGED;

	return 0;
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
	return !netif_usb_input(src, size);
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg)
{
	return (uint16_t)netif_usb_clean_output(ref, dst);
}

void tud_network_init_cb(void)
{
	netif_usb_clean_input(0);
}

int netif_usb_output(const void *data, uint16_t datasize)
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

void netif_usb_input_done(void)
{
	tud_network_recv_renew();
}

void netif_usb_get_mac(void *iface, uint8_t mac[6])
{
	(void)iface;
	memcpy(mac, tud_network_mac_address, 6);
}
