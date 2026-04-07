/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/board.h"

#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
#include <zephyr/version.h>

#include <stdio.h>
#include <string.h>

#include "libmcu/compiler.h"

LOG_MODULE_REGISTER(usb, LOG_LEVEL_INF);

#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
/* New USB stack. Uses usbd_* APIs instead of deprecated usb_enable(). */
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/usbd_msg.h>

/* USB VID/PID for MADI device */
#define MADI_USB_VID	0x1209
#define MADI_USB_PID	0x3000

USBD_DEVICE_DEFINE(madi_usbd,
		DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)),
		MADI_USB_VID,
		MADI_USB_PID);
/* String Descriptors */
USBD_DESC_LANG_DEFINE(madi_lang);
USBD_DESC_MANUFACTURER_DEFINE(madi_mfr, "libmcu");
USBD_DESC_PRODUCT_DEFINE(madi_product, "MADI");
USBD_DESC_SERIAL_NUMBER_DEFINE(madi_sn);
#if KERNEL_VERSION_MAJOR >= 4
/* NCS 4.x only: configuration descriptor node (not in upstream 3.7) */
USBD_DESC_CONFIG_DEFINE(madi_fs_cfg_desc, "FS Configuration");
#endif
/* API Differences between NCS and upstream:
 * - NCS 4.x: USBD_CONFIGURATION_DEFINE takes 4 args (includes desc_nd)
 * - Upstream 3.7: USBD_CONFIGURATION_DEFINE takes 3 args
 * Detection: KERNEL_VERSION_MAJOR >= 4 means NCS, < 4 means upstream */
#if KERNEL_VERSION_MAJOR >= 4
/* NCS 4.x - 4 args: name, attrib, power, desc_nd */
USBD_CONFIGURATION_DEFINE(madi_fs_config,
		USB_SCD_SELF_POWERED | USB_SCD_REMOTE_WAKEUP,
		100, /* bMaxPower: 100 * 2mA = 200mA */
		&madi_fs_cfg_desc);
#else
/* Upstream Zephyr 3.7 - 3 args: name, attrib, power */
USBD_CONFIGURATION_DEFINE(madi_fs_config,
		USB_SCD_SELF_POWERED | USB_SCD_REMOTE_WAKEUP,
		100); /* bMaxPower: 100 * 2mA = 200mA */
#endif

static void on_usbd_msg(struct usbd_context *const ctx,
		const struct usbd_msg *const msg)
{
	switch (msg->type) {
	case USBD_MSG_VBUS_READY:
		LOG_INF("USB VBUS ready");
		if (usbd_can_detect_vbus(ctx)) {
			int err = usbd_enable(ctx);
			if (err) {
				LOG_ERR("usbd_enable failed: %d", err);
			}
		}
		break;
	case USBD_MSG_VBUS_REMOVED:
		LOG_INF("USB disconnected");
		usbd_disable(ctx);
		break;
#if KERNEL_VERSION_MAJOR >= 4
	case USBD_MSG_CONFIGURATION:
		LOG_INF("USB configured");
		break;
#endif
	case USBD_MSG_CDC_ACM_CONTROL_LINE_STATE:
		LOG_INF("USB CDC ACM ready");
		break;
	case USBD_MSG_SUSPEND:
		LOG_INF("USB suspended");
		break;
	case USBD_MSG_RESUME:
		LOG_INF("USB resumed");
		break;
	case USBD_MSG_RESET:
		LOG_INF("USB reset");
		break;
	default:
		break;
	}
}

static int init_usb_new_stack(void)
{
	int ret;

	/* Add string descriptors */
	ret = usbd_add_descriptor(&madi_usbd, &madi_lang);
	if (ret) {
		LOG_ERR("Failed to add language descriptor (err %d)", ret);
		return ret;
	}

	ret = usbd_add_descriptor(&madi_usbd, &madi_mfr);
	if (ret) {
		LOG_ERR("Failed to add manufacturer descriptor (err %d)", ret);
		return ret;
	}

	ret = usbd_add_descriptor(&madi_usbd, &madi_product);
	if (ret) {
		LOG_ERR("Failed to add product descriptor (err %d)", ret);
		return ret;
	}

	ret = usbd_add_descriptor(&madi_usbd, &madi_sn);
	if (ret) {
		LOG_ERR("Failed to add serial descriptor (err %d)", ret);
		return ret;
	}

	/* Add configuration */
	ret = usbd_add_configuration(&madi_usbd, USBD_SPEED_FS, &madi_fs_config);
	if (ret) {
		LOG_ERR("Failed to add USB configuration (err %d)", ret);
		return ret;
	}

	/* API Differences between NCS and upstream:
	 * - NCS 4.x: usbd_register_all_classes takes 4 args(includes blocklist)
	 * - Upstream 3.7: usbd_register_all_classes takes 3 args
	 * Detection: KERNEL_VERSION_MAJOR >= 4 means NCS, < 4 means upstream */
	/* Register all available classes (CDC ACM will be auto-registered) */
#if KERNEL_VERSION_MAJOR >= 4
	/* NCS 4.x - 4 args: ctx, speed, cfg, blocklist */
	ret = usbd_register_all_classes(&madi_usbd, USBD_SPEED_FS, 1, NULL);
#else
	/* Upstream Zephyr 3.7 - 3 args: ctx, speed, cfg */
	ret = usbd_register_all_classes(&madi_usbd, USBD_SPEED_FS, 1);
#endif
	if (ret) {
		LOG_ERR("Failed to register USB classes (err %d)", ret);
		return ret;
	}

	/* Register message callback for status events */
	ret = usbd_msg_register_cb(&madi_usbd, on_usbd_msg);
	if (ret) {
		LOG_ERR("Failed to register message callback (err %d)", ret);
		return ret;
	}

	/* Initialize USB device */
	ret = usbd_init(&madi_usbd);
	if (ret) {
		LOG_ERR("Failed to initialize USB (err %d)", ret);
		return ret;
	}

	/* On nRF52840, the UDC driver can detect VBUS presence.
	 * In that case, usbd_enable() must be called from the VBUS_READY
	 * callback (on_usbd_msg) to avoid a UsageFault during USB init.
	 * Only call usbd_enable() directly if VBUS detection is unavailable. */
	if (!usbd_can_detect_vbus(&madi_usbd)) {
		LOG_INF("No VBUS detection, enabling USB directly");
		ret = usbd_enable(&madi_usbd);
		if (ret) {
			LOG_ERR("Failed to enable USB (err %d)", ret);
		}
	}

	return ret;
}

