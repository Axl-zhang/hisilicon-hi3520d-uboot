/******************************************************************************
*    Copyright (c) 2009-2012 by Hisi.
*    All rights reserved.
******************************************************************************/

#include <common.h>
#include <asm/io.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <asm/errno.h>
#include <asm/arch/platform.h>

#include "../spi_ids.h"
#include "hifmc100.h"

#define GD_SPI_CMD_RDSR1		0x35	/* Read Status Register-1 */

/*****************************************************************************/
/*
   enable QE bit if QUAD read write is supported by GD "25qxxx" SPI
*/
static int spi_gd25qxxx_qe_enable(struct hifmc_spi *spi)
{
	unsigned char config, status, op;
	unsigned int regval;
	const char *str[] = {"Disable", "Enable"};
	struct hifmc_host *host = (struct hifmc_host *)spi->host;

	op = spi_is_quad(spi);

	QE_MSG("\t* Start GD SPI nor %s Quad.\n", str[op]);

	config = spi_general_get_flash_register(spi, GD_SPI_CMD_RDSR1);
	QE_MSG("\t  Read GD SR-1[%#x], val: %#x\n", GD_SPI_CMD_RDSR1, config);
	if (SPI_NOR_GET_QE_BY_CR(config) == op) {
		QE_MSG("\t* Quad was %sd, status:%#x\n", str[op], config);
		return op;
	}

	/* First, we enable/disable QE for 16Pin GD flash, use WRSR[01h] cmd */
	QE_MSG("\t  First, 16Pin GD flash %s Quad.\n", str[op]);

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

	config = spi_general_get_flash_register(spi, GD_SPI_CMD_RDSR1);
	QE_MSG("\t  Read GD SR-1[%#x], val: %#x\n", GD_SPI_CMD_RDSR1, config);
	if (SPI_NOR_GET_QE_BY_CR(config) == op) {
		QE_MSG("\t  16P %s Quad success reg:%#x\n", str[op], config);
		goto qe_end;
	} else
		QE_MSG("\t  16P %s Quad failed, reg:%#x\n", str[op], config);

	/* Second, we enable/disable QE for 8Pin GD flash, use WRSR2[31h] cmd */
	QE_MSG("\t  Second, 8Pin GD flash %s Quad.\n", str[op]);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	QE_MSG("\t  Read Status Register[%#x]:%#x\n", SPI_CMD_RDSR, status);
	if (!(status & STATUS_WEL_MASK))
		spi->driver->write_enable(spi);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDSR2);
	QE_MSG("\t  Read SR-2[%#x], val: %#x\n", SPI_CMD_RDSR2, config);
	if (op)
		config |= SPI_NOR_CR_QE_MASK;
	else
		config &= ~SPI_NOR_CR_QE_MASK;
	writeb(config, host->iobase);
	QE_MSG("\t  Write IO[%#x]%#x\n", (unsigned int)host->iobase,
			*(unsigned char *)host->iobase);

	regval = FMC_CMD_CMD1(SPI_CMD_WRSR2);
	hifmc_write(host, FMC_CMD, regval);
	QE_MSG("\t  Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = FMC_OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	QE_MSG("\t  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(SPI_NOR_CR_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	QE_MSG("\t  Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATE_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	QE_MSG("\t  Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	spi->driver->wait_ready(spi);

	config = spi_general_get_flash_register(spi, SPI_CMD_RDSR2);
	QE_MSG("\t  Read GD SR-2[%#x], val: %#x\n", SPI_CMD_RDSR2, config);
	if (SPI_NOR_GET_QE_BY_CR(config) == op)
		QE_MSG("\t  8P %s Quad success, reg[%#x].\n", str[op], config);
	else
		DBG_MSG("Error: %s Quad failed, reg[%#x]\n", str[op], config);

qe_end:

	QE_MSG("\t* End GD SPI nor %s Quad end.\n", str[op]);

	return op;
}

