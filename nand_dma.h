#ifndef _SUNXI_NAND_DMA_H
#define _SUNXI_NAND_DMA_H

int nand_request_dma(void);
void nand_release_dma(void);
void nand_start_dma(int rw, void *buff_virt, dma_addr_t buff_addr, u32 buff_len);
int wait_dma_finish(void);

#endif
