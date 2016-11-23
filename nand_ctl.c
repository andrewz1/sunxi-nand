#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/cacheflush.h>

#include "main.h"

#define BUFF_SZ (SZ_16K)

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

static const u8 ecc_bytes[] = { 32, 46, 54, 60, 74, 88, 102, 110, 116, };
static const u8 ecc_bits[] =  { 16, 24, 28, 32, 40, 48,  56,  60,  64, };

static u8 *buff;
static dma_addr_t buff_dma;
static int rbp; /* cur buff pos for read */

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
	clrset_wbit(NAND_CTL, NAND_RB_SEL, (rb & 0x3) << 8);
}

static inline void nand_set_cesel(int chip)
{
	if (chip < 0)
		set_wbit(NAND_CTL, NAND_CE_SEL);
	else
		clrset_wbit(NAND_CTL, NAND_CE_SEL, (chip & 0xf) << 24);
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
#define _ECC_MASK_EN (NAND_ECC_EN | NAND_ECC_PIPELINE | NAND_ECC_EXCEPTION)
#define _ECC_MASK_DS (NAND_ECC_EN | NAND_ECC_PIPELINE | NAND_ECC_EXCEPTION | NAND_ECC_BLOCK_SIZE)

static inline void disable_ecc(void)
{
	clr_wbit(NAND_ECC_CTL, _ECC_MASK_DS);
}

/* ecc block size set to 1024 */
static inline void enable_ecc(void)
{
	clrset_wbit(NAND_ECC_CTL, _ECC_MASK_DS, _ECC_MASK_EN);
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

static inline int do_cmd(u32 cmd)
{
	if (wait_cmd_fifo())
		return -1;
	writel(cmd, NAND_CMD);
	return wait_cmd_finish();
}

static inline int do_cmd_dma(u32 cmd, int rw, dma_addr_t dma_addr, u32 dma_len)
{
	int err;

	if (wait_cmd_fifo())
		return -1;
	nand_to_dma();
	nand_start_dma(rw, dma_addr, dma_len);
	writel(cmd, NAND_CMD);
	err = wait_dma_finish();
	nand_to_ahb();
	if (err)
		return -1;
	return wait_cmd_finish();
}

static inline u32 get_status(void)
{
	u32 cmd;

	nand_to_ahb();
	cmd = NAND_CMD_STATUS
		| NAND_SEND_FIRST_CMD;
	_SET_ADDR(cmd, 0);
	_SET_READ(cmd);
	writel(1, NAND_CNT);
	if (do_cmd(cmd))
		return NAND_ERR_STAT;
	return (readl(NAND_RAM0_BASE) & 0xff);
}

static inline int wait_status(u32 mask, u32 res)
{
	int timeout = NAND_TMO;
	u32 err;

	do {
		err = get_status();
		if (err == NAND_ERR_STAT)
			return -1;
		else if ((err & mask) == res)
			return err;
		udelay(1);
		timeout--;
	} while (timeout > 0);
	return -1;
}

#define _MAX_BITFLIP	4
/* 
 return real value of corrected bits if above (chip->ecc.strength - _MAX_BITFLIP)
 if we always return real value then periodic bitflip error occur and many
 eraseblocks fall in bbt
 todo: make this value configurable
 */
static inline u32 ecc_result(struct nand_chip *chip, int step)
{
	u32 reg = (step & 0xc) >> 2;
	u32 shift = (step & 0x3) << 3;
	u32 corr;

	 /* get corrected bits for step */
	corr = (readl(NAND_ERR_CNT(reg)) >> shift) & 0xff;
	if (corr > (chip->ecc.strength - _MAX_BITFLIP))
		return corr;
	return 0;
}

int nand_controller_init(void)
{
	int ret;

	buff = dma_alloc_coherent(NULL, BUFF_SZ, &buff_dma, GFP_KERNEL);
	if (!buff)
		return -ENOMEM;
	ret = nand_request_clk();
	if (ret < 0)
		goto free_dma;
	ret = nand_request_gpio();
	if (ret < 0)
		goto release_clk;
	nand_controller_reset();
	ret = nand_request_irq(); /* not used */
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
free_dma:
	dma_free_coherent(NULL, BUFF_SZ, buff, buff_dma);
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
	dma_free_coherent(NULL, BUFF_SZ, buff, buff_dma);
}

int __nand_chip_reset(void)
{
	int timeout = NAND_TMO;
	u32 cmd;

	nand_to_ahb();
	cmd = NAND_CMD_RESET
		| NAND_SEND_FIRST_CMD;
	_SET_ADDR(cmd, 0);
	_SET_NODATA(cmd);
	if (do_cmd(cmd))
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
	u32 cmd, len = 8;

	nand_to_ahb();
	cmd = NAND_CMD_READID
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_DATA_METHOD;
	_SET_ADDR(cmd, 1);
	writel(addr & 0xff, NAND_ADDR_LOW);
	_SET_READ(cmd);
	writel(len, NAND_CNT);
	return do_cmd_dma(cmd, 0, buff_dma, len);
		// return -1;
	// for (i = 4; i < 8; i++)
	// 	if (!(memcmp(buff, buff + i, 8 - i)))
	// 		break;
	// return 0;
}

int __nand_chip_param(int addr)
{
	u32 cmd, len = 1024;

	nand_to_ahb();
	cmd = NAND_CMD_PARAM
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_DATA_METHOD;
	_SET_ADDR(cmd, 1);
	writel(addr & 0xff, NAND_ADDR_LOW);
	_SET_READ(cmd);
	writel(len, NAND_CNT);
	return do_cmd_dma(cmd, 0, buff_dma, len);
}

int __nand_chip_read_page(struct mtd_info *mtd, int page)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	struct nand_chip *chip = &info->chip;
	u32 cmd;
	int err;

	rbp = 0;
	nand_set_page(info->page);
	nand_set_ecc(info->ecc);
	enable_random(page);
	enable_ecc();
	nand_to_ahb();
	cmd = NAND_RCMD
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_SEND_SECOND_CMD
		| NAND_CMD_TYPE_PAGE;
	writel(NAND_RSET, NAND_RCMD_SET);
	_SET_ADDR(cmd, 5);
	writel((page & 0xffffU) << 16, NAND_ADDR_LOW);
	writel((page & 0xff0000U) >> 16, NAND_ADDR_HIGH);
	_SET_READ(cmd);
	writel(chip->ecc.steps, NAND_BLOCK_NUM);
	err = do_cmd_dma(cmd, 0, buff_dma, mtd->writesize);
	disable_ecc();
	disable_random();
	return err;
}

int __nand_chip_read_page1k(struct mtd_info *mtd, int page)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	struct nand_chip *chip = &info->chip;
	u32 cmd;
	int err;

	rbp = 0;
	nand_set_page(info->page);
	nand_set_ecc(info->ecc1k);
	enable_random1k();
	enable_ecc();
	nand_to_ahb();
	cmd = NAND_RCMD
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_SEND_SECOND_CMD
		| NAND_CMD_TYPE_PAGE
		| NAND_SEQ;
	writel(NAND_RSET, NAND_RCMD_SET);
	_SET_ADDR(cmd, 5);
	writel((page & 0xffffU) << 16, NAND_ADDR_LOW);
	writel((page & 0xff0000U) >> 16, NAND_ADDR_HIGH);
	_SET_READ(cmd);
	writel(chip->ecc.steps, NAND_BLOCK_NUM);
	err = do_cmd_dma(cmd, 0, buff_dma, mtd->writesize);
	disable_ecc();
	disable_random();
	return err;
}

