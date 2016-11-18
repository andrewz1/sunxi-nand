#ifndef _SUNXI_NAND_REGS_H
#define _SUNXI_NAND_REGS_H

#include <mach/hardware.h>

#define NAND_PHY_BASE				0x01c03000U
#define NAND_BASE					IO_ADDRESS(NAND_PHY_BASE)

#define NAND_o_CTL					0x00
#define NAND_o_ST					0x04
#define NAND_o_INT					0x08
#define NAND_o_TIMING_CTL			0x0c
#define NAND_o_TIMING_CFG			0x10
#define NAND_o_ADDR_LOW				0x14
#define NAND_o_ADDR_HIGH			0x18
#define NAND_o_BLOCK_NUM			0x1c
#define NAND_o_CNT					0x20
#define NAND_o_CMD					0x24
#define NAND_o_RCMD_SET				0x28
#define NAND_o_WCMD_SET				0x2c
#define NAND_o_IO_REG				0x30
#define NAND_o_ECC_CTL				0x34
#define NAND_o_ECC_ST				0x38
#define NAND_o_EFR					0x3c
#define NAND_o_ERR_CNT(n)			(4 * (n) + 0x40) /* n = 0..3 */
#define NAND_o_USER_DATA(n)			(4 * (n) + 0x50) /* n = 0..15 */
#define NAND_o_EFNAND_STA			0x90
#define NAND_o_SPARE_AREA			0xa0
#define NAND_o_PAT_ID				0xa4
#define NAND_o_RDATA_STA_CTL		0xa8
#define NAND_o_RDATA_STA_0			0xac
#define NAND_o_RDATA_STA_1			0xb0
#define NAND_o_MDMA_ADDR			0xc0
#define NAND_o_MDMA_CNT				0xc4
#define NAND_o_IO_DATA				0x300
#define NAND_o_RAM0_BASE			0x400
#define NAND_o_RAM1_BASE			0x800

#define NAND_CTL					(NAND_BASE + NAND_o_CTL)
#define NAND_ST						(NAND_BASE + NAND_o_ST)
#define NAND_INT					(NAND_BASE + NAND_o_INT)
#define NAND_TIMING_CTL				(NAND_BASE + NAND_o_TIMING_CTL)
#define NAND_TIMING_CFG				(NAND_BASE + NAND_o_TIMING_CFG)
#define NAND_ADDR_LOW				(NAND_BASE + NAND_o_ADDR_LOW)
#define NAND_ADDR_HIGH				(NAND_BASE + NAND_o_ADDR_HIGH)
#define NAND_BLOCK_NUM				(NAND_BASE + NAND_o_BLOCK_NUM)
#define NAND_CNT					(NAND_BASE + NAND_o_CNT)
#define NAND_CMD					(NAND_BASE + NAND_o_CMD)
#define NAND_RCMD_SET				(NAND_BASE + NAND_o_RCMD_SET)
#define NAND_WCMD_SET				(NAND_BASE + NAND_o_WCMD_SET)
#define NAND_IO_REG					(NAND_PHY_BASE + NAND_o_IO_REG) /* for DMA */
#define NAND_ECC_CTL				(NAND_BASE + NAND_o_ECC_CTL)
#define NAND_ECC_ST					(NAND_BASE + NAND_o_ECC_ST)
#define NAND_EFR					(NAND_BASE + NAND_o_EFR)
#define NAND_ERR_CNT(n)				(NAND_BASE + NAND_o_ERR_CNT(n))
#define NAND_USER_DATA(n)			(NAND_BASE + NAND_o_USER_DATA(n))
#define NAND_EFNAND_STA				(NAND_BASE + NAND_o_EFNAND_STA)
#define NAND_SPARE_AREA				(NAND_BASE + NAND_o_SPARE_AREA)
#define NAND_PAT_ID					(NAND_BASE + NAND_o_PAT_ID)
#define NAND_RDATA_STA_CTL			(NAND_BASE + NAND_o_RDATA_STA_CTL)
#define NAND_RDATA_STA_0			(NAND_BASE + NAND_o_RDATA_STA_0)
#define NAND_RDATA_STA_1			(NAND_BASE + NAND_o_RDATA_STA_1)
#define NAND_MDMA_ADDR				(NAND_BASE + NAND_o_MDMA_ADDR)
#define NAND_MDMA_CNT				(NAND_BASE + NAND_o_MDMA_CNT)
#define NAND_IO_DATA				(NAND_BASE + NAND_o_IO_DATA)
#define NAND_RAM0_BASE				(NAND_BASE + NAND_o_RAM0_BASE)
#define NAND_RAM1_BASE				(NAND_BASE + NAND_o_RAM1_BASE)

