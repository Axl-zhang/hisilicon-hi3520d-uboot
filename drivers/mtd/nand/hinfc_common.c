/******************************************************************************
*    Copyright (c) 2009-2014 by Hisilicon.
*    All rights reserved.
* ***
*    Create By Hisi.
*
******************************************************************************/

#include <common.h>
#include <nand.h>
#include <linux/mtd/nand.h>
#include <hinfc_common.h>
#include <match_table.h>

/*****************************************************************************/
struct nand_flash_dev *(*nand_get_spl_flash_type)(
	struct mtd_info *mtd,
	struct nand_chip *chip,
	struct nand_flash_dev_ex *flash_dev_ex) = NULL;

int (*nand_oob_resize)(struct mtd_info *mtd, struct nand_chip *chip,
	struct nand_flash_dev_ex *flash_dev_ex) = NULL;

/*****************************************************************************/
#if defined(CONFIG_NAND_FLASH_HISNFC100) \
	|| defined(CONFIG_NAND_FLASH_HINFC610) \
	|| defined(CONFIG_HIFMC_SPI_NAND_SUPPORT)
static struct match_type_str ecc2name[] = {
	{NAND_ECC_0BIT,  "none"     },
	{NAND_ECC_8BIT,  "4bit/512" },
	{NAND_ECC_16BIT, "8bit/512" },
	{NAND_ECC_24BIT, "24bit/1K" },
	{NAND_ECC_28BIT, "28bit/1K" },
	{NAND_ECC_40BIT, "40bit/1K" },
	{NAND_ECC_42BIT, "42bit/1K" },
	{NAND_ECC_64BIT, "64bit/1K" },
};

const char *nand_ecc_name(int type)
{
	return type2str(ecc2name, ARRAY_SIZE(ecc2name), type, "unknown");
}

/*****************************************************************************/
static struct match_type_str page2name[] = {
	{ NAND_PAGE_512B, "512" },
	{ NAND_PAGE_2K,   "2K" },
	{ NAND_PAGE_4K,   "4K" },
	{ NAND_PAGE_8K,   "8K" },
	{ NAND_PAGE_16K,  "16K" },
	{ NAND_PAGE_32K,  "32K" },
};

const char *nand_page_name(int type)
{
	return type2str(page2name, ARRAY_SIZE(page2name), type, "unknown");
}

/*****************************************************************************/
static struct match_reg_type page2size[] = {
	{ _512B, NAND_PAGE_512B },
	{ _2K, NAND_PAGE_2K },
	{ _4K, NAND_PAGE_4K },
	{ _8K, NAND_PAGE_8K },
	{ _16K, NAND_PAGE_16K },
	{ _32K, NAND_PAGE_32K },
};

int nandpage_size2type(int size)
{
	return reg2type(page2size, ARRAY_SIZE(page2size), size, NAND_PAGE_2K);
}

int nandpage_type2size(int size)
{
	return type2reg(page2size, ARRAY_SIZE(page2size), size, NAND_PAGE_2K);
}
#endif

