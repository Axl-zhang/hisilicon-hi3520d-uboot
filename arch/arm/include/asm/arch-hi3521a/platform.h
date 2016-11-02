#ifndef	__HI_CHIP_REGS_H__
#define	__HI_CHIP_REGS_H__

#define RAM_START_ADRS			0x04010500
#define FMC_REG_BASE			0x10000000
#define FMC_ADDR_MODE_REG		0x8c

#define TIMER0_REG_BASE			0x12000000
#define TIMER1_REG_BASE			0x12000020
#define TIMER2_REG_BASE			0x12010000
#define TIMER3_REG_BASE			0x12010020
#define REG_TIMER_RELOAD		0x0
#define REG_TIMER_VALUE			0x4
#define REG_TIMER_CONTROL		0x8

#define CRG_REG_BASE			0x12040000
#define REG_CRG13			0x34
#define FMC_CRG29			0x74
#define PERI_CRG33			0x84
#define UART_CKSEL_MASK			(0x3 << 18)
#define UART_CKSEL_24M			(0x2 << 18)
#define REG_ETH_CRG			0x78
#define REG_ETH_MAC_IF			0x8C

#define SYS_CTRL_REG_BASE		0x12050000
#define REG_BASE_SCTL                   SYS_CTRL_REG_BASE
#define REG_SC_CTRL			0
#define REG_SC_SYSRES			0x4
#define REG_SC_GEN0			0x0138
#define REG_SC_GEN1			0x013c
#define REG_SC_GEN2			0x0140
#define REG_SC_GEN3			0x0144
#define REG_SC_GEN4			0x0148

#define WDG_REG_BASE			0x12070000
#define UART0_REG_BASE			0x12080000
#define UART1_REG_BASE			0x12090000
#define UART2_REG_BASE			0x120A0000
#define RTC_REG_BASE			0x120B0000
#define IO_CONFIG_REG_BASE		0x120F0000
#define DDRC0_REG_BASE			0x12110000
#define MISC_REG_BASE			0x12120000
#define IR_REG_BASE			0x12140000
#define GPIO0_REG_BASE			0x12150000
#define GPIO1_REG_BASE			0x12160000
#define GPIO2_REG_BASE			0x12170000
#define GPIO3_REG_BASE			0x12180000
#define GPIO4_REG_BASE			0x12190000
#define GPIO5_REG_BASE			0x121A0000
#define GPIO6_REG_BASE			0x121B0000
#define GPIO7_REG_BASE			0x121C0000
#define GPIO8_REG_BASE			0x121D0000
#define GPIO9_REG_BASE			0x121E0000
#define GPIO10_REG_BASE			0x121F0000
#define GPIO11_REG_BASE			0x12200000
#define GPIO12_REG_BASE			0x12210000
#define GPIO13_REG_BASE			0x12220000
#define FMC_MEM_BASE			0x14000000
#define DDR_MEM_BASE			0x80000000

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define REG_SYSSTAT			0x008C
#define BOOT_FROM_NAND	2
#define BOOT_FROM_DDR	1
#define BOOT_FROM_SPI	0

/********** Communication Register and flag used by bootrom *************/
#define REG_START_FLAG         (SYS_CTRL_REG_BASE + REG_SC_GEN1)
#define CONFIG_START_MAGIC     (0x444f574e)

/*-----------------------------------------------------------------------
 * DDR Training
 * ----------------------------------------------------------------------*/

#define STACK_TRAINING			0x04014000
#define _HI3521A_V100                  (0x003521a100LL)
#define _HI3521A_MASK                  (0xFFFFFFFFFFLL)
/******************************************************************************/

/*-----------------------------------------------------------------------
 * FMC
 * ----------------------------------------------------------------------*/

/*****************************************************************************/
#define FMC_CRG29_CLK_SEL(_clk)			(((_clk) & 0x3) << 2)
#define FMC_CRG29_CLK_EN			(1 << 1)
#define FMC_CRG29_SOFT_RST_REQ			(1 << 0)

