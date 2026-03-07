// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2013 Altera Corporation <www.altera.com>
 */

#include <clk.h>
#include <common.h>
#include <dm.h>
#include <reset.h>
#include <wdt.h>
#include <asm/io.h>
#include <asm/utils.h>
#include <linux/bitops.h>

#define DW_WDT_CR	0x00
#define DW_WDT_TORR	0x04
#define DW_WDT_CRR	0x0C

#define WDOG_CONTROL_REG_WDT_EN_MASK		0x01
#define WDOG_CONTROL_REG_RESP_MODE_MASK		0x02
#define WDOG_CONTROL_REG_RESP_PULSE_LENGTH_MASK		0x07
#define WDOG_CONTROL_REG_RESP_PULSE_LENGTH_POS		(2)
#define WDOG_CONTROL_REG_RESET_MODE_MASK		0x1
#define WDOG_CONTROL_REG_RESET_MODE_POS			(1)


#define DW_WDT_CR_EN_OFFSET	0x00
#define DW_WDT_CR_RMOD_OFFSET	0x01
#define DW_WDT_CRR_RESTART_VAL	0x76

struct designware_wdt_priv {
	void __iomem	*base;
	unsigned int	clk_khz;
	struct reset_ctl_bulk resets;
};

typedef enum {
	WDT_ResetPulseLength_2_PCLK_CYCLES = 0,
	WDT_ResetPulseLength_4_PCLK_CYCLES,
	WDT_ResetPulseLength_8_PCLK_CYCLES,
	WDT_ResetPulseLength_16_PCLK_CYCLES,
	WDT_ResetPulseLength_32_PCLK_CYCLES,
	WDT_ResetPulseLength_64_PCLK_CYCLES,
	WDT_ResetPulseLength_128_PCLK_CYCLES,
	WDT_ResetPulseLength_256_PCLK_CYCLES,
} eWDT_ResetPulseLength_t;

#define DW_WDT_DEFAULT_RESET_PULSE_LENGTH	WDT_ResetPulseLength_64_PCLK_CYCLES

/*
 * Set the watchdog time interval.
 * Counter is 32 bit.
 */
static int designware_wdt_settimeout(void __iomem *base, unsigned int clk_khz,
				     unsigned int timeout)
{
	signed int i;

	/* calculate the timeout range value */
	i = log_2_n_round_up(timeout * clk_khz) - 16;
	i = clamp(i, 0, 15);
	writel(i | (i << 4), base + DW_WDT_TORR);

	return 0;
}

static void designware_wdt_enable(void __iomem *base)
{
	unsigned int val;

	val = readl(base + DW_WDT_CR);
	val |= (WDOG_CONTROL_REG_WDT_EN_MASK);
	writel(val, base + DW_WDT_CR);
}

static unsigned int designware_wdt_is_enabled(void __iomem *base)
{
	return readl(base + DW_WDT_CR) & BIT(0);
}

static void designware_wdt_reset_common(void __iomem *base)
{
	if (designware_wdt_is_enabled(base))
		/* restart the watchdog counter */
		writel(DW_WDT_CRR_RESTART_VAL, base + DW_WDT_CRR);

	//printf("%s:%d\n", __func__, __LINE__);
}

#if !CONFIG_IS_ENABLED(WDT)
void hw_watchdog_reset(void)
{
	designware_wdt_reset_common((void __iomem *)CONFIG_DW_WDT_BASE);
}

void hw_watchdog_init(void)
{
	/* reset to disable the watchdog */
	hw_watchdog_reset();
	/* set timer in miliseconds */
	designware_wdt_settimeout((void __iomem *)CONFIG_DW_WDT_BASE,
				  CONFIG_DW_WDT_CLOCK_KHZ,
				  CONFIG_WATCHDOG_TIMEOUT_MSECS);
	/* enable the watchdog */
	designware_wdt_enable((void __iomem *)CONFIG_DW_WDT_BASE);
	/* reset the watchdog */
	hw_watchdog_reset();
}
#else
static int designware_wdt_reset(struct udevice *dev)
{
	struct designware_wdt_priv *priv = dev_get_priv(dev);

	designware_wdt_reset_common(priv->base);

	return 0;
}

