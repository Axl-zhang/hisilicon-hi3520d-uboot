/******************************************************************************
 *    Copyright (c) 2014 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create By hisilicon
******************************************************************************/

#include <linux/mtd/nand.h>
#include <exports.h>
#include "hifmc100_spi_ids.h"
#include "hinfc_common.h"
#include "hifmc100.h"

#include "hifmc100_spi_general.c"

/*****************************************************************************/
#define SET_READ_STD(_dummy_, _size_, _clk_) \
	static struct spi_op_info read_std_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_STD, SPI_CMD_READ_STD, _dummy_, _size_, _clk_ }

#define READ_STD(_dummy_, _size_, _clk_) read_std_##_dummy_##_size_##_clk_

/*****************************************************************************/
#define SET_READ_FAST(_dummy_, _size_, _clk_) \
	static struct spi_op_info read_fast_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_FAST, SPI_CMD_READ_FAST, _dummy_, _size_, _clk_ }

#define READ_FAST(_dummy_, _size_, _clk_) read_fast_##_dummy_##_size_##_clk_

/*****************************************************************************/
#define SET_READ_DUAL(_dummy_, _size_, _clk_) \
	static struct spi_op_info read_dual_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_DUAL, SPI_CMD_READ_DUAL, _dummy_, _size_, _clk_ }

#define READ_DUAL(_dummy_, _size_, _clk_) read_dual_##_dummy_##_size_##_clk_

/*****************************************************************************/
#define SET_READ_DUAL_ADDR(_dummy_, _size_, _clk_) \
	static struct spi_op_info read_dual_addr_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_DUAL_ADDR, SPI_CMD_READ_DUAL_ADDR, _dummy_, _size_, _clk_ }

#define READ_DUAL_ADDR(_dummy_, _size_, _clk_) \
	read_dual_addr_##_dummy_##_size_##_clk_

/*****************************************************************************/
#define SET_READ_QUAD(_dummy_, _size_, _clk_) \
	static struct spi_op_info read_quad_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_QUAD, SPI_CMD_READ_QUAD, _dummy_, _size_, _clk_ }

#define READ_QUAD(_dummy_, _size_, _clk_) read_quad_##_dummy_##_size_##_clk_

/*****************************************************************************/
#define SET_READ_QUAD_ADDR(_dummy_, _size_, _clk_) \
	static struct spi_op_info read_quad_addr_##_dummy_##_size_##_clk_ = { \
	SPI_IF_READ_QUAD_ADDR, SPI_CMD_READ_QUAD_ADDR, _dummy_, _size_, _clk_ }

#define READ_QUAD_ADDR(_dummy_, _size_, _clk_) \
	read_quad_addr_##_dummy_##_size_##_clk_

/*****************************************************************************/
#define SET_WRITE_STD(_dummy_, _size_, _clk_) \
	static struct spi_op_info write_std_##_dummy_##_size_##_clk_ = { \
	SPI_IF_WRITE_STD, SPI_CMD_WRITE_STD, _dummy_, _size_, _clk_ }

#define WRITE_STD(_dummy_, _size_, _clk_) write_std_##_dummy_##_size_##_clk_

/*****************************************************************************/
#define SET_WRITE_QUAD(_dummy_, _size_, _clk_) \
	static struct spi_op_info write_quad_##_dummy_##_size_##_clk_ = { \
	SPI_IF_WRITE_QUAD, SPI_CMD_WRITE_QUAD, _dummy_, _size_, _clk_ }

#define WRITE_QUAD(_dummy_, _size_, _clk_) \
	write_quad_##_dummy_##_size_##_clk_

/*****************************************************************************/
#define SET_ERASE_SECTOR_128K(_dummy_, _size_, _clk_) \
	static struct spi_op_info erase_sector_128k_##_dummy_##_size_##_clk_ \
	= { SPI_IF_ERASE_SECTOR_128K, SPI_CMD_SE_128K, _dummy_, _size_, _clk_ }

#define ERASE_SECTOR_128K(_dummy_, _size_, _clk_) \
	erase_sector_128k_##_dummy_##_size_##_clk_

#define SET_ERASE_SECTOR_256K(_dummy_, _size_, _clk_) \
	static struct spi_op_info erase_sector_256k_##_dummy_##_size_##_clk_ \
	= { SPI_IF_ERASE_SECTOR_256K, SPI_CMD_SE_256K, _dummy_, _size_, _clk_ }

