/*************************************************************************
* si_drv_pll_vo_cfg.c
*************************************************************************/
#include "si_drv_pll_vo.h"

/************************************************************************/
/*      Constants Definitions                                           */
/************************************************************************/
#define REC(ifr, ofr, nr0, nf0, od0, pd0, cpif, nr1, nf1, od1, pd1)     {ifr, ofr, {nr0, nf0, od0, pd0, cpif}, {nr1, nf1, od1, pd1, 0} }

/************************************************************************/
/*      Static Variables                                                */
/************************************************************************/
SiiDrvPllVoCapRec_t sSiiDrvPllVoCapTbl[] =
{
	/*   Input        Output        PLL0                          PLL1                             */
	/*   Frequency    Frequency     NR   NF  OD   PD     cpif     NR   NF  OD  PD      vm Format   */

	/* 480p/576p input formats */                                                
	REC(  27000000UL,    27000000UL,     0,   43, 21, 43,       0,     0,   0,  0,  0), // 22 480p/576p
	REC(  27000000UL,    74250000UL,     0,   43, 15, 43,       0,     0,   0,  0,  0), // 23 720P50/1080i50/1080p25
	REC(  27000000UL,    74175824UL,     0,   43, 15, 43,       0,     0,   0,  0,  0), // 27 720P60/1080i60/1080p24/1080p30
	REC(  27000000UL,   148500000UL,     1,   44,  8,  1,       0,     0,   0,  0,  0), // 24 1080p50
	REC(  27000000UL,   297000000UL,     1,   44,  4,  1,       0,     0,   0,  0,  0), // 25 4K50 (420)    REC(  27000000UL,    74175824,    13,  500,  7,  2,       0,     0,   0,  0,  0), // 27 720P60/1080i60/1080p24/1080p30
	REC(  27000000UL,   148351648UL,    13,  500,  7,  1,       0,     0,   0,  0,  0), // 28 1080p60
	REC(  27000000UL,   296703298UL,     1,   40,  7,  1,       0,    13,  50,  2,  1), // 29 4K60 (420)

	REC(         0,           0,     0,    0,  0,  0,       0,     0,   0,  0,  0)
};

/** END of File *********************************************************/
