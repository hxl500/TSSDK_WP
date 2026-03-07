/*
 * Copyright (c) 2021, TsingMicro Technology Co., Ltd - All Rights Reserved
 *
 * SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause
 * Author      : liuyangyang@tsingmicro.com
 * Data        : 08-30-2021
 * Version     : V1.0
 * Description : SIU mode read/write one page
 *				DMA mode read/write less than 16 pages once time.
 *               Boot DMA, hw bch32, suppper squence, 2r2w, smal block, dma master support.
 *               AES, DMA slave, clear nand, toggle nand and sync mode not support.
 *
 * HWBBM:
 * ----------------------------------------------
 *          user blk      | good blk pool  |4 bbt
 * ----------------------------------------------
 * 4bbt in tail: search bbt description and store 1 bbt for mirror.
 * good-blk-pool: tail first to replease the bad block in user block.
 */

#ifndef TMS_TX536_NAND_H
#define TMS_TX536_NAND_H
#include <linux/mtd/rawnand.h>

#define PSEC_TO_MSEC(x) div_round_up((x), 1000000000ULL)

#define DEBUG

//#define __VALIDATION__
//#define __ATF__
//#define __UBOOT__
//#define __KERNEL__

#define __USE_HW_BBM__			(0) //do not use.
#define __USE_SW_ECC__			(0) //hardware ok, don't use software ecc.
//#define __USE_DMA__				//don't use dma in ATF.
//#define __CACHE_PAGE__		    //don't use cache page in ATF.
#define __TEST_CASE__			(0)

#if __TEST_CASE__
#define __ENABLE_ERASE__		(1)
#define __ENABLE_WRITE__		(1)
#define VERBOSE INFO	//TEST
#ifdef pr_debug
#undef pr_debug
#define pr_debug printk
#endif

#else
#define __ENABLE_ERASE__		(1)
#define __ENABLE_WRITE__		(1)
#endif

// 15page ok; 100page testing....
#ifdef __USE_DMA__
#define TEST_BYTES (2048 * 5)
#else
#define TEST_BYTES (2048 * 64 * 8)
#endif
#define TEST_OFFSET 0x200000
#define TEST_MEMx 0 //1

#ifndef __ATF__
#define DDR_MEM_ADDR_SRC (0x40000000) //test address.
#define DDR_MEM_ADDR_DST (0x42000000)
#define DDR_MEM_ADDR_SRC_2 (DDR_MEM_ADDR_SRC + 0x10)
#define DDR_MEM_ADDR_DST_2 (DDR_MEM_ADDR_DST + 0x10000000)

#define div_round_up(val, div) __extension__ ({	\
	__typeof__(div) _div = (div);		\
	((val) + _div - (__typeof__(div)) 1) / _div;		\
})

#else
#define DDR_MEM_ADDR_SRC (0xf1200000) //test address.
#define DDR_MEM_ADDR_DST (0xf1240000)
#define DDR_MEM_ADDR_SRC_2 (DDR_MEM_ADDR_SRC + 0x10)
#define DDR_MEM_ADDR_DST_2 (DDR_MEM_ADDR_DST + 0x3000)
#endif

#define TS_NAND_MAX_CLK		20000000
/* TS NAND definition */
#define NAND_BASE_ADDR		0xF0DA8000
#define BCH_GF_PARAM_M		14
#define BCH_DATA_UNIT		512
#define FLASH_SHORT_DELAY	100
#define FLASH_LONG_DELAY	1000
#define FLASH_WIDTH_16		16
#define FLASH_WIDTH_8		8
#define BBT_PAGE_MASK		0xffffff3f
#define WRITE_SIZE_512		512
#define WRITE_SIZE_2048		2048
#define WRITE_SIZE_4096		4096
#define WRITE_SIZE_8192		8192
#define ECC_STRENGTH_8		8
#define ECC_STRENGTH_16		16
#define ECC_STRENGTH_24		24
#define ECC_STRENGTH_32		32
#define ECC_STRENGTH_48		48
#define EMPTY_PAGE		0xff
#define ADDR1_MASK0		0x00ffffff
#define ADDR2_MASK0		0xff000000
#define ADDR1_MASK1		0xffff
#define ADDR1_MASK2		0xff
#define OOB_MASK		0xff
//#define EXT_ADDR_MASK		0x8000000

/* Status bits */
#define NAND_STATUS_FAIL	0x01
#define NAND_STATUS_FAIL_N1	0x02
#define NAND_STATUS_TRUE_READY	0x20
#define NAND_STATUS_READY	0x40
#define NAND_STATUS_WP		0x80

/* Bit field in FLAS_TYPE */
#define FLASH_PIN			BIT(15)
#define FLASH_TYPE_512			0x4000
#define FLASH_TYPE_2K			0x5000
#define FLASH_TYPE_4K			0x6000
#define FLASH_TYPE_8K			0x7000
#define FLASH_SIZE_CONFIGURABLEOOB	(0x0 << 9)
#define FLASH_SIZE_400OOB		(0x1 << 9)
#define FLASH_SIZE_436OOB		(0x2 << 9)
#define FLASH_SIZE_640OOB		(0x3 << 9)

/* Bit field in FLASH_STATUS */
#define NFLASHx_READY(x)		BIT(MEM0_ST + x)

/* Bit field in FLASH_NF_ACCESS */
#define NFLASH_ENABLE_ALTERNATIVE	(0x0 << 15)
#define AUTO_RESET			BIT(16)
#define DISABLE_AUTO_RESET		(0x0 << 16)
#define NFLASH_REG_WIDTH_RESERVED	(0x3 << 10)
#define NFLASH_REG_WIDTH_32		(0x2 << 10)
#define NFLASH_REG_WIDTH_16		(0x1 << 10)
#define NFLASH_REG_WIDTH_8		(0x0 << 10)

/* Bit field in FLASH_NF_COUNT */
#define REG_CMD_COUNT_EMPTY		0x3
#define REG_CMD_COUNT_3TOGO		0x2
#define REG_CMD_COUNT_2TOGO		0x1
#define REG_CMD_COUNT_1TOGO		0x0
#define REG_ADDR_COUNT_EMPTY		(0x7 << 4)
#define REG_ADDR_COUNT_5		(0x4 << 4)
#define REG_ADDR_COUNT_4		(0x3 << 4)
#define REG_ADDR_COUNT_3		(0x2 << 4)
#define REG_ADDR_COUNT_2		(0x1 << 4)
#define REG_ADDR_COUNT_1		(0x0 << 4)
#define REG_DATA_COUNT_EMPTY		(0x3fff << 8)
#define REG_DATA_COUNT_512_DATA		(0x1FF << 8)
#define REG_DATA_COUNT_2k_DATA		(0x7FF << 8)
#define REG_DATA_COUNT_4k_DATA		(0xFFF << 8)
#define REG_DATA_COUNT_DATA_1		(0x0 << 8)
#define REG_DATA_COUNT_DATA_2		(0x1 << 8)
#define REG_DATA_COUNT_DATA_3		(0x2 << 8)
#define REG_DATA_COUNT_DATA_4		(0x3 << 8)
#define REG_DATA_COUNT_DATA_5		(0x4 << 8)
#define REG_DATA_COUNT_DATA_6		(0x5 << 8)
#define REG_DATA_COUNT_DATA_7		(0x6 << 8)
#define REG_DATA_COUNT_DATA_8		(0x7 << 8)
#define REG_OOB_COUNT_EMPTY		(0x3ff << 22)

