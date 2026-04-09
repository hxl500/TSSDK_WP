// SPDX-License-Identifier: GPL-2.0+
/*
 * Tsingmicro otpc Driver
 *
 * Copyright (C) 2014
 *
 */
// #define DEBUG
#include <common.h>
#include <dm.h>
#include <fuse.h>
#include <log.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <asm/arch/sys_proto.h>
#include <dm/of_access.h>
#include <dm/ofnode.h>
#include <errno.h>
#include <clk.h>

#if defined(CONFIG_ARCH_TS_TX536)
#include <power/pmic.h>
#include <power/regulator.h>
#endif

#define BYTE_PER_WORD           (4)
#define BIT_PER_BYTE            (8)
#define BIT_PER_WORD            (32)
#define PRG_RETRY_TIMES         (16)
#define RD_RETRY_TIMES          (16)

#define TS_OTPC_RD_FINISH      (0x00010000)
#define TS_OTPC_PRG_FINISH     (0x01000000)
#define TS_OTPC_RD_WORD(num)   (num << 0)
#define TS_OTPC_PRG_WORD(num)  (num << 16)
#define TS_OTPC_PRG_BIT(num)   (num << 22)

#define TS_OTPC_BUSY  (0x1)
#define TS_OTPC_RD_STAT_FINISH     (0 << 16)
#define TS_OTPC_PRG_STAT_FINISH     (0 << 24)

/*CTRL BIT MASK*/
#define TS_OTPC_CTRL_RD_START_APB_MASK            (0x1UL<<0)
#define TS_OTPC_CTRL_UPDATE_SR_MASK               (0x1UL<<1)
#define TS_OTPC_CTRL_PRG_START_APB_MASK           (0x1UL<<16)
#define TS_OTPC_CTRL_PRG_SEL_APB_MASK             (0x1UL<<24)

/*STATUS0 BIT MASK*/
#define TS_OTPC_STATUS0_BUSY_MASK                 (0x1UL<<0)
#define TS_OTPC_STATUS0_INIT_DONE_MASK            (0x1UL<<16)
#define TS_OTPC_STATUS0_PRG_FINISH_MASK           (0x1UL<<24)

/*STATUS1 BIT MASK*/
#define TS_OTPC_STATUS1_I_JTAG_DISABLE_MASK       (0x1UL<<0)
#define TS_OTPC_STATUS1_I_RD_STATE_MASK           (0x7UL<<16)
#define TS_OTPC_STATUS1_I_PRG_STATE_MASK          (0xFUL<<24)

typedef struct {
	uint32_t OTPC_CTRL;  /* Offset: 0x000h (R/W)  EFUSE CTRL Register */
	uint32_t OTPC_STATUS0;/* Offset: 0x004h (R/W)  EFUSE STATUS0 Register */
	uint32_t OTPC_STATUS1;/* Offset: 0x008h (R/W)  EFUSE STATUS1 Register */
	uint32_t OTPC_MARGIN_ADDR;/* Offset: 0x00ch (R/W)  EFUSE MARGIN Register */
	uint32_t OTPC_DATA;/* Offset: 0x010h (R/W)  EFUSE DATA Register */
	uint32_t OTPC_PRG_CTL0;/* Offset: 0x014h (R/W)  EFUSE PROGRAM CTL0 Register */
	uint32_t OTPC_PRG_CTL1;/* Offset: 0x018h (R/W)  EFUSE PROGRAM CTL1 Register */
	uint32_t OTPC_PRG_CTL2;/* Offset: 0x01ch (R/W)  EFUSE PROGRAM CTL2 Register */
	uint32_t OTPC_PRG_CTL3;/* Offset: 0x020h (R/W)  EFUSE PROGRAM CTL3 Register */
	uint32_t OTPC_PRG_CTL4;/* Offset: 0x024h (R/W)  EFUSE PROGRAM CTL4 Register */
	uint32_t OTPC_PRG_CTL5;/* Offset: 0x028h (R/W)  EFUSE PROGRAM CTL5 Register */
	uint32_t OTPC_PRG_CTL6;/* Offset: 0x02ch (R/W)  EFUSE PROGRAM CTL6 Register */
	uint32_t OTPC_PRG_CTL7;/* Offset: 0x030h (R/W)  EFUSE PROGRAM CTL7 Register */
	uint32_t OTPC_PRG_CTL8;/* Offset: 0x034h (R/W)  EFUSE PROGRAM CTL8 Register */
	uint32_t OTPC_RD_CTL0;/* Offset: 0x038h (R/W)  EFUSE READ CTL0 Register */
	uint32_t OTPC_RD_CTL1;/* Offset: 0x03ch (R/W)  EFUSE READ CTL1 Register */
	uint32_t OTPC_RD_CTL2;/* Offset: 0x040h (R/W)  EFUSE READ CTL2 Register */
	uint32_t OTPC_RD_CTL3;/* Offset: 0x044h (R/W)  EFUSE READ CTL3 Register */
	uint32_t OTPC_RD_CTL4;/* Offset: 0x048h (R/W)  EFUSE READ CTL4 Register */
	uint32_t OTPC_RD_CTL5;/* Offset: 0x04ch (R/W)  EFUSE READ CTL5 Register */
	uint32_t OTPC_RD_CTL6;/* Offset: 0x050h (R/W)  EFUSE READ CTL6 Register */
} ts_otpc_reg_t;

