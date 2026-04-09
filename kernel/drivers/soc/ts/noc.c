/*
 * Tsingmicro Generic Register Files setup
 *
 */

#include <linux/err.h>
#include <linux/mfd/syscon.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

struct ts_noc_value {
	const char *desc;
	u32 reg;
	u32 val;
	u32 mask;
	u32 shift;
};

struct ts_noc_info {
	const struct ts_noc_value *values;
	int num_values;
};

#define TX536_noc_SMMU_STAT 0xF0

static const struct ts_noc_value tx536_defaults[] __initconst = {
};

static const struct ts_noc_info tx536_noc __initconst = {
	.values = tx536_defaults,
	.num_values = ARRAY_SIZE(tx536_defaults),
};

static const struct of_device_id ts_noc_dt_match[] __initconst = {
	{
		.compatible = "ts,tx536-noc",
		.data = (void *)&tx536_noc,
	},
	{ /* sentinel */ },
};

static int __init ts_noc_init(void)
{
	const struct ts_noc_info *noc_info;
	const struct of_device_id *match;
	struct device_node *np;
	struct regmap *noc;
	int ret, i;
	unsigned int temp;

	np = of_find_matching_node_and_match(NULL, ts_noc_dt_match,
					     &match);
	if (!np)
		return -ENODEV;
	if (!match || !match->data) {
		pr_err("%s: missing noc data\n", __func__);
		return -EINVAL;
	}

	noc_info = match->data;

	noc = syscon_node_to_regmap(np);
	if (IS_ERR(noc)) {
		pr_err("%s: could not get noc syscon\n", __func__);
		return PTR_ERR(noc);
	}

	for (i = 0; i < noc_info->num_values; i++) {
		const struct ts_noc_value *val = &noc_info->values[i];

		pr_debug("%s: adjusting %s in %#6x to %#10x\n", __func__,
			val->desc, val->reg, val->val<<val->shift);

		regmap_read(noc, val->reg, &temp);
		temp = (temp & (~(val->mask<<val->shift))) | (val->val<<val->shift);
		ret = regmap_write(noc, val->reg, temp);
		if (ret < 0)
			pr_err("%s: write to %#6x failed with %d\n", __func__, val->reg, ret);
	}

	return 0;
}
postcore_initcall(ts_noc_init);
