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

struct ts_smu_value {
	const char *desc;
	u32 reg;
	u32 val;
	u32 mask;
	u32 shift;
};

struct ts_smu_info {
	const struct ts_smu_value *values;
	int num_values;
};

#define TX536_SMU_WDT_RESET_CTL 0x38
#define TX536_SMU_VI_PWR_CTL 0x70
#define TX536_SMU_VI_LP_CTL 0x7c
#define TX536_SMU_VO_PWR_CTL 0x80
#define TX536_SMU_VO_LP_CTL 0x8c

#define TX536_SMU_RTC_CLK_CTL 0xf0

static const struct ts_smu_value tx536_defaults[] __initconst = {
	{"power on vi",      TX536_SMU_VI_PWR_CTL, 0x1, 0x3, 0},
	{"vi exit low pwr",  TX536_SMU_VI_LP_CTL, 0x2, 0x3, 0},
	{"power on vo",      TX536_SMU_VO_PWR_CTL, 0x1, 0x3, 0},
	{"vo exit low pwr",  TX536_SMU_VO_LP_CTL, 0x2, 0x3, 0},
	{"internal rtc clk",  TX536_SMU_RTC_CLK_CTL, 0x3, 0x3, 0},
	{"top wdt reset",  TX536_SMU_WDT_RESET_CTL, 0x1, 0x1, 4},
	{"sec wdt reset",  TX536_SMU_WDT_RESET_CTL, 0x1, 0x1, 5},
};

static const struct ts_smu_info tx536_smu __initconst = {
	.values = tx536_defaults,
	.num_values = ARRAY_SIZE(tx536_defaults),
};

static const struct of_device_id ts_smu_dt_match[] __initconst = {
	{
		.compatible = "ts,tx536-smu",
		.data = (void *)&tx536_smu,
	},
	{ /* sentinel */ },
};

static int __init ts_smu_init(void)
{
	const struct ts_smu_info *smu_info;
	const struct of_device_id *match;
	struct device_node *np;
	struct regmap *smu;
	int ret, i;
	unsigned int temp;

	np = of_find_matching_node_and_match(NULL, ts_smu_dt_match,
					     &match);
	if (!np)
		return -ENODEV;
	if (!match || !match->data) {
		pr_err("%s: missing smu data\n", __func__);
		return -EINVAL;
	}

	smu_info = match->data;

	smu = syscon_node_to_regmap(np);
	if (IS_ERR(smu)) {
		pr_err("%s: could not get smu syscon\n", __func__);
		return PTR_ERR(smu);
	}

	for (i = 0; i < smu_info->num_values; i++) {
		const struct ts_smu_value *val = &smu_info->values[i];

		pr_debug("%s: adjusting %s in %#6x to %#10x\n", __func__,
			val->desc, val->reg, val->val<<val->shift);

		regmap_read(smu, val->reg, &temp);
		temp = (temp & (~(val->mask<<val->shift))) | (val->val<<val->shift);
		ret = regmap_write(smu, val->reg, temp);
		if (ret < 0)
			pr_err("%s: write to %#6x failed with %d\n", __func__, val->reg, ret);
	}
	msleep(1);

	return 0;
}
postcore_initcall(ts_smu_init);
