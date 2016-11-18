#ifndef _SUNXI_NAND_DMA_H
#define _SUNXI_NAND_DMA_H

int nand_request_dma(void);
void nand_release_dma(void);
void nand_start_dma(int rw, dma_addr_t buff_addr, u32 len);
int wait_dma_finish(void);

#endif
