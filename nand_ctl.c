#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/cacheflush.h>

#include "main.h"

#define BUFF_SZ (SZ_8K)

#define _RESET_RB	do {	\
	rbp = 0; rbl = 0;		\
} while (0)

#define _RESET_WB	do {	\
	wbp = 0; wbl = BUFF_SZ;	\
} while (0)

static const u16 random_seed[128] = {
	0x2b75, 0x0bd0, 0x5ca3, 0x62d1, 0x1c93, 0x07e9, 0x2162, 0x3a72,
	0x0d67, 0x67f9, 0x1be7, 0x077d, 0x032f, 0x0dac, 0x2716, 0x2436,
	0x7922, 0x1510, 0x3860, 0x5287, 0x480f, 0x4252, 0x1789, 0x5a2d,
	0x2a49, 0x5e10, 0x437f, 0x4b4e, 0x2f45, 0x216e, 0x5cb7, 0x7130,
	0x2a3f, 0x60e4, 0x4dc9, 0x0ef0, 0x0f52, 0x1bb9, 0x6211, 0x7a56,
	0x226d, 0x4ea7, 0x6f36, 0x3692, 0x38bf, 0x0c62, 0x05eb, 0x4c55,
	0x60f4, 0x728c, 0x3b6f, 0x2037, 0x7f69, 0x0936, 0x651a, 0x4ceb,
	0x6218, 0x79f3, 0x383f, 0x18d9, 0x4f05, 0x5c82, 0x2912, 0x6f17,
	0x6856, 0x5938, 0x1007, 0x61ab, 0x3e7f, 0x57c2, 0x542f, 0x4f62,
	0x7454, 0x2eac, 0x7739, 0x42d4, 0x2f90, 0x435a, 0x2e52, 0x2064,
	0x637c, 0x66ad, 0x2c90, 0x0bad, 0x759c, 0x0029, 0x0986, 0x7126,
	0x1ca7, 0x1605, 0x386a, 0x27f5, 0x1380, 0x6d75, 0x24c3, 0x0f8e,
	0x2b7a, 0x1418, 0x1fd1, 0x7dc1, 0x2d8e, 0x43af, 0x2267, 0x7da3,
	0x4e3d, 0x1338, 0x50db, 0x454d, 0x764d, 0x40a3, 0x42e6, 0x262b,
	0x2d2e, 0x1aea, 0x2e17, 0x173d, 0x3a6e, 0x71bf, 0x25f9, 0x0a5d,
	0x7c57, 0x0fbe, 0x46ce, 0x4939, 0x6b17, 0x37bb, 0x3e91, 0x76db,
};

#if 0
static const u32 oob_val[128] = { /* oob values for erased pages */
	0xc2b8530a, 0x887b4a0e, 0x96c49c5b, 0x8258ad8b, 0x40e2542d, 0x99d88874, 0xfa1c0d7a, 0x0a094d49,
	0x104695a3, 0x3ba10f29, 0x781e0a82, 0x35911896, 0xe2982c8a, 0x971f617e, 0x3abaf30c, 0x4d5e467d,
	0x55dd9878, 0xdce07bd5, 0x32c913b6, 0xd52466da, 0x4101a957, 0x2f813f56, 0x9d6f79c1, 0x3ce50426,
	0xca4fb240, 0x0072aa13, 0xb5a318d1, 0x9aa88ccb, 0xe35c2e7a, 0xca3fb2ff, 0xc6a05cd5, 0x5f7d81f8,
	0x6cc63ba2, 0x70d2146d, 0xc21bad70, 0xee403daa, 0xe2cc2c45, 0x7e5e027d, 0x806355d1, 0x8c4b45b1,
	0x98708a14, 0x0c3abaf3, 0x91cc97bb, 0x548664a2, 0xe5f427e5, 0x2aad3339, 0x1e9d7c86, 0x49ca484d,
	0xcf57be60, 0x5b12716d, 0x50a594d8, 0x8164a9db, 0x3f76fe1c, 0x9def783e, 0xec843b5a, 0x32baecf3,
	0x72f1ec17, 0xc533a6ef, 0x19d97789, 0xf7ac1f3a, 0x00b6ab1c, 0xc49fa480, 0x06515d96, 0xc397af60,
	0xf954099a, 0x493e4902, 0xcf144165, 0x1c1d7a86, 0xf6101d6a, 0xf0b3eb10, 0x981f757e, 0x34eae433,
	0x28b134e9, 0x94a49b24, 0xbc30faeb, 0x4bbdb0f8, 0x9c537a6e, 0x08194a89, 0x11c297ac, 0x9a577261,
	0xe457da60, 0x6d89c748, 0x94bc9b04, 0xab90cf6b, 0x0d12b96c, 0x72661222, 0xa14cd644, 0x78e9f437,
	0x6f8dc0b8, 0xa93b36f1, 0x9afb73f1, 0x1d2a86cc, 0x8e974361, 0xc5cfa7bf, 0x4d5a4672, 0x6f51c197,
	0xa83b350e, 0xfb780e0a, 0xc1eba830, 0x93276ede, 0x94b09b14, 0x081d4a86, 0x3042ea53, 0x65ca27b2,
	0xe7a020d5, 0x6df6381d, 0x54219ad7, 0x13029153, 0x95136691, 0x30d11469, 0xfcc3fbaf, 0x1f1d7e86,
	0x17969f63, 0x9d37791e, 0x2d6539d9, 0x4e2dbd38, 0x85af58c1, 0xc9d3b790, 0xddbc78fa, 0x917f69fe,
	0xb0c4eba4, 0xafdf3f81, 0xbd60f9d4, 0xe1c4285a, 0x22fd2c06, 0x214529a6, 0xccf3bbef, 0x4eaa4332,
};
#endif

