/**
 * ddr_training_boot.c
 *
 * Copyright (c) 2009-2014, HiSilicon Technologies Co., Ltd.
 * All rights reserved.
 *
 * Special function for ddr training when power up.
 */

#include "ddr_training_impl.h"
#include <ddr_interface.h>

#ifdef DDR_TRAINING_UART_CONFIG
extern void uart_early_put_hex(int hex);
extern void uart_early_putc(int chr);
#endif

/**
 * ddr_result_data_save
 * @ddrtr_result
 * @training
 * @mode
 *
 * Save DDR tarining result.
 */
void ddr_result_data_save(void *ddrtr_result,
		struct training_data *training, unsigned int mode)
{
	/* nothing to do when ddr training on power up */
}

/**
 * ddr_ddrt_get_test_addr
 * @void
 *
 * Get DDRT test address.
 */
int ddr_ddrt_get_test_addr(void)
{
	return DDRT_CFG_TEST_ADDR_BOOT;
}

#ifdef DDR_TRAINING_UART_CONFIG
#ifdef DDR_TRAINING_MINI_LOG_CONFIG
/**
 * ddr_training_error
 * @mask
 * @phy
 * @byte
 * @dq
 *
 * Display DDR training error when boot.
 */
void ddr_training_error(unsigned int mask, unsigned int phy, int byte, int dq)
{
	uart_early_putc('E');
	uart_early_put_hex(mask);
	uart_early_putc('P');
	uart_early_put_hex(phy);
	uart_early_putc('B');
	uart_early_put_hex(byte);
	uart_early_putc('D');
	uart_early_put_hex(dq);
}
void ddr_training_start(void)
{
	uart_early_putc('D');
	uart_early_putc('D');
	uart_early_putc('R');
}
void ddr_training_suc(void)
{
	uart_early_putc('S');
}
#else
/**
 * ddr_training_local_str
 * @void
 *
 * Define string to print.
 */
void ddr_training_local_str(void)
{
	asm volatile(
		"str_wl:\n\t"
		".asciz \"WL\"\n\t"
		".align 2\n\t"

		"str_hwg:\n\t"
		".asciz \"HWG\"\n\t"
		".align 2\n\t"

		"str_gate:\n\t"
		".asciz \"Gate\"\n\t"
		".align 2\n\t"

		"str_ddrt:\n\t"
		".asciz \"DDRT\"\n\t"
		".align 2\n\t"

		"str_hwrd:\n\t"
		".asciz \"HWRD\"\n\t"
		".align 2\n\t"

		"str_mpr:\n\t"
		".asciz \"MPR\"\n\t"
		".align 2\n\t"

		"str_dataeye:\n\t"
		".asciz \"Dataeye\"\n\t"
		".align 2\n\t"

		"str_err:\n\t"
		".asciz \" Err:\"\n\t"
		".align 2\n\t"

		"str_phy:\n\t"
		".asciz \"Phy\"\n\t"
		".align 2\n\t"

		"str_byte:\n\t"
		".asciz \"Byte\"\n\t"
		".align 2\n\t"

		"str_dq:\n\t"
		".asciz \"DQ\"\n\t"
		".align 2\n\t"

		"str_ddrtr_start:\n\t"
		".asciz \"\r\\nDDRTR \"\n\t"
		".align 2\n\t"

		"str_ddrtr_suc:\n\t"
		".asciz \"Suc\"\n\t"
		".align 2\n\t"
		);
}
/**
 * ddr_training_error
 * @mask
 * @phy
 * @byte
 * @dq
 *
 * Display DDR training error when boot.
 */
void ddr_training_error(unsigned int mask, unsigned int phy, int byte, int dq)
{
	uart_early_putc('\r');
	uart_early_putc('\n');
	/* error type */
	switch (mask) {
	case DDR_ERR_WL:
		asm volatile(
				"adr	r0, str_wl\n\t"
				"bl	uart_early_puts"
			    );
		break;
	case DDR_ERR_HW_GATING:
		asm volatile(
				"adr	r0, str_hwg\n\t"
				"bl	uart_early_puts"
			    );
		break;
	case DDR_ERR_GATING:
		asm volatile(
				"adr	r0, str_gate\n\t"
				"bl	uart_early_puts"
			    );
		break;
	case DDR_ERR_DDRT_TIME_OUT:
		asm volatile(
				"adr	r0, str_ddrt\n\t"
				"bl	uart_early_puts"
			    );
		break;
	case DDR_ERR_HW_RD_DATAEYE:
		asm volatile(
				"adr	r0, str_hwrd\n\t"
				"bl	uart_early_puts"
			    );
		break;
	case DDR_ERR_MPR:
		asm volatile(
				"adr	r0, str_mpr\n\t"
				"bl	uart_early_puts"
			    );
		break;
	case DDR_ERR_DATAEYE:
		asm volatile(
				"adr	r0, str_dataeye\n\t"
				"bl	uart_early_puts"
			    );
		break;
	default:
		break;
	}

	/* error string */
	asm volatile(
			"adr	r0, str_err\n\t"
			"bl	uart_early_puts"
		    );

	/* error phy */
	if (0 != phy) {
		asm volatile(
				"adr	r0, str_phy\n\t"
				"bl	uart_early_puts"
			    );
		uart_early_put_hex(phy);
	}

	/* error byte */
	if (-1 != byte) {
		asm volatile(
				"adr	r0, str_byte\n\t"
				"bl	uart_early_puts"
			    );
		uart_early_put_hex(byte);
	}

	/* error dq */
	if (-1 != dq) {
		asm volatile(
				"adr	r0, str_dq\n\t"
				"bl	uart_early_puts"
			    );
		uart_early_put_hex(dq);
	}

}

/**
 * ddr_training_start
 * @void
 *
 * Display DDR training start when boot.
 */
void ddr_training_start(void)
{
	asm volatile(
			"push	{lr}\n\t"
			"adr	r0, str_ddrtr_start\n\t"
			"bl	uart_early_puts\n\t"
			"pop	{lr}"
		    );
}

/**
 * ddr_training_suc
 * @void
 *
 * Display DDR training result when boot.
 */
void ddr_training_suc(void)
{
	asm volatile(
			"push	{lr}\n\t"
			"adr	r0, str_ddrtr_suc\n\t"
			"bl	uart_early_puts\n\t"
			"pop	{lr}"
		    );
}
#endif /* DDR_TRAINING_CUT_CODE_CONFIG */
#else
void ddr_training_error(unsigned int mask, unsigned int phy, int byte, int dq)
{
	return;
}
void ddr_training_suc(void) { return; }
void ddr_training_start(void) { return; }
#endif /* DDR_TRAINING_UART_CONFIG */
