/*
 * Tsingmicro Generic Register Files setup
 *
 */

#include <linux/err.h>
#include <linux/mfd/syscon.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

struct ts_sys_value {
	const char *desc;
	u32 reg;
	u32 val;
	u32 mask;
	u32 shift;
};

struct ts_sys_info {
	const struct ts_sys_value *values;
	int num_values;
};


static const struct of_device_id ts_sys_dt_match[] __initconst = {
	{ /* sentinel */ },
};

static int __init ts_sys_init(void)
{
	const struct ts_sys_info *sys_info;
	const struct of_device_id *match;
	struct device_node *np;
	struct regmap *sys;
	int ret, i;
	unsigned int temp;

	np = of_find_matching_node_and_match(NULL, ts_sys_dt_match,
			&match);
	if (!np)
		return -ENODEV;
	if (!match || !match->data) {
		pr_err("%s: missing sys data\n", __func__);
		return -EINVAL;
	}

	sys_info = match->data;

	sys = syscon_node_to_regmap(np);
	if (IS_ERR(sys)) {
		pr_err("%s: could not get sys syscon\n", __func__);
		return PTR_ERR(sys);
	}

	for (i = 0; i < sys_info->num_values; i++) {
		const struct ts_sys_value *val = &sys_info->values[i];

		pr_err("%s: adjusting %s in %#6x to %#10x\n", __func__,
			val->desc, val->reg, val->val<<val->shift);

		regmap_read(sys, val->reg, &temp);
		temp = (temp & (~(val->mask<<val->shift))) | (val->val<<val->shift);
		ret = regmap_write(sys, val->reg, temp);
		if (ret < 0)
			pr_err("%s: write to %#6x failed with %d\n", __func__, val->reg, ret);
	}

	return 0;
}
postcore_initcall(ts_sys_init);
