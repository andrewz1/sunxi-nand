#ifndef _SUNXI_NAND_NAND1K_H
#define _SUNXI_NAND_NAND1K_H

#if 0

int nand1k_init(void);
void nand1k_exit(void);

#else

static inline int nand1k_init(void)
{
	return 0;
}

static inline void nand1k_exit(void)
{
	return;
}

#endif

#endif
