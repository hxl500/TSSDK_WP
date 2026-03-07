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
#if defined(__UBOOT__)
#include <errno.h>
//#include <platform_def.h>
//#include <common/debug.h>
//#include <lib/mmio.h>
//#include <drivers/delay_timer.h>
//#include <ts_hw.h>
//#include <drivers/tsingmicro/mtd/ts_nand_raw.h>
//#include <ts_system.h>
#include <common.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/sizes.h>
#include <log.h>
#include <asm/io.h>
#include <memalign.h>
#include <nand.h>
#include <dm/device_compat.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include <linux/iopoll.h>
#include <linux/errno.h>
#include <linux/mtd/rawnand.h>
#include <asm/gpio.h>
#include <fdtdec.h>
#include <bouncebuf.h>
#include <dm.h>
#include "ts_nand.h"
#include <dm/device.h>
#define __maybe_unused                  __attribute__((__unused__))
#else
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/mtd/mtd.h>
#include <linux/module.h>
#include <linux/slab.h>
//#include <assert.h>
#include "ts_nand.h"
//#include <stdio.h>
MODULE_LICENSE("GPL");
#define TS_NAND_NAME    "ts-nand"
#endif

s_command nfcCmd;
s_NFC_Info *ts_nfc;

#ifdef __ATF__
static s_NFC_Regs *nfc_regs = (s_NFC_Regs *)(uintptr_t)TS_NFCHOST_BASE;
#else
static s_NFC_Regs *nfc_regs; //(s_NFC_Regs *)(uintptr_t)NFC_BASE;
#endif
static unsigned int *pread, *pwrite;
#if defined(__UBOOT__)
static struct nand_ecclayout eccoob;
#else
static struct nand_ecclayout_user eccoob;
#endif

#define ONFI_SIGNATURE_ADDR					0x20U

#define ONFI_FETURE_TIMING_MODE_ADDR		0x01U
#define ONFI_FETURE_OUT_DRV_STRENGTH_ADDR	0x80U
#define ONFI_FETURE_RB_STRENGTH_ADDR		0x81U
#define ONFI_FETURE_ARRAY_OP_MODE_ADDR		0x90U

/* CRC calculation */
#define CRC_POLYNOM 0x8005U
#define CRC_INIT_VALUE 0x4F4EU

/* Status register */
//#define NAND_STATUS_WP				BIT(7)
//#define NAND_STATUS_READY			BIT(6)
#define NAND_STATUS_ARRAY_READY		NAND_STATUS_TRUE_READY /*internal data operation*/
#define NAND_STATUS_CACHE_LAST_FAIL	NAND_STATUS_FAIL_N1//BIT(1)
//#define NAND_STATUS_FAIL			BIT(0)
#define NAND_READY (NAND_STATUS_READY | NAND_STATUS_ARRAY_READY)
/* Timeout for device interface reset */
#define TIMEOUT_US_1_MS 1000U
#define TIMEOUT_US_1_S (1000 * 1000U)

/* unit us */
//data transfer from cell to register
#define TIME_TR (25)
//program time 300-600
#define TIME_TPROG (300)
//read cache busy time 5-25
#define TIME_TCBSYR (10)
//cache program short busy time 5-600
#define TIME_TCBSYW (5)
//two plane program/erase page
#define TIME_DBSY (1)
//busy time for SET/GET FEATURES
#define TIME_TFEAT (1)
//dummy busy time 0.5-1
#define TIME_TDBSY (1)
//block erase time 3-10ms
#define TIME_TBERS (3 * 1000)
//device resetting time read/program/erase:10/20/500
#define TIME_TRST (500)
/* tWHR 80ns~ */
#define TIME_TWHR (1)


#define TS_BBM_LEN 2U
//#define TS_MAX_ECC_BYTES 14U //bch8 is 13bytes,
#define TIMEOUT_US_10_MS 10000U
/* Timings */
#define TS_TIO 8000U
#define TS_TSYNC 3000U
#define TS_PCR_TIMING_MASK GENMASK_32(3, 0)
#define TS_PMEM_PATT_TIMING_MASK GENMASK_32(7, 0)
#define TS_PSEC_PER_MSEC (1000UL * 1000UL * 1000UL)
#define TS_THIZ 0x01U

enum nfc_ecc {
	TS_ECC_HAM = 1U, //tx536 don't has this function.
	TS_ECC_BCH2 = 2U,		//
	TS_ECC_BCH4 = 4U,		//7bytes
	TS_ECC_BCH8 = 8U,		//13-14bytes
	TS_ECC_BCH16 = 16U,	//26-28bytes, don't use.
	TS_ECC_BCH24 = 24U,
	TS_ECC_BCH32 = 32U,	//52-56bytes, dont't use.
	TS_ECC_BCH48 = 48U		//84bytes
};

#ifdef CONFIG_SYS_NAND_USE_FLASH_BBT
static u8 bbt_pattern[] = {'T', 'S', 'M', 'B', 'b', 't', '0' };
static u8 bbt_mirror_pattern[] = {'1', 't', 'b', 'B', 'M', 'S', 'T' };

static struct nand_bbt_descr bbt_main_descr = {
	.options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
		| NAND_BBT_2BIT | NAND_BBT_VERSION,
	.offs =	8,
	.len = 6,
	.veroffs = 14,
	.maxblocks = 8,		/* Last 8 blocks in each chip */
	.pattern = bbt_pattern
};

static struct nand_bbt_descr bbt_mirror_descr = {
	.options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE
		| NAND_BBT_2BIT | NAND_BBT_VERSION,
	.offs =	8,
	.len = 6,
	.veroffs = 14,
	.maxblocks = 8,		/* Last 8 blocks in each chip */
	.pattern = bbt_mirror_pattern
};
#endif

#if defined(__KERNEL__)
#define assert(expr) \
do { \
	if (unlikely(!(expr))) { \
		printk(KERN_ERR "Assertion failed! %s,%s,%s,line=%d\n", \
		#expr, __FILE__, __func__, __LINE__); \
	} \
} while (0)
#endif

static struct nand_ecclayout __maybe_unused ecc_layout_2KB_bch4bit = {
	.eccbytes = 32,
	.eccpos = {
		32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = { {2, 30} }
};

static struct nand_ecclayout __maybe_unused ecc_layout_2KB_bch8bit = {
	.eccbytes = 64,
	.eccpos = {
		32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63,
		64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 87,
		88, 89, 90, 91, 92, 93, 94, 95},
	.oobfree = { {1, 4}, {6, 26} }
};

static struct nand_ecclayout __maybe_unused ecc_layout_4KB_bch4bit = {
	.eccbytes = 64,
	.eccpos = {
		32,  33,  34,  35,  36,  37,  38,  39,
		40,  41,  42,  43,  44,  45,  46,  47,
		48,  49,  50,  51,  52,  53,  54,  55,
		56,  57,  58,  59,  60,  61,  62,  63,
		96,  97,  98,  99,  100, 101, 102, 103,
		104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127},
	/* Bootrom looks in bytes 0 & 5 for bad blocks */
	.oobfree = { {6, 26}, { 64, 32} }
};

static struct nand_ecclayout __maybe_unused ecc_layout_8KB_bch4bit = {
	.eccbytes = 128,
	.eccpos = {
		32,  33,  34,  35,  36,  37,  38,  39,
		40,  41,  42,  43,  44,  45,  46,  47,
		48,  49,  50,  51,  52,  53,  54,  55,
		56,  57,  58,  59,  60,  61,  62,  63,

		96,  97,  98,  99,  100, 101, 102, 103,
		104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127,

		160, 161, 162, 163, 164, 165, 166, 167,
		168, 169, 170, 171, 172, 173, 174, 175,
		176, 177, 178, 179, 180, 181, 182, 183,
		184, 185, 186, 187, 188, 189, 190, 191,

		224, 225, 226, 227, 228, 229, 230, 231,
		232, 233, 234, 235, 236, 237, 238, 239,
		240, 241, 242, 243, 244, 245, 246, 247,
		248, 249, 250, 251, 252, 253, 254, 255},

	/* Bootrom looks in bytes 0 & 5 for bad blocks */
	.oobfree = { {1, 4}, {6, 26}, { 64, 32}, {128, 32}, {192, 32} }
};

static struct nand_ecclayout __maybe_unused ecc_layout_4KB_bch8bit = {
	.eccbytes = 128,
	.eccpos = {
		32,  33,  34,  35,  36,  37,  38,  39,
		40,  41,  42,  43,  44,  45,  46,  47,
		48,  49,  50,  51,  52,  53,  54,  55,
		56,  57,  58,  59,  60,  61,  62,  63},
	.oobfree = { }
};

static struct nand_ecclayout __maybe_unused ecc_layout_8KB_bch8bit = {
	.eccbytes = 256,
	.eccpos = {},
	/* HW ECC handles all ECC data and all spare area is free for OOB */
	.oobfree = {{0, 160} }
};

//#define U16_MAX		((uint16_t)~0U)
#define ONFI_DYN_TIMING_MAX U16_MAX

/*
 * For non-ONFI chips we use the highest possible value for tPROG and tBERS.
 * tR and tCCS will take the default values precised in the ONFI specification
 * for timing mode 0, respectively 200us and 500ns.
 *
 * These four values are tweaked to be more accurate in the case of ONFI chips.
 */
static /*const*/ struct nand_data_interface onfi_sdr_timings[] = {
	/* Mode 0 */
	{
		.type = NAND_SDR_IFACE,
		//.timings.mode = 0,
/* NAND ONFI default value mode 0 in picosecond */
#define NAND_TADL_MIN 400000UL	//400ns
#define NAND_TALH_MIN 20000UL
#define NAND_TALS_MIN 50000UL
#define NAND_TAR_MIN 25000UL
#define NAND_TCCS_MIN 500000UL	//500ns
#define NAND_TCEA_MIN 100000UL
#define NAND_TCEH_MIN 20000UL
#define NAND_TCH_MIN 20000UL
#define NAND_TCHZ_MAX 100000UL
#define NAND_TCLH_MIN 20000UL
#define NAND_TCLR_MIN 20000UL
#define NAND_TCLS_MIN 50000UL
#define NAND_TCOH_MIN 0UL
#define NAND_TCS_MIN 70000UL
#define NAND_TDH_MIN 20000UL
#define NAND_TDS_MIN 40000UL
#define NAND_TFEAT_MAX 1000000UL
#define NAND_TIR_MIN 10000UL
#define NAND_TITC_MIN 1000000UL
#define NAND_TR_MAX 200000000UL
#define NAND_TRC_MIN 100000UL
#define NAND_TREA_MAX 40000UL	//40ns
#define NAND_TREH_MIN 30000UL	//30ns
#define NAND_TRHOH_MIN 0UL
#define NAND_TRHW_MIN 200000UL	//200ns
#define NAND_TRHZ_MAX 200000UL
#define NAND_TRLOH_MIN 0UL
#define NAND_TRP_MIN 50000UL	//50ns
#define NAND_TRR_MIN 40000UL	//40ns
#define NAND_TRST_MAX 250000000000ULL
#define NAND_TWB_MAX 200000UL	//200ns
#define NAND_TWC_MIN 100000UL
#define NAND_TWH_MIN 30000UL
#define NAND_TWHR_MIN 120000UL //120ns
#define NAND_TWP_MIN 50000UL
#define NAND_TWW_MIN 100000UL
		.timings.sdr = {
			.tCCS_min = 500000,
			.tR_max = 200000000,
			.tPROG_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tBERS_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tADL_min = 400000,
			.tALH_min = 20000,
			.tALS_min = 50000,
			.tAR_min = 25000,
			.tCEA_max = 100000,
			.tCEH_min = 20000,
			.tCH_min = 20000,
			.tCHZ_max = 100000,
			.tCLH_min = 20000,
			.tCLR_min = 20000,
			.tCLS_min = 50000,
			.tCOH_min = 0,
			.tCS_min = 70000,
			.tDH_min = 20000,
			.tDS_min = 40000,
			.tFEAT_max = 1000000,
			.tIR_min = 10000,
			.tITC_max = 1000000,
			.tRC_min = 100000,
			.tREA_max = 40000,
			.tREH_min = 30000,
			.tRHOH_min = 0,
			.tRHW_min = 200000,
			.tRHZ_max = 200000,
			.tRLOH_min = 0,
			.tRP_min = 50000,
			.tRR_min = 40000,
			.tRST_max = 250000000000ULL,
			.tWB_max = 200000,
			.tWC_min = 100000,
			.tWH_min = 30000,
			.tWHR_min = 120000,
			.tWP_min = 50000,
			.tWW_min = 100000,
		},
	},
	/* Mode 1 */
	{
		.type = NAND_SDR_IFACE,
		//.timings.mode = 1,
		.timings.sdr = {
			.tCCS_min = 500000,
			.tR_max = 200000000,
			.tPROG_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tBERS_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tADL_min = 400000,
			.tALH_min = 10000,
			.tALS_min = 25000,
			.tAR_min = 10000,
			.tCEA_max = 45000,
			.tCEH_min = 20000,
			.tCH_min = 10000,
			.tCHZ_max = 50000,
			.tCLH_min = 10000,
			.tCLR_min = 10000,
			.tCLS_min = 25000,
			.tCOH_min = 15000,
			.tCS_min = 35000,
			.tDH_min = 10000,
			.tDS_min = 20000,
			.tFEAT_max = 1000000,
			.tIR_min = 0,
			.tITC_max = 1000000,
			.tRC_min = 50000,
			.tREA_max = 30000,
			.tREH_min = 15000,
			.tRHOH_min = 15000,
			.tRHW_min = 100000,
			.tRHZ_max = 100000,
			.tRLOH_min = 0,
			.tRP_min = 25000,
			.tRR_min = 20000,
			.tRST_max = 500000000,
			.tWB_max = 100000,
			.tWC_min = 45000,
			.tWH_min = 15000,
			.tWHR_min = 80000,
			.tWP_min = 25000,
			.tWW_min = 100000,
		},
	},
	/* Mode 2 */
	{
		.type = NAND_SDR_IFACE,
		//.timings.mode = 2,
		.timings.sdr = {
			.tCCS_min = 500000,
			.tR_max = 200000000,
			.tPROG_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tBERS_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tADL_min = 400000,
			.tALH_min = 10000,
			.tALS_min = 15000,
			.tAR_min = 10000,
			.tCEA_max = 30000,
			.tCEH_min = 20000,
			.tCH_min = 10000,
			.tCHZ_max = 50000,
			.tCLH_min = 10000,
			.tCLR_min = 10000,
			.tCLS_min = 15000,
			.tCOH_min = 15000,
			.tCS_min = 25000,
			.tDH_min = 5000,
			.tDS_min = 15000,
			.tFEAT_max = 1000000,
			.tIR_min = 0,
			.tITC_max = 1000000,
			.tRC_min = 35000,
			.tREA_max = 25000,
			.tREH_min = 15000,
			.tRHOH_min = 15000,
			.tRHW_min = 100000,
			.tRHZ_max = 100000,
			.tRLOH_min = 0,
			.tRR_min = 20000,
			.tRST_max = 500000000,
			.tWB_max = 100000,
			.tRP_min = 17000,
			.tWC_min = 35000,
			.tWH_min = 15000,
			.tWHR_min = 80000,
			.tWP_min = 17000,
			.tWW_min = 100000,
		},
	},
	/* Mode 3 */
	{
		.type = NAND_SDR_IFACE,
		//.timings.mode = 3,
		.timings.sdr = {
			.tCCS_min = 500000,
			.tR_max = 200000000,
			.tPROG_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tBERS_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tADL_min = 400000,
			.tALH_min = 5000,
			.tALS_min = 10000,
			.tAR_min = 10000,
			.tCEA_max = 25000,
			.tCEH_min = 20000,
			.tCH_min = 5000,
			.tCHZ_max = 50000,
			.tCLH_min = 5000,
			.tCLR_min = 10000,
			.tCLS_min = 10000,
			.tCOH_min = 15000,
			.tCS_min = 25000,
			.tDH_min = 5000,
			.tDS_min = 10000,
			.tFEAT_max = 1000000,
			.tIR_min = 0,
			.tITC_max = 1000000,
			.tRC_min = 30000,
			.tREA_max = 20000,
			.tREH_min = 10000,
			.tRHOH_min = 15000,
			.tRHW_min = 100000,
			.tRHZ_max = 100000,
			.tRLOH_min = 0,
			.tRP_min = 15000,
			.tRR_min = 20000,
			.tRST_max = 500000000,
			.tWB_max = 100000,
			.tWC_min = 30000,
			.tWH_min = 10000,
			.tWHR_min = 80000,
			.tWP_min = 15000,
			.tWW_min = 100000,
		},
	},
	/* Mode 4 */
	{
		.type = NAND_SDR_IFACE,
		//.timings.mode = 4,
		.timings.sdr = {
			.tCCS_min = 500000,
			.tR_max = 200000000,
			.tPROG_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tBERS_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tADL_min = 400000,
			.tALH_min = 5000,
			.tALS_min = 10000,
			.tAR_min = 10000,
			.tCEA_max = 25000,
			.tCEH_min = 20000,
			.tCH_min = 5000,
			.tCHZ_max = 30000,
			.tCLH_min = 5000,
			.tCLR_min = 10000,
			.tCLS_min = 10000,
			.tCOH_min = 15000,
			.tCS_min = 20000,
			.tDH_min = 5000,
			.tDS_min = 10000,
			.tFEAT_max = 1000000,
			.tIR_min = 0,
			.tITC_max = 1000000,
			.tRC_min = 25000,
			.tREA_max = 20000,
			.tREH_min = 10000,
			.tRHOH_min = 15000,
			.tRHW_min = 100000,
			.tRHZ_max = 100000,
			.tRLOH_min = 5000,
			.tRP_min = 12000,
			.tRR_min = 20000,
			.tRST_max = 500000000,
			.tWB_max = 100000,
			.tWC_min = 25000,
			.tWH_min = 10000,
			.tWHR_min = 80000,
			.tWP_min = 12000,
			.tWW_min = 100000,
		},
	},
	/* Mode 5 */
	{
		.type = NAND_SDR_IFACE,
		//.timings.mode = 5,
/* NAND ONFI default value mode 5 in picosecond */
#define NAND_5_TADL_MIN  70000UL	//70ns
#define NAND_5_TALH_MIN   5000UL
#define NAND_5_TALS_MIN  10000UL
#define NAND_5_TAR_MIN   10000UL
#define NAND_5_TCCS_MIN 100000UL	//100ns
#define NAND_5_TCEA_MAX  25000UL
#define NAND_5_TCEH_MIN  20000UL
#define NAND_5_TCH_MIN    5000UL
#define NAND_5_TCHZ_MAX  30000UL
#define NAND_5_TCLH_MIN   5000UL
#define NAND_5_TCLR_MIN  10000UL
#define NAND_5_TCLS_MIN  10000UL
#define NAND_5_TCOH_MIN  15000UL
#define NAND_5_TCS_MIN   15000UL
#define NAND_5_TDH_MIN    5000UL
#define NAND_5_TDS_MIN    7000UL
#define NAND_5_TFEAT_MAX 1000000UL
#define NAND_5_TIR_MIN      0UL
#define NAND_5_TITC_MIN 1000000UL
#define NAND_5_TR_MAX 100000000UL
#define NAND_5_TRC_MIN   20000UL
#define NAND_5_TREA_MAX  16000UL	//16ns
#define NAND_5_TREH_MIN   7000UL	//7ns
#define NAND_5_TRHOH_MIN     0UL
#define NAND_5_TRHW_MIN 100000UL	//100ns
#define NAND_5_TRHZ_MAX 100000UL
#define NAND_5_TRLOH_MIN  5000UL
#define NAND_5_TRP_MIN   10000UL	//10ns
#define NAND_5_TRR_MIN   20000UL	//20ns
#define NAND_5_TRST_MAX 500000000ULL //500us
#define NAND_5_TWB_MAX  100000UL	//100ns
#define NAND_5_TWC_MIN   20000UL
#define NAND_5_TWH_MIN    7000UL
#define NAND_5_TWHR_MIN  80000UL //80ns
#define NAND_5_TWP_MIN   10000UL
#define NAND_5_TWW_MIN  100000UL
		.timings.sdr = {
			.tCCS_min = NAND_5_TCCS_MIN,
			.tR_max = 200000000,
			.tPROG_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tBERS_max = 1000000ULL * ONFI_DYN_TIMING_MAX,
			.tADL_min = NAND_5_TADL_MIN,
			.tALH_min = NAND_5_TALH_MIN,
			.tALS_min = NAND_5_TALS_MIN,
			.tAR_min = NAND_5_TAR_MIN,
			.tCEA_max = NAND_5_TCEA_MAX,
			.tCEH_min = NAND_5_TCEH_MIN,
			.tCH_min = NAND_5_TCH_MIN,
			.tCHZ_max = NAND_5_TCHZ_MAX,
			.tCLH_min = NAND_5_TCLH_MIN,
			.tCLR_min = NAND_5_TCLR_MIN,
			.tCLS_min = NAND_5_TCLS_MIN,
			.tCOH_min = NAND_5_TCOH_MIN,
			.tCS_min = NAND_5_TCS_MIN,
			.tDH_min = NAND_5_TDH_MIN,
			.tDS_min = NAND_5_TDS_MIN,
			.tFEAT_max = NAND_5_TFEAT_MAX,
			.tIR_min = NAND_5_TIR_MIN,
			.tITC_max = NAND_5_TITC_MIN,
			.tRC_min = NAND_5_TRC_MIN,
			.tREA_max = NAND_5_TREA_MAX,
			.tREH_min = NAND_5_TREH_MIN,
			.tRHOH_min = NAND_5_TRHOH_MIN,
			.tRHW_min = NAND_5_TRHW_MIN,
			.tRHZ_max = NAND_5_TRHZ_MAX,
			.tRLOH_min = NAND_5_TRLOH_MIN,
			.tRP_min = NAND_5_TRP_MIN,
			.tRR_min = NAND_5_TRR_MIN,
			.tRST_max = NAND_5_TRST_MAX,
			.tWB_max = NAND_5_TWB_MAX,
			.tWC_min = NAND_5_TWC_MIN,
			.tWH_min = NAND_5_TWH_MIN,
			.tWHR_min = NAND_5_TWHR_MIN,
			.tWP_min = NAND_5_TWP_MIN,
			.tWW_min = NAND_5_TWW_MIN,
		},
	},
};

static void raw_nand_read_page_data(struct mtd_info *mtd,
		uint32_t drop_bytes, uintptr_t buff, unsigned int length, bool use_bus8);
static int nand_read_data(struct mtd_info *mtd, uintptr_t data, unsigned int length, bool use_8bit);
static int detect_onfi(struct mtd_info *mtd);
static int nand_mtd_block_is_bad(struct mtd_info *mtd, unsigned int block);
static void nfc_set_ecc(bool enable);
static int nand_read_id_0(struct mtd_info *mtd);

static int ts_nand_read_page_raw(struct mtd_info *mtd,
			      struct nand_chip *chip, uint8_t *buf,
			      int oob_required, int page);
static int ts_nand_write_page_raw(struct mtd_info *mtd,
			       struct nand_chip *chip, const uint8_t *buf,
			       int oob_required, int page);

static inline uint32_t mmio_read_32(uintptr_t addr)
{
	//uint32_t tmp;

	//tmp = *(volatile uint32_t*)addr;

	//return tmp;
	return readl((uint32_t *)addr);
}

#define mmio_read_16	readw
#define mmio_read_8		readb
static inline void mmio_write_32(uintptr_t addr, uint32_t value)
{
	// *(volatile uint32_t*)addr = value;
	writel(value, (unsigned char *)addr);
}
#define mmio_write_16(addr, val)	writew(val, (unsigned short *)addr)
#define mmio_write_8(addr, val)		writeb(val, (unsigned char *)addr)

#define mmio_clrsetbits_32(addr, clear, set) \
		mmio_write_32(addr, (mmio_read_32(addr) & ~(clear)) | (set))

#if defined(__VALIDATION__) && 0

#define VERBOSE qw_printf
#define INFO	qw_printf
#define WARN	qw_printf
#define ERROR	qw_printf
#define printf	qw_printf

#define GENMASK_32(h, l) \
	(((0xFFFFFFFF) << (l)) & (0xFFFFFFFF >> (32 - 1 - (h))))
#define udelay(x)	delay(x)
#endif
#define GENMASK_32				GENMASK
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) < (y) ? (y) : (x))

#define TS_NFC_MAX_CLK (240000000)
uint64_t ts_clk_get_rate(void)
{
#if 0
#ifdef IMAGE_BL1
	return TS_NFC_MAX_CLK;
#else
	uint32_t val, div;
	uintptr_t reg;
	uint64_t freq = TS_NFC_MAX_CLK;
//TODO...
	uint64_t cpll = TS_NFC_MAX_CLK; //update after...

	reg = TS_CLK_GEN_BASE + TS_CLK_GEN_NFC_CLKB_OFF;
	val = mmio_read_32(reg);
	if ((val & 0x1) == NFC_CLKB_SEL_OSC24) {
		freq = TS_NFC_MAX_CLK;
	} else {
		div = (val >> 4) & 0x07;
		freq = div_round_up(cpll, div);
	}
	return freq;
#endif
#else
	return TS_NFC_MAX_CLK;
#endif
}

uint32_t nfc_get_interrupt_status(void)
{
	return mmio_read_32((uintptr_t)&nfc_regs->int_status);
}

uint32_t nfc_clear_interrupts(enum e_int_status unset_int)
{
	uint32_t mask = mmio_read_32((uintptr_t)&nfc_regs->int_mask);

	RESETBIT(mask, unset_int);
	mmio_write_32((uintptr_t)&nfc_regs->int_mask, mask);

	return 0;
}