// #define TS_OTPC_BASE 0xF0D92000

static ts_otpc_reg_t *ts_otpc_regs;    // (ts_otpc_reg_t *)TS_OTPC_BASE;

#if defined(CONFIG_ARCH_TS_TX5112)
uint32_t efuse_user_space_min = 7;
uint32_t efuse_user_space_max = 14;
#else
uint32_t efuse_user_space_min = 18;
uint32_t efuse_user_space_max = 28;
#endif

static int32_t is_ts_efuse_uer_space(uint32_t word)
{
	if ((word >= efuse_user_space_min) && (word <= efuse_user_space_max))
		return 1;
	else
		return 0;
}

#if defined(CONFIG_ARCH_TS_TX536)
static int32_t efuse_power_on_flag;
static int32_t ts_efuse_power_on(int32_t flag)
{
	struct udevice *dev;
	int ret;
	char *name = "LDO6";

	ret = pmic_get("ip6103@30", &dev);

	ret = regulators_enable_boot_on(false);
	if (ret)
		return ret;

	ret = regulator_get_by_platname(name, &dev);
	if (ret) {
		debug("%s: Cannot find regulator %s\n", __func__, name);
		return ret;
	}
	ret = regulator_set_value(dev, 1800000);
	if (ret) {
		debug("%s: Cannot set regulator %s\n", __func__, name);
		return ret;
	}

	ret = regulator_set_enable(dev, flag);
	if (ret) {
		debug("%s: Cannot enable %s\n", __func__, name);
		return ret;
	}

	if (flag)
		efuse_power_on_flag = 1;
	else
		efuse_power_on_flag = 0;

	return 0;
}
#endif

static int32_t ts_otpc_clk_enable(void)
{
	int ret;
	struct clk clk;
	struct udevice *dev;

	#if CONFIG_IS_ENABLED(CLK)
	ret = uclass_get_device_by_driver(UCLASS_MISC,
						  DM_GET_DRIVER(ts_otpc),
						  &dev);
	if (ret)
		return ret;

	ret = clk_get_by_name(dev, "otpc_pclk", &clk);
	if (ret) {
		dev_err(dev, "failed to get clock\n");
		return ret;
	}

	ret = clk_enable(&clk);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP) {
		clk_free(&clk);
		dev_err(dev, "failed to enable clock\n");
		return ret;
	}
	#endif

	return ret;
}

static int32_t ts_otpc_read_fuse(uint32_t word, uint32_t *val)
{
	uint32_t status0;
	uint32_t flag = 0;
	uint32_t temp = 0;
	int32_t retry = RD_RETRY_TIMES;
	uint32_t i = 0xfffff;
	// uint32_t user_space_flag = 0; //open read permission for pid

	// user_space_flag = is_ts_efuse_uer_space(word);
	// if(user_space_flag == 0) {
	// printf("addr %d cannot access,user space is %d to %d\n",
	//        word, efuse_user_space_min, efuse_user_space_max);
	// return -EINVAL;
	// }

#if defined(CONFIG_ARCH_TS_TX536)
	if (!efuse_power_on_flag)
		ts_efuse_power_on(1);
#endif

	ts_otpc_clk_enable();

	do {
		status0 = ts_otpc_regs->OTPC_STATUS0;
		if (status0 & TS_OTPC_STATUS0_INIT_DONE_MASK)
			break;
		i--;
		if (i == 0)
			return -EINVAL;
	} while (1);

	ts_otpc_regs->OTPC_MARGIN_ADDR = TS_OTPC_RD_WORD(word);
	udelay(1);
	ts_otpc_regs->OTPC_CTRL |= TS_OTPC_CTRL_RD_START_APB_MASK;

	do {
		temp = ts_otpc_regs->OTPC_STATUS1;
		flag = temp & TS_OTPC_STATUS1_I_RD_STATE_MASK;
		if (flag == TS_OTPC_RD_STAT_FINISH)
			break;

		udelay(1);
	} while (retry--);

	if (retry == 0)
		return -EINVAL;

	*val = ts_otpc_regs->OTPC_DATA;

	return 0;
}