/* Bit field in FLASH_FLASH_ACCESS_START */
#define NFLASH_GO		BIT(0)
#define NFLASH_FIFO_REQ		BIT(2)
#define NFLASH_RD		BIT(13)
#define NFLASH_WT		(BIT(12) | BIT(13))

/* Bit field in FLASH_NF_ECC_RESET */
#define RESET_NFLASH_RESET	BIT(2)
#define RESET_NFLASH_FIFO	BIT(1)
#define RESET_NFLASH_ECC	BIT(0)
#define ECC_RESET_ALL \
	RESET_NFLASH_RESET | RESET_NFLASH_FIFO | RESET_NFLASH_ECC

/* Bit field in FLASH_NF_ECC_CONTROL */
#define ENABLE_ECC_GENERATION	BIT(8)
#define DISABLE_ECC_GENERATION	(0 << 8)

/* Flash FIFO control */
#define FIFO_READ		2
#define FIFO_WRITE		3

/* NFLASH INTERRUPT */
#define REGIRQ_CLEAR		BIT(0)
#define F_ADDR_ERR		2

/* BCH ECC field definition */
#define BCH_COMPARE		BIT(0)
#define	BCH_ENABLE		BIT(8)
#define	BCH_DISABLE		(0 << 8)
#define	BCH_DECODE		BIT(1)
#define	BCH_ENCODE		(0 << 1)
#define BCH_DECO_DONE		BIT(30)
#define BCH_GEN_DONE		BIT(31)
#define	BCH_UNCORRECTABLE	0x3
#define	BCH_CORRECTABLE_ERR	0x2
#define	BCH_NO_ERR		0x1
#define	BCH_BUSY		0x0
#define BCH_ERR_MASK		0x3
#define BCH_ERR_NUM_MASK	0x3F
#define BCH_ERR_LOC_MASK	0x3FFF
#define BCH_CORRECT_LOC_MASK	0x7
#define BCH_ERR_CAP_8		(0x0 << 9)
#define BCH_ERR_CAP_16		(0x1 << 9)
#define BCH_ERR_CAP_24		(0x2 << 9)
#define BCH_ERR_CAP_40		(0x3 << 9)

#define BCH_GF_PARAM_M		14


/* Definition for DMA bitfield */
#define TX_DMA_ENABLE	BIT(0)
#define RX_DMA_ENABLE	BIT(0)
#define DMA_CHECK_OWNER	BIT(1)
#define OWN_DMA			0
#define OWN_CPU			1

#define TS_DMA_DEPTH	3
#define TS_DMA_DESC_NUM	(BIT(0) << TS_DMA_DEPTH)
#define TS_DMA_Q_PTR_MASK	0x1fff

struct dma_q_base_depth_t {
	u32 depth		:  4 ; /* bits 3:0 */
	u32 base		: 28 ; /* bits 31:4 */
};

struct tx_descriptor_t {
	unsigned int buf_adr; /* Buff addr */
	unsigned int buf_adr_hi	:  8 ; /* bits 7:0 */
	unsigned int buf_len	:  16 ;  /* bits 23:8 */
	unsigned int sgm	:  1 ;  /* bits 24 */
	unsigned int rsrvd	:  6 ;  /* bits 30:25 */
	unsigned int own	:  1 ;  /* bits 31:31 */
};

struct rx_descriptor_t {
	unsigned int buf_adr; /* Buff addr */
	unsigned int buf_adr_hi	:  8 ; /* bits 7:0 */
	unsigned int buf_len	: 16 ;  /* bits 23:8 */
	unsigned int rsrvd	:  7 ;  /* bits 30:24 */
	unsigned int own	:  1 ;  /* bits 31:31 */
};

struct dma_global {
	u32 dma_glb_dma_lso_ctrl;
	u32 dma_glb_lso_interrupt;
	u32 dma_glb_lso_intenable;
	u32 dma_glb_dma_lso_vlan_tag_type0;
	u32 dma_glb_dma_lso_vlan_tag_type1;
	u32 dma_glb_dma_lso_axi_user_sel0;
	u32 dma_glb_axi_user_pat0;
	u32 dma_glb_axi_user_pat1;
	u32 dma_glb_axi_user_pat2;
	u32 dma_glb_axi_user_pat3;
	u32 dma_glb_fast_reg_pe0;
	u32 dma_glb_fast_reg_pe1;
	u32 dma_glb_dma_lso_tx_fdes_addr0;
	u32 dma_glb_dma_lso_tx_fdes_addr1;
	u32 dma_glb_dma_lso_tx_cdes_addr0;
	u32 dma_glb_dma_lso_tx_cdes_addr1;
	u32 dma_glb_dma_lso_tx_des_word0;
	u32 dma_glb_dma_lso_tx_des_word1;
	u32 dma_glb_dma_lso_lso_para_word0;
	u32 dma_glb_dma_lso_lso_para_word1;
	u32 dma_glb_dma_lso_debug0;
	u32 dma_glb_dma_lso_debug1;
	u32 dma_glb_dma_lso_debug2;
	u32 dma_glb_dma_lso_spare0;
	u32 dma_glb_dma_lso_spare1;
	u32 dma_glb_dma_ssp_rx_ctrl;
	u32 dma_glb_dma_ssp_tx_ctrl;
	u32 dma_glb_dma_ssp_axi_user_sel0;
	u32 dma_glb_dma_ssp_axi_user_sel1;
	u32 dma_glb_dma_ssp_rx_fdes_addr0;
	u32 dma_glb_dma_ssp_rx_fdes_addr1;
	u32 dma_glb_dma_ssp_rx_cdes_addr0;
	u32 dma_glb_dma_ssp_rx_cdes_addr1;
	u32 dma_glb_dma_ssp_rx_des_word0;
	u32 dma_glb_dma_ssp_rx_des_word1;
	u32 dma_glb_dma_ssp_tx_fdes_addr0;
	u32 dma_glb_dma_ssp_tx_fdes_addr1;
	u32 dma_glb_dma_ssp_tx_cdes_addr0;
	u32 dma_glb_dma_ssp_tx_cdes_addr1;
	u32 dma_glb_dma_ssp_tx_des_word0;
	u32 dma_glb_dma_ssp_tx_des_word1;
	u32 dma_glb_dma_ssp_debug0;
	u32 dma_glb_dma_ssp_debug1;
	u32 dma_glb_dma_ssp_debug2;
	u32 dma_glb_dma_ssp_spare0;
	u32 dma_glb_dma_ssp_spare1;
};