void __maybe_unused nfc_irqhandler(int32_t idx)
{
	// TODO
	pr_debug("*********** interrupt nfc ***********\r\n");

	uint32_t status = ts_nfc->int_status = nfc_get_interrupt_status();

	if (GETBIT(status, PROT_INT_FL))
		pr_debug("intr: Erase/Write protected area interupt enable.\r\n");

	if (GETBIT(status, CMD_END_INT_FL)) {
		pr_debug("intr:Transfer Command sequence ended.\r\n");
		SETBIT(ts_nfc->int_status_old, CMD_END_INT_FL);
		ts_nfc->cmd_complate = 1;
	}
	if (GETBIT(status, DATA_REG_FL))
		pr_debug("intr: Data in DATA_REG is available.\r\n");


	if (GETBIT(status, DMA_INT_FL)) {
		pr_debug("intr: DMA transfer ended.\r\n");
		ts_nfc->cmd_complate = 1;
	}

	if (GETBIT(status, TRANS_ERR_FL))
		pr_debug("intr: The transfer on the slave interface error.\r\n");

	if (GETBIT(status, SS_READY_INT_FL))
		pr_debug("intr: The Supper Sequence finished.\r\n");

	if (GETBIT(status, PG_SZ_ERR_INT_FL))
		pr_debug("intr: Data Size error occur.\r\n");

	if (GETBIT(status, MEM0_RDY_INT_FL)) {
		pr_debug("intr: The memory device 0 is ready for the new command.\r\n");
		//nfc_config.cmd_complate = 1;
	}

	if (GETBIT(status, MEM1_RDY_INT_FL)) {
		pr_debug("intr: The device 1 is ready for the new command..\r\n");
		//nfc_config.cmd_complate = 1;
	}
	if (GETBIT(status, MEM2_RDY_INT_FL))
		pr_debug("intr: The device 2 is ready for the new command..\r\n");

	if (GETBIT(status, MEM3_RDY_INT_FL))
		pr_debug("intr: The device 3 is ready for the new command..\r\n");

	if (GETBIT(status, MEM4_RDY_INT_FL))
		pr_debug("intr: The device 4 is ready for the new command..\r\n");


	if (GETBIT(status, MEM5_RDY_INT_FL))
		pr_debug("intr: The device 5 is ready for the new command..\r\n");

	if (GETBIT(status, MEM6_RDY_INT_FL))
		pr_debug("intr: The device 6 is ready for the new command..\r\n");

	if (GETBIT(status, MEM7_RDY_INT_FL))
		pr_debug("intr: The device 7 is ready for the new command..\r\n");

	if (GETBIT(status, STAT_ERR_INT0_FL)) {
		pr_debug("intr: PROGRAM PAGE and BLOCK ERASE operations, most recently operation on device 0 failed.\r\n");
		//repeat transfer ?
#if 0
		if ((ts_nfc->command & 0x6000) || (ts_nfc->command & 0x8000))
			nfc_exec_onfi_command(ts_nfc->command);

#endif
	}

	if (GETBIT(status, STAT_ERR_INT1_FL))
		pr_debug("intr: PROGRAM PAGE and BLOCK ERASE operations, most recently operation on device 1 failed..\r\n");

	if (GETBIT(status, STAT_ERR_INT2_FL))
		pr_debug("intr: PROGRAM PAGE and BLOCK ERASE operations, most recently operation on device 2 failed..\r\n");

	if (GETBIT(status, STAT_ERR_INT3_FL))
		pr_debug("intr: PROGRAM PAGE and BLOCK ERASE operations, most recently operation on device 3 failed..\r\n");

	if (GETBIT(status, STAT_ERR_INT4_FL))
		pr_debug("intr: PROGRAM PAGE and BLOCK ERASE operations, most recently operation on device 4 failed..\r\n");


	if (GETBIT(status, STAT_ERR_INT5_FL))
		pr_debug("intr: PROGRAM PAGE and BLOCK ERASE operations, most recently operation on device 5 failed.\r\n");

	if (GETBIT(status, STAT_ERR_INT6_FL))
		pr_debug("intr: PROGRAM PAGE and BLOCK ERASE operations, most recently operation on device 6 failed..\r\n");

	if (GETBIT(status, STAT_ERR_INT7_FL))
		pr_debug("intr: PROGRAM PAGE and BLOCK ERASE operations, most recently operation on device 7 failed.\r\n");


	if (GETBIT(status, ECC_INT0_FL)) {
		pr_debug("intr: Selected Flag (source is ECC_UNC/ERROR/OVER_0) inthe ECC module is set.\r\n");
		nfc_check_ecc_state();
	}
	if (GETBIT(status, ECC_INT1_FL)) {
		pr_debug("intr: Selected Flag (source is ECC_UNC/ERROR/OVER_1) inthe ECC module is set.\r\n");
		nfc_check_ecc_state();
	}
	if (GETBIT(status, ECC_INT2_FL)) {
		pr_debug("intr: Selected Flag (source is ECC_UNC/ERROR/OVER_2) inthe ECC module is set.\r\n");
		nfc_check_ecc_state();
	}
	if (GETBIT(status, ECC_INT3_FL)) {
		pr_debug("intr: Selected Flag (source is ECC_UNC/ERROR/OVER_3) inthe ECC module is set.\r\n");
		nfc_check_ecc_state();
	}
	if (GETBIT(status, ECC_INT4_FL)) {
		pr_debug("intr: Selected Flag (source is ECC_UNC/ERROR/OVER_4) inthe ECC module is set.\r\n");
		nfc_check_ecc_state();
	}
	if (GETBIT(status, ECC_INT5_FL)) {
		pr_debug("intr: Selected Flag (source is ECC_UNC/ERROR/OVER_5) inthe ECC module is set.\r\n");
		nfc_check_ecc_state();
	}
	if (GETBIT(status, ECC_INT6_FL)) {
		pr_debug("intr: Selected Flag (source is ECC_UNC/ERROR/OVER_6) inthe ECC module is set.\r\n");
		nfc_check_ecc_state();
	}
	if (GETBIT(status, ECC_INT7_FL)) {
		pr_debug("intr: Selected Flag (source is ECC_UNC/ERROR/OVER_7) inthe ECC module is set.\r\n");
		nfc_check_ecc_state();
	}

	//nfc_ClrIntr(status);
	mmio_write_32((uintptr_t)&nfc_regs->int_status, 0);
}

void nfc_interrupt_enable(bool enable)
{
#if defined(__VALIDATION__)
	int ret;
	void *handler;
	uint32_t base, irq;
#endif

	if (ts_nfc->interrupt_en) {
		mmio_write_32((uintptr_t)&nfc_regs->int_mask, 0xffffffff); //enable all interrupt.
		mmio_write_32((uintptr_t)&nfc_regs->int_status, 0x0);		 //clean all interrupt.
#if defined(__VALIDATION__)
		ret = target_nfc_init(0, &base, &irq, &handler);
		if (ret < 0 || ret >= CONFIG_NFC_NUM)
			return;// ERROR;


		static struct IrqTableEntry Entry;

		Entry.Handler = handler;
		Entry.data = NULL;

		drv_irq_register(irq, &Entry);
		drv_irq_enable(irq);
#endif
	}
}

int get_nand_write_protect_state(uint32_t *status)
{
	*status = mmio_read_32((uintptr_t)&nfc_regs->mem_ctrl);

	pr_debug("MEM_CTRL: back select: %d\n", *status >> BANK_SEL);
	pr_debug("MEM_CTRL: memory select: %d\n", *status & 0x07);
	if (!GETBIT(*status, MEM0_WP))
		pr_debug("MEM_CTRL: MEM0_WP\n");

	if (!GETBIT(*status, MEM1_WP))
		pr_debug("MEM_CTRL: MEM1_WP\n");

	if (!GETBIT(*status, MEM2_WP))
		pr_debug("MEM_CTRL: MEM2_WP\n");

	if (!GETBIT(*status, MEM3_WP))
		pr_debug("MEM_CTRL: MEM3_WP\n");

	if (!GETBIT(*status, MEM4_WP))
		pr_debug("MEM_CTRL: MEM4_WP\n");

	if (!GETBIT(*status, MEM5_WP))
		pr_debug("MEM_CTRL: MEM5_WP\n");

	if (!GETBIT(*status, MEM6_WP))
		pr_debug("MEM_CTRL: MEM6_WP\n");

	if (!GETBIT(*status, MEM7_WP))
		pr_debug("MEM_CTRL: MEM7_WP\n");


	return *status;
}

void nfc_status_print(uint32_t status)
{
	if (GETBIT(status, MEM0_ST))
		pr_debug("STATUS: device 0 ready.\n");
	else
		pr_debug("STATUS: device 0 busy.\n");


	if (GETBIT(status, MEM1_ST))
		pr_debug("STATUS: device 1 ready.\n");
	else
		pr_debug("STATUS: device 1 busy.\n");


	if (GETBIT(status, MEM2_ST))
		pr_debug("STATUS: device 2 ready.\n");
	else
		pr_debug("STATUS: device 2 busy.\n");


	if (GETBIT(status, MEM3_ST))
		pr_debug("STATUS: device 3 ready.\n");
	else
		;//pr_debug("STATUS: device 3 busy.\n");


	if (GETBIT(status, MEM4_ST))
		pr_debug("STATUS: device 4 ready.\n");
	else
		;//pr_debug("STATUS: device 4 busy.\n");


	if (GETBIT(status, MEM5_ST))
		pr_debug("STATUS: device 5 ready.\n");
	else
		;//pr_debug("STATUS: device 5 busy.\n");


	if (GETBIT(status, MEM6_ST))
		pr_debug("STATUS: device 6 ready.\n");
	else
		;//pr_debug("STATUS: device 6 busy.\n");


	if (GETBIT(status, MEM7_ST))
		pr_debug("STATUS: device 7 ready.\n");
	else
		;//pr_debug("STATUS: device 7 busy.\n");


	if (GETBIT(status, CTRL_STAT))
		pr_debug("STATUS: controller busy.\n");
	else
		pr_debug("STATUS: controller ready.\n");


	if (GETBIT(status, DATASIZE_ERROR_ST))
		pr_debug("STATUS: ECC enable DATA_SIZE reg: incorrect value.\n");
	else
		pr_debug("STATUS: ECC enable DATA_SIZE reg: correct value.\n");


	if (GETBIT(status, DATA_REG_ST))
		pr_debug("STATUS: data in DATA_REG is available.\n");
	else
		pr_debug("STATUS: data in DATA_REG is not available.\n");


	if (GETBIT(status, CMD_ID))
		pr_debug("STATUS: CMD_ID = 0x%x\n", status >> CMD_ID);

}

uint32_t get_nfc_status(void)
{
	uint32_t status = mmio_read_32((uintptr_t)&nfc_regs->status); //0x08
	//nfc_status_print(status);

	return status;
}

uint32_t get_nand_mlun_index(void)
{
	uint32_t index = mmio_read_32((uintptr_t)&nfc_regs->mlun) & 0x07;
	//0xbc = 0x00 LUN_INDEX
	if (index >= ONE_LUN_SIZE)
		index = ONE_LUN_SIZE - 1;


	return index;
}


int get_nand_mlun_status(void)
{
	uint32_t status0, status1;

	status0 = mmio_read_32((uintptr_t)&nfc_regs->lun_status_0);
	status1 = mmio_read_32((uintptr_t)&nfc_regs->lun_status_1);

	pr_debug("%s:lun status0 = 0x%x, lun status1 = 0x%x\n",
			__func__, status0, status1);

	return 0;
}

int nand_get_pagesize(struct mtd_info *mtd)
{
	return mtd->writesize;
}

int nand_get_oobsize(struct mtd_info *mtd)
{
	return mtd->oobsize; //mtd->oobsize saved by onfi_param.spare_bytes_per_page.
}

void nfc_wait_cmd_transfer_complate_intr_flag(void)
{
	uint32_t timeout_us = 0;

	while (timeout_us < TIMEOUT_US_10_MS * 10) {
#ifdef __ATF__
		nfc_irqhandler(0);
#endif
		if (GETBIT(ts_nfc->int_status, CMD_END_INT_FL)) {
			RESETBIT(ts_nfc->int_status, CMD_END_INT_FL);
			break;
		}
		if (GETBIT(ts_nfc->int_status_old, CMD_END_INT_FL)) {
			RESETBIT(ts_nfc->int_status_old, CMD_END_INT_FL);
			break;
		}
		udelay(10);
		timeout_us += 10;
	}
	pr_err("%s:timeout_us=%d\n", __func__, timeout_us);
}

void nfc_fifo_flush(void)
{
	mmio_write_32((uintptr_t)&nfc_regs->fifo_init, TRUE); //flushing of FIFO. ready-time > 3ms ?
	udelay(1);
	pr_debug("%s\n", __func__);
}

void nfc_fifo_state_print(uint32_t status)
{
	pr_debug("FIFO status: ");
	if (GETBIT(status, DF_R_EMPTY))
		pr_debug(" DF_R_EMPTY..");

	if (GETBIT(status, DF_R_FULL))
		pr_debug(" DF_R_FULL.");

	if (GETBIT(status, DF_W_EMPTY))
		pr_debug(" DF_W_EMPTY..");

	if (GETBIT(status, DF_W_FULL))
		pr_debug(" DF_W_FULL..");


	if (GETBIT(status, CF_EMPTY))
		pr_debug(" CF_EMPTY..");

	if (GETBIT(status, CF_FULL))
		pr_debug(" CF_FULL..");

	if (GETBIT(status, CF_ACCPT_R))
		pr_debug(" CF_ACCPT_R..");

	if (GETBIT(status, CF_ACCPT_W))
		pr_debug(" CF_ACCPT_W..");

	pr_debug("\n");
}


uint32_t nfc_fifo_get_state(uint32_t *status)
{
	*status = mmio_read_32((uintptr_t)&nfc_regs->fifo_state); //0xb4
	//mmio_write_32((uintptr_t)&nfc_regs->fifo_state, 0x3 << DF_R_FULL);
	//nfc_fifo_state_print(*status);

	return *status;
}

int nfc_data_fifo_not_empty(void)
{
	uint32_t rd_dfifo_unempty;

	nfc_fifo_get_state(&rd_dfifo_unempty);
	if (GETBIT(rd_dfifo_unempty, DF_R_EMPTY) == FALSE)
		return true;


	return false;
}

int nfc_data_fifo_wait_not_empty(void)
{
	uint32_t delay_us = 0;

	while (delay_us <= TIMEOUT_US_1_MS) {
		if (nfc_data_fifo_not_empty()) {
			//break;
			return true;
		}

		udelay(TIME_TR);
		delay_us += TIME_TR;

	}
	return false;
}

int nfc_data_fifo_wait_empty(bool force)
{
	uint32_t delay_us = 0;
	/*data fifo need more than 6ms to empty.*/
	while (delay_us <= TIME_TBERS * 2) {
		udelay(5);
		delay_us += 5;
		if (false == nfc_data_fifo_not_empty())
			return true;

	}
	pr_info("%s: timeout_us=%d!\n", __func__, delay_us);

	if (force)
		nfc_fifo_flush();

	return false;
}

int nfc_data_fifo_not_full(void)
{
	uint32_t rd_dfifo_unfull, delay_us = TIME_TR * 100;

	while (delay_us) {
		nfc_fifo_get_state(&rd_dfifo_unfull);
		if (GETBIT(rd_dfifo_unfull, DF_W_FULL) == FALSE)
			return true;

		udelay(TIME_TR);
		delay_us -= TIME_TR;
	}
	pr_err("%s: timeout!\n", __func__);

	return false;
}

int nfc_cmd_fifo_wait_empty(void)
{
	uint32_t fifo_status;
	uint32_t delay_us = 0;

	while (delay_us <= TIME_TBERS * 2) {
		udelay(5);
		delay_us += 5;
		nfc_fifo_get_state(&fifo_status);
		if (GETBIT(fifo_status, CF_EMPTY))
			return true;

	}
	pr_err("%s: timeout_us=%d!\n", __func__, delay_us);

	nfc_fifo_flush();

	return false;
}


int nfc_cmd_fifo_check_not_full(void)
{
	uint32_t rc_dfifo_unfull;

	nfc_fifo_get_state(&rc_dfifo_unfull);
	if (GETBIT(rc_dfifo_unfull, CF_FULL) == FALSE)
		return true;


	return false;
}

void nand_status_print(uint8_t status)
{
	if (GETBIT(status, 0))
		pr_debug("BLOCK ERASE / PAGE (CACHE)PROGRAM fail!\n");
	//else
		//pr_debug("BLOCK ERASE / PAGE (CACHE)PROGRAM Pass!\n");


	if (GETBIT(status, 1))
		pr_debug("N-1 page Cache Program Fail !\n");

	if (!GETBIT(status, 5))
		pr_debug("Array Operation device Busy !\n");
	//else
		//pr_debug("Array Operation device Ready !\n");


	if (!GETBIT(status, 6))
		pr_debug("Cache oprations Busy!\n");
	//else
		//pr_debug("Cache oprations Ready!\n");


	if (!GETBIT(status, 7))
		pr_debug("Protected !\n");
	//else
		//pr_debug("Not Protected !\n");

}

void nfc_read_data_reg(uint8_t *status)
{
	/* 78h flow 3 row address, auto increase enable will repeat increase the address.*/
	*status = mmio_read_8(&nfc_regs->data_reg);
}

void nfc_get_command_status(struct nand_chip *chip, uint8_t *status)
{
	if (ts_nfc->auto_read_stat_en == false) {
		//nfc_wait_nand_ready(mtd, chip);
		if ((true != ts_nfc->row_addr_auto_increase) && (ts_nfc->mlun_en ||
		 GETBIT(chip->onfi_params.opt_cmd, READ_STATUS_ENHANCED_EN))) {
			nfc_exec_onfi_command(ONFI_CMD_SELECT_LUN_WITH_STATUS);
		} else {
			nfc_exec_onfi_command(ONFI_CMD_READ_STARUS);
		}
	}

	if (status) {
		nfc_read_data_reg(status);
		nand_status_print(*status);
	}

	/*device status state recovery, some device require it.*/
	nfc_exec_onfi_command(0x00);
}

/*
 * erase and program check.
 * return: 0:success 1:fail
 */
int nfc_check_command_status(struct nand_chip *chip)
{
	nfc_get_command_status(chip, &ts_nfc->nand_status);
	if (ts_nfc->nand_status & NAND_STATUS_FAIL) {
		pr_err("%s:command failed!\n", __func__);
		//return -1;
	}

	return ts_nfc->nand_status;
}

int nfc_check_last_command_status(struct nand_chip *chip)
{
	nfc_get_command_status(chip, &ts_nfc->nand_status);
	if (ts_nfc->nand_status & NAND_STATUS_CACHE_LAST_FAIL) {
		pr_err("%s:last command failed!\n", __func__);
		return -1;
	}

	return 0;
}

int rawnand_wait_command_complete(struct mtd_info *mtd,
		uint32_t delay_us, uint32_t max_timeout_us, bool send_cmd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t status;
	bool ret = false, ready = false;
	uint32_t timeout_us = 0;

	if (delay_us == 0)
		delay_us = 10;
	else if (delay_us > max_timeout_us)
		delay_us = max_timeout_us;


	while ((timeout_us < max_timeout_us) && !ret) {
		status = get_nfc_status(); //0x08
		if (GETBIT(status, MEM0_ST +
			ts_nfc->ready_dev) == TRUE << (MEM0_ST + ts_nfc->ready_dev)) {
			ret = true;
			return ret;
		} //else {
			//asm("nop");
			//pr_debug("%s:%d: %d device not ready!\n", __func__, __LINE__, ts_nfc->ready_dev);
		//}
		udelay(delay_us);
		timeout_us += delay_us;
	}
	if (!ret) {
		pr_debug("%s:%d: timeout %d device %d not ready!\n",
			__func__, __LINE__, timeout_us, ts_nfc->ready_dev);
	}

	if (send_cmd && (ret == false) /*ts_nfc->check_RB_line == false*/) {
		ts_nfc->nand_status = 0;
		nfc_get_command_status(chip, &ts_nfc->nand_status);
		if (NAND_READY == (ts_nfc->nand_status & NAND_READY)) {
			ret = ready = true;
		} else {
			pr_warn("%s:%d: timeout_us=%d status=0x%x\n",
			 __func__, __LINE__, timeout_us, ts_nfc->nand_status);
			ready = false;
		}
	}

	return ret;
}

/*
 * random read time(tR):25us Max
 * page Program(tPROG): 300us Typ.
 * Block Erase Time(tBERS): 3ms Typ.
 * device ready judge: RB_line hight and CE hight
 */
int nfc_wait_nand_ready(struct mtd_info *mtd, struct nand_chip *chip)
{
	uint32_t delay_us = 5;
	uint32_t max_timeout =
			MAX((uint32_t)chip->onfi_params.t_bers, (uint32_t)TIME_TBERS);

	return rawnand_wait_command_complete(mtd, delay_us, max_timeout, true);
}

/**
 * ts_waitfunc - wait until a command is done
 * @mtd: MTD device structure
 * @chip: NAND chip structure
 *
 * Wait for controller and FLASH to both be ready.
 */

static int ts_waitfunc(struct mtd_info *mtd, struct nand_chip *chip)
{
	uint8_t status;
	int ret;

	/* wait until both controller and NAND are ready */
	ret = nfc_wait_nand_ready(mtd, chip);
	/* if controller or NAND stalled, return error */
	if (ret != true) {
		pr_err("%s:device not ready\n", __func__);
		//return -1;
	}
	/* write NAND status command */
	nfc_get_command_status(chip, &status);
	/* Chip sometimes reporting write protect even when it's not */
	return status | NAND_STATUS_WP;
}

void nfc_select_ready_device(int8_t dev_index)
{
	RESET3BIT(ts_nfc->mem_ctrl, MEM0_ST);
	ts_nfc->mem_ctrl |= (1 << dev_index) << MEM0_WP; //disable write protect.
	ts_nfc->ready_dev = dev_index;
	ts_nfc->mem_ctrl |= ts_nfc->ready_dev;
	mmio_write_32((uintptr_t)&nfc_regs->mem_ctrl, ts_nfc->mem_ctrl);
	pr_debug("select MEM device %d !\n", dev_index);
}

#if 0
void __maybe_unused nfc_scan_nand_device_x_state_ready(void)
{
	uint32_t status;
	uint32_t timeout_us = 0;

	while (timeout_us < (TIMEOUT_US_1_MS * 50)) {
		status = get_nfc_status();	//0x08

		if (GETBIT(status, MEM0_ST)) {
			nfc_select_ready_device(MEM0_ST);
			break;
		}

		if (GETBIT(status, MEM1_ST)) {
			nfc_select_ready_device(MEM1_ST);
			break;
		}

		if (GETBIT(status, MEM2_ST)) {
			nfc_select_ready_device(MEM2_ST);
			break;
		}

		if (GETBIT(status, MEM3_ST)) {
			nfc_select_ready_device(MEM3_ST);
			break;
		}

		if (GETBIT(status, MEM4_ST)) {
			nfc_select_ready_device(MEM4_ST);
			break;
		}
		if (GETBIT(status, MEM5_ST)) {
			nfc_select_ready_device(MEM5_ST);
			break;
		}
		if (GETBIT(status, MEM6_ST)) {
			nfc_select_ready_device(MEM6_ST);
			break;
		}
		if (GETBIT(status, MEM7_ST)) {
			nfc_select_ready_device(MEM7_ST);
			break;
		}

		udelay(5);
		timeout_us += 5;
	}
}
#endif

int nfc_wait_dcu_ready(struct mtd_info *mtd)
{
	uint32_t status;
	uint32_t timeout_us = 0;
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t max_timeout = MAX(chip->onfi_params.t_bers, (uint16_t)TIME_TBERS);

	while (timeout_us < max_timeout) {
		status = get_nfc_status(); //0x08
		if (GETBIT(status, CTRL_STAT) == DCU_READY) { //wait_data DCU_READY
			return true;
		}
		/* dcu ready time very short sometimes, 1us check it.*/
		udelay(1);
		timeout_us += 1;
		pr_debug("%s: timeout_us=%d\n", __func__, timeout_us);
	}
	pr_info("%s: timeout_us=%d\n", __func__, timeout_us);

	return false;
}

int nfc_wait_dma_ready(void)
{
	uint32_t status;
	uint32_t timeout_us = 0;

	while (timeout_us < TIMEOUT_US_10_MS * 40) {
		status = mmio_read_32((uintptr_t)&nfc_regs->dma_ctrl); //0x70
		if (GETBIT(status, ERR_FLAG)) {
			//repeat transfer??
			pr_debug("DMA transfer error flag. \r\n");
		}
		if (GETBIT(status, DMA_READY) == TRUE << DMA_READY) { //wait_data DMA_READY
			pr_debug("DMA ready flag. transfer is completed. \r\n");
			return true;
		}

		if (ts_nfc->interrupt_en != TRUE) {
			udelay(1000 * 5);
			return true;
		}

#ifdef __ATF__
		//nfc_irqhandler(0);
#endif
		if (GETBIT(ts_nfc->int_status, DMA_INT_FL)) {
			RESETBIT(ts_nfc->int_status, DMA_INT_FL);
			return true;
		}

		if (GETBIT(ts_nfc->int_status, STAT_ERR_INT0_FL + ts_nfc->ready_dev)) {
			RESETBIT(ts_nfc->int_status, STAT_ERR_INT0_FL);
			break;
		}
		udelay(50);
		timeout_us += 50;
	}
	pr_err("%s: timeout_us=%d\n", __func__, timeout_us);

	return false;
}

static void nfc_ctrl_set_buswidth(bool set)
{
	mmio_clrsetbits_32((uintptr_t)&nfc_regs->control,
			0x01 << IO_WIDTH, (set ? 0x01 << IO_WIDTH : 0U));
}

void nfc_ctrl_auto_read_status_enable(bool enable)
{
	if (enable) {
		ts_nfc->auto_read_stat_en = true;
		SETBIT(ts_nfc->control, AUTO_READ_STAT_EN);
		/*mmio_write_32((uintptr_t)&nfc_regs->status_mask,
		 * 0xffffffff & ~((NAND_STATUS_CACHE_LAST_FAIL | NAND_STATUS_FAIL) << 8
		 * | (NAND_STATUS_READY | NAND_STATUS_ARRAY_READY)));
		 */
		//0xfc9f
		mmio_write_32((uintptr_t)&nfc_regs->status_mask, 0xffff);
	} else {
		ts_nfc->auto_read_stat_en = false;
		RESETBIT(ts_nfc->control, AUTO_READ_STAT_EN);
	}

	mmio_write_32((uintptr_t)&nfc_regs->control, ts_nfc->control);
}

uint32_t calc_cur_block(struct mtd_info *mtd, const uint64_t offset)
{
	uint32_t cur_blk = 0;

	cur_blk = (offset + 1) >> mtd->erasesize_shift;

	return cur_blk;
}

void nand_check_offset_of_block_is_bad(struct mtd_info *mtd, uint64_t *offset)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t block_is_bad = 1, cur_blk;

	while (block_is_bad) {
		cur_blk = calc_cur_block(mtd, *offset);
		if (cur_blk > chip->onfi_params.blocks_per_lun)
			break;


		if (nand_mtd_block_is_bad(mtd, cur_blk)) {
			pr_info("offset=0x%llx in a bad block !\n", *offset);
			*offset += mtd->erasesize;
		} else {
			block_is_bad = 0;
			nand_device_reset(mtd, 0);
			// must reset, otherwise, write-read has errors.
			break;
		}
	}
}


