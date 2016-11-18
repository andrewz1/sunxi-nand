#ifndef _SUNXI_NAND_IRQ_H
#define _SUNXI_NAND_IRQ_H

static inline int nand_request_irq(void)
{
	return 0;
}

static inline void nand_release_irq(void)
{
	return;
}

#endif
