/* dma routines for old sunxi arch */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/completion.h>
#include <mach/dma.h>
#include <asm/cacheflush.h>

#include "main.h"

static int dma_ch = -1;
static void *dma_seq = NULL;
static DECLARE_COMPLETION(nand_dma_done);
static struct sw_dma_client nand_dma_client = {
	.name = DRIVER_NAME,
};

static void nand_buff_done(struct sw_dma_chan *ch, void *buf, int size, enum sw_dma_buffresult res)
{
	complete_all(&nand_dma_done);
}

int nand_request_dma(void)
{
	int ret;

	if (dma_ch > 0)
		return 0;
	dma_ch = sw_dma_request(DMACH_DNAND, &nand_dma_client, NULL);
	if (dma_ch < 0)
		return dma_ch;
	ret = sw_dma_set_buffdone_fn(dma_ch, nand_buff_done);
	if (ret < 0)
		goto free_dma;
	return 0;
free_dma:
	sw_dma_free(dma_ch, &nand_dma_client);
	dma_ch = -1;
	return ret;
}

void nand_release_dma(void)
{
	if (dma_ch < 0)
		return;
	sw_dma_free(dma_ch, &nand_dma_client);
	dma_ch = -1;
}

void nand_start_dma(int rw, dma_addr_t buff_addr, u32 len)
{
	struct dma_hw_conf nand_hwconf;
	int ret;

	memset(&nand_hwconf, 0, sizeof(nand_hwconf));
	if (rw == 0) { /* read */
		nand_hwconf.drqsrc_type		= D_DRQSRC_NAND;
		nand_hwconf.drqdst_type		= D_DRQDST_SDRAM;
		nand_hwconf.xfer_type		= DMAXFER_D_BWORD_S_BWORD;
		nand_hwconf.address_type	= DMAADDRT_D_LN_S_IO;
		nand_hwconf.dir				= SW_DMA_RDEV;
		nand_hwconf.hf_irq			= SW_DMA_IRQ_FULL;
		nand_hwconf.from			= NAND_IO_REG;
		nand_hwconf.to				= buff_addr;
		nand_hwconf.cmbk			= 0x7f077f07;
	} else { /* write */
		nand_hwconf.drqsrc_type		= D_DRQSRC_SDRAM;
		nand_hwconf.drqdst_type		= D_DRQDST_NAND;
		nand_hwconf.xfer_type		= DMAXFER_D_BWORD_S_BWORD;
		nand_hwconf.address_type	= DMAADDRT_D_IO_S_LN;
		nand_hwconf.dir				= SW_DMA_WDEV;
		nand_hwconf.hf_irq			= SW_DMA_IRQ_FULL;
		nand_hwconf.to				= NAND_IO_REG;
		nand_hwconf.from			= buff_addr;
		nand_hwconf.cmbk			= 0x7f077f07;
	}
	INIT_COMPLETION(nand_dma_done);
	ret = sw_dma_config(dma_ch, &nand_hwconf);
	ret = sw_dma_setflags(dma_ch, SW_DMAF_AUTOSTART);
//	__cpuc_flush_dcache_area((void *)buff_addr, len);
	ret = sw_dma_enqueue(dma_ch, dma_seq++, buff_addr, len);
//	ret = sw_dma_ctrl(dma_ch, SW_DMAOP_START);
}

int wait_dma_finish(void)
{
	unsigned long t = wait_for_completion_timeout(&nand_dma_done, HZ/10);
	if (!t)
		return -EAGAIN;
	return 0;
}