#define _ECC_INIT(hi, lo) ((hi) << 16 | (lo))

static const u32 ecc_bytes[] = {
	_ECC_INIT(16, 32),
	_ECC_INIT(24, 46),
	_ECC_INIT(28, 54),
	_ECC_INIT(32, 60),
	_ECC_INIT(40, 74),
	_ECC_INIT(48, 88),
	_ECC_INIT(56, 102),
	_ECC_INIT(60, 110),
	_ECC_INIT(64, 116),
};

static void *rbuff;
static dma_addr_t rbuff_dma;
static int rbp, rbl; /* cur rbuff pos and left bytes */

static void *wbuff;
static dma_addr_t wbuff_dma;
static int wbp, wbl = BUFF_SZ; /* cur wbuff pos and left bytes */

static inline void nand_controller_enable(void)
{
	set_wbit(NAND_CTL, NAND_EN);
}

static inline void nand_controller_disable(void)
{
	clr_wbit(NAND_CTL, NAND_EN);
}

static inline void nand_controller_reset(void)
{
	set_wbit(NAND_CTL, NAND_RESET);
	while(readl(NAND_CTL) & NAND_RESET)
		udelay(1);
}

static inline void nand_set_busw(int busw)
{
	if (busw)
		set_wbit(NAND_CTL, NAND_BUS_WIDTH);
	else
		clr_wbit(NAND_CTL, NAND_BUS_WIDTH);
}

static inline void nand_set_rbsel(u32 rb)
{
	clrset_wbit(NAND_CTL, NAND_RB_SEL, (rb & 0x3) << 3);
}

static inline void nand_set_cesel(u32 ce)
{
	clrset_wbit(NAND_CTL, NAND_CE_SEL, (ce & 0xf) << 24);
}

static inline void nand_ce_act(int on)
{
	if (on)
		set_wbit(NAND_CTL, NAND_CE_ACT);
	else
		clr_wbit(NAND_CTL, NAND_CE_ACT);
}

static inline void nand_set_page(u32 page)
{
	clrset_wbit(NAND_CTL, NAND_PAGE_MASK, page << NAND_PAGE_SHIFT);
	writel(1 << (page + 10), NAND_SPARE_AREA);
}

static inline void nand_set_ecc(u32 ecc)
{
	clrset_wbit(NAND_ECC_CTL, NAND_ECC_MASK, ecc << NAND_ECC_SHIFT);
}

/* don't clear ecc_mode */
#define _ECC_MASK (NAND_ECC_EN | NAND_ECC_PIPELINE | NAND_ECC_EXCEPTION | NAND_ECC_BLOCK_SIZE)

static inline void disable_ecc(void)
{
	clr_wbit(NAND_ECC_CTL, _ECC_MASK);
}

/* ecc block size set to 1024 */
static inline void enable_ecc(int pipline)
{
	u32 cfg, rnd;

	rnd = readl(NAND_ECC_CTL) & NAND_RND_EN;
	cfg = NAND_ECC_EN;
	if (pipline)
		cfg |= NAND_ECC_PIPELINE;
	// if random enabled, disable exception
	if (!rnd)
		cfg |= NAND_ECC_EXCEPTION;
	clrset_wbit(NAND_ECC_CTL, _ECC_MASK, cfg);
}

