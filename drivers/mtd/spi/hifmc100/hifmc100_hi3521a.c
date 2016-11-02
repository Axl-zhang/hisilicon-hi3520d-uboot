/******************************************************************************
*    Copyright (c) 2009-2011 by Hisilicon.
*    All rights reserved.
******************************************************************************/


/* bit[7]	0: 3 Bytes address mode  1: 4Bytes address mode */
#define FMC_BOOT_MODE_MASK			(0x1 << 7)

#define GET_FMC_BOOT_MODE ({ \
	int boot_mode = 0; \
	boot_mode = readl(SYS_CTRL_REG_BASE + FMC_ADDR_MODE_REG); \
	boot_mode &= FMC_BOOT_MODE_MASK; \
	boot_mode; })

/*****************************************************************************/
static void hifmc100_set_spi_nor_system_clock(struct hifmc_host *host,
	struct spi_operation *op, int clk_en)
{
	unsigned old, regval = readl(CRG_REG_BASE + FMC_CRG29);

	old = regval;

	regval &= ~FMC_CLK_SEL_MASK;
	regval |= FMC_CLK_SEL_24M;		/* Default Clock */

	if (op && op->clock) {
		regval &= ~FMC_CLK_SEL_MASK;
		regval |= op->clock & FMC_CLK_SEL_MASK;
	}

	if (clk_en)
		regval |= FMC_CRG29_CLK_EN;
	else
		regval &= ~FMC_CRG29_CLK_EN;

	if (regval != old)
		writel(regval, (CRG_REG_BASE + FMC_CRG29));
}

/*****************************************************************************/
static void hifmc100_spi_nor_get_best_clock(unsigned int *clock)
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

