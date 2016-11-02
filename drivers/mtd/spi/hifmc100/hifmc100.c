/******************************************************************************
 *    Copyright (c) 2009-2010 by Hisilicon
 *    All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
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
static int start_up_addr_mode;
static struct hifmc_host fmc_host;
static struct mtd_info_ex spi_info_ex = {.type = 0, };

#ifdef CONFIG_HI3521A
	#include "hifmc100_hi3521a.c"
#endif

/*****************************************************************************/
static void debug_register_dump(void)
{
	int ix;
	unsigned int regbase = CONFIG_HIFMC_REG_BASE_ADDRESS;

	printf("Register dump:");
	for (ix = 0; ix <= 0x98; ix += 0x04) {
		if (!(ix & 0x0F))
			printf("\n0x%08X: ", (regbase + ix));
		printf("%08X ", readl(regbase + ix));
	}
	printf("\n");
}

/*****************************************************************************/
static void hifmc100_set_host_addr_mode(struct hifmc_host *host, int enable)
{
	unsigned int regval = hifmc_read(host, FMC_CFG);

	/* 1: SPI_NOR_ADDR_MODE_4_BYTES  0: SPI_NOR_ADDR_MODE_3_BYTES */
	if (enable)
		regval |= SPI_NOR_ADDR_MODE_MASK;
	else
		regval &= ~SPI_NOR_ADDR_MODE_MASK;

	hifmc_write(host, FMC_CFG, regval);
}

/*****************************************************************************/
static void hifmc100_shutdown(void)
{
	if (start_up_addr_mode == SPI_NOR_ADDR_MODE_3_BYTES) {
		int ix;
		struct hifmc_host *host = &fmc_host;
		struct hifmc_spi *spi = host->spi;

		for (ix = 0; ix < host->num_chip; ix++, spi++) {
			if (spi->addrcycle == 4) {
				spi->driver->wait_ready(spi);
				spi->driver->entry_4addr(spi, DISABLE);
			}
		}
	}
}

/*****************************************************************************/
static void hifmc100_map_iftype_and_clock(struct hifmc_spi *spi)
{
	int ix;
	const int iftype_read[] = {
		SPI_IF_READ_STD,       IF_TYPE_STD,
		SPI_IF_READ_FAST,      IF_TYPE_STD,
		SPI_IF_READ_DUAL,      IF_TYPE_DUAL,
		SPI_IF_READ_DUAL_ADDR, IF_TYPE_DIO,
		SPI_IF_READ_QUAD,      IF_TYPE_QUAD,
		SPI_IF_READ_QUAD_ADDR, IF_TYPE_QIO,
		0, 0,
	};
	const int iftype_write[] = {
		SPI_IF_WRITE_STD,       IF_TYPE_STD,
		SPI_IF_WRITE_DUAL,      IF_TYPE_DUAL,
		SPI_IF_WRITE_DUAL_ADDR, IF_TYPE_DIO,
		SPI_IF_WRITE_QUAD,      IF_TYPE_QUAD,
		SPI_IF_WRITE_QUAD_ADDR, IF_TYPE_QIO,
		0, 0,
	};

	for (ix = 0; iftype_write[ix]; ix += 2) {
		if (spi->write->iftype == iftype_write[ix]) {
			spi->write->iftype = iftype_write[ix + 1];
			break;
		}
	}
	hifmc100_spi_nor_get_best_clock(&spi->write->clock);

	for (ix = 0; iftype_read[ix]; ix += 2) {
		if (spi->read->iftype == iftype_read[ix]) {
			spi->read->iftype = iftype_read[ix + 1];
			break;
		}
	}
	hifmc100_spi_nor_get_best_clock(&spi->read->clock);

	hifmc100_spi_nor_get_best_clock(&spi->erase->clock);
	spi->erase->iftype = IF_TYPE_STD;
}