#define _RND_MASK	(NAND_RND_EN | NAND_RND_DIRECTION | NAND_RND_SIZE | NAND_RND_MASK)
#define _RND_1K		(NAND_RND_EN | NAND_RND_1K)

static inline void disable_random(void)
{
	clr_wbit(NAND_ECC_CTL, _RND_MASK);
}

static inline void enable_random1k(void)
{
	clrset_wbit(NAND_ECC_CTL, _RND_MASK, _RND_1K);
}

static inline void enable_random(u32 page)
{
	u32 cfg;

	cfg = random_seed[page % 128];
	cfg <<= NAND_RND_SHIFT;
	cfg |= NAND_RND_EN;
	clrset_wbit(NAND_ECC_CTL, _RND_MASK, cfg);
}

static inline void nand_to_ahb(void)
{
	clr_wbit(NAND_CTL, NAND_RAM_METHOD);
}

static inline void nand_to_dma(void)
{
	set_wbit(NAND_CTL, NAND_RAM_METHOD);
}

static inline void nand_save(struct nand_save *s)
{
	s->ctl = readl(NAND_CTL);
	s->ecc = readl(NAND_ECC_CTL);
	s->oob = readl(NAND_SPARE_AREA);
}

static inline void nand_restore(struct nand_save *s)
{
	writel(s->ctl, NAND_CTL);
	writel(s->ecc, NAND_ECC_CTL);
	writel(s->oob, NAND_SPARE_AREA);
}

static inline int wait_cmd_fifo(void)
{
	int timeout = NAND_TMO;

	do {
		if (!(readl(NAND_ST) & NAND_CMD_FIFO_STATUS))
			return 0;
		udelay(1);
		timeout--;
	} while (timeout > 0);
	return -1;
}

static inline int wait_cmd_finish(void)
{
	int timeout = NAND_TMO;

	do {
		if (readl(NAND_ST) & NAND_CMD_INT_FLAG) {
			set_wbit(NAND_ST, NAND_CMD_INT_FLAG);
			return 0;
		}
		udelay(1);
		timeout--;
	} while (timeout > 0);
	return -1;
}

#define _CMD_DMA_FLAGS (NAND_CMD_INT_FLAG | NAND_DMA_INT_FLAG)

static inline int wait_cmd_dma(void) /* not used - need to test */
{
	int timeout = NAND_TMO;

	do {
		if ((readl(NAND_ST) & _CMD_DMA_FLAGS) == _CMD_DMA_FLAGS) {
			set_wbit(NAND_ST, _CMD_DMA_FLAGS);
			return 0;
		}
		udelay(1);
		timeout--;
	} while (timeout > 0);
	return -1;
}

static inline int do_cmd(u32 cmd)
{
	if (wait_cmd_fifo() < 0)
		return -1;
	writel(cmd, NAND_CMD);
	return wait_cmd_finish();
}

static inline int do_cmd_dma(u32 cmd, int rw, dma_addr_t dma_addr, u32 dma_len)
{
	if (wait_cmd_fifo() < 0)
		return -1;
	nand_to_dma();
	nand_start_dma(rw, dma_addr, dma_len);
	writel(cmd, NAND_CMD);
	if (wait_dma_finish() < 0) {
		nand_to_ahb();
		return -1;
	}
	nand_to_ahb();
	return wait_cmd_finish();
}

static inline int get_status(void)
{
	u32 cmd;

	writel(0, NAND_RAM0_BASE);
	nand_to_ahb();
	cmd = NAND_CMD_STATUS | NAND_SEND_FIRST_CMD;
	_SET_ADDR(cmd, 0);
	_SET_READ(cmd);
	writel(1, NAND_CNT);
	if (do_cmd(cmd) < 0)
		return -1;
	return (int)(readl(NAND_RAM0_BASE) & 0xff);
}

static inline int wait_status(u32 mask, u32 res)
{
	int timeout = NAND_TMO, err;

	do {
		err = get_status();
		if (err < 0) {
			nand_err("%s: get_status error\n", __func__);
			return -1;
		}
		else if ((err & mask) == res)
			return err;
		udelay(1);
		timeout--;
	} while (timeout > 0);
	nand_err("%s: timeout, last status %d\n", __func__, err);
	return -1;
}

