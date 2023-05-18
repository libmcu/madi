#include "pusb/usbd.h"

#include "rom/gpio.h"
#include "driver/gpio.h"
#include "hal/gpio_ll.h"
#include "hal/usb_hal.h"
#include "soc/usb_periph.h"

#include "esp_idf_version.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include "esp_private/periph_ctrl.h"
#else
#include "driver/periph_ctrl.h"
#endif

static void configure_pins(usb_hal_context_t *usb)
{
	/* usb_periph_iopins currently configures USB_OTG as USB Device.
	 * Introduce additional parameters in usb_hal_context_t when adding
	 * support for USB Host. */
	for (const usb_iopin_dsc_t *iopin = usb_periph_iopins;
			iopin->pin != -1; ++iopin) {
		if ((usb->use_external_phy) || (iopin->ext_phy_only == 0)) {
			esp_rom_gpio_pad_select_gpio(iopin->pin);
			if (iopin->is_output) {
				esp_rom_gpio_connect_out_signal(iopin->pin,
						iopin->func, false, false);
			} else {
				esp_rom_gpio_connect_in_signal(iopin->pin,
						iopin->func, false);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
				if ((iopin->pin != GPIO_MATRIX_CONST_ZERO_INPUT) && (iopin->pin != GPIO_MATRIX_CONST_ONE_INPUT)) {
#else
				if ((iopin->pin != GPIO_FUNC_IN_LOW) && (iopin->pin != GPIO_FUNC_IN_HIGH)) {
#endif

					gpio_ll_input_enable(&GPIO, iopin->pin);
				}
			}
			esp_rom_gpio_pad_unhold(iopin->pin);
		}
	}

	if (!usb->use_external_phy) {
		gpio_set_drive_capability(USBPHY_DM_NUM, GPIO_DRIVE_CAP_3);
		gpio_set_drive_capability(USBPHY_DP_NUM, GPIO_DRIVE_CAP_3);
	}
}

int usbd_port_init(void)
{
	periph_module_reset(PERIPH_USB_MODULE);
	periph_module_enable(PERIPH_USB_MODULE);

	usb_hal_context_t hal = {
		.use_external_phy = false,
	};
	usb_hal_init(&hal);
	configure_pins(&hal);

	return 0;
}