unsigned int nand_get_current_position(struct mtd_info *mtd)
{
	uint32_t row, col;

	nfc_wait_dcu_ready(mtd); //wait dcu...
	row = mmio_read_32((uintptr_t)&nfc_regs->addr0_row);
	col = mmio_read_32((uintptr_t)&nfc_regs->addr0_col);
	row = col + (row * mtd->writesize);
	//pr_debug("address 0: 0x%08x\n", row);

	return row;
}

void __maybe_unused nand_get_current_position_addr0(struct mtd_info *mtd,
		uint32_t *row, uint32_t *col)
{
	nfc_wait_dcu_ready(mtd);
	*row = mmio_read_32((uintptr_t)&nfc_regs->addr0_row);
	*col = mmio_read_32((uintptr_t)&nfc_regs->addr0_col);
}

void __maybe_unused nand_get_current_position_addr1(struct mtd_info *mtd,
		uint32_t *row, uint32_t *col)
{
	nfc_wait_dcu_ready(mtd);
	*row = mmio_read_32((uintptr_t)&nfc_regs->addr1_row);
	*col = mmio_read_32((uintptr_t)&nfc_regs->addr1_col);
}

unsigned int nfc_setting_data_register_size(enum e_DATA_REG_SIZE reg_size)
{
	/* The READ STATUS Command(70h),  DEVICE STATUS Command(72h),
	 * SELECT LUN WITH STATUS Command(78h), LUN STATUS Command(71h)
	 * must be select single byte. must select the DATA_REG register as data destination.
	 */
	//uint32_t reg_size_val = mmio_read_32((uintptr_t)&nfc_regs->data_reg_size);
	if (/*ts_nfc->check_RB_line == false ||*/ ts_nfc->auto_read_stat_en ||
	 (ts_nfc->command & 0x7000) == (ONFI_CMD_READ_STARUS & 0x7000)) {
		reg_size = SINGLE_BYTE;
	}
	mmio_write_32((uintptr_t)&nfc_regs->data_reg_size, (uint32_t)reg_size);

	return (uint32_t)reg_size;
}

int nfc_send_command(s_command *nfc_cmd)
{
	ts_nfc->command =
			nfc_cmd->cmd_2 << CMD_2 | nfc_cmd->cmd_1_3 << CMD_1_3 |
			nfc_cmd->cmd_0 << CMD_0 | nfc_cmd->data_sel << DATA_SEL |
			nfc_cmd->input_sel << INPUT_SEL | nfc_cmd->cmd_seq << CMD_SEQ;

	mmio_write_32((uintptr_t)&nfc_regs->command, ts_nfc->command);
	pr_debug("%s: command = 0x%x\n", __func__, ts_nfc->command);

	return 0;
}


void nfc_exec_command_default_delay(uint32_t onfiCmd)
{
	uint32_t delay = 0;

	switch (onfiCmd) {
	/*RESET COMMANDS*/
	case ONFI_CMD_RESET:
	case ONFI_CMD_SYNCH_RESET:
	case ONFI_CMD_LUN_RESET:
		delay = TIME_TRST;
		udelay(delay);
		break;
	/*IDENTIFICATION OPERATIONS*/
	case ONFI_CMD_READ_ID:
		/*no busy*/
		break;
	case ONFI_CMD_READ_PARAMETER_PAGE:
	case ONFI_CMD_READ_UNIQUE_ID:
		delay = (MIN(ts_nfc->onfi_params->t_r, (uint16_t)TIME_TR));
		udelay(delay);
		break;
	/*CONFIGURATION OPERATIONS*/
	case ONFI_CMD_GET_FEATURES:
	case ONFI_CMD_SET_FEATURES:
		delay = (TIME_TFEAT);
		udelay(delay);
		break;
	/*STATUS OPERATIONS*/
	case ONFI_CMD_SELECT_LUN_WITH_STATUS:
		/*no busy*/
		break;
	case ONFI_CMD_READ_STARUS:
	case ONFI_CMD_LUN_STATUS:
	case ONFI_CMD_DEVICE_STATUS:
	case ONFI_CMD_VOLUME_SELECT:
		delay = (TIME_TWHR);
		udelay(delay);
		break;
	/*COLUMN ADDRESS OPERATIONS*/
	case ONFI_CMD_CHANGE_READ_COLUMN:
	case ONFI_CMD_SELECT_CACHE_REGISTER:
		delay = (TIME_TWHR);
		udelay(delay);
		break;
	case ONFI_CMD_CHANGE_WRITE_COLUMN:
		break;
	/*READ OPERATIONS*/
	case ONFI_CMD_READ_PAGE:
		delay = (MIN(ts_nfc->onfi_params->t_r, (uint16_t)TIME_TR));
		udelay(delay);
		break;
	case ONFI_CMD_READ_PAGE_CACHE:
	case ONFI_CMD_READ_PAGE_CACHE_LAST:
		delay = (TIME_TCBSYR);
		udelay(delay);
		break;
	case ONFI_CMD_READ_MULTIPLANE:
		delay = (TIME_TDBSY);
		udelay(delay);
		break;
	case ONFI_CMD_TWO_PLANE_PAGE_READ:
		delay = (MIN(ts_nfc->onfi_params->t_r, (uint16_t)TIME_TR));
		udelay(delay);
		break;
	case ONFI_CMD_QUEUE_PAGE_READ:
		delay = (MIN(ts_nfc->onfi_params->t_r, (uint16_t)TIME_TR));
		udelay(delay);
		break;
	/*PROGRAM OPERATION*/
	case ONFI_CMD_PROGRAM_PAGE:
	case ONFI_CMD_PROGRAM_PAGE_IMD:
		/*device busy when write data to fifo and exec the 0x10 cmd. */
		//udelay(MIN(ts_nfc->onfi_params->t_prog, (uint16_t)TIME_TPROG));
		break;
	case ONFI_CMD_PROGRAM_PAGE_DEL:
		break;
	case ONFI_CMD_PROGRAM_PAGE_1:
		break;
	case ONFI_CMD_PROGRAM_PAGE_CACHE:
		delay = (TIME_TCBSYW);
		udelay(delay);
		break;
	case ONFI_CMD_PROGRAM_MULTIPLANE:
		delay = (TIME_TDBSY);
		udelay(delay);
		break;
	case ONFI_CMD_WRITE_PAGE:
		delay = (MIN(ts_nfc->onfi_params->t_prog, (uint16_t)TIME_TPROG));
		udelay(delay);
		break;
	case ONFI_CMD_WRITE_PAGE_CACHE:
		delay = (TIME_TCBSYW);
		udelay(delay);
		break;
	case ONFI_CMD_WRITE_MULTIPLANE:
		delay = (TIME_TDBSY);
		udelay(delay);
		break;
	//ERASE OPERATION
	case ONFI_CMD_ERASE_MULTIPLANE:
		delay = (TIME_TDBSY);
		udelay(delay);
		//break;
	case ONFI_CMD_ERASE_BLOCK:
		delay = (MIN(ts_nfc->onfi_params->t_bers, (uint16_t)TIME_TBERS));
		udelay(delay);
		break;
	/*COPYBACK OPERATION*/
	case ONFI_CMD_COPYBACK_READ:
		delay = (MIN(ts_nfc->onfi_params->t_r, (uint16_t)TIME_TR));
		udelay(delay);
		break;
	case ONFI_CMD_COPYBACK_PROGRAM:
		//udelay(MIN(ts_nfc->onfi_params->t_prog, (uint16_t)TIME_TPROG));
		break;
	case ONFI_CMD_COPYBACK_PROGRAM_1:
		break;
	case ONFI_CMD_COPYBACK_MULTIPLANE:
		delay = (TIME_TDBSY);
		udelay(delay);
		break;
	/*OTP OPERATION*/
	case ONFI_CMD_PROGRAM_OTP:
	case ONFI_CMD_DATA_PROTECT_OTP:
		//udelay(MIN(ts_nfc->onfi_params->t_prog, (uint16_t)TIME_TPROG));
		break;
	case ONFI_CMD_READ_PAGE_OTP:
		delay = (MIN(ts_nfc->onfi_params->t_r, (uint16_t)TIME_TR));
		udelay(delay);
		break;

	default:
		break;
	}

	pr_debug("%s: CMD:0x%x delay=%d\n", __func__, onfiCmd, delay);
}

void nfc_exec_command(uint32_t *onfiCmd)
{
	uint32_t delay_us = 1000;

	if ((*onfiCmd & 0x7000) == (ONFI_CMD_READ_STARUS & 0x7000)) {
		nfc_setting_data_register_size(SINGLE_BYTE);
		SETBIT(*onfiCmd, DATA_SEL); //1:DATA register
	} else {
		RESETBIT(*onfiCmd, DATA_SEL); //0:FIFO module
	}

	while (delay_us) {
		if (nfc_cmd_fifo_check_not_full())
			break;

		udelay(100);
		delay_us -= 100;
	}
	mmio_write_32((uintptr_t)&nfc_regs->command, *onfiCmd);
}

int nfc_exec_onfi_command_dma(uint32_t onfiCmd)
{
	ts_nfc->command = onfiCmd;

	SETBIT(ts_nfc->command, INPUT_SEL); //1:DMA module
	nfc_exec_command(&ts_nfc->command);

	nfc_exec_command_default_delay(onfiCmd);
	pr_debug("%s: command = 0x%x\n", __func__, ts_nfc->command);

	return 0;
}

int nfc_exec_onfi_command_siu(uint32_t onfiCmd)
{
	ts_nfc->command = onfiCmd;

	RESETBIT(ts_nfc->command, INPUT_SEL); //0:SIU module
	nfc_exec_command(&ts_nfc->command);

	nfc_exec_command_default_delay(onfiCmd);
	pr_debug("%s: command = 0x%x\n", __func__, ts_nfc->command);

	return 0;
}

int nfc_exec_onfi_command(uint32_t onfiCmd)
{
	uint32_t ret;

	if (ts_nfc->dma_en)
		ret = nfc_exec_onfi_command_dma(onfiCmd);
	else
		ret = nfc_exec_onfi_command_siu(onfiCmd);


	return ret;
}

void nfc_ecc_state_print(uint32_t state)
{
	if (ts_nfc->hw_ecc_en != true)
		return;


	//pr_err("%s:", __func__);
	if (GETBIT(state, ECC_ERROR_0))
		pr_err(" ECC_ERROR_0");

	if (GETBIT(state, ECC_ERROR_1))
		pr_err(" ECC_ERROR_1");

	if (GETBIT(state, ECC_ERROR_2))
		pr_err(" ECC_ERROR_2");

	if (GETBIT(state, ECC_ERROR_3))
		pr_err(" ECC_ERROR_3");

	if (GETBIT(state, ECC_ERROR_4))
		pr_err(" ECC_ERROR_4");

	if (GETBIT(state, ECC_ERROR_5))
		pr_err(" ECC_ERROR_5");

	if (GETBIT(state, ECC_ERROR_6))
		pr_err(" ECC_ERROR_6");

	if (GETBIT(state, ECC_ERROR_7))
		pr_err(" ECC_ERROR_7");


	if (GETBIT(state, ECC_UNC_0))
		pr_err(" ECC_UNC_0");

	if (GETBIT(state, ECC_UNC_1))
		pr_err(" ECC_UNC_1");

	if (GETBIT(state, ECC_UNC_2))
		pr_err(" ECC_UNC_2");

	if (GETBIT(state, ECC_UNC_3))
		pr_err(" ECC_UNC_3");

	if (GETBIT(state, ECC_UNC_4))
		pr_err(" ECC_UNC_4");

	if (GETBIT(state, ECC_UNC_5))
		pr_err(" ECC_UNC_5");

	if (GETBIT(state, ECC_UNC_6))
		pr_err(" ECC_UNC_6");

	if (GETBIT(state, ECC_UNC_7))
		pr_err(" ECC_UNC_7");


	if (GETBIT(state, ECC_OVER_0))
		pr_err(" ECC_OVER_0");

	if (GETBIT(state, ECC_OVER_1))
		pr_err(" ECC_OVER_1");

	if (GETBIT(state, ECC_OVER_2))
		pr_err(" ECC_OVER_2");

	if (GETBIT(state, ECC_OVER_3))
		pr_err(" ECC_OVER_3");

	if (GETBIT(state, ECC_OVER_4))
		pr_err(" ECC_OVER_4");

	if (GETBIT(state, ECC_OVER_5))
		pr_err(" ECC_OVER_5");

	if (GETBIT(state, ECC_OVER_6))
		pr_err(" ECC_OVER_6");

	if (GETBIT(state, ECC_OVER_7))
		pr_err(" ECC_OVER_7");

	//pr_err("\n");
}

int nfc_check_ecc_state(void)
{
	uint32_t state = mmio_read_32((uintptr_t)&nfc_regs->ecc_stat);

	mmio_write_32((uintptr_t)&nfc_regs->ecc_stat, 0);
	nfc_ecc_state_print(state);

	return state;
}

void nfc_assemble_hw_ecc(struct mtd_info *mtd,
		enum e_INTERRUPT_ECC_SEL ecc_intr_level)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t ecc_offset = mtd->writesize + 2;
	enum e_ECC_CAP ecc_cap = ECC_CAP_4;
	uint8_t ecc_threshold = 32;

	//config partial ecc bytes.
	RESET2BIT(ts_nfc->control, ECC_BLOCK_SIZE); //0:256 bytes 1:512 bytes
	switch (chip->onfi_params.data_bytes_per_ppage) {
	case 256:
		RESETBIT(ts_nfc->control, ECC_BLOCK_SIZE); //0:256 bytes 1:512 bytes
		pr_debug("256 partial bytes.");
		break;

	case 512:
		SETBIT(ts_nfc->control, ECC_BLOCK_SIZE); //0:256 bytes 1:512 bytes
		pr_debug("512 partial bytes.");
		break;

	case 1024:
		SETBIT(ts_nfc->control, ECC_BLOCK_SIZE + 1); //0:256 bytes 1:512 bytes
		pr_debug("1024 partial bytes.");
		break;

	default:
		SETBIT(ts_nfc->control, ECC_BLOCK_SIZE); //0:256 bytes 1:512 bytes
		pr_debug("error partial bytes =%d\n",
				chip->onfi_params.data_bytes_per_ppage);
		break;
	}
	mmio_write_32((uintptr_t)&nfc_regs->control, ts_nfc->control);

	//config ecc_cap
	switch (chip->ecc.strength) {
	case 4:
		ecc_cap = ECC_CAP_4;
		break;
	case 8:
		ecc_cap = ECC_CAP_8;
		break;
	case 16:
		ecc_cap = ECC_CAP_16;
		break;
	case 24:
		ecc_cap = ECC_CAP_24;
		break;
	case 32:
		ecc_cap = ECC_CAP_32;
		break;

	default:
		ecc_cap = ECC_CAP_4;
		break;
	}

	switch (ecc_cap) {
	case ECC_CAP_4:
		ecc_threshold = 4;
		break;
	case ECC_CAP_8:
		ecc_threshold = 8;
		break;
	case ECC_CAP_16:
		ecc_threshold = 16;
		break;
	case ECC_CAP_24:
		ecc_threshold = 24;
		break;
	case ECC_CAP_32:
		ecc_threshold = 32;
		break;
	case ECC_CAP_48:
		ecc_threshold = 48;
		break;

	default:
		ecc_threshold = 4;
		break;
	}

	mmio_write_32((uintptr_t)&nfc_regs->ecc_ctrl, ecc_intr_level << ECC_SEL |
			ecc_threshold << ERR_THRESHOLD | ecc_cap << ECC_CAP); //0x100

	/* config ecc_offset
	 */
	if (!chip->ecc.bytes) {
		chip->ecc.bytes =
			((ecc_threshold * 13 + (8 - ((ecc_threshold * 13) % 8))) / 8);
	}

	if (chip->onfi_params.spare_bytes_per_ppage)
		ecc_offset = mtd->writesize + mtd->oobsize - (chip->ecc.total + 1);

	if (ecc_offset % 2) {
		ecc_offset = ecc_offset + (ecc_offset % 2);
		//have to be aligned to two bytes when works in the synchronous mode.
	}
	if (ecc_offset < (mtd->writesize + TS_BBM_LEN))
		ecc_offset = mtd->writesize + (mtd->writesize % 2) + TS_BBM_LEN;

	ts_nfc->ecc_offset = ecc_offset;

	mmio_write_32((uintptr_t)&nfc_regs->ecc_offset, ecc_offset);
	pr_debug("ecc cap=%d ecc_blocks=%d ecc_offset=%d\n",
			ecc_threshold, chip->onfi_params.data_bytes_per_ppage, ecc_offset);
}

int nfc_setup_dma(struct mtd_info *mtd, uint32_t addr, uint32_t length,
		enum e_DMA_MODE dma_mode, enum e_DMA_BURST dma_burst, bool start)
{
	uint32_t dmaCnt = length % 4;

	if (dmaCnt)
		dmaCnt = length + 4 - dmaCnt;
	else
		dmaCnt = length;

	dmaCnt > 0xfffffffd ? dmaCnt = 0xfffffffd : dmaCnt;
	if (ts_nfc->boot_en)
		length > mtd->writesize ? (dmaCnt = mtd->writesize) : (dmaCnt = length);


	//if (mmio_read_32((uintptr_t)&nfc_regs->dma_cnt) != dmaCnt)
	{
		mmio_write_32((uintptr_t)&nfc_regs->dma_cnt, dmaCnt); //DMA_CNT bytes.
	}

	nfc_wait_dma_ready();
	mmio_write_32((uintptr_t)&nfc_regs->dma_addr_l, addr); //0x64 DMA_ADDRL
	//mmio_write_32((uintptr_t)&nfc_regs->dma_tlvl, 0x40);	 //test

	ts_nfc->dma_ctrl =
		start << DMA_START | dma_mode << DMA_MODE | dma_burst << DMA_BURST;
	//if (mmio_read_32((uintptr_t)&nfc_regs->dma_ctrl) != ts_nfc->dma_ctrl) {
		mmio_write_32((uintptr_t)&nfc_regs->dma_ctrl, ts_nfc->dma_ctrl); //0x70 DMA_CTRL
	//}

	pr_debug("%s: dma_addr_l = 0x%x, dma_cnt = 0x%x, dma_ctrl = 0x%x\n",
		 __func__, addr, dmaCnt, ts_nfc->dma_ctrl);

	return dmaCnt;
}

struct ts_timings {
	uint8_t seq0_tccs :6;  //[0:5] CLKB clock signal. Change column setup
	uint8_t seq0_tadl :6;  //[8:13] ALE to data loading time for synchronous interface.
			   //ALE to data start time for asynchronous interface.
	uint8_t seq0_trhw :6; //[16:21] Data output to command, address, or data input time for synchronous interface.
			   //RE# high to WE# low time for asynchronous interface.
	uint8_t seq0_twhr :6; //[24:29] Command cycle to data output time for synchronous interface.
	//WE# high to RE# low time for asynchronous interface.

	uint8_t seq1_twb :6; //[0:5] Busy time for interface change.
			 //The busy time when the interface changes from asynchronous to
			 // synchronous using the SET FEATURES(EFh)command,
			 //or from synchronous to asynchronous using the REST(FFh) command.
	uint8_t seq1_trr :6; //[8:13] Read high to Read low.
	//TRR time period from rising edge on read/buys intput line to
	// the moment when the read enable signal can be asserted.

	uint8_t asyn_trwp :4; //[0:3] RE# or WE# pulse width. tRP
	uint8_t asyn_trwh :4; //[4:7] RE# or WE# high hold time. tREH

	uint8_t syn_tcad :4; //[0:3] Command, Address, Data delay.
};

int nfc_update_timings(struct mtd_info *mtd, int chipnr,
				    const struct nand_data_interface *iface)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);
	uint64_t freq, cycle;
	uint8_t tWHR, tRHW, tADL, tCCS, tRR, tWB, tREH, tRP, tCAD;
	//CLKB clock....
	freq = info->clk_rate; //ts_clk_get_rate();
	cycle = 1000000000000 / freq;
	tWHR = iface->timings.sdr.tWHR_min / cycle;
	tRHW = iface->timings.sdr.tRHW_min / cycle;
	tADL = iface->timings.sdr.tADL_min / cycle;
	tCCS = iface->timings.sdr.tCCS_min / cycle;
	mmio_write_32((uintptr_t)&nfc_regs->time_seq_0,
	 tWHR << TWHR | tRHW << TRHW | tADL << TADL | tCCS << TCCS); //0x90 TIME_SEQ0

	tRR = iface->timings.sdr.tRR_min / cycle;
	tWB = iface->timings.sdr.tWB_max / cycle;
	mmio_write_32((uintptr_t)&nfc_regs->time_seq_1,
		 tRR << TRR | tWB << TWB); //0x94 TIME_SEQ1

	tREH = iface->timings.sdr.tREH_min / cycle;
	tRP = iface->timings.sdr.tRP_min / cycle;
	mmio_write_32((uintptr_t)&nfc_regs->timings_asyn,
	 (tREH & 0x0f) << TRWH | (tRP & 0x0f) << TRWP); //0x88 TIME_ASYN

	tCAD = 45000 / cycle;
	mmio_write_32((uintptr_t)&nfc_regs->timings_syn,
		 (tCAD & 0x0f) << TCAD); //0x8c TIME_SYN
	pr_debug("%s: twhr:%d trhw:%d tadl:%d tccs:%d trr:%d trb:%d treh:%d trp:%d tcad:%d\n",
	 __func__, tWHR, tRHW, tADL, tCCS, tRR, tWB, tREH, tRP, tCAD);

	return 0;
}

int __maybe_unused nfc_setting_timings(void)
{
	//CLKB clock....
	//mmio_write_32((uintptr_t)&nfc_regs->time_seq_0, 0x2d321932);//0x90 TIME_SEQ0
	mmio_write_32((uintptr_t)&nfc_regs->time_seq_0,
		 0x3f << TWHR | 0x3f << TRHW | 0x19 << TADL | 0x32 << TCCS); //0x90 TIME_SEQ0

	//mmio_write_32((uintptr_t)&nfc_regs->time_seq_1, 0x00000a32);//0x94 TIME_SEQ1
	mmio_write_32((uintptr_t)&nfc_regs->time_seq_1,
		 0x14 << TRR | 0x3f << TWB); //0x94 TIME_SEQ1

	//mmio_write_32((uintptr_t)&nfc_regs->timings_asyn, 0x0000008f);//0x88 TIME_ASYN
	mmio_write_32((uintptr_t)&nfc_regs->timings_asyn,
		 0x0a << TRWH | 0x0c << TRWP); //0x88 TIME_ASYN

	//mmio_write_32((uintptr_t)&nfc_regs->timings_syn, 0x00000005);//0x8c TIME_SYN
	mmio_write_32((uintptr_t)&nfc_regs->timings_syn, 0x5 << TCAD); //0x8c TIME_SYN
	pr_debug("%s:\n", __func__);
	return 0;
}

void nfc_ctrl_block_size_set(uint32_t block_size)
{
	if (block_size == 0)
		return;


	ts_nfc->control = mmio_read_32((uintptr_t)&nfc_regs->control);

	//00:32 pages per block.  01:64  10:128  11:256
	RESET2BIT(ts_nfc->control, BLOCK_SIZE_BIT);
	switch (block_size) {
	case 32:
		RESETBIT(ts_nfc->control, BLOCK_SIZE_BIT);
		break;
	case 64:
		SETBIT(ts_nfc->control, BLOCK_SIZE_BIT);
		break;
	case 128:
		RESETBIT(ts_nfc->control, BLOCK_SIZE_BIT);
		SETBIT(ts_nfc->control, BLOCK_SIZE_BIT + 1);
		break;
	case 256:
		SETBIT(ts_nfc->control, BLOCK_SIZE_BIT);
		SETBIT(ts_nfc->control, BLOCK_SIZE_BIT + 1);
		break;

	default:
		SETBIT(ts_nfc->control, BLOCK_SIZE_BIT);
		break;
	}

	mmio_write_32((uintptr_t)&nfc_regs->control, ts_nfc->control);
}

void nfc_ctrl_bbm_enable(bool enable)
{
	ts_nfc->bbm_en = enable;
	if (enable)
		SETBIT(ts_nfc->control, BBM_EN);
	else
		RESETBIT(ts_nfc->control, BBM_EN);


	mmio_write_32((uintptr_t)&nfc_regs->control, ts_nfc->control);
}

int nfc_ctrl_init_default(void)
{
	if (ts_nfc->auto_read_stat_en && ts_nfc->interrupt_en) {
		/* auto read PROGROM, ERASE command status, then trigger interrupt */
		SETBIT(ts_nfc->control, AUTO_READ_STAT_EN);
	} else {
		RESETBIT(ts_nfc->control, AUTO_READ_STAT_EN);
	}

	if (ts_nfc->mlun_en)
		SETBIT(ts_nfc->control, MLUN_EN);
	else
		RESETBIT(ts_nfc->control, MLUN_EN);


	RESETBIT(ts_nfc->control, SMALL_BLOCK_EN);
	RESET2BIT(ts_nfc->control, WORK_MODE); //asynchronous mode
	RESETBIT(ts_nfc->control, CLN_EN);	  //Clear NAND Disabled.

	if (ts_nfc->row_addr_auto_increase) {
		SETBIT(ts_nfc->control, ADDR1_AUTO_INCR);
		SETBIT(ts_nfc->control, ADDR0_AUTO_INCR);
	} else {
		RESETBIT(ts_nfc->control, ADDR1_AUTO_INCR);
		RESETBIT(ts_nfc->control, ADDR0_AUTO_INCR);
	}

	if (ts_nfc->protect_en)
		SETBIT(ts_nfc->control, PROT_EN);
	else
		RESETBIT(ts_nfc->control, PROT_EN);


	if (ts_nfc->bbm_en)
		SETBIT(ts_nfc->control, BBM_EN);
	else
		RESETBIT(ts_nfc->control, BBM_EN);


	RESETBIT(ts_nfc->control, IO_WIDTH);	//0:8 bits  1:16 bits
	RESETBIT(ts_nfc->control, DEV_STACK);
	//1:Two 8 bit device are selected.This bit is ignored when IO_WIDTH bit is cleared.
	//64 page per block
	SETBIT(ts_nfc->control, BLOCK_SIZE_BIT);	//0:32 1:64 page per block

	if (ts_nfc->hw_ecc_en)
		SETBIT(ts_nfc->control, ECC_EN);
	else
		RESETBIT(ts_nfc->control, ECC_EN);


	if (ts_nfc->interrupt_en)
		SETBIT(ts_nfc->control, INT_EN);
	else
		RESETBIT(ts_nfc->control, INT_EN);


	if (ts_nfc->retry_en) {
		SETBIT(ts_nfc->control, RETRY_EN);
		//by slave DMA. if disabled it, the interrupt status has TRANS_ERR_FL error in SIU mode.
	} else {
		RESETBIT(ts_nfc->control, RETRY_EN);
	}
	//ECC Block Size:4bit/512 bytes
	SETBIT(ts_nfc->control, ECC_BLOCK_SIZE); //0:256 bytes 1:512 bytes

	if (ts_nfc->check_RB_line) {
		// disable auto read status, R/B line used.
		RESETBIT(ts_nfc->control, READ_STATUS_EN);
	} else {
		SETBIT(ts_nfc->control, READ_STATUS_EN);
		/* mmio_write_32((uintptr_t)&nfc_regs->status_mask,
		 * 0xffffffff & ~((NAND_STATUS_CACHE_LAST_FAIL | NAND_STATUS_FAIL) << 8
		 * | (NAND_STATUS_READY | NAND_STATUS_ARRAY_READY)));
		 */
		mmio_write_32((uintptr_t)&nfc_regs->status_mask, 0xffff); // set 0 busy
	}

	mmio_write_32((uintptr_t)&nfc_regs->control, ts_nfc->control); //0x04 CONTROL

	return 0;
}

