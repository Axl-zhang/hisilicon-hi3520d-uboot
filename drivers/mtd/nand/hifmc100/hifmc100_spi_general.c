/*****************************************************************************
 *    Copyright (c) 2014 by Hisilicon
 *    All rights reserved.
 *****************************************************************************/

#include <asm/io.h>

/*****************************************************************************/
/*
  Send set/get features command to SPI Nand flash
*/
unsigned char spi_nand_feature_op(struct hifmc_op *spi, unsigned char op,
				unsigned char addr, unsigned char val)
{
	unsigned reg;
	const char *str[] = {"Get", "Set"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	hifmc_write(host, FMC_INT_CLR, FMC_INT_CLR_OP_DONE);

	if ((GET_OP == op) && (STATUS_ADDR == addr)) {
		SR_MSG("\t  |-* Start Get Status\n");

		reg = FMC_OP_CFG_FM_CS(host->cmd_option.chipselect);
		hifmc_write(host, FMC_OP_CFG, reg);
		SR_MSG("\t    |-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

		reg = FMC_OP_READ_STATUS_EN(ENABLE) | FMC_OP_REG_OP_START;
		hifmc_write(host, FMC_OP, reg);
		SR_MSG("\t    |-Set OP[%#x]%#x\n", FMC_OP, reg);

		FMC_CMD_WAIT_CPU_FINISH(host);

		val = hifmc_read(host, FMC_STATUS);
		SR_MSG("\t  |-* End Get Status, result: %#x\n", val);
		return val;
	}

	FT_MSG("\t  |-* Start %s Feature[%#x]\n", str[op], addr);

	hifmc100_ecc0_switch(host, ENABLE);

	reg = FMC_CMD_CMD1(op ? SPI_CMD_SET_FEATURE : SPI_CMD_GET_FEATURES);
	hifmc_write(host, FMC_CMD, reg);
	FT_MSG("\t    |-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	hifmc_write(host, FMC_ADDRL, addr);
	FT_MSG("\t    |-Set ADDRL[%#x]%#x\n", FMC_ADDRL, addr);

	reg = FMC_OP_CFG_FM_CS(host->cmd_option.chipselect)
		| FMC_OP_CFG_ADDR_NUM(FEATURES_OP_ADDR_NUM);
	hifmc_write(host, FMC_OP_CFG, reg);
	FT_MSG("\t    |-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_DATA_NUM_CNT(FEATURES_DATE_NUM);
	hifmc_write(host, FMC_DATA_NUM, reg);
	FT_MSG("\t    |-Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_ADDR_EN(ENABLE)
		| FMC_OP_REG_OP_START;

	if (SET_OP == op) {
		reg |= FMC_OP_WRITE_DATE_EN(ENABLE);
		writeb(val, host->iobase);
		FT_MSG("\t    |-Write IO[%#x]%#x\n", (unsigned)host->iobase,
				*(unsigned char *)host->iobase);
	} else
		reg |= FMC_OP_READ_DATE_EN(ENABLE);

	hifmc_write(host, FMC_OP, reg);
	FT_MSG("\t    |-Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	if (GET_OP == op) {
		val = readb(host->iobase);
		FT_MSG("\t    |-Read IO[%#x]%#x\n", (unsigned)host->iobase,
				*(unsigned char *)host->iobase);
	}

	hifmc100_ecc0_switch(host, DISABLE);

	FT_MSG("\t  |-* End %s Feature[%#x]\n", str[op], addr);

	return val;
}

/*****************************************************************************/
/*
  Read status[C0H]:[0]bit OIP, judge whether the device is busy or not
*/
static int spi_general_wait_ready(struct hifmc_op *spi)
{
	unsigned char status;
	unsigned int deadline = 1 << 15;

	do {
		status = spi_nand_feature_op(spi, GET_OP, STATUS_ADDR, 0);
		if (!(status & STATUS_OIP_MASK)) {
			if (status & (STATUS_E_FAIL_MASK | STATUS_P_FAIL_MASK))
				return status;
			return 0;
		}

		udelay(1);

	} while (deadline--);

	DBG_MSG("Error: spi nand wait ready timeout, status: %#x\n", status);

	return 1;
}

/*****************************************************************************/
/*
  Send write enable command to SPI Nand, status[C0H]:[2]bit WEL must be set 1
*/
static int spi_general_write_enable(struct hifmc_op *spi)
{
	unsigned int reg;
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	WE_MSG("\t  |-* Start Write Enable\n");

	reg = spi_nand_feature_op(spi, GET_OP, STATUS_ADDR, 0);
	if (reg & STATUS_WEL_MASK) {
		DBG_MSG("Warning: Write Enable was opened! reg: %#x\n", reg);
		return 0;
	}

	reg = hifmc_read(host, FMC_GLOBAL_CFG);
	if (reg & FMC_GLOBAL_CFG_WP_ENABLE) {
		reg &= ~FMC_GLOBAL_CFG_WP_ENABLE;
		hifmc_write(host, FMC_GLOBAL_CFG, reg);
		WE_MSG("\t    |-Set GLOBAL_CFG[%#x]%#x\n", FMC_GLOBAL_CFG, reg);
	}

	reg = FMC_CMD_CMD1(SPI_CMD_WREN);
	hifmc_write(host, FMC_CMD, reg);
	WE_MSG("\t    |-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = FMC_OP_CFG_FM_CS(host->cmd_option.chipselect);
	hifmc_write(host, FMC_OP_CFG, reg);
	WE_MSG("\t    |-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_OP_CMD1_EN(ENABLE) | FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	WE_MSG("\t    |-Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	if (WE_DBG) {
		spi->driver->wait_ready(spi);

		reg = spi_nand_feature_op(spi, GET_OP, STATUS_ADDR, 0);
		if (reg & STATUS_WEL_MASK)
			WE_MSG("\t    |-Write Enable success. reg: %#x\n", reg);
		else {
			DBG_MSG("Error: Write Enable failed! reg: %#x\n", reg);
			return reg;
		}
	}

	WE_MSG("\t  |-* End Write Enable\n");
	return 0;
}

/*****************************************************************************/
/*
  judge whether SPI Nand support QUAD read/write or not
*/
static int spi_is_quad(struct hifmc_op *spi)
{
	if (QE_DBG) {
		DBG_MSG("hifmc: SPI write iftype:%d\n", spi->write->iftype);
		DBG_MSG("hifmc: SPI read iftype:%d\n", spi->read->iftype);
	}

	if (IF_TYPE_QUAD == spi->write->iftype
		|| IF_TYPE_QUAD == spi->read->iftype
		|| IF_TYPE_QIO == spi->read->iftype)
		return 1;

	return 0;
}

/*****************************************************************************/
/*
  Send set features command to SPI Nand, feature[B0H]:[0]bit QE would be set
*/
static int spi_general_qe_enable(struct hifmc_op *spi)
{
	unsigned reg, op;
	const char *str[] = {"Disable", "Enable"};

	op = spi_is_quad(spi);

	QE_MSG("* SPI Nand Quad %s start.\n", str[op]);

	reg = spi_nand_feature_op(spi, GET_OP, FEATURE_ADDR, 0);
	QE_MSG("  Read Quad feature[%#x]%#x\n", FEATURE_ADDR, reg);
	if ((reg & FEATURE_QE_ENABLE) == op) {
		QE_MSG("* Quad was %sd!\n", str[op]);
		return op;
	}

	if (op == ENABLE)
		reg |= FEATURE_QE_ENABLE;
	else
		reg &= ~FEATURE_QE_ENABLE;

	spi_nand_feature_op(spi, SET_OP, FEATURE_ADDR, reg);
	QE_MSG("  SPI Nand %s Quad\n", str[op]);

	spi->driver->wait_ready(spi);

	reg = spi_nand_feature_op(spi, GET_OP, FEATURE_ADDR, 0);
	if ((reg & FEATURE_QE_ENABLE) == op)
		QE_MSG("* SPI Nand %s Quad succeed!\n", str[op]);
	else
		DBG_MSG("Error: %s Quad failed! reg[%#x]\n", str[op], reg);

	return op;
}