#define ERASE_SECTOR_256K(_dummy_, _size_, _clk_) \
	erase_sector_256k_##_dummy_##_size_##_clk_

/*****************************************************************************/
SET_READ_STD(1, INFINITE, 24);

SET_READ_FAST(1, INFINITE, 104);
SET_READ_FAST(1, INFINITE, 120);

SET_READ_DUAL(1, INFINITE, 104);
SET_READ_DUAL(1, INFINITE, 120);

SET_READ_DUAL_ADDR(1, INFINITE, 104);
SET_READ_DUAL_ADDR(1, INFINITE, 120);

SET_READ_QUAD(1, INFINITE, 104);
SET_READ_QUAD(1, INFINITE, 120);

SET_READ_QUAD_ADDR(2, INFINITE, 104);
SET_READ_QUAD_ADDR(1, INFINITE, 120);

/*****************************************************************************/
SET_WRITE_STD(0, 256, 24);

SET_WRITE_QUAD(0, 256, 104);
SET_WRITE_QUAD(0, 256, 120);

/*****************************************************************************/
SET_ERASE_SECTOR_128K(0, _128K, 24);

SET_ERASE_SECTOR_256K(0, _256K, 24);

/*****************************************************************************/
static struct spi_nand_driver spi_nand_driver_general = {
	.wait_ready   = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.qe_enable = spi_general_qe_enable,
};

#include "hifmc100_spi_nand_esmt.c"
static struct spi_nand_driver  spi_nand_driver_esmt = {
	.wait_ready   = spi_general_wait_ready,
	.write_enable = spi_general_write_enable,
	.qe_enable = spi_nand_esmt_qe_enable,
};

/*****************************************************************************/
#define SPI_NAND_ID_TAB_VER		"1.3"

/******* SPI Nand ID Table ***************************************************
* Version	Manufacturer	Chip Name	Size		Operation
*  1.0		ESMT		F50L512M41A	64MB		Add 5 chip
*		GD		5F1GQ4UAYIG	128MB
*		GD		5F2GQ4UAYIG	256MB
*		GD		5F4GQ4UAYIG	512MB
*		GD		5F4GQ4UBYIG	512MB
*  1.1		ESMT		F50L1G41A	128MB		Add 2 chip
*		Winbond		W25N01GV	128MB
*  1.2		GD		5F1GQ4UBYIG	128MB		Add 2 chip
*		GD		5F2GQ4UBYIG	256MB
*  1.3		ATO		ATO25D1GA	128MB		Add 1 chip
******************************************************************************/
struct hifmc_chip_info hifmc_spi_nand_flash_table[] = {
	/* ESMT F50L512M41A 512Mbit */
	{
		.name      = "F50L512M41A",
		.id        = {0xC8, 0x20},
		.id_len    = 2,
		.chipsize  = _64M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos    = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_nand_driver_esmt,
	},

	/* ESMT F50L1G41A 1Gbit */
	{
		.name      = "F50L1G41A",
		.id        = {0xC8, 0x21},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos    = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_nand_driver_esmt,
	},

	/* GD 5F1GQ4UAYIG 1Gbit */
	{
		.name      = "5F1GQ4UAYIG",
		.id        = {0xc8, 0xf1},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos    = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_nand_driver_general,
	},

	/* GD 5F1GQ4UBYIG 1Gbit */
	{
		.name      = "5F1GQ4UBYIG",
		.id        = {0xc8, 0xd1},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos    = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_nand_driver_general,
	},

	/* GD 5F2GQ4UAYIG 2Gbit */
	{
		.name      = "5F2GQ4UAYIG",
		.id        = {0xc8, 0xf2},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos    = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_nand_driver_general,
	},

	/* GD 5F2GQ4UBYIG 2Gbit */
	{
		.name      = "5F2GQ4UBYIG",
		.id        = {0xc8, 0xd2},
		.id_len    = 2,
		.chipsize  = _256M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 128,
		.badblock_pos    = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_nand_driver_general,
	},

	/* GD 5F4GQ4UAYIG 4Gbit */
	{
		.name      = "5F4GQ4UAYIG",
		.id        = {0xc8, 0xf4},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos    = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_nand_driver_general,
	},