int __do_read_page(struct mtd_info *mtd, int page)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	struct nand_chip *chip = &info->chip;
	u32 *oob = (u32 *)chip->oob_poi;
	u32 err_mask = (1 << chip->ecc.steps) - 1;
	u32 empty_mask = err_mask << 16;
	int err, i;

	if (page < info->pages1k)
		err = __nand_chip_read_page1k(mtd, page);
	else
		err = __nand_chip_read_page(mtd, page);
	if (err < 0)
		return -1;
	if ((readl(NAND_ECC_ST) & empty_mask) == empty_mask) {
		memset(buff, 0xff, mtd->writesize);
		for (i = 0; i < chip->ecc.steps; i++)
			oob[i] = 0xffffffffU;
	} else {
		for (i = 0; i < chip->ecc.steps; i++)
			oob[i] = readl(NAND_USER_DATA(i));
	}
	return 0;
}

int __nand_chip_erase(struct mtd_info *mtd, int page)
{
	u32 pgs_per_blk = mtd->erasesize / mtd->writesize;
	u32 cmd;

	page = rounddown(page, pgs_per_blk);
	nand_to_ahb();
	cmd = NAND_ERASE
		| NAND_SEND_FIRST_CMD
		| NAND_WAIT_FLAG
		| NAND_SEND_SECOND_CMD;
	writel(NAND_ESET, NAND_RCMD_SET);
	_SET_ADDR(cmd, 3);
	writel(page, NAND_ADDR_LOW);
	_SET_NODATA(cmd);
	if (do_cmd(cmd))
		return -1;
	if (wait_status(NAND_STATUS_FAIL, 0) < 0)
		return -1;
	return 0;
}