static void designware_wdt_set_reset_pulse_length(void __iomem *base,
					eWDT_ResetPulseLength_t value)
{
	u32 val = readl(base + DW_WDT_CR);

	val &= ~(WDOG_CONTROL_REG_RESP_PULSE_LENGTH_MASK << WDOG_CONTROL_REG_RESP_PULSE_LENGTH_POS);
	val |= (value << WDOG_CONTROL_REG_RESP_PULSE_LENGTH_POS);
	writel(val, base + DW_WDT_CR);
}

static int designware_wdt_stop(struct udevice *dev)
{
	struct designware_wdt_priv *priv = dev_get_priv(dev);
	unsigned int val;
	int ret;

	designware_wdt_reset(dev);

#if CONFIG_IS_ENABLED(DM_RESET)
	ret = reset_assert_bulk(&priv->resets);
	if (ret)
		return ret;

	ret = reset_deassert_bulk(&priv->resets);
	if (ret)
		return ret;
#endif

	val = readl(priv->base + DW_WDT_CR);
	val &= ~(WDOG_CONTROL_REG_WDT_EN_MASK);
	writel(val, priv->base + DW_WDT_CR);

	return 0;
}

static int designware_wdt_start(struct udevice *dev, u64 timeout, ulong flags)
{
	struct designware_wdt_priv *priv = dev_get_priv(dev);

	designware_wdt_stop(dev);

	designware_wdt_set_reset_pulse_length(priv->base, DW_WDT_DEFAULT_RESET_PULSE_LENGTH);

	/* set timer in miliseconds */
	designware_wdt_settimeout(priv->base, priv->clk_khz, timeout);

	designware_wdt_enable(priv->base);

	/* reset the watchdog */
	return designware_wdt_reset(dev);
}

static int designware_wdt_probe(struct udevice *dev)
{
	struct designware_wdt_priv *priv = dev_get_priv(dev);
	__maybe_unused int ret;

	priv->base = dev_remap_addr(dev);

	if (!priv->base)
		return -EINVAL;

#if CONFIG_IS_ENABLED(CLK)
	struct clk clk;

	ret = clk_get_by_index(dev, 1, &clk);
	if (ret)
		return ret;

	priv->clk_khz = clk_get_rate(&clk);
	priv->clk_khz = priv->clk_khz / 1000;
	if (!priv->clk_khz)
		return -EINVAL;
#else
	priv->clk_khz = CONFIG_DW_WDT_CLOCK_KHZ;
#endif

#if CONFIG_IS_ENABLED(DM_RESET)
	ret = reset_get_bulk(dev, &priv->resets);
	if (ret)
		return ret;

	ret = reset_deassert_bulk(&priv->resets);
	if (ret)
		return ret;
#endif
	#define SMU_BASE (0xf12a2000)
	unsigned int regValue;
	regValue = readl((void __iomem *)SMU_BASE + 0x38);
	regValue |= (1 << 4);
	writel(regValue, (void __iomem *)SMU_BASE + 0x38);

	/* reset to disable the watchdog */
	return designware_wdt_stop(dev);
}

static const struct wdt_ops designware_wdt_ops = {
	.start = designware_wdt_start,
	.reset = designware_wdt_reset,
	.stop = designware_wdt_stop,
};

static const struct udevice_id designware_wdt_ids[] = {
	{ .compatible = "snps,dw-wdt"},
	{}
};

U_BOOT_DRIVER(designware_wdt) = {
	.name = "designware_wdt",
	.id = UCLASS_WDT,
	.of_match = designware_wdt_ids,
	.priv_auto_alloc_size = sizeof(struct designware_wdt_priv),
	.probe = designware_wdt_probe,
	.ops = &designware_wdt_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
#endif
