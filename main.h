#ifndef _SUNXI_NAND_MAIN_H
#define _SUNXI_NAND_MAIN_H

#include <linux/err.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>
#include <mach/hardware.h>
#include <mach/platform.h>
#include <plat/sys_config.h>

#include "defs.h"
#include "nand_io.h"
#include "nand_regs.h"

#include "nand_clock.h"
#include "nand_irq.h"
#include "nand_dma.h"
#include "nand_gpio.h"
#include "nand_ctl.h"

#define MAX_PARTS	2

struct nand_info {
	struct mtd_info mtd;
	struct nand_chip chip;
	struct nand_ecclayout el;
	struct mtd_partition parts[MAX_PARTS]; /* don't want alloc separate memory :) */
	int page; /* for controller */
	int ecc;
	int ecc1k;
	int pages1k; /* pages in 1k mode from start (for BROM boot) - default to 512 */
};

int get_nand_param(char *param);

#endif
