#include "pusb/usbd.h"
#include <pthread.h>
#include "tusb.h"

#define USBD_STACK_SIZE_BYTES		2048U

enum status {
	STATUS_UNKNOWN,
	STATUS_UNPLUGGED,
	STATUS_PLUGGED,
};
static volatile int status;

static void *usbd_task(void *e)
{
	(void)e;

	while (status) {
		tud_task();

		usbd_cdc_acm_step();
		usbd_cdc_net_step();
	}

	pthread_exit(NULL);
	return NULL;
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
	usbd_cdc_acm_init();
	usbd_cdc_net_init();
	usbd_port_init();

	tud_init(BOARD_TUD_RHPORT);
	status = STATUS_UNPLUGGED;

	return 0;
}