#define FMC_CLK_SEL_MASK			(0x3 << 2)

#define CLK_24M					0x0
#define CLK_83M					0x1
#define CLK_150M				0x2

#define FMC_CLK_SEL_24M		FMC_CRG29_CLK_SEL(CLK_24M)
#define FMC_CLK_SEL_83M		FMC_CRG29_CLK_SEL(CLK_83M)
#define FMC_CLK_SEL_150M	FMC_CRG29_CLK_SEL(CLK_150M)

/*****************************************************************************/
#define FMC_CFG					0x00
#define FMC_CFG_SPI_NOR_ADDR_MODE(_mode)	((_mode) << 10)
#define FMC_CFG_BLOCK_SIZE(_size)		(((_size) & 0x3) << 8)
#define FMC_CFG_ECC_TYPE(_type)			(((_type) & 0x7) << 5)
#define FMC_CFG_PAGE_SIZE(_size)		(((_size) & 0x3) << 3)
#define FMC_CFG_FLASH_SEL(_type)		(((_type) & 0x3) << 1)
#define FMC_CFG_OP_MODE(_mode)			((_mode) & 0x1)

#define SPI_NOR_ADDR_MODE_3_BYTES		0x0
#define SPI_NOR_ADDR_MODE_4_BYTES		0x1

#define SPI_NOR_ADDR_MODE_SHIFT			10
#define SPI_NOR_ADDR_MODE_MASK			(0x1 << SPI_NOR_ADDR_MODE_SHIFT)

#define BLOCK_SIZE_64_PAGE			0x0

#define BLOCK_SIZE_SHIFT			8
#define BLOCK_SIZE_MASK				(0x3 << BLOCK_SIZE_SHIFT)

#define ECC_TYPE_0BIT				0x0
#define ECC_TYPE_8BIT				0x1
#define ECC_TYPE_16BIT				0x2
#define ECC_TYPE_24BIT				0x3
#define ECC_TYPE_28BIT				0x4

#define ECC_TYPE_SHIFT				5
#define ECC_TYPE_MASK				(0x7 << ECC_TYPE_SHIFT)

#define PAGE_SIZE_2KB				0x0
#define PAGE_SIZE_4KB				0x1

#define PAGE_SIZE_SHIFT				3
#define PAGE_SIZE_MASK				(0x3 << PAGE_SIZE_SHIFT)

#define FLASH_TYPE_SPI_NOR			0x0
#define FLASH_TYPE_SPI_NAND			0x1

#define FLASH_SEL_SHIFT				1
#define FLASH_SEL_MASK				(0x3 << FLASH_SEL_SHIFT)

#define OP_MODE_BOOT				0x0
#define OP_MODE_NORMAL				0x1

#define OP_MODE_MASK				0x1

/*****************************************************************************/
#define FMC_GLOBAL_CFG				0x04
#define FMC_GLOBAL_CFG_WP_ENABLE		(1 << 6)

/*****************************************************************************/
#define FMC_TIMING_SPI_CFG			0x08
#define FMC_TIMING_SPI_CFG_TCSH(_n)		(((_n) & 0xf) << 8)
#define FMC_TIMING_SPI_CFG_TCSS(_n)		(((_n) & 0xf) << 4)
#define FMC_TIMING_SPI_CFG_TSHSL(_n)		((_n) & 0xf)

#define CS_HOLD_TIME				0x6
#define CS_SETUP_TIME				0x6
#define CS_DESELECT_TIME			0xf

/*****************************************************************************/
#define FMC_INT					0x18
#define FMC_INT_AHB_OP				(1 << 7)
#define FMC_INT_DMA_ERR				(1 << 5)
#define FMC_INT_ERR_ALARM			(1 << 4)
#define FMC_INT_ERR_INVALID			(1 << 3)
#define FMC_INT_ERR_VALID			(1 << 2)
#define FMC_INT_OP_FAIL				(1 << 1)
#define FMC_INT_OP_DONE				(1 << 0)

