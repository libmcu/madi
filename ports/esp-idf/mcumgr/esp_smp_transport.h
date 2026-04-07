/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef ESP_SMP_TRANSPORT_H
#define ESP_SMP_TRANSPORT_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include "libmcu/mgmt_transport.h"

/**
 * @brief Initialise the ESP SMP transport.
 *
 * Installs the USB-JTAG (or UART) driver and starts the receive task.
 * The physical medium is selected at compile time via CONFIG_ESP_CONSOLE_*:
 *   CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG → usb_serial_jtag driver
 *   otherwise                          → uart_vfs / stdin VFS
 *
 * @param[in] on_recv  Called for each complete decoded SMP packet.
 * @param[in] ctx      Opaque context forwarded to @p on_recv.
 * @return 0 on success, negative errno on failure.
 */
int esp_smp_transport_init(mgmt_transport_rx_callback_t on_recv, void *ctx);

/**
 * @brief Transmit a raw SMP packet (Base64 + CRC framing applied internally).
 *
 * @param[in] data  SMP packet bytes.
 * @param[in] len   Packet length in bytes.
 * @return 0 on success, -1 on failure.
 */
int esp_smp_transport_send(const void *data, size_t len);

/**
 * @brief Stop the transport and release its resources.
 */
void esp_smp_transport_deinit(void);

#if defined(__cplusplus)
}
#endif

#endif /* ESP_SMP_TRANSPORT_H */