#define _MAX_BITFLIP	4
/* 
 return real value of corrected bits if above (chip->ecc.strength - _MAX_BITFLIP)
 if we always return real value then periodic bitflip error occur and many
 eraseblocks fall in bbt
 todo: make this value configurable
 */
static inline int ecc_result(struct nand_chip *chip, int step)
{
	u32 mask = 1 << (step & 0xf);
	u32 reg = (step & 0xc) >> 2;
	u32 shift = (step & 0x3) << 3;
	u32 corr;

	if (readl(NAND_ECC_ST) & mask)
		return -1;
	 /* get corrected bits for step */
	corr = (readl(NAND_ERR_CNT(reg)) >> shift) & 0xff;
	if (corr > (chip->ecc.strength - _MAX_BITFLIP))
		return corr;
	return 0;
}

int nand_controller_init(void)
{
	int ret;

	rbuff = dma_alloc_coherent(NULL, BUFF_SZ, &rbuff_dma, GFP_KERNEL);
	if (!rbuff)
		return -ENOMEM;
	wbuff = dma_alloc_coherent(NULL, BUFF_SZ, &wbuff_dma, GFP_KERNEL);
	if (!wbuff) {
		ret = -ENOMEM;
		goto free_rdma;
	}
	ret = nand_request_clk();
	if (ret < 0)
		goto free_wdma;
	ret = nand_request_gpio();
	if (ret < 0)
		goto release_clk;
	nand_controller_reset();
	ret = nand_request_irq();
	if (ret < 0)
		goto release_gpio;
	ret = nand_request_dma();
	if (ret < 0)
		goto release_irq;
	nand_controller_enable();
	nand_set_rbsel(0);
	nand_set_cesel(0);
	return 0;
release_irq:
	nand_release_irq();
release_gpio:
	nand_release_gpio();
release_clk:
	nand_release_clk();
free_wdma:
	dma_free_coherent(NULL, BUFF_SZ, wbuff, wbuff_dma);
free_rdma:
	dma_free_coherent(NULL, BUFF_SZ, rbuff, rbuff_dma);
	return ret;
}

void nand_controller_exit(void)
{
	nand_controller_disable();
	nand_controller_reset();
	nand_release_dma();
	nand_release_irq();
	nand_release_gpio();
	nand_release_clk();
	dma_free_coherent(NULL, BUFF_SZ, wbuff, wbuff_dma);
	dma_free_coherent(NULL, BUFF_SZ, rbuff, rbuff_dma);
}

#define _RST_CNT 10

int __nand_chip_reset(void)
{
	int i, timeout = NAND_TMO;
	u32 cmd;

	nand_to_ahb();
	for (i = 0; i < _RST_CNT; i++) {
		cmd = NAND_CMD_RESET
			| NAND_SEND_FIRST_CMD;
		_SET_ADDR(cmd, 0);
		_SET_NODATA(cmd);
		if (do_cmd(cmd) >= 0)
			break;
		mdelay(1);
	}
	if (i == _RST_CNT)
		return -1;
	do {
		if ((readl(NAND_ST) & NAND_RB_ALL) == NAND_RB_ALL)
			return 0;
		udelay(1);
		timeout--;
	} while (timeout > 0);
	return -1;
}

int __nand_chip_readid(int addr)
{
	int i;
	u32 cmd, len = 8;

	_RESET_RB;
	nand_to_ahb();
	cmd = NAND_CMD_READID
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_DATA_METHOD;
	_SET_ADDR(cmd, 1);
	writel(addr & 0xff, NAND_ADDR_LOW);
	writel(0, NAND_ADDR_HIGH);
	_SET_READ(cmd);
	writel(len, NAND_CNT);
	if (do_cmd_dma(cmd, 0, rbuff_dma, len) < 0)
		return -1;
	for (i = 4; i < 8; i++)
		if (!(memcmp(rbuff, rbuff + i, 8 - i)))
			break;
	rbl = i;
	return 0;
}

int __nand_chip_param(int addr)
{
	u32 cmd, len = 1024;

	_RESET_RB;
	nand_to_ahb();
	cmd = NAND_CMD_PARAM
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_DATA_METHOD;
	_SET_ADDR(cmd, 1);
	writel(addr & 0xff, NAND_ADDR_LOW);
	writel(0, NAND_ADDR_HIGH);
	_SET_READ(cmd);
	writel(len, NAND_CNT);
	if (do_cmd_dma(cmd, 0, rbuff_dma, len) < 0)
		return -1;
	rbl = len;
	return 0;
}