int nfc_mlun_select_index(uint8_t index)
{
	if (index >= ONE_LUN_SIZE)
		index = ONE_LUN_SIZE - 1;


	if (get_nand_mlun_index() == index)
		return 0;


	mmio_write_32((uintptr_t)&nfc_regs->mlun,
		TWO_LUN << LUN_SEL | index << MLUN_IDX); //0xbc = 0x00 LUN_INDEX

	nfc_exec_onfi_command(ONFI_CMD_SELECT_LUN_WITH_STATUS); //test....

	get_nfc_status();
	get_nand_mlun_status();

	return index;
}

int nand_setting_page_size(struct mtd_info *mtd,
		uint32_t data_size, bool oob_required)
{
	uint32_t page_size = nand_get_pagesize(mtd);
	uint32_t oob_size = nand_get_oobsize(mtd);

	if (data_size < 4)
		data_size = 4;

	if (data_size > (0x1 << 15) - 1) { //max 32767
		data_size = (0x1 << 15) - 1;
	}

	if (data_size > (page_size + oob_size)) {
		/*if data_size equl page+oob size, hw ecc used will trigger error.*/
		data_size = page_size + oob_size;
	} else if (page_size == 0) {
		data_size = page_size = ONE_PAGE_SIZE;
	}

	/*not a pagesize when write/read oob and read param page, otherwize is pagesize*/
	if (oob_required == FALSE)
		data_size = page_size;


	mmio_write_32((uintptr_t)&nfc_regs->data_size, data_size);

	return data_size;
}


static int nand_block_address_update_set(struct mtd_info *mtd,
		const uint64_t ori_offset, uint64_t *new_offset)
{
	uint32_t row_addr, col_addr;
#if 1 //defined(__ATF__) || defined(__UBOOT__)
	uint32_t cur_block, blk_offset;

	if (ori_offset > mtd->size)
		return -1;


	*new_offset = ori_offset;
	cur_block = calc_cur_block(mtd, ori_offset);
	if (cur_block > ARRAY_SIZE(ts_nfc->bbt_lite)) {
		/* avoid bbtlite[cur_block] out of bounds */
		mmio_write_32((uintptr_t)&nfc_regs->addr0_row, ori_offset >> mtd->writesize); //0x28 ADDR0_PAGE
		mmio_write_32((uintptr_t)&nfc_regs->addr0_col, ori_offset & mtd->writesize_mask); //0x24 ADDR0_COL

		return 0;
	}

	if (ts_nfc->bbt_lite_created && ts_nfc->bbt_lite[cur_block] &&
			(cur_block < ARRAY_SIZE(ts_nfc->bbt_lite))) {
		blk_offset = (ts_nfc->bbt_lite[cur_block] - cur_block) * mtd->erasesize;

		pr_debug("curblk=%d destblk=%d offset=0x%llx + 0x%x\n",
			cur_block, ts_nfc->bbt_lite[cur_block], ori_offset, blk_offset);
		*new_offset += blk_offset;
	}
#endif

	col_addr = *new_offset % mtd->writesize;
	row_addr = *new_offset / mtd->writesize;

	mmio_write_32((uintptr_t)&nfc_regs->addr0_row, row_addr); //0x28 ADDR0_PAGE
	mmio_write_32((uintptr_t)&nfc_regs->addr0_col, col_addr); //0x24 ADDR0_COL
	//printf("addr0_row=0x%x addr0_col=0x%x\n", row_addr, col_addr);

	return 0;
}


int nand_address_row_set(struct mtd_info *mtd, uint64_t offset)
{
	uint32_t row_addr;

	if (offset > mtd->size)
		return -1;


	row_addr = offset >> mtd->writesize_shift;

	mmio_write_32((uintptr_t)&nfc_regs->addr0_row, row_addr); //0x28 ADDR0_PAGE
	pr_debug("raw_addr=0x%x\n", row_addr);

	return 0;
}

int nand_address_col_set(struct mtd_info *mtd, uint64_t offset)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t col_addr;

	if (offset > mtd->size)
		return -1;


	col_addr = (uint16_t)offset /*& GENMASK(mtd->writesize_shift, 0)*/;
	nfc_wait_nand_ready(mtd, chip);
	mmio_write_32((uintptr_t)&nfc_regs->addr0_col, col_addr); //0x24 ADDR0_COL
	pr_debug("col_addr=0x%x\n", col_addr);

	return 0;
}

int nand_address_set(struct mtd_info *mtd, uint64_t offset)
{
	nand_address_col_set(mtd, offset);
	nand_address_row_set(mtd, offset);

	return 0;
}

void nand_address_set_oob(struct mtd_info *mtd,
			uint32_t row_addr, uint32_t col_addr)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	if (col_addr > (mtd->writesize + chip->onfi_params.spare_bytes_per_page)) {
		pr_err("col_addr 0x%x error!\n", col_addr);
		row_addr += col_addr / mtd->writesize;
		col_addr %= (mtd->writesize + chip->onfi_params.spare_bytes_per_page);
	} else if ((col_addr == mtd->writesize) && (ts_nfc->small_page == true)) {
		col_addr += 6;
	}

	mmio_write_32((uintptr_t)&nfc_regs->addr0_row, row_addr); //0x28 ADDR0_PAGE
	mmio_write_32((uintptr_t)&nfc_regs->addr0_col, col_addr); //0x24 ADDR0_COL
}

void nand_address1_set(struct mtd_info *mtd, uint64_t offset)
{
	uint32_t row_addr, col_addr;

	col_addr = offset % mtd->writesize;
	row_addr = offset / mtd->writesize;

	mmio_write_32((uintptr_t)&nfc_regs->addr1_row, row_addr); //0x28 ADDR0_PAGE
	mmio_write_32((uintptr_t)&nfc_regs->addr1_col, col_addr); //0x24 ADDR0_COL
}


void nand_lun_reset(struct mtd_info *mtd, uint64_t offset)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	nand_address_set(mtd, offset);
	nfc_exec_onfi_command(ONFI_CMD_LUN_RESET); //0xfa addr1~5 0x00 delay5ms
	nfc_wait_nand_ready(mtd, chip);
}

int nand_device_reset(struct mtd_info *mtd, uint8_t dev_index)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	/* use time 60us */
	nfc_select_ready_device(dev_index);

	nfc_fifo_flush();

	nfc_exec_onfi_command(ONFI_CMD_RESET); //0xff dealy9ms (first 36ms)

	nfc_wait_nand_ready(mtd, chip);
	nfc_wait_dcu_ready(mtd);
	nfc_cmd_fifo_wait_empty();

	return 0;
}

int nand_device_pre_config(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	chip->onfi_params.spare_bytes_per_page = 64;
	chip->onfi_params.data_bytes_per_ppage = 512;
	chip->onfi_params.spare_bytes_per_ppage = 16;

	chip->onfi_params.byte_per_page = mtd->writesize = ONE_PAGE_SIZE;
	mtd->erasesize = ONE_BLOCK_SIZE * ONE_PAGE_SIZE;
	ts_nfc->buswidth = SZ_8;
	chip->onfi_params.lun_count = ONE_LUN_SIZE;
	mtd->size = ONE_DEV_BYTES;

	return 0;
}

int nfc_hw_default_config(struct mtd_info *mtd)
{
	ts_nfc->ready_dev = TEST_MEMx;
	ts_nfc->e_work_mode = ASYNCHRONOUS;

	ts_nfc->boot_en = FALSE;
	ts_nfc->retry_en = TRUE; //FALSE;

	ts_nfc->mlun_en = FALSE;//TRUE; //can not enable ! .....
	//ts_nfc->mlun_en = TRUE; //can not enable ! .....
#ifdef __USE_DMA__
	ts_nfc->dma_en = TRUE;
#else
	ts_nfc->dma_en = FALSE;
#endif

	ts_nfc->hw_ecc_en = false;

	if (ts_nfc->e_work_mode != ASYNCHRONOUS)
		ts_nfc->hw_ecc_en = FALSE; //synchronous mode, the ECC must be turned off.


	/* Auto read state enabled will influence some command:
	 * READ PARAMETER PAGE (ECh)
	 * READ UNIQUE ID (EDh)
	 * GET FEATURES (EEh)
	 * The READ MODE (00h) command must be used to re-enable data output mode.
	 */
	//ts_nfc->auto_read_stat_en = TRUE;
	ts_nfc->auto_read_stat_en = FALSE; // onfi detect failed when enable the auto_read_stat_en.

	/*must be enable check_RB_line, because the NFC auto send 0x70 then send 0x00 when exec commands(not avoid),
	 * if not check the RB_line, the data not ready, the data will read error.
	 */
	ts_nfc->check_RB_line = TRUE;	// check R/B line in ATF.

	ts_nfc->protect_en = FALSE; //TRUE;
#if defined(__ATF__) || defined(__UBOOT__)
	ts_nfc->interrupt_en = /*TRUE;/*/FALSE;
#else
	ts_nfc->interrupt_en = TRUE;
#endif

#if __USE_HW_BBM__
	ts_nfc->bbm_en = TRUE;
#else
	ts_nfc->bbm_en = FALSE;
#endif

	/* auto increase row address */
	ts_nfc->row_addr_auto_increase = TRUE; //FALSE;

	nand_device_pre_config(mtd);

	return 0;
}

void nfc_ctrl_judge_small_block(struct mtd_info *mtd, uint32_t page)
{
	if (mtd->writesize >= 2048) {
		ts_nfc->small_page = false;
		RESETBIT(ts_nfc->control, SMALL_BLOCK_EN);
	} else {
		ts_nfc->small_page = true;
		SETBIT(ts_nfc->control, SMALL_BLOCK_EN);
	}
	mmio_write_32((uintptr_t)&nfc_regs->control, ts_nfc->control); //0x04 CONTROL
}

void nfc_bbm_record_set(uint32_t dev0_ptr, uint32_t dev0_size)
{
		mmio_write_32((uintptr_t)&nfc_regs->dev0_ptr, dev0_ptr);
		//mmio_write_32((uintptr_t)&nfc_regs->dev1_ptr, ts_nfc->dev1_ptr);
		mmio_write_32((uintptr_t)&nfc_regs->dev0_size, dev0_size);
}

int ts_nand_scan_bbt_lite(struct mtd_info *mtd, s_NFC_Info *nfc)
{
	uint32_t i, j, blocks;

	blocks = mtd->size / mtd->erasesize;

	nfc->bbt_lite_created = false;
	for (i = 0, j = 0; i < sizeof(nfc->bbt_lite); i++, j++) {
		while (j <= blocks) {
			if (nand_mtd_block_is_bad(mtd, j)) {
				j++;
				if (j > blocks)
					break;

			} else {
				break;
			}
		}
		nfc->bbt_lite[i] = j;
		//printf("i=%d j=%d\n", i, j);
	}

	nfc->bbt_lite_created = true;

	return 0;
}

int nand_device_init(struct mtd_info *mtd)
{
	int ret;
	struct nand_chip *chip = mtd_to_nand(mtd);

	ret = nand_read_id_0(mtd);
	if (ret)
		return ret;


	nfc_ctrl_judge_small_block(mtd, mtd->writesize);
	ret = detect_onfi(mtd);
	if (ret)
		return ret;


	nand_device_reset(mtd, MEM0_ST); // flush the data fifo.
	//TODO...
	/* update the nfc configure and timing after detect. */
	nand_setting_page_size(mtd, mtd->writesize, false); //0x84 mtd->writesize -- 2048 bytes //max:32K
	nfc_ctrl_judge_small_block(mtd, mtd->writesize);
	nfc_ctrl_block_size_set(chip->onfi_params.pages_per_block);

	ts_nand_scan_bbt_lite(mtd, ts_nfc);
	nand_device_reset(mtd, 0);

#if __USE_HW_BBM__
	//BBM_EN  write DEV_PTRx DEV_SIZEx
	//if (ts_nfc->bbm_en)
	{
		//1.search bbt in nand flash.
		static bool searched;

		if (searched == false) {
			//ts_nfc->dev0_ptr = (uintptr_t)p_bbm_dev0_ptr;
			//ts_nfc->dev0_size = 0;//BBM_RMAP_DEV_SIZE;
			//ts_nfc->dev1_ptr = NULL;//(uintptr_t)p_bbm_dev1_ptr;
			//ts_nfc->dev1_size = BBM_RMAP_DEV_SIZE;
			//nfc_ctrl_bbm_enable(false);
			//create bbt
			nand_scan_bbt(ts_nfc->bbt_lite[sizeof(ts_nfc->bbt_lite) - 1]);
			searched = true;
			//nfc_ctrl_bbm_enable(true);
		}

		nfc_bbm_record_set(ts_nfc->dev0_ptr, ts_nfc->dev0_size);
		pr_debug("bbm: dev0_ptr=0x%p, dev0_size=0x%x\n",
				(void *)(uintptr_t)ts_nfc->dev0_ptr, ts_nfc->dev0_size);
		pr_info("bbm: dev0_ptr=0x%p, dev0_size=0x%x\n",
				(void *)(uintptr_t)ts_nfc->dev0_ptr, ts_nfc->dev0_size);
	}

	nfc_bbm_flush_remap_flag();

#endif

	return 0;
}

int nfc_hw_init(struct mtd_info *mtd)
{
	nfc_fifo_flush();
	nfc_hw_default_config(mtd);
	nfc_ctrl_init_default();
	nfc_interrupt_enable(ts_nfc->interrupt_en);
	//if (ts_nfc->hw_ecc_en)
		//nfc_assemble_hw_ecc(mtd, ECC_SEL_UNC);	// set default val.


	nfc_setting_data_register_size(SINGLE_BYTE);

	//nand_device_reset(mtd, MEM1_ST);	// has error.
	nand_device_reset(mtd, MEM0_ST);
	// write default page_size.
	nand_setting_page_size(mtd, mtd->writesize, false);

	//PROT_EN write PROTECT...

	get_nand_write_protect_state(&ts_nfc->mem_ctrl);

	return 0;
}

static int __maybe_unused
	nand_mtd_block_mark_bad(struct mtd_info *mtd, unsigned int block)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	unsigned int nbpages_per_block = mtd->erasesize /
									 mtd->writesize;
	//uint8_t bbm_marker_main[2] = {03,00};
	uint8_t bbm_marker[2] = {03, 00};
	uint8_t buff[4096 + 256];
	uint8_t page;
	int ret;

	memset(buff, 0xff, sizeof(buff));
	memcpy(&buff[0], bbm_marker, sizeof(bbm_marker));
	memcpy(&buff[mtd->writesize], bbm_marker, sizeof(bbm_marker));

	nfc_wait_dcu_ready(mtd);
	for (page = 0U; page < 1U; page++) {
		ret = nand_write_oob_page_cmd(mtd, block * nbpages_per_block +
				page * (nbpages_per_block - 1), 0, (uintptr_t)buff,
				mtd->writesize + chip->onfi_params.spare_bytes_per_page);
		if (ret != 0)
			return ret;

	}
	nfc_wait_dcu_ready(mtd);

	return 0;
}

static int nand_mtd_block_is_bad(struct mtd_info *mtd, unsigned int block)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	unsigned int nbpages_per_block = mtd->erasesize /
									 mtd->writesize;
	uint8_t bbm_marker_main[4]__aligned(4) = {0};
	uint8_t bbm_marker[4]__aligned(4) = {0};
	uint8_t page;
	int ret;

	nfc_wait_dcu_ready(mtd);
	for (page = 0U; page < 1U; page++) {
		ret = nand_read_oob_page_cmd(mtd, block * nbpages_per_block +
			page * (nbpages_per_block - 1), 0,
			(uintptr_t)bbm_marker_main, sizeof(bbm_marker_main));
		if (ret != 0)
			return ret;


		ret = nand_read_oob_page_cmd(mtd, block * nbpages_per_block +
			page * (nbpages_per_block - 1), mtd->writesize,
			(uintptr_t)bbm_marker, sizeof(bbm_marker));
		if (ret != 0)
			return ret;


		if (ts_nfc->buswidth == SZ_8) {
			if (chip->onfi_params.jedec_id == 0xc8) {//"GIGADEVICE"
				if ((bbm_marker_main[0] != 0xff) && (bbm_marker[0] != 0xff)) {
					pr_warn("Block %u is bad, main0=0x%x spare0=0x%x\n",
						block, *(uint16_t *)bbm_marker_main, *(uint16_t *)bbm_marker);
					return 1;
				}

			} else if (chip->onfi_params.jedec_id == 0x2c) { //micron
				if (bbm_marker[0] != 0xff) {
					pr_warn("Block %u is bad, spare0=0x%x\n", block, *(uint8_t *)bbm_marker);
					return 1;
				}
			} else {
				if (bbm_marker[0] != GENMASK_32(7, 0)) {
					pr_warn("Block %u is bad, spare0=0x%x\n", block, *(uint8_t *)bbm_marker);
					return 1;
				}
			}

		} else { //buswidth == 16
			if ((bbm_marker[0] != GENMASK_32(7, 0)) ||
				 (bbm_marker[1] != GENMASK_32(7, 0))) {
				pr_warn("Block %u is bad, spare0=0x%x spare1=0x%x\n",
					block, bbm_marker[0], bbm_marker[1]);
				return 1;
			}

		}
	}
	nfc_wait_dcu_ready(mtd);

	return 0;
}


//int nand_scan_bbt(struct s_NFC_Info ts_nfc, uint8_t *buf, struct nand_bbt_descr *bd, int chip)
int nand_scan_bbt(struct mtd_info *mtd, uint32_t block)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t count = 0;
	int ret;
	static struct bbm_record record[2];
	uint32_t nb_blk_in_dev =
			chip->onfi_params.blocks_per_lun * chip->onfi_params.lun_count;
	uint16_t bbt_blk = nb_blk_in_dev - 1;

	ts_nfc->good_pool_blk_pos = bbt_blk - 5;

	ts_nfc->dev0_ptr = (uintptr_t)&record[0];
	ts_nfc->dev0_size = 1;//count;
	nfc_bbm_record_set(ts_nfc->dev0_ptr, ts_nfc->dev0_size);

	nfc_set_ecc(false);
	for (; block < nb_blk_in_dev; block++) {
		ret = nand_mtd_block_is_bad(mtd, block);
		if (ret == 1) {
			record[0].record_line[count].src = ts_nfc->good_pool_blk_pos--;
			record[0].record_line[count].dst = (uint16_t)block;
			//record[1].record_line[count].src = goot_pool_blk_last - count;
			//record[1].record_line[count].dst = (uint16_t)block;
			pr_warn("record%p,record0 = 0x%x record1 = 0x%x\n",
				 &record[0], (uint32_t)record[0].record_line[count].src,
				(uint32_t)record[1].record_line[count].src);
			count++;
		}
	}

	pr_warn("bad block sum %d\n", count);

#if __USE_HW_BBM__
	nfc_bbm_flush_remap_flag();
#endif

	nfc_set_ecc(true);

	return 0;
}

#if __ENABLE_ERASE__
int nand_check_erase_block_data(struct mtd_info *mtd)
{
	uint32_t i;

	/* read one page for test. */
	nfc_exec_onfi_command_siu(ONFI_CMD_READ_PAGE);

	for (i = 0; i < ONE_BLOCK_SIZE * mtd->writesize / 4; i++) {
		//4096 = 16k bytes...
		//nfc_data_fifo_not_empty();
		pr_debug("[%d]", i);
		if (read_check(&nfc_regs->fifo_data, 0xffffffff))
			pr_debug("error![%d]", i);

	}

	return 0;
}

uint8_t nand_erase_siu(struct mtd_info *mtd,
		uint64_t offset, uint32_t length, bool force)
{ //TODO: offset not a start of block. read -> erase -> write.
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t i = 0, erase_blocks;
	uint64_t offset1 = offset;
	uint8_t ret = 0;

	erase_blocks = length >> mtd->erasesize_shift;
	i = length % mtd->erasesize;
	if (i)
		erase_blocks += 1; //TODO COPYBACK...

	pr_debug("nfc_block_erase addr=0x%llx len=0x%x blocks=%d.\n",
			offset, length, erase_blocks);
	while (erase_blocks) {
		if (force == false) {
			/* search a good block and set row address. */
			nand_block_address_update_set(mtd, offset, &offset1);

			/* judeg the good block.  must check! */
			nand_check_offset_of_block_is_bad(mtd, &offset1);
		}

		/* erase the block */
		nfc_cmd_fifo_wait_empty();
		nfc_wait_nand_ready(mtd, chip);
		nfc_wait_dcu_ready(mtd);

		nand_address_set(mtd, offset1);
		nfc_exec_onfi_command_siu(ONFI_CMD_ERASE_BLOCK);

		nfc_wait_nand_ready(mtd, chip);/*erase use time 3ms*/
		nfc_get_command_status(chip, &ts_nfc->nand_status);
		if (ts_nfc->nand_status & NAND_STATUS_FAIL) {
			pr_err("0x%08llx erase failed, bad block !\n", offset1);
			ret = ts_nfc->nand_status;
		}

		nfc_wait_dcu_ready(mtd);
		nfc_cmd_fifo_wait_empty();

		offset += mtd->erasesize;
		erase_blocks--;
		//nand_check_erase_block_data(mtd);
	}

	return ret;
}

int ts_nand_erase_one_block(struct mtd_info *mtd, int page)
{
	return nand_erase_siu(mtd, page << mtd->writesize_shift, mtd->erasesize, true);
}

int nand_erase_siu_mlun(struct mtd_info *mtd, uint64_t offset, uint32_t length)
{ //TODO: offset not a start of block. read -> erase -> write.
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t i = 0, erase_blocks;

	//nand_check_offset_of_block_is_bad(mtd, &offset);

	pr_debug("nfc_block_erase addr=0x%llx len=0x%x.\n", offset, length);

	erase_blocks = length / mtd->erasesize;
	i = length % mtd->erasesize;
	if (i)
		erase_blocks += 1;


	nand_address_set(mtd, offset);

	//erase.....
	nfc_cmd_fifo_wait_empty();
	nfc_wait_nand_ready(mtd, chip);
	nfc_wait_dcu_ready(mtd);

#if 0
	if (ts_nfc->mlun_en) {
		nfc_mlun_select_index(0);
#if 0
		nfc_exec_onfi_command_siu(ONFI_CMD_ERASE_BLOCK);
#else
		nfc_exec_onfi_command_siu(ONFI_CMD_ERASE_MULTIPLANE);
#endif
		nfc_wait_nand_ready(mtd, chip);
		nand_address_set(mtd, offset + (0x1 << ROW_PLANE_BIT + ROW_BIT));
		//nfc_mlun_select_index(1);
	}

	nfc_exec_onfi_command_siu(ONFI_CMD_ERASE_BLOCK);

#else
	nand_address_set(mtd, offset);
	nand_address1_set(mtd, offset + (0x1 << (ROW_PLANE_BIT + ROW_BIT)));

	nfcCmd.cmd_seq = 0x0E;
	nfcCmd.input_sel = INPUT_SIU;
	nfcCmd.data_sel = DATA_SEL_FIFO;
	nfcCmd.cmd_0 = NAND_CMD_ERASE1;
	nfcCmd.cmd_1_3 = NAND_CMD_ERASE1;
	nfcCmd.cmd_2 = NAND_CMD_ERASE2;
	nfc_send_command(&nfcCmd);

#endif

	get_nand_mlun_status();
	nfc_cmd_fifo_wait_empty();
	nfc_wait_nand_ready(mtd, chip);
	nfc_wait_dcu_ready(mtd);
	//nfc_get_command_status(chip, &ts_nfc->nand_status);

	/* read one page for test.*/
	//nfc_mlun_select_index(0);
	nand_address_set(mtd, offset);
	nand_check_erase_block_data(mtd);

	//nfc_mlun_select_index(1);
	offset += (0x1 << (ROW_PLANE_BIT + ROW_BIT));
	nand_address_set(mtd, offset);
	nand_check_erase_block_data(mtd);

	return length;
}
#endif


#if __ENABLE_WRITE__
static void raw_nand_write_data(struct mtd_info *mtd,
							  uintptr_t buff, unsigned int length, bool use_bus8)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);
	uintptr_t data_base = (uintptr_t)&info->regs->fifo_data;

	if (use_bus8 && (chip->options & NAND_BUSWIDTH_16)) {
		/* Reconfigure bus width to 8-bit */
		nfc_ctrl_set_buswidth(false);
	}

	if ((((uintptr_t)buff & BIT(0)) != 0U) && (length != 0U)) {
		mmio_write_8(data_base, *(uint8_t *)buff);
		//pr_debug(" 0x%02x ", *buff);
		buff += sizeof(uint8_t);
		length -= sizeof(uint8_t);
	}

	if ((((uintptr_t)buff & GENMASK_32(1, 0)) != 0U) && (length >= sizeof(uint16_t))) {
		mmio_write_16(data_base, *(uint16_t *)buff);
		//pr_debug(" 0x%04x ", *(uint16_t *)buff);
		buff += sizeof(uint16_t);
		length -= sizeof(uint16_t);
	}

	/* 32bit aligned */
	while (length >= sizeof(uint32_t) && nfc_data_fifo_not_full()) {
		mmio_write_32(data_base, *(uint32_t *)buff);
		//pr_debug(" 0x%08x ", *(uint32_t *)buff);
		buff += sizeof(uint32_t);
		length -= sizeof(uint32_t);
	}

	/* Read remaining bytes */
	if (length && nfc_data_fifo_not_full()) {
		mmio_write_16(data_base, *(uint16_t *)buff);
		//pr_debug(" 0x%04x ", *(uint16_t *)buff);
		buff += length;
		length -= length;
	}


	if (use_bus8 && (chip->options & NAND_BUSWIDTH_16)) {
		/* Reconfigure bus width to 16-bit */
		nfc_ctrl_set_buswidth(true);
	}
}

