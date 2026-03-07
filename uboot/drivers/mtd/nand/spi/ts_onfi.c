// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 Tsing Micro, Inc.
 *
 * Authors:
 *	Peter Pan <liuyagnyang@tsingmicro.com>
 */

#ifndef __UBOOT__
#include <malloc.h>
#include <linux/device.h>
#include <linux/kernel.h>
#endif
#include <linux/bitops.h>
#include <linux/mtd/spinand.h>
#include <linux/printk.h>

#if 0
#ifdef debug
#undef debug
#define debug printk
#endif

#ifdef pr_debug
#undef pr_debug
#define pr_debug printk
#endif

#ifdef pr_warn
#undef pr_warn
#define pr_warn printk
#endif

#ifdef pr_info
#undef pr_info
#define pr_info printk
#endif
#endif

#define SPINAND_MFR_ONFI		0xa1 //fudan micro.

#define ONFI_STATUS_ECC_MASK		GENMASK(7, 4)
#define ONFI_STATUS_ECC_NO_BITFLIPS	(0 << 4)
#define ONFI_STATUS_ECC_1TO3_BITFLIPS	(1 << 4)
#define ONFI_STATUS_ECC_4TO6_BITFLIPS	(3 << 4)
#define ONFI_STATUS_ECC_7TO8_BITFLIPS	(5 << 4)

#if 1
/*
 * As per datasheet, die selection is done by the 6th bit of Die
 * Select Register (Address 0xD0).
 */
#define ONFI_DIE_SELECT_REG	0xD0

#define ONFI_SELECT_DIE(x)	((x) << 6)
#endif

static SPINAND_OP_VARIANTS(read_cache_variants,               //read one page
		/*SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 2, NULL, 0),*/
		SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
		/*SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(0, 1, NULL, 0),*/
		SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(false, 0, 1, NULL, 0));

static SPINAND_OP_VARIANTS(write_cache_variants,   //write one page
		/*SPINAND_PROG_LOAD_X4(true, 0, NULL, 0),*/
		SPINAND_PROG_LOAD(true, 0, NULL, 0));

static SPINAND_OP_VARIANTS(update_cache_variants, //random write
		/*SPINAND_PROG_LOAD_X4(false, 0, NULL, 0),*/
		SPINAND_PROG_LOAD(false, 0, NULL, 0));
#if 0
static SPINAND_OP_VARIANTS(read_cache_variants_all,               //read one page
		SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 2, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(false, 0, 1, NULL, 0));

static SPINAND_OP_VARIANTS(write_cache_variants_all,   //write one page
		SPINAND_PROG_LOAD_X4(true, 0, NULL, 0),
		SPINAND_PROG_LOAD(true, 0, NULL, 0));

static SPINAND_OP_VARIANTS(update_cache_variants_all, //random write
		SPINAND_PROG_LOAD_X4(false, 0, NULL, 0),
		SPINAND_PROG_LOAD(false, 0, NULL, 0));
#endif
static int ts_onfi_8_ooblayout_ecc(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *region)
{
	if (section)
		return -ERANGE;

	region->offset = mtd->oobsize / 2;
	region->length = mtd->oobsize / 2;

	return 0;
}

static int ts_onfi_8_ooblayout_free(struct mtd_info *mtd, int section,
				   struct mtd_oob_region *region)
{
	if (section)
		return -ERANGE;

	/* Reserve 2 bytes for the BBM. */
	region->offset = 2;
	region->length = (mtd->oobsize / 2) - 2;

	return 0;
}

static const struct mtd_ooblayout_ops ts_onfi_8_ooblayout = {
	.ecc = ts_onfi_8_ooblayout_ecc,
	.rfree = ts_onfi_8_ooblayout_free,
};

static int ts_onfi_select_target(struct spinand_device *spinand,
				unsigned int target)
{
	#if 0
	struct spi_mem_op op = SPINAND_SET_FEATURE_OP(ONFI_DIE_SELECT_REG,
						      spinand->scratchbuf);

	if (target > 1)
		return -EINVAL;

	*spinand->scratchbuf = ONFI_SELECT_DIE(target);

	return spi_mem_exec_op(spinand->slave, &op);
	#endif

	return 0;
}

static int ts_onfi_8_ecc_get_status(struct spinand_device *spinand,
				   u8 status)
{
	switch (status & ONFI_STATUS_ECC_MASK) {
	case STATUS_ECC_NO_BITFLIPS:
		return 0;

	case STATUS_ECC_UNCOR_ERROR:
		return -EBADMSG;

	case ONFI_STATUS_ECC_1TO3_BITFLIPS:
		return 3;

	case ONFI_STATUS_ECC_4TO6_BITFLIPS:
		return 6;

	case ONFI_STATUS_ECC_7TO8_BITFLIPS:
		return 8;

	default:
		break;
	}

	return -EINVAL;
}