int __nand_chip_read_page(struct mtd_info *mtd, int page)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	u32 cmd, tmp;
	u32 len = mtd->writesize;
	u32 blk = mtd->writesize >> 10;
	struct nand_save save;

	_RESET_RB;
	nand_save(&save);
	nand_set_page(info->page);
	nand_set_ecc(info->ecc);
	enable_random(page);
	enable_ecc(1);
	nand_to_ahb();
	cmd = NAND_RCMD
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_SEND_SECOND_CMD
		| NAND_CMD_TYPE_PAGE;
	writel(NAND_RSET, NAND_RCMD_SET);
	_SET_ADDR(cmd, 5);
	tmp = (page & 0xffffU) << 16;
	writel(tmp, NAND_ADDR_LOW);
	tmp = (page & 0xff0000U) >> 16;
	writel(tmp, NAND_ADDR_HIGH);
	_SET_READ(cmd);
	writel(blk, NAND_BLOCK_NUM);
	if (do_cmd_dma(cmd, 0, rbuff_dma, len) < 0) {
		nand_restore(&save);
		return -1;
	}
	nand_restore(&save);
	rbl = len;
	return 0;
}

int __nand_chip_read_page1k(struct mtd_info *mtd, int page)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	u32 cmd, tmp;
	u32 len = mtd->writesize;
	u32 blk = mtd->writesize >> 10;
	struct nand_save save;

	_RESET_RB;
	nand_save(&save);
	nand_set_page(info->page);
	nand_set_ecc(info->ecc1k);
	enable_random1k();
	enable_ecc(1);
	nand_to_ahb();
	cmd = NAND_RCMD
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_SEND_SECOND_CMD
		| NAND_CMD_TYPE_PAGE
		| NAND_SEQ;
	writel(NAND_RSET, NAND_RCMD_SET);
	_SET_ADDR(cmd, 5);
	tmp = (page & 0xffffU) << 16;
	writel(tmp, NAND_ADDR_LOW);
	tmp = (page & 0xff0000U) >> 16;
	writel(tmp, NAND_ADDR_HIGH);
	_SET_READ(cmd);
	writel(blk, NAND_BLOCK_NUM);
	if (do_cmd_dma(cmd, 0, rbuff_dma, len) < 0) {
		nand_restore(&save);
		return -1;
	}
	nand_restore(&save);
	rbl = len;
	return 0;
}

int __do_read_page(struct mtd_info *mtd, int column, int page)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);

	if (column > 0) {
		nand_err("%s: column %d\n", __func__, column);
		return -1;
	}
	if (page < info->pages1k)
		return __nand_chip_read_page1k(mtd, page);
	else
		return __nand_chip_read_page(mtd, page);
}

int __do_read_oob(struct mtd_info *mtd, int page)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	struct nand_chip *chip = &info->chip;
	int i;
	u32 mask = (1 << chip->ecc.steps) - 1;

	/* first try to read plain page */
	if (__do_read_page(mtd, 0, page) < 0)
		return -1;
	_RESET_RB;
	if ((readl(NAND_ECC_ST) & mask) == mask)
		for (i = 0; i < chip->ecc.steps; i++)
			writel(0xffffffffU, NAND_USER_DATA(i));
	return 0;
}

int __nand_chip_erase(struct mtd_info *mtd, int page)
{
	u32 cmd, blk_mask;

	blk_mask = 1 << (mtd->erasesize_shift - mtd->writesize_shift);
	blk_mask = ~(blk_mask - 1);
	blk_mask &= 0xffffffU;
	nand_to_ahb();
	cmd = NAND_ERASE
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_SEND_SECOND_CMD;
	writel(NAND_ESET, NAND_RCMD_SET);
	_SET_ADDR(cmd, 3);
	writel(page & blk_mask, NAND_ADDR_LOW);
	writel(0, NAND_ADDR_HIGH);
	_SET_NODATA(cmd);
	if (do_cmd(cmd) < 0)
		return -1;
	if (wait_status(NAND_STATUS_FAIL, 0) < 0)
		return -1;
	return 0;
}

