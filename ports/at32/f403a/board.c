/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "at32f403a_407.h"

/**
  * @brief  system clock config program
  * @note   the system clock is configured as follow:
  *         - system clock        = hext / 2 * pll_mult
  *         - system clock source = pll (hext)
  *         - hext                = 8000000
  *         - sclk                = 240000000
  *         - ahbdiv              = 1
  *         - ahbclk              = 240000000
  *         - apb2div             = 2
  *         - apb2clk             = 120000000
  *         - apb1div             = 2
  *         - apb1clk             = 120000000
  *         - pll_mult            = 60
  *         - pll_range           = GT72MHZ (greater than 72 mhz)
  * @param  none
  * @retval none
  */
static void initialize_clock(void)
{
	crm_reset();
	crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);
	while (crm_hext_stable_wait() == ERROR) { } /* wait till hext is ready */

	/* config pll clock resource */
	crm_pll_config(CRM_PLL_SOURCE_HEXT_DIV, CRM_PLL_MULT_60, CRM_PLL_OUTPUT_RANGE_GT72MHZ);

	crm_hext_clock_div_set(CRM_HEXT_DIV_2); /* config hext division */

	crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE); /* enable pll */

	while (crm_flag_get(CRM_PLL_STABLE_FLAG) != SET) { } /* wait till pll is ready */

	crm_ahb_div_set(CRM_AHB_DIV_1); /* config ahbclk */
	crm_apb2_div_set(CRM_APB2_DIV_2); /* config apb2clk */
	crm_apb1_div_set(CRM_APB1_DIV_2); /* config apb1clk */

	crm_auto_step_mode_enable(TRUE); /* enable auto step mode */

	crm_sysclk_switch(CRM_SCLK_PLL); /* select pll as system clock source */

	/* wait till pll is used as system clock source */
	while (crm_sysclk_switch_status_get() != CRM_SCLK_PLL) { }

	crm_auto_step_mode_enable(FALSE); /* disable auto step mode */

	system_core_clock_update(); /* update system_core_clock global variable */
}

void board_init(void)
{
	initialize_clock();
}