static void raw_nand_fill_data(struct mtd_info *mtd,
							uint64_t offset, unsigned int length, bool use_bus8)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);
	uintptr_t data_base = (uintptr_t)&info->regs->fifo_data;

	uint8_t buff[4] = {0xff, 0xff, 0xff, 0xff};

	if (use_bus8 && (chip->options & NAND_BUSWIDTH_16)) {
		/* Reconfigure bus width to 8-bit */
		nfc_ctrl_set_buswidth(false);
	}

	if ((((uintptr_t)offset & BIT(0)) != 0U) && (length != 0U)) {
		mmio_write_8(data_base, *buff);
		//pr_debug(" 0x%02x ", *buff);
		length -= sizeof(uint8_t);
	}

	if ((((uintptr_t)offset & GENMASK_32(1, 0)) != 0U) && (length >= sizeof(uint16_t))) {
		mmio_write_16(data_base, *(uint16_t *)buff);
		//pr_debug(" 0x%04x ", *(uint16_t *)buff);
		length -= sizeof(uint16_t);
	}

	/* 32bit aligned */
	while (length >= sizeof(uint32_t)) {
		mmio_write_32(data_base, *(uint32_t *)buff);
		//pr_debug(" 0x%08x ", *(uint32_t *)buff);
		length -= sizeof(uint32_t);
	}

	/* Read remaining bytes */
	if (length >= sizeof(uint16_t)) {
		mmio_write_16(data_base, *(uint16_t *)buff);
		//pr_debug(" 0x%04x ", *(uint16_t *)buff);
		length -= sizeof(uint16_t);
	}

	if (length != 0U) {
		mmio_write_8(data_base, *buff);
		//pr_debug(" 0x%02x ", *buff);
		length -= sizeof(uint8_t);
	}

	if (use_bus8 && (chip->options & NAND_BUSWIDTH_16)) {
		/* Reconfigure bus width to 16-bit */
		nfc_ctrl_set_buswidth(true);
	}
}

/*
 * reutrn: 0: success  1: fail
 */
int nand_write_one_page_siu(struct mtd_info *mtd, uint64_t offset,
		const uintptr_t buff, uint32_t length, uint32_t *length_write)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uintptr_t p = buff;
	uint32_t remainder = offset % mtd->writesize;
	uint8_t status;

	pr_debug("%s: addr=0x%lx offset=0x%llx len=0x%x.\n",
			__func__, buff, offset, length);
	asm("isb");
	length = (length > mtd->writesize ? mtd->writesize : length);
	/* if offset val not a page start, then set offset to page start.(HWECC required) */
	if (remainder) {
		offset -= remainder;
		nand_address_col_set(mtd, offset);
	}
	nand_setting_page_size(mtd, mtd->writesize, false);
	if (ts_nfc->row_addr_auto_increase == FALSE)
		nand_address_set(mtd, offset);


	//Writing...
	nfc_exec_onfi_command_siu(ONFI_CMD_PROGRAM_PAGE);
	if (remainder)
		raw_nand_fill_data(mtd, 0, remainder, TRUE);//TODO random write.

	raw_nand_write_data(mtd, p, length, TRUE);

	if (length < mtd->writesize || remainder) {
		raw_nand_fill_data(mtd, remainder + length,
			mtd->writesize - (remainder + length), TRUE);
		nfc_fifo_flush();
	}
	udelay(MIN(chip->onfi_params.t_prog, (uint16_t)TIME_TPROG));

	nfc_get_command_status(chip, NULL);
	nfc_wait_nand_ready(mtd, chip);
	nfc_read_data_reg(&status);
	nfc_cmd_fifo_wait_empty();

	*length_write += length;
#if defined(__UBOOT__)
	return 0;
#else
	return status;
#endif
}

int nand_write_multi_pages_siu(struct mtd_info *mtd, uint64_t offset,
		const uintptr_t buff, uint32_t length, uint32_t *length_write)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t i, j, len = 0, tmp;
	uintptr_t p = buff;

	nand_setting_page_size(mtd, mtd->writesize, false);
	if (ts_nfc->row_addr_auto_increase == FALSE)
		nand_address_set(mtd, offset);

	nfc_wait_dcu_ready(mtd);
	get_nand_write_protect_state(&ts_nfc->mem_ctrl);

	for (j = 0; j < (length / mtd->writesize) - 1; j++) {
		nfc_exec_onfi_command_siu(ONFI_CMD_PROGRAM_PAGE_CACHE);
		for (i = 0; i < mtd->writesize / 4; i++) {
			mmio_write_32((uintptr_t)&nfc_regs->fifo_data,
					 tmp = *(uint32_t *)p); //0x38
			//pr_debug("wr:0x%lx:0x%08x ", p, tmp);
			nfc_data_fifo_not_full();
			p += 4;
		}
		if (ts_nfc->row_addr_auto_increase == FALSE) {
			pr_debug("0x%08x write down.\n", nand_get_current_position(mtd));
			offset += (j + 1) * mtd->writesize;
			nand_address_set(mtd, offset);
		}
		nfc_get_command_status(chip, &ts_nfc->nand_status);
		if (ts_nfc->nand_status & NAND_STATUS_CACHE_LAST_FAIL) {
			pr_err("%s:last command failed!\n", __func__);
			//TODO
		}
		if (ts_nfc->nand_status & NAND_STATUS_FAIL) {
			pr_err("%s:command failed!\n", __func__);
			//TODO
		}

		len += mtd->writesize;
		//nfc_wait_nand_ready(mtd, chip);
	}

	if (ts_nfc->row_addr_auto_increase == FALSE) {
		nfc_wait_dcu_ready(mtd);
		offset += j * mtd->writesize;
		nand_address_set(mtd, offset);
	}

	nfc_exec_onfi_command_siu(ONFI_CMD_PROGRAM_PAGE);
	for (i = 0; i < mtd->writesize / 4; i++) {
		mmio_write_32((uintptr_t)&nfc_regs->fifo_data, tmp = *(uint32_t *)p);
		//pr_debug("wr:0x%lx:0x%08x ", p, tmp);
		nfc_data_fifo_not_full();
		p += 4;
	}
	pr_debug("\n");
	nfc_get_command_status(chip, &ts_nfc->nand_status);
	if (ts_nfc->nand_status & NAND_STATUS_CACHE_LAST_FAIL) {
		pr_err("%s:last command failed!\n", __func__);
		//TODO
	}
	if (ts_nfc->nand_status & NAND_STATUS_FAIL) {
		pr_err("%s:command failed!\n", __func__);
		//TODO
	}

	pr_debug("0x%08x write down.\n", nand_get_current_position(mtd)); //test.
	len += mtd->writesize;
	*length_write += len;
	nfc_cmd_fifo_wait_empty();
	nfc_wait_nand_ready(mtd, chip);

	return nfc_check_command_status(chip);
}

int nand_write_pages_siu(struct mtd_info *mtd, uint64_t offset,
		const uintptr_t buff, uint32_t length, uint32_t *length_write)
{
	uint32_t pages, remainder, new_page_offset = 0;
	uintptr_t p = buff;

	pr_debug("%s: address = 0x%lx, nand offset = 0x%llx, length = 0x%x\n",
		 __func__, buff, offset, length);

	nand_address_set(mtd, offset);

	remainder = offset % mtd->writesize;
	new_page_offset = mtd->writesize - remainder;
	if (new_page_offset && (new_page_offset < mtd->writesize)
		 && (new_page_offset <= length)) {//TODO CHANGE_COL
		nand_write_one_page_siu(mtd, offset, p, new_page_offset, length_write);
		offset += new_page_offset;
		p += new_page_offset;
		length -= new_page_offset;
	}

	pages = length / mtd->writesize;
	while (pages) {
#ifdef __CACHE_PAGE__
		nand_write_multi_pages_siu(mtd, offset, p,
				pages * mtd->writesize, length_write);
		p += pages * mtd->writesize;
		offset += pages * mtd->writesize;
		length -= pages * mtd->writesize;
		pages -= pages;
#else
		nand_write_one_page_siu(mtd, offset, p, mtd->writesize, length_write);
		offset += mtd->writesize;
		p += mtd->writesize;
		length -= mtd->writesize;
		pages--;
#endif
	}

	remainder = length % mtd->writesize;
	if (remainder) {
		nand_write_one_page_siu(mtd, offset, p, remainder, length_write);
		length -= remainder;
	}

	return length;
}

int nand_write_multi_pages_dma(struct mtd_info *mtd, uint64_t offset,
		const uintptr_t buff, uint32_t length, uint32_t *length_write)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t dma_start = 1, pages, remainder;
	uintptr_t p = buff;

	pr_info("%s: address = 0x%lx, nand offset = 0x%llx, length = 0x%x\n",
		 __func__, buff, offset, length);

	nand_address_set(mtd, offset);

	if (ts_nfc->mlun_en) {
		if (offset >= mtd->erasesize * chip->onfi_params.blocks_per_lun)
			nfc_mlun_select_index(1);
		else
			nfc_mlun_select_index(0);

	}

	nfc_wait_dcu_ready(mtd);
	get_nand_write_protect_state(&ts_nfc->mem_ctrl);

	pages = length / mtd->writesize;
	while (pages) {
		/* set dma addr */
		nfc_setup_dma(mtd, (uint32_t)(uintptr_t)p, /*1*/ length,
			DMA_SFRS_MANAGED_MODE, UNKNOW_TRANS_INCREMENT, dma_start);

		nfc_exec_onfi_command_dma(ONFI_CMD_PROGRAM_PAGE_CACHE);
		//wait for CMD_FIFO_EMPTY MEM0_ST
		//nfc_cmd_fifo_wait_empty();
		//nfc_wait_dma_ready();
		//nfc_wait_nand_ready(mtd, chip);
		if (ts_nfc->interrupt_en)
			nfc_wait_cmd_transfer_complate_intr_flag();

		nfc_get_command_status(chip, &ts_nfc->nand_status);
		if (ts_nfc->nand_status & NAND_STATUS_CACHE_LAST_FAIL) {
			pr_err("%s:last command failed!\n", __func__);
			//TODO
		}
		if (ts_nfc->nand_status & NAND_STATUS_FAIL) {
			pr_err("%s:command failed!\n", __func__);
			//TODO
		}

		//dma_start = 0;
		length -= mtd->writesize; //debug test
		*length_write += mtd->writesize;
		p += mtd->writesize;
		pages--;
	}

	remainder = length % mtd->writesize;
	if (remainder) {
		/* set dma addr */
		nfc_setup_dma(mtd, (uint32_t)(uintptr_t)p, /*1*/ length,
			DMA_SFRS_MANAGED_MODE, UNKNOW_TRANS_INCREMENT, dma_start);

		nfc_exec_onfi_command_dma(ONFI_CMD_PROGRAM_PAGE_CACHE);
		//wait for CMD_FIFO_EMPTY MEM0_ST
		//nfc_cmd_fifo_wait_empty();
		//nfc_wait_dma_ready();
		//nfc_wait_nand_ready(mtd, chip);
		if (ts_nfc->interrupt_en)
			nfc_wait_cmd_transfer_complate_intr_flag();

		nfc_get_command_status(chip, &ts_nfc->nand_status);
		if (ts_nfc->nand_status & NAND_STATUS_CACHE_LAST_FAIL) {
			pr_err("%s:last command failed!\n", __func__);
			//TODO
		}
		if (ts_nfc->nand_status & NAND_STATUS_FAIL) {
			pr_err("%s:command failed!\n", __func__);
			//TODO
		}
		//dma_start = 0;
		length -= remainder;	//debug test
		p += remainder;
		*length_write += remainder;
	}
	// 4. write to nand.
	nfc_exec_onfi_command_dma(ONFI_CMD_PROGRAM_PAGE);

	nfc_cmd_fifo_wait_empty();
	nfc_wait_dma_ready();
	nfc_wait_nand_ready(mtd, chip);

	return nfc_check_command_status(chip);
}

int nand_write_pages_dma(struct mtd_info *mtd, uint64_t offset,
		const uintptr_t buff, uint32_t length, uint32_t *length_write)
{
	uint32_t pages, remainder, sum = 0;
	uintptr_t p = buff;

	pr_info("%s: address = 0x%lx, nand offset = 0x%llx, length = 0x%x\n",
		 __func__, buff, offset, length);

	pages = length / mtd->writesize;
	remainder = length % mtd->writesize;

	nand_address_set(mtd, offset);

	while (pages) {
#ifdef __CACHE_PAGE__
		nand_write_multi_pages_dma(mtd, offset, p, pages * mtd->writesize, &sum);
		p += pages * mtd->writesize;
		offset += pages * mtd->writesize;
		pages -= pages;
#else
		nand_write_multi_pages_dma(mtd, offset, p, mtd->writesize, &sum);
		offset += mtd->writesize;
		p += mtd->writesize;
		pages--;
#endif
	}

	if (remainder) {
		nand_write_multi_pages_dma(mtd, offset, p, remainder, &sum);
		offset += remainder;
		p += remainder;
	}

	*length_write = sum;

	if (length != *length_write) {
		pr_err("%s:length=0x%x, length_write=0x%x\n",
				__func__, length, *length_write);
	}

	return sum;
}

int nand_write_one_page_siu_oob_raw(struct mtd_info *mtd,
		uint32_t page, uint64_t offset, const uintptr_t buff, uint32_t length)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	//nfc_set_ecc(false);

	nand_setting_page_size(mtd, length, WRITE_OOB);
	nand_address_set_oob(mtd, page, offset);

	nfc_fifo_flush();
	nfc_exec_onfi_command_siu(ONFI_CMD_PROGRAM_PAGE);

	raw_nand_write_data(mtd, buff, length, TRUE);

	//nfc_cmd_fifo_wait_empty();
	nfc_wait_nand_ready(mtd, chip);
	//nfc_wait_dcu_ready(mtd);

	// *length_write += length;

	//nfc_set_ecc(true);

	return nfc_check_command_status(chip);
}


#endif

int nand_write_blocks(struct mtd_info *mtd, uint64_t offset,
		const uintptr_t buffer, size_t length)
{
#if __ENABLE_WRITE__
	uint32_t length_write = 0;
	uint32_t blocks = 0;
	uint64_t offset1, new_block_offset;
	size_t len = length;
	uintptr_t buf = buffer;

	new_block_offset = mtd->erasesize - (offset % mtd->erasesize);
	new_block_offset = MIN((uint64_t)length, new_block_offset);
	if (0 != (offset % mtd->erasesize)) {
		nand_erase_siu(mtd, offset, new_block_offset, false); //TODO copy back...
		//nand_check_offset_of_block_is_bad(mtd, &offset);  // check current block.
		nand_block_address_update_set(mtd, offset, &offset1);
		if (ts_nfc->dma_en)
			nand_write_pages_dma(mtd, offset1, buf, new_block_offset, &length_write);
		else
			nand_write_pages_siu(mtd, offset1, buf, new_block_offset, &length_write);

		length -= new_block_offset;
		buf += new_block_offset;
		offset += new_block_offset;
	}

	blocks = length / mtd->erasesize;
	while (blocks) {
		nand_erase_siu(mtd, offset, mtd->erasesize, false);
		//nand_check_offset_of_block_is_bad(mtd, &offset);
		nand_block_address_update_set(mtd, offset, &offset1);
		if (ts_nfc->dma_en)
			nand_write_pages_dma(mtd, offset1, buf, mtd->erasesize, &length_write);
		else
			nand_write_pages_siu(mtd, offset1, buf, mtd->erasesize, &length_write);

		length -= mtd->erasesize;
		buf += mtd->erasesize;
		offset += mtd->erasesize;
		blocks--;
	}

	if (length > 0) {
		nand_erase_siu(mtd, offset, length, false); // TODO copyback...
		//nand_check_offset_of_block_is_bad(mtd, &offset);
		nand_block_address_update_set(mtd, offset, &offset1);
		if (ts_nfc->dma_en)
			nand_write_pages_dma(mtd, offset1, buf, length, &length_write);
		else
			nand_write_pages_siu(mtd, offset1, buf, length, &length_write);

		length -= length;
		buf += length;
		offset += length;
	}

	if (len != length_write) {
// #if defined(CONFIG_ARCH_TS_TX536)	
// 		pr_err("%s: lenght=0x%lx write_length=0x%x\n", __func__, len, length_write);
// #else
// 		pr_err("%s: lenght=0x%x write_length=0x%x\n", __func__, len, length_write);
// #endif		
// 		//return -1;
	}
#endif

	return 0;
}

int ts_raw_nand_write(struct mtd_info *mtd, uint32_t offset,
			const uintptr_t buffer, size_t length)
{
	return nand_write_blocks(mtd, offset, buffer, length);
}

static void raw_nand_read_page_data(struct mtd_info *mtd, uint32_t drop_bytes,
		 const uintptr_t buff, unsigned int length, bool use_bus8)
{
	/* The data fifo not suppotr read_8 read_16, not a whole value! */
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);
	uintptr_t data_base = (uintptr_t)&info->regs->fifo_data;
	static uint32_t tmp_int;
	uint32_t offset_align = drop_bytes % 4;
	//uintptr_t p = 0;
	uint8_t *p = (void *)buff;
	static const uint8_t int_size = sizeof(uint32_t);
	bool need_align = FALSE;

	if (use_bus8 && (chip->options & NAND_BUSWIDTH_16))
		nfc_ctrl_set_buswidth(false);

/*
 * addr=0x1000000 offset=0x43e73 len=0x18d=397=396+1 => Read remaining bytes.
 * addr=0x100018d offset=0x44000 len=0x800. 0x18d=397 not 4byte align, =>
 * tmp_buf need align too.
 */
	if ((buff & GENMASK_32(1, 0)) || offset_align) {
		need_align = TRUE;
		pr_debug("%s:%d:need align: addr=0x%lx drop_bytes=0x%x len=0x%x.\n",
					__func__, __LINE__, buff, drop_bytes, length);
	}

	/* data_fifo_check_not_empty indicated only
	 * one ecc block(512-bytes) ready when HW-ECC enable.
	 */
	if (false == nfc_data_fifo_wait_not_empty()) {
		pr_err("%s:%d: remainder 0x%x data not ready!\n",
				__func__, __LINE__, length);
	}

	/* control offset align, drop needless data. */
	if ((length >= sizeof(uint32_t)) &&
			offset_align && nfc_data_fifo_not_empty()) {
		tmp_int = mmio_read_32(data_base) >> (8 * offset_align);
		memcpy((uint8_t *)p, &tmp_int, sizeof(uint32_t) - offset_align);
		//pr_debug(" 0x%08x ", *(uint32_t *)p);
		p += (sizeof(uint32_t) - offset_align);
		length -= (sizeof(uint32_t) - offset_align);
	}

	/* 32bit aligned */
	if (need_align) {
		while (length >= int_size) {
			tmp_int = mmio_read_32(data_base);
			memcpy((uint8_t *)p, &tmp_int, int_size);
			p += int_size;
			length -= int_size;
		}
	} else {
		while (length >= int_size) {
			*(uint32_t *)p = mmio_read_32(data_base);
			p += int_size;
			length -= int_size;
		}
	}

	/* Read remaining bytes */
	if (length) {
		//nfc_data_fifo_not_empty(); //less then 4byte, don't check.
		tmp_int = mmio_read_32(data_base);
		memcpy((uint8_t *)p, &tmp_int, length);
		pr_debug(" 0x%x ", *(uint32_t *)p);
		length -= length;
	}

	if (use_bus8 && (chip->options & NAND_BUSWIDTH_16)) {
		/* Reconfigure bus width to 16-bit */
		nfc_ctrl_set_buswidth(true);
	}
	p = NULL;
}

static void raw_nand_read_param_data(struct mtd_info *mtd,
		const uintptr_t buff, unsigned int length, bool use_bus8)
{
	 raw_nand_read_page_data(mtd, 0, buff, length, TRUE);
}

static void raw_nand_drop_words(struct mtd_info *mtd, uint64_t offset,
		 unsigned int length, bool use_bus8, bool drop_tail)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);
	uintptr_t data_base = (uintptr_t)&info->regs->fifo_data;
	static const uint8_t int_size = sizeof(uint32_t);
	int max_drop_size = mtd->writesize + mtd->oobsize;

	if (use_bus8 && (chip->options & NAND_BUSWIDTH_16))
		nfc_ctrl_set_buswidth(false);


	/* data_fifo_check_not_empty indicated only
	 * one ecc block(512-bytes) ready when HW-ECC enable.
	 */
	if (false == nfc_data_fifo_wait_not_empty())
		pr_err("%s:%d: remainder 0x%x data not ready!\n", __func__, __LINE__, length);


	/* The nfc controller 32bit align , u16 and u8 read will trigger error! */
	offset = offset - offset % 4;
	length = length - length % 4;
	/* 32bit aligned */
	while (length >= int_size) {
		mmio_read_32(data_base);
		length -= int_size;
		if (0 == (length & 0xffe0)) {
			if (false == nfc_data_fifo_not_empty())
				break;
		}
	}

	/* Read remaining bytes disable, when drop head!*/
	if (drop_tail) {
		/* drop tail, drop remaining data. */
		while (nfc_data_fifo_not_empty() && max_drop_size) {
			mmio_read_32(data_base);
			max_drop_size -= 4;
		}
	}

	if (use_bus8 && (chip->options & NAND_BUSWIDTH_16)) {
		/* Reconfigure bus width to 16-bit */
		nfc_ctrl_set_buswidth(true);
	}
}

void rawnand_read_one_page_cmd(struct mtd_info *mtd, uint64_t offset)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	/* if offset val is not a page start,
	 * then set offset to page start.(HWECC required)
	 */
	nand_address_col_set(mtd, 0);

	if (ts_nfc->row_addr_auto_increase == FALSE)
		nand_address_row_set(mtd, offset);


	nand_setting_page_size(mtd, mtd->writesize, false); //hw ecc ok.
	nfc_exec_onfi_command_siu(ONFI_CMD_READ_PAGE);
	/* first read device need more than tR time to read, otherwise read error.
	 *micro nand (and other nand) first read 2k, CE is low, the controller
	 *think device is busy even though RB_line is hight.
	 *so, this function only delay 100us and it don't send 0x70 cmd.
	 */
	rawnand_wait_command_complete(mtd, 10,
		MAX(chip->onfi_params.t_r * 2, (uint16_t)TIME_TR * 4), false);
	//nfc_wait_nand_ready(mtd, chip);
}

void rawnand_read_page_from_cache_cmd(struct mtd_info *mtd,
		uint64_t offset, uint32_t length)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t remainder = (uint64_t)offset % mtd->writesize;
	/* if offset val is not a page start,
	 * then set offset to page start.(HWECC required)
	 */
	if (remainder)
		nand_address_col_set(mtd, remainder);

	if (ts_nfc->row_addr_auto_increase == FALSE)
		nand_address_row_set(mtd, offset);

	nand_setting_page_size(mtd, length, false); //0x84 //max:32K
	//nand_change_read_column_cmd(mtd, offset + remainder, (uintptr_t)p, length);
	if (ts_nfc->mlun_en) {
		/* if mlun enabled */
		nand_address_set(mtd, offset);
		nfc_exec_onfi_command_siu(ONFI_CMD_SELECT_CACHE_REGISTER);
	} else {
		nfc_exec_onfi_command_siu(ONFI_CMD_CHANGE_READ_COLUMN);
	}
	rawnand_wait_command_complete(mtd, 10,
		MAX(chip->onfi_params.t_r, (uint16_t)TIME_TR), false);
}

int nand_read_one_page_siu(struct mtd_info *mtd, uint64_t offset,
		const uintptr_t buff, uint32_t length, uint32_t *length_read)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	/*use time 2.5ms + 3.8ms ecc enabled. clk=24MHz 400k/s*/
	uintptr_t p = buff;
	uint32_t remainder = (uint64_t)offset % mtd->writesize;

	pr_debug("%s: addr=0x%lx offset=0x%llx len=0x%x.\n",
		__func__, buff, offset, length);
	asm("isb");
	rawnand_read_one_page_cmd(mtd, offset);
	if (remainder) {
		nfc_data_fifo_wait_not_empty();
		raw_nand_drop_words(mtd, 0, mtd->writesize + mtd->oobsize, TRUE, true);
		if (false == nfc_data_fifo_wait_empty(false))
			pr_err("%s:%d data fifo not empty\n", __func__, __LINE__);

		rawnand_read_page_from_cache_cmd(mtd, offset, length);
	}

	/* data_fifo_check_not_empty indicated only
	 * one ecc block(512-bytes) ready when HW-ECC enable.
	 */
	nfc_data_fifo_wait_not_empty();

	raw_nand_read_page_data(mtd, 0, p, length, TRUE);
	*length_read += length;

	if ((length != mtd->writesize || remainder)) {
		if (nfc_data_fifo_not_empty()) {
			raw_nand_drop_words(mtd, remainder + length,
				mtd->writesize - (remainder + length), TRUE, true);
		}

		if (nfc_data_fifo_not_empty()) {
			pr_warn("%s:%d:flush fifo: addr=0x%lx offset=0x%llx len=0x%x.\n",
				__func__, __LINE__, buff, offset, length);
			nfc_fifo_flush();
		}
	}

	if (false == nfc_data_fifo_wait_empty(false))
		pr_err("%s:%d data fifo not empty\n", __func__, __LINE__);

	nfc_wait_nand_ready(mtd, chip);
	if (nfc_check_ecc_state() != 0) {
		pr_err("%s: addr=0x%lx offset=0x%llx len=0x%x ecc error!.\n",
			__func__, buff, offset, length);
	}

	nfc_wait_dcu_ready(mtd);

	if (length < mtd->writesize || remainder) {
		/* reback offset. auto increment row address need reback.*/
		/* if nfc not ready, clo address set will failed.*/
		nand_address_col_set(mtd, 0);
		nand_setting_page_size(mtd, mtd->writesize, 0); //0x84 //max:32K
	}
	nfc_cmd_fifo_wait_empty();

	return length;
}

