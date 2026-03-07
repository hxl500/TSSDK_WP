/*
 * Tingmicro On-Chip OTP driver
 *
 * Copyright (C) 2015 Stefan Wahren <stefan.wahren@i2se.com>
 *
 * Based on the driver from Huang Shijie and Christoph G. Baumann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/nvmem-provider.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/stmp_device.h>
#include <linux/reset.h>
#include <linux/regulator/consumer.h>

/* OCOTP registers and bits */
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

static ts_otpc_reg_t *ts_otpc_regs;

struct ts_ocotp {
	struct clk *clk;
	void __iomem *base;
	struct nvmem_device *nvmem;
	struct nvmem_config *config;
};

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

static int32_t ts_otpc_read_fuse(void *base, uint32_t word, uint32_t *val)
{
	uint32_t status0;
	uint32_t flag = 0;
	uint32_t temp = 0;
	int32_t retry = RD_RETRY_TIMES;
	uint32_t i = 0xfffff;
	// uint32_t user_space_flag = 0;  //open read permission for pid

	// user_space_flag = is_ts_efuse_uer_space(word);
	// if(user_space_flag == 0) {
	// pr_err("addr %d cannot access,user space is %d to %d\n",
	// word, efuse_user_space_min, efuse_user_space_max);
	// *val = 0xFFFFFFFF;
	// return -EINVAL;
	// }

	ts_otpc_regs = (ts_otpc_reg_t *)base;

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

/*
 * efuse memory have 32 words,
 * a word has 4 bytes
 */

static int ts_ocotp_read(void *context, unsigned int offset,
			  void *val, size_t bytes)
{
	struct ts_ocotp *otp = context;
	u32 *buf = val;
	u32 word_data;
	// int ret;
	// pr_err("base=0x%x, offset=%d, bytes= %d", (u32)otp->base, offset, bytes);

	while (bytes) {
		ts_otpc_read_fuse(otp->base, offset/BYTE_PER_WORD, &word_data);
		// pr_err("word_offset=%d, bytes= 0x%08x\n", offset/BYTE_PER_WORD, word_data);
		*buf++ = word_data;
		bytes -= 4;
		offset += 4;
	}

	return 0;
}

static int32_t ts_otpc_efuse_set_bit(void *base, uint32_t word, uint32_t bit)
{
	uint32_t flag = 0;
	uint32_t temp = 0;
	int32_t retry = PRG_RETRY_TIMES;

	ts_otpc_regs = (ts_otpc_reg_t *)base;

	while ((ts_otpc_regs->OTPC_STATUS0 & TS_OTPC_STATUS0_BUSY_MASK) == TS_OTPC_BUSY)
		;

	ts_otpc_regs->OTPC_MARGIN_ADDR = TS_OTPC_PRG_WORD(word) | TS_OTPC_PRG_BIT(bit);
	udelay(500);
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

int32_t ts_otpc_write_fuse(void *base, uint32_t word, uint32_t val)
{
	int32_t ret;
	int32_t idx;
	uint32_t user_space_flag = 0;

	user_space_flag = is_ts_efuse_uer_space(word);
	// if (user_space_flag == 0) {
	//	pr_err("addr %d cannot access,user space is %d to %d !!\n",
	//			 word, efuse_user_space_min, efuse_user_space_max);
	//	return -EINVAL;
	// }

	for (idx = 0; idx < BIT_PER_WORD; idx++) {
		if ((val >> idx) & 0x1) {
			ret = ts_otpc_efuse_set_bit(base, word, idx);

			if (ret != 0) {
				pr_err("drv_efuse_set_bit in write key error %d\n", ret);
				return ret;
			}
		}
	}

	return 0;
}

#if defined(CONFIG_ARCH_TS_TX536)
static int32_t efuse_power_on_flag;
static int32_t ts_efuse_power_on(struct device *dev, int32_t flag)
{
	int ret;
	int v;
	struct regulator *regulator;

	pr_err("ts efuse power on\n");

	regulator = devm_regulator_get_optional(dev, "name");
	if (IS_ERR(regulator)) {
		ret = PTR_ERR(regulator);
		dev_err(dev, "Failed to request %d\n", ret);
		// return ret;
		return 0;
	}

	ret = regulator_set_voltage(regulator, 1800000, 1800000);
	if (ret) {
		dev_err(dev, "regulator_set_voltage error\n");
		return ret;
	}

	v = regulator_get_voltage(regulator);
	// printk(KERN_INFO "voltage:%d", v);

	ret = regulator_enable(regulator);
	if (ret) {
		dev_err(dev, "Couldn't disable regulator\n");
		return ret;
	}

	if (flag)
		efuse_power_on_flag = 1;
	else
		efuse_power_on_flag = 0;

	return 0;
}
#endif

static int ts_ocotp_write(void *context, unsigned int offset, void *val,
			   size_t bytes)
{
	struct ts_ocotp *otp = context;
	u32 *buf = val;

	#if defined(CONFIG_ARCH_TS_TX536)
	if (!efuse_power_on_flag)
		ts_efuse_power_on(otp->config->dev, 1);
	#endif

	dev_info(otp->config->dev, "input argument: offset=%d, bytes= %d\n", offset, (unsigned int)bytes);

	/* allow only writing one complete OTP word at a time */
	if ((bytes % otp->config->word_size) ||
		(offset % otp->config->word_size)) {
		dev_err(otp->config->dev,
		"argument need multiple of %d, offset=%d, bytes=%d\n",
		otp->config->word_size, offset, (unsigned int)bytes);
		return -EINVAL;
	}


	while (bytes) {
		dev_info(otp->config->dev, "write:word_offset=%d, word_data= 0x%08x\n", offset/BYTE_PER_WORD, *buf);
		ts_otpc_write_fuse(otp->base, offset/BYTE_PER_WORD, *buf);
		buf++;
		bytes -= 4;
		offset += 4;
	}

	return 0;

}

static struct nvmem_config ocotp_config = {
	.name = "ts-ocotp",
	.stride = 4,
	.word_size = 4,
	.owner = THIS_MODULE,
	.reg_read = ts_ocotp_read,
	.reg_write = ts_ocotp_write,
};

struct ts_data {
	int size;
};

static const struct ts_data ts_otp_data = {
	#if defined(CONFIG_ARCH_TS_TX5112)
	.size = 0x40,
	#else
	.size = 0x78,
	#endif
};

static const struct of_device_id ts_ocotp_match[] = {
	{ .compatible = "ts,ts-ocotp", .data = &ts_otp_data },
	{ /* sentinel */},
};
MODULE_DEVICE_TABLE(of, ts_ocotp_match);

static int ts_ocotp_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct ts_data *data;
	struct ts_ocotp *otp;
	struct resource *res;
	const struct of_device_id *match;
	int ret;
	struct reset_control *rstc;

	match = of_match_device(dev->driver->of_match_table, dev);
	if (!match || !match->data)
		return -EINVAL;

	otp = devm_kzalloc(dev, sizeof(*otp), GFP_KERNEL);
	if (!otp)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	otp->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(otp->base))
		return PTR_ERR(otp->base);

#if 1
	otp->clk = devm_clk_get(&pdev->dev, "otpc_pclk");
	if (IS_ERR(otp->clk))
		return PTR_ERR(otp->clk);

	ret = clk_prepare_enable(otp->clk);
	if (ret < 0) {
		dev_err(dev, "failed to prepare clk: %d\n", ret);
		return ret;
	}
#endif

	rstc = devm_reset_control_get_optional_exclusive(&pdev->dev, "ts_otpc");
	if (IS_ERR(rstc)) {
		ret = PTR_ERR(rstc);
		dev_err(&pdev->dev, "Cannot get pdm reset.\n");
		return -1;
	}
	reset_control_deassert(rstc);

	data = match->data;

	ocotp_config.size = data->size;
	ocotp_config.priv = otp;
	ocotp_config.dev = dev;
	otp->config = &ocotp_config;
	otp->nvmem = nvmem_register(&ocotp_config);
	if (IS_ERR(otp->nvmem)) {
		ret = PTR_ERR(otp->nvmem);
		goto err_clk;
	}

	platform_set_drvdata(pdev, otp);

	dev_info(&pdev->dev, "TS OTPC driver loaded\n");

	return 0;

err_clk:
	clk_unprepare(otp->clk);

	return ret;
}

static int ts_ocotp_remove(struct platform_device *pdev)
{
	struct ts_ocotp *otp = platform_get_drvdata(pdev);

	clk_unprepare(otp->clk);

	return nvmem_unregister(otp->nvmem);
}

static struct platform_driver ts_ocotp_driver = {
	.probe = ts_ocotp_probe,
	.remove = ts_ocotp_remove,
	.driver = {
		.name = "ts-ocotp",
		.of_match_table = ts_ocotp_match,
	},
};

module_platform_driver(ts_ocotp_driver);
MODULE_AUTHOR("ys <ys@tsingmicro.com>");
MODULE_DESCRIPTION("driver for OCOTP in DTxx");
MODULE_LICENSE("GPL v2");