static int32_t ts_otpc_efuse_set_bit(uint32_t word, uint32_t bit)
{
	uint32_t flag = 0;
	uint32_t temp = 0;
	int32_t retry = PRG_RETRY_TIMES;

	debug("drv_efuse_set_bit efuse offset 0x%x bit 0x%x\n", word, bit);

	while ((ts_otpc_regs->OTPC_STATUS0 & TS_OTPC_STATUS0_BUSY_MASK) == TS_OTPC_BUSY)
		;

	ts_otpc_regs->OTPC_MARGIN_ADDR = TS_OTPC_PRG_WORD(word) | TS_OTPC_PRG_BIT(bit);
	udelay(1);
	ts_otpc_regs->OTPC_CTRL |= TS_OTPC_CTRL_PRG_START_APB_MASK;

	do {
		temp = ts_otpc_regs->OTPC_STATUS1;
		flag = temp & TS_OTPC_STATUS1_I_PRG_STATE_MASK;
		if (flag == TS_OTPC_PRG_STAT_FINISH)
			break;

		udelay(1);
	} while (retry--);

	if (retry == 0)
		return -EINVAL;

	return 0;
}

int32_t ts_otpc_write_fuse(uint32_t word, uint32_t val)
{
	int32_t ret;
	int32_t idx;
	uint32_t user_space_flag = 0;

	user_space_flag = is_ts_efuse_uer_space(word);
	// if (user_space_flag == 0) {
	//	printf("addr %d cannot access,user space is %d to %d\n",
	//			word, efuse_user_space_min, efuse_user_space_max);
	//	return -EINVAL;
	// }

#if defined(CONFIG_ARCH_TS_TX536)
	if (!efuse_power_on_flag)
		ts_efuse_power_on(1);
#endif

	ts_otpc_clk_enable();

	for (idx = 0; idx < BIT_PER_WORD; idx++) {
		if ((val >> idx) & 0x1) {
			ret = ts_otpc_efuse_set_bit(word, idx);

			if (ret != 0) {
				debug("drv_efuse_set_bit in write key error %d\n", ret);
				return ret;
			}
		}
	}

	return 0;
}
/*
 * tx536 efuse has 32 words, bank0 - bank3
 * a bank has 8 words : word0 - word7
 * every word has 4 bytes
 * bank = word_No / 8
 * word = word_No % 8
 */
static int ts_otpc_valid(u32 bank, u32 word)
{
	if (bank > 4)
		return -EINVAL;
	if (word > 7)
		return -EINVAL;
	return 0;
}

/*
 * The 'fuse' command API
 */
int fuse_read(u32 bank, u32 word, u32 *val)
{
	int ret;

	ret = ts_otpc_valid(bank, word);
	if (ret)
		return ret;

	return ts_otpc_read_fuse((bank << 3) | word, val);
}

int fuse_prog(u32 bank, u32 word, u32 val)
{
	int ret;

	ret = ts_otpc_valid(bank, word);
	if (ret)
		return ret;

	return ts_otpc_write_fuse((bank << 3) | word, val);
}

int fuse_sense(u32 bank, u32 word, u32 *val)
{
	/* We do not support sensing :-( */
	pr_err("We do not support sensing :-(\n");
	return -EINVAL;
}

int fuse_override(u32 bank, u32 word, u32 val)
{
	/* We do not support overriding :-( */
	pr_err("We do not support overriding :-(\n");
	return -EINVAL;
}

bool ts_otpc_init_flag;
static int ts_otpc_bind(struct udevice *dev)
{
	fdt_addr_t addr;

	if (ts_otpc_init_flag)
		return 0;

	debug("pinctrl_probe\n");

	addr = dev_read_addr(dev);
	if (addr == FDT_ADDR_T_NONE) {
		printf("%s get addr0 failed", __func__);
		return -EINVAL;
	}

	ts_otpc_regs = (ts_otpc_reg_t *)addr;

	debug("ts_otpc_regs_base:%08x\n", (u32)addr);

	printf("ts otpc bind,base:%08x\n", (u32)addr);
	ts_otpc_init_flag = true;

	return 0;
}

static const struct udevice_id ts_otpc_match[] = {
	{ .compatible = "tsingmicro,ts-otpc" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(ts_otpc) = {
	.name = "ts_otpc",
	.id = UCLASS_MISC,
	.of_match = ts_otpc_match,
	.bind = ts_otpc_bind,
};