/* NAND_CTL */
#define NAND_EN						(1 << 0)
#define NAND_RESET					(1 << 1)
#define NAND_BUS_WIDTH				(1 << 2)
#define NAND_RB_SEL					(0x3 << 3)
#define NAND_CE_ACT					(1 << 6)
#define NAND_PAGE_SIZE				(0xf << 8)
#define NAND_RAM_METHOD				(1 << 14)
#define NAND_DMA_TYPE				(1 << 15)
#define NAND_ALE_POL				(1 << 16)
#define NAND_CLE_POL				(1 << 17)
#define NAND_NF_TYPE				(0x3 << 18)
#define NAND_DDR_REN				(1 << 20)
#define NAND_DDR_RM					(1 << 21)
#define NAND_CE_SEL					(0xf << 24)

#define NAND_PAGE_1K				0
#define NAND_PAGE_2K				1
#define NAND_PAGE_4K				2
#define NAND_PAGE_8K				3
#define NAND_PAGE_16K				4
#define NAND_PAGE_SHIFT				8
#define NAND_PAGE_MASK				(0xf << NAND_PAGE_SHIFT)

/* NAND_ST */
#define NAND_B2R_INT_FLAG			(1 << 0)
#define NAND_CMD_INT_FLAG			(1 << 1)
#define NAND_DMA_INT_FLAG			(1 << 2)
#define NAND_CMD_FIFO_STATUS		(1 << 3)
#define NAND_STA					(1 << 4)
#define NAND_RB_STATE0				(1 << 8)
#define NAND_RB_STATE1				(1 << 9)
#define NAND_RB_STATE2				(1 << 10)
#define NAND_RB_STATE3				(1 << 11)
#define NAND_RDATA_STA1				(1 << 12)
#define NAND_RDATA_STA0				(1 << 13)

#define NAND_RB_ALL					(0xf << 8)

/* NAND_INT */
#define NAND_B2R_INT_ENABLE			(1 << 0)
#define NAND_CMD_INT_ENABLE			(1 << 1)
#define NAND_DMA_INT_ENABLE			(1 << 2)

#define NAND_INT_MASK				(0x7 << 0)

/* NAND_TIMING_CTL */
#define NAND_DC_CTL					(0x3f << 0)
#define NAND_READ_PIPE				(0xf << 8)

/* NAND_TIMING_CFG */
#define NAND_T_WB					(0x3 << 0)
#define NAND_T_ADL					(0x3 << 3)
#define NAND_T_WHR					(0x3 << 4)
#define NAND_T_RHW					(0x3 << 6)
#define NAND_T_CAD					(0x7 << 8)
#define NAND_T_CDQSS				(1 << 11)
#define NAND_T_CS					(0x3 << 12)
#define NAND_T_CLHZ					(0x3 << 14)
#define NAND_T_CCS					(0x3 << 16)
#define NAND_T_WC					(0x3 << 18)

/* NAND_ADDR_LOW */
#define NAND_ADDR_DATA1				(0xff << 0)
#define NAND_ADDR_DATA2				(0xff << 8)
#define NAND_ADDR_DATA3				(0xff << 16)
#define NAND_ADDR_DATA4				(0xff << 24)

/* NAND_ADDR_HIGH */
#define NAND_ADDR_DATA5				(0xff << 0)
#define NAND_ADDR_DATA6				(0xff << 8)
#define NAND_ADDR_DATA7				(0xff << 16)
#define NAND_ADDR_DATA8				(0xff << 24)

/* NAND_BLOCK_NUM */
#define NAND_DATA_BLOCK_NUM			(0x1f << 0)

/* NAND_CNT */
#define NAND_DATA_CNT				(0x3ff << 0)