struct dma_ssp {
	u32 dma_q_rxq_control;
	u32 dma_q_rxq_base_depth;
	u32 dma_q_rxq_base;
	u32 dma_q_rxq_wptr;
	u32 dma_q_rxq_rptr;
	u32 dma_q_rxq_pktcnt;
	u32 dma_q_txq_control;
	u32 dma_q_txq_base_depth;
	u32 dma_q_txq_base;
	u32 dma_q_txq_wptr;
	u32 dma_q_txq_rptr;
	u32 dma_q_txq_pktcnt;
	u32 dma_q_rxq_interrupt;
	u32 dma_q_rxq_intenable;
	u32 dma_q_txq_interrupt;
	u32 dma_q_txq_intenable;
	u32 dma_q_rxq_misc_interrupt;
	u32 dma_q_rxq_misc_intenable;
	u32 dma_q_txq_misc_interrupt;
	u32 dma_q_txq_misc_intenable;
	u32 dma_q_rxq_coal_interrupt;
	u32 dma_q_rxq_coal_intenable;
	u32 dma_q_txq_coal_interrupt;
	u32 dma_q_txq_coal_intenable;
	u32 dma_q_rxq_frag_buff_addr0;
	u32 dma_q_rxq_frag_buff_addr1;
	u32 dma_q_rxq_frag_buff_size;
	u32 dma_q_txq_frag_buff_addr0;
	u32 dma_q_txq_frag_buff_addr1;
	u32 dma_q_txq_frag_buff_size;
	u32 dma_q_dma_spare_0;
	u32 dma_q_dma_spare_1;
};


#define NAND_ONFI_DETECT		(1)
#define WRITE_OOB				(1)

#define BBM_RMAP_DEV_SIZE		(0x01 << 13) // equal or greater than blocks
/* GD9FS8G8E3A NAND flash features
 * not support sync mode. not support onfi 2.x
 * not support mlun.
 */
// DEV > TARGET > LUN > PLANE > BLOCK > PAGE > CELL
#define ONE_PAGE_SIZE			(2048)	//2048 + 64 oob (0x200 count uint32_t register)
#define ONE_OOB_SIZE			(64)	//64byte per page.
#define ONE_BLOCK_SIZE			(64)	//pages 64 *(2048 + 64)
#define ONE_PLANE_SIZE			(2048)	//blocks 2048 * (128K + 4K)
#define ONE_LUN_SIZE			(2)		//planes 2 * 4096 * 2048 *(128K + 4K)
#define ONE_DEV_SIZE			(2)		//LUN 2 = 2 MEM
#define ONE_DEV_BYTES			(ONE_DEV_SIZE * ONE_LUN_SIZE * ONE_PLANE_SIZE * \
								 ONE_BLOCK_SIZE * ONE_BLOCK_SIZE) //1G BYTES

/* A0-A11:[12BIT = 0 ~ 4095] column address in the page, 0 ~ 2048+64-1 valid, otherwise error.
 * A12-A17:[6BIT = 0 ~ 63] one block size.
 * A18:[1BIT] plane address(for multi-plane operations) / block address (for normal operations)
 * A18-A29:[12BIT = 0 ~ 4095] block address.(A19-A29 11BIT 2048 for multi-plane operations.)
 * A30-A31:[2BIT = 0 ~ 3] LUN address.
 */
#define ROW_BIT				(12)	  //col bit: 0~11  row bit: 12,13,...
#define ROW_PLANE_BIT		(6) //18 - 12
#define BLOCK_BIT			ROW_PLANE_BIT
#define LUN_BIT				(18) //30 - 12
#define SECOND_PLANE		(0x1U << (ROW_PLANE_BIT + ROW_BIT))
#define SECOND_LUN			(0x1U << (LUN_BIT + ROW_BIT))
/* GD9FS8G8E3A NAND flash features END */


#define SETBIT(reg, bit) (reg = ((reg) | ((0x00000001) << (bit))))
#define RESETBIT(reg, bit) (reg = ((reg) & (~(0x00000001 << (bit)))))
#define RESET2BIT(reg, bit) (reg = ((reg) & (~(0x00000003 << (bit)))))
#define RESET3BIT(reg, bit) (reg = ((reg) & (~(0x00000007 << (bit)))))
#define GETBIT(reg, bit) ((reg) & ((0x00000001U) << (bit)))
#define GET2BIT(reg, bit) ((reg) & ((0x00000003) << (bit)))

#define TRUE		(1)
#define FALSE		(0)
//#define ERROR						(-1)

/** no seq_16 seq_17 seq_18 seq_19 seq_24 seq_25**/
//RESET COMMANDS
/* CMD tWB */
#define SEQ_0		(0x00)
/* CMD ADDR1 ADDR2 ADDR2 tWB */
#define SEQ_20		(0x14 + 0x20)

//IDENTIFICATION OPERATIONS
/* CMD ADDR tWHR (DATA0~N)*/
#define SEQ_1		(0x01 + 0x20)
/*CMD ADDR tWB(R-B-R) tRR (DATA0~N)*/
#define SEQ_2		(0x02 + 0x20)

//CONFIGURATION OPERATIONS
/*CMD ADDR tADL DATA0~N tWB(R-B-R)*/
#define SEQ_3		(0x03)

//STATUS OPERATIONS
/*CMD tWHR DATA0*/
#define SEQ_4		(0x04 + 0x20)
/*CMD ADDR0 ADDR1 ADDR2 tWHR DATA0*/
#define SEQ_5		(0x05 + 0x20)
/*CMD ADDR*/
#define SEQ_21		(0x15)

//COLUMN ADDRESS OPERATIONS
/*CMD ADDR1 ADDR2 CMD tCCS */
#define SEQ_6		(0x06 + 0x20)
/*CMD ADDR1~5 CMD tCCS (DATA0~N)*/
#define SEQ_7		(0x07 + 0x20)
/*CMD ADDR1~2 tCCS (DATA0~N)*/
#define SEQ_8		(0x08) //10 85 H ?
/*CMD ADDR1~5 tADL DATA0~N CMD tWB*/
#define SEQ_12		(0x0C)  //error?

//READ OPERATIONS
/*CMD ADDR1~5 CMD tWB tRR (DATA0~N)*/
#define SEQ_10		(0x0A + 0x20)
/*CMD (DATA...)*/
#define SEQ_11		(0x0B + 0x20)
/*CMD ADDR1~5 CMD*/
#define SEQ_9		(0x09 + 0x20)
/*CMD ADDR1~5 CMD ADDR1~5 CMD tWB*/
#define SEQ_15		(0x0F + 0x20)
/*CMD ADDR1~7 tRR (DATA0~N)*/
#define SEQ_22		(0x16 + 0x20)

