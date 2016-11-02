/******************************************************************************
 *    Flash Memory Controller v100 Device Driver
 *    Copyright (c) 2014 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By Hisilicon
 *
******************************************************************************/

#ifndef HIFMC100H
#define HIFMC100H

/******************************************************************************/
/* These macroes are for debug only, reg option is slower then dma option */
#undef HIFMC100_SPI_NAND_SUPPORT_REG_READ
/* #define HIFMC100_SPI_NAND_SUPPORT_REG_READ */

#undef HIFMC100_SPI_NAND_SUPPORT_REG_WRITE
/* #define HIFMC100_SPI_NAND_SUPPORT_REG_WRITE */

/*****************************************************************************/
#define REG_CNT_HIGH_BLOCK_NUM_SHIFT		10

#define REG_CNT_BLOCK_NUM_MASK			0x3ff
#define REG_CNT_BLOCK_NUM_SHIFT			22

#define REG_CNT_PAGE_NUM_MASK			0x3f
#define REG_CNT_PAGE_NUM_SHIFT			16

#define REG_CNT_WRAP_MASK			0xf
#define REG_CNT_WRAP_SHIFT			12

#define REG_CNT_ECC_OFFSET_MASK			0xfff
#define REG_CNT_ECC_8BIT_OFFSET			1054
#define REG_CNT_ECC_16BIT_OFFSET		1056
#define REG_CNT_ECC_24BIT_OFFSET		1082

/*****************************************************************************/
#define SPI_NAND_MAX_PAGESIZE			4096
#define SPI_NAND_MAX_OOBSIZE			256

#define HIFMC100_BUFFER_LEN	(SPI_NAND_MAX_PAGESIZE + SPI_NAND_MAX_OOBSIZE)

/* DMA address align with 32 bytes. */
#define HIFMC100_DMA_ALIGN			32

#define HIFMC100_CHIP_DELAY			25

#define HIFMC100_ADDR_CYCLE_MASK		0x2

/*****************************************************************************/

/* Boot init debug print */
#define BT_DBG					0

/* Erase debug print */
#define ER_DBG					0

/* Write debug print */
#define WR_DBG					0

/* Read debug print */
#define RD_DBG					0

/* Quad Enable debug print */
#define QE_DBG					0

/* Status Register debug print */
#define SR_DBG					0

/* Features debug print */
#define FT_DBG					0

/* Write Enable debug print */
#define WE_DBG					0

/*****************************************************************************/
#define BT_MSG(_fmt, arg...) \
	do { \
		if (BT_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define ER_MSG(_fmt, arg...) \
	do { \
		if (ER_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define WR_MSG(_fmt, arg...) \
	do { \
		if (WR_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define RD_MSG(_fmt, arg...) \
	do { \
		if (RD_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define QE_MSG(_fmt, arg...) \
	do { \
		if (QE_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define SR_MSG(_fmt, arg...) \
	do { \
		if (SR_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define FT_MSG(_fmt, arg...) \
	do { \
		if (FT_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define WE_MSG(_fmt, arg...) \
	do { \
		if (WE_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

/*****************************************************************************/
struct hifmc_cmd_option {
	unsigned char chipselect;
	unsigned char command;
	unsigned char last_cmd;
	unsigned char address_h;
	unsigned int address_l;
	unsigned int date_num;
	unsigned short option;
	unsigned short op_config;
};

struct hifmc_host;

struct hifmc_host {
	struct nand_chip *chip;
	struct mtd_info  *mtd;
	struct hifmc_op spi[CONFIG_SYS_MAX_NAND_DEVICE];
	struct hifmc_cmd_option cmd_option;

	void __iomem *iobase;
	void __iomem *regbase;

	unsigned int fmc_cfg;
	unsigned int fmc_cfg_ecc0;

	unsigned int offset;

	struct device *dev;

	unsigned int addr_cycle;
	unsigned int addr_value[2];
	unsigned int cache_addr_value[2];
	unsigned int column;
	unsigned int block_page_mask;

	unsigned int dma_buffer;
	unsigned int dma_oob;

	unsigned int ecctype;
	unsigned int pagesize;
	unsigned int oobsize;

	/* This is maybe an un-aligment address, only for malloc or free */
	char *buforg;
	char *buffer;

	int add_partition;

	/* BOOTROM read two bytes to detect the bad block flag */
#define HINFC_BAD_BLOCK_POS              0
	unsigned char *bbm;  /* nand bad block mark */
	unsigned short *epm;  /* nand empty page mark */

	unsigned int uc_er;

	void (*set_system_clock)(struct hifmc_host *host, int clk_en);

	void (*send_cmd_pageprog)(struct hifmc_host *host);
	void (*send_cmd_status)(struct hifmc_host *host);
	void (*send_cmd_readstart)(struct hifmc_host *host);
	void (*send_cmd_erase)(struct hifmc_host *host);
	void (*send_cmd_readid)(struct hifmc_host *host);
	void (*send_cmd_reset)(struct hifmc_host *host);
};

/*****************************************************************************/
int hifmc100_host_init(struct hifmc_host *host);

void hifmc100_nand_init(struct nand_chip *chip);

int hifmc100_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip,
			struct nand_flash_dev_ex *flash_dev_ex);

void hifmc100_ecc0_switch(struct hifmc_host *host, unsigned char op);

/******************************************************************************/
#endif /* HIFMC100H */

