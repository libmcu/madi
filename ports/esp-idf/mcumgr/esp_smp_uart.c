/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "esp_smp_transport.h"

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/usb_serial_jtag.h"
#include "driver/uart_vfs.h"
#include "mbedtls/base64.h"
#include "esp_log.h"
#include "mgmt/mgmt.h"
#include "libmcu/crc16.h"

#define TAG "smp_transport"

#define SMP_RX_BUF_SIZE     2048U
#define SMP_TX_BUF_SIZE     (MGMT_MAX_MTU + MGMT_HDR_SIZE + 4U)
#define SMP_BASE64_BUF_SIZE ((SMP_TX_BUF_SIZE * 4U / 3U) + 4U)
#define SMP_BASE64_LINE_MAX 128U
#define SMP_LINE_BUF_SIZE   (2U + SMP_BASE64_LINE_MAX + 1U)
#if !CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
#define SMP_UART_DRV_BUF    2048U
#define SMP_UART_NUM        0
#endif

#define SMP_FRAME_START_1   0x06U
#define SMP_FRAME_START_2   0x09U
#define SMP_FRAME_CONT_1    0x04U
#define SMP_FRAME_CONT_2    0x14U
#define SMP_FRAME_END       0x0AU

enum rx_state {
	WAIT_HDR1,
	WAIT_HDR2,
	PAYLOAD,
};

struct esp_smp_ctx {
	mgmt_transport_rx_callback_t on_recv;
	void                        *recv_ctx;
	TaskHandle_t                 rx_task;
	/* RX */
	uint8_t                      rx_frame[SMP_RX_BUF_SIZE];
	size_t                       rx_frame_len;
	uint8_t                      decoded[SMP_RX_BUF_SIZE];
	/* TX */
	uint8_t                      tx_frame[SMP_TX_BUF_SIZE];
	uint8_t                      tx_base64[SMP_BASE64_BUF_SIZE];
	uint8_t                      tx_line[SMP_LINE_BUF_SIZE];
};

static struct esp_smp_ctx s_ctx;

static int phy_write(const uint8_t *data, size_t len)
{
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
	int n = usb_serial_jtag_write_bytes(data, len, portMAX_DELAY);
#else
	int n = uart_write_bytes((uart_port_t)SMP_UART_NUM,
			(const char *)data, len);
#endif
	return (n == (int)len) ? 0 : -1;
}

int esp_smp_transport_send(const void *data, size_t len)
{
	struct esp_smp_ctx *ctx = &s_ctx;

	if (len > MGMT_MAX_MTU + MGMT_HDR_SIZE) {
		return -1;
	}

	/* Raw frame: [len_hi][len_lo][payload...][crc_hi][crc_lo] */
	ctx->tx_frame[0] = (uint8_t)(len >> 8);
	ctx->tx_frame[1] = (uint8_t)(len & 0xFFU);
	memcpy(ctx->tx_frame + 2, data, len);
	uint16_t crc = crc16_xmodem(data, len);
	ctx->tx_frame[2 + len]     = (uint8_t)(crc >> 8);
	ctx->tx_frame[2 + len + 1] = (uint8_t)(crc & 0xFFU);

	size_t total_raw = 2U + len + 2U;
	size_t base64_len = 0;
	mbedtls_base64_encode(ctx->tx_base64, sizeof(ctx->tx_base64),
			&base64_len, ctx->tx_frame, total_raw);

	size_t offset = 0;
	bool first = true;
	while (offset < base64_len) {
		size_t chunk = base64_len - offset;
		if (chunk > SMP_BASE64_LINE_MAX) {
			chunk = SMP_BASE64_LINE_MAX;
		}
		size_t line_len = 0;
		ctx->tx_line[line_len++] = first ? SMP_FRAME_START_1 : SMP_FRAME_CONT_1;
		ctx->tx_line[line_len++] = first ? SMP_FRAME_START_2 : SMP_FRAME_CONT_2;
		memcpy(ctx->tx_line + line_len, ctx->tx_base64 + offset, chunk);
		line_len += chunk;
		ctx->tx_line[line_len++] = SMP_FRAME_END;
		if (phy_write(ctx->tx_line, line_len) != 0) {
			return -1;
		}
		first = false;
		offset += chunk;
	}
	return 0;
}