//PROGRAM OPERATION
/*CMD ADDR1~5 tADL DATA0~N CMD tWB*/
#define SEQ_12		(0x0C)
/*CMD ADDR1~3 tADL DATA0~N CMD tWB*/
#define SEQ_23		(0x17)
/*CMD ADDR1~5 tADL (DATA0~N)*/
#define SEQ_13		(0x0D)

//ERASE OPERATION
/*CMD ADDR1~3 CMD tWB*/
#define SEQ_14		(0x0E)


/*CMD ADDR1~5 tRR DATA0~N*/
#define SEQ_17		(0x11 + 0x20)

#define SEQ_18		(0x12 + 0x20)
#define SEQ_19		(0x13)
/*CMD ADDR1~3 CMD ADDR1~3 CMD tWB*/
#define SEQ_24		(0x18)
/*CMD ADDR1~5 CMD ADDR1~2 CMD tWHR DATA0~N*/
#define SEQ_25		(0x19 + 0x20)

/*** CADENCE ONFI COMMANDS ***/
//non-directional-SEQ 0x00 0x0e 0x15 and write-SEQ 0x03 0x08 0x0c 0x0d 0x17 0x18
//RESET COMMANDS
#define ONFI_CMD_RESET (0x0000FF00)
#define ONFI_CMD_SYNCH_RESET (0x0000FC00)
#define ONFI_CMD_LUN_RESET (0x0000FA14 + 0x20)
//IDENTIFICATION OPERATIONS
#define ONFI_CMD_READ_ID (0x00009001 + 0x20)
#define ONFI_CMD_READ_PARAMETER_PAGE (0x0000EC02 + 0x20)
#define ONFI_CMD_READ_UNIQUE_ID (0x0000ED02 + 0x20)
//CONFIGURATION OPERATIONS
#define ONFI_CMD_GET_FEATURES (0x0000EE02 + 0x20)
#define ONFI_CMD_SET_FEATURES (0x0000EF03)
//STATUS OPERATIONS
#define ONFI_CMD_READ_STARUS (0x00007004 + 0x20)
#define ONFI_CMD_SELECT_LUN_WITH_STATUS (0x00007805 + 0x20)
#define ONFI_CMD_LUN_STATUS (0x00007105 + 0x20)
#define ONFI_CMD_DEVICE_STATUS (0x00007204 + 0x20)
#define ONFI_CMD_VOLUME_SELECT (0x0000E115)
//COLUMN ADDRESS OPERATIONS
#define ONFI_CMD_CHANGE_READ_COLUMN (0xE0000506 + 0x20)
#define ONFI_CMD_SELECT_CACHE_REGISTER (0xE0000607 + 0x20)
#define ONFI_CMD_CHANGE_WRITE_COLUMN (0x00008508) //10 85 H ?
#define ONFI_CMD_CHANGE_ROW_ADDRESS (0x0011850C)  //error?
//READ OPERATIONS
#define ONFI_CMD_READ_PAGE (0x3000000A + 0x20)
#define ONFI_CMD_READ_PAGE_CACHE (0x0000310B + 0x20)
#define ONFI_CMD_READ_PAGE_CACHE_LAST (0x00003F0B + 0x20)
#define ONFI_CMD_READ_MULTIPLANE (0x00320009 + 0x20)
#define ONFI_CMD_TWO_PLANE_PAGE_READ (0x0030000F + 0x20)
#define ONFI_CMD_QUEUE_PAGE_READ (0x37000716 + 0x20)
//PROGRAM OPERATION
#define ONFI_CMD_PROGRAM_PAGE (0x0010800C)
#define ONFI_CMD_PROGRAM_PAGE_IMD (0x00108017)
#define ONFI_CMD_PROGRAM_PAGE_DEL (0x00138017)
#define ONFI_CMD_PROGRAM_PAGE_1 (0x0000800D)
#define ONFI_CMD_PROGRAM_PAGE_CACHE (0x0015800C)
#define ONFI_CMD_PROGRAM_MULTIPLANE (0x0011800C)
#define ONFI_CMD_WRITE_PAGE (0x00001000)
#define ONFI_CMD_WRITE_PAGE_CACHE (0x00001500)
#define ONFI_CMD_WRITE_MULTIPLANE (0x00001100)
//ERASE OPERATION
#define ONFI_CMD_ERASE_BLOCK (0x00D0600E)
#define ONFI_CMD_ERASE_MULTIPLANE (0x00D1600E)
//COPYBACK OPERATION
#define ONFI_CMD_COPYBACK_READ (0x3500000A + 0x20)
#define ONFI_CMD_COPYBACK_PROGRAM (0x00108509 + 0x20)
#define ONFI_CMD_COPYBACK_PROGRAM_1 (0x0000850D)
#define ONFI_CMD_COPYBACK_MULTIPLANE (0x0011850C)
//OTP OPERATION
#define ONFI_CMD_PROGRAM_OTP (0x0010A00C)
#define ONFI_CMD_DATA_PROTECT_OTP (0x0010A509 + 0x20)
#define ONFI_CMD_READ_PAGE_OTP (0x3000AF0A + 0x20)

/* enable ecc 0x3000 cmd: read: 0x00(CE_en) addr1~5 0x30(CE_dis ready->busy 35us)   -->
 *				(ready) 0x05 ecc_addr1 ecc_addr2 0xE0 (ecc_data0~28  (0xe0))
 *						0x05 addr1_col addr2_col 0xE0 data0~2048
 *						clk = 24MHz use time > 42us + 3.4ms
 * enable ecc 0x8010 cmd: write: 0x80(CE_en) addr_col1 addr2 addr_row3 addr4 addr5 (write enable )data0~2048 --->
 *						0x85 ecc_addr1 ecc_addr2 ecc_data0~28
 *						0x10 (CE_dis [ready->busy->ready ~= 400us]) [write one page]
 *						clk = 24MHz use time > 3ms + 400us.
 * erase : (ready)0x60 addr1 addr2 addr3 0xD0 (busy->ready 3ms)
 */

// TIME_SEQ_0 < all less than 64.
#define TSSC_DEF		(50) //0x32 // > tadl
#define TADL_DEF		(10) //0xa
#define TRHW_DEF		(50)//(100) //0x64
#define TWHR_DEF		(80)  //0x50 //60-80

//TIME_SEQ_1 : all less than 64.
#define TWB_DEF			(63) //(1000) //0x3e8, max 0x7f ? //1us
#define TRR_DEF			(20)	//0x14

//TIMINGS_ASYN : all less than 16
#define TRWH_DEF		(10) //0x0a
#define TRWP_DEF		(12) //0x0c //10-12
//TIMEINGS_SYN : all less than 16
#define TCAD_DEF		(5) //5-20