static struct spinand_info ts_onfi_spinand_table[] = {
	/* FM25L01 1Gb 1.8V 0XA1 0XA5 */
	SPINAND_INFO("FM25LS01", 0xA5,
		     NAND_MEMORG(1, 2048, 128, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&ts_onfi_8_ooblayout,
				     ts_onfi_8_ecc_get_status)),
	/* XT26G02E 2Gb 3.3v 0X2C 0X24 */
	SPINAND_INFO("XT26G02E", 0x24,
		     NAND_MEMORG(1, 2048, 128, 64, 1024, 2, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&ts_onfi_8_ooblayout,
				     ts_onfi_8_ecc_get_status)),
	/* replaced by onfi detect. */
	SPINAND_INFO("onfi", 0xff,
		     //NAND_MEMORG(1, 4096, 256, 64, 2048, 1, 1, 1),
		     NAND_MEMORG(1, 2048, 128, 64, 1024, 1, 1, 1),
		     NAND_ECCREQ(8, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     /*SPINAND_HAS_CR_FEAT_BIT,*/
		     0,
		     SPINAND_ECCINFO(&ts_onfi_8_ooblayout,
				     ts_onfi_8_ecc_get_status),
		     SPINAND_SELECT_TARGET(ts_onfi_select_target)),
};

static int ts_onfi_spinand_detect(struct spinand_device *spinand)
{
	u8 *id = spinand->id.data;
	int ret;

	/*
	 * FM25L01 SPI NAND read ID need a dummy byte,
	 * so the first byte in raw_id is dummy.
	 */
	if (id[1] != SPINAND_MFR_ONFI) {
		//return 0;
		pr_info("Manufacturer id = 0x%x\n", *(uint32_t *)id);
	}

	/* first pre config. */
	ret = spinand_match_and_init(spinand, ts_onfi_spinand_table,
				     ARRAY_SIZE(ts_onfi_spinand_table), /*id[2]*/ 0xA5);
	if (ret)
		return ret;

	return 1;
}


struct nand_onfi_params {
	/* rev info and features block */
	/* 'O' 'N' 'F' 'I'  */
	u8 sig[4];
	__le16 revision;
	__le16 features;
	__le16 opt_cmd;
	u8 reserved0[2];
	__le16 ext_param_page_length; /* since ONFI 2.1 */
	u8 num_of_param_pages;        /* since ONFI 2.1 */
	u8 reserved1[17];

	/* manufacturer information block */
	char manufacturer[12];
	char model[20];
	u8 jedec_id;
	__le16 date_code;
	u8 reserved2[13];

	/* memory organization block */
	__le32 byte_per_page;
	__le16 spare_bytes_per_page;
	__le32 data_bytes_per_ppage;
	__le16 spare_bytes_per_ppage;
	__le32 pages_per_block;
	__le32 blocks_per_lun;
	u8 lun_count;
	u8 addr_cycles;
	u8 bits_per_cell;
	__le16 bb_per_lun;
	__le16 block_endurance;
	u8 guaranteed_good_blocks;
	__le16 guaranteed_block_endurance;
	u8 programs_per_page;
	u8 ppage_attr;
	u8 ecc_bits;
	u8 interleaved_bits;
	u8 interleaved_ops;
	u8 reserved3[13];

	/* electrical parameter block */
	u8 io_pin_capacitance_max;
	__le16 async_timing_mode;
	__le16 program_cache_timing_mode;
	__le16 t_prog;
	__le16 t_bers;
	__le16 t_r;
	__le16 t_ccs;
	__le16 src_sync_timing_mode;
	u8 src_ssync_features;
	__le16 clk_pin_capacitance_typ;
	__le16 io_pin_capacitance_typ;
	__le16 input_pin_capacitance_typ;
	u8 input_pin_capacitance_max;
	u8 driver_strength_support;
	__le16 t_int_r;
	__le16 t_adl;
	u8 reserved4[8];

	/* vendor */
	__le16 vendor_revision;
	u8 vendor[88];

	__le16 crc;
} __packed;

/* CRC calculation */
#define CRC_POLYNOM 0x8005U
#define CRC_INIT_VALUE 0x4F4EU

static uint16_t nand_check_crc(uint16_t crc, uint8_t *data_in,
							   unsigned int data_len)
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

static int spinand_read_param_op(struct spinand_device *spinand,
	struct nand_onfi_params *page, struct spinand_info *onfi_info)
{
	int ret = 0, i = 0;
	uint16_t crc[3] = {0, 1, 2};
	int val;
	struct nand_device *nand = spinand_to_nand(spinand);
	struct mtd_info *mtd = nanddev_to_mtd(nand);