int nand_read_one_page_siu_raw(struct mtd_info *mtd, uint32_t offset,
		uintptr_t buff, uint32_t length, uint32_t *length_read)
{
	nfc_set_ecc(false);
	nand_read_one_page_siu(mtd, offset, buff, length, length_read);
	//nfc_set_ecc(true);

	return length;
}

int nand_read_one_page_siu_oob_raw(struct mtd_info *mtd, uint32_t page,
		 uint64_t col_offset, const uintptr_t buff, uint32_t length)
{
	/*use time 50us + 40us */
	struct nand_chip *chip = mtd_to_nand(mtd);
	uintptr_t p = buff;

	//nfc_set_ecc(false);

	nand_setting_page_size(mtd, length, WRITE_OOB);
	nand_address_set_oob(mtd, page, col_offset);

	//nfc_fifo_flush();
	nfc_exec_onfi_command_siu(ONFI_CMD_READ_PAGE);

	nfc_wait_nand_ready(mtd, chip);
	raw_nand_read_param_data(mtd, p, length, TRUE);

	if (nfc_data_fifo_not_empty())
		raw_nand_drop_words(mtd, 0, mtd->writesize - length, true, true);


	//nfc_set_ecc(true);

	return 0;
}

int nand_read_multi_pages_siu(struct mtd_info *mtd, uint64_t offset,
		 const uintptr_t buff, uint32_t length, uint32_t *length_read)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint32_t i = 0, j = 0, len = 0, tmp;
	uintptr_t p = buff;
	/*remainder page don't care in this function.*/
	pr_debug("%s: addr=0x%lx offset=0x%llx len=0x%x.\n",
			__func__, buff, offset, length);

	nand_setting_page_size(mtd, mtd->writesize, 0);
	nand_address_set(mtd, offset);

	//nfc_wait_dcu_ready(mtd);
	nfc_wait_nand_ready(mtd, chip);

	get_nand_write_protect_state(&ts_nfc->mem_ctrl);

	pr_debug("0x%08x read start.\n", nand_get_current_position(mtd));
	nfc_exec_onfi_command_siu(ONFI_CMD_READ_PAGE);
	nfc_data_fifo_wait_not_empty();
	/*has a bug, cache first page delect, repeat for second page.*/
	raw_nand_drop_words(mtd, 0, mtd->writesize, TRUE, true);

	//nfc_wait_nand_ready(mtd, chip); //check ready will occur error.
	for (j = 0; j < (length / mtd->writesize) - 1; j++) {
		nfc_exec_onfi_command_siu(ONFI_CMD_READ_PAGE_CACHE);
		//check ready will occur error.
		rawnand_wait_command_complete(mtd, 10,
			MAX(chip->onfi_params.t_r * 2, (uint16_t)TIME_TR * 4), false);
		nfc_data_fifo_wait_not_empty();

		for (i = 0; i < mtd->writesize / 4; i++) {
			nfc_data_fifo_not_empty();
			tmp = mmio_read_32((uintptr_t)&nfc_regs->fifo_data);
			memcpy((void *)p, &tmp, sizeof(uint32_t));
			//pr_debug("rd:0x%x:0x%08x ", p, tmp);
			p += 4;
		}
		if ((ts_nfc->row_addr_auto_increase == FALSE) && FALSE) {
			//not useful.
			//nfc_wait_dcu_ready(mtd);
			offset += (j + 1) * mtd->writesize;
			nand_address_set(mtd, offset);
		}
		len += mtd->writesize;
		nfc_check_ecc_state();
	}

	nfc_exec_onfi_command_siu(ONFI_CMD_READ_PAGE_CACHE_LAST);
	//check ready occur error.
	rawnand_wait_command_complete(mtd, 10,
		MAX(chip->onfi_params.t_r * 2, (uint16_t)TIME_TR * 4), false);
	nfc_data_fifo_wait_not_empty();

	for (i = 0; i < mtd->writesize / 4; i++) {
		nfc_data_fifo_not_empty();
		tmp = mmio_read_32((uintptr_t)&nfc_regs->fifo_data);
		memcpy((void *)p, &tmp, sizeof(uint32_t));
		//pr_debug("rd:0x%x:0x%08x ", p, tmp);
		p += 4;
	}
	pr_debug("\n");
	len += mtd->writesize;
	*length_read += len;
	nfc_check_ecc_state();

	nfc_wait_nand_ready(mtd, chip);
	pr_debug("0x%08x read end.\n", nand_get_current_position(mtd));
	nfc_wait_dcu_ready(mtd);

	return len < length ? len : length;
}

int nand_read_pages_siu(struct mtd_info *mtd, uint64_t offset,
		const uintptr_t buff, uint32_t length, uint32_t *length_read)
{
	uint32_t pages, remainder, new_page_offset = 0;
	uintptr_t p = buff;

	pr_debug("%s: address = 0x%lx, nand offset = 0x%llx, length = 0x%x\n",
		 __func__, buff, offset, length);

	nand_address_set(mtd, offset);

	remainder = offset % mtd->writesize;
	new_page_offset = mtd->writesize - remainder;
	if (new_page_offset && (new_page_offset < mtd->writesize)
		&& (new_page_offset <= length)) {
		if (ts_nfc->hw_ecc_en)
			nand_read_one_page_siu(mtd, offset, p, new_page_offset, length_read);
		else
			nand_read_one_page_siu_raw(mtd, offset, p, new_page_offset, length_read);

		offset += new_page_offset;
		p += new_page_offset;
		length -= new_page_offset;
	}

	pages = length / mtd->writesize;
	while (pages) {
#ifdef __CACHE_PAGE__
		nand_read_multi_pages_siu(mtd, offset, p, pages * mtd->writesize, length_read);
		p += pages * mtd->writesize;
		offset += pages * mtd->writesize;
		length -= pages * mtd->writesize;
		pages -= pages;
#else
		if (ts_nfc->hw_ecc_en)
			nand_read_one_page_siu(mtd, offset, p, mtd->writesize, length_read);
		else
			nand_read_one_page_siu_raw(mtd, offset, p, mtd->writesize, length_read);

		offset += mtd->writesize;
		p += mtd->writesize;
		length -= mtd->writesize;
		pages--;
#endif
	}

	remainder = length % mtd->writesize;
	if (remainder) {
		if (ts_nfc->hw_ecc_en)
			nand_read_one_page_siu(mtd, offset, p, remainder, length_read);
		else
			nand_read_one_page_siu_raw(mtd, offset, p, remainder, length_read);

		length -= remainder;
	}

	return length;
}

int nand_read_one_page_dma(struct mtd_info *mtd, uint64_t offset,
				const uintptr_t buff, uint32_t length, uint32_t *length_read)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	/* offset position is not a page start, dma function ok ? */
	uint32_t cnt = 0, dma_start = 1;
	uintptr_t p = buff;

	length = mtd->writesize;

	pr_info("%s: address = 0x%lx, nand offset = 0x%llx, length = 0x%x\n",
		 __func__, buff, offset, length);

	nand_address_set(mtd, offset);
	nand_setting_page_size(mtd, mtd->writesize, 0); //0x84 //max:32K

	if (ts_nfc->mlun_en) {
		if (offset >= mtd->erasesize * chip->onfi_params.blocks_per_lun)
			nfc_mlun_select_index(1);
		else
			nfc_mlun_select_index(0);
	}

	//nfc_wait_dcu_ready(mtd);
	get_nand_write_protect_state(&ts_nfc->mem_ctrl);
	/* must change quickly when send read command, to avid rewrite src buffer. */
	mmio_write_32((uintptr_t)&nfc_regs->dma_addr_l, (uint32_t)(uintptr_t)p);

	/* read from nand. */
	nfc_exec_onfi_command_dma(ONFI_CMD_READ_PAGE);
	//wait for CMD_FIFO_EMPTY DCU_READY
	//nfc_wait_nand_ready(mtd, chip);
	nfc_wait_dma_ready();

	/* set dma addr */
	nfc_setup_dma(mtd, (uint32_t)(uintptr_t)p, /*1*/ length,
		DMA_SFRS_MANAGED_MODE, UNKNOW_TRANS_INCREMENT, dma_start);

	if (ts_nfc->interrupt_en)
		nfc_wait_cmd_transfer_complate_intr_flag();


	nfc_wait_dma_ready();
	nfc_setup_dma(mtd, (uint32_t)(uintptr_t)p, /*1*/ length,
		 DMA_SFRS_MANAGED_MODE, UNKNOW_TRANS_INCREMENT, 0);
	nfc_wait_nand_ready(mtd, chip);
	//nfc_cmd_fifo_wait_empty();
	//nfc_wait_dma_ready();
	length -= mtd->writesize;	//debug test
	length_read += mtd->writesize;
	p += mtd->writesize;
	//nfc_wait_dcu_ready(mtd);

	while (cnt < 100)
		cnt++;

	nfc_cmd_fifo_wait_empty();
	//nfc_get_command_status(chip, &ts_nfc->nand_status);

	return length;
}

int nand_read_multi_pages_dma(struct mtd_info *mtd, uint64_t offset,
			const uintptr_t buff, uint32_t length, uint32_t *length_read)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	/* offset position is not a page start, dma function ok ? */
	uint32_t cnt = 0, dma_start = 1, pages, remainder;
	uintptr_t p = buff;

	pr_info("%s: address = 0x%lx, nand offset = 0x%llx, length = 0x%x\n",
		 __func__, buff, offset, length);

	nand_address_set(mtd, offset);
	nand_setting_page_size(mtd, mtd->writesize, 0); //0x84 //max:32K

	if (ts_nfc->mlun_en) {
		if (offset >= mtd->erasesize * chip->onfi_params.blocks_per_lun)
			nfc_mlun_select_index(1);
		else
			nfc_mlun_select_index(0);

	}

	nfc_wait_dcu_ready(mtd);
	get_nand_write_protect_state(&ts_nfc->mem_ctrl);
	/* must change quickly when send read command, to avid rewrite src buffer. */
	mmio_write_32((uintptr_t)&nfc_regs->dma_addr_l, (uint32_t)(uintptr_t)p);

	/* read from nand. */
	nfc_exec_onfi_command_dma(ONFI_CMD_READ_PAGE);
	//wait for CMD_FIFO_EMPTY DCU_READY
	nfc_wait_nand_ready(mtd, chip);
	nfc_wait_dma_ready();

	pages = length / mtd->writesize;
	while (pages) {
		/* set dma addr */
		nfc_setup_dma(mtd, (uint32_t)(uintptr_t)p, /*1*/ length,
			 DMA_SFRS_MANAGED_MODE, UNKNOW_TRANS_INCREMENT, dma_start);

		nfc_exec_onfi_command_dma(ONFI_CMD_READ_PAGE_CACHE);
		//wait for CMD_FIFO_EMPTY DCU_READY
		if (ts_nfc->interrupt_en)
			nfc_wait_cmd_transfer_complate_intr_flag();

		//nfc_cmd_fifo_wait_empty();
		//nfc_wait_dma_ready();
		length -= mtd->writesize;	//debug test
		length_read += mtd->writesize;
		p += mtd->writesize;
		pages--;
	}

	remainder = length % mtd->writesize;
	if (remainder) {
		/* set dma addr */
		nfc_setup_dma(mtd, (uint32_t)(uintptr_t)p, /*1*/ length,
			 DMA_SFRS_MANAGED_MODE, UNKNOW_TRANS_INCREMENT, dma_start);
		nfc_exec_onfi_command_dma(ONFI_CMD_READ_PAGE_CACHE);
		//wait for CMD_FIFO_EMPTY DCU_READY

		if (ts_nfc->interrupt_en)
			nfc_wait_cmd_transfer_complate_intr_flag();

		length -= remainder;	//debug test.
		length_read += remainder;
		p += mtd->writesize;
	}

	nfc_exec_onfi_command_dma(ONFI_CMD_READ_PAGE_CACHE_LAST);
	nfc_cmd_fifo_wait_empty();
	//nfc_wait_dcu_ready(mtd);
	nfc_wait_nand_ready(mtd, chip);
	nfc_wait_dma_ready();

	while (cnt < 100)
		cnt++;

	//nfc_get_command_status(chip, &ts_nfc->nand_status);

	return length;
}

int nand_read_pages_dma(struct mtd_info *mtd, uint64_t offset,
	 const uintptr_t buff, uint32_t length, uint32_t *length_read)
{
	uint32_t pages, remainder, new_page_offset = 0;
	uintptr_t p = buff;

	pr_info("%s: address = 0x%lx, nand offset = 0x%llx, length = 0x%x\n",
		 __func__, buff, offset, length);

	nand_address_set(mtd, offset);

	remainder = offset % mtd->writesize;
	new_page_offset = mtd->writesize - remainder;
	if (new_page_offset && (new_page_offset < mtd->writesize)
		 && (new_page_offset <= length)) {//TODO CHANGE_COL
		//nand_read_multi_pages_dma(mtd, offset, p, new_page_offset, length_read);
		nand_read_one_page_dma(mtd, offset, p, new_page_offset, length_read);
		offset += new_page_offset;
		p += new_page_offset;
		length -= new_page_offset;
	}

	pages = length / mtd->writesize;
	while (pages) {
#ifdef __CACHE_PAGE__
		nand_read_multi_pages_dma(mtd, offset, p, pages * mtd->writesize, length_read);
		p += pages * mtd->writesize;
		offset += pages * mtd->writesize;
		length -= pages * mtd->writesize;
		pages -= pages;
#else
		//nand_read_multi_pages_dma(mtd, offset, p, mtd->writesize, length_read);
		nand_read_one_page_dma(mtd, offset, p, mtd->writesize, length_read);
		offset += mtd->writesize;
		p += mtd->writesize;
		length -= mtd->writesize;
		pages--;
#endif
	}

	remainder = length % mtd->writesize;
	if (remainder) {
		//nand_read_multi_pages_dma(mtd, offset, p, remainder, length_read);
		nand_read_one_page_dma(mtd, offset, p, remainder, length_read);
		length -= remainder;
	}

	return length;
}

int nand_read_blocks(struct mtd_info *mtd, uint64_t offset,
		const uintptr_t buffer, size_t length, size_t *outlength)
{
	uint32_t blocks = 0;
	uint64_t offset1, new_block_offset;
	size_t len = length;
	uintptr_t buf = buffer;

	/* init the read length.*/
	*outlength = 0;
	new_block_offset = mtd->erasesize - (offset % mtd->erasesize);
	new_block_offset = MIN((uint64_t)length, new_block_offset);
	if (0 != (offset % mtd->erasesize)) {
		//nand_check_offset_of_block_is_bad(mtd, &offset);  // check current block.
		nand_block_address_update_set(mtd, offset, &offset1);
		if (ts_nfc->dma_en)
			nand_read_pages_dma(mtd, offset1, buf, new_block_offset, (uint32_t *)outlength);
		else
			nand_read_pages_siu(mtd, offset1, buf, new_block_offset, (uint32_t *)outlength);

		length -= new_block_offset;
		buf += new_block_offset;
		offset += new_block_offset;
	}

	blocks = length / mtd->erasesize;
	while (blocks) {
		//nand_check_offset_of_block_is_bad(mtd, &offset);
		nand_block_address_update_set(mtd, offset, &offset1);
		if (ts_nfc->dma_en)
			nand_read_pages_dma(mtd, offset1, buf, mtd->erasesize, (uint32_t *)outlength);
		else
			nand_read_pages_siu(mtd, offset1, buf, mtd->erasesize, (uint32_t *)outlength);

		length -= mtd->erasesize;
		buf += mtd->erasesize;
		offset += mtd->erasesize;
		blocks--;
	}

	if (length) {
		nand_block_address_update_set(mtd, offset, &offset1);
		if (ts_nfc->dma_en)
			nand_read_pages_dma(mtd, offset1, buf, length, (uint32_t *)outlength);
		else
			nand_read_pages_siu(mtd, offset1, buf, length, (uint32_t *)outlength);

		length -= length;
		buf += length;
		offset += length;
	}
	buf = 0;

	if (len != *outlength) {
// #if defined(CONFIG_ARCH_TS_TX536)	
// 		pr_err("%s: lenght=0x%lx outlength=0x%lx\n", __func__, len, *outlength);
// #else
// 		pr_err("%s: lenght=0x%x outlength=0x%x\n", __func__, len, *outlength);
// #endif		
		//return -1;
	}

	return 0;
}

int ts_raw_nand_read(struct mtd_info *mtd, uint32_t offset,
		const uintptr_t buffer, size_t length, size_t *outlength)
{
	return nand_read_blocks(mtd, offset, buffer, length, outlength);
}

int ts_mtd_read_page(struct mtd_info *mtd, unsigned int page, uintptr_t buffer)
{
	size_t length_read = 0;

	return nand_read_blocks(mtd, page * mtd->writesize, buffer, mtd->writesize, &length_read);
}

#if NAND_ONFI_DETECT
static int nand_read_data(struct mtd_info *mtd,
		uintptr_t data, unsigned int length, bool use_8bit)
{
	raw_nand_read_page_data(mtd, 0, data, length, TRUE);

	return 0;
}

int nand_change_read_column_cmd(struct mtd_info *mtd,
	 unsigned int offset, uintptr_t buffer, unsigned int len)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	int ret;

	nand_address_col_set(mtd, offset);

	ret = nfc_exec_onfi_command_siu(ONFI_CMD_CHANGE_READ_COLUMN);
	if (ret != 0)
		return ret;

	nfc_wait_nand_ready(mtd, chip);

	ret = nand_read_data(mtd, buffer, len, TRUE);
	nfc_fifo_flush();
	return ret;
}


int nand_write_oob_page_cmd(struct mtd_info *mtd, unsigned int page, unsigned int offset,
					   uintptr_t buffer, unsigned int len)
{
	int ret = 0;
#if __ENABLE_WRITE__
	nfc_set_ecc(false);
	ret = nand_write_one_page_siu_oob_raw(mtd, page, offset, buffer, len);
#endif

	return ret;
}

int nand_read_oob_page_cmd(struct mtd_info *mtd, unsigned int page,
		unsigned int col_offset, uintptr_t buffer, unsigned int len)
{
	int ret = 0;

	nfc_set_ecc(false);
	//pr_debug(">%s page %u offset %u buffer 0x%lx\n", __func__, page, offset, buffer);
	ret = nand_read_one_page_siu_oob_raw(mtd, page, col_offset, buffer, len);
	return ret;
}

static uint16_t nand_check_crc(uint16_t crc,
		uint8_t *data_in, unsigned int data_len)
{
	uint32_t i;
	uint32_t j;
	uint32_t bit;

	for (i = 0U; i < data_len; i++) {
		uint8_t cur_param = *data_in++;

		for (j = BIT(7); j != 0U; j >>= 1) {
			bit = crc & BIT(15);
			crc <<= 1;

			if ((cur_param & j) != 0U)
				bit ^= BIT(15);


			if (bit != 0U)
				crc ^= CRC_POLYNOM;

		}

		crc &= GENMASK(15, 0);
	}

	return crc;
}

static int nand_read_id(struct mtd_info *mtd,
		uint8_t addr, const uintptr_t id, unsigned int size)
{
	int ret;

	nfc_set_ecc(false);
	nfc_fifo_flush();
	nand_address_set(mtd, addr);
	nand_setting_page_size(mtd, size, true);

	ret = nfc_exec_onfi_command_siu(ONFI_CMD_READ_ID);
	if (ret != 0)
		return ret;

	raw_nand_read_param_data(mtd, id, size, true);
	if (nfc_data_fifo_not_empty()) {
		nfc_fifo_flush();
		return -1;
		//raw_nand_drop_words(mtd, size, mtd->writesize - size, true, true);
	}
	nfc_set_ecc(true);

	return ret;
}

/**
 * onfi_fill_interface_config - Initialize an interface config from a given
 *                              ONFI mode
 * @chip: The NAND chip
 * @iface: The interface configuration to fill
 * @type: The interface type
 * @timing_mode: The ONFI timing mode
 */
void onfi_fill_interface_config(struct mtd_info *mtd,
		struct nand_data_interface *iface, enum nand_data_interface_type type,
		unsigned int timing_mode)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_onfi_params *onfi = &chip->onfi_params;

	if (type != NAND_SDR_IFACE)
		return;

	if (timing_mode >= ARRAY_SIZE(onfi_sdr_timings))
		return;


	/*
	 * Initialize timings that cannot be deduced from timing mode:
	 * tPROG, tBERS, tR and tCCS.
	 * These information are part of the ONFI parameter page.
	 */
	if (onfi->t_r) {
		struct nand_sdr_timings *timings = &iface->timings.sdr;

		/* microseconds -> picoseconds */
		timings->tPROG_max = 1000000ULL * onfi->t_prog;
		timings->tBERS_max = 1000000ULL * onfi->t_bers;
		timings->tR_max = 1000000ULL * onfi->t_r;

		/* nanoseconds -> picoseconds */
		timings->tCCS_min = 1000UL * onfi->t_ccs;
	}
}

/**
 * nand_get_features_op - Do a GET FEATURES operation
 * @chip: The NAND chip
 * @feature: feature id
 * @data: 4 bytes of data
 *
 * This function sends a GET FEATURES command and waits for the NAND to be
 * ready before returning.
 * This function does not select/unselect the CS line.
 *
 * Returns 0 on success, a negative error code otherwise.
 */
static int nand_get_features(struct mtd_info *mtd, struct nand_chip *chip,
			int feature, uint8_t *data)
{
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);
	uintptr_t data_base = (uintptr_t)&info->regs->fifo_data;

	nfc_set_ecc(false);

	nand_setting_page_size(mtd, 4, WRITE_OOB);
	nand_address_col_set(mtd, feature);

	nfc_exec_onfi_command_siu(ONFI_CMD_GET_FEATURES);

	if (nfc_data_fifo_not_empty())
		*data = (uint8_t) mmio_read_32(data_base);


	nfc_wait_nand_ready(mtd, chip);

	nfc_set_ecc(true);

	return 0;
}

static int nand_set_features(struct mtd_info *mtd, struct nand_chip *chip,
			int feature, uint8_t *data)
{
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);
	uintptr_t data_base = (uintptr_t)&info->regs->fifo_data;

	nfc_set_ecc(false);

	nand_setting_page_size(mtd, 4, WRITE_OOB);
	nand_address_col_set(mtd, feature);

	nfc_exec_onfi_command_siu(ONFI_CMD_SET_FEATURES);

	if (nfc_data_fifo_not_full())
		mmio_write_32(data_base, *(uint32_t *)data);


	nfc_wait_nand_ready(mtd, chip);

	nfc_set_ecc(true);

	return 0;
}

static void nfc_nand_setup_timing(struct mtd_info *mtd, int chipnr,
				    const struct nand_data_interface *iface)

{
	nfc_update_timings(mtd, 0, iface);
}

static void update_data_interface(struct mtd_info *mtd, struct nand_chip *chip)
{
	uint8_t i, timing_mode = 0;

	nand_get_features(mtd, chip, ONFI_FETURE_TIMING_MODE_ADDR, &timing_mode);
	for (i = 0; i < 6; i++) {
		if (chip->onfi_params.async_timing_mode & BIT(i))
			timing_mode = i;

	}
	nand_set_features(mtd, chip, ONFI_FETURE_TIMING_MODE_ADDR, &timing_mode);
	chip->data_interface = ts_nfc->iface = &onfi_sdr_timings[timing_mode];
	onfi_fill_interface_config(mtd, ts_nfc->iface, NAND_SDR_IFACE, timing_mode);
}