/*****************************************************************************/
#define FMC_INT_EN				0x1c
#define FMC_INT_EN_AHB_OP			(1 << 7)
#define FMC_INT_EN_DMA_ERR			(1 << 5)
#define FMC_INT_EN_ERR_ALARM			(1 << 4)
#define FMC_INT_EN_ERR_INVALID			(1 << 3)
#define FMC_INT_EN_ERR_VALID			(1 << 2)
#define FMC_INT_EN_OP_FAIL			(1 << 1)
#define FMC_INT_EN_OP_DONE			(1 << 0)

/*****************************************************************************/
#define FMC_INT_CLR				0x20
#define FMC_INT_CLR_AHB_OP			(1 << 7)
#define FMC_INT_CLR_DMA_ERR			(1 << 5)
#define FMC_INT_CLR_ERR_ALARM			(1 << 4)
#define FMC_INT_CLR_ERR_INVALID			(1 << 3)
#define FMC_INT_CLR_ERR_VALID			(1 << 2)
#define FMC_INT_CLR_OP_FAIL			(1 << 1)
#define FMC_INT_CLR_OP_DONE			(1 << 0)

#define FMC_INT_CLR_ALL				0xbf

/*****************************************************************************/
#define FMC_CMD					0x24
#define FMC_CMD_CMD2(_cmd)			(((_cmd) & 0xff) << 8)
#define FMC_CMD_CMD1(_cmd)			((_cmd) & 0xff)

/*****************************************************************************/
#define FMC_ADDRH				0x28
#define FMC_ADDRH_SET(_addr)			((_addr) & 0xff)

/*****************************************************************************/
#define FMC_ADDRL				0x2c
#define FMC_ADDRL_BLOCK_H_MASK(_page)		(((_page) & 0xffff) << 16)
#define FMC_ADDRL_BLOCK_L_MASK(_page)		((_page) & 0xffc0)

#define READ_ID_ADDR				0x00
#define PROTECTION_ADDR				0xa0
#define FEATURE_ADDR				0xb0
#define STATUS_ADDR				0xc0

/*****************************************************************************/
#define FMC_OP_CFG				0x30
#define FMC_OP_CFG_FM_CS(_cs)			((_cs) << 11)
#define FMC_OP_CFG_FORCE_CS_EN(_en)		((_en) << 10)
#define FMC_OP_CFG_MEM_IF_TYPE(_type)		(((_type) & 0x7) << 7)
#define FMC_OP_CFG_ADDR_NUM(_addr)		(((_addr) & 0x7) << 4)
#define FMC_OP_CFG_DUMMY_NUM(_dummy)		((_dummy) & 0xf)

#define IF_TYPE_STD				0x0
#define IF_TYPE_DUAL				0x1
#define IF_TYPE_DIO				0x2
#define IF_TYPE_QUAD				0x3
#define IF_TYPE_QIO				0x4

#define IF_TYPE_SHIFT				7
#define IF_TYPE_MASK				(0x7 << IF_TYPE_SHIFT)

#define READ_ID_ADDR_NUM			1
#define FEATURES_OP_ADDR_NUM			1
#define STD_OP_ADDR_NUM				3

/*****************************************************************************/
#define FMC_SPI_OP_ADDR				0x34

/*****************************************************************************/
#define FMC_DATA_NUM				0x38
#define FMC_DATA_NUM_CNT(_n)			((_n) & 0x3fff)

#define FEATURES_DATE_NUM			1
#define READ_OOB_BB_LEN				1
#define MAX_SPI_NOR_ID_LEN			8
#define MAX_SPI_NAND_ID_LEN			3

