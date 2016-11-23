#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <mach/clock.h>

#include "main.h"

static struct clk *ahb_nand_clk = NULL;
static struct clk *mod_nand_clk = NULL;
static struct clk *parent_clk = NULL;

/* "hosc","sdram_pll_p","sata_pll_2","sclk_none" */
#define PARENT_CLK		"hosc" /* 24MHz */

int nand_request_clk(void)
{
	int err;

	ahb_nand_clk = clk_get(NULL, "ahb_nfc"); /* nfc clock gate */
	if (IS_ERR(ahb_nand_clk))
		return PTR_ERR(ahb_nand_clk);
	mod_nand_clk = clk_get(NULL, "nfc");
	if (IS_ERR(mod_nand_clk)) {
		err = PTR_ERR(ahb_nand_clk);
		goto put_ahb_clk;
	}
	parent_clk = clk_get(NULL, PARENT_CLK);
	if (IS_ERR(parent_clk)) {
		err = PTR_ERR(parent_clk);
		goto put_nand_clk;
	}
	err = clk_set_parent(mod_nand_clk, parent_clk);
	if (err < 0)
		goto put_parent_clk;
	err = clk_set_rate(mod_nand_clk, NAND_DEF_FREQ); /* default nand clock */
	if (err < 0)
		goto put_parent_clk;
	err = clk_prepare_enable(ahb_nand_clk);
	if (err < 0)
		goto put_parent_clk;
	err = clk_prepare_enable(mod_nand_clk);
	if (err < 0)
		goto unprep_ahb_clk;
	return 0;
unprep_ahb_clk:
	clk_disable_unprepare(ahb_nand_clk);
put_parent_clk:
	clk_put(parent_clk);
put_nand_clk:
	clk_put(mod_nand_clk);
put_ahb_clk:
	clk_put(ahb_nand_clk);
	return err;
}

void nand_release_clk(void)
{
	clk_disable_unprepare(mod_nand_clk);
	clk_disable_unprepare(ahb_nand_clk);
	clk_put(parent_clk);
	clk_put(mod_nand_clk);
	clk_put(ahb_nand_clk);
}

int nand_set_mod_clk(u32 nand_clk)
{
	return clk_set_rate(mod_nand_clk, nand_clk);
}

u32 nfc_get_mod_clk(void)
{
	return clk_get_rate(mod_nand_clk);
}
