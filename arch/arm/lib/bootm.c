/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA
 *
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <u-boot/zlib.h>
#include <asm/byteorder.h>

#include <cmd_bootss.h>

DECLARE_GLOBAL_DATA_PTR;

extern char *slave_boot_start;

#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
defined(CONFIG_CMDLINE_TAG) || \
defined(CONFIG_INITRD_TAG) || \
defined(CONFIG_SERIAL_TAG) || \
defined(CONFIG_REVISION_TAG) || \
defined(CONFIG_ETHADDR_TAG) || \
defined(CONFIG_ETHMDIO_INF) || \
defined(CONFIG_NANDID_TAG) || \
defined(CONFIG_SPIID_TAG)
static void setup_start_tag (bd_t *bd);

# ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd);
# endif
static void setup_commandline_tag (bd_t *bd, char *commandline);

# ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start,
			      ulong initrd_end);
# endif
static void setup_end_tag (bd_t *bd);

# if defined(CONFIG_ETHMDIO_INF)
static void setup_eth_mdiointf_tag(bd_t *bd, char *mdio_intf);
#endif

# if defined(CONFIG_ETHADDR_TAG)
static void setup_ethaddr_tag(bd_t *bd, char* ethaddr);
#endif

#if defined(CONFIG_MULT_CPU_TAG)
static void setup_mult_cpu_boot_tag(bd_t *bd, char *bootaddr);
#endif

# if defined(CONFIG_NANDID_TAG)
static void setup_nandid_tag(bd_t *bd);
#endif

# if defined(CONFIG_SPIID_TAG)
static void setup_spiid_tag(bd_t *bd);
#endif

static struct tag *params;
#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

int do_bootm_linux(int flag, int argc, char *argv[], bootm_headers_t *images)
{
	bd_t	*bd = gd->bd;
	char	*s;
	int	machid = bd->bi_arch_number;
	void	(*theKernel)(int zero, int arch, uint params);

#ifdef CONFIG_CMDLINE_TAG
#ifdef CONFIG_HI3536_A7
	char *commandline = getenv("slave_bootargs");
#else
	char *commandline = getenv("bootargs");
#endif
#endif

	if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
		return 1;

	theKernel = (void (*)(int, int, uint))images->ep;

	s = getenv ("machid");
	if (s) {
		machid = simple_strtoul (s, NULL, 16);
		printf ("Using machid 0x%x from environment\n", machid);
	}

	show_boot_progress (15);

	debug ("## Transferring control to Linux (at address %08lx) ...\n",
	       (ulong) theKernel);

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG)
	setup_start_tag (bd);
#ifdef CONFIG_SERIAL_TAG
	setup_serial_tag (&params);
#endif
#ifdef CONFIG_REVISION_TAG
	setup_revision_tag (&params);
#endif
#ifdef CONFIG_SETUP_MEMORY_TAGS
	setup_memory_tags (bd);
#endif
#ifdef CONFIG_CMDLINE_TAG
	setup_commandline_tag (bd, commandline);
#endif
#ifdef CONFIG_INITRD_TAG
	if (images->rd_start && images->rd_end)
		setup_initrd_tag (bd, images->rd_start, images->rd_end);
#endif
#if defined(CONFIG_ETHMDIO_INF)
	setup_eth_mdiointf_tag(bd, getenv("mdio_intf"));
#endif
#if defined(CONFIG_ETHADDR_TAG)
	setup_ethaddr_tag(bd, getenv("ethaddr"));
#endif
#if defined(CONFIG_MULT_CPU_TAG)
	/* setup_mult_cpu_boot_tag(bd, getenv("slaveboot")); */
	setup_mult_cpu_boot_tag(bd, slave_boot_start);
#endif

#if defined(CONFIG_NANDID_TAG)
	setup_nandid_tag(bd);
#endif
#if defined(CONFIG_SPIID_TAG)
	setup_spiid_tag(bd);
#endif
	setup_end_tag (bd);
#endif

	/* we assume that the kernel is in place */
	printf ("\nStarting kernel ...\n\n");

#ifdef CONFIG_USB_DEVICE
	{
		extern void udc_disconnect (void);
		udc_disconnect ();
	}
#endif

	cleanup_before_linux ();

	theKernel (0, machid, bd->bi_boot_params);
	/* does not return */

	return 1;
}


#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
defined(CONFIG_CMDLINE_TAG) || \
defined(CONFIG_INITRD_TAG) || \
defined(CONFIG_SERIAL_TAG) || \
defined(CONFIG_REVISION_TAG) || \
defined(CONFIG_ETHADDR_TAG)
static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *) bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}


#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd)
{
	int i;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size (tag_mem32);

		params->u.mem.start = bd->bi_dram[i].start;
		params->u.mem.size = bd->bi_dram[i].size;

		params = tag_next (params);
	}
}
#endif /* CONFIG_SETUP_MEMORY_TAGS */