/*****************************************************************************/
#define FMC_OP					0x3c
#define FMC_OP_DUMMY_EN(_en)			((_en) << 8)
#define FMC_OP_CMD1_EN(_en)			((_en) << 7)
#define FMC_OP_ADDR_EN(_en)			((_en) << 6)
#define FMC_OP_WRITE_DATE_EN(_en)		((_en) << 5)
#define FMC_OP_CMD2_EN(_en)			((_en) << 4)
#define FMC_OP_WAIT_READY_EN(_en)		((_en) << 3)
#define FMC_OP_READ_DATE_EN(_en)		((_en) << 2)
#define FMC_OP_READ_STATUS_EN(_en)		((_en) << 1)
#define FMC_OP_REG_OP_START			1

/*****************************************************************************/
#define FMC_DMA_LEN				0x40
#define FMC_DMA_LEN_SET(_len)			((_len) & 0x0fffffff)

/*****************************************************************************/
#define FMC_DMA_AHB_CTRL			0x48
#define FMC_DMA_AHB_CTRL_DMA_PP_EN		(1 << 3)
#define FMC_DMA_AHB_CTRL_BURST16_EN		(1 << 2)
#define FMC_DMA_AHB_CTRL_BURST8_EN		(1 << 1)
#define FMC_DMA_AHB_CTRL_BURST4_EN		1

#define ALL_BURST_ENABLE			(FMC_DMA_AHB_CTRL_BURST16_EN \
						| FMC_DMA_AHB_CTRL_BURST8_EN \
						| FMC_DMA_AHB_CTRL_BURST4_EN)

/*****************************************************************************/
#define FMC_DMA_SADDR_D0			0x4c

/*****************************************************************************/
#define FMC_DMA_SADDR_D1			0x50

/*****************************************************************************/
#define FMC_DMA_SADDR_D2			0x54

/*****************************************************************************/
#define FMC_DMA_SADDR_D3			0x58

/*****************************************************************************/
#define FMC_DMA_SADDR_OOB			0x5c

/*****************************************************************************/
#define FMC_DMA_BLK_SADDR			0x60
#define FMC_DMA_BLK_SADDR_SET(_addr)		((_addr) & 0xffffff)

/*****************************************************************************/
#define FMC_DMA_BLK_LEN				0x64
#define FMC_DMA_BLK_LEN_SET(_len)		((_len) & 0xffff)

/*****************************************************************************/
#define FMC_OP_CTRL				0x68
#define FMC_OP_CTRL_RD_OPCODE(_code)		(((_code) & 0xff) << 16)
#define FMC_OP_CTRL_WR_OPCODE(_code)		(((_code) & 0xff) << 8)
#define FMC_OP_CTRL_RD_OP_SEL(_op)		(((_op) & 0x3) << 4)
#define FMC_OP_CTRL_DMA_OP(_type)		((_type) << 2)
#define FMC_OP_CTRL_RW_OP(_op)			((_op) << 1)
#define FMC_OP_CTRL_DMA_OP_READY		1

#define RD_OP_READ_ALL_PAGE			0x0
#define RD_OP_READ_OOB				0x1
#define RD_OP_BLOCK_READ			0x2

#define RD_OP_SHIFT				4
#define RD_OP_MASK				(0x3 << RD_OP_SHIFT)

#define OP_TYPE_DMA				0x0
#define OP_TYPE_REG				0x1

#define RW_OP_READ				0x0
#define RW_OP_WRITE				0x1

/*****************************************************************************/
#define FMC_OP_PARA				0x70
#define FMC_OP_PARA_RD_OOB_ONLY			(1 << 1)

/*****************************************************************************/
#define FMC_BOOT_SET				0x74
#define FMC_BOOT_SET_DEVICE_ECC_EN		(1 << 3)
#define FMC_BOOT_SET_BOOT_QUAD_EN		(1 << 1)

/*****************************************************************************/
#define FMC_STATUS				0xac

#define GET_OP					0
#define SET_OP					1

