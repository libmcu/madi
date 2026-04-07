/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "console_sync.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <errno.h>

#if defined(CONFIG_USE_SEGGER_RTT)
#include <SEGGER_RTT.h>
#elif defined(__ZEPHYR__)
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#endif

#if defined(__ZEPHYR__)
#if DT_HAS_CHOSEN(zephyr_console)
#define CONSOLE_SYNC_HAS_ZEPHYR_CONSOLE	1
#else
#define CONSOLE_SYNC_HAS_ZEPHYR_CONSOLE	0
#endif
#else
#define CONSOLE_SYNC_HAS_ZEPHYR_CONSOLE	0
#endif

static pthread_mutex_t mutex;

static void lock_console(void)
{
	pthread_mutex_lock(&mutex);
}

static void unlock_console(void)
{
	pthread_mutex_unlock(&mutex);
}

int console_sync_write(const void *data, size_t datasize)
{
	lock_console();

	int rc = 0;

#if defined(__ZEPHYR__) && !CONSOLE_SYNC_HAS_ZEPHYR_CONSOLE
	const uint8_t *ptr = (const uint8_t *)data;
	for (size_t i = 0; i < datasize; i++) {
		printk("%c", ptr[i]);
	}
	rc = (int)datasize;
#else
	rc = (int)fwrite(data, 1, datasize, stdout);

	if (rc >= 0 && datasize > 0) {
		fflush(stdout);
#if defined(__ZEPHYR__) || !defined(TARGET_PLATFORM_madi_nrf52840)
		if (!isatty(fileno(stdout))) {
			fsync(fileno(stdout));
		}
#endif
	}
#endif
	unlock_console();

	return rc;
}

int console_sync_read(void *buf, size_t bufsize)
{
	lock_console();
	int rc = 0;
#if defined(CONFIG_USE_SEGGER_RTT)
	rc = (int)SEGGER_RTT_Read(0, buf, bufsize);
#elif defined(__ZEPHYR__) && CONSOLE_SYNC_HAS_ZEPHYR_CONSOLE
	const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	if (!device_is_ready(uart)) {
		rc = -ENODEV;
		goto out;
	}
	uint8_t *ptr = (uint8_t *)buf;
	for (size_t i = 0; i < bufsize; i++) {
		int c = uart_poll_in(uart, &ptr[i]);
		if (c < 0) {
		    break;
		}
		rc++;
	}
out:
#elif defined(__ZEPHYR__)
	rc = -ENODEV;
#else
	rc = (int)fread(buf, 1, bufsize, stdin);
#endif
	unlock_console();
	return rc;
}

int console_sync_init(void)
{
	/* Disable buffering on stdin */
	setvbuf(stdin, NULL, _IONBF, 0);

	/* Enable non-blocking mode on stdin and stdout */
	fcntl(fileno(stdout), F_SETFL, 0 /*| O_NONBLOCK*/);
	fcntl(fileno(stdin), F_SETFL, 0 /*| O_NONBLOCK*/);

	pthread_mutex_init(&mutex, NULL);

#if defined(CONFIG_USE_SEGGER_RTT)
	volatile uint32_t *dhcsr = (uint32_t*)0xE000EDF0;
	if (*dhcsr & 1) {
		SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0,
				SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
	} else {
		SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0,
				SEGGER_RTT_MODE_NO_BLOCK_SKIP);
	}
#endif

	return 0;
}

int console_sync_deinit(void)
{
	pthread_mutex_destroy(&mutex);
	return 0;
}
