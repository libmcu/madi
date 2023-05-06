#include "pusb/usbd.h"
#include <semaphore.h>
#include "tusb.h"
#include "libmcu/ringbuf.h"
#include "libmcu/board.h"

#if !defined(MIN)
#define MIN(a, b)	(((a) > (b))? (b) : (a))
#endif

static struct ringbuf cdc_acm_rxhandle;
static uint8_t cdc_acm_rxbuf[512];
static sem_t cdc_acm_rxevt;

void tud_cdc_rx_cb(uint8_t itf)
{
}

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
}

int usbd_cdc_acm_write(const void *data, size_t datasize)
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

	tud_cdc_write_flush();

	return (int)offset;
}

int usbd_cdc_acm_read(void *buf, size_t bufsize)
{
	size_t len = MIN(ringbuf_length(&cdc_acm_rxhandle), bufsize);

	if (len == 0) {
		sem_wait(&cdc_acm_rxevt);
	}

	return (int)ringbuf_read(&cdc_acm_rxhandle, 0, buf, len);
}

void usbd_cdc_acm_step(void)
{
	if (tud_cdc_available()) {
		uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];
		size_t len = (size_t)tud_cdc_read(buf, sizeof(buf));
		ringbuf_write(&cdc_acm_rxhandle, buf, len);
		sem_post(&cdc_acm_rxevt);
	}

	tud_cdc_write_flush();
}

void usbd_cdc_acm_init(void)
{
	sem_init(&cdc_acm_rxevt, 0, 0);
	ringbuf_create_static(&cdc_acm_rxhandle,
			cdc_acm_rxbuf, sizeof(cdc_acm_rxbuf));
}
