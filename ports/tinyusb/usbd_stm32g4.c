#include "pusb/usbd.h"
#include "tusb.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_it.h"

void USB_LP_IRQHandler(void)
{
	tud_int_handler(0);
}

void USB_HP_IRQHandler(void)
{
	tud_int_handler(0);
}

void USBWakeUp_IRQHandler(void)
{
	tud_int_handler(0);
}

int usbd_port_init(void)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {
		.PeriphClockSelection = RCC_PERIPHCLK_USB,
		.UsbClockSelection = RCC_USBCLKSOURCE_PLL,
	};
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		return -1;
	}
	__HAL_RCC_USB_CLK_ENABLE();

	NVIC_SetPriority(USB_HP_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_SetPriority(USB_LP_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_SetPriority(USBWakeUp_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);

	return 0;
}
