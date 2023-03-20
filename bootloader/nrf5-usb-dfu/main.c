#include <stdint.h>
#include <stdbool.h>

#include "tusb.h"

#include "nrfx.h"
#include "nrfx/hal/nrf_gpio.h"
#include "nrfx/drivers/include/nrfx_power.h"
#include "nrfx/drivers/include/nrfx_uarte.h"
#include "nrfx_nvmc.h"

#include "nrf_sdm.h"
#include "nrf_soc.h"

#define BUTTON_PIN		39

#define APP_BASEADDR		0x2A000UL
#define APP_FLASH_SIZE		0xD6000UL

static volatile uint32_t system_ticks = 0;

// tinyusb function that handles power event (detected, ready, removed)
// We must call it within SD's SOC event handler, or set it as power event handler if SD is not enabled.
extern void tusb_hal_nrf_power_event(uint32_t event);

// nrf power callback, could be unused if SD is enabled or usb is disabled (board_test example)
static void power_event_handler(nrfx_power_usb_evt_t event)
{
	tusb_hal_nrf_power_event((uint32_t)event);
}

static void board_init(void)
{
	nrf_gpio_cfg_input(BUTTON_PIN, NRF_GPIO_PIN_PULLUP);
	SysTick_Config(SystemCoreClock / 1000);

	// Priorities 0, 1, 4 (nRF52) are reserved for SoftDevice
	// 2 is highest for application
	NVIC_SetPriority(USBD_IRQn, 2);

	uint32_t usb_reg;
	uint8_t sd_en = false;
	sd_softdevice_is_enabled(&sd_en);

	if (sd_en) {
		sd_power_usbdetected_enable(true);
		sd_power_usbpwrrdy_enable(true);
		sd_power_usbremoved_enable(true);

		sd_power_usbregstatus_get(&usb_reg);
	} else {
		// Power module init
		const nrfx_power_config_t pwr_cfg = { 0 };
		nrfx_power_init(&pwr_cfg);

		// Register tusb function as USB power handler
		// cause cast-function-type warning
		const nrfx_power_usbevt_config_t config = { .handler = power_event_handler };
		nrfx_power_usbevt_init(&config);

		nrfx_power_usbevt_enable();

		usb_reg = NRF_POWER->USBREGSTATUS;
	}

	if (usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk) {
		tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_DETECTED);
	}
	if (usb_reg & POWER_USBREGSTATUS_OUTPUTRDY_Msk) {
		tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_READY);
	}
}

static void board_uninit(void)
{
	NVIC_DisableIRQ(SysTick_IRQn);
	SysTick->CTRL = 0;

	nrf_gpio_cfg_default(BUTTON_PIN);

	nrfx_power_usbevt_disable();
	nrfx_power_usbevt_uninit();
	nrfx_power_uninit();
}

static bool is_button_pressed(void)
{
	return nrf_gpio_pin_read(BUTTON_PIN) == 0;
}

static uint32_t proc_soc(void)
{
	uint32_t soc_evt;
	uint32_t err = sd_evt_get(&soc_evt);

	if (err == NRF_SUCCESS) {
		/*------------- usb power event handler -------------*/
		int32_t usbevt = (soc_evt == NRF_EVT_POWER_USB_DETECTED) ? NRFX_POWER_USB_EVT_DETECTED :
			(soc_evt == NRF_EVT_POWER_USB_POWER_READY) ? NRFX_POWER_USB_EVT_READY :
			(soc_evt == NRF_EVT_POWER_USB_REMOVED) ? NRFX_POWER_USB_EVT_REMOVED : -1;

		if (usbevt >= 0) {
			tusb_hal_nrf_power_event(usbevt);
		}
	}

	return err;
}

static uint32_t proc_ble(void)
{
	return NRF_ERROR_NOT_FOUND;
}

static __attribute__((naked)) void set_vector_table(uint32_t vect_addr)
{
	(void)vect_addr;
	__asm("svc %0\n" \
		"bx r14\n" \
		:: "I" (0x13) : "r0");
}

static void erase_and_flash(uint16_t blk, const uint8_t *data, uint16_t len)
{
	uint32_t pagesize = nrfx_nvmc_flash_page_size_get();
	uint32_t bytes_occupied = blk * 64UL;
	uint32_t flash_addr = APP_BASEADDR + bytes_occupied;

	if (flash_addr >= APP_BASEADDR + APP_FLASH_SIZE) {
		return;
	}

	if (bytes_occupied % pagesize == 0) {
		nrfx_nvmc_page_erase(flash_addr);
	}

	nrfx_nvmc_bytes_write(flash_addr, data, len);
}

void SysTick_Handler(void)
{
	system_ticks++;
}

void USBD_IRQHandler(void)
{
	tud_int_handler(0);
}

void SD_EVT_IRQHandler(void)
{
	while ((proc_ble() != NRF_ERROR_NOT_FOUND) ||
			(proc_soc() != NRF_ERROR_NOT_FOUND)) {
	}
}

void nrf_error_cb(uint32_t id, uint32_t pc, uint32_t info)
{
	(void)id;
	(void)pc;
	(void)info;
}

void tud_mount_cb(void)
{
}

void tud_unmount_cb(void)
{
}

void tud_suspend_cb(bool remote_wakeup_en)
{
	(void)remote_wakeup_en;
}

void tud_resume_cb(void)
{
}

uint32_t tud_dfu_get_timeout_cb(uint8_t alt, uint8_t state)
{
	if (state == DFU_DNBUSY) {
		return alt == 0 ? 1 : 100;
	} else if (state == DFU_MANIFEST) {
	}

	return 0;
}

void tud_dfu_download_cb(uint8_t alt, uint16_t block_num, uint8_t const* data, uint16_t length)
{
	if (alt != 0) {
		return;
	}

	erase_and_flash(block_num, data, length);
	tud_dfu_finish_flashing(DFU_STATUS_OK);
}

void tud_dfu_manifest_cb(uint8_t alt)
{
	tud_dfu_finish_flashing(DFU_STATUS_OK);
}

uint16_t tud_dfu_upload_cb(uint8_t alt, uint16_t block_num, uint8_t* data, uint16_t length)
{
	(void)alt;
	(void)block_num;
	(void)data;
	(void)length;

	return 0;
}

void tud_dfu_abort_cb(uint8_t alt)
{
	(void)alt;
}

void tud_dfu_detach_cb(void)
{
}

int main(void)
{
	board_init();

	if (is_button_pressed()) {
		tud_init(0);
		while (1) {
			tud_task();
		}
	}

	board_uninit();

	set_vector_table(APP_BASEADDR);
	__set_MSP(((uint32_t *)APP_BASEADDR)[0]);
	__DSB();
	__ISB();
	((void (*)(void))((uint32_t *)APP_BASEADDR)[1])();
}
