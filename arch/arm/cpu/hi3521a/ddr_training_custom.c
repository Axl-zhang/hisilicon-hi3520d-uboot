/**
 * ddr_training_custom.c
 *
 * Copyright (c) 2009-2014, HiSilicon Technologies Co., Ltd.
 * All rights reserved.
 *
 * DDR training custom implement.
 */

#include <ddr_interface.h>
#include "ddr_training_impl.h"

/**
 * ddr_cmd_prepare_copy
 * @void
 *
 * Do some prepare before copy code from DDR to SRAM.
 * Keep empty when nothing to do.
 */
void ddr_cmd_prepare_copy(void) { return; }

/**
 * ddr_cmd_site_save
 * @void
 *
 * Save site before DDR training command execute .
 * Keep empty when nothing to do.
 */
void ddr_cmd_site_save(void) { return; }

/**
 * ddr_cmd_site_restore
 * @void
 *
 * Restore site after DDR training command execute.
 * Keep empty when nothing to do.
 */
void ddr_cmd_site_restore(void) { return; }

int ddr_sw_training_custom_func(void *ddrtr_result)
{
	unsigned int base_dmc = DDR_REG_BASE_DMC0;
	unsigned int base_phy = DDR_REG_BASE_PHY0;
	int result = 0;
	unsigned int auto_ref_timing = REG_READ(base_dmc + DDR_DMC_TIMING2);

	/* check sw ddr training enable */
	if (DDR_BYPASS_ALL_MASK == REG_READ(DDR_REG_BASE_SYSCTRL
		+ SYSCTRL_DDR_TRAINING_CFG))
		return 0;

#ifdef DDR_WL_TRAINING_CONFIG
	/* write leveling */
	if (!ddr_training_check_bypass(DDR_BYPASS_WL_MASK)) {
		/* disable auto refresh */
		REG_WRITE(auto_ref_timing & DMC_AUTO_TIMING_DIS,
			base_dmc + DDR_DMC_TIMING2);
		result += ddr_write_leveling(base_dmc, base_phy);
		/* enable auto refresh */
		REG_WRITE(auto_ref_timing, base_dmc + DDR_DMC_TIMING2);
	}
#endif

	/* dataeye */
	if (!ddr_training_check_bypass(DDR_BYPASS_DATAEYE_MASK)) {
		ddr_ddrt_init(base_dmc, DDR_DDRT_MODE_DATAEYE);
		result += ddr_dataeye_training(base_dmc, base_phy,
			ddrtr_result, DDR_DATAEYE_NORMAL_ADJUST);
	}

#ifdef DDR_HW_TRAINING_CONFIG
	/* hardware read */
	if (result && !ddr_training_check_bypass(DDR_BYPASS_HW_MASK)) {
		result = ddr_hw_training(base_dmc, base_phy);
		result += ddr_dataeye_training(base_dmc, base_phy,
			ddrtr_result, DDR_DATAEYE_ABNORMAL_ADJUST);
	}
#endif

#ifdef DDR_MPR_TRAINING_CONFIG
	/* mpr */
	if (result && !ddr_training_check_bypass(DDR_BYPASS_MPR_MASK)) {
		result = ddr_mpr_training(base_dmc, base_phy);
		result += ddr_dataeye_training(base_dmc, base_phy,
			ddrtr_result, DDR_DATAEYE_ABNORMAL_ADJUST);
	}
#endif

#ifdef DDR_GATE_TRAINING_CONFIG
	/* gate */
	if (!ddr_training_check_bypass(DDR_BYPASS_GATE_MASK)) {
		ddr_ddrt_init(base_dmc, DDR_DDRT_MODE_GATE);
		/* disable auto refresh */
		REG_WRITE(auto_ref_timing & DMC_AUTO_TIMING_DIS,
			base_dmc + DDR_DMC_TIMING2);
		result += ddr_gate_training(base_dmc, base_phy);
		/* enable auto refresh */
		REG_WRITE(auto_ref_timing, base_dmc + DDR_DMC_TIMING2);
	}
#endif

#ifdef DDR_VREF_TRAINING_CONFIG
	if (!ddr_training_check_bypass(DDR_BYPASS_VREF_MASK)) {
		ddr_ddrt_init(base_dmc, DDR_DDRT_MODE_DATAEYE);
		result += ddr_vref_training(base_dmc, base_phy,
			ddrtr_result);
	}
#endif

	return result;
}