int __nand_chip_write_page(struct mtd_info *mtd, int page, int cache)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	struct nand_chip *chip = &info->chip;
	u32 cmd;
	int err;

	nand_set_page(info->page);
	nand_set_ecc(info->ecc);
	enable_random(page);
	enable_ecc();
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
	writel((page & 0xffffU) << 16, NAND_ADDR_LOW);
	writel((page & 0xff0000U) >> 16, NAND_ADDR_HIGH);
	_SET_WRITE(cmd);
	writel(chip->ecc.steps, NAND_BLOCK_NUM);
	err = do_cmd_dma(cmd, 1, buff_dma, mtd->writesize);
	disable_ecc();
	disable_random();
	if (err)
		return -1;
	if (cache)
		return 0;
	if (wait_status(NAND_STATUS_FAIL, 0) < 0)
		return -1;
	return 0;
}

int __nand_chip_write_page1k(struct mtd_info *mtd, int page, int cache)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	struct nand_chip *chip = &info->chip;
	u32 cmd;
	int err;

	nand_set_page(info->page);
	nand_set_ecc(info->ecc1k);
	enable_random1k();
	enable_ecc();
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
	writel((page & 0xffffU) << 16, NAND_ADDR_LOW);
	writel((page & 0xff0000U) >> 16, NAND_ADDR_HIGH);
	_SET_WRITE(cmd);
	writel(chip->ecc.steps, NAND_BLOCK_NUM);
	err = do_cmd_dma(cmd, 1, buff_dma, mtd->writesize);
	disable_ecc();
	disable_random();
	if (err)
		return -1;
	if (cache)
		return 0;
	if (wait_status(NAND_STATUS_FAIL, 0) < 0)
		return -1;
	return 0;
}

int __do_write_page(struct mtd_info *mtd, int page, int cache)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	struct nand_chip *chip = &info->chip;
	u32 *oob = (u32 *)chip->oob_poi;
	int i;

	for (i = 0; i < chip->ecc.steps; i++)
		writel(oob[i], NAND_USER_DATA(i));
	if (page < info->pages1k)
		return __nand_chip_write_page1k(mtd, page, cache);
	else
		return __nand_chip_write_page(mtd, page, cache);
}

/* linux kernel funcs */

u8 chip_read_byte(struct mtd_info *mtd)
{
	u8 ret = buff[rbp];
	rbp++;
	return ret;
}

u16 chip_read_word(struct mtd_info *mtd)
{
	u16 *bp = (u16 *)(buff + rbp);
	u16 ret = *bp;
	rbp += 2;
	return ret;
}

void chip_write_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
	memcpy(buff, buf, len);
}

void chip_read_buf(struct mtd_info *mtd, u8 *buf, int len)
{
	memcpy(buf, buff + rbp, len);
	rbp += len;
}

void chip_select_chip(struct mtd_info *mtd, int chip)
{
	nand_set_cesel(chip);
}

void chip_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
	return;
}