static bool process_rx_frame(struct esp_smp_ctx *ctx)
{
	if (ctx->rx_frame_len == 0U || (ctx->rx_frame_len & 0x3U) != 0U) {
		return false;
	}

	size_t decoded_len = 0;
	int ret = mbedtls_base64_decode(ctx->decoded, sizeof(ctx->decoded),
			&decoded_len, ctx->rx_frame, ctx->rx_frame_len);
	if (ret != 0 || decoded_len < 4U) {
		return false;
	}

	uint16_t pkt_len = ((uint16_t)ctx->decoded[0] << 8) | ctx->decoded[1];
	if (decoded_len < (size_t)pkt_len + 2U) {
		/* Partial packet — continuation frames still pending. */
		return false;
	}
	if (decoded_len != (size_t)pkt_len + 2U) {
		ctx->rx_frame_len = 0;
		return false;
	}

	const uint8_t *pkt_data = ctx->decoded + 2;
	if (crc16_xmodem(pkt_data, pkt_len) != 0U) {
		ESP_LOGE(TAG, "CRC mismatch");
		ctx->rx_frame_len = 0;
		return true;
	}

	if (pkt_len < 2U) {
		ctx->rx_frame_len = 0;
		return true;
	}

	if (ctx->on_recv) {
		ctx->on_recv(pkt_data, pkt_len - 2U, ctx->recv_ctx);
	}

	ctx->rx_frame_len = 0;
	return true;
}

static void rx_task(void *param)
{
	struct esp_smp_ctx *ctx = (struct esp_smp_ctx *)param;
	uint8_t byte;
	enum rx_state state = WAIT_HDR1;
	uint8_t expected_hdr2 = 0;
	bool line_is_start = false;
#if !CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
	const int stdin_fd = fileno(stdin);
#endif

	while (1) {
		int rc;
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
		rc = usb_serial_jtag_read_bytes(&byte, 1, portMAX_DELAY);
#else
		rc = read(stdin_fd, &byte, 1);
#endif
		if (rc != 1) {
			continue;
		}

		switch (state) {
		case WAIT_HDR1:
			if (byte == SMP_FRAME_START_1) {
				expected_hdr2 = SMP_FRAME_START_2;
				line_is_start = true;
				state = WAIT_HDR2;
			} else if (byte == SMP_FRAME_CONT_1) {
				expected_hdr2 = SMP_FRAME_CONT_2;
				line_is_start = false;
				state = WAIT_HDR2;
			}
			break;

		case WAIT_HDR2:
			if (byte == expected_hdr2) {
				if (line_is_start) {
					ctx->rx_frame_len = 0;
				} else if (ctx->rx_frame_len == 0U) {
					state = WAIT_HDR1;
					break;
				}
				state = PAYLOAD;
			} else {
				state = WAIT_HDR1;
			}
			break;

		case PAYLOAD:
			if (byte == SMP_FRAME_END) {
				(void)process_rx_frame(ctx);
				state = WAIT_HDR1;
				break;
			}
			if (ctx->rx_frame_len < sizeof(ctx->rx_frame)) {
				ctx->rx_frame[ctx->rx_frame_len++] = byte;
			} else {
				ESP_LOGE(TAG, "frame overflow");
				ctx->rx_frame_len = 0;
				state = WAIT_HDR1;
			}
			break;

		default:
			state = WAIT_HDR1;
			break;
		}
	}
}

int esp_smp_transport_init(mgmt_transport_rx_callback_t on_recv, void *ctx)
{
	s_ctx.on_recv  = on_recv;
	s_ctx.recv_ctx = ctx;

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
	if (!usb_serial_jtag_is_driver_installed()) {
		usb_serial_jtag_driver_config_t cfg =
				USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
		esp_err_t err = usb_serial_jtag_driver_install(&cfg);
		if (err != ESP_OK) {
			ESP_LOGE(TAG, "usb_serial_jtag_driver_install: %s",
					esp_err_to_name(err));
			return -1;
		}
	}
#else
	if (!uart_is_driver_installed((uart_port_t)SMP_UART_NUM)) {
		esp_err_t err = uart_driver_install((uart_port_t)SMP_UART_NUM,
				SMP_UART_DRV_BUF, SMP_UART_DRV_BUF, 0, NULL, 0);
		if (err != ESP_OK) {
			ESP_LOGE(TAG, "uart_driver_install: %s",
					esp_err_to_name(err));
			return -1;
		}
	}
	uart_vfs_dev_use_driver(SMP_UART_NUM);
	int flags = fcntl(fileno(stdin), F_GETFL, 0);
	if (flags >= 0 && (flags & O_NONBLOCK)) {
		fcntl(fileno(stdin), F_SETFL, flags & ~O_NONBLOCK);
	}
	uart_vfs_dev_port_set_rx_line_endings(SMP_UART_NUM, ESP_LINE_ENDINGS_LF);
	uart_vfs_dev_port_set_tx_line_endings(SMP_UART_NUM, ESP_LINE_ENDINGS_LF);
#endif

	xTaskCreate(rx_task, "smp_rx", 8192, &s_ctx, 5, &s_ctx.rx_task);
	return 0;
}

void esp_smp_transport_deinit(void)
{
	if (s_ctx.rx_task) {
		vTaskDelete(s_ctx.rx_task);
		s_ctx.rx_task = NULL;
	}
}
