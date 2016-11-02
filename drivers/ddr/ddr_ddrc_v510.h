/**
 * ddr_ddrc_v510.h
 *
 * Copyright (c) 2009-2014, HiSilicon Technologies Co., Ltd.
 * All rights reserved.
 *
 * DDRC V500 register define.
 */


/******** DMC **************************/
/* base address: DDR_REG_BASE_DMC0 DDR_REG_BASE_DMC1 */
/* register offset address */
#define DDR_DMC_CFG_PD			0x28   /* PowerDown */
#define DDR_DMC_CFG_DDRMODE		0x50
#define DDR_DMC_CFG_EMRS01		0xf0
#define DDR_DMC_TIMING2			0x108
#define DDR_DMC_SFCREQ			0xc
#define DDR_DMC_SFCCMD			0x210
#define DDR_DMC_SFCADDR			0x214   /* read col and row */
#define DDR_DMC_SFCBANK			0x218
#define DDR_DMC_SFC_RDATA0		0x4A8   /* SFC read data[31:0] */
#define DDR_DMC_SFC_RDATA1		0x4AC   /* SFC read data[63:32] */
#define DDR_DMC_SFC_RDATA2		0x4B0   /* SFC read data[95:64] */
#define DDR_DMC_SFC_RDATA3		0x4B4   /* SFC read data[127:96] */
#define DDR_DMC_SFC_RDATA4		0x4B8   /* SFC read data[159:128] */
#define DDR_DMC_SFC_RDATA5		0x4BC   /* SFC read data[191:160] */
#define DDR_DMC_SFC_RDATA6		0x4C0   /* SFC read data[223:192] */
#define DDR_DMC_SFC_RDATA7		0x4C4   /* SFC read data[255:224] */

/* register mask */
#define DMC_CMD_MRS_MASK		0xffff
/* storing data bus width. [00]8bit, [01]16bit, [10]32bit, [11]64bit */
#define DMC_MEM_WIDTH_MASK		0x3
#define DMC_MRS_MASK			0xffff  /* [15:0] Mode Register mask */
#define DMC_MR0_BL_MASK			0x3
#define DMC_CFG_DRAM_TYPE_MASK	0xf /* [3:0]101:DDR2, 110:DDR3, 111:DDR4 */

/* register bit */
#define DMC_MEM_WIDTH_BIT		4       /* storing data bus width */
/* [CUSTOM] precharge disable/enable bit */
#define DMC_SFC_PRE_DIS_BIT		30
/* [CUSTOM] [29:12]config MR when LMR command */
#define DMC_SFC_CMD_MRS_BIT		12
#define DMC_SFC_RANK_BIT		16      /* [CUSTOM] [31:16]sfc_rank */

/* register value */
#define DMC_BANK_MR1			1
#define DMC_BANK_MR3			0x3
#define DMC_CMD_TYPE_LMR		0x2
#define DMC_CMD_TYPE_READ		0x5      /* read  */
#define DMC_CMD_TYPE_PRECHARGE_ALL	0x6      /* precharge all */
#define DMC_CMD_MRS_MR3			0x4      /* MR3 = 0x4 */
#define DMC_CMD_MRS_A7			0x80
/* value 1 means exexute command. cmd_rank[0] control DDR RANK0 */
#define DMC_CMD_RANK0			0x1
#define DMC_MR0_BL_BUST8		0x0      /* BC8 (fixed) */
#define DMC_MR0_BL_BUST4		0x2      /* BC4 (fixed) */
#define DMC_AUTO_TIMING_DIS		0xfffff000 /* auto refresh disable */
#define DMC_POWER_DOWN_DIS		0xfffffffe /* powerDown disable */
#define DMC_SCRAMB_DIS			0xffffbfff /* scramb disable */
#define DMC_CFG_DRAM_TYPE_DDR4	0x7        /* DDR4 */

#ifndef DDR_PHY_NUM
#define DDR_PHY_NUM				2 /* phy number */
#endif

#define DMC_SFC_CMD_WRITE(sfc_cmd, addr) \
		REG_WRITE(sfc_cmd | (1 << DMC_SFC_PRE_DIS_BIT), addr)