int chip_init_size(struct mtd_info *mtd, struct nand_chip *chip, u8 *id_data)
{
	struct nand_info *info = container_of(mtd, struct nand_info, mtd);
	struct nand_flash_dev *d;
	int err, ecc_max, found = 0;
	u32 ecc_blks, ecc_blk, oobsize;

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
	oobsize = mtd->oobsize;
	mtd->oobsize = 0; /* just in case */
	mtd->writebufsize = mtd->writesize;
	info->page = ffs(mtd->writesize >> 10) - 1;
	ecc_blks = 1 << info->page;
	/* get params from fex */
	info->pages1k = get_nand_param("pages1k");
	if (info->pages1k < 0)
		info->pages1k = PAGES_1K;
	nand_info("PAGES1k set to %d pages (%uK)\n", info->pages1k, info->pages1k << info->page);
	info->ecc = get_nand_param("ecc");
	if (info->ecc < 0) { /* calc maximum */
		ecc_blk = oobsize / ecc_blks;
		for (ecc_max = NAND_ECC_BCH64; ecc_max >= NAND_ECC_BCH16; ecc_max--)
			if (ecc_blk >= (ecc_bytes[ecc_max]))
				break;
		if (ecc_max < 0)
			ecc_max = 0;
		info->ecc = ecc_max;
	}
	nand_info("ECC set to %d (%ubits/1K)\n", info->ecc, ecc_bits[info->ecc]);
	info->ecc1k = get_nand_param("ecc1k");
	if (info->ecc1k < 0)
		info->ecc1k = ECC_1K_DEF;
	nand_info("ECC1k set to %d (%ubits/1K)\n", info->ecc1k, ecc_bits[info->ecc1k]);
	memset(&info->el, 0, sizeof(struct nand_ecclayout));
	info->el.oobfree[0].length = ecc_blks * 4; /* one u32 per ecc block */
	mtd->oobsize = ecc_blks * 4;
	chip->ecc.mode = NAND_ECC_HW;
	chip->ecc.steps = ecc_blks;
	chip->ecc.size = SZ_1K;
	chip->ecc.bytes = ecc_bytes[info->ecc];
	chip->ecc.strength = ecc_bits[info->ecc];
	chip->ecc.total = chip->ecc.bytes * chip->ecc.steps;
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
	int err;

	nand_to_dma();
	nand_start_dma(dma_rw, dma_addr, dma_len);
	writel(cmd, NAND_CMD);
	err = wait_dma_finish();
	nand_to_ahb();
	if (err)
		return -1;
	nand_to_ahb();
	return wait_cmd_finish();
}

void chip_erase_cmd(struct mtd_info *mtd, int page)
{
	__nand_chip_erase(mtd, page);
}

void chip_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
	u32 *sp = (u32 *)buff;

	switch (command) {
	case NAND_CMD_RESET:
		__nand_chip_reset();
		break;
	case NAND_CMD_READID:
		rbp = 0;
		__nand_chip_readid(column);
		break;
	case NAND_CMD_PARAM:
		rbp = 0;
		__nand_chip_param(column);
		break;
	case NAND_CMD_READ0:
	case NAND_CMD_READOOB:
		__do_read_page(mtd, page_addr);
		break;
	case NAND_CMD_STATUS:
		rbp = 0;
		*sp = get_status();
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

int chip_write_page(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf, int page, int cached, int raw)
{
	memcpy(buff, buf, mtd->writesize);
	if (__do_write_page(mtd, page, cached) < 0)
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

int chip_ecc_read_page(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf, int page)
{
	u32 mask, ecc_st = readl(NAND_ECC_ST);
	int i;

	memcpy(buf, buff, mtd->writesize);
	for (i = 0; i < chip->ecc.steps; i++) {
		mask = 1 << i;
		if (ecc_st & mask)
			mtd->ecc_stats.failed++;
		else
			mtd->ecc_stats.corrected += ecc_result(chip, i);
	}
	return 0;
}

int chip_ecc_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf, int page)
{
	memcpy(buf, buff, mtd->writesize);
	return 0;
}

int chip_ecc_read_oob(struct mtd_info *mtd, struct nand_chip *chip, int page, int sndcmd)
{
	if (sndcmd)
		if (__do_read_page(mtd, page) < 0)
			return -EIO;
	return 0;
}

void chip_ecc_write_page(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf)
{
	memcpy(buff, buf, mtd->writesize);
}

int chip_ecc_write_oob(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	memset(buff, 0xff, mtd->writesize);
	if (__do_write_page(mtd, page, 0) < 0)
		return -EIO;
	return 0;
}