#define PROTECTION_BRWD_MASK			(1 << 7)
#define PROTECTION_BP3_MASK			(1 << 6)
#define PROTECTION_BP2_MASK			(1 << 5)
#define PROTECTION_BP1_MASK			(1 << 4)
#define PROTECTION_BP0_MASK			(1 << 3)

#define ANY_BP_ENABLE(_val)		((PROTECTION_BP3_MASK & _val) \
					|| (PROTECTION_BP2_MASK & _val) \
					|| (PROTECTION_BP1_MASK & _val) \
					|| (PROTECTION_BP0_MASK & _val))

#define ALL_BP_MASK				(PROTECTION_BP3_MASK \
						| PROTECTION_BP2_MASK \
						| PROTECTION_BP1_MASK \
						| PROTECTION_BP0_MASK)

#define FEATURE_ECC_ENABLE			(1 << 4)
#define FEATURE_QE_ENABLE			(1 << 0)

#define STATUS_ECC_MASK				(0x3 << 4)
#define STATUS_P_FAIL_MASK			(1 << 3)
#define STATUS_E_FAIL_MASK			(1 << 2)
#define STATUS_WEL_MASK				(1 << 1)
#define STATUS_OIP_MASK				(1 << 0)

/*****************************************************************************/
#define FMC_VERSION				0xbc

#define HIFMC_VER_100				(0x100)

#undef  DISABLE
#define DISABLE					0

#undef  ENABLE
#define ENABLE					1

/*****************************************************************************/
#define hifmc_read(_host, _reg) \
	readl((unsigned)((char *)_host->regbase + (_reg)))

#define hifmc_write(_host, _reg, _value) \
	writel((unsigned)(_value), (unsigned)((char *)_host->regbase + (_reg)))

/*****************************************************************************/
#ifndef DBG_MSG
#define DBG_MSG(_fmt, arg...) \
	printf("%s(%d): " _fmt, __FILE__, __LINE__, ##arg);
#endif

#ifndef DBG_BUG
	#define DBG_BUG(fmt, args...) \
	do { \
		printf("%s(%d): BUG: " fmt, __FILE__, __LINE__, ##args); \
		while (1) \
			; \
	} while (0)
#endif

/*****************************************************************************/
#define FMC_WAIT_TIMEOUT 10000000

#define FMC_CMD_WAIT_CPU_FINISH(_host) \
	do { \
		unsigned regval, timeout = FMC_WAIT_TIMEOUT; \
		do { \
			regval = hifmc_read((_host), FMC_OP); \
			--timeout; \
		} while ((regval & FMC_OP_REG_OP_START) && timeout); \
		if (!timeout) \
			DBG_BUG("Wait cmd cpu finish timeout!\n"); \
	} while (0)

/*****************************************************************************/
#define FMC_DMA_WAIT_INT_FINISH(_host) \
	do { \
		unsigned regval, timeout = FMC_WAIT_TIMEOUT; \
		do { \
			regval = hifmc_read((_host), FMC_INT); \
			--timeout; \
		} while ((!(regval & FMC_INT_OP_DONE) && timeout)); \
		if (!timeout) { \
			debug_register_dump(); \
			DBG_BUG("Wait dma int finish timeout!\n"); \
		} \
	} while (0)

/*****************************************************************************/
#define FMC_DMA_WAIT_CPU_FINISH(_host) \
	do { \
		unsigned regval, timeout = FMC_WAIT_TIMEOUT; \
		do { \
			regval = hifmc_read((_host), FMC_OP_CTRL); \
			--timeout; \
		} while ((regval & FMC_OP_CTRL_DMA_OP_READY) && timeout); \
		if (!timeout) { \
			debug_register_dump(); \
			DBG_BUG("Wait dma cpu finish timeout\n"); \
		} \
	} while (0)

#endif /* End of __HI_CHIP_REGS_H__ */