static int nand_read_param_page(struct mtd_info *mtd,
		const struct nand_onfi_params *page, uint8_t addr, uint32_t len)
{
	/* use time 320us +1.3ms */
	struct nand_chip *chip = mtd_to_nand(mtd);
	//uint8_t addr = 0U;
	int ret = 0, i = 0;
	uint16_t crc[3] = {0, 1, 2};
	int val;

	nfc_set_ecc(false);
	/* Even though the nand device ready, nfc dont't send 0x70/0x78 command,
	 * but not reset the device, the 0xec command has error sometimes.
	 */
	nand_device_reset(mtd, 0);
	nand_address_set(mtd, addr);
	nand_setting_page_size(mtd, len, true);
	nfc_wait_dcu_ready(mtd);
	nfc_wait_nand_ready(mtd, chip);
	asm("isb");
	nfc_exec_onfi_command_siu(ONFI_CMD_READ_PARAMETER_PAGE);
	nfc_wait_nand_ready(mtd, chip);

	raw_nand_read_param_data(mtd, (uintptr_t)page, len, true);

	if (nfc_data_fifo_not_empty())
		raw_nand_drop_words(mtd, sizeof(*page), mtd->writesize - len, true, true);


	if (strncmp((char *)&page->sig, "ONFI", 4) != 0) {
		pr_warn("Error ONFI detection sig=0x%x\n", *(uint32_t *)page->sig);
		ret = -EINVAL;
	}

	crc[0] = nand_check_crc(CRC_INIT_VALUE, (uint8_t *)page, 254U);
	if (crc[0] != page->crc) {
		pr_warn("Error reading param crc=0x%x calc crc=0x%x\n",
			page->crc, crc[0]);
		ret = -EINVAL;
	}
	//ret = -1;//test
	while (ret != 0 && i < 3) {
		nfc_fifo_flush();
		nand_change_read_column_cmd(mtd, i++ * 256,
				 (uintptr_t)page, sizeof(*page));
		if (nfc_data_fifo_not_empty()) {
			raw_nand_drop_words(mtd, sizeof(*page),
				 mtd->writesize - sizeof(*page), true, true);
		}
		if (strncmp((char *)&page->sig, "ONFI", 4) != 0) {
			pr_warn("Error ONFI detection sig=0x%x\n", *(uint32_t *)page->sig);
			continue;
		}

		crc[i] = nand_check_crc(CRC_INIT_VALUE, (uint8_t *)page, 254U);
		if (crc[i] != page->crc) {
			pr_warn("Error reading param crc=0x%x calc crc=0x%x\n",
				 page->crc, crc[i]);
			ret = -EINVAL;
		} else {
			ret = 0;
			break;
		}
		if ((crc[0] == crc[1]) || (crc[1] == crc[2]) || (crc[0] == crc[2])) {
			pr_debug("reading param %d crc=0x%x same.\n", i, crc[i]);
			ret = 0;
			break;
		}
	}

	if (ret) {
		/* disable hw ecc. */
		nfc_set_ecc(false);
		ts_nfc->hw_ecc_en = false;
		goto err_out;
	} else {
		memcpy((void *)&chip->onfi_params,
			 (void *)page, sizeof(chip->onfi_params));
	}

	/* Check version */
	val = le16_to_cpu(page->revision);
	if (val & (1 << 5))
		chip->onfi_version = 23;
	else if (val & (1 << 4))
		chip->onfi_version = 22;
	else if (val & (1 << 3))
		chip->onfi_version = 21;
	else if (val & (1 << 2))
		chip->onfi_version = 20;
	else if (val & (1 << 1))
		chip->onfi_version = 10;

	if (!chip->onfi_version) {
		pr_info("unsupported ONFI version: %d\n", val);
		ret = -EINVAL;
	}
#if defined(__KERNEL__)
	if (!mtd->name)
		mtd->name = page->model;
#endif

	mtd->writesize = le32_to_cpu(page->byte_per_page);
	mtd->writesize_shift = ffs(mtd->writesize - 1);

	/*
	 * pages_per_block and blocks_per_lun may not be a power-of-2 size
	 * (don't ask me who thought of this...). MTD assumes that these
	 * dimensions will be power-of-2, so just truncate the remaining area.
	 */
	mtd->erasesize = 1 << (fls(le32_to_cpu(page->pages_per_block)) - 1);
	mtd->erasesize *= mtd->writesize;
	mtd->erasesize_shift = ffs(mtd->erasesize - 1);

	mtd->oobsize = le16_to_cpu(page->spare_bytes_per_page);

	/* See erasesize comment */
	chip->chipsize = 1 << (fls(le32_to_cpu(page->blocks_per_lun)) - 1);
	chip->chipsize *= (uint64_t)mtd->erasesize * page->lun_count;
	chip->bits_per_cell = page->bits_per_cell;

	/* some chip ecc_bits equl 0. */
	if (page->ecc_bits != GENMASK_32(7, 0)) {
		if (page->ecc_bits >= chip->ecc.strength) {
			/*page->ecc_bits first.*/
			chip->ecc.strength = page->ecc_bits;
		} else {
			/* some device ecc_bits equl 0. */
			//TODO...
			//page->ecc_bits = chip->ecc.strength;
		}
	}

	if (page->ecc_bits != 0xff) {
		chip->ecc_strength_ds = page->ecc_bits;
		chip->ecc_step_ds = 512;
	} else if (chip->onfi_version >= 21 &&
		(page->features & ONFI_FEATURE_EXT_PARAM_PAGE)) {
		/* The Extended Parameter Page is supported since ONFI 2.1. */
		//if (nand_flash_detect_ext_param_page(mtd, chip, page))
			//pr_warn("Failed to detect ONFI extended param page\n");
	} else {
		pr_warn("Could not retrieve ONFI ECC requirements\n");
	}


	if ((page->features & ONFI_FEAT_BUS_WIDTH_16)) {
		ts_nfc->buswidth = SZ_16;
		chip->options |= NAND_BUSWIDTH_16;
	} else {
		ts_nfc->buswidth = SZ_8;
	}

	mtd->size = page->pages_per_block *
		page->byte_per_page * page->blocks_per_lun * page->lun_count;

	//UPDATE NFC

	update_data_interface(mtd, chip);

	nfc_fifo_flush();
	pr_info("Page size %u, block_size %u, Size %llu, ecc %u, buswidth %u\n",
		mtd->writesize, mtd->erasesize, mtd->size,
			chip->ecc.strength,	ts_nfc->buswidth);

	ts_nfc->nfc_inited = true;

	return 0;

err_out:
	pr_err("ERROUT(%d): ret=%d\n", __LINE__, ret);
	nfc_nand_setup_timing(mtd, 0, &onfi_sdr_timings[0]);
	nand_device_reset(mtd, 0);
	nfc_fifo_flush();
	return ret;
}

static int nand_read_id_0(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	char id[5];
	int ret;

	ret = nand_read_id(mtd, 0, (uintptr_t)id, sizeof(id));
	if (ret != 0)
		return ret;


	pr_debug("Raw Nand Manufacturer 0x%x.\n", id[0]);
	pr_debug("Device ID 0x%x.\n", id[1]);

	pr_debug("internal Chip Number is %d.\n", 0x01 << (id[2] & 0x03));
	if (id[0] == 0x2c)
		pr_debug("Cell Type: %d Level Cell\n", (((id[2] >> 2) & 0x03) + 1));
	else
		pr_debug("Cell Type: %d Level Cell\n", 0x1 << (((id[2] >> 2) & 0x03) + 1));

	pr_debug("Number of Simultaneously Programmed Pages: %d\n",
			0x1 << ((id[2] >> 4) & 0x03));
	pr_debug("%s Interleaved Program between Multiple Die.\n",
			GETBIT(id[2], 6) ? "Supported" : "Not Supported");
	pr_debug("%s Cache Programming.\n",
			GETBIT(id[2], 7) ? "Supported" : "Not Supported");

	pr_debug("Page Size %d KB\n", 0x1 << (id[3] & 0x03));
	mtd->writesize = 1024 * (0x1 << (id[3] & 0x03));
	mtd->writesize_shift = ffs(mtd->writesize - 1);
	pr_debug("Size of spare area (byte per 512-byte):%d\n",
			GETBIT(id[3], 2) << 4);
	pr_debug("Seral Access Time %s ns.\n", GETBIT(id[3], 7) ? "20" : "25");
	pr_debug("Block Size %d KB.\n", 64 * (0x01 << ((id[3] >> 4) & 0x03)));
	mtd->erasesize = 1024 * 64 * (0x01 << ((id[3] >> 4) & 0x03));
	mtd->erasesize_shift = ffs(mtd->erasesize - 1);
	pr_debug("Organization: %s.\n", GETBIT(id[3], 6) ? "x16" : "x8");
	ts_nfc->buswidth = (GETBIT(id[3], 6) ? SZ_16 : SZ_8);

	chip->ecc.strength = MAX(chip->ecc.strength, 0x01 << (id[4] & 0x03));
	pr_debug("ECC level %d\n", chip->ecc.strength);
	pr_debug("Internal ECC %s\n.", GETBIT(id[4], 7) ? "disabled" : "enabled");

	chip->onfi_params.lun_count = 0x01 << ((id[4] >> 2) & 0x03);
	pr_debug("Plane Number per CE# %d\n", 0x01 << ((id[4] >> 2) & 0x03));

	chip->onfi_params.byte_per_page = mtd->writesize;
	chip->onfi_params.spare_bytes_per_page =
			(GETBIT(id[3], 2)) * 16 * mtd->writesize / 512; //not real.

	return ret;
}

static int detect_onfi(struct mtd_info *mtd)
{
	int ret;
	char id[4];
	struct nand_chip *chip = mtd_to_nand(mtd);

	nand_device_reset(mtd, 0);

	ret = nand_read_id(mtd, ONFI_SIGNATURE_ADDR, (uintptr_t)id, sizeof(id));
	if (ret != 0)
		return ret;

	if (strncmp(id, "ONFI", sizeof(id)) != 0) {
		pr_warn("NAND Non ONFI detected 0x%x\n", *(uint32_t *)id);
		return -ENODEV;
	}

	/*scan_ident not detect onfi, retry it.*/
	return nand_read_param_page(mtd, (struct nand_onfi_params *)
			chip->buffers->databuf, 0, sizeof(chip->onfi_params));
}
#endif

int read_check(uint32_t *addr, uint32_t exp_data)
{
	uint32_t rdata_1;

	rdata_1 = mmio_read_32((uintptr_t)addr);
	if (rdata_1 != exp_data) {
		pr_debug("addr = 0x%p, rdata = 0x%x exp_data = 0x%x\n", addr, rdata_1, exp_data);
		return 1;
	}

	return 0;
}


void check_ddr_test_data(uint32_t length, void *src, void *dest)
{
	int i = 0;

	for (i = 0; i < length / 4; i++) {
		read_check((uint32_t *)(uintptr_t)(DDR_MEM_ADDR_DST + i * 4),
			 *(uint32_t *)(uintptr_t)(DDR_MEM_ADDR_SRC + i * 4));
	}
	pr_debug("%s: len = 0x%x\n", __func__, length);
}


void prepare_ddr_test_data(uint32_t length)
{
	uint32_t i = 0;

#if defined(__ATF__) || defined(__UBOOT__)
	for (i = 0; i < length / 4; i++)
		mmio_write_32((uintptr_t)(DDR_MEM_ADDR_DST + i * 4), 0);

#endif

	for (i = 0; i < length / 4; i++) { //first page write end
		mmio_write_32((uintptr_t)(DDR_MEM_ADDR_SRC + i * 4),
			 0x20000000 + i * 4);
	}
	pr_debug("%s: len = 0x%x\n", __func__, length);
}

#if __TEST_CASE__
int nfc_test_case(struct mtd_info *mtd, void *src_addr, void *dst_addr)
{
	size_t len = 0;

	if (ts_nfc->mlun_en)
		nfc_mlun_select_index(0); // exec when memory busy.


#if 0
	/* user test case. */
#if defined(__ATF__) || true
	prepare_ddr_test_data(TEST_BYTES);
#endif

	//nfc_select_ready_device(i);
	//nfc_scan_whole_address();
	//nand_erase_siu(mtd, TEST_OFFSET, TEST_BYTES, false);
	ts_raw_nand_write(mtd, TEST_OFFSET, (uintptr_t)src_addr, TEST_BYTES);
	ts_raw_nand_read(mtd, TEST_OFFSET, (uintptr_t)dst_addr, TEST_BYTES, &len);
#else

	ts_raw_nand_read(mtd, TEST_OFFSET, (uintptr_t)src_addr, TEST_BYTES, &len);
	ts_raw_nand_write(mtd, TEST_OFFSET + 0x200000, (uintptr_t)src_addr, TEST_BYTES);
	ts_raw_nand_read(mtd, TEST_OFFSET + 0x200000, (uintptr_t)dst_addr, TEST_BYTES, &len);

#endif
	check_ddr_test_data(TEST_BYTES, src_addr, dst_addr);

	return 0;
}
#endif

static void nfc_set_ecc(bool enable)
{
	if (ts_nfc->hw_ecc_en != true)
		return;


	if (enable != GETBIT(mmio_read_32((uintptr_t)&nfc_regs->control), ECC_EN)) {
		mmio_clrsetbits_32((uintptr_t)&nfc_regs->control,
			 0x01 << ECC_EN, (enable ? true << ECC_EN : false));
		pr_debug("%s %sabled !\n", __func__, enable ? "en" : "dis");
	}
}

static void nfc_ecc_setup(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

#if defined(__ATF__)
	/* enable ecc by default in ATF. */
	chip->ecc.mode = NAND_ECC_HW;
#endif

	chip->ecc.size = chip->onfi_params.data_bytes_per_ppage;
	if (!chip->ecc.size)
		chip->ecc.size = 512;


	chip->ecc.steps = mtd->writesize / chip->ecc.size;

	if ((chip->ecc.strength > 0) && chip->onfi_params.data_bytes_per_ppage) {
		/*According to device tree value.*/
		//chip->ecc.mode = NAND_ECC_HW;
		//nfc_set_ecc(true);
	} else {
		//chip->ecc.mode = NAND_ECC_NONE;
		/* disable hw ecc. */
		nfc_set_ecc(false);
		ts_nfc->hw_ecc_en = false;
	}

		switch (chip->ecc.strength) {
		case TS_ECC_BCH4:
			chip->ecc.bytes = 7;
			break;
		case TS_ECC_BCH8:
			chip->ecc.bytes = 13;
			break;
		case TS_ECC_BCH16:
			chip->ecc.bytes = 28;
			break;
		case TS_ECC_BCH24:
			chip->ecc.bytes = 42;
			break;
		case TS_ECC_BCH32:
			chip->ecc.bytes = 56;
			break;
		case TS_ECC_BCH48:
			chip->ecc.bytes = 84;
			break;

		case TS_ECC_HAM:
			chip->ecc.bytes = 3; /*not support*/
		default:
			/* Use TS ECC BCH4 */
			pr_warn("%s: ecc strength %d not match! ", __func__, chip->ecc.strength);
			if (mtd->oobsize >= 64) {
				chip->ecc.bytes = 7;
				chip->ecc.strength = TS_ECC_BCH4;
				pr_warn("use default ecc strength %d, per block ecc bytes %d.\n",
					 chip->ecc.strength, chip->ecc.bytes);
			}
			break;
		}

		/* Set buswidth */
		if (chip->options & NAND_BUSWIDTH_16) {
			if (chip->ecc.bytes < 14)
				chip->ecc.bytes++;

			nfc_ctrl_set_buswidth(TRUE);
		}


		if (!chip->ecc.steps)
			chip->ecc.steps = mtd->writesize / chip->ecc.size;


		chip->ecc.total = chip->ecc.bytes * chip->ecc.steps;
		//if ((chip->ecc.strength >= 4) &&
		//chip->onfi_params.spare_bytes_per_page
		/* && ts_nfc->hw_ecc_en*/
		//)
		{
			nfc_assemble_hw_ecc(mtd, ECC_SEL_UNC);
		}

	if (chip->ecc.mode == NAND_ECC_HW) {
		ts_nfc->hw_ecc_en = true;
		nfc_set_ecc(true);
	} else {
		nfc_set_ecc(false);
		ts_nfc->hw_ecc_en = false;
	}

}

int ts_nand_init(struct mtd_info *mtd)
{
	/* then detect onfi */
	nand_device_init(mtd);
	nfc_update_timings(mtd, 0, ts_nfc->iface);
	/* after detect onfi, setup default ecc. */
	nfc_ecc_setup(mtd);

#if __TEST_CASE__
	//nand_scan_bbt(mtd);
	//nand_mtd_block_mark_bad(mtd, 2);
	//nfc_test_case(mtd, src_addr, dst_addr);
	//nand_device_reset(mtd, 0);
#endif

	return 0;
}

int ts_nfc_init(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	/* Enable Clock */

	/* Reset IP */
#ifdef __ATF__
	ts_de_reset_nfc();
#endif

	/* Setup default timings */
	nfc_nand_setup_timing(mtd, 0, &onfi_sdr_timings[0]);
	/* Setup default IP registers */
	nfc_hw_init(mtd);

	return 0;
}


/**
 * Read one byte from the chip
 *
 * @param mtd	MTD device structure
 * Return:	data byte
 *
 * Read function for 8bit bus-width
 */
static uint8_t read_byte(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);
	static uint32_t command;

	if (!chip->oob_poi)
		chip->oob_poi = chip->buffers->databuf + mtd->writesize;


	if (info->last_cmd != command || info->new_cmd == true) {
		command = info->last_cmd;
		info->last_byte = 0;
		info->new_cmd = false;
	}

	if (info->spare_only)
		return (chip->oob_poi)[info->last_byte++];
	else
		return (chip->buffers->databuf)[info->last_byte++];

}

/**
 * Read len bytes from the chip into a buffer
 *
 * @param mtd	MTD device structure
 * @param buf	buffer to store data to
 * @param len	number of bytes to read
 *
 * Read function for 8bit bus-width
 */
static void read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);

	static uint32_t command;
	static uint8_t *p;

	if (info->last_cmd != command || info->new_cmd == true) {
		command = info->last_cmd;
		if (info->spare_only) {
			info->spare_only = false;
			p = chip->oob_poi;
		} else {
			p = chip->buffers->databuf;
		}
		info->new_cmd = false;
	}

	if (p != buf)
		memcpy(buf, p, len);

}

static void write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
#if __ENABLE_WRITE__
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);

	static uint32_t command;
	static uint8_t *p;

	if (info->last_cmd != command || info->new_cmd == true) {
		command = info->last_cmd;
		//if (info->spare_only && chip->oob_poi) {
		p = chip->buffers->databuf;
		info->new_cmd = false;
	}

	if (p != buf)
		memcpy(p, buf, len);

#endif
}

/**
 * Check READY pin status to see if it is ready or not
 *
 * @param mtd	MTD device structure
 * @return
 *	1 - ready
 *	0 - not ready
 */
static int nand_dev_ready(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	int reg_val;
	struct nand_drv *info =
	    (struct nand_drv *)nand_get_controller_data(chip);

	reg_val = readl(&info->regs->status);
	if (reg_val & NFLASHx_READY(MEM0_ST))
		return 1;
	else
		return 0;
}

static void nand_select_chip(struct mtd_info *mtd, int chipnr)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct ts_nand_info *nand_info = nand_get_controller_data(chip);

	if ((chipnr >= 0) && (chipnr < 8)) {
		nand_info->cur_chip = chipnr;
		nfc_select_ready_device(chipnr);
	}
}

int init_nand_siu(struct nand_chip *chip)
{

	return 0;
}

#ifdef _TS_DMA_
int init_nand_dma(struct nand_chip *chip)
{
	int i;
	struct nand_drv *info =
	    (struct nand_drv *)nand_get_controller_data(chip);

	setbits_le32(&info->dma_glb->dma_glb_dma_lso_ctrl, TX_DMA_ENABLE);
	setbits_le32(&info->dma_glb->dma_glb_dma_ssp_rx_ctrl,
		     TX_DMA_ENABLE | DMA_CHECK_OWNER);
	setbits_le32(&info->dma_glb->dma_glb_dma_ssp_tx_ctrl,
		     RX_DMA_ENABLE | DMA_CHECK_OWNER);

	info->tx_desc = malloc_cache_aligned((sizeof(struct tx_descriptor_t) *
					      TS_DMA_DESC_NUM));
	info->rx_desc = malloc_cache_aligned((sizeof(struct rx_descriptor_t) *
					      TS_DMA_DESC_NUM));

	if (!info->rx_desc && info->tx_desc) {
		printf("Fail to alloc DMA descript!\n");
		kfree(info->tx_desc);
		return -ENOMEM;
	} else if (info->rx_desc && !info->tx_desc) {
		printf("Fail to alloc DMA descript!\n");
		kfree(info->tx_desc);
		return -ENOMEM;
	}

	/* set RX DMA base address and depth */
	clrsetbits_le32(&info->dma_nand->dma_q_rxq_base_depth,
			GENMASK(31, 4), (uintptr_t)info->rx_desc);
	clrsetbits_le32(&info->dma_nand->dma_q_rxq_base_depth,
			GENMASK(3, 0), TS_DMA_DEPTH);

	/* set TX DMA base address and depth */
	clrsetbits_le32(&info->dma_nand->dma_q_txq_base_depth,
			GENMASK(31, 4), (uintptr_t)info->tx_desc);
	clrsetbits_le32(&info->dma_nand->dma_q_txq_base_depth,
			GENMASK(3, 0), TS_DMA_DEPTH);

	memset((unsigned char *)info->tx_desc, 0,
	       (sizeof(struct tx_descriptor_t) * TS_DMA_DESC_NUM));
	memset((unsigned char *)info->rx_desc, 0,
	       (sizeof(struct rx_descriptor_t) * TS_DMA_DESC_NUM));

	for (i = 0; i < TS_DMA_DESC_NUM; i++) {
		/* set owner bit as SW */
		info->tx_desc[i].own = 1;
		/* enable Scatter-Gather memory copy */
		info->tx_desc[i].sgm = 0x1;
	}

	return 0;
}
#endif
/**
 * Send command to NAND device
 *
 * @param mtd		MTD device structure
 * @param command	the command to be sent
 * @param column	the column address for this command, -1 if none
 * @param page_addr	the page address for this command, -1 if none
 */
static void ts_nand_command(struct mtd_info *mtd, unsigned int command,
			    int column, int page_addr)
{
	/*
	 * read  page: raw page + oob size
	 * write page: hwecc page size.
	 */
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);
	int ret;

	nfc_fifo_flush();
	/*
	 * Write out the command to the device.
	 *
	 * Only command NAND_CMD_RESET or NAND_CMD_READID will come
	 * here before mtd->writesize is initialized.
	 */
	if (page_addr >= 0) {
		info->page_addr = page_addr;
		nand_address_row_set(mtd, page_addr << mtd->writesize_shift);
	}

	if (column >= 0) {
		info->col_addr = column;
		nand_address_col_set(mtd, column);
	} else {
		column = 0;
	}

	/* Reset FIFO before issue new command */
	info->flush_fifo = true;
	info->ecc_en = false;
	info->spare_only = false;
	info->status_request = false;
	info->last_byte = 0;
	info->new_cmd = true;
	/*
	 * Program and erase have their own busy handlers
	 * status and sequential in needs no delay
	 */
	switch (command) {

	/* support standard commnad. */
	case NAND_CMD_READID:
		info->ecc_en = false;
		info->flush_fifo = true;
		info->last_cmd = ONFI_CMD_READ_ID;
		info->col_addr = column;
		info->page_addr = 0;
		info->spare_only = false;
		nand_read_id(mtd, column, (uintptr_t)chip->buffers->databuf, NAND_MAX_ID_LEN);
		break;
	case NAND_CMD_PARAM:
		info->ecc_en = false;
		info->flush_fifo = true;
		info->last_cmd = ONFI_CMD_READ_PARAMETER_PAGE;
		info->col_addr = column;
		info->page_addr = 0;
		nand_read_param_page(mtd, (struct nand_onfi_params *)
			chip->buffers->databuf, column, sizeof(chip->onfi_params));
		break;

	case NAND_CMD_READOOB:
		/* Emulate NAND_CMD_READOOB row=page, col=chip->badblockpos*/
		assert(mtd->writesize != 0);
		if (mtd->writesize < SZ_512)
			column += (mtd->writesize + 5);
		else
			column += mtd->writesize;

		info->last_cmd = ONFI_CMD_READ_PAGE;
		info->col_addr = column;
		info->page_addr = page_addr;
		//info->ecc_en = (column ? false : true);
		info->ecc_en = false;
		info->flush_fifo = true;
		info->spare_only = true;
		info->status_request = false;
		nfc_set_ecc(info->ecc_en);
		memset(chip->oob_poi, 0xff,
			 mtd->oobsize - info->col_addr % mtd->writesize);
		nand_read_one_page_siu_oob_raw(mtd, info->page_addr,
			info->col_addr, (uintptr_t)chip->oob_poi,
				 mtd->oobsize - (info->col_addr % mtd->writesize));
		break;
	case NAND_CMD_READ0:
		//1. exit_status_op: page=-1, column=-1
		/* Command */
		/* addresses */
		if (page_addr == -1 && column == -1) {
			nfc_exec_onfi_command_siu(0x0);
			break;
		}
		info->last_cmd = ONFI_CMD_READ_PAGE;
		info->col_addr = column;
		info->page_addr = page_addr;
		info->ecc_en = true;
		info->flush_fifo = false;
		info->spare_only = false;
		info->status_request = false;
		nfc_set_ecc(info->ecc_en);
		nand_read_one_page_siu(mtd, (info->page_addr << mtd->writesize_shift) |
			info->col_addr, (uintptr_t)chip->buffers->databuf,
			mtd->writesize - info->col_addr, (uint32_t *)&ret);
		break;
	case NAND_CMD_SEQIN:
		/* Command */
		/* addresses: row=page, col=offset */
		info->last_cmd = ONFI_CMD_PROGRAM_PAGE;
		info->col_addr = column;
		info->page_addr = page_addr;
		info->flush_fifo = false;
		info->spare_only = false;
		info->status_request = false;

		break;
	case NAND_CMD_PAGEPROG:
		/* addresses: row=-1, col=-1 */
		/* SEQIN -> write_buf -> PAGEPROG.*/
		#if __ENABLE_WRITE__
		info->ecc_en = true;
		nfc_set_ecc(info->ecc_en);
		nand_write_one_page_siu(mtd, (info->page_addr << mtd->writesize_shift) |
			info->col_addr, (uintptr_t)chip->buffers->databuf,
			mtd->writesize /*+ mtd->oobsize*/, (uint32_t *)&ret);
		#endif
		break;
	case NAND_CMD_ERASE1:
		/* Command */
		/* addresses: row=page, col_addr=-1 */
		info->page_addr = page_addr;
		info->col_addr = 0;
		info->last_cmd = ONFI_CMD_ERASE_BLOCK;
		info->ecc_en = false;
		info->flush_fifo = false;
		info->spare_only = false;
		info->status_request = false;
		#if __ENABLE_ERASE__
		nand_erase_siu(mtd, (info->page_addr <<
			mtd->writesize_shift), mtd->erasesize, true);
		#endif
		break;
	case NAND_CMD_ERASE2:
		/*col=-1, row=-1*/
		break;
	case NAND_CMD_STATUS:
		/* Command */
		/*col=-1, row=-1*/
		info->last_cmd = ONFI_CMD_READ_STARUS;
		info->ecc_en = false;
		info->flush_fifo = false;
		info->spare_only = false;
		info->status_request = true;
		nfc_get_command_status(chip, (uint8_t *)&(chip->buffers->databuf));
		chip->buffers->databuf[0] |= NAND_STATUS_WP;
		break;
	case NAND_CMD_RESET:
		/* Command */
		/* addresses: row=-1, col=-1 */
		/* Issue command */
		info->last_cmd = ONFI_CMD_RESET;
		info->ecc_en = false;
		info->flush_fifo = false;
		info->spare_only = false;
		info->status_request = false;
		nand_device_reset(mtd, 0);
		break;

	case NAND_CMD_RNDIN:
		/* addresses: row=-1, col=offset */
		info->col_addr = column;
		info->last_cmd = ONFI_CMD_CHANGE_WRITE_COLUMN;
		info->ecc_en = false;
		info->flush_fifo = false;
		info->spare_only = false;
		info->status_request = false;
		//TODO...
		//nfc_exec_onfi_command(ONFI_CMD_CHANGE_WRITE_COLUMN);
		break;
	case NAND_CMD_GET_FEATURES:
		/*col=feature, row=-1*/
		info->last_cmd = ONFI_CMD_GET_FEATURES;
		info->ecc_en = false;
		info->flush_fifo = false;
		info->spare_only = false;
		info->status_request = false;
		nand_get_features(mtd, chip, column, (uint8_t *)chip->buffers->databuf);
		break;
	case NAND_CMD_SET_FEATURES:
		/*col=feature, row=-1, write_byte*/
		info->last_cmd = ONFI_CMD_SET_FEATURES;
		info->ecc_en = false;
		info->flush_fifo = false;
		info->spare_only = false;
		info->status_request = false;
		nand_set_features(mtd, chip, column, (uint8_t *)chip->buffers->databuf);
		break;

	/* Extended commands for large page devices */
	case NAND_CMD_RNDOUTSTART:
	//case NAND_CMD_RNDOUT:
		/*nand base no this cmd.*/
		info->last_cmd = ONFI_CMD_CHANGE_READ_COLUMN;
		info->ecc_en = false;
		info->flush_fifo = false;
		info->spare_only = false;
		info->status_request = false;
		nand_change_read_column_cmd(mtd, (info->page_addr << mtd->writesize_shift) |
			 info->col_addr, (uintptr_t)chip->buffers->databuf, mtd->writesize);
		break;
	case NAND_CMD_CACHEDPROG:
		/*nand base no this cmd.*/
		info->last_cmd = ONFI_CMD_PROGRAM_PAGE_CACHE;
		info->ecc_en = false;
		info->flush_fifo = false;
		info->spare_only = false;
		info->status_request = false;
		//Todo....
		//nfc_exec_onfi_command(ONFI_CMD_PROGRAM_PAGE_CACHE); //TODO check
		break;

#if defined(__UBOOT__)
	case NAND_CMD_LOCK:
	case NAND_CMD_UNLOCK1:
	case NAND_CMD_UNLOCK2:
#endif
	case NAND_CMD_READ1:

	default:
		pr_err("%s: Unsupported command %d\n", __func__, command);
		return;
	}

	nfc_fifo_flush();/* clean fifo.*/
	pr_debug("command 0x%x\n", command);
}