int __nand_chip_write_page(struct mtd_info *mtd, int page, int cache)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	u32 cmd, tmp;
	u32 len = mtd->writesize;
	u32 blk = mtd->writesize >> 10;
	struct nand_save save;

	nand_save(&save);
	nand_set_page(info->page);
	nand_set_ecc(info->ecc);
	enable_random(page);
	enable_ecc(1);
	nand_to_ahb();
	cmd = NAND_WCMD
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_SEND_SECOND_CMD
		| NAND_CMD_TYPE_PAGE;
	if (cache)
		writel(NAND_WSET_C, NAND_WCMD_SET);
	else
		writel(NAND_WSET, NAND_WCMD_SET);
	_SET_ADDR(cmd, 5);
	tmp = (page & 0xffffU) << 16;
	writel(tmp, NAND_ADDR_LOW);
	tmp = (page & 0xff0000U) >> 16;
	writel(tmp, NAND_ADDR_HIGH);
	_SET_WRITE(cmd);
	writel(blk, NAND_BLOCK_NUM);
	if (do_cmd_dma(cmd, 1, wbuff_dma, len) < 0) {
		nand_restore(&save);
		return -1;
	}
	nand_restore(&save);
	if (cache)
		return 0;
	if (wait_status(NAND_STATUS_FAIL, 0) < 0)
		return -1;
	return 0;
}

int __nand_chip_write_page1k(struct mtd_info *mtd, int page, int cache)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	u32 cmd, tmp;
	u32 len = mtd->writesize;
	u32 blk = mtd->writesize >> 10;
	struct nand_save save;

	nand_save(&save);
	nand_set_page(info->page);
	nand_set_ecc(info->ecc1k);
	enable_random1k();
	enable_ecc(1);
	nand_to_ahb();
	cmd = NAND_WCMD
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_SEND_SECOND_CMD
		| NAND_CMD_TYPE_PAGE
		| NAND_SEQ;
	if (cache)
		writel(NAND_WSET_C, NAND_WCMD_SET);
	else
		writel(NAND_WSET, NAND_WCMD_SET);
	_SET_ADDR(cmd, 5);
	tmp = (page & 0xffffU) << 16;
	writel(tmp, NAND_ADDR_LOW);
	tmp = (page & 0xff0000U) >> 16;
	writel(tmp, NAND_ADDR_HIGH);
	_SET_WRITE(cmd);
	writel(blk, NAND_BLOCK_NUM);
	if (do_cmd_dma(cmd, 1, wbuff_dma, len) < 0) {
		nand_restore(&save);
		return -1;
	}
	nand_restore(&save);
	if (cache)
		return 0;
	if (wait_status(NAND_STATUS_FAIL, 0) < 0)
		return -1;
	return 0;
}

int __do_write_page(struct mtd_info *mtd, int column, int page, int cache)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);

	if (column > 0) {
		nand_err("%s: column %d\n", __func__, column);
		return -1;
	}
	if (page < info->pages1k)
		return __nand_chip_write_page1k(mtd, page, cache);
	else
		return __nand_chip_write_page(mtd, page, cache);
}

/* linux kernel funcs */


u8 chip_read_byte(struct mtd_info *mtd)
{
	u8 *pp = (rbuff + rbp);

	if ((void *)pp < rbuff || (void *)pp >= (rbuff + BUFF_SZ) || rbl == 0)
		return 0xffU;
	rbp++;
	rbl--;
	return *pp;
}

u16 chip_read_word(struct mtd_info *mtd)
{
	u16 *pp = (rbuff + rbp);

	if ((void *)pp < rbuff || (void *)pp >= (rbuff + BUFF_SZ) || rbl == 0)
		return 0xffffU;
	rbp += 2;
	rbl -= 2;
	return *pp;
}

void chip_write_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
	int l = min(wbl, len);
	memcpy(wbuff + wbp, buf, l);
	wbp += l;
	wbl -= l;
}

void chip_read_buf(struct mtd_info *mtd, u8 *buf, int len)
{
	int l = min(rbl, len);
	memcpy(buf, rbuff + rbp, l);
	rbp += l;
	rbl -= l;
}

int chip_verify_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
	return (memcmp(buf, rbuff, len) ? -EFAULT : 0);
}

void chip_select_chip(struct mtd_info *mtd, int chip)
{
	if (chip < 0)
		return;
	nand_set_cesel((u32)chip);
}

int chip_block_bad(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	return 0;
}

int chip_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	return 0;
}

void chip_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
	return;
}

