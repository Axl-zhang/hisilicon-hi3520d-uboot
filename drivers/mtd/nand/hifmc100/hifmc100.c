/******************************************************************************
 *    Flash Memory Controller v100 Device Driver
 *    Copyright (c) 2014 Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By hisilicon
 *
******************************************************************************/

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <malloc.h>
#include <match_table.h>
#include <asm/arch/platform.h>
#include <linux/mtd/nand.h>
#include <hinfc_common.h>
#include "hifmc100_spi_ids.h"
#include "hifmc100.h"

#ifdef CONFIG_HI3521A
	#include "hifmc100_hi3521a.c"
#endif

unsigned int fmc_ip_ver;

static struct hifmc_host fmc_host = {
	.chip = NULL,
};

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
void hifmc100_ecc0_switch(struct hifmc_host *host, unsigned char op)
{
	unsigned int config, cmp_cfg;

	if (BT_DBG) {
		config = hifmc_read(host, FMC_CFG);
		DBG_MSG("  * Get CFG[%#x]%#x\n", FMC_CFG, config);

		if (op)
			cmp_cfg = host->fmc_cfg;
		else
			cmp_cfg = host->fmc_cfg_ecc0;

		if (cmp_cfg != config)
			DBG_MSG("Warning: FMC config[%#x] is different.\n",
					cmp_cfg);
	}

	if (op == ENABLE)
		config = host->fmc_cfg_ecc0;
	else if (op == DISABLE)
		config = host->fmc_cfg;
	else {
		DBG_MSG("Error: Invalid opcode: %d\n", op);
		return;
	}

	hifmc_write(host, FMC_CFG, config);
	BT_MSG("  * Set CFG[%#x]%#x\n", FMC_CFG, config);
}