/* NAND_CMD */
#define NAND_CMD_LOW_BYTE			(0xff << 0)
#define NAND_CMD_HIGH_BYTE			(0xff << 8)
#define NAND_ADR_NUM				(0x7 << 16)
#define NAND_SEND_ADR				(1 << 19)
#define NAND_ACCESS_DIR				(1 << 20)
#define NAND_DATA_TRANS				(1 << 21)
#define NAND_SEND_FIRST_CMD			(1 << 22)
#define NAND_WAIT_FLAG				(1 << 23)
#define NAND_SEND_SECOND_CMD		(1 << 24)
#define NAND_SEQ					(1 << 25)
#define NAND_DATA_METHOD			(1 << 26)
#define NAND_ROW_ADDR_AUTO			(1 << 27)
#define NAND_SEND_THIRD_CMD			(1 << 28)
#define NAND_SEND_FOURTH_CMD		(1 << 29)
#define NAND_CMD_TYPE				(0x3 << 30)

#define NAND_CMD_TYPE_COMMON		(0x0 << 30)
#define NAND_CMD_TYPE_SPECIAL		(0x1 << 30)
#define NAND_CMD_TYPE_PAGE			(0x2 << 30)

/* NAND_RCMD_SET */
#define NAND_READ_CMD				(0xff << 0)
#define NAND_RANDOM_READ_CMD0		(0xff << 8)
#define NAND_RANDOM_READ_CMD1		(0xff << 16)

#define NAND_RCMD					0x00
#define NAND_RSET					0x00e00530U /* read */

#define NAND_ERASE					0x60
#define NAND_ESET					0x000000d0U /* erase */

/* NAND_WCMD_SET */
#define NAND_PROGRAM_CMD			(0xff << 0)
#define NAND_RANDOM_WRITE_CMD		(0xff << 8)
#define NAND_READ_CMD1				(0xff << 16)
#define NAND_READ_CMD0				(0xff << 24)

#define NAND_WCMD					0x80
#define NAND_WSET					0x70008510U /* write */
#define NAND_WSET_C					0x70008515U /* cache write */

/* NAND_ECC_CTL */
#define NAND_ECC_EN					(1 << 0)
#define NAND_ECC_PIPELINE			(1 << 3)
#define NAND_ECC_EXCEPTION			(1 << 4)
#define NAND_ECC_BLOCK_SIZE			(1 << 5)
#define NAND_RND_EN					(1 << 9)
#define NAND_RND_DIRECTION			(1 << 10)
#define NAND_RND_SIZE				(1 << 11)

#define NAND_RND_SHIFT				16
#define NAND_RND_MASK				(0x7fffU << NAND_RND_SHIFT)
#define NAND_RND_1K					(0x4a80U << NAND_RND_SHIFT)
#define NAND_OOB_1K					0x271ade8cU

#define NAND_ECC_BCH16				0
#define NAND_ECC_BCH24				1
#define NAND_ECC_BCH28				2
#define NAND_ECC_BCH32				3
#define NAND_ECC_BCH40				4
#define NAND_ECC_BCH48				5
#define NAND_ECC_BCH56				6
#define NAND_ECC_BCH60				7
#define NAND_ECC_BCH64				8
#define NAND_ECC_SHIFT				12
#define NAND_ECC_MASK				(0xf << NAND_ECC_SHIFT)

/* NAND_ECC_ST */
#define NAND_ECC_ERR_MASK			0x0000ffffU
#define NAND_ECC_PAT_MASK			0xffff0000U

/* NAND_EFR */
#define NAND_ECC_DEBUG				(0x7f << 0)
#define NAND_WP_CTRL				(1 << 8)

/* NAND_ERR_CNT(n) */
#define n_ECC_COR_NUM(n)			(0xff << ((n) * 8)) /* n = 0..3 */

/* NAND_EFNAND_STA */
#define NAND_EF_NAND_STATUS			(0xff << 0)

/* NAND_SPARE_AREA */
#define NDFC_SPARE_ADR				(0xffff << 0)

/* NAND_PAT_ID */
#define n_PAT_ID(n)					(0x3 << ((n) * 2)) /* n = 0..15 */

/* NAND_RDATA_STA_CTL */
#define NAND_RDATA_STA_TH			(0x3ffff << 0)
#define NAND_RDATA_STA_EN			(1 << 24)

/* NAND_MDMA_CNT */
#define n_MDMA_CNT					(0x7fff << 0)

#define NAND_DEF_FREQ				20000000U	/* 20MHz */

// #define CMD_CHIP_RESET				0xffU
// #define CMD_CHIP_READID				0x90U
// #define CMD_CHIP_PAGE_READ			0x00U

#endif