/* NAND request types */
#define NAND_REQ_CMD 0x0000000000ULL
#define NAND_REQ_ADDR 0x1000000000ULL
#define NAND_REQ_DATAIN 0x2000000000ULL
#define NAND_REQ_DATAOUT 0x3000000000ULL
#define NAND_REQ_WAIT 0x4000000000ULL
#define NAND_REQ_MASK GENMASK_64(38, 36)
#define NAND_REQ_BUS_WIDTH_8 BIT_64(39)

#define PARAM_PAGE_SIZE 256

#define ONFI_REV_21 BIT(3)
#define ONFI_FEAT_BUS_WIDTH_16 BIT(0)
#define ONFI_FEAT_EXTENDED_PARAM BIT(7)


#if 1
/* NAND bus width */
#define NAND_BUS_WIDTH_8		U(0)
#define NAND_BUS_WIDTH_16		U(1)
#else
#define NAND_BUS_WIDTH_8 U(8)
#define NAND_BUS_WIDTH_16 U(16)
#endif

#define MAX_CS 1U

struct bbm_record_line {
	uint16_t src;	// store bad block origin address.
	uint16_t dst;	// store remaped good block address. cat dst => src.
} __packed;
struct bbm_record {
	struct bbm_record_line record_line[8];
} __packed;

enum e_OPTION_CMD {
	CACHE_PROGRAM_EN = 0,
	CACHE_READ_EN,
	GET_SET_FEATURES_EN,
	READ_STATUS_ENHANCED_EN,
	COPYBACK_EN,
	READ_NIQUE_ID_EN,
};

enum e_FEATURES {
	SUPPOTR_DATA_BUS_X16 = 0,
	SUPPORT_MULTI_LUN,
	SUPPORT_NONSEQ_PAGE_WR,
	SUPPORT_INTEREAVED,
	SUPPORT_ODD_TO_EVEN_PAGE_COPYBACK,
};


/*** controller registers ***/
#define DCU_READY (0)
#define DCU_BYSY (1)

#define TWO_LUN (0) //default
#define FOUT_LUN (1)
#define EIGHT_LUN (2)

#define DATA_SEL_FIFO (0)
#define DATA_SEL_DATA_REG (1)

#define INPUT_SIU (0)
#define INPUT_DMA (1)

#define RMP_INIT_BIT (0) //BBM, Remap initial flag. update by software, cleared by hardware.

typedef struct _S_COMMAND {
	uint8_t cmd_seq;
	bool input_sel;
	bool data_sel;
	uint8_t cmd_0;
	uint8_t cmd_1_3;
	uint8_t cmd_2;
} s_command;

enum e_COMMAND {
	CMD_SEQ = 0,
	INPUT_SEL = 6, //0:SIU module  1:DMA module
	DATA_SEL = 7,  //0:FIFO module  1:DATA register
	CMD_0 = 8,
	CMD_1_3 = 16,
	CMD_2 = 24,
};

enum e_WORK_MODE {
	ASYNCHRONOUS = 0,
	SYNCHRONOUS = 1,
	TOOGLE = 2,
};

enum e_control {
	READ_STATUS_EN = 0, //Automatically READ STATUS / check RNB lines. 0:checks RNB lines  1:sends READ S
	ECC_BLOCK_SIZE = 1, //The ECC Block Size. 00:256 bytes 01:512  10:1024  11:not available
	RETRY_EN = 3,		//Retry response enable.
	INT_EN = 4,			//Global Interrupt enables.
	ECC_EN = 5,			//Hardware ECC support enable.
	BLOCK_SIZE_BIT = 6,		//00:32 pages per block.  01:64  10:128  11:256
	DEV_STACK = 11,		//0:Single 16bit device is selected. 1:Two 8-bit devices are selected.
	IO_WIDTH = 12,		//0:8 bits  1:16bits
	BBM_EN = 13,		// bad block management.
	PROT_EN = 14,		//0:protect desabled  1:protect enabled.

	ADDR0_AUTO_INCR = 16, // ADDR0_ROW 0:auto increment disabled.  1:enabled.
	ADDR1_AUTO_INCR = 17, // ADDR1_ROW 0:disabled  1:enabled
	CLN_EN = 18,		  //clear nand enable. 0:disabled  1:enabled

	WORK_MODE = 19,			//00:asynchronous  01:source synchronous  10:Toogle mode  11:not available
	SMALL_BLOCK_EN = 21,	//0:bit block mode enable  1:small block mode enable.
	MLUN_EN = 22,			//0:disabled  1:multi lun mode enabled.
	AUTO_READ_STAT_EN = 23, //0:disabled  1:auto read Status mode enabled.
};

//status_mask same as status.
enum e_status {				 //READ-ONLY
	MEM0_ST = 0, //0:device busy  1:device ready
	MEM1_ST = 1,
	MEM2_ST = 2,
	MEM3_ST = 3,
	MEM4_ST = 4,
	MEM5_ST = 5,
	MEM6_ST = 6,
	MEM7_ST = 7,
	CTRL_STAT = 8,		   //0:controller ready 1:controller busy
	DATASIZE_ERROR_ST = 9, //ECC enable DATA_SIZE reg: 0:correct value 1:incorrect value
	DATA_REG_ST = 10,	   //0:data in DATA_REG is not available  1:is available
	CMD_ID = 16,		   //[16:23] 8BIT,
};

enum e_int_mask {
	PROT_INT_EN = 0,	  //Erase/Write protected area interupt enable. 0:disabled 1:enabled
	CMD_END_INT_EN = 1,	  //Command sequence ended. 0:disabled  1:enabled
	DATA_REG_INT_EN = 2,  //Data in DATA_REG is available. 0:disabled 1:enabled
	DMA_INT_EN = 3,		  //DMA transfer ended. 0:disabled 1:enable
	TRANS_ERR_EN = 4,	  //The transfer on the slave interface error. 0:disabled 1:enabled
	SS_READY_INT_EN = 5,  //The Supper Sequence finished. 0:disabled 1:enabled
	PG_SZ_ERR_INT_EN = 6, //Data Size error occur. 0:disabled 1:enabled
	MEM0_RDY_INT_EN = 8,  //The memory device 0 is ready for the new command. 0:disabled 1:enabled
	MEM1_RDY_INT_EN = 9,  //The device 1 is ready for the new command.
	MEM2_RDY_INT_EN = 10,
	MEM3_RDY_INT_EN = 11,
	MEM4_RDY_INT_EN = 12,
	MEM5_RDY_INT_EN = 13,
	MEM6_RDY_INT_EN = 14,
	MEM7_RDY_INT_EN = 15,
	STAT_ERR_INT0_EN = 16, //PROGRAM PAGE and BLOCK ERASE: 0:disabled 1:enabled
	STAT_ERR_INT1_EN,
	STAT_ERR_INT2_EN,
	STAT_ERR_INT3_EN,
	STAT_ERR_INT4_EN,
	STAT_ERR_INT5_EN,
	STAT_ERR_INT6_EN,
	STAT_ERR_INT7_EN,
	ECC_INT0_EN = 24, //Enables the interrupt from the ECC module status. 0:disabled 1:enabled
	ECC_INT1_EN,
	ECC_INT2_EN,
	ECC_INT3_EN,
	ECC_INT4_EN,
	ECC_INT5_EN,
	ECC_INT6_EN,
	ECC_INT7_EN = 31,
};

