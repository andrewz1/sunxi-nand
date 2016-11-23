#include <linux/kernel.h>
#include <linux/module.h>

#include "main.h"

/* 3rd byte in ID */
#define INT_CHIP_NUM_MASK			(0b00000011)
#define INT_CHIP_NUM_SHIFT			(0)

#define CELL_TYPE_MASK				(0b00001100)
#define CELL_TYPE_SHIFT				(2)

#define SIMUL_PAGES_MASK			(0b00110000)
#define SIMUL_PAGES_SHIFT			(4)

#define PROG_MULT_DIE_MASK			(0b01000000)

#define WRITE_CACHE_MASK			(0b10000000)

/* 4th byte in ID */
#define PAGE_SIZE_MASK				(0b00000011)
#define PAGE_SIZE_2K				(0b00000000)
#define PAGE_SIZE_4K				(0b00000001)
#define PAGE_SIZE_8K				(0b00000010)
// #define PAGE_SIZE_16K				(0b00000011)

#define BLOCK_SIZE_MASK				(0b10110000)
#define BLOCK_SIZE_128K				(0b00000000)
#define BLOCK_SIZE_256K				(0b00010000)
#define BLOCK_SIZE_512K				(0b00100000)
#define BLOCK_SIZE_768K				(0b00110000)
#define BLOCK_SIZE_1M				(0b10000000)
#define BLOCK_SIZE_2M				(0b10010000)

#define SPARE_SIZE_MASK				(0b01001100)
#define SPARE_SIZE_640				(0b01001000)
#define SPARE_SIZE_448				(0b00001000)
#define SPARE_SIZE_224				(0b00000100)
#define SPARE_SIZE_128				(0b00000000)
#define SPARE_SIZE_64				(0b00001100)
#define SPARE_SIZE_32				(0b01000000)
#define SPARE_SIZE_16				(0b01000100)

/* 5th byte in ID */
#define PLANE_NUM_MASK				(0b00001100)
#define PLANE_NUM_SHIFT				(2)

#define ECC_LEVEL_MASK				(0b01110000)
#define ECC_LEVEL_SHIFT				(4)
#define ECC_LEVEL_NONE				(0b00000000)
#define ECC_LEVEL_1_512				(0b00010000)
#define ECC_LEVEL_2_512				(0b00100000)
#define ECC_LEVEL_4_512				(0b00110000)
#define ECC_LEVEL_8_512				(0b01000000)
#define ECC_LEVEL_24_1K				(0b01010000)
#define ECC_LEVEL_32_1K				(0b01100000)
#define ECC_LEVEL_40_1K				(0b01110000)

/* 6th byte in ID */

#define NAND_TECH_MASK				(0b00000111)
#define NAND_TECH_SHIFT				(0)
#define NAND_TECH_48nm				(0b00000000)
#define NAND_TECH_41nm				(0b00000001)
#define NAND_TECH_32nm				(0b00000010)
#define NAND_TECH_26nm				(0b00000011)

#define EDO_SUP_MASK				(0b01000000)

#define SYNC_INT_MASK				(0b10000000)

int hynix_init_size(struct mtd_info *mtd, struct nand_chip *chip, u8 *id_data)
{
	/* page size */
	switch (id_data[3] & PAGE_SIZE_MASK) {
	case PAGE_SIZE_2K:
		mtd->writesize = SZ_2K;
		break;
	case PAGE_SIZE_4K:
		mtd->writesize = SZ_4K;
		break;
	case PAGE_SIZE_8K:
		mtd->writesize = SZ_8K;
		break;
	default:
		mtd->writesize = SZ_16K; /* check this */
		break;
	}
	/* block size */
	switch (id_data[3] & BLOCK_SIZE_MASK) {
	case BLOCK_SIZE_128K:
		mtd->erasesize = SZ_128K;
		break;
	case BLOCK_SIZE_256K:
		mtd->erasesize = SZ_256K;
		break;
	case BLOCK_SIZE_512K:
		mtd->erasesize = SZ_512K;
		break;
	case BLOCK_SIZE_768K: /* need to test */
		mtd->erasesize = SZ_512K + SZ_256K;
		break;
	case BLOCK_SIZE_1M:
		mtd->erasesize = SZ_1M;
		break;
	case BLOCK_SIZE_2M:
		mtd->erasesize = SZ_2M;
		break;
	default:
		return -ENODEV;
		break;
	}
	/* real oob size */
	switch (id_data[3] & SPARE_SIZE_MASK) {
	case SPARE_SIZE_640:
		mtd->oobsize = 640;
		break;
	case SPARE_SIZE_448:
		mtd->oobsize = 448;
		break;
	case SPARE_SIZE_224:
		mtd->oobsize = 224;
		break;
	case SPARE_SIZE_128:
		mtd->oobsize = 128;
		break;
	case SPARE_SIZE_64:
		mtd->oobsize = 64;
		break;
	case SPARE_SIZE_32:
		mtd->oobsize = 32;
		break;
	case SPARE_SIZE_16:
		mtd->oobsize = 16;
		break;
	default:
		return -ENODEV;
		break;
	}
	return 0;
}