int chip_init_size(struct mtd_info *mtd, struct nand_chip *chip, u8 *id_data)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	struct nand_flash_dev *d;
	int err, ecc, found = 0;
	u32 blks, ecc_blk;

	for (d = nand_flash_ids; d->name; d++)
		if (id_data[1] == d->id) {
			found++;
			break;
		}
	if (!found)
		return -ENODEV;
	if (d->pagesize || d->erasesize) /* ignore very old chips now */
		return -ENODEV;
	chip->options = d->options;
	mtd->size = d->chipsize;
	mtd->size <<= 20;
	mtd->writebufsize = BUFF_SZ;
#if 0
	switch (id_data[0]) {
	case NAND_MFR_HYNIX:
	case NAND_MFR_SAMSUNG:
		err = hynix_init_size(mtd, chip, id_data);
		break;
	default:
		err = -ENODEV;
		break;
	}
#else
	err = hynix_init_size(mtd, chip, id_data);
#endif
	if (err < 0)
		return err;
	info->page = mtd->writesize_shift - 10;
	blks = 1 << info->page;
	/* get params from fex */
	info->pages1k = get_nand_param("pages1k");
	if (info->pages1k < 0)
		info->pages1k = PAGES_1K;
	nand_info("PAGES1k set to %d pages (%uK)\n", info->pages1k, info->pages1k << info->page);
	info->ecc = get_nand_param("ecc");
	if (info->ecc < 0) { /* calc maximum */
		ecc_blk = err / blks;
		for (ecc = NAND_ECC_BCH64; ecc >= NAND_ECC_BCH16; ecc--)
			if (ecc_blk >= (ecc_bytes[ecc] & 0xffffU))
				break;
		if (ecc < 0)
			ecc = 0;
		info->ecc = ecc;
	}
	nand_info("ECC set to %d (%ubits/1K)\n", info->ecc, ecc_bytes[info->ecc] >> 16);
	info->ecc1k = get_nand_param("ecc1k");
	if (info->ecc1k < 0)
		info->ecc1k = ECC_1K_DEF;
	nand_info("ECC1k set to %d (%ubits/1K)\n", info->ecc1k, ecc_bytes[info->ecc1k] >> 16);
	memset(&info->el, 0, sizeof(struct nand_ecclayout));
	info->el.oobfree[0].length = blks << 2; /* one u32 per ecc block */
	mtd->oobsize = blks << 2;
	chip->ecc.mode = NAND_ECC_HW;
	chip->ecc.steps = blks;
	chip->ecc.size = SZ_1K;
	chip->ecc.bytes = ecc_bytes[info->ecc] & 0xffffU;
	chip->ecc.total = chip->ecc.bytes * blks;
	chip->ecc.strength = ecc_bytes[info->ecc] >> 16;
	chip->ecc.layout = &info->el;
	nand_set_page(info->page);
	nand_set_ecc(info->ecc);
	disable_random();
	disable_ecc();
	chip->options |= NAND_NO_SUBPAGE_WRITE;
	chip->bbt_options |= NAND_BBT_USE_FLASH | NAND_BBT_NO_OOB | NAND_BBT_PERCHIP;
	return (d->options & NAND_BUSWIDTH_16) ? 1 : 0;
}

int chip_dev_ready(struct mtd_info *mtd)
{
	return ((readl(NAND_ST) & NAND_RB_ALL) == NAND_RB_ALL);
}

static inline int send_cmd(u32 cmd)
{
	writel(cmd, NAND_CMD);
	return wait_cmd_finish();
}

static inline int send_cmd_dma(u32 cmd, int dma_rw, dma_addr_t dma_addr, u32 dma_len)
{
	nand_to_dma();
	nand_start_dma(dma_rw, dma_addr, dma_len);
	writel(cmd, NAND_CMD);
	if (wait_dma_finish() < 0) {
		nand_to_ahb();
		return -1;
	}
	nand_to_ahb();
	return wait_cmd_finish();
}

void chip_erase_cmd(struct mtd_info *mtd, int page)
{
	__nand_chip_erase(mtd, page);
}

void chip_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
	int err;
	u8 *pp = rbuff;