enum e_int_status {
	PROT_INT_FL = 0,	  //Erase/Write protected area interupt enable.
	CMD_END_INT_FL = 1,	  //Command sequence ended.
	DATA_REG_FL = 2,	  //Data in DATA_REG is available.
	DMA_INT_FL = 3,		  //DMA transfer ended.
	TRANS_ERR_FL = 4,	  //The transfer on the slave interface error.
	SS_READY_INT_FL = 5,  //The Supper Sequence finished.
	PG_SZ_ERR_INT_FL = 6, //Data Size error occur.
	MEM0_RDY_INT_FL = 8,  //The memory device 0 is ready for the new command.
	MEM1_RDY_INT_FL = 9,  //The device 1 is ready for the new command.
	MEM2_RDY_INT_FL = 10,
	MEM3_RDY_INT_FL = 11,
	MEM4_RDY_INT_FL = 12,
	MEM5_RDY_INT_FL = 13,
	MEM6_RDY_INT_FL = 14,
	MEM7_RDY_INT_FL = 15,
	STAT_ERR_INT0_FL = 16, //PROGRAM PAGE and BLOCK ERASE
	STAT_ERR_INT1_FL,
	STAT_ERR_INT2_FL,
	STAT_ERR_INT3_FL,
	STAT_ERR_INT4_FL,
	STAT_ERR_INT5_FL,
	STAT_ERR_INT6_FL,
	STAT_ERR_INT7_FL,
	ECC_INT0_FL = 24, //Enables the interrupt from the ECC module status.
	ECC_INT1_FL,
	ECC_INT2_FL,
	ECC_INT3_FL,
	ECC_INT4_FL,
	ECC_INT5_FL,
	ECC_INT6_FL,
	ECC_INT7_FL = 31,
};

enum e_time_seq_0 {
	TCCS = 0,  //[0:5] CLKB clock signal. Change column setup
	TADL = 8,  //[8:13] ALE to data loading time for synchronous interface.
			   //ALE to data start time for asynchronous interface.
	TRHW = 16, //[16:21] Data output to command, address, or data input time for synchronous interface.
			   //RE# high to WE# low time for asynchronous interface.
	TWHR = 24, //[24:29] Command cycle to data output time for synchronous interface.
	//WE# high to RE# low time for asynchronous interface.
};

enum e_time_seq_1 {
	TWB = 0, //[0:5] Busy time for interface change.
			 //The busy time when the interface changes from asynchronous to
			 // synchronous using the SET FEATURES(EFh)command,
			 //or from synchronous to asynchronous using the REST(FFh) command.
	TRR = 8, //[8:13] Read high to Read low.
	//TRR time period from rising edge on read/buys intput line to
	// the moment when the read enable signal can be asserted.
};

enum e_timings_asyn {
	TRWP = 0, //[0:3] RE# or WE# pulse width. tRP
	TRWH = 4, //[4:7] RE# or WE# high hold time. tREH
};

enum e_timeings_syn {
	TCAD = 0, //[0:3] Command, Address, Data delay.
};

enum e_fifo_state {
	DF_R_EMPTY = 0, //FIFO empty state bit. - read direction.
	DF_W_FULL = 1,	//FIFO full state bit. - write direction.
	CF_EMPTY = 2,	//Command FIFO empty flag.
	CF_FULL = 3,	//Command FIFO full flag.
	CF_ACCPT_R = 4, //Command FIFO accept flag - read direction.
	CF_ACCPT_W = 5, //Command FIFO accept flag = write direction.
	DF_R_FULL = 6,	//FIFO full state bit. - read direction.
	DF_W_EMPTY = 7, //FIFO empty state bit. - write direction.
};

enum e_MLUN {
	MLUN_IDX = 0, //[0:2]MLUN_IDX LUN address offset.
	LUN_SEL = 8,  //[8:9]LUN number:00:two LUN-es 01:four 10:eight
};

enum e_MEM_CTRL {
	MEM_CE = 0,	 //[0:2]The number of selected memory is binary coded.
	MEM0_WP = 8, // low valid.
	MEM1_WP,
	MEM2_WP,
	MEM3_WP,
	MEM4_WP,
	MEM5_WP,
	MEM6_WP,
	MEM7_WP,
	BANK_SEL = 16, //The number of selected bank is binary coded.
};

enum e_DMA_BURST {
	FOUR_TRANS_INCREMENT = (0),
	SIXTEEN_TRANS_CONSTANT = (1),
	SINGLE_TRANS_INCREMENT = (2),
	UNKNOW_TRANS_INCREMENT = (3),
	EIGHT_TRANS_INCREMENT = (4),
	SIXTEEN_TRANS_INCREMENT = (5),
};

enum e_DMA_MODE {
	DMA_SFRS_MANAGED_MODE = (0),
	DMA_SCATTER_GATHER_MODE = (1),
};

enum e_DMA_CTRL {
	DMA_READY = 0, //ready flag. The flag is set when transfer is completed. read-only
	ERR_FLAG = 1,  //error occurs during the DMA transfer. read-only
	DMA_BURST = 2, //000:four transfers. address increment.
	//001:sixteen transfers stream. address constant.
	//010:single transfer(address increment)
	//011:brust of unspecified length(address incement)
	//100:eight transfers. address increment.
	//101:sixteen transfers. address increment.
	DMA_MODE = 5,  //0:the SFR-s managed mode. 1:the Scatter-Gather mode.
	DMA_START = 7, // DMA start.
};

enum e_DATA_REG_SIZE {
	SINGLE_BYTE = 0,
	TWO_LOWER_BYTES = 1,
	THREE_LOWER_BYTES = 2,
	ALL_FOUR_BYTES = 3,
};

/**
 * max ECC bytes per one command send to device
 *
 * ecc-cap  coorection bytes    max number
 *          per one block       of ecc blocks
 * 4			7					146
 * 8			14					73
 * 16			28					36
 * 24			42					24
 * 32			56					18
 * 48			84					12
 */
enum e_ECC_CAP {
	ECC_CAP_4 = 0,
	ECC_CAP_8,
	ECC_CAP_16,
	ECC_CAP_24,
	ECC_CAP_32,
	ECC_CAP_48,
};