#elif defined(CONFIG_USB_DEVICE_STACK)
/* Legacy USB stack (USB_DEVICE_STACK) - older Zephyr versions
 * Uses deprecated but still functional usb_enable() API.
 * Kept for backward compatibility with older Zephyr versions. */
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/usb/usb_dc.h>

static void on_usb_status(enum usb_dc_status_code status, const uint8_t *param)
{
	ARG_UNUSED(param);
	switch (status) {
	case USB_DC_CONNECTED:
		LOG_INF("USB connected");
		break;
	case USB_DC_CONFIGURED:
		LOG_INF("USB CDC ACM ready");
		break;
	case USB_DC_DISCONNECTED:
		LOG_INF("USB disconnected");
		break;
	case USB_DC_SUSPEND:
		LOG_INF("USB suspended");
		break;
	case USB_DC_RESUME:
		LOG_INF("USB resumed");
		break;
	default:
		break;
	}
}

static int init_usb_legacy_stack(void)
{
	int ret = usb_enable(on_usb_status);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB (err %d)", ret);
	}
	return ret;
}

#else
#error "No USB device stack configured. Enable USB_DEVICE_STACK_NEXT or USB_DEVICE_STACK."
#endif

void board_init(void)
{
	int ret;

#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
	ret = init_usb_new_stack();
#elif defined(CONFIG_USB_DEVICE_STACK)
	ret = init_usb_legacy_stack();
#endif

	if (ret != 0) {
		LOG_ERR("Failed to initialize USB (err %d)", ret);
	}
}

void board_reboot(void)
{
	sys_reboot(SYS_REBOOT_COLD);
}

board_reboot_reason_t board_get_reboot_reason(void)
{
	uint32_t cause = 0;

	hwinfo_get_reset_cause(&cause);
	hwinfo_clear_reset_cause();

	if (cause & RESET_WATCHDOG) {
		return BOARD_REBOOT_WDT;
	} else if (cause & RESET_PIN) {
		return BOARD_REBOOT_PIN;
	} else if (cause & RESET_SOFTWARE) {
		return BOARD_REBOOT_SOFT;
	} else if (cause & RESET_POR) {
		return BOARD_REBOOT_POWER;
	} else if (cause & RESET_DEBUG) {
		return BOARD_REBOOT_DEBUGGER;
	} else if (cause & RESET_BROWNOUT) {
		return BOARD_REBOOT_BROWNOUT;
	}

	return BOARD_REBOOT_UNKNOWN;
}

const char *board_get_serial_number_string(void)
{
	static char sn[17];

	if (sn[0] == '\0') {
		uint8_t dev_id[8];
		ssize_t len = hwinfo_get_device_id(dev_id, sizeof(dev_id));

		if (len > 0) {
			snprintf(sn, sizeof(sn),
				"%02x%02x%02x%02x%02x%02x%02x%02x",
				dev_id[0], dev_id[1], dev_id[2], dev_id[3],
				dev_id[4], dev_id[5], dev_id[6], dev_id[7]);
		} else {
			strncpy(sn, "unknown", sizeof(sn) - 1);
		}
	}

	return sn;
}

const char *board_name(void)
{
	return "madi_nrf52840";
}

LIBMCU_NO_INSTRUMENT
uint32_t board_get_time_since_boot_ms(void)
{
	return k_uptime_get_32();
}

LIBMCU_NO_INSTRUMENT
uint64_t board_get_time_since_boot_us(void)
{
	return (uint64_t)k_uptime_get() * 1000U;
}

LIBMCU_NO_INSTRUMENT
void *board_get_current_thread(void)
{
	return k_current_get();
}

uint32_t board_random(void)
{
	return sys_rand32_get();
}