	/* GD 5F4GQ4UBYIG 4Gbit */
	{
		.name      = "5F4GQ4UBYIG",
		.id        = {0xc8, 0xd4},
		.id_len    = 2,
		.chipsize  = _512M,
		.erasesize = _256K,
		.pagesize  = _4K,
		.oobsize   = 256,
		.badblock_pos    = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 120),
			&READ_DUAL(1, INFINITE, 120),
			&READ_DUAL_ADDR(1, INFINITE, 120),
			&READ_QUAD(1, INFINITE, 120),
			&READ_QUAD_ADDR(1, INFINITE, 120),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 120),
			0
		},
		.erase     = {
			&ERASE_SECTOR_256K(0, _256K, 24),
			0
		},
		.driver    = &spi_nand_driver_general,
	},

	/* Winbond W25N01GV 1Gbit */
	{
		.name      = "W25N01GV",
		.id        = {0xef, 0xaa, 0x21},
		.id_len    = 3,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos    = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_DUAL(1, INFINITE, 104),
			&READ_DUAL_ADDR(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			&READ_QUAD_ADDR(2, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_nand_driver_general,
	},

	/* ATO ATO25D1GA 1Gbit */
	{
		.name      = "ATO25D1GA",
		.id        = {0x9b, 0x12},
		.id_len    = 2,
		.chipsize  = _128M,
		.erasesize = _128K,
		.pagesize  = _2K,
		.oobsize   = 64,
		.badblock_pos    = BBP_FIRST_PAGE,
		.read      = {
			&READ_STD(1, INFINITE, 24),
			&READ_FAST(1, INFINITE, 104),
			&READ_QUAD(1, INFINITE, 104),
			0
		},
		.write     = {
			&WRITE_STD(0, 256, 24),
			&WRITE_QUAD(0, 256, 104),
			0
		},
		.erase     = {
			&ERASE_SECTOR_128K(0, _128K, 24),
			0
		},
		.driver    = &spi_nand_driver_general,
	},

	{	.id_len    = 0,	},
};

/*****************************************************************************/
static void hifmc100_spi_search_rw(struct hifmc_chip_info *spiinfo,
				struct spi_op_info *spiop_rw, unsigned iftype,
				unsigned max_dummy, int rw_type)
{
	int ix = 0;
	struct spi_op_info **spiop, **fitspiop;

	for (fitspiop = spiop = (rw_type ? spiinfo->write : spiinfo->read);
		(*spiop) && ix < MAX_SPI_NAND_OP; spiop++, ix++)
		if (((*spiop)->iftype & iftype)
			&& ((*spiop)->dummy <= max_dummy)
			&& (*fitspiop)->iftype < (*spiop)->iftype) {
			fitspiop = spiop;
		}

	memcpy(spiop_rw, (*fitspiop), sizeof(struct spi_op_info));
}

/*****************************************************************************/
static void hifmc100_spi_get_erase(struct hifmc_chip_info *spiinfo,
				struct spi_op_info *spiop_erase)
{
	int ix;

	spiop_erase->size = 0;
	for (ix = 0; ix < MAX_SPI_NAND_OP; ix++) {
		if (spiinfo->erase[ix] == NULL)
			break;
		if (spiinfo->erasesize == spiinfo->erase[ix]->size) {
			memcpy(&spiop_erase[ix], spiinfo->erase[ix],
					sizeof(struct spi_op_info));
			break;
		}
	}
}

/*****************************************************************************/
static void hifmc100_map_iftype_and_clock(struct hifmc_op *spi)
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
		SPI_IF_WRITE_QUAD,      IF_TYPE_QUAD,
		0, 0,
	};

	for (ix = 0; iftype_write[ix]; ix += 2) {
		if (spi->write->iftype == iftype_write[ix]) {
			spi->write->iftype = iftype_write[ix + 1];
			break;
		}
	}
	hifmc100_spi_nand_get_best_clock(&spi->write->clock);

	for (ix = 0; iftype_read[ix]; ix += 2) {
		if (spi->read->iftype == iftype_read[ix]) {
			spi->read->iftype = iftype_read[ix + 1];
			break;
		}
	}
	hifmc100_spi_nand_get_best_clock(&spi->read->clock);

	hifmc100_spi_nand_get_best_clock(&spi->erase->clock);
	spi->erase->iftype = IF_TYPE_STD;
}

