// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2016-2017 Micron Technology, Inc.
 *
 * Authors:
 *	Peter Pan <wangyu@tsingmicro.com>
 */

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/mtd/spinand.h>

#define SPINAND_MFR_KOWIN		0x01

#define KOWIN_STATUS_ECC_MASK		GENMASK(7, 4)
#define KOWIN_STATUS_ECC_NO_BITFLIPS	(0 << 4)
#define KOWIN_STATUS_ECC_1TO3_BITFLIPS	(1 << 4)
#define KOWIN_STATUS_ECC_4TO6_BITFLIPS	(3 << 4)
#define KOWIN_STATUS_ECC_7TO8_BITFLIPS	(5 << 4)

#define KOWIN_CFG_CR			BIT(0)

/*
 * As per datasheet, die selection is done by the 6th bit of Die
 * Select Register (Address 0xD0).
 */
#define KOWIN_DIE_SELECT_REG	0xD0

#define KOWIN_SELECT_DIE(x)	((x) << 6)

static SPINAND_OP_VARIANTS(quadio_read_cache_variants,
		SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 2, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(false, 0, 1, NULL, 0));

static SPINAND_OP_VARIANTS(x4_write_cache_variants,
		SPINAND_PROG_LOAD_X4(true, 0, NULL, 0),
		SPINAND_PROG_LOAD(true, 0, NULL, 0));

static SPINAND_OP_VARIANTS(x4_update_cache_variants,
		SPINAND_PROG_LOAD_X4(false, 0, NULL, 0),
		SPINAND_PROG_LOAD(false, 0, NULL, 0));

/* Kowin  KANY1D4S2WD Device */
static SPINAND_OP_VARIANTS(x4_read_cache_variants,
			   SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
			   SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
			   SPINAND_PAGE_READ_FROM_CACHE_OP(true, 0, 1, NULL, 0),
			   SPINAND_PAGE_READ_FROM_CACHE_OP(false, 0, 1, NULL, 0));

static SPINAND_OP_VARIANTS(x1_write_cache_variants,
			   SPINAND_PROG_LOAD(true, 0, NULL, 0));

static SPINAND_OP_VARIANTS(x1_update_cache_variants,
			   SPINAND_PROG_LOAD(false, 0, NULL, 0));

static SPINAND_OP_VARIANTS(read_cache_variants,
		//SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(0, 1, NULL, 0),
		//SPINAND_PAGE_READ_FROM_CACHE_X4_OP(0, 1, NULL, 0),
		//SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(0, 1, NULL, 0),
		//SPINAND_PAGE_READ_FROM_CACHE_X2_OP(0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(true, 0, 1, NULL, 0),
		SPINAND_PAGE_READ_FROM_CACHE_OP(false, 0, 1, NULL, 0));

static SPINAND_OP_VARIANTS(write_cache_variants,
		//SPINAND_PROG_LOAD_X4(true, 0, NULL, 0),
		SPINAND_PROG_LOAD(true, 0, NULL, 0));

static SPINAND_OP_VARIANTS(update_cache_variants,
		//SPINAND_PROG_LOAD_X4(false, 0, NULL, 0),
		SPINAND_PROG_LOAD(false, 0, NULL, 0));

static int kowin_8_ooblayout_ecc(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *region)
{
	if (section)
		return -ERANGE;

	region->offset = mtd->oobsize / 2;
	region->length = mtd->oobsize / 2;

	return 0;
}

static int kowin_8_ooblayout_free(struct mtd_info *mtd, int section,
				   struct mtd_oob_region *region)
{
	if (section)
		return -ERANGE;

	/* Reserve 2 bytes for the BBM. */
	region->offset = 2;
	region->length = (mtd->oobsize / 2) - 2;

	return 0;
}

static const struct mtd_ooblayout_ops kowin_8_ooblayout = {
	.ecc = kowin_8_ooblayout_ecc,
	.free = kowin_8_ooblayout_free,
};

static int kowin_4_ooblayout_ecc(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *region)
{
	struct spinand_device *spinand = mtd_to_spinand(mtd);

	if (section >= spinand->base.memorg.pagesize /
			mtd->ecc_step_size)
		return -ERANGE;

	region->offset = (section * 16) + 8;
	region->length = 8;

	return 0;
}

static int kowin_4_ooblayout_free(struct mtd_info *mtd, int section,
				   struct mtd_oob_region *region)
{
	struct spinand_device *spinand = mtd_to_spinand(mtd);

	if (section >= spinand->base.memorg.pagesize /
			mtd->ecc_step_size)
		return -ERANGE;

	if (section) {
		region->offset = 16 * section;
		region->length = 8;
	} else {
		/* section 0 has two bytes reserved for the BBM */
		region->offset = 2;
		region->length = 6;
	}

