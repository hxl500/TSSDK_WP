/*
 * Tsingmicro Generic Register Files setup
 *
 */

#include <linux/err.h>
#include <linux/mfd/syscon.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

struct ts_grf_value {
	const char *desc;
	u32 reg;
	u32 val;
	u32 mask;
	u32 shift;
};

struct ts_grf_info {
	const struct ts_grf_value *values;
	int num_values;
};

#define TX536_GRF_SMMU_STAT 0xF0
#define TX536_GRF_GMAC_CFG 0x34
#define TX536_GRF_DDRC_CFG0	0x38
#define TX536_GRF_DDRC_CFG1	0x3c

#define GRF_MII_MODE 0x0
#define GRF_RGMII_MODE 0x1
#define GRF_RMII_MODE 0x4
#define GRF_GMII_MODE 0x0

static const struct ts_grf_value tx536_defaults[] __initconst = {
	{"gmac_cfg", TX536_GRF_GMAC_CFG, GRF_RGMII_MODE, 0x7, 0},
	/*{"ddr_cfg", TX536_GRF_DDRC_CFG0, 0xf, 0xf, 24},*/
};

static const struct ts_grf_info tx536_grf __initconst = {
	.values = tx536_defaults,
	.num_values = ARRAY_SIZE(tx536_defaults),
};

static const struct of_device_id ts_grf_dt_match[] __initconst = {
	{
		.compatible = "ts,tx536-grf",
		.data = (void *)&tx536_grf,
	},
	{ /* sentinel */ },
};

static int __init ts_grf_init(void)
{
	const struct ts_grf_info *grf_info;
	const struct of_device_id *match;
	struct device_node *np;
	struct regmap *grf;
	int ret, i;
	unsigned int temp;
	uint64_t pm;

	np = of_find_matching_node_and_match(NULL, ts_grf_dt_match,
					     &match);
	if (!np)
		return -ENODEV;
	if (!match || !match->data) {
		pr_err("%s: missing grf data\n", __func__);
		return -EINVAL;
	}

	grf_info = match->data;

	grf = syscon_node_to_regmap(np);
	if (IS_ERR(grf)) {
		pr_err("%s: could not get grf syscon\n", __func__);
		return PTR_ERR(grf);
	}

	for (i = 0; i < grf_info->num_values; i++) {
		const struct ts_grf_value *val = &grf_info->values[i];

		pr_debug("%s: adjusting %s in %#6x to %#10x\n", __func__,
			val->desc, val->reg, val->val<<val->shift);

		regmap_read(grf, val->reg, &temp);
		temp = (temp & (~(val->mask<<val->shift))) | (val->val<<val->shift);
		ret = regmap_write(grf, val->reg, temp);
		if (ret < 0)
			pr_err("%s: write to %#6x failed with %d\n", __func__, val->reg, ret);
	}

	ret = of_property_read_u64(np, "grf_mctl_qos_0", &pm);
	if (!ret) {
		ret = regmap_write(grf, TX536_GRF_DDRC_CFG0, pm);
		if (ret < 0)
			pr_err("%s: write to %#6x failed with %d\n", __func__, TX536_GRF_DDRC_CFG0, ret);
	} else {
		pr_err("%s: read grf_mctl_qos_0 %d\n", __func__, ret);
	}

	ret = of_property_read_u64(np, "grf_mctl_qos_1", &pm);
	if (!ret) {
		ret = regmap_write(grf, TX536_GRF_DDRC_CFG1, pm);
		if (ret < 0)
			pr_err("%s: write to %#6x failed with %d\n", __func__, TX536_GRF_DDRC_CFG1, ret);
	} else {
		pr_err("%s: read grf_mctl_qos_1 %d\n", __func__, ret);
	}

	return 0;
}
postcore_initcall(ts_grf_init);