#define DMC_SFC_BANK_WRITE(sfc_bank, addr) \
		REG_WRITE(sfc_bank | (DMC_CMD_RANK0 << DMC_SFC_RANK_BIT), addr)

#define DMC_MPR_CHECK_BIT_0_127(base_dmc, byte_index, dq_index) \
			ddr_mpr_extract(base_dmc, byte_index, dq_index, \
				DDR_DMC_SFC_RDATA3, DDR_DMC_SFC_RDATA2, \
				DDR_DMC_SFC_RDATA1, DDR_DMC_SFC_RDATA0)
#define DMC_MPR_CHECK_BIT_128_255(base_dmc, byte_index, dq_index) \
			ddr_mpr_extract(base_dmc, byte_index, dq_index, \
				DDR_DMC_SFC_RDATA7, DDR_DMC_SFC_RDATA6, \
				DDR_DMC_SFC_RDATA5, DDR_DMC_SFC_RDATA4)

#define DMC_DISABLE_SCRAMB(relate_reg, i, base_dmc) \
			REG_WRITE(relate_reg->dmc_scramb[i] & DMC_SCRAMB_DIS, \
				base_dmc + DDR_DMC_CFG_DDRMODE)

#define DMC_RESTORE_SCRAMB(relate_reg, i, base_dmc) \
			REG_WRITE(relate_reg->dmc_scramb[i], \
				base_dmc + DDR_DMC_CFG_DDRMODE);
/******** AXI **************************/
/**
 *              DMC -- PHY
 *             /
 * DDRT -- AXI
 *             \
 *              DMC -- PHY
 */
/* base address: DDR_REG_BASE_AXI */
/* register offset address */
#define DDR_AXI_REGION_ATTRIB0           0x104  /* region 0 */
#define DDR_AXI_REGION_ATTRIB1           0x114  /* region 1 */

/* register mask */
#define AXI_REGION_ATTRIB_CH_MASK        0xffff0000 /* channel mask */

/* register value */
/* Map to the single channel, independent address */
#define AXI_RNG_ATTR_CH_MODE             0x4
#define AXI_RNG_ATTR_CH_START_0          0x0
#define AXI_RNG_ATTR_CH_START_1          0x1

/********data define************************************/
struct ddr_ddrc_data {
	unsigned int region_attrib[DDR_PHY_NUM];
};

#define DDR_AXI_SAVE_FUNC(relate_reg) \
	do { \
		relate_reg->ddrc.region_attrib[0]   = \
			REG_READ(DDR_REG_BASE_AXI + DDR_AXI_REGION_ATTRIB0); \
		relate_reg->ddrc.region_attrib[1]   = \
			REG_READ(DDR_REG_BASE_AXI + DDR_AXI_REGION_ATTRIB1); \
	} while (0)

#define DDR_AXI_RESTORE_FUNC(relate_reg) \
	do { \
		REG_WRITE(relate_reg->ddrc.region_attrib[0], \
			DDR_REG_BASE_AXI + DDR_AXI_REGION_ATTRIB0); \
		REG_WRITE(relate_reg->ddrc.region_attrib[1], \
			DDR_REG_BASE_AXI + DDR_AXI_REGION_ATTRIB1); \
	} while (0)

#define DDR_AXI_SWITCH_FUNC(index, relate_reg) \
	do { \
		unsigned int ch_start = (index == 0 ? \
			AXI_RNG_ATTR_CH_START_0 : AXI_RNG_ATTR_CH_START_1); \
		REG_WRITE((relate_reg->ddrc.region_attrib[0] \
			& AXI_REGION_ATTRIB_CH_MASK) \
			| AXI_RNG_ATTR_CH_MODE | ch_start, \
			DDR_REG_BASE_AXI + DDR_AXI_REGION_ATTRIB0); \
		REG_WRITE((relate_reg->ddrc.region_attrib[1] \
			& AXI_REGION_ATTRIB_CH_MASK) \
			| AXI_RNG_ATTR_CH_MODE | ch_start, \
			DDR_REG_BASE_AXI + DDR_AXI_REGION_ATTRIB1); \
	} while (0)
