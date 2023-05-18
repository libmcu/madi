#include "pusb/usbd.h"
#include <pthread.h>
#include <errno.h>
#include "tusb.h"

#define USBD_STACK_SIZE_BYTES		3072U
#define USBD_PERIODIC_CALLBACKS_LEN	3

enum status {
	STATUS_UNKNOWN,
	STATUS_UNPLUGGED,
	STATUS_PLUGGED,
};
static volatile int status;

static struct periodic_callback {
	usbd_periodic_callback_t func;
	void *arg;
} periodic_callbacks[USBD_PERIODIC_CALLBACKS_LEN];

static void run_periodic_callbacks(void)
{
	for (int i = 0; i < USBD_PERIODIC_CALLBACKS_LEN; i++) {
		struct periodic_callback *p = &periodic_callbacks[i];
		if (p->func) {
			(*p->func)(p->arg);
		}
	}
}

static void *usbd_task(void *e)
{
	(void)e;

	while (status) {
		tud_task();
		run_periodic_callbacks();
	}

	pthread_exit(NULL);
	return NULL;
}

void tud_resume_cb(void)
{
}

void tud_suspend_cb(bool remote_wakeup_en)
{
	(void)remote_wakeup_en;
}

void tud_umount_cb(void)
{
	status = STATUS_UNPLUGGED;
}

void tud_mount_cb(void)
{
	status = STATUS_PLUGGED;
}

int usbd_register_periodic_callback(usbd_periodic_callback_t cb, void *arg)
{
	struct periodic_callback *p;

	for (int i = 0; i < USBD_PERIODIC_CALLBACKS_LEN; i++) {
		p = &periodic_callbacks[i];
		if (p->func == NULL) {
			p->func = cb;
			p->arg = arg;
			break;
		}
	}

	if (p->func != cb) {
		return -ENOENT;
	}

	return 0;
}

int usbd_enable(void)
{
	struct sched_param param;
	pthread_attr_t attr;
	pthread_t thread;

	pthread_attr_init(&attr);
	pthread_attr_getschedparam(&attr, &param);
	pthread_attr_setstacksize(&attr, USBD_STACK_SIZE_BYTES);
	pthread_create(&thread, &attr, usbd_task, 0);

	return 0;
}

int usbd_disable(void)
{
	status = STATUS_UNKNOWN;
	return 0;
}

int usbd_init(void)
{
	memset(periodic_callbacks, 0, sizeof(periodic_callbacks));

	usbd_cdc_acm_init();
	usbd_cdc_net_init();

	usbd_port_init();

	tud_init(BOARD_TUD_RHPORT);
	status = STATUS_UNPLUGGED;

	return 0;
}