/**
 * Set up NAND bus width and page size
 *
 * @param info		nand_info structure
 * Return: 0 if ok, -1 on error
 */
static int set_bus_width_page_size(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct nand_drv *info =
	    (struct nand_drv *)nand_get_controller_data(chip);

	if (info->config.buswidth == SZ_8) {
		nfc_ctrl_set_buswidth(false);
	} else if (info->config.buswidth == SZ_16) {
		//chip->options |= NAND_buswidth; //TODO CHECK
		nfc_ctrl_set_buswidth(true);
	} else {
		nfc_ctrl_set_buswidth(false);
		pr_debug("%s: Unsupported bus width %d\n", __func__,
		      info->config.buswidth);
		return -1;
	}

	nfc_ctrl_judge_small_block(mtd, mtd->writesize);
	switch (mtd->writesize) {
	case SZ_256:
	case SZ_512:
	case SZ_2K:
	case SZ_4K:
	case SZ_8K:
		nand_setting_page_size(mtd, mtd->writesize, true);
		break;
	default:
		pr_debug("%s: Unsupported page size %d\n", __func__,
		      mtd->writesize);
		return -1;
	}

	return 0;
}

/**
 * Page read/write function
 *
 * @param mtd		mtd info structure
 * @param chip		nand chip info structure
 * @param buf		data buffer
 * @param page		page number
 * @param with_ecc	1 to enable ECC, 0 to disable ECC
 * @param is_writing	0 for read, 1 for write
 * Return:		0 when successfully completed
 *			-ETIMEDOUT when command timeout
 */
static int nand_rw_page(struct mtd_info *mtd, struct nand_chip *chip,
		uint8_t *buf, int page, int oob_required, int with_ecc, int is_writing)
{
	struct nand_drv *info = (struct nand_drv *)nand_get_controller_data(chip);
#if 1
	if (is_writing) {
		write_buf(mtd, buf, mtd->writesize);
	} else {
		read_buf(mtd, buf, mtd->writesize);
		if (oob_required && (info->spare_only == false)) {
			//nand_rw_oob(mtd, chip, page, 0, 0);
			nand_read_one_page_siu_oob_raw(mtd, info->page_addr,
				mtd->writesize, (uintptr_t)chip->oob_poi, mtd->oobsize);
		}
	}

#else
	int ret;
	unsigned int reg_v, ext_addr, addr, dma_index;
	struct tx_descriptor_t *tx_desc;
	struct rx_descriptor_t *rx_desc;
	struct nand_drv *info =
	    (struct nand_drv *)nand_get_controller_data(chip);
	/* reset ecc control */
	//clrsetbits_le32(&info->reg->flash_nf_ecc_reset, GENMASK(31, 0), RESET_NFLASH_ECC);

	/*  flash interrupt */
	//clrsetbits_le32(&info->reg->flash_flash_interrupt, GENMASK(0, 0), REGIRQ_CLEAR);

	/* reset ecc control */
	//clrsetbits_le32(&info->reg->flash_nf_ecc_reset, GENMASK(31, 0), RESET_NFLASH_ECC);

	/* Disable TXQ */
	//clrbits_le32(&info->dma_nand->dma_q_txq_control, GENMASK(0, 0));

	/* Clear interrupt */
	//setbits_le32(&info->dma_nand->dma_q_rxq_coal_interrupt, GENMASK(0, 0));
	//setbits_le32(&info->dma_nand->dma_q_txq_coal_interrupt, GENMASK(0, 0));

	if (with_ecc == 1) {
		switch (info->config.nand_ecc_strength) {
		case ECC_STRENGTH_8:
			reg_v = BCH_ERR_CAP_8;
			break;
		case ECC_STRENGTH_16:
			reg_v = BCH_ERR_CAP_16;
			break;
		case ECC_STRENGTH_24:
			reg_v = BCH_ERR_CAP_24;
			break;
		case ECC_STRENGTH_40:
			reg_v = BCH_ERR_CAP_40;
			break;
		default:
			reg_v = BCH_ERR_CAP_16;
			break;
		}
		reg_v |= BCH_ENABLE;

		/* BCH decode for flash read */
		if (is_writing == 0)
			reg_v |= BCH_DECODE;
		//clrsetbits_le32(&info->reg->flash_nf_bch_control, GENMASK(31, 0), reg_v);
	} else {
		//clrsetbits_le32(&info->reg->flash_nf_bch_control, GENMASK(31, 0), 0);
	}

	/* Fill Extend address */
	ext_addr = ((page << chip->page_shift) / EXT_ADDR_MASK);

	//clrsetbits_le32(&info->reg->flash_nf_access, GENMASK(7, 0), (uintptr_t)ext_addr);

	addr = (uintptr_t)((page << chip->page_shift) % EXT_ADDR_MASK);
	addr = (uintptr_t)(addr + info->flash_base);

	dma_index = readl(&info->dma_nand->dma_q_txq_wptr) & TS_DMA_Q_PTR_MASK;

	tx_desc = info->tx_desc;
	rx_desc = info->rx_desc;

	/* TX/RX descriptor for page data */
	tx_desc[dma_index].own = OWN_DMA;
	tx_desc[dma_index].buf_len = mtd->writesize;
	rx_desc[dma_index].own = OWN_DMA;
	rx_desc[dma_index].buf_len = mtd->writesize;
	if (is_writing == 0) {
		tx_desc[dma_index].buf_adr = (uintptr_t)addr;
		rx_desc[dma_index].buf_adr = (uintptr_t)(buf);
	} else {
		tx_desc[dma_index].buf_adr = (uintptr_t)buf;
		rx_desc[dma_index].buf_adr = (uintptr_t)(addr);
	}

	dma_index++;
	dma_index %= TS_DMA_DESC_NUM;

	/* TX/RX descriptor for OOB area */
	addr = (uintptr_t)(addr + mtd->writesize);
	tx_desc[dma_index].own = OWN_DMA;
	tx_desc[dma_index].buf_len = mtd->oobsize;
	rx_desc[dma_index].own = OWN_DMA;
	rx_desc[dma_index].buf_len = mtd->oobsize;
	if (is_writing) {
		tx_desc[dma_index].buf_adr = (uintptr_t)(chip->oob_poi);
		rx_desc[dma_index].buf_adr = (uintptr_t)addr;
	} else {
		tx_desc[dma_index].buf_adr = (uintptr_t)addr;
		rx_desc[dma_index].buf_adr = (uintptr_t)(chip->oob_poi);
		dma_index++;
		dma_index %= TS_DMA_DESC_NUM;
	}

	//if (is_writing == 1)
		//clrsetbits_le32(&info->reg->flash_fifo_control, GENMASK(1, 0), FIFO_WRITE);
	//else
		//clrsetbits_le32(&info->reg->flash_fifo_control, GENMASK(1, 0), FIFO_READ);


	/* Start FIFO request */
	//clrsetbits_le32(&info->reg->flash_flash_access_start, GENMASK(2, 2), NFLASH_FIFO_REQ);

	/* Update DMA write pointer */
	//clrsetbits_le32(&info->dma_nand->dma_q_txq_wptr, GENMASK(12, 0), dma_index);

	/* Start DMA */
	//clrsetbits_le32(&info->dma_nand->dma_q_txq_control, GENMASK(0, 0), TX_DMA_ENABLE);

	/* Wait TX DMA done */
	//ret = readl_poll_timeout(&info->dma_nand->dma_q_txq_coal_interrupt,
		 reg_v, (reg_v & 1), FLASH_LONG_DELAY);
	if (ret) {
		pr_err("TX DMA timeout\n");
		return -ETIMEDOUT;
	}
	/* clear tx interrupt */
	//setbits_le32(&info->dma_nand->dma_q_txq_coal_interrupt, 1);

	/* Wait RX DMA done */
	//ret = readl_poll_timeout(&info->dma_nand->dma_q_rxq_coal_interrupt,
		 reg_v, (reg_v & 1), FLASH_LONG_DELAY);
	if (ret) {
		pr_err("RX DMA timeout\n");
		return -ETIMEDOUT;
	}
	/* clear rx interrupt */
	//setbits_le32(&info->dma_nand->dma_q_rxq_coal_interrupt, 1);

	/* wait NAND CMD done */
	if (is_writing == 0) {
		if (!nand_waitfor_cmd_completion(info->reg, NFLASH_FIFO_REQ))
			printf("%s: Command timeout\n", __func__);
	}

	/* Update DMA read pointer */
	//clrsetbits_le32(&info->dma_nand->dma_q_rxq_rptr, GENMASK(12, 0), dma_index);

	/* ECC correction */
	if (with_ecc == 1) {
		//ret = readl_poll_timeout(&info->reg->flash_nf_bch_status,
					//reg_v, (reg_v & BCH_GEN_DONE), FLASH_LONG_DELAY);
		if (ret) {
			pr_err("BCH_GEN timeout! flash_nf_bch_status=[0x%x]\n", reg_v);
			return -ETIMEDOUT;
		}

		if (is_writing == 0)
			ts_do_bch_decode(mtd, chip, buf, page, addr);
		else
			ts_do_bch_encode(mtd, chip, page);
	}

	if (is_writing) {
		dma_index++;
		dma_index %= TS_DMA_DESC_NUM;

		/* Update DMA R/W pointer */
		clrsetbits_le32(&info->dma_nand->dma_q_txq_wptr, GENMASK(12, 0),
				dma_index);

		/* Wait TX DMA done */
		ret =
		   readl_poll_timeout(&info->dma_nand->dma_q_txq_coal_interrupt,
				      reg_v, (reg_v & 1), FLASH_LONG_DELAY);
		if (ret) {
			pr_err("TX DMA timeout\n");
			return -ETIMEDOUT;
		}
		/* clear tx interrupt */
		setbits_le32(&info->dma_nand->dma_q_txq_coal_interrupt, 1);

		/* Wait RX DMA done */
		ret =
		   readl_poll_timeout(&info->dma_nand->dma_q_rxq_coal_interrupt,
				      reg_v, (reg_v & 1), FLASH_LONG_DELAY);
		if (ret) {
			pr_err("RX DMA timeout\n");
			return -ETIMEDOUT;
		}
		/* clear rx interrupt */
		setbits_le32(&info->dma_nand->dma_q_rxq_coal_interrupt, 1);

		/* wait NAND CMD done */
		if (!nand_waitfor_cmd_completion(info->reg, NFLASH_FIFO_REQ))
			printf("%s: Command timeout\n", __func__);

		/* Update DMA R/W pointer */
		clrsetbits_le32(&info->dma_nand->dma_q_rxq_rptr, GENMASK(12, 0),
				dma_index);
	}
#endif

	return 0;
}

/**
 * Hardware ecc based page read function
 *
 * @param mtd	mtd info structure
 * @param chip	nand chip info structure
 * @param buf	buffer to store read data
 * @param page	page number to read
 * Return:	0 when successfully completed
 *		-ETIMEDOUT when command timeout
 */
static int nand_read_page_hwecc(struct mtd_info *mtd,
				struct nand_chip *chip, uint8_t *buf,
				int oob_required, int page)
{
	int ret;

	ret = nand_rw_page(mtd, chip, buf, page, oob_required, 1, 0);
	if (ret)
		return ret;

	/* Reset FIFO */

	return 0;
}

/**
 * Hardware ecc based page write function
 *
 * @param mtd	mtd info structure
 * @param chip	nand chip info structure
 * @param buf	data buffer
 * Return:	0 when successfully completed
 *		-ETIMEDOUT when command timeout
 */
static int nand_write_page_hwecc(struct mtd_info *mtd,
				 struct nand_chip *chip, const uint8_t *buf,
				 int oob_required, int page)
{
	int ret;

	ret = nand_rw_page(mtd, chip, (uint8_t *)buf, page, oob_required, 1, 1);
	if (ret)
		return ret;

	/* Reset FIFO */

	return 0;
}

/**
 * Read raw page data without ecc
 *
 * @param mtd	mtd info structure
 * @param chip	nand chip info structure
 * @param buf	buffer to store read data
 * @param page	page number to read
 * Return:	0 when successfully completed
 *		-ETIMEDOUT when command timeout
 */
static int ts_nand_read_page_raw(struct mtd_info *mtd,
			      struct nand_chip *chip, uint8_t *buf,
			      int oob_required, int page)
{
	int ret;

	ret = nand_rw_page(mtd, chip, buf, page, oob_required, 0, 0);
	if (ret)
		return ret;

	/* Reset FIFO */

	return 0;
}

/**
 * Raw page write function
 *
 * @param mtd	mtd info structure
 * @param chip	nand chip info structure
 * @param buf	data buffer
 * Return:	0 when successfully completed
 *		-ETIMEDOUT when command timeout
 */
static int ts_nand_write_page_raw(struct mtd_info *mtd,
			       struct nand_chip *chip, const uint8_t *buf,
			       int oob_required, int page)
{
	int ret;

	ret = nand_rw_page(mtd, chip, (uint8_t *)buf, page, oob_required, 0, 1);
	if (ret)
		return ret;

	/* Reset FIFO */

	return 0;
}

/**
 * OOB data read/write function
 *
 * @param mtd		mtd info structure
 * @param chip		nand chip info structure
 * @param page		page number to read
 * @param with_ecc	1 to enable ECC, 0 to disable ECC
 * @param is_writing	0 for read, 1 for write
 * Return:		0 when successfully completed
 *			-ETIMEDOUT when command timeout
 */
static int nand_rw_oob(struct mtd_info *mtd, struct nand_chip *chip,
		       int page, int with_ecc, int is_writing)
{
	u32 reg_val;

	if (is_writing) {
		reg_val = NFLASHx_READY(MEM0_ST) | NFLASH_WT;
		pwrite = (unsigned int *)chip->oob_poi;
		#if __ENABLE_WRITE__
		nand_write_one_page_siu_oob_raw(mtd, page,
			 mtd->writesize, (uintptr_t)pwrite, 2/*mtd->oobsize*/);
		#endif
	} else {
		reg_val = NFLASHx_READY(MEM0_ST) | NFLASH_RD;
		pread = (unsigned int *)chip->oob_poi;
		memset(pread, 0xff, mtd->oobsize);
		nand_read_one_page_siu_oob_raw(mtd, page,
			 mtd->writesize, (uintptr_t)pread, mtd->oobsize);
	}

	return 0;
}

/**
 * OOB data read function
 *
 * @param mtd		mtd info structure
 * @param chip		nand chip info structure
 * @param page		page number to read
 */
static int nand_read_oob(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	int ret;

	//chip->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
	ret = nand_rw_oob(mtd, chip, page, 0, 0);
	/* Reset FIFO */

	return ret;
}

/**
 * OOB data write function
 *
 * @param mtd	mtd info structure
 * @param chip	nand chip info structure
 * @param page	page number to write
 * Return:	0 when successfully completed
 *		-ETIMEDOUT when command timeout
 */
static int nand_write_oob(struct mtd_info *mtd, struct nand_chip *chip,
			  int page)
{
	int ret;

	ret = nand_rw_oob(mtd, chip, page, 0, 1);
	/* Reset FIFO */

	return ret;
}


/**
 * config flash type
 *
 * @param chip	nand chip info structure
 */
static void nand_config_flash_type(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	if (chip->page_shift)
		mtd->writesize_shift = chip->page_shift;

	if (chip->phys_erase_shift)
		mtd->erasesize_shift = chip->phys_erase_shift;


	nfc_update_timings(mtd, 0, ts_nfc->iface);
	nfc_ecc_setup(mtd);
}

/**
 * config oob layout
 *
 * @param chip  nand chip info structure
 * Return:	0 when successfully completed
 *		-EINVAL when ECC bytes exceed OOB size
 */
static int nand_config_oob_layout(struct nand_chip *chip)
{
	int i, ecc_start_offset;
	struct mtd_info *mtd = nand_to_mtd(chip);

	/* Calculate byte count for ECC */
	eccoob.eccbytes = (mtd->writesize / chip->ecc.size) * chip->ecc.bytes;

	if (mtd->oobsize < eccoob.eccbytes) {
		pr_err("Spare area(%d) too small for BCH%d\n", chip->ecc.bytes,
		       chip->ecc.strength / 8);
		pr_err("page_sz: %d\n", chip->ecc.size);
		pr_err("oob_sz: %d\n", chip->ecc.bytes);
		return -EINVAL;
	}

	/* Update OOB layout */
	ecc_start_offset = mtd->oobsize - eccoob.eccbytes;
	memset(eccoob.eccpos, 0, sizeof(eccoob.eccpos));
	for (i = 0; i < eccoob.eccbytes; ++i)
		eccoob.eccpos[i] = i + ecc_start_offset;

	/* Unused spare area
	 * OOB[0] is bad block marker.
	 * Extra two byte is reserved as
	 * erase marker just right before ECC code.
	 */
	eccoob.oobavail = mtd->oobsize - eccoob.eccbytes - 2;
	eccoob.oobfree[0].offset = 2;
	eccoob.oobfree[0].length =
	    mtd->oobsize - eccoob.eccbytes - eccoob.oobfree[0].offset - 1;

	return 0;
}

#if defined(__KERNEL__)
/*
 * this macro allows us to convert from an MTD structure to our own
 * device context (ts_nand) structure.
 */
static inline struct ts_nand_info *mtd_to_ts(struct mtd_info *mtd)
{
	return container_of(mtd_to_nand(mtd), struct ts_nand_info, nand_chip);
}

static int ts_ooblayout_ecc(struct mtd_info *mtd, int section,
				struct mtd_oob_region *oobregion)
{
	//struct ts_nand_info *ts = mtd_to_ts(mtd);
	struct nand_chip *chip = mtd_to_nand(mtd);

	if (section)
		return -ERANGE;

	//oobregion->offset = ts->oob_skip_bytes;
	oobregion->offset = mtd->oobsize - chip->ecc.total;
	oobregion->length = chip->ecc.total;

	return 0;
}

static int ts_ooblayout_free(struct mtd_info *mtd, int section,
				 struct mtd_oob_region *oobregion)
{
	//struct ts_nand_info *ts = mtd_to_ts(mtd);
	struct nand_chip *chip = mtd_to_nand(mtd);

	if (section)
		return -ERANGE;

	//oobregion->offset = chip->ecc.total + ts->oob_skip_bytes;
	oobregion->offset = 2;
	oobregion->length = mtd->oobsize - chip->ecc.total - oobregion->offset;

	return 0;
}

static const struct mtd_ooblayout_ops ts_ooblayout_ops = {
	.ecc = ts_ooblayout_ecc,
	.free = ts_ooblayout_free,
};

/* initialize driver data structures */
static void ts_drv_init(struct ts_nand_info *ts)
{
	/*
	 * the completion object will be used to notify
	 * the callee that the interrupt is done
	 */
	init_completion(&ts->complete);

	/*
	 * the spinlock will be used to synchronize the ISR with any
	 * element that might be access shared data (interrupt status)
	 */
	spin_lock_init(&ts->irq_lock);
}
#endif

int ts_nand_init_ctrl(struct ts_nand_info *ts_nand)
{
	struct nand_chip *chip = &ts_nand->nand_chip;
	struct nand_drv *info = &ts_nand->nand_ctrl;
	struct nand_config *config = &info->config;
	struct mtd_info *mtd;
	int ret;

	mtd = nand_to_mtd(chip); //export mtd addr.
	ts_nfc = config;
	ts_nfc->onfi_params = &chip->onfi_params;
	nfc_regs = info->regs;
	chip->data_interface = ts_nfc->iface = &onfi_sdr_timings[0];

#if defined(__KERNEL__)
	ts_drv_init(ts_nand);

	/* Request IRQ after all the hardware initialization is finished */
	ret = devm_request_irq(ts_nand->dev, ts_nand->irq, ts_isr,
			       IRQF_SHARED, TS_NAND_NAME, ts_nand);
	if (ret) {
		dev_err(ts_nand->dev, "Unable to request IRQ\n");
		return ret;
	}

	ts_nfc->interrupt_en = true;
	nfc_interrupt_enable(ts_nfc->interrupt_en);

	nand_set_flash_node(chip, ts_nand->dev->of_node);
	/* Fallback to the default name if DT did not give "label" property */
	if (!mtd->name)
		mtd->name = "ts-nand";
#endif

#ifdef CONFIG_SYS_NAND_USE_FLASH_BBT
	/*
	 * We'll use a bad block table stored in-flash and don't
	 * allow writing the bad block marker to the flash.
	 */
	//chip->bbt_options |= NAND_BBT_USE_FLASH | NAND_BBT_NO_OOB_BBM;
	chip->bbt_options |= NAND_BBT_USE_FLASH | NAND_BBT_NO_OOB;
	chip->bbt_td = &bbt_main_descr;
	chip->bbt_md = &bbt_mirror_descr;
#endif

#if defined(__UBOOT__)
	chip->ecc.layout = &eccoob; //init ecc layout
#endif

	chip->IO_ADDR_R = &info->regs->fifo_data;
	chip->IO_ADDR_W = &info->regs->fifo_data;

	chip->cmdfunc = ts_nand_command;
	chip->read_byte = read_byte;
	chip->read_buf = read_buf;
	chip->write_buf = write_buf;
	chip->select_chip = nand_select_chip;
	chip->setup_data_interface = nfc_update_timings;
	chip->dev_ready = nand_dev_ready;
	chip->onfi_set_features = nand_set_features;
	chip->onfi_get_features = nand_get_features;
	chip->waitfunc = ts_waitfunc;

	chip->ecc.read_page = nand_read_page_hwecc;
	chip->ecc.write_page = nand_write_page_hwecc;
	chip->ecc.read_page_raw = ts_nand_read_page_raw;
	chip->ecc.write_page_raw = ts_nand_write_page_raw;
	chip->ecc.read_oob = nand_read_oob;
	chip->ecc.write_oob = nand_write_oob;
	chip->ecc.read_oob_raw = nand_read_oob;
	chip->ecc.write_oob_raw = nand_write_oob;

	/* erase will restore, default markbad or check bad is well.*/
	//chip->erase = ts_nand_erase_one_block;
	//chip->block_bad = nand_mtd_block_is_bad;
	//chip->block_markbad = nand_mtd_block_mark_bad;

	nand_set_controller_data(chip, &ts_nand->nand_ctrl);

	/* Disable subpage writes as we do not provide ecc->hwctl */
	chip->options |= NAND_BUSWIDTH_AUTO | NAND_NO_SUBPAGE_WRITE |
		 NAND_SKIP_BBTSCAN | NAND_USE_BOUNCE_BUFFER;
#if defined(__UBOOT__)
	if (ts_nand->dev)
		chip->flash_node = dev_of_offset(ts_nand->dev);
#endif
	/* Configure flash type as P-NAND */
	//clrsetbits_le32(&info->reg->flash_type, FLASH_PIN, FLASH_TYPE_4K | FLASH_SIZE_436OOB);
	config->buswidth = SZ_8; /* must be 8 */

	ret = ts_nfc_init(chip);
	if (ret)
		return ret;


#if defined(__KERNEL__)
	chip->buffers = kzalloc(sizeof(*chip->buffers), GFP_KERNEL);
	if (!chip->buffers) {
		ret = -ENOMEM;
		goto disable_irq;
	}

	chip->buffers->databuf = kmalloc(8192, GFP_KERNEL);
	if (!chip->buffers->databuf) {
		ret = -ENOMEM;
		goto free_buf;
	}

	ts_nand->nand_ctrl.buf = chip->buffers->databuf;
#endif

	/* read device tree */
	/* ident ONFI. */
	ret = nand_scan_ident(mtd, CONFIG_SYS_NAND_MAX_CHIPS, NULL);
	if (ret)
		return ret;

	ret = nand_device_init(mtd);
	if (ret)
		return ret;

	/* Reconfig flash type according to ONFI */
	nand_config_flash_type(chip);


#if __TEST_CASE__
	//nand_scan_bbt(mtd);
	//nand_mtd_block_mark_bad(mtd, 2);
	//nfc_test_case(mtd, src_addr, dst_addr);
	//nand_device_reset(mtd, 0);
#endif
	ret = set_bus_width_page_size(mtd);
	if (ret)
		return ret;

	/* Set the bad block position */
	chip->badblockpos =
	    mtd->writesize >
	    512 ? NAND_LARGE_BADBLOCK_POS : NAND_SMALL_BADBLOCK_POS;

	/* Arrange OOB layout */
	ret = nand_config_oob_layout(chip);
	if (ret)
		return ret;
#if 0
	/* Init DMA descriptor ring */
	ret = init_nand_dma(chip);
	if (ret)
		return ret;
#endif

	ret = nand_scan_tail(mtd);
	if (ret)
		return ret;

	if (chip->erase/* == single_erase*/)
		chip->erase = ts_nand_erase_one_block;

#if defined(__UBOOT__)
	ret = nand_register(0, mtd);
	if (ret) {
		pr_err("Failed to register MTD: %d\n", ret);
		return ret;
	}
#elif defined(__KERNEL__)
	ret = mtd_device_register(mtd, NULL, 0);
	if (ret) {
		dev_err(ts_nand->dev, "Failed to register MTD: %d\n", ret);
		goto free_buf;
	}
#endif

	ret = set_bus_width_page_size(mtd);
	if (ret)
		return ret;

	printf("P-NAND    : %s\n", mtd->name);
	printf("Chip  Size: %lldMB\n", chip->chipsize / (1024 * 1024));
	printf("Number Chips: %d\n", chip->numchips);
	printf("Block Size: %dKB\n", mtd->erasesize / 1024);
	printf("Page  Size: %dB\n", mtd->writesize);
	printf("OOB   Size: %dB\n", mtd->oobsize);

	return 0;
}