/*****************************************************************************/
static void hifmc100_dma_transfer(struct hifmc_host *host,
	unsigned int spi_start_addr, unsigned char *dma_buffer,
	unsigned char rw_op, unsigned int size)
{
	unsigned char if_type = 0, dummy = 0;
	unsigned char w_cmd = 0, r_cmd = 0;
	unsigned int regval;
	struct hifmc_spi *spi = host->spi;

	DMA_MSG("\t\t * Start dma transfer => [%#x], len[%#x].\n",
			spi_start_addr, size);

	regval = FMC_INT_CLR_ALL;
	hifmc_write(host, FMC_INT_CLR, regval);
	DMA_MSG("\t\t   Set INT_CLR[%#x]%#x\n", FMC_INT_CLR, regval);

	regval = spi_start_addr;
	hifmc_write(host, FMC_ADDRL, regval);
	DMA_MSG("\t\t   Set ADDRL[%#x]%#x\n", FMC_ADDRL, regval);

	if (rw_op == RW_OP_WRITE) {
		if_type = spi->write->iftype;
		dummy = spi->write->dummy;
		w_cmd = spi->write->cmd;
	} else if (rw_op == RW_OP_READ) {
		if_type = spi->read->iftype;
		dummy = spi->read->dummy;
		r_cmd = spi->read->cmd;
	}

	regval = FMC_OP_CFG_FM_CS(spi->chipselect)
		| FMC_OP_CFG_MEM_IF_TYPE(if_type)
		| FMC_OP_CFG_ADDR_NUM(spi->addrcycle)
		| FMC_OP_CFG_DUMMY_NUM(dummy);
	hifmc_write(host, FMC_OP_CFG, regval);
	DMA_MSG("\t\t   Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DMA_LEN_SET(size);
	hifmc_write(host, FMC_DMA_LEN, regval);
	DMA_MSG("\t\t   Set DMA_LEN[%#x]%#x\n", FMC_DMA_LEN, regval);

	regval = (unsigned int)dma_buffer;
	hifmc_write(host, FMC_DMA_SADDR_D0, regval);
	DMA_MSG("\t\t   Set DMA_SADDR_D0[%#x]%#x\n", FMC_DMA_SADDR_D0, regval);

	regval = FMC_OP_CTRL_RD_OPCODE(r_cmd)
		| FMC_OP_CTRL_WR_OPCODE(w_cmd)
		| FMC_OP_CTRL_RW_OP(rw_op)
		| FMC_OP_CTRL_DMA_OP_READY;
	hifmc_write(host, FMC_OP_CTRL, regval);
	DMA_MSG("\t\t   Set OP_CTRL[%#x]%#x\n", FMC_OP_CTRL, regval);

	FMC_DMA_WAIT_INT_FINISH(host);

	DMA_MSG("\t\t * End dma transfer.\n");
}

/*****************************************************************************/
#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_READ
static char *hifmc100_reg_read_buf(struct hifmc_host *host,
	struct hifmc_spi *spi, unsigned int spi_start_addr,
	unsigned int size, unsigned char *buffer)
{
	unsigned int regval;

	DMA_MSG("* Start reg read, from:%#x len:%#x\n", spi_start_addr, size);

	if (size > HIFMC100_DMA_MAX_SIZE)
		DBG_BUG("reg read out of reg range.\n");

	hifmc_write(host, FMC_ADDRL, spi_start_addr);
	DMA_MSG("  Set ADDRL[%#x]%#x\n", FMC_ADDRL, spi_start_addr);

	regval = FMC_DATA_NUM_CNT(size);
	hifmc_write(host, FMC_DATA_NUM, regval);
	DMA_MSG("  Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CTRL_RD_OPCODE(spi->read->cmd)
		| FMC_OP_CTRL_DMA_OP(OP_TYPE_REG)
		| FMC_OP_CTRL_RW_OP(RW_OP_READ)
		| FMC_OP_CTRL_DMA_OP_READY;
	hifmc_write(host, FMC_OP_CTRL, regval);
	DMA_MSG("  Set OP_CTRL[%#x]%#x\n", FMC_OP_CTRL, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	memcpy(buffer, host->iobase, size);

	DMA_MSG("* End reg page read.\n");

	return buffer;
}

/*****************************************************************************/
static int hifmc100_reg_read(struct spi_flash *spiflash, u32 from,
	size_t len, void *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = (unsigned char *)buf;
	struct hifmc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hifmc_spi *spi = host->spi;

	if ((from + len) > spiflash->size) {
		DBG_MSG("read area out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DBG_MSG("read length is 0.\n");
		return 0;
	}

	if (spi->driver->wait_ready(spi)) {
		DBG_MSG("Error: Dma read wait ready fail!\n");
		return -EBUSY;
	}

	host->set_system_clock(host, spi->read, ENABLE);

	while (len > 0) {
		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("read memory out of range.\n");
			if (spi->driver->wait_ready(spi))
				goto fail;
			host->set_system_clock(host, spi->read, ENABLE);
		}

		num = ((from + len) >= spi->chipsize) ?
				(spi->chipsize - from) : len;

		while (num >= HIFMC100_DMA_MAX_SIZE) {
			hifmc100_reg_read_buf(host, spi,
				from, HIFMC100_DMA_MAX_SIZE, ptr);
			ptr  += HIFMC100_DMA_MAX_SIZE;
			from += HIFMC100_DMA_MAX_SIZE;
			len  -= HIFMC100_DMA_MAX_SIZE;
			num  -= HIFMC100_DMA_MAX_SIZE;
		}

		if (num) {
			hifmc100_reg_read_buf(host, spi,
				from, num, ptr);
			from += num;
			ptr  += num;
			len  -= num;
		}
	}
	result = 0;
fail:

	return result;
}
#endif /* HIFMC100_SPI_NOR_SUPPORT_REG_READ */

/*****************************************************************************/
static int hifmc100_dma_read(struct spi_flash *spiflash, u32 from, size_t len,
				void *buf)
{
	int num;
	unsigned char *ptr = (unsigned char *)buf;
	struct hifmc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hifmc_spi *spi = host->spi;

	RD_MSG("* Start dma read, from:%#x len:%#x\n", from, len);

	if ((from + len) > spiflash->size) {
		DBG_MSG("read area out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DBG_MSG("read length is 0.\n");
		return 0;
	}

	if (spi->driver->wait_ready(spi)) {
		DBG_MSG("Error: Dma read wait ready fail!\n");
		return -EBUSY;
	}

	while (len > 0) {
		while (from >= spi->chipsize) {
			from -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("read memory out of range.\n");
		}

		num = ((from + len) >= spi->chipsize)
			? (spi->chipsize - from) : len;
		while (num >= HIFMC100_DMA_MAX_SIZE) {
			hifmc100_dma_transfer(host, from,
				(unsigned char *)host->dma_buffer, RW_OP_READ,
				HIFMC100_DMA_MAX_SIZE);
			memcpy(ptr, host->buffer, HIFMC100_DMA_MAX_SIZE);
			ptr  += HIFMC100_DMA_MAX_SIZE;
			from += HIFMC100_DMA_MAX_SIZE;
			len  -= HIFMC100_DMA_MAX_SIZE;
			num  -= HIFMC100_DMA_MAX_SIZE;
		}

		if (num) {
			hifmc100_dma_transfer(host, from,
					(unsigned char *)host->dma_buffer,
					RW_OP_READ, num);
			memcpy(ptr, host->buffer, num);
			from += num;
			ptr  += num;
			len  -= num;
		}
	}

	RD_MSG("* End dma read.\n");

	return 0;
}

/*****************************************************************************/
static void hifmc100_read_ids(struct hifmc_host *host,
			unsigned char chipselect, unsigned char *buffer)
{
	unsigned int regval;

	BT_MSG("* Read SPI(cs:%d) ID start.\n", chipselect);

	regval = FMC_CMD_CMD1(SPI_CMD_RDID);
	hifmc_write(host, FMC_CMD, regval);
	BT_MSG("  Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = FMC_OP_CFG_FM_CS(chipselect);
	hifmc_write(host, FMC_OP_CFG, regval);
	BT_MSG("  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_DATA_NUM_CNT(MAX_SPI_NOR_ID_LEN);
	hifmc_write(host, FMC_DATA_NUM, regval);
	BT_MSG("  Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_READ_DATE_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	BT_MSG("  Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	memcpy(buffer, host->iobase, MAX_SPI_NOR_ID_LEN);

	BT_MSG("* Read CS: %d ID: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
			chipselect, buffer[0], buffer[1], buffer[2], buffer[3],
			buffer[4], buffer[5]);

	return;
}

/*****************************************************************************/
static int hifmc100_reg_erase_one_block(struct hifmc_host *host,
	struct hifmc_spi *spi, unsigned int offset)
{
	unsigned int regval;

	OP_MSG("\t\t * Start erase one block, offset:%#x\n", offset);

	regval = spi->driver->wait_ready(spi);
	if (regval) {
		DBG_MSG("Error: Erase wait ready fail! reg:%#x\n", regval);
		return 1;
	}

	spi->driver->write_enable(spi);

	host->set_system_clock(host, spi->erase, ENABLE);

	regval = FMC_CMD_CMD1(spi->erase->cmd);
	hifmc_write(host, FMC_CMD, regval);
	OP_MSG("\t\t   Set CMD[%#x]%#x\n", FMC_CMD, regval);

	regval = offset;
	hifmc_write(host, FMC_ADDRL, regval);
	OP_MSG("\t\t   Set ADDRL[%#x]%#x\n", FMC_ADDRL, regval);

	regval = FMC_OP_CFG_FM_CS(spi->chipselect)
		| FMC_OP_CFG_MEM_IF_TYPE(spi->erase->iftype)
		| FMC_OP_CFG_ADDR_NUM(spi->addrcycle)
		| FMC_OP_CFG_DUMMY_NUM(spi->erase->dummy);
	hifmc_write(host, FMC_OP_CFG, regval);
	OP_MSG("\t\t   Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, regval);

	regval = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_ADDR_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, regval);
	OP_MSG("\t\t   Set OP[%#x]%#x\n", FMC_OP, regval);

	FMC_CMD_WAIT_CPU_FINISH(host);

	OP_MSG("\t\t * End erase one block.\n");

	return 0;
}

/*****************************************************************************/
static int hifmc100_dma_write(struct spi_flash *spiflash, u32 to, size_t len,
	const void *buf)
{
	int num;
	int result = -EIO;

	unsigned char *ptr = (unsigned char *)buf;
	struct hifmc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hifmc_spi *spi = host->spi;

	if (WR_DBG)
		printf("\n");
	WR_MSG("\t\t* Start dma write, to:%#x len:%#x\n", to, len);

	if ((to + len) > spiflash->size) {
		DBG_MSG("write data out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DBG_MSG("write length is 0.\n");
		return 0;
	}

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
	if (host->level) {
		if ((BP_CMP_TOP == host->cmp)
		    && ((to + len) > host->start_addr)) {
			puts("\n");
			DBG_MSG("write area to[%#x => %#x] is locked,",
					host->start_addr, (to + len));
			DBG_MSG(" please unlock these blocks.\n");
			return -EINVAL;
		}

		if ((BP_CMP_BOTTOM == host->cmp) && (to < host->end_addr)) {
			unsigned end = ((to + len) > host->end_addr)
				       ? host->end_addr : (to + len);

			puts("\n");
			DBG_MSG("write area to[%#x => %#x] is locked, please",
					to, end);
			DBG_MSG(" unlock these blocks.\n");
			return -EINVAL;
		}
	}
#endif /* CONFIG_CMD_SPI_BLOCK_PROTECTION */

	if (spi->driver->wait_ready(spi))
		goto fail;

	spi->driver->write_enable(spi);

	if (to & HIFMC100_DMA_MAX_MASK) {
		num = HIFMC100_DMA_MAX_SIZE - (to & HIFMC100_DMA_MAX_MASK);
		if (num > len)
			num = len;
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;

			spi->driver->write_enable(spi);
		}
		memcpy(host->buffer, ptr, num);
		WR_MSG("\t\t  DMA align write, to:%#x len:%#x.\n", to, num);
		hifmc100_dma_transfer(host, to,
			(unsigned char *)host->dma_buffer, RW_OP_WRITE, num);

		to  += num;
		ptr += num;
		len -= num;
	}

	while (len > 0) {
		num = ((len >= HIFMC100_DMA_MAX_SIZE)
			? HIFMC100_DMA_MAX_SIZE : len);
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;

			spi->driver->write_enable(spi);
		}

		memcpy(host->buffer, ptr, num);
		WR_MSG("\t\t  DMA split write, to:%#x len:%#x.\n", to, num);
		hifmc100_dma_transfer(host, to,
			(unsigned char *)host->dma_buffer, RW_OP_WRITE, num);

		to  += num;
		ptr += num;
		len -= num;
	}

	result = 0;
fail:
	WR_MSG("\t\t* End dma write.\n");
	return result;
}

/*****************************************************************************/
#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_WRITE
static int hifmc100_reg_write_buf(struct hifmc_host *host,
		struct hifmc_spi *spi, u32 spi_start_addr,
	size_t size, unsigned char *buffer)
{
	if (size > HIFMC100_DMA_MAX_SIZE)
		DBG_BUG("reg read out of reg range.\n");

	if (spi->driver->wait_ready(spi))
		return 1;

	memcpy((char *)host->iobase, buffer, size);

	spi->driver->write_enable(spi);

	hifmc_write(host, FMC_INT_CLR, FMC_INT_CLR_ALL);

	hifmc_write(host, FMC_CMD, FMC_CMD_CMD1(spi->write->cmd));

	hifmc_write(host, FMC_OP_CFG, FMC_OP_CFG_FM_CS(spi->chipselect)
				| FMC_OP_CFG_MEM_IF_TYPE(spi->write->iftype));

	hifmc_write(host, FMC_ADDRL, spi_start_addr);

	hifmc_write(host, FMC_OP_CTRL, (FMC_OP_CTRL_WR_OPCODE(spi->write->cmd)
			| FMC_OP_CTRL_DMA_OP(OP_TYPE_REG)
			| FMC_OP_CTRL_RW_OP(RW_OP_WRITE)
			| FMC_OP_CTRL_DMA_OP_READY));

	FMC_DMA_WAIT_CPU_FINISH(host);

	return 0;
}

/*****************************************************************************/
static int hifmc100_reg_write(struct spi_flash *spiflash, u32 to, size_t len,
	const void *buf)
{
	int num;
	int result = -EIO;
	unsigned char *ptr = (unsigned char *)buf;
	struct hifmc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hifmc_spi *spi = host->spi;

	if ((to + len) > spiflash->size) {
		DBG_MSG("write data out of range.\n");
		return -EINVAL;
	}

	if (!len) {
		DBG_MSG("write length is 0.\n");
		return 0;
	}

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
	if (host->level) {
		if ((BP_CMP_TOP == host->cmp)
		    && ((to + len) > host->start_addr)) {
			puts("\n");
			DBG_MSG("write area to[%#x => %#x] is locked,",
					host->start_addr, (to + len));
			DBG_MSG(" please unlock these blocks.\n");
			return -EINVAL;
		}

		if ((BP_CMP_BOTTOM == host->cmp) && (to < host->end_addr)) {
			unsigned end = ((to + len) > host->end_addr)
				       ? host->end_addr : (to + len);

			puts("\n");
			DBG_MSG("write area to[%#x => %#x] is locked, please",
					to, end);
			DBG_MSG(" unlock these blocks.\n");
			return -EINVAL;
		}
	}
#endif /* CONFIG_CMD_SPI_BLOCK_PROTECTION */

	if (spi->driver->wait_ready(spi))
		goto fail;

	host->set_system_clock(host, spi->write, ENABLE);

	if (to & HIFMC100_DMA_MAX_MASK) {
		num = HIFMC100_DMA_MAX_SIZE - (to & HIFMC100_DMA_MAX_MASK);
		if (num > (int)len)
			num = (int)len;

		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;

			host->set_system_clock(host, spi->write, ENABLE);
		}

		if (hifmc100_reg_write_buf(host, spi, to, num, ptr))
			goto fail;

		to  += num;
		ptr += num;
		len -= num;
	}

	while (len > 0)	{
		num = ((len >= HIFMC100_DMA_MAX_SIZE) ?
				HIFMC100_DMA_MAX_SIZE : len);
		while (to >= spi->chipsize) {
			to -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("write memory out of range.\n");

			if (spi->driver->wait_ready(spi))
				goto fail;
			host->set_system_clock(host, spi->write, ENABLE);
		}
		if (hifmc100_reg_write_buf(host, spi, to, num, ptr))
			goto fail;
		to  += num;
		ptr += num;
		len -= num;
	}
fail:
	return 0;
}
#endif /* HIFMC100_SPI_NOR_SUPPORT_REG_WRITE */

/*****************************************************************************/
static int hifmc100_reg_erase(struct spi_flash *spiflash, u32 offset,
	size_t length)
{
	struct hifmc_host *host = SPIFLASH_TO_HOST(spiflash);
	struct hifmc_spi *spi = host->spi;

	if (OP_DBG)
		printf("\n");
	OP_MSG("\t\t* Start reg erase, offset:%#x len:%#x\n", offset, length);

	if (offset + length > spiflash->size) {
		DBG_MSG("Error: Erase area out of range of mtd.\n");
		return -EINVAL;
	}

	if (offset & (host->erasesize - 1)) {
		DBG_MSG("Error: Erase start address is not alignment.\n");
		return -EINVAL;
	}

	if (length & (host->erasesize - 1)) {
		DBG_MSG("Error: Erase length is not alignment.\n");
		return -EINVAL;
	}

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
	if (host->level) {
		if ((BP_CMP_TOP == host->cmp)
		    && ((offset + length) > host->start_addr)) {
			puts("\n");
			DBG_MSG("erase area offset[%#x => %#x] is locked,",
					host->start_addr, (offset + length));
			DBG_MSG(" please unlock these blocks.\n");
			return -EINVAL;
		}

		if ((BP_CMP_BOTTOM == host->cmp) && (offset < host->end_addr)) {
			unsigned end = ((offset + length) > host->end_addr)
				       ? host->end_addr : (offset + length);

			puts("\n");
			DBG_MSG("erase area offset[%#x => %#x] is locked,",
					offset, end);
			DBG_MSG(" please unlock these blocks.\n");
			return -EINVAL;
		}
	}
#endif /* CONFIG_CMD_SPI_BLOCK_PROTECTION */

	while (length) {
		if (spi->chipsize <= offset) {
			offset -= spi->chipsize;
			spi++;
			if (!spi->name)
				DBG_BUG("Error: Erase memory out of range.\n");
		}

		if (hifmc100_reg_erase_one_block(host, spi, offset))
			return -1;

		offset += spi->erase->size;
		length -= spi->erase->size;
	}

	OP_MSG("\t\t* End reg erase.\n");

	return 0;
}

/*****************************************************************************/
static int hifmc100_spi_probe(struct hifmc_host *host)
{
	unsigned int total = 0;
	unsigned char ids[MAX_SPI_NOR_ID_LEN], ix, chipselect = 0;
	struct spi_info *spiinfo;
	struct hifmc_spi *spi = host->spi;

	host->num_chip = 0;

	for (; chipselect < CONFIG_HIFMC100_CHIP_NUM; chipselect++) {

		hifmc100_read_ids(host, chipselect, ids);

		/* can't find spi flash device. */
		if (!(ids[0] | ids[1] | ids[2])
			|| ((ids[0] & ids[1] & ids[2]) == 0xFF))
			continue;

		printf("SPI Nor(cs %d) ID: %#x %#x %#x", chipselect, ids[0],
				ids[1], ids[2]);

		spiinfo = spi_serach_ids(hifmc100_spi_info_table, ids);
		for (ix = 3; (spiinfo) && (ix < spiinfo->id_len); ix++)
			printf(" %#x", ids[ix]);
		printf("\n");

		if (spiinfo) {
			spi->name = spiinfo->name;
			spi->chipselect = chipselect;
			spi->chipsize   = spiinfo->chipsize;
			spi->erasesize  = spiinfo->erasesize;
			spi->addrcycle  = spiinfo->addrcycle;
			spi->driver     = spiinfo->driver;

			spi->host       = host;
			spi->iobase = (char *)host->iobase + total;

			spi_search_rw(spiinfo, spi->read,
				HIFMC100_SPI_NOR_SUPPORT_READ,
				HIFMC100_SPI_NOR_SUPPORT_MAX_DUMMY, READ);
			hifmc100_map_iftype_and_clock(spi);

			spi->driver->qe_enable(spi);

			spi_search_rw(spiinfo, spi->read,
				HIFMC100_SPI_NOR_SUPPORT_READ,
				HIFMC100_SPI_NOR_SUPPORT_MAX_DUMMY, READ);

			spi_search_rw(spiinfo, spi->write,
				HIFMC100_SPI_NOR_SUPPORT_WRITE,
				HIFMC100_SPI_NOR_SUPPORT_MAX_DUMMY, WRITE);

			spi_get_erase(spiinfo, spi->erase);
			hifmc100_map_iftype_and_clock(spi);

			/* auto check fmc_addr_mode 3 bytes or 4 bytes */
			start_up_addr_mode = GET_FMC_BOOT_MODE;

			if (start_up_addr_mode == SPI_NOR_ADDR_MODE_3_BYTES) {
				BT_MSG("start up mode is 3 bytes\n");
				spi->driver->entry_4addr(spi, ENABLE);
			} else
				BT_MSG("start up mode is 4 bytes\n");

			printf("Block:%sB ", ultohstr(spi->erasesize));
			printf("Chip:%sB ", ultohstr(spi->chipsize));
			printf("Name:\"%s\"\n", spi->name);

			if (BT_DBG) {
				printf("SPI Nor(cs %d): ", spi->chipselect);
				if (spi->addrcycle == 4)
					printf("4 addr SPI ");
				else
					printf("3 addr SPI ");

				printf("read:0x%x,%02X,0x%x ",
						spi->read->iftype,
						spi->read->cmd,
						spi->read->clock);
				printf("write:0x%x,%02X,0x%x ",
						spi->write->iftype,
						spi->write->cmd,
						spi->write->clock);
				printf("erase:0x%x,%02X,0x%x\n",
						spi->erase[0].iftype,
						spi->erase[0].cmd,
						spi->erase[0].clock);
			}

			if (spi_info_ex.type == 0) {
				spi_info_ex.type      = MTD_NORFLASH;
				spi_info_ex.chipsize  = spi->chipsize;
				spi_info_ex.erasesize = spi->erasesize;
				spi_info_ex.pagesize  = 1;
				spi_info_ex.addrcycle = spi->addrcycle;

				if (spiinfo->id_len > sizeof(spi_info_ex.ids)) {
					printf("BUG! ID len out of range.\n");
					BUG();
				}

				spi_info_ex.id_length = spiinfo->id_len;
				memcpy(spi_info_ex.ids, spiinfo->id,
						spiinfo->id_len);

				strncpy(spi_info_ex.name, spi->name,
						sizeof(spi_info_ex.name));
				spi_info_ex.name[sizeof(spi_info_ex.name) - 1]
					= '\0';
			}

			host->num_chip++;
			total += spi->chipsize;
			spi++;
		} else
			printf("Spi(cs%d): find unrecognized spi flash.\n",
				chipselect);
	}

	spi_info_ex.numchips = host->num_chip;

	return host->num_chip;
}

/*****************************************************************************/
static int hifmc100_probe(struct hifmc_host *host)
{
	struct spi_flash *spiflash = host->spiflash;

	host->set_system_clock(host, NULL, ENABLE);
	hifmc_write(host, FMC_TIMING_SPI_CFG,
			FMC_TIMING_SPI_CFG_TCSH(CS_HOLD_TIME)
			| FMC_TIMING_SPI_CFG_TCSS(CS_SETUP_TIME)
			| FMC_TIMING_SPI_CFG_TSHSL(CS_DESELECT_TIME));

	if (!hifmc100_spi_probe(host))
		return -1;

	spiflash->erase = hifmc100_reg_erase;
#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_WRITE
	spiflash->write = hifmc100_reg_write;
#else
	spiflash->write = hifmc100_dma_write;
#endif

#ifdef HIFMC100_SPI_NOR_SUPPORT_REG_READ
	spiflash->read  = hifmc100_reg_read;
#else
	spiflash->read  = hifmc100_dma_read;
#endif

	return 0;
}

/*****************************************************************************/
static void hifmc_probe_spi_size(struct hifmc_host *host,
		struct spi_flash *spiflash)
{
	int ix = 1;
	struct hifmc_spi *spi = host->spi;
	int total = spi->chipsize;
	int erasesize = spi->erasesize;

	for (++spi; ix < host->num_chip; ix++, spi++)
		total += spi->chipsize;

	spiflash->size = total;
	host->erasesize = erasesize;

	BT_MSG("SPI Nor flash size: %dB, chip num: %d\n", spiflash->size,
			host->num_chip);
}

/*****************************************************************************/
static struct spi_flash *hifmc100_init(void)
{
	unsigned int reg, dev_type;
	struct hifmc_host *host = &fmc_host;
	static struct spi_flash *spiflash;

	if (spiflash)
		return spiflash;

	memset(host, 0, sizeof(struct hifmc_host));
	memset(&spi_info_ex, 0, sizeof(struct mtd_info_ex));

	host->regbase = (void *)CONFIG_HIFMC_REG_BASE_ADDRESS;
	host->iobase  = (void *)CONFIG_HIFMC_BUFFER_BASE_ADDRESS;

	reg = hifmc_read(host, FMC_CFG);
	if ((reg & ECC_TYPE_MASK) != ECC_TYPE_0BIT) {
		reg &= ~ECC_TYPE_MASK;
		reg |= FMC_CFG_ECC_TYPE(ECC_TYPE_0BIT);
	}

	dev_type = (reg & FLASH_SEL_MASK) >> FLASH_SEL_SHIFT;
	if (dev_type != FLASH_TYPE_SPI_NOR) {
		DBG_MSG("Error: Device type[%d] isn't SPI Nor!\n", dev_type);
		return NULL;
	}

	if ((reg & OP_MODE_MASK) == OP_MODE_BOOT)
		reg |= FMC_CFG_OP_MODE(OP_MODE_NORMAL);

	hifmc_write(host, FMC_CFG, reg);

	host->dma_buffer = (unsigned int)host->buffer;
	host->set_system_clock = hifmc100_set_spi_nor_system_clock;
	host->set_host_addr_mode = hifmc100_set_host_addr_mode;

	spiflash = host->spiflash;

	spiflash->name = "hi_fmc";

	if (hifmc100_probe(host)) {
		printf("Can't find a valid spi nor flash chip.\n");
		goto fail;
	}

	hifmc_probe_spi_size(host, spiflash);

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
	host->cmp = BP_CMP_UPDATE_FLAG;
	hifmc100_get_spi_lock_info(&(host->cmp), &(host->level));
#endif

success:
	return spiflash;

fail:
	if (spiflash)
		spiflash = NULL;
	goto success;
}

/*****************************************************************************/
static struct mtd_info_ex *hifmc100_getinfo(void)
{
	if (spi_info_ex.type == 0) {
		if (hifmc100_init() == NULL)
			return NULL;
	}
	return &spi_info_ex;
}


/*****************************************************************************/
int hifmc100_spiflash_init(struct spi_flash **spiflash,
				struct mtd_info_ex **spiinfo)
{
	static unsigned int fmc_ip_ver;

	/* hifmc flash type check */
	if (get_boot_media() != BOOT_MEDIA_SPIFLASH) {
		printf("Boot start media isn't SPI Nor.\n");
		return -ENODEV;
	}

	/* hifmc ip version check */
	if (fmc_ip_ver != HIFMC_VER_100) {
		printf("Check Flash Memory Controller v100 ... ");
		fmc_ip_ver = readl(CONFIG_HIFMC_REG_BASE_ADDRESS + FMC_VERSION);
		if (fmc_ip_ver != HIFMC_VER_100) {
			printf("\n");
			return -ENODEV;
		}
		printf("Found\n");
	}

	(*spiflash) = hifmc100_init();
	(*spiinfo) = hifmc100_getinfo();
	add_shutdown(hifmc100_shutdown);

	return 1;
}
/*****************************************************************************/

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
/*****************************************************************************/
void spi_lock_update_address(unsigned char cmp, unsigned char level,
			unsigned int *start_addr, unsigned int *end_addr)
{
	unsigned int lock_len, erasesize, chipsize;

	erasesize = *start_addr;
	chipsize = *end_addr;

	*start_addr = 0;

	if (level) {
		lock_len = erasesize << (level - 1);
		if (lock_len > chipsize)
			lock_len = chipsize;

		if (BP_CMP_BOTTOM == cmp)
			*end_addr = lock_len;
		else if (BP_CMP_TOP == cmp)
			*start_addr = chipsize - lock_len;

		printf("Spi is locked. lock address[%#x => %#x]\n",
				*start_addr, *end_addr);
	} else {
		if (BP_CMP_BOTTOM == cmp)
			*end_addr = 0;
		else if (BP_CMP_TOP == cmp)
			*start_addr = chipsize;
	}
}

/*****************************************************************************/
unsigned hifmc100_get_spi_lock_info(unsigned char *cmp, unsigned char *level)
{
	unsigned char status, config, update_flag = *cmp;
	struct hifmc_host *host = &fmc_host;
	struct hifmc_spi *spi = host->spi;

	spi->driver->wait_ready(spi);

	status = spi_general_get_flash_register(spi, SPI_CMD_RDSR);
	BP_MSG("Get Status Register[%#x]\n", status);
	*level = (status & SPI_NOR_SR_BP_MASK) >> SPI_NOR_SR_BP0_SHIFT;

	config = spi_general_get_flash_register(spi, SPI_CMD_RDCR);
	BP_MSG("Get Config Register[%#x]\n", config);
	*cmp = (config & SPI_NOR_SR_TB_MASK) >> SPI_NOR_SR_TB_SHIFT;

	if (update_flag == BP_CMP_UPDATE_FLAG) {
		host->start_addr = host->erasesize;
		host->end_addr = host->spiflash[0].size;
		spi_lock_update_address(*cmp, *level, &(host->start_addr),
				&(host->end_addr));
	}

	return (config << SPI_NOR_CR_SHIFT) | status;
}

/*****************************************************************************/
static void hifmc100_set_BP_level(struct hifmc_host *host, unsigned char cmp,
					unsigned char level)
{
	unsigned char old_level, old_cmp = 0;
	unsigned int val, reg;
	struct hifmc_spi *spi = host->spi;

	BP_MSG("* Start BP %s level %d\n", (cmp ? "bottom" : "top"), level);

	val = hifmc100_get_spi_lock_info(&old_cmp, &old_level);
	BP_MSG("  Read CR:SR[%#x]\n", val);

	if ((old_cmp == cmp) && (old_level == level))
		return;

	spi->driver->write_enable(spi);

	if ((old_cmp != cmp) && level && (level != BP_LEVEL_MAX)) {
		val &= ~(SPI_NOR_SR_TB_MASK << SPI_NOR_CR_SHIFT);
		val |= cmp << (SPI_NOR_SR_TB_SHIFT + SPI_NOR_CR_SHIFT);
	}

	if (old_level != level) {
		val &= ~SPI_NOR_SR_BP_MASK;
		val |= level << SPI_NOR_SR_BP0_SHIFT;
	}

	reg = hifmc_read(host, FMC_GLOBAL_CFG);
	if (reg & FMC_GLOBAL_CFG_WP_ENABLE) {
		BP_MSG("  Hardware protected enable!, reg[%#x]\n", reg);
		reg &= ~FMC_GLOBAL_CFG_WP_ENABLE;
		hifmc_write(host, FMC_GLOBAL_CFG, reg);
		val &= ~SPI_NOR_SR_SRWD_MASK;
		BP_MSG("Disable SR[%d]:SRWD and WP#\n", SPI_NOR_SR_SRWD_SHIFT);
	}

	writeb(val, host->iobase);
	BP_MSG(" WriAte IO[%#x]%#x\n", (unsigned int)host->iobase,
			*(unsigned char *)host->iobase);

	reg = FMC_CMD_CMD1(SPI_CMD_WRSR);
	hifmc_write(host, FMC_CMD, reg);
	BP_MSG(" Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = FMC_OP_CFG_FM_CS(spi->chipselect);
	hifmc_write(host, FMC_OP_CFG, reg);
	BP_MSG(" Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_DATA_NUM_CNT(SPI_NOR_SR_LEN);
	hifmc_write(host, FMC_DATA_NUM, reg);
	BP_MSG(" Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_WRITE_DATE_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	BP_MSG(" Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	BP_MSG("* Set BP level end.\n");
}

/*****************************************************************************/
void hifmc100_spi_lock(unsigned char cmp, unsigned char level)
{
	unsigned char old_level, old_cmp = 0;
	struct hifmc_host *host = &fmc_host;

	/* Set lock start of top to bottom, warning */
	if ((cmp == BP_CMP_BOTTOM) && (host->cmp == BP_CMP_TOP)) {
		puts("Warning: Set bottom option will change a OTP(One Time ");
		puts("Program) bit!\nThere is no way to recover if them ");
		puts("are changed.\nAre sure of what you are doing!\n");
		puts("Really set start of bottom address? <y/n>\n");
		if (getc() != 'y') {
			puts("Set start of bottom address aborted\n");
			return;
		}
	}

	/* Set lock start of bottom to top, error */
	if ((level != BP_LEVEL_0) && (level != BP_LEVEL_MAX)
	    && (cmp == BP_CMP_TOP) && (host->cmp == BP_CMP_BOTTOM)) {
		DBG_MSG("Error: Set top option will change a OTP(One Time ");
		DBG_MSG("Program) bit when it is changed!\n");
		return;
	}

	hifmc100_set_BP_level(host, cmp, level);

	hifmc100_get_spi_lock_info(&old_cmp, &old_level);
	if ((old_cmp != cmp) && level && (level != BP_LEVEL_MAX)) {
		DBG_MSG("Error: Current lock start of %s address, but set ",
				(old_cmp ? "bottom" : "top"));
		DBG_MSG("value: %s\n", (cmp ? "bottom" : "top"));
		return;
	}

	if (old_level != level) {
		DBG_MSG("Error: Current lock level: %d, but set value: %d\n",
			old_level, level);
		return;
	}

	host->start_addr = host->erasesize;
	host->end_addr = host->spiflash[0].size;
	spi_lock_update_address(cmp, level, &(host->start_addr),
				&(host->end_addr));

	if ((level != BP_LEVEL_0) && (level != BP_LEVEL_MAX))
		host->cmp = cmp;
	host->level = level;

	return;
}

/*****************************************************************************/
#endif /* CONFIG_CMD_SPI_BLOCK_PROTECTION */