/*****************************************************************************/
static void hifmc100_spi_ids_probe(struct hifmc_host *host,
				struct hifmc_chip_info *spi_dev)
{
	unsigned reg;
	struct hifmc_op *spi = host->spi;

	spi->host = host;
	spi->driver = spi_dev->driver;

	hifmc100_spi_search_rw(spi_dev, spi->read,
			HIFMC100_SPI_NAND_SUPPORT_READ,
			HIFMC100_SPI_NAND_SUPPORT_MAX_DUMMY, SPI_NAND_READ);

	hifmc100_spi_search_rw(spi_dev, spi->write,
			HIFMC100_SPI_NAND_SUPPORT_WRITE,
			HIFMC100_SPI_NAND_SUPPORT_MAX_DUMMY, SPI_NAND_WRITE);

	hifmc100_spi_get_erase(spi_dev, spi->erase);
	hifmc100_map_iftype_and_clock(spi);

	spi->driver->qe_enable(spi);

	/* disable write protection */
	reg = spi_nand_feature_op(spi, GET_OP, PROTECTION_ADDR, 0);
	BT_MSG("* Get feature[%#x]%#x\n", PROTECTION_ADDR, reg);
	if (ANY_BP_ENABLE(reg)) {
		reg &= ~ALL_BP_MASK;
		BT_MSG(" Set feature[%#x]%#x\n", PROTECTION_ADDR, reg);
		spi_nand_feature_op(spi, SET_OP, PROTECTION_ADDR, reg);

		spi->driver->wait_ready(spi);

		reg = spi_nand_feature_op(spi, GET_OP, PROTECTION_ADDR, 0);
		BT_MSG("* Check feature[%#x]%#x\n", PROTECTION_ADDR, reg);
		if (ANY_BP_ENABLE(reg))
			DBG_MSG("Error: Write protection disable failed!\n");
	}

	/* disable chip internal ECC */
	reg = spi_nand_feature_op(spi, GET_OP, FEATURE_ADDR, 0);
	if (reg & FEATURE_ECC_ENABLE) {
		reg &= ~FEATURE_ECC_ENABLE;
		spi_nand_feature_op(spi, SET_OP, FEATURE_ADDR, reg);

		spi->driver->wait_ready(spi);

		reg = spi_nand_feature_op(spi, GET_OP, FEATURE_ADDR, 0);
		if (reg & FEATURE_ECC_ENABLE)
			DBG_MSG("Error: Chip internal ECC disable failed!\n");
	}
}

/*****************************************************************************/
static struct nand_flash_dev *spi_nand_get_flash_info(struct mtd_info *mtd,
		struct nand_chip *chip, struct nand_flash_dev_ex *flash_dev_ex)
{
	unsigned char ix;
	unsigned char *byte = flash_dev_ex->ids;
	struct hifmc_host *host = chip->priv;
	struct hifmc_chip_info *spi_dev = hifmc_spi_nand_flash_table;
	struct nand_flash_dev *flash_type = &flash_dev_ex->flash_dev;

	for (ix = 2; ix < MAX_SPI_NAND_ID_LEN; ix++)
		byte[ix] = chip->read_byte(mtd);

	printf("SPI Nand(cs %d) ID: %#x %#x", host->cmd_option.chipselect,
			byte[0], byte[1]);

	for (; spi_dev->id_len; spi_dev++) {
		if (memcmp(byte, spi_dev->id, spi_dev->id_len))
			continue;

		for (ix = 2; ix < spi_dev->id_len; ix++)
			printf(" %#x", byte[ix]);

		printf(" Name:\"%s\"\n", spi_dev->name);

		flash_type->name = spi_dev->name;
		flash_type->id = byte[1];
		flash_type->pagesize  = spi_dev->pagesize;
		flash_type->chipsize = spi_dev->chipsize >> 20;
		flash_type->erasesize = spi_dev->erasesize;
		flash_type->options = chip->options;

		flash_dev_ex->oobsize = spi_dev->oobsize;

		hifmc100_spi_ids_probe(host, spi_dev);

		return flash_type;
	}
	printf("\n");

	return NULL;
}

/*****************************************************************************/
void spi_nand_ids_register(void)
{
	printf("SPI Nand ID Table Version %s\n", SPI_NAND_ID_TAB_VER);
	nand_get_spl_flash_type = spi_nand_get_flash_info;
}

