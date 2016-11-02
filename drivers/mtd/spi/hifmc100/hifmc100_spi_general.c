/*****************************************************************************
 *    Copyright (c) 2009-2011 by Hisilicon
 *    All rights reserved.
 *****************************************************************************/

#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <asm/errno.h>
#include <asm/arch/platform.h>

#include "../spi_ids.h"
#include "hifmc100.h"

/*****************************************************************************/
u_char spi_general_get_flash_register(struct hifmc_spi *spi, u_char cmd)
{
	unsigned char status;
	unsigned int regval;
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	SR_MSG("\t * Start get flash Register[%#x]\n", cmd);

	regval = FMC_OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	SR_MSG("\t   Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	if (cmd == SPI_CMD_RDSR) {
		regval = FMC_OP_READ_STATUS_EN(ENABLE) | FMC_OP_REG_OP_START;
		goto cmd_config_done;
	}

	hifmc_write(host, FMC_CMD, cmd);
	SR_MSG("\t   Set CMD[%#x]%#x\n", FMC_CMD, cmd);

	regval = FMC_DATA_NUM_CNT(SPI_NOR_CR_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	SR_MSG("\t   Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_READ_DATE_EN(ENABLE)
		| FMC_OP_REG_OP_START;
cmd_config_done:
	hifmc_write(host, FMC_OP, regval);
	SR_MSG("\t   Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	if (cmd == SPI_CMD_RDSR)
		status = hifmc_read(host, FMC_STATUS);
	else
		status = readb(host->iobase);
	SR_MSG("\t * End get flash Register[%#x], val:%#x\n", cmd, status);

	return status;
}

/*****************************************************************************/
static int spi_general_wait_ready(struct hifmc_spi *spi)
{
	unsigned char status;
	unsigned int deadline = 1 << 20;

	do {
		status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
		if (!(status & SPI_NOR_SR_WIP_MASK))
			return 0;

		udelay(1);

	} while (deadline--);

	DBG_MSG("Error: spi nor wait ready timeout, status[%#x]\n", status);

	return 1;
}

/*****************************************************************************/
static int spi_general_write_enable(struct hifmc_spi *spi)
{
	unsigned char status;
	unsigned int reg;
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	if (WE_DBG)
		printf("\n");
	WE_MSG("\t  * Start Write Enable\n");

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	WE_MSG("\t    Read Status Register[%#x]:%#x\n", SPI_CMD_RDSR, status);
	if (status & STATUS_WEL_MASK) {
		DBG_MSG("Warning: Write Enable was opened! reg: %#x\n", status);
		return 0;
	}

	reg = hifmc_read(host, FMC_GLOBAL_CFG);
	if (reg & FMC_GLOBAL_CFG_WP_ENABLE) {
		reg &= ~FMC_GLOBAL_CFG_WP_ENABLE;
		hifmc_write(host, FMC_GLOBAL_CFG, reg);
		WE_MSG("\t    Set GLOBAL_CFG[%#x]%#x\n", FMC_GLOBAL_CFG, reg);
	}

	reg = FMC_CMD_CMD1(SPI_CMD_WREN);
	hifmc_write(host, FMC_CMD, reg);
	WE_MSG("\t    Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = FMC_OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, reg);
	WE_MSG("\t    Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_OP_CMD1_EN(ENABLE) | FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	WE_MSG("\t    Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

#if WE_DBG
	spi->driver->wait_ready(spi);

	reg = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	if (reg & STATUS_WEL_MASK)
		WE_MSG("\t    Write Enable success.reg: %#x\n", reg);
	else {
		DBG_MSG("Error: Write Enable failed! status: %#x\n",
				status);
		return status;
	}
#endif

	WE_MSG("\t  * End Write Enable\n");

	return 0;
}

/*****************************************************************************/
/*
  enable 4byte address for SPI which memory more than 16M
*/
static int spi_general_entry_4addr(struct hifmc_spi *spi, int enable)
{
	unsigned char status;
	unsigned int regval;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	AC_MSG("\t* Start SPI Nor flash %s 4-byte mode.\n", str[enable]);

	if (spi->addrcycle != SPI_4BYTE_ADDR_LEN) {
		AC_MSG("\t* Flash isn't support entry 4-byte mode.\n");
		return 0;
	}

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR3);
	AC_MSG("\t  Read Status Register-3[%#x]:%#x\n", SPI_CMD_RDSR3, status);
	if (SPI_NOR_GET_4BYTE_BY_CR(status) == enable) {
		AC_MSG("\t* 4-byte was %sd, reg:%#x\n", str[enable], status);
		return 0;
	}

	if (enable)
		regval = SPI_CMD_EN4B;
	else
		regval = SPI_CMD_EX4B;
	hifmc_write(host, FMC_CMD, FMC_CMD_CMD1(regval));
	AC_MSG("\t  Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = FMC_OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	AC_MSG("\t  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_OP_CMD1_EN(ENABLE) | FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	AC_MSG("\t  Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	host->set_host_addr_mode(host, enable);

	if (AC_DBG) {
		spi->driver->wait_ready(spi);

		status = spi_general_get_flash_register(spi, SPI_CMD_RDSR3);
		AC_MSG("\t  Read SR-3[%#x]:%#x\n", SPI_CMD_RDSR3, status);
		if (SPI_NOR_GET_4BYTE_BY_CR(status) == enable)
			AC_MSG("\t  %s 4-byte success, SR3:%#x\n", str[enable],
					status);
		else
			DBG_MSG("Error: %s 4-byte failed! SR3:%#x\n",
					str[enable], status);
	}

	AC_MSG("\t* End SPI Nor flash %s 4-byte mode.\n", str[enable]);

	return 0;
}

/*****************************************************************************/
/*
  judge whether SPI Nor support QUAD read write or not
*/
int spi_is_quad(struct hifmc_spi *spi)
{
	const char *str[] = {"STD", "DUAL", "DIO", "QUAD", "QIO"};

	QE_MSG("\t* SPI read iftype: %s write iftype: %s\n",
			str[spi->read->iftype], str[spi->write->iftype]);

	if ((IF_TYPE_QUAD == spi->read->iftype)
			|| (IF_TYPE_QIO == spi->read->iftype)
			|| (IF_TYPE_QIO == spi->write->iftype)
			|| (IF_TYPE_QIO == spi->write->iftype))
		return 1;

	return 0;
}

/*****************************************************************************/
/*
   enable QE bit if QUAD read write is supported by SPI
*/
static int spi_general_qe_enable(struct hifmc_spi *spi)
{
	unsigned char status, config, op;
	unsigned int regval;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	op = spi_is_quad(spi);

	QE_MSG("\t* Start SPI Nor %s Quad.\n", str[op]);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDCR);
	QE_MSG("\t  Read Config Register[%#x]%#x\n", SPI_CMD_RDCR, config);
	if (SPI_NOR_GET_QE_BY_CR(config) == op) {
		QE_MSG("\t* Quad was %sd, config:%#x\n", str[op], config);
		return op;
	}

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	QE_MSG("\t  Read Status Register[%#x]%#x\n", SPI_CMD_RDSR, status);

	spi->driver->write_enable(spi);

	if (op)
		config |= SPI_NOR_CR_QE_MASK;
	else
		config &= ~SPI_NOR_CR_QE_MASK;
	writeb(status, host->iobase);
	writeb(config, host->iobase + SPI_NOR_SR_LEN);
	QE_MSG("\t  Write IO[%#x]%#x\n", (unsigned int)host->iobase,
			*(unsigned short *)host->iobase);

	regval = FMC_CMD_CMD1(SPI_CMD_WRSR);
	hifmc_write(host, FMC_CMD, regval);
	QE_MSG("\t  Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = FMC_OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	QE_MSG("\t  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(SPI_NOR_SR_LEN + SPI_NOR_CR_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	QE_MSG("\t  Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATE_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	QE_MSG("\t  Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDCR);
	if (SPI_NOR_GET_QE_BY_CR(config) == op)
		QE_MSG("\t  %s Quad success, config:%#x\n", str[op], config);
	else
		DBG_MSG("Error: %s Quad failed! reg:%#x\n", str[op], config);

	QE_MSG("\t* End SPI Nor %s Quad.\n", str[op]);

	return op;
}

/*****************************************************************************/
/*
  some chip don't QUAD enable
*/
static int spi_do_not_qe_enable(struct hifmc_spi *spi)
{
	return 0;
}