//	nand_err("%s: %04x, %d, %d\n", __func__, command, column, page_addr);
	switch (command) {
	case NAND_CMD_RESET:
		__nand_chip_reset();
		break;
	case NAND_CMD_READID:
		__nand_chip_readid(column);
		break;
	case NAND_CMD_PARAM:
		__nand_chip_param(column);
		break;
	case NAND_CMD_READOOB:
		__do_read_oob(mtd, page_addr);
		break;
	case NAND_CMD_READ0:
		__do_read_page(mtd, column, page_addr);
		break;
	case NAND_CMD_STATUS:
		_RESET_RB;
		err = get_status();
		if (err < 0)
			*pp = 0xffU;
		else
			*pp = (u8)err;
		rbl = 1;
		break;
	case NAND_CMD_ERASE1:
		__nand_chip_erase(mtd, page_addr);
		break;
	case NAND_CMD_ERASE2:
		break;
	default:
		nand_err("%s: %02x, %d, %d\n", __func__, command, column, page_addr);
		break;
	}
}

int chip_waitfunc(struct mtd_info *mtd, struct nand_chip *chip)
{
	return wait_status(NAND_STATUS_FAIL, 0);
}

int chip_scan_bbt(struct mtd_info *mtd)
{
	nand_err("%s:\n", __func__);
	return 0;
}

int chip_write_page(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf, int page, int cached, int raw)
{
	int i;
	u32 *oob = (u32 *)chip->oob_poi;

	_RESET_WB;
	chip_write_buf(mtd, buf, mtd->writesize);
	for (i = 0; i < chip->ecc.steps; i++)
		writel(oob[i], NAND_USER_DATA(i));
	if (__do_write_page(mtd, 0, page, cached) < 0)
		return -EIO;
	return 0;
}

/* ecc funcs */

void chip_ecc_hwctl(struct mtd_info *mtd, int mode)
{
	return;
}

int chip_ecc_calculate(struct mtd_info *mtd, const u8 *dat, u8 *ecc_code)
{
	return 0;
}

int chip_ecc_correct(struct mtd_info *mtd, u8 *dat, u8 *read_ecc, u8 *calc_ecc)
{
	return 0;
}

static int __chip_ecc_read_page(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf, int page, int raw)
{
	int i, err;
	u32 ecc_st;
	u32 *oob = (u32 *)chip->oob_poi;
	u32 mask = (1 << chip->ecc.steps) - 1;

	ecc_st = readl(NAND_ECC_ST) & mask;
	if (ecc_st == mask) { /* empty page */
		memset(buf, 0xff, mtd->writesize);
		for (i = 0; i < chip->ecc.steps; i++)
			oob[i] = 0xffffffffU;
	} else {
		if (ecc_st)
			nand_err("ECC status for page %d is 0x%04x\n", page, ecc_st);
		chip_read_buf(mtd, buf, mtd->writesize);
		for (i = 0; i < chip->ecc.steps; i++) {
			oob[i] = readl(NAND_USER_DATA(i));
			if (!raw) {
				err = ecc_result(chip, i);
				if (err < 0)
					mtd->ecc_stats.failed++;
				else
					mtd->ecc_stats.corrected += err;
			}
		}
	}
	return 0;
}

int chip_ecc_read_page(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf, int page)
{
	return __chip_ecc_read_page(mtd, chip, buf, page, 0);
}

int chip_ecc_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf, int page)
{
	return __chip_ecc_read_page(mtd, chip, buf, page, 1);
}

int chip_ecc_read_oob(struct mtd_info *mtd, struct nand_chip *chip, int page, int sndcmd)
{
	int i;
	u32 *oob = (u32 *)chip->oob_poi;
	u32 mask = (1 << chip->ecc.steps) - 1;

	if (sndcmd) {
		if (__do_read_page(mtd, 0, page) < 0)
			return -EIO;
	}
	_RESET_RB;
	if ((readl(NAND_ECC_ST) & mask) == mask)
		for (i = 0; i < chip->ecc.steps; i++)
			oob[i] = 0xffffffffU;
	else
		for (i = 0; i < chip->ecc.steps; i++)
			oob[i] = readl(NAND_USER_DATA(i));
	return 0;
}

void chip_ecc_write_page(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf)
{
	int i;
	u32 *oob = (u32 *)chip->oob_poi;

	_RESET_WB;
	chip_write_buf(mtd, buf, mtd->writesize);
	for (i = 0; i < chip->ecc.steps; i++)
		writel(oob[i], NAND_USER_DATA(i));
}

int chip_ecc_write_oob(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	int i;
	u32 *oob = (u32 *)chip->oob_poi;

	_RESET_WB;
	memset(wbuff, 0xffU, mtd->writesize);
	for (i = 0; i < chip->ecc.steps; i++)
		writel(oob[i], NAND_USER_DATA(i));
	if (__do_write_page(mtd, 0, page, 0) < 0)
		return -EIO;
	return 0;
}
