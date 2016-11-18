#ifndef _SUNXI_NAND_CLOCK_H
#define _SUNXI_NAND_CLOCK_H

int nand_request_clk(void);
void nand_release_clk(void);
int nand_set_mod_clk(u32 nand_clk);
u32 nand_get_mod_clk(void);

#endif