/*****************************************************************************/
static void hifmc100_send_cmd_pageprog(struct hifmc_host *host)
{
	unsigned char pages_per_block_shift;
	unsigned int reg, block_num, block_num_h, page_num;
	struct hifmc_op *spi = host->spi;
	struct nand_chip *chip = host->chip;
#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
	const char *op = "Reg";
#else
	const char *op = "Dma";
#endif

	WR_MSG("* Enter %s page program!\n", op);

	reg = spi->driver->wait_ready(spi);
	if (reg) {
		DBG_MSG("Error: %s program wait ready failed! status[%#x]\n",
				op, reg);
		return;
	}

	reg = spi->driver->write_enable(spi);
	if (reg) {
		DBG_MSG("Error: %s program write enable failed! reg: %#x\n",
				op, reg);
		return;
	}

	reg = FMC_INT_CLR_ALL;
	hifmc_write(host, FMC_INT_CLR, reg);
	WR_MSG("  Set INT_CLR[%#x]%#x\n", FMC_INT_CLR, reg);

	reg = FMC_OP_CFG_FM_CS(host->cmd_option.chipselect)
		| FMC_OP_CFG_MEM_IF_TYPE(spi->write->iftype);
	hifmc_write(host, FMC_OP_CFG, reg);
	WR_MSG("  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	pages_per_block_shift = chip->phys_erase_shift - chip->page_shift;
	block_num = host->addr_value[1] >> pages_per_block_shift;
	block_num_h = block_num >> REG_CNT_HIGH_BLOCK_NUM_SHIFT;
	reg = FMC_ADDRH_SET(block_num_h);
	hifmc_write(host, FMC_ADDRH, reg);
	WR_MSG("  Set ADDRH[%#x]%#x\n", FMC_ADDRH, reg);

	page_num = host->addr_value[1] - (block_num << pages_per_block_shift);
	reg = ((block_num & REG_CNT_BLOCK_NUM_MASK) << REG_CNT_BLOCK_NUM_SHIFT)
	     | ((page_num & REG_CNT_PAGE_NUM_MASK) << REG_CNT_PAGE_NUM_SHIFT);
	hifmc_write(host, FMC_ADDRL, reg);
	WR_MSG("  Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

#ifndef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
	reg = host->dma_buffer;
	hifmc_write(host, FMC_DMA_SADDR_D0, reg);
	WR_MSG("  Set DMA_SADDR_D[0x40]%#x\n", reg);

	reg = host->dma_oob;
	hifmc_write(host, FMC_DMA_SADDR_OOB, reg);
	WR_MSG("  Set DMA_SADDR_OOB[%#x]%#x\n", FMC_DMA_SADDR_OOB, reg);
#endif

	reg = FMC_OP_CTRL_WR_OPCODE(spi->write->cmd)
#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
		| FMC_OP_CTRL_DMA_OP(OP_TYPE_REG)
#else
		| FMC_OP_CTRL_DMA_OP(OP_TYPE_DMA)
#endif
		| FMC_OP_CTRL_RW_OP(RW_OP_WRITE)
		| FMC_OP_CTRL_DMA_OP_READY;
	hifmc_write(host, FMC_OP_CTRL, reg);
	WR_MSG("  Set OP_CTRL[%#x]%#x\n", FMC_OP_CTRL, reg);

	FMC_DMA_WAIT_INT_FINISH(host);

	if (WR_DBG) {
		reg = spi->driver->wait_ready(spi);
		if (reg & STATUS_P_FAIL_MASK)
			DBG_MSG("Error: %s program failed! reg: %#x\n",
								op, reg);
	}

	WR_MSG("* End %s page program!\n", op);
}

/*****************************************************************************/
static void hifmc100_send_cmd_readstart(struct hifmc_host *host)
{
	unsigned char pages_per_block_shift, only_oob = 0;
	unsigned short wrap = 0;
	unsigned int reg, block_num, block_num_h, page_num, addr_of = 0;
	struct hifmc_op *spi = host->spi;
	struct nand_chip *chip = host->chip;
#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_READ
	char *op = "reg";
#else
	char *op = "dma";
#endif

	RD_MSG("\t      |-* Start %s page read\n", op);

	RD_MSG("\t\t|-Wait ready before %s page read\n", op);
	reg = spi->driver->wait_ready(spi);
	if (reg) {
		DBG_MSG("Error: %s read wait ready fail! reg:%#x\n", op, reg);
		return;
	}

	if ((host->addr_value[0] == host->cache_addr_value[0])
		&& (host->addr_value[1] == host->cache_addr_value[1])) {
		RD_MSG("\t\t|-%s page read cache hit! addr1[%#x], addr0[%#x]\n",
			op, host->addr_value[1], host->addr_value[0]);
		return;
	}

	reg = FMC_INT_CLR_ALL;
	hifmc_write(host, FMC_INT_CLR, reg);
	RD_MSG("\t\t|-Set INT_CLR[%#x]%#x\n", FMC_INT_CLR, reg);

	if (host->cmd_option.last_cmd == NAND_CMD_READOOB) {
		only_oob = 1;
		host->cmd_option.op_config =
			FMC_OP_CTRL_RD_OP_SEL(RD_OP_READ_OOB);
	} else
		host->cmd_option.op_config =
			FMC_OP_CTRL_RD_OP_SEL(RD_OP_READ_ALL_PAGE);

	reg = FMC_OP_CFG_FM_CS(host->cmd_option.chipselect)
		| FMC_OP_CFG_MEM_IF_TYPE(spi->read->iftype)
		| FMC_OP_CFG_DUMMY_NUM(spi->read->dummy);
	hifmc_write(host, FMC_OP_CFG, reg);
	RD_MSG("\t\t|-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	pages_per_block_shift = chip->phys_erase_shift - chip->page_shift;
	block_num = host->addr_value[1] >> pages_per_block_shift;
	block_num_h = block_num >> REG_CNT_HIGH_BLOCK_NUM_SHIFT;

	reg = FMC_ADDRH_SET(block_num_h);
	hifmc_write(host, FMC_ADDRH, reg);
	RD_MSG("\t\t|-Set ADDRH[%#x]%#x\n", FMC_ADDRH, reg);

	page_num = host->addr_value[1] - (block_num << pages_per_block_shift);
	if (only_oob)
		switch (host->ecctype) {
		case NAND_ECC_8BIT:
			addr_of = REG_CNT_ECC_8BIT_OFFSET;
			break;
		case NAND_ECC_16BIT:
			addr_of = REG_CNT_ECC_16BIT_OFFSET;
			break;
		case NAND_ECC_24BIT:
			addr_of = REG_CNT_ECC_24BIT_OFFSET;
			break;
		case NAND_ECC_0BIT:
		default:
			break;
		}

	reg = ((block_num & REG_CNT_BLOCK_NUM_MASK) << REG_CNT_BLOCK_NUM_SHIFT)
		| ((page_num & REG_CNT_PAGE_NUM_MASK) << REG_CNT_PAGE_NUM_SHIFT)
		| ((wrap & REG_CNT_WRAP_MASK) << REG_CNT_WRAP_SHIFT)
		| (addr_of & REG_CNT_ECC_OFFSET_MASK);
	hifmc_write(host, FMC_ADDRL, reg);
	RD_MSG("\t\t|-Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

#ifndef HIFMC100_SPI_NAND_SUPPORT_REG_READ
	reg = host->dma_buffer;
	hifmc_write(host, FMC_DMA_SADDR_D0, reg);
	RD_MSG("\t\t|-Set DMA_SADDR_D0[%#x]%#x\n", FMC_DMA_SADDR_D0, reg);

	reg = host->dma_oob;
	hifmc_write(host, FMC_DMA_SADDR_OOB, reg);
	RD_MSG("\t\t|-Set DMA_SADDR_OOB[%#x]%#x\n", FMC_DMA_SADDR_OOB, reg);
#endif

	reg = FMC_OP_CTRL_RD_OPCODE(spi->read->cmd)
		| host->cmd_option.op_config
#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_READ
		| FMC_OP_CTRL_DMA_OP(OP_TYPE_REG)
#else
		| FMC_OP_CTRL_DMA_OP(OP_TYPE_DMA)
#endif
		| FMC_OP_CTRL_RW_OP(RW_OP_READ)
		| FMC_OP_CTRL_DMA_OP_READY;
	hifmc_write(host, FMC_OP_CTRL, reg);
	RD_MSG("\t\t|-Set OP_CTRL[%#x]%#x\n", FMC_OP_CTRL, reg);

	FMC_DMA_WAIT_INT_FINISH(host);

	host->cache_addr_value[0] = host->addr_value[0];
	host->cache_addr_value[1] = host->addr_value[1];

	RD_MSG("\t      |-* End %s page read\n", op);
}

/*****************************************************************************/
static void hifmc100_send_cmd_erase(struct hifmc_host *host)
{
	unsigned int reg;
	struct hifmc_op *spi = host->spi;

	if (ER_DBG)
		printf("\n");
	ER_MSG("\t\t|-* Start send cmd erase!\n");

	reg = spi->driver->wait_ready(spi);
	if (reg) {
		DBG_MSG("Error: Erase wait ready fail! status: %#x\n", reg);
		return;
	}

	reg = spi->driver->write_enable(spi);
	if (reg) {
		DBG_MSG("Error: Erase write enable failed! reg: %#x\n", reg);
		return;
	}

	reg = FMC_INT_CLR_ALL;
	hifmc_write(host, FMC_INT_CLR, reg);
	ER_MSG("\t\t* |-Set INT_CLR[%#x]%#x\n", FMC_INT_CLR, reg);

	reg = spi->erase->cmd;
	hifmc_write(host, FMC_CMD, FMC_CMD_CMD1(reg));
	ER_MSG("\t\t* |-Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = FMC_ADDRL_BLOCK_H_MASK(host->addr_value[1])
		| FMC_ADDRL_BLOCK_L_MASK(host->addr_value[0]);
	hifmc_write(host, FMC_ADDRL, reg);
	ER_MSG("\t\t* |-Set ADDRL[%#x]%#x\n", FMC_ADDRL, reg);

	reg = FMC_OP_CFG_FM_CS(host->cmd_option.chipselect)
		| FMC_OP_CFG_MEM_IF_TYPE(spi->erase->iftype)
		| FMC_OP_CFG_ADDR_NUM(STD_OP_ADDR_NUM)
		| FMC_OP_CFG_DUMMY_NUM(spi->erase->dummy);
	hifmc_write(host, FMC_OP_CFG, reg);
	ER_MSG("\t\t* |-Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_ADDR_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	ER_MSG("\t\t* |-Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	ER_MSG("\t\t|-* End send cmd erase!\n");
}

/*****************************************************************************/
static void hifmc100_send_cmd_status(struct hifmc_host *host)
{
	unsigned char status, addr = STATUS_ADDR;
	unsigned char cmd = host->cmd_option.last_cmd;
	struct hifmc_op *spi = host->spi;

	if ((cmd == NAND_CMD_ERASE1) || (cmd == NAND_CMD_PAGEPROG)) {
		addr = PROTECTION_ADDR;

		if (ER_DBG || WR_DBG) {
			status = spi_nand_feature_op(spi, GET_OP, addr, 0);

			ER_MSG("\t\t* Erase get %#x reg: %#x\n", addr, status);
			WR_MSG("\t\t* Write get %#x reg: %#x\n", addr, status);
		}
	}
}

/*****************************************************************************/
static void hifmc100_send_cmd_readid(struct hifmc_host *host)
{
	unsigned int reg;

	BT_MSG("* Start send cmd read ID.\n");

	hifmc100_ecc0_switch(host, ENABLE);

	hifmc_write(host, FMC_CMD, FMC_CMD_CMD1(SPI_CMD_RDID));
	hifmc_write(host, FMC_ADDRL, READ_ID_ADDR);

	reg = FMC_OP_CFG_FM_CS(host->cmd_option.chipselect)
		| FMC_OP_CFG_ADDR_NUM(READ_ID_ADDR_NUM);
	hifmc_write(host, FMC_OP_CFG, reg);
	BT_MSG("  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_DATA_NUM_CNT(MAX_SPI_NAND_ID_LEN);
	hifmc_write(host, FMC_DATA_NUM, reg);
	BT_MSG("  Set DATA_NUM[%#x]%#x\n", FMC_DATA_NUM, reg);

	reg = FMC_OP_CMD1_EN(ENABLE)
		| FMC_OP_ADDR_EN(ENABLE)
		| FMC_OP_READ_DATE_EN(ENABLE)
		| FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	BT_MSG("  Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	hifmc100_ecc0_switch(host, DISABLE);

	BT_MSG("* End send cmd read flash ID.\n");
}

/*****************************************************************************/
static void hifmc100_send_cmd_reset(struct hifmc_host *host)
{
	unsigned int reg;

	BT_MSG("* Start send cmd reset.\n");

	reg = FMC_CMD_CMD1(SPI_CMD_RESET);
	hifmc_write(host, FMC_CMD, reg);
	BT_MSG("  Set CMD[%#x]%#x\n", FMC_CMD, reg);

	reg = FMC_OP_CFG_FM_CS(host->cmd_option.chipselect);
	hifmc_write(host, FMC_OP_CFG, reg);
	BT_MSG("  Set OP_CFG[%#x]%#x\n", FMC_OP_CFG, reg);

	reg = FMC_OP_CMD1_EN(ENABLE) | FMC_OP_REG_OP_START;
	hifmc_write(host, FMC_OP, reg);
	BT_MSG("  Set OP[%#x]%#x\n", FMC_OP, reg);

	FMC_CMD_WAIT_CPU_FINISH(host);

	BT_MSG("* End send cmd reset.\n");
}

/*****************************************************************************/
static uint8_t hifmc100_read_byte(struct mtd_info *mtd)
{
	unsigned char cmd, value = 0;
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	cmd = host->cmd_option.last_cmd;

	if (cmd == NAND_CMD_READID) {
		value = readb(chip->IO_ADDR_R + host->offset);
		host->offset++;
		if (host->cmd_option.date_num == host->offset)
			host->cmd_option.last_cmd = 0;
		return value;
	}

	if ((cmd == NAND_CMD_ERASE1) || (cmd == NAND_CMD_PAGEPROG)) {
		value = hifmc_read(host, FMC_STATUS);
		if (ANY_BP_ENABLE(value))
			value &= ~NAND_STATUS_WP;
		else
			value |= NAND_STATUS_WP;

		host->cmd_option.last_cmd = 0;

		return value;
	}

	if (cmd == NAND_CMD_ERASE2) {
		value = hifmc_read(host, FMC_STATUS);
		if (value & STATUS_E_FAIL_MASK)
			value |= NAND_STATUS_FAIL;
		return value;
	}

	if (host->cmd_option.command == NAND_CMD_STATUS) {
		value = hifmc_read(host, FMC_STATUS);
		return value;
	}

	if (cmd == NAND_CMD_READOOB) {
		value  = readb(host->dma_oob + host->offset);
		host->offset++;
		return value;
	}

	host->offset++;

	return readb(host->buffer + host->column + host->offset - 1);
}

/*****************************************************************************/
static u16 hifmc100_read_word(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	host->offset += 2;
	return readw(host->buffer + host->column + host->offset - 2);
}

/*****************************************************************************/
static void hifmc100_write_buf(struct mtd_info *mtd,
	const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
	if (buf == chip->oob_poi)
		memcpy((char *)host->iobase + host->pagesize, buf, len);
	else
		memcpy((char *)host->iobase, buf, len);
#else
	if (buf == chip->oob_poi)
		memcpy((char *)host->dma_oob, buf, len);
	else
		memcpy((char *)host->dma_buffer, buf, len);
#endif
	return;
}

/*****************************************************************************/
static void hifmc100_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

#ifdef HIFMC100_SPI_NAND_SUPPORT_REG_READ
	if (buf == chip->oob_poi)
		memcpy(buf, (char *)host->iobase + host->pagesize, len);
	else
		memcpy(buf, (char *)host->iobase, len);
#else
	if (buf == chip->oob_poi)
		memcpy(buf, (char *)host->dma_oob, len);
	else
		memcpy(buf, (char *)host->dma_buffer, len);
#endif
	return;
}

/*****************************************************************************/
static void hifmc100_select_chip(struct mtd_info *mtd, int chipselect)
{
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	if (chipselect < 0)
		return;

	if (chipselect > CONFIG_SYS_MAX_NAND_DEVICE)
		DBG_BUG("Error: Invalid chipselect: %d\n", chipselect);

	host->cmd_option.chipselect = chipselect;
	if (host->mtd != mtd)
		host->mtd = mtd;

	if (!(chip->options & NAND_BROKEN_XD)) {
		switch (chip->state) {
		case FL_ERASING:
			ER_MSG("\t\t* Set last cmd[%#x]\n", NAND_CMD_ERASE1);
			host->cmd_option.last_cmd = NAND_CMD_ERASE1;
			break;

		case FL_WRITING:
			WR_MSG("\t\t* Set last cmd[%#x]\n", NAND_CMD_PAGEPROG);
			host->cmd_option.last_cmd = NAND_CMD_PAGEPROG;
			break;

		default:
			break;
		}
	}
}

/*****************************************************************************/
static void hifmc100_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned ctrl)
{
	unsigned char cmd;
	int is_cache_invalid = 1;
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	if (ctrl & NAND_ALE) {
		unsigned int addr_value = 0;
		unsigned int addr_offset = 0;

		if (ctrl & NAND_CTRL_CHANGE) {
			host->addr_cycle = 0x0;
			host->addr_value[0] = 0x0;
			host->addr_value[1] = 0x0;
		}
		addr_offset = host->addr_cycle << 3;

		if (host->addr_cycle >= HIFMC100_ADDR_CYCLE_MASK) {
			addr_offset = (host->addr_cycle -
					HIFMC100_ADDR_CYCLE_MASK) << 3;
			addr_value = 1;
		}

		host->addr_value[addr_value] |=
			((dat & 0xff) << addr_offset);

		host->addr_cycle++;
	}

	if ((ctrl & NAND_CLE) && (ctrl & NAND_CTRL_CHANGE)) {
		cmd = dat & 0xff;
		host->cmd_option.command = cmd;
		switch (cmd) {
		case NAND_CMD_PAGEPROG:
			host->offset = 0;
			host->send_cmd_pageprog(host);
			break;

		case NAND_CMD_READSTART:
			is_cache_invalid = 0;
			if (host->addr_value[0] == host->pagesize)
				host->cmd_option.last_cmd = NAND_CMD_READOOB;
			host->send_cmd_readstart(host);
			break;

		case NAND_CMD_ERASE2:
			host->cmd_option.last_cmd = cmd;
			host->send_cmd_erase(host);
			break;

		case NAND_CMD_READID:
			memset((unsigned char *)(chip->IO_ADDR_R), 0,
				MAX_SPI_NAND_ID_LEN);
			host->cmd_option.last_cmd = cmd;
			host->cmd_option.date_num = MAX_SPI_NAND_ID_LEN;
			host->send_cmd_readid(host);
			break;

		case NAND_CMD_STATUS:
			host->send_cmd_status(host);
			break;

		case NAND_CMD_SEQIN:
			break;

		case NAND_CMD_ERASE1:
			break;

		case NAND_CMD_READ0:
			host->cmd_option.last_cmd = cmd;
			break;

		case NAND_CMD_RESET:
			host->send_cmd_reset(host);
			break;

		default:
			break;
		}
	}

	if ((dat == NAND_CMD_NONE) && host->addr_cycle) {
		if (host->cmd_option.command == NAND_CMD_SEQIN
		    || host->cmd_option.command == NAND_CMD_READ0
		    || host->cmd_option.command == NAND_CMD_READID) {
			host->offset = 0x0;
			host->column = (host->addr_value[0] & 0xffff);
		}
	}

	if (is_cache_invalid) {
		host->cache_addr_value[0] = ~0;
		host->cache_addr_value[1] = ~0;
	}
}

/*****************************************************************************/
static int hifmc100_dev_ready(struct mtd_info *mtd)
{
	unsigned int reg;
	unsigned long deadline = 1 << 15;
	struct nand_chip *chip = mtd->priv;
	struct hifmc_host *host = chip->priv;

	do {
		hifmc_write(host, FMC_INT_CLR, FMC_INT_CLR_OP_DONE);

		reg = FMC_OP_CFG_FM_CS(host->cmd_option.chipselect);
		hifmc_write(host, FMC_OP_CFG, reg);

		reg = FMC_OP_READ_STATUS_EN(ENABLE) | FMC_OP_REG_OP_START;
		hifmc_write(host, FMC_OP, reg);

		FMC_CMD_WAIT_CPU_FINISH(host);

		reg = hifmc_read(host, FMC_STATUS);
		if (!(reg & STATUS_OIP_MASK))
			return 1;

		udelay(1);

	} while (deadline--);

	DBG_MSG("Error: SPI Nand wait ready timeout, status: %#x\n", reg);

	return 0;
}

/*****************************************************************************/
/*
 * 'host->epm' only use the first oobfree[0] field, it looks very simple, But...
 */
static struct nand_ecclayout nand_ecc_default = {
	.oobfree = {{2, 30} }
};

/*****************************************************************************/
static struct nand_config_info hifmc_spi_nand_config_table[] = {
	{NAND_PAGE_4K,	NAND_ECC_24BIT,	200,	&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_16BIT,	144,	&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_8BIT,	128,	&nand_ecc_default},
	{NAND_PAGE_4K,	NAND_ECC_0BIT,	32,	&nand_ecc_default},

	{NAND_PAGE_2K,	NAND_ECC_24BIT,	128,	&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_16BIT,	88,	&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_8BIT,	64,	&nand_ecc_default},
	{NAND_PAGE_2K,	NAND_ECC_0BIT,	32,	&nand_ecc_default},

	{0,		0,		0,	NULL},
};

/*****************************************************************************/
/* used the best correct arithmetic. */
struct nand_config_info *hifmc100_get_best_ecc(struct mtd_info *mtd)
{
	struct nand_config_info *best = NULL;
	struct nand_config_info *info = hifmc_spi_nand_config_table;

	for (; info->layout; info++) {
		if (nandpage_type2size(info->pagetype) != mtd->writesize)
			continue;

		if (mtd->oobsize < info->oobsize)
			continue;

		if (!best || (best->ecctype < info->ecctype))
			best = info;
	}

	if (!best) {
		DBG_BUG(ERSTR_DRIVER "Driver does not support the pagesize");
		DBG_BUG(ERSTR_DRIVER "(%d) and oobsize(%d).\n",
				mtd->writesize, mtd->oobsize);
	}
	return best;
}

/*****************************************************************************/
/* force the pagesize and ecctype */
struct nand_config_info *hifmc100_force_ecc(struct mtd_info *mtd, int pagetype,
		int ecctype, char *cfgmsg, int allow_pagediv)
{
	int pagesize;
	struct nand_config_info *fit = NULL;
	struct nand_config_info *info = hifmc_spi_nand_config_table;

	for (; info->layout; info++) {
		if (info->pagetype == pagetype && info->ecctype == ecctype) {
			fit = info;
			break;
		}
	}

	if (!fit) {
		debug_register_dump();
		DBG_BUG(ERSTR_DRIVER "Driver(%s mode) does not support this ",
				cfgmsg);
		DBG_BUG(ERSTR_DRIVER "SPI Nand Flash pagesize:%s, ecctype:%s\n",
				nand_page_name(pagetype),
				nand_ecc_name(ecctype));
		return NULL;
	}

	pagesize = nandpage_type2size(pagetype);
	if ((pagesize != mtd->writesize)
		&& (pagesize > mtd->writesize || !allow_pagediv)) {
		debug_register_dump();
		DBG_BUG(ERSTR_HARDWARE "Hardware (%s mode) configure ", cfgmsg);
		DBG_BUG(ERSTR_HARDWARE "pagesize  %d, but the SPI ", pagesize);
		DBG_BUG(ERSTR_HARDWARE "Nand Flash pageszie is %d\n",
				mtd->writesize);
		return NULL;
	}

	if (fit->oobsize > mtd->oobsize) {
		DBG_BUG(ERSTR_HARDWARE "(%s mode) The SPI Nand Flash offer ",
				cfgmsg);
		DBG_BUG(ERSTR_HARDWARE "space area is %d bytes, but the ",
				mtd->oobsize);
		DBG_BUG(ERSTR_HARDWARE "controller request %dbytes in ecc %s. ",
				fit->oobsize, nand_ecc_name(ecctype));
		DBG_BUG(ERSTR_HARDWARE "Please make sure the hardware ECC ");
		DBG_BUG(ERSTR_HARDWARE "configuration is correct.");
		return NULL;
	}

	return fit;
}

/*****************************************************************************/
int hifmc100_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip,
	struct nand_flash_dev_ex *flash_dev_ex)
{
	char *start_type = "unknown";
	struct nand_config_info *best = NULL;
	struct hifmc_host *host = chip->priv;

#ifdef CONFIG_HIFMC100_AUTO_PAGESIZE_ECC
	unsigned int reg;

	best = hifmc100_get_best_ecc(mtd);
	start_type = "Auto";

	BT_MSG("Auto Config, Page size: %s Ecc type: %s OOB size: %d\n",
			nand_page_name(best->pagetype),
			nand_ecc_name(best->ecctype), best->oobsize);
	reg = hifmc_read(host, FMC_CFG);
	BT_MSG("Get auto config: %#x\n", reg);
	reg &= ~PAGE_SIZE_MASK;
	switch (best->pagetype) {
	case NAND_PAGE_2K:
		reg |= FMC_CFG_PAGE_SIZE(PAGE_SIZE_2KB);
		break;
	case NAND_PAGE_4K:
		reg |= FMC_CFG_PAGE_SIZE(PAGE_SIZE_4KB);
		break;
	default:
		reg |= FMC_CFG_PAGE_SIZE(PAGE_SIZE_2KB);
	}

	reg &= ~ECC_TYPE_MASK;
	switch (best->ecctype) {
	case NAND_ECC_0BIT:
		reg |= FMC_CFG_ECC_TYPE(ECC_TYPE_0BIT);
		break;
	case NAND_ECC_8BIT:
		reg |= FMC_CFG_ECC_TYPE(ECC_TYPE_8BIT);
		break;
	case NAND_ECC_16BIT:
		reg |= FMC_CFG_ECC_TYPE(ECC_TYPE_16BIT);
		break;
	case NAND_ECC_24BIT:
		reg |= FMC_CFG_ECC_TYPE(ECC_TYPE_24BIT);
		break;
	default:
		reg |= FMC_CFG_ECC_TYPE(ECC_TYPE_8BIT);
	}

	hifmc_write(host, FMC_CFG, reg);

	host->fmc_cfg = reg;
	BT_MSG("Set best config: %#x\n", reg);
#endif /* CONFIG_HIFMC100_AUTO_PAGESIZE_ECC */

#ifdef CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC
	unsigned int reg_page, reg_ecc, page, ecc;

	#ifdef CONFIG_HIFMC100_AUTO_PAGESIZE_ECC
	#error you SHOULD NOT define CONFIG_HIFMC100_AUTO_PAGESIZE_ECC \
		and CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC at the same time
	#endif

	reg_page = (host->fmc_cfg & PAGE_SIZE_MASK) >> PAGE_SIZE_SHIFT;
	switch (reg_page) {
	case PAGE_SIZE_2KB:
		page = NAND_PAGE_2K;
		break;
	case PAGE_SIZE_4KB:
		page = NAND_PAGE_4K;
		break;
	default:
		page = NAND_PAGE_2K;
	}

	reg_ecc = (host->fmc_cfg & ECC_TYPE_MASK) >> ECC_TYPE_SHIFT;
	switch (reg_ecc) {
	case ECC_TYPE_8BIT:
		ecc = NAND_ECC_8BIT;
		break;
	case ECC_TYPE_16BIT:
		ecc = NAND_ECC_16BIT;
		break;
	case ECC_TYPE_24BIT:
		ecc = NAND_ECC_24BIT;
		break;
	default:
		ecc = NAND_ECC_8BIT;
	}
	best = hifmc100_force_ecc(mtd, page, ecc, "hardware config", 0);
	start_type = "Hardware";

#endif /* CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC */

#ifdef CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE
	unsigned int pagetype, reg_page;

	#ifdef CONFIG_HIFMC100_AUTO_PAGESIZE_ECC
	#error you SHOULD NOT define CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE \
		and CONFIG_HIFMC100_AUTO_PAGESIZE_ECC at the same time
	#endif

	#ifdef CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC
	#error you SHOULD NOT define CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE \
		and CONFIG_HIFMC100_HARDWARE_PAGESIZE_ECC at the same time
	#endif

	switch (mtd->writesize) {
	case _2K:
		pagetype = NAND_PAGE_2K;
		break;
	case _4K:
		pagetype = NAND_PAGE_4K;
		break;
	default:
		pagetype = NAND_PAGE_2K;
		break;
	}
	best = hifmc100_force_ecc(mtd, pagetype, NAND_ECC_0BIT,
			"force config", 0);
	start_type = "AutoForce";

	switch (best->pagetype) {
	case NAND_PAGE_2K:
		reg_page = PAGE_SIZE_2KB;
		break;
	case NAND_PAGE_4K:
		reg_page = PAGE_SIZE_4KB;
		break;
	default:
		reg_page = PAGE_SIZE_2KB;
	}

	host->fmc_cfg &= ~(PAGE_SIZE_MASK | ECC_TYPE_MASK);
	host->fmc_cfg |= (FMC_CFG_ECC_TYPE(ECC_TYPE_0BIT)
			| FMC_CFG_PAGE_SIZE(reg_page));
#endif /* CONFIG_HIFMC100_PAGESIZE_AUTO_ECC_NONE */

	if (!best) {
		DBG_BUG(ERSTR_HARDWARE "Please configure SPI Nand Flash ");
		DBG_BUG(ERSTR_HARDWARE "pagesize and ecctype!\n");
	}

	if (best->ecctype != NAND_ECC_0BIT)
		mtd->oobsize = best->oobsize;

	chip->ecc.layout = best->layout;

	host->ecctype  = best->ecctype;
	host->pagesize = nandpage_type2size(best->pagetype);
	host->oobsize  = mtd->oobsize;
	host->block_page_mask = ((mtd->erasesize / mtd->writesize) - 1);

	host->dma_oob = host->dma_buffer + host->pagesize;

	host->bbm = (unsigned char *)(host->buffer + host->pagesize
			+ HINFC_BAD_BLOCK_POS);

	host->epm = (unsigned short *)(host->buffer + host->pagesize
			+ chip->ecc.layout->oobfree[0].offset + 28);

	host->fmc_cfg_ecc0 = (host->fmc_cfg & ~ECC_TYPE_MASK) | ECC_TYPE_0BIT;

	if (mtd->writesize > SPI_NAND_MAX_PAGESIZE
		|| mtd->oobsize > SPI_NAND_MAX_OOBSIZE) {
		DBG_BUG(ERSTR_DRIVER "Driver does not support this SPI Nand ");
		DBG_BUG(ERSTR_DRIVER "Flash. Please increase ");
		DBG_BUG(ERSTR_DRIVER "SPI_NAND_MAX_PAGESIZE and ");
		DBG_BUG(ERSTR_DRIVER "SPI_NAND_MAX_OOBSIZE.\n");
	}

	if (mtd->writesize != host->pagesize) {
		unsigned int shift = 0;
		unsigned int writesize = mtd->writesize;
		while (writesize > host->pagesize) {
			writesize >>= 1;
			shift++;
		}
		chip->chipsize = chip->chipsize >> shift;
		mtd->erasesize = mtd->erasesize >> shift;
		mtd->writesize = host->pagesize;
		DBG_MSG("SPI Nand divide into 1/%u\n", (1 << shift));
	}

	flash_dev_ex->start_type = start_type;
	flash_dev_ex->oobsize = host->oobsize;
	flash_dev_ex->ecctype = host->ecctype;

	return 0;
}

/*****************************************************************************/
void hifmc100_nand_init(struct nand_chip *chip)
{
	memset((char *)chip->IO_ADDR_R, 0xff, HIFMC100_BUFFER_LEN);

	chip->read_byte   = hifmc100_read_byte;
	chip->read_word   = hifmc100_read_word;
	chip->write_buf   = hifmc100_write_buf;
	chip->read_buf    = hifmc100_read_buf;

	chip->select_chip = hifmc100_select_chip;

	chip->cmd_ctrl    = hifmc100_cmd_ctrl;
	chip->dev_ready   = hifmc100_dev_ready;

	chip->chip_delay  = HIFMC100_CHIP_DELAY;

	chip->options = NAND_NO_AUTOINCR | NAND_BBT_SCANNED | NAND_BROKEN_XD;

	chip->ecc.layout  = NULL;
	chip->ecc.mode    = NAND_ECC_NONE;
}

/*****************************************************************************/
int hifmc100_host_init(struct hifmc_host *host)
{
	unsigned int reg, dev_type, align_mask;

	host->iobase  = (void __iomem *)CONFIG_HIFMC_BUFFER_BASE_ADDRESS;
	host->regbase = (void __iomem *)CONFIG_HIFMC_REG_BASE_ADDRESS;

	reg = hifmc_read(host, FMC_CFG);
	dev_type = (reg & FLASH_SEL_MASK) >> FLASH_SEL_SHIFT;
	if (dev_type != FLASH_TYPE_SPI_NAND) {
		DBG_MSG("Error: Device type isn't SPI Nand. reg: %#x\n", reg);
		return -ENODEV;
	}

	if ((reg & OP_MODE_MASK) == OP_MODE_BOOT)
		reg |= FMC_CFG_OP_MODE(OP_MODE_NORMAL);

	hifmc_write(host, FMC_CFG, reg);
	BT_MSG("* Enter normal mode, config: %#x\n", reg);

	host->fmc_cfg = reg;
	host->fmc_cfg_ecc0 = (reg & ~ECC_TYPE_MASK) | ECC_TYPE_0BIT;

	host->addr_cycle = 0;
	host->addr_value[0] = 0;
	host->addr_value[1] = 0;
	host->cache_addr_value[0] = ~0;
	host->cache_addr_value[1] = ~0;

	host->buforg = kmalloc((HIFMC100_BUFFER_LEN + HIFMC100_DMA_ALIGN),
				GFP_KERNEL);
	if (!host->buforg) {
		DBG_MSG("Error: Can't malloc memory for SPI Nand driver.\n");
		return -ENOMEM;
	}

	/* DMA need 32 bytes alignment */
	align_mask = HIFMC100_DMA_ALIGN - 1;
	host->dma_buffer = (u_int)(host->buforg + align_mask) & ~align_mask;

	host->buffer = (char *)host->dma_buffer;
	memset(host->buffer, 0xff, HIFMC100_BUFFER_LEN);

	host->send_cmd_pageprog  = hifmc100_send_cmd_pageprog;
	host->send_cmd_status    = hifmc100_send_cmd_status;
	host->send_cmd_readstart = hifmc100_send_cmd_readstart;
	host->send_cmd_erase     = hifmc100_send_cmd_erase;
	host->send_cmd_readid    = hifmc100_send_cmd_readid;
	host->send_cmd_reset     = hifmc100_send_cmd_reset;
	host->set_system_clock   = hifmc100_set_spi_nand_system_clock;

	hifmc_write(host, FMC_TIMING_SPI_CFG,
			FMC_TIMING_SPI_CFG_TCSH(CS_HOLD_TIME)
			| FMC_TIMING_SPI_CFG_TCSS(CS_SETUP_TIME)
			| FMC_TIMING_SPI_CFG_TSHSL(CS_DESELECT_TIME));

	hifmc_write(host, FMC_DMA_AHB_CTRL, ALL_BURST_ENABLE);

	return 0;
}

/*****************************************************************************/
int board_nand_init(struct nand_chip *nand)
{
	struct hifmc_host *host = &fmc_host;

	/* boot media check */
	if (get_boot_media() != BOOT_MEDIA_NAND) {
		printf("Boot media isn't SPI Nand flash.\n");
		return -ENODEV;
	}

	/* spi nand is initialized */
	if (host->chip) {
		printf("Host was initialized.\n");
		return 0;
	}

	/* fmc ip version check */
	if (fmc_ip_ver != HIFMC_VER_100) {
		printf("Check Flash Memory Controller v100 ... ");
		fmc_ip_ver = readl(CONFIG_HIFMC_REG_BASE_ADDRESS + FMC_VERSION);
		if (fmc_ip_ver != HIFMC_VER_100) {
			printf("\n");
			return -ENODEV;
		}
		printf("Found\n");
	}

	/* hifmc host init */
	memset((char *)host, 0, sizeof(struct hifmc_host));
	if (hifmc100_host_init(host)) {
		DBG_MSG("Error: SPI Nand host init failed!\n");
		return -ENOMEM;
	}
	host->chip = nand;

	/* enable system clock */
	if (host->set_system_clock)
		host->set_system_clock(host, ENABLE);

	/* hifmc nand_chip struct init */
	nand->priv = host;
	hifmc100_nand_init(nand);

	spi_nand_ids_register();
	nand_oob_resize = hifmc100_ecc_probe;

	return 0;
}

/*****************************************************************************/
static struct match_reg_type ecc_type2reg_table[] = {
	{et_ecc_4bit,	 NAND_ECC_8BIT},
	{et_ecc_8bit,	 NAND_ECC_16BIT},
	{et_ecc_24bit1k, NAND_ECC_24BIT},
};

/*****************************************************************************/
static int spi_nand_ecc_type2reg(int ecctype)
{
	return type2reg(ecc_type2reg_table, ARRAY_SIZE(ecc_type2reg_table),
			ecctype, et_ecc_4bit);
}

/*****************************************************************************/
int nand_get_ecctype(void)
{
	if (!fmc_host.chip) {
		printf("spi nand flash uninitialized.\n");
		return -1;
	}

	return spi_nand_ecc_type2reg(fmc_host.ecctype);
}