enum e_INTERRUPT_ECC_SEL {
	ECC_SEL_ERROR = 0,
	ECC_SEL_UNC,
	ECC_SEL_OVER,
};

enum e_ECC_CTRL {
	ECC_CAP = 0,	   //correction ability: 000:4  001:8  010:16  011:24  100:32  101:48
	ERR_THRESHOLD = 8, //errors overflow size.
	ECC_SEL = 16,	   //The ECC interupt source select. 00:ECC_ERROR  01:ECC_UNC 1x:ECC_OVER
};

enum e_ECC_STAT {
	ECC_ERROR_0 = 0,
	ECC_ERROR_1,
	ECC_ERROR_2,
	ECC_ERROR_3,
	ECC_ERROR_4,
	ECC_ERROR_5,
	ECC_ERROR_6,
	ECC_ERROR_7,
	ECC_UNC_0 = 8,
	ECC_UNC_1,
	ECC_UNC_2,
	ECC_UNC_3,
	ECC_UNC_4,
	ECC_UNC_5,
	ECC_UNC_6,
	ECC_UNC_7,
	ECC_OVER_0 = 16,
	ECC_OVER_1,
	ECC_OVER_2,
	ECC_OVER_3,
	ECC_OVER_4,
	ECC_OVER_5,
	ECC_OVER_6,
	ECC_OVER_7 = 23,
};

struct nfc_cs_reg {
	uintptr_t data_base;
	uintptr_t cmd_base;
	uintptr_t addr_base;
};

//typedef struct _NFC_Info{
typedef struct nand_config {
	int enabled;		/* 1 to enable, 0 to disable */
	s32 buswidth;		/* bit width, must be 8 */

	void __iomem	*nand_base;

	bool boot_en;
	bool retry_en;
	bool mlun_en;
	bool dma_en;
	bool hw_ecc_en;
	bool sw_ecc_en;
	bool auto_read_stat_en;
	bool check_RB_line;
	bool protect_en;
	bool interrupt_en;
	bool bbm_en;
	bool row_addr_auto_increase;
	bool small_page;

	enum e_WORK_MODE e_work_mode;

	uint8_t ready_dev; //0,1,2...
	bool cmd_complate;
	enum e_DMA_MODE dma_mode;
	enum e_DMA_BURST dma_burst;

	bool data_sel;	//0:FIFO 1:DATA regs
	uint32_t int_status_old;
	uint8_t nand_status; // store 70 or 78 command status.

	struct nand_onfi_params *onfi_params;
	struct nand_data_interface *iface;
	uint32_t good_pool_blk_pos;
	uint8_t bbt_lite[80];	// 80 good blocks(10M range) will scan and mark.
	bool bbt_lite_created;
	bool nfc_inited;

	uint32_t command; // 0x00
	uint32_t control;
	uint32_t status;
	uint32_t status_mask;

	uint32_t int_mask;	// 0x10
	uint32_t int_status; // new and old
	uint32_t ecc_ctrl;
	uint32_t ecc_offset; //must big than DATA_SIZE

	uint32_t ecc_stat;  // 0x20
	uint32_t addr0_col; //[0:15] Column address. A15-A0 address bits.
	uint32_t addr0_row;
	//[0:23] Row address. A39-A16 address bits(Page address, Block address and LUN address in the ONFI case)
	uint32_t addr1_col;

	uint32_t addr1_row; // 0x30
	uint32_t protect;
	//[0:15] PROT_DOWN [16:31]PROT_UP  Block-size precision.
	uint32_t fifo_data;
	//[0:31] FIFO_DATA :an entry point to the FIFO module for the SIU module. works on 32-bit words.
	uint32_t data_reg;  //read-only [0:31] storage of the data that is read in the registered mode.

	uint32_t data_reg_size; // 0x40
	uint32_t dev0_ptr;
	uint32_t dev1_ptr;
	uint32_t dev2_ptr;

	uint32_t dev3_ptr; // 0x50
	uint32_t dev4_ptr;
	uint32_t dev5_ptr;
	uint32_t dev6_ptr;

	uint32_t dev7_ptr; // 0x60
	uint32_t dma_addr_l;
	uint32_t dma_addr_h; // do not use.
	uint32_t dma_cnt;

	uint32_t dma_ctrl; // 0x70
	uint32_t bbm_ctrl;
	uint32_t reserved_1[2];

	uint32_t mem_ctrl; //0x80
	uint32_t data_size;
	uint32_t timings_asyn;
	uint32_t timings_syn;

	uint32_t time_seq_0; //0x90
	uint32_t time_seq_1;
	uint32_t time_gen_seq_0;
	uint32_t time_gen_seq_1;

	uint32_t time_gen_seq_2; //0xA0
	uint32_t reserved_2[3];

	uint32_t fifo_init; //0xb0
	uint32_t fifo_state;
	uint32_t gen_seq_ctrl;
	uint32_t mlun;

	uint32_t dev0_size; //0xc0
	uint32_t dev1_size;
	uint32_t dev2_size;
	uint32_t dev3_size;

	uint32_t dev4_size; //0xd0
	uint32_t dev5_size;
	uint32_t dev6_size;
	uint32_t dev7_size;

	uint32_t ss_ccnt0; //0xe0
	uint32_t ss_ccnt1;
	uint32_t ss_cnt;
	uint32_t ss_addr_dev_ctrl;

	uint32_t ss_cmd0; //0xf0
	uint32_t ss_cmd1;
	uint32_t ss_cmd2;
	uint32_t ss_cmd3;

	uint32_t ss_addr; //0x100
	uint32_t ss_msel;
	uint32_t ss_req;
	uint32_t ss_brk;

	uint32_t reserved_3; //0x110
	uint32_t dma_tlvl;
	uint32_t aes_ctrl;
	uint32_t aes_dataw;

	uint32_t aes_svect; //0x120
	uint32_t cmd_mark;
	uint32_t lun_status_0;
	uint32_t lun_status_1;

	uint32_t timings_toggle; //0x130
	uint32_t time_gen_seq_3;
	uint32_t dqs_delay;
	uint32_t cne_mask;

	uint32_t cne_val; //0x140
	uint32_t cne_ctrl;
	uint32_t internal_status;
	uint32_t reserved_4;

	uint32_t param_reg; //0x150

} s_NFC_Info;

