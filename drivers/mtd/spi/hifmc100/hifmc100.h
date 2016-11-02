/******************************************************************************
*    Copyright (c) 2009-2010 by Hisilicon.
*    All rights reserved.
******************************************************************************/

#ifndef HIFMC100H
#define HIFMC100H


/*****************************************************************************/
#define HIFMC100_DMA_MAX_SIZE			(4096)
#define HIFMC100_DMA_MAX_MASK			(HIFMC100_DMA_MAX_SIZE - 1)

/*****************************************************************************/
#define HIFMC100_SPI_NOR_SUPPORT_READ (SPI_IF_READ_STD \
		| SPI_IF_READ_FAST \
		| SPI_IF_READ_DUAL \
		| SPI_IF_READ_DUAL_ADDR \
		| SPI_IF_READ_QUAD \
		| SPI_IF_READ_QUAD_ADDR)

#define HIFMC100_SPI_NOR_SUPPORT_WRITE (SPI_IF_WRITE_STD \
		| SPI_IF_WRITE_DUAL \
		| SPI_IF_WRITE_DUAL_ADDR \
		| SPI_IF_WRITE_QUAD \
		| SPI_IF_WRITE_QUAD_ADDR)

#define HIFMC100_SPI_NOR_SUPPORT_MAX_DUMMY        (7)

/*****************************************************************************/
/* These macroes are for debug only, reg read is slower then dma read */
#undef HIFMC100_SPI_NOR_SUPPORT_REG_READ
/* #define HIFMC100_SPI_NOR_SUPPORT_REG_READ */

#undef HIFMC100_SPI_NOR_SUPPORT_REG_WRITE
/* #define HIFMC100_SPI_NOR_SUPPORT_REG_WRITE */

/*****************************************************************************/
#undef  READ
#define READ           1

#undef  WRITE
#define WRITE          0

#define SPI_NOR_SR_LEN		1	/* Status Register length(byte) */
#define SPI_NOR_CR_LEN		1	/* Config Register length(byte) */

#define SPI_NOR_CR_SHIFT	8	/* Config Register shift(bit) */

#define SPI_NOR_CR_4BYTE_SHIFT	5
#define SPI_NOR_CR_4BYTE_MASK		(1 << SPI_NOR_CR_4BYTE_SHIFT)
#define SPI_NOR_GET_4BYTE_BY_CR(cr)	(((cr) & SPI_NOR_CR_4BYTE_MASK) \
						>> SPI_NOR_CR_4BYTE_SHIFT)

#define SPI_NOR_CR_QE_SHIFT		1
#define SPI_NOR_CR_QE_MASK		(1 << SPI_NOR_CR_QE_SHIFT)
#define SPI_NOR_GET_QE_BY_CR(cr)	(((cr) & SPI_NOR_CR_QE_MASK) \
						>> SPI_NOR_CR_QE_SHIFT)

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
#define DEBUG_SPI_NOR_BP	0

#define SPI_NOR_SR_SRWD_SHIFT	7
#define SPI_NOR_SR_SRWD_MASK	(1 << SPI_NOR_SR_SRWD_SHIFT)

#define SPI_NOR_SR_BP0_SHIFT	2
#define SPI_NOR_SR_BP_WIDTH	0xf
#define SPI_NOR_SR_BP_MASK	(SPI_NOR_SR_BP_WIDTH << SPI_NOR_SR_BP0_SHIFT)

#define SPI_NOR_SR_TB_SHIFT	3
#define SPI_NOR_SR_TB_MASK	(1 << SPI_NOR_SR_TB_SHIFT)

#endif /* CONFIG_CMD_SPI_BLOCK_PROTECTION */

#define SPI_NOR_SR_WIP_MASK	(1 << 0)

/*****************************************************************************/
struct hifmc_spi;

struct spi_driver {
	int (*wait_ready)(struct hifmc_spi *spi);
	int (*write_enable)(struct hifmc_spi *spi);
	int (*entry_4addr)(struct hifmc_spi *spi, int enable);
	int (*bus_prepare)(struct hifmc_spi *spi, int op);
	int (*qe_enable)(struct hifmc_spi *spi);
};

struct hifmc_spi {
	char *name;
	int chipselect;
	unsigned long long chipsize;
	unsigned int erasesize;
	unsigned int addrcycle;

	struct spi_operation  read[1];
	struct spi_operation  write[1];
	struct spi_operation  erase[MAX_SPI_OP];
	void *host;
	void *iobase;
	struct spi_driver *driver;
};

struct hifmc_host {
	struct spi_flash spiflash[1];
	void  *regbase;
	void  *iobase;
	unsigned int erasesize;

	void (*set_system_clock)(struct hifmc_host *host,
					struct spi_operation *op, int clk_en);

	void (*set_host_addr_mode)(struct hifmc_host *host, int enable);

	unsigned int dma_buffer;
	int num_chip;
	struct hifmc_spi spi[CONFIG_HIFMC100_CHIP_NUM + 1];
	char buffer[HIFMC100_DMA_MAX_SIZE];
#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
	unsigned int start_addr;
	unsigned int end_addr;
	unsigned char cmp;
	unsigned char level;
#endif
};

#ifdef CONFIG_CMD_SPI_BLOCK_PROTECTION
unsigned hifmc100_get_spi_lock_info(unsigned char *cmp, unsigned char *level);
void hifmc100_spi_lock(unsigned char cmp, unsigned char level);
extern u_char spi_general_get_flash_register(struct hifmc_spi *spi, u_char cmd);
#endif

#define SPIFLASH_TO_HOST(_spiflash)     ((struct hifmc_host *)(_spiflash))

/*****************************************************************************/
/* Boot init debug print */
#define BT_DBG				0
/* OP command debug print */
#define OP_DBG				0
/* Write debug print */
#define WR_DBG				0
/* Read debug print */
#define RD_DBG				0
/* DMA read or write debug print */
#define DMA_DBG				0
/* Quad Enable debug print */
#define QE_DBG				0
/* 3-4byte Address Cycle */
#define AC_DBG				0
/* Status Register debug print */
#define SR_DBG				0
/* Config Register debug print */
#define CR_DBG				0
/* Write Enable debug print */
#define WE_DBG				0
/* Block Protection debug print */
#define BP_DBG				0

/*****************************************************************************/
#define BT_MSG(_fmt, arg...) \
	do { \
		if (BT_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define OP_MSG(_fmt, arg...) \
	do { \
		if (OP_DBG) \
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

#define DMA_MSG(_fmt, arg...) \
	do { \
		if (DMA_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define QE_MSG(_fmt, arg...) \
	do { \
		if (QE_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define AC_MSG(_fmt, arg...) \
	do { \
		if (AC_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define SR_MSG(_fmt, arg...) \
	do { \
		if (SR_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define CR_MSG(_fmt, arg...) \
	do { \
		if (CR_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define WE_MSG(_fmt, arg...) \
	do { \
		if (WE_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

#define BP_MSG(_fmt, arg...) \
	do { \
		if (BP_DBG) \
			DBG_MSG(_fmt, ##arg) \
	} while (0)

/*****************************************************************************/
extern struct spi_info hifmc100_spi_info_table[];

/*****************************************************************************/
#endif /* HIFMC100H */

