#ifndef _SUNXI_NAND_NAND_CTL_H
#define _SUNXI_NAND_NAND_CTL_H

#define NAND_TMO	100000 /* us */
#define ECC_1K_DEF	1
#define PAGES_1K	512

#define _SET_ADDR(c, x) do {	\
	(c) &= ~NAND_ADR_NUM;		\
	if ((x)) {					\
		(c) |= NAND_SEND_ADR;	\
		(c) |= ((x) - 1) << 16;	\
	} else						\
		(c) &= ~NAND_SEND_ADR;	\
} while (0)

#define _SET_READ(c) do {		\
	(c) |= NAND_DATA_TRANS;		\
	(c) &= ~NAND_ACCESS_DIR;	\
} while (0);

#define _SET_WRITE(c) do {		\
	(c) |= NAND_DATA_TRANS;		\
	(c) |= NAND_ACCESS_DIR;		\
} while (0);

#define _SET_NODATA(c) do {		\
	(c) &= ~NAND_DATA_TRANS;	\
	(c) &= ~NAND_ACCESS_DIR;	\
} while (0);

struct nand_save {
	u32 ctl;
	u32 ecc;
	u32 oob;
};

int nand_controller_init(void);
void nand_controller_exit(void);

int __nand_chip_reset(void);
int __nand_chip_readid(int addr);
// int nand_chip_read_page(void __iomem *b, u32 col, u32 page);
// int nand_chip_read_page1k(void __iomem *b, u32 page, u32 count);
// int nand_chip_read_boot(void __iomem *b, u32 page, u32 ecc, u32 read_size);
// int nand_chip_detect(void);

void __test_rew(struct mtd_info *mtd);

int hynix_init_size(struct mtd_info *mtd, struct nand_chip *chip, u8 *id_data);

u8 chip_read_byte(struct mtd_info *mtd);
u16 chip_read_word(struct mtd_info *mtd);
void chip_write_buf(struct mtd_info *mtd, const u8 *buf, int len);
void chip_read_buf(struct mtd_info *mtd, u8 *buf, int len);
int chip_verify_buf(struct mtd_info *mtd, const u8 *buf, int len);
void chip_select_chip(struct mtd_info *mtd, int chip);
int chip_block_bad(struct mtd_info *mtd, loff_t ofs, int getchip);
int chip_block_markbad(struct mtd_info *mtd, loff_t ofs);
void chip_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl);
int chip_init_size(struct mtd_info *mtd, struct nand_chip *this, u8 *id_data);
int chip_dev_ready(struct mtd_info *mtd);
void chip_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr);
int chip_waitfunc(struct mtd_info *mtd, struct nand_chip *this);
void chip_erase_cmd(struct mtd_info *mtd, int page);
int chip_scan_bbt(struct mtd_info *mtd);
int chip_write_page(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf, int page, int cached, int raw);

void chip_ecc_hwctl(struct mtd_info *mtd, int mode);
int chip_ecc_calculate(struct mtd_info *mtd, const u8 *dat, u8 *ecc_code);
int chip_ecc_correct(struct mtd_info *mtd, u8 *dat, u8 *read_ecc, u8 *calc_ecc);

int chip_ecc_read_page(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf, int page);
int chip_ecc_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf, int page);
// int chip_ecc_read_subpage(struct mtd_info *mtd, struct nand_chip *chip, u32 offs, u32 len, u8 *buf);
int chip_ecc_read_oob(struct mtd_info *mtd, struct nand_chip *chip, int page, int sndcmd);
int chip_ecc_read_oob_raw(struct mtd_info *mtd, struct nand_chip *chip, int page, int sndcmd);

void chip_ecc_write_page(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf);
void chip_ecc_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf);
int chip_ecc_write_oob(struct mtd_info *mtd, struct nand_chip *chip, int page);
int chip_ecc_write_oob_raw(struct mtd_info *mtd, struct nand_chip *chip, int page);

#endif
