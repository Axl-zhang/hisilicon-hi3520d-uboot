/******************************************************************************
*    Copyright (c) 2014 by Hisilicon.
*    All rights reserved.
******************************************************************************/

#include <common.h>
#include <asm/io.h>
#include <linux/mtd/nand.h>
#include <hinfc_common.h>
#include "hifmc100_spi_ids.h"
#include "hifmc100.h"

/*****************************************************************************/
void hifmc100_set_spi_nand_system_clock(struct hifmc_host *host, int clk_en)
{
	unsigned int old_val, regval = readl(CRG_REG_BASE + FMC_CRG29);

	old_val = regval;

	if (!(regval & FMC_CLK_SEL_MASK)) {
		regval &= ~FMC_CLK_SEL_MASK;
		regval |= FMC_CLK_SEL_83M;
	}

	if (clk_en)
		regval |= FMC_CRG29_CLK_EN;
	else
		regval &= ~FMC_CRG29_CLK_EN;

	if (regval != old_val)
		writel(regval, (CRG_REG_BASE + FMC_CRG29));
}

/*****************************************************************************/
void hifmc100_spi_nand_get_best_clock(unsigned int *clock)
{
	int ix;
	int clk_reg;
#define CLK_2X(_clk)	(((_clk) + 1) >> 1)
	unsigned int sysclk[] = {
		CLK_2X(24),	FMC_CLK_SEL_24M,
		CLK_2X(83),	FMC_CLK_SEL_83M,
		CLK_2X(150),	FMC_CLK_SEL_150M,
		0, 0,
	};
#undef CLK_2X

	clk_reg = FMC_CLK_SEL_24M;
	for (ix = 0; sysclk[ix]; ix += 2) {
		if (*clock < sysclk[ix])
			break;
		clk_reg = sysclk[ix + 1];
	}

	*clock = clk_reg;
}