static void setup_commandline_tag (bd_t *bd, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

	params = tag_next (params);
}
#ifdef CONFIG_ETHMDIO_INF
static void setup_eth_mdiointf_tag(bd_t *bd, char *mdio_intf)
{
	unsigned char mac[6];
	if (!mdio_intf)
		return ;
	params->hdr.tag = CONFIG_ETH_MDIO_INF_TAG_VAL;
	params->hdr.size = 4;

	memcpy(&params->u, mdio_intf, (strlen(mdio_intf)+1));
	params = tag_next(params);
}
#endif
#ifdef CONFIG_ETHADDR_TAG
static void string_to_mac(unsigned char *mac, char* s)
{
	int i;
	char *e;

	for (i = 0; i < 6; ++i) {
		mac[i] = s ? simple_strtoul(s, &e, 16) : 0;
		if (s)
			s = (*e) ? e+1 : e;
	}
}

static void setup_ethaddr_tag(bd_t *bd, char *ethaddr)
{
	unsigned char mac[6];
	if (!ethaddr)
		return ;

	params->hdr.tag = CONFIG_ETHADDR_TAG_VAL;
	params->hdr.size = 4;

	string_to_mac(&mac[0], ethaddr);
	memcpy(&params->u, mac, 6);

	params = tag_next(params);
}
#endif

#ifdef CONFIG_MULT_CPU_TAG
static void setup_mult_cpu_boot_tag(bd_t *bd, char *bootaddr)
{
	if (!bootaddr)
		return;

	params->hdr.tag = CONFIG_MULT_CPU_TAG_VAL;
	params->hdr.size = 8;

	memcpy(&params->u, bootaddr, (strlen(bootaddr)+1));
	params = tag_next(params);
}
#endif

#ifdef CONFIG_NANDID_TAG
extern struct nand_tag nandtag[1];

static void setup_nandid_tag(bd_t *bd)
{
	if (nandtag->length == 0)
		return;

	params->hdr.tag = ATAG_NDNDID;
	params->hdr.size = (sizeof(struct tag_header)
			+ sizeof(struct nand_tag)) >> 2;

	memcpy(&params->u, nandtag, sizeof(struct nand_tag));

	params = tag_next(params);
}
#endif

#ifdef CONFIG_SPIID_TAG
extern struct spi_tag spitag[1];

static void setup_spiid_tag(bd_t *bd)
{
	if (spitag->id_len == 0)
		return;

	params->hdr.tag = ATAG_SPIID;
	params->hdr.size = (sizeof(struct tag_header) +
			sizeof(struct spi_tag)) >> 2;

	memcpy(&params->u, &spitag, sizeof(struct spi_tag));

	params = tag_next(params);
}
#endif

#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next (params);
}
#endif /* CONFIG_INITRD_TAG */

#ifdef CONFIG_SERIAL_TAG
void setup_serial_tag (struct tag **tmp)
{
	struct tag *params = *tmp;
	struct tag_serialnr serialnr;
	void get_board_serial(struct tag_serialnr *serialnr);

	get_board_serial(&serialnr);
	params->hdr.tag = ATAG_SERIAL;
	params->hdr.size = tag_size (tag_serialnr);
	params->u.serialnr.low = serialnr.low;
	params->u.serialnr.high= serialnr.high;
	params = tag_next (params);
	*tmp = params;
}
#endif

#ifdef CONFIG_REVISION_TAG
void setup_revision_tag(struct tag **in_params)
{
	u32 rev = 0;
	u32 get_board_rev(void);

	rev = get_board_rev();
	params->hdr.tag = ATAG_REVISION;
	params->hdr.size = tag_size (tag_revision);
	params->u.revision.rev = rev;
	params = tag_next (params);
}
#endif  /* CONFIG_REVISION_TAG */


static void setup_end_tag (bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

/*============================================================================
 *
 * Function used for snapshot boot (bootss command)
 *
 *============================================================================*/
#ifdef CONFIG_SNAPSHOT_BOOT
extern void restore_processor_state_ext(void *, unsigned long);

static void printf_ss_info(unsigned s, unsigned c)
{
	unsigned int tmp;
	asm volatile ("mrc p15, 0, %0, c0, c2, 4" : "=r"(tmp) : : "memory");
	if (((tmp >> 16) & 0xf) != 1)
		printf("WARN: it does not support DMB,DSB and ISB!\n");

	debug("\nbootss: jumping to kernel resume point: 0x%08x with 0x%08x\n",
			 s, c);
}


#define __NO_IOMEM_FUNCTIONS__
#include <asm/arch/mmu.h>
#include <asm/mmu.h>
#include <asm/cache-cp15.h>
#undef  __NO_IOMEM_FUNCTIONS__


void ss_jump_to_resume_ext(unsigned long func, unsigned long data)
{
	/* calculate a physical address based on a Linux TTB */
	data = __virt_to_phys(data);

	if (func == (unsigned long)-1 || data == (unsigned long)-1)
		return;

	printf_ss_info(func, data);

	/* MMU off, flush all, invalidate all */
	cleanup_before_linux();

	/* memory/data/instruction barriers */
	asm volatile ("dmb" : : : "memory");
	asm volatile ("dsb" : : : "memory");
	asm volatile ("isb" : : : "memory");

	/* restore CP15 state, which was saved in Linux */
	restore_processor_state_ext((void *)data, func);
}
#endif