	return 0;
}

static const struct mtd_ooblayout_ops kowin_4_ooblayout = {
	.ecc = kowin_4_ooblayout_ecc,
	.free = kowin_4_ooblayout_free,
};

static int kowin_select_target(struct spinand_device *spinand,
				unsigned int target)
{
	struct spi_mem_op op = SPINAND_SET_FEATURE_OP(KOWIN_DIE_SELECT_REG,
						      spinand->scratchbuf);

	if (target > 1)
		return -EINVAL;

	*spinand->scratchbuf = KOWIN_SELECT_DIE(target);

	return spi_mem_exec_op(spinand->spimem, &op);
}

static int kowin_8_ecc_get_status(struct spinand_device *spinand,
				   u8 status)
{
	switch (status & KOWIN_STATUS_ECC_MASK) {
	case STATUS_ECC_NO_BITFLIPS:
		return 0;

	case STATUS_ECC_UNCOR_ERROR:
		return -EBADMSG;

	case KOWIN_STATUS_ECC_1TO3_BITFLIPS:
		return 3;

	case KOWIN_STATUS_ECC_4TO6_BITFLIPS:
		return 6;

	case KOWIN_STATUS_ECC_7TO8_BITFLIPS:
		return 8;

	default:
		break;
	}

	return -EINVAL;
}

static const struct spinand_info kowin_spinand_table[] = {
	/* M78A 1Gb 3.3V */
	SPINAND_INFO("KANY1D4S2WD",
		     SPINAND_ID(SPINAND_READID_METHOD_OPCODE_DUMMY, 0x15),
		     NAND_MEMORG(1, 2048, 64, 64, 1024, 20, 1, 1, 1),
		     NAND_ECCREQ(4, 512),
		     SPINAND_INFO_OP_VARIANTS(&read_cache_variants,
					      &write_cache_variants,
					      &update_cache_variants),
		     0,
		     SPINAND_ECCINFO(&kowin_8_ooblayout,
				     kowin_8_ecc_get_status),
		     SPINAND_SELECT_TARGET(kowin_select_target)),
};

int spinand_read_reg_op(struct spinand_device *spinand, u8 reg, u8 *val);

int spinand_write_reg_op(struct spinand_device *spinand, u8 reg, u8 val);

static int kowin_spinand_init(struct spinand_device *spinand)
{
	/*
	 * M70A device series enable Continuous Read feature at Power-up,
	 * which is not supported. Disable this bit to avoid any possible
	 * failure.
	 */
	 u8 val = 0;

	if (spinand->flags & SPINAND_HAS_CR_FEAT_BIT)
		return spinand_upd_cfg(spinand, KOWIN_CFG_CR, 0);

	/* HWP_EN must be enabled first before block unlock region is set */
	spinand_read_reg_op(spinand, REG_BLOCK_LOCK, &val);
	spinand_write_reg_op(spinand, REG_BLOCK_LOCK, val | (1 << 1));

	return 0;
}

static int kowin_spinand_detect(struct spinand_device *spinand)
{
	u8 *id = spinand->id.data;
	int ret;
	u8 index = ARRAY_SIZE(kowin_spinand_table) - 1;
	static u8 id_val = 0xff;
	/*
	 */
	if (id[0] != SPINAND_MFR_KOWIN) {
		//return 0;
		pr_debug("%s: Manufacturer id = 0x%x\n", __func__, *(uint32_t *)id);
	}

	/* first pre config. */
#if defined(__UBOOT__)
	ret = spinand_match_and_init(spinand, ts_onfi_spinand_table,
				     ARRAY_SIZE(ts_onfi_spinand_table), /*id[2]*/ 0xA5);
	if (ret)
		return ret;
#else
	if (*(kowin_spinand_table[index].devid.id) != id[1]) {
		id_val = id[1];
		//kowin_spinand_table[index].devid.id = &id_val;
	}

	ret = spinand_match_and_init(spinand, kowin_spinand_table,
				     ARRAY_SIZE(kowin_spinand_table), SPINAND_READID_METHOD_OPCODE_DUMMY);
	if (ret)
		return ret;
#endif

	return 1;
}

static const struct spinand_manufacturer_ops kowin_spinand_manuf_ops = {
	.detect = kowin_spinand_detect,
	.init = kowin_spinand_init,
};

const struct spinand_manufacturer kowin_spinand_manufacturer = {
	.id = SPINAND_MFR_KOWIN,
	.name = "Kowin",
	.chips = kowin_spinand_table,
	.nchips = ARRAY_SIZE(kowin_spinand_table),
	.ops = &kowin_spinand_manuf_ops,
};