	struct nand_pos pos = {
		.target = 0,
		.lun = 0,
		.plane = 0,
		.eraseblock = 0,
		.page = 0x01,
	};

	struct nand_page_io_req req = {
		.pos = pos,
		.datalen = nand->memorg.pagesize,
		.dataoffs = 0,
		.ooboffs = 0,
		.ooblen = 0,
		.databuf.out = spinand->databuf,
		.mode = MTD_OPS_RAW,
	};

	//spinand_reset_op(spinand);
	memset(page, 0, sizeof(*page));

	for (i = 0; i < 3; i++) {
		ret = spinand_read_page(spinand, &req, false);
		if (ret) {
			debug("read param error, ret=%d\n", ret);
			//spinand_reset_op(spinand);
			//continue;
		}

		memcpy(page, spinand->databuf + i * sizeof(*page), sizeof(*page));

		if (strncmp((char *)&page->sig, "ONFI", 4) != 0) {
			pr_warn("Error ONFI detection sig=0x%x\n", *(uint32_t *)page->sig);
			continue;
		}

		crc[i] = nand_check_crc(CRC_INIT_VALUE, (uint8_t *)page, 254U);
		if (crc[i] != page->crc) {
			pr_warn("Error reading param crc=0x%x calc crc=0x%x\n", page->crc, crc[i]);
			ret = -1;
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
	if (ret || i >= 3)
		return ret;


	onfi_info->model = page->model;

	struct nand_memory_organization memorg = NAND_MEMORG(page->bits_per_cell, page->byte_per_page,
		 page->spare_bytes_per_page, page->pages_per_block, page->blocks_per_lun,
		 1 << page->interleaved_bits, page->lun_count, 1);

	onfi_info->memorg = memorg;
	memcpy(&onfi_info->memorg, &memorg, sizeof(memorg));

	onfi_info->eccreq.step_size = page->data_bytes_per_ppage;
	switch (page->ecc_bits) {
	case 4:
	case 8:
	case 16:
	case 24:
	case 32:
	case 48:
		onfi_info->eccreq.strength = page->ecc_bits;
		break;

	default:
		break;
	}

	/* Check version */
	val = le16_to_cpu(page->revision);
	debug("ONFI version: %d\n", val);
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

	mtd->size = page->pages_per_block *
								 page->byte_per_page *
								 page->blocks_per_lun * page->lun_count;

	pr_info("ONFI: Page size %u, block_size %u, Size %llu, ecc %u\n",
		 mtd->writesize, mtd->erasesize, mtd->size, onfi_info->eccreq.strength);


	return ret;
}

static int ts_onfi_spinand_init(struct spinand_device *spinand)
{
	struct nand_onfi_params param;
	u8 *id = spinand->id.data;
	u8 onfi_index = ARRAY_SIZE(ts_onfi_spinand_table) - 1;
	uint ret, i;

	ret = spinand_upd_cfg(spinand, CFG_QUAD_ENABLE, CFG_QUAD_ENABLE);
	if (ret)
		debug("%s:%d Enable qual mode error! ret=%d\n", __func__, __LINE__, ret);

	ret = spinand_upd_cfg(spinand, CFG_OTP_ENABLE, CFG_OTP_ENABLE);
	if (ret)
		debug("%s:%d update cfg error! ret=%d\n", __func__, __LINE__, ret);

	/* second, detect onfi. */
	ret = spinand_read_param_op(spinand, &param, &ts_onfi_spinand_table[onfi_index]);
	if (ret) {
		debug("%s:%d read param error! ret=%d\n", __func__, __LINE__, ret);
	} else {
		for (i = 0; i <= sizeof(param); i++)
			debug(" 0x%x ", ((uint8_t *)&param)[i]);


		if (ts_onfi_spinand_table[onfi_index].devid != id[2])
			ts_onfi_spinand_table[onfi_index].devid = id[2];


		/* reconfig. */
		ret = spinand_match_and_init(spinand, ts_onfi_spinand_table,
					     ARRAY_SIZE(ts_onfi_spinand_table), id[2]);
		if (ret) {
			debug("%s:%d read param error! ret=%d\n", __func__, __LINE__, ret);
			return ret;
		}
	}

	ret = spinand_upd_cfg(spinand, CFG_OTP_ENABLE, false);
	if (ret)
		debug("%s:%d update cfg error! ret=%d\n", __func__, __LINE__, ret);


	return 0;
}

static const struct spinand_manufacturer_ops ts_onfi_spinand_manuf_ops = {
	.detect = ts_onfi_spinand_detect,
	.init = ts_onfi_spinand_init,
};

const struct spinand_manufacturer ts_onfi_spinand_manufacturer = {
	.id = SPINAND_MFR_ONFI,
	.name = "onfi",
	.ops = &ts_onfi_spinand_manuf_ops,
};
