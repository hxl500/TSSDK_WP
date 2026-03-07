/*
 * Tsingmicro Secure Generic Register Files setup
 *
 * needs to be removed from kernel, here just for quick debug
 */

#include <linux/err.h>
#include <linux/mfd/syscon.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

struct ts_sgrf_value {
	const char *desc;
	u32 reg;
	u32 val;
	u32 mask;
	u32 shift;
};

struct ts_sgrf_info {
	const struct ts_sgrf_value *values;
	int num_values;
};

#define TX536_SGRF_SMMU_CFG1 0x54
#define TX536_SGRF_DMA0_CFG0 0x30

static const struct ts_sgrf_value tx536_defaults[] __initconst = {
	{"dma0 manager ns",  TX536_SGRF_DMA0_CFG0, 0x1, 0x1, 1},
	{"dma0 irq ns",      TX536_SGRF_DMA0_CFG0, 0xf, 0x3f, 2}, //dma0 support 4-irqs
	{"dma0 perip ns",    TX536_SGRF_DMA0_CFG0, 0x3, 0x3, 8},
	{"dma0 bypass smmu", TX536_SGRF_SMMU_CFG1, 0x1, 0xf, 27},
	{"dma0 disable lp",  TX536_SGRF_SMMU_CFG1, 0x0, 0x1, 17},
	{"isp disable lp",  TX536_SGRF_SMMU_CFG1, 0x0, 0x1, 6},
};

static const struct ts_sgrf_info tx536_sgrf __initconst = {
	.values = tx536_defaults,
	.num_values = ARRAY_SIZE(tx536_defaults),
};

static const struct of_device_id ts_sgrf_dt_match[] __initconst = {
	{
		.compatible = "ts,tx536-sgrf",
		.data = (void *)&tx536_sgrf,
	},
	{ /* sentinel */ },
};

static int __init ts_sgrf_init(void)
{
	const struct ts_sgrf_info *sgrf_info;
	const struct of_device_id *match;
	struct device_node *np;
	struct regmap *sgrf;
	int ret, i;
	unsigned int temp;

	np = of_find_matching_node_and_match(NULL, ts_sgrf_dt_match,
					     &match);
	if (!np)
		return -ENODEV;
	if (!match || !match->data) {
		pr_err("%s: missing sgrf data\n", __func__);
		return -EINVAL;
	}

	sgrf_info = match->data;

	sgrf = syscon_node_to_regmap(np);
	if (IS_ERR(sgrf)) {
		pr_err("%s: could not get sgrf syscon\n", __func__);
		return PTR_ERR(sgrf);
	}

	for (i = 0; i < sgrf_info->num_values; i++) {
		const struct ts_sgrf_value *val = &sgrf_info->values[i];

		pr_debug("%s: adjusting %s in %#6x to %#10x\n", __func__,
			val->desc, val->reg, val->val<<val->shift);

		regmap_read(sgrf, val->reg, &temp);
		temp = (temp & (~(val->mask<<val->shift))) | (val->val<<val->shift);
		ret = regmap_write(sgrf, val->reg, temp);
		if (ret < 0)
			pr_err("%s: write to %#6x failed with %d\n", __func__, val->reg, ret);
	}

	return 0;
}
postcore_initcall(ts_sgrf_init);