//typedef struct _NFC_Regs{
typedef struct nand_ctlr {
	uint32_t command; // 0x00
	uint32_t control;
	uint32_t status;
	uint32_t status_mask; //[0:7]STATE_MASK  [8:15]ERROR_MASK

	uint32_t int_mask; // 0x10
	uint32_t int_status;
	uint32_t ecc_ctrl;
	uint32_t ecc_offset;

	uint32_t ecc_stat;  // 0x20
	uint32_t addr0_col; //[0:15]page-in number.
	uint32_t addr0_row; //[0:23]lun:2[1bit] + block:2048[11bit] + page:64[6bit]
	uint32_t addr1_col;

	uint32_t addr1_row; // 0x30
	uint32_t protect;
	uint32_t fifo_data;
	uint32_t data_reg;

	uint32_t data_reg_size; // 0x40
	uint32_t dev0_ptr;
	uint32_t dev1_ptr;
	uint32_t dev2_ptr;

	uint32_t dev3_ptr; // 0x50
	uint32_t dev4_ptr;
	uint32_t dev5_ptr;
	uint32_t dev6_ptr;

	uint32_t dev7_ptr; // 0x60
	uint32_t dma_addr_l;
	uint32_t dma_addr_h;
	uint32_t dma_cnt;

	uint32_t dma_ctrl; // 0x70
	uint32_t bbm_ctrl;
	uint32_t reserved_1[2];

	uint32_t mem_ctrl; //0x80
	uint32_t data_size;
	uint32_t timings_asyn;
	uint32_t timings_syn;

	uint32_t time_seq_0; //0x90
	uint32_t time_seq_1;
	uint32_t time_gen_seq_0;
	uint32_t time_gen_seq_1;

	uint32_t time_gen_seq_2; //0xA0
	uint32_t reserved_2[3];

	uint32_t fifo_init; //0xb0
	uint32_t fifo_state;
	uint32_t gen_seq_ctrl;
	uint32_t mlun; //[0:2]MLUN_IDX LUN address offset. [8:9]LUN number:00:two LUN-es 01:four 10:eight

	uint32_t dev0_size; //0xc0
	uint32_t dev1_size;
	uint32_t dev2_size;
	uint32_t dev3_size;

	uint32_t dev4_size; //0xd0
	uint32_t dev5_size;
	uint32_t dev6_size;
	uint32_t dev7_size;

	uint32_t ss_ccnt0; //0xe0
	uint32_t ss_ccnt1;
	uint32_t ss_cnt;
	uint32_t ss_addr_dev_ctrl;

	uint32_t ss_cmd0; //0xf0
	uint32_t ss_cmd1;
	uint32_t ss_cmd2;
	uint32_t ss_cmd3;

	uint32_t ss_addr; //0x100
	uint32_t ss_msel;
	uint32_t ss_req;
	uint32_t ss_brk;

	uint32_t reserved_3; //0x110
	uint32_t dma_tlvl;
	uint32_t aes_ctrl;	//not used.
	uint32_t aes_dataw;

	uint32_t aes_svect; //0x120
	uint32_t cmd_mark;
	uint32_t lun_status_0;
	uint32_t lun_status_1;

	uint32_t timings_toggle; //0x130
	uint32_t time_gen_seq_3;
	uint32_t dqs_delay;
	uint32_t cne_mask;

	uint32_t cne_val; //0x140
	uint32_t cne_ctrl;
	uint32_t internal_status;
	uint32_t reserved_4;

	uint32_t param_reg; //0x150

} s_NFC_Regs;

/* Information about an attached NAND chip */
struct nand_drv {
	struct nand_ctlr __iomem *regs;	/* Register Interface */
	struct nand_config config;

	unsigned int		last_cmd;
	uint16_t			col_addr;
	uint32_t			page_addr;
	int					spare_only;
	bool				status_request;
	unsigned int		last_byte; //alread read bytes
	bool				ecc_en;
	bool				flush_fifo;
	bool				new_cmd;
	uint64_t			clk_rate;	/* bus interface clock rate */
	void *buf;

#if 0
	u32 fifo_index;
	struct dma_global *dma_glb;
	struct dma_ssp *dma_nand;
	struct tx_descriptor_t *tx_desc;
	struct rx_descriptor_t *rx_desc;
#endif
};

struct ts_nand_info {
#if defined(__UBOOT__)
	struct udevice *dev;
#endif
	struct nand_drv nand_ctrl;
	struct nand_chip nand_chip;
	uint8_t		raw_oob_mode;
	unsigned int revision;
	unsigned int max_ecc_strength_supported;
	int		cur_chip;

#if defined(__KERNEL__)
	struct device *dev;

	/* elements used by ISR */
	struct completion complete;
	spinlock_t irq_lock;
	uint32_t irq_mask;
	uint32_t irq_status;
	int irq;

	dma_addr_t dma_addr;
	int dma_avail;
	int oob_skip_bytes;
#endif
};

#if defined(__LINUX__)
static inline void nand_set_flash_node(struct nand_chip *chip,
				       struct device_node *np)
{
	mtd_set_of_node(&chip->mtd, np);
}

static inline struct device_node *nand_get_flash_node(struct nand_chip *chip)
{
	return mtd_get_of_node(&chip->mtd);
}

static inline struct nand_chip *mtd_to_nand(struct mtd_info *mtd)
{
	return container_of(mtd, struct nand_chip, mtd);
}

static inline struct mtd_info *nand_to_mtd(struct nand_chip *chip)
{
	return &chip->mtd;
}

static inline void *nand_get_controller_data(struct nand_chip *chip)
{
	return chip->priv;
}

static inline void nand_set_controller_data(struct nand_chip *chip, void *priv)
{
	chip->priv = priv;
}
#endif

int ts_mtd_read_page(struct mtd_info *mtd, unsigned int page, uintptr_t buffer);
void nand_check_offset_of_block_is_bad(struct mtd_info *mtd, uint64_t *offset);
int nfc_check_ecc_state(void);
int nand_device_reset(struct mtd_info *mtd, uint8_t dev_index);
int nand_write_oob_page_cmd(struct mtd_info *mtd, unsigned int page, unsigned int offset,
					   uintptr_t buffer, unsigned int len);
int nand_read_oob_page_cmd(struct mtd_info *mtd, unsigned int page, unsigned int col_offset,
					   uintptr_t buffer, unsigned int len);
int read_check(uint32_t *addr, uint32_t exp_data);
int nand_scan_bbt(struct mtd_info *mtd, uint32_t block);
int nfc_exec_onfi_command(uint32_t onfiCmd);
void nfc_bbm_flush_remap_flag(void);
int nand_change_read_column_cmd(struct mtd_info *mtd,
	unsigned int offset, uintptr_t buffer, unsigned int len);

int ts_raw_nand_init(struct mtd_info *mtd, unsigned long long *size, uint32_t *erase_size);
int ts_raw_nand_read(struct mtd_info *mtd, uint32_t offset, const uintptr_t buffer, size_t length, size_t *outlength);
int ts_raw_nand_write(struct mtd_info *mtd, uint32_t offset, uintptr_t buffer, size_t length);

int ts_nand_init_ctrl(struct ts_nand_info *ts_nand);
void ts_nand_remove(struct ts_nand_info *ts_nand);
#endif /* TMS_TX536_NAND_H */
