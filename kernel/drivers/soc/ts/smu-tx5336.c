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
#include <linux/reboot.h>
#include <asm/proc-fns.h>

struct tsm_smu_value {
	const char *desc;
	u32 reg;
	u32 val;
	u32 mask;
	u32 shift;
};

struct tsm_smu_info {
	const struct tsm_smu_value *values;
	int num_values;
};

#define tx5336_SMU_CHIP_PWR_MODE_CFG	0xc
#define tx5336_SMU_CHIP_WAKE_UP_CFG		0x40
#define tx5336_SMU_WDT_RESET_CTL		0x10
#define tx5336_SMU_RTC_CLK_CFG			0x80

struct regmap *g_smu;
static const struct tsm_smu_value tx5336_defaults[] __initconst = {
	//{"disable exitmode", tx5336_SMU_CHIP_PWR_MODE_CFG, 0x0, 0x1, 0},
	//{"chip wake up", tx5336_SMU_CHIP_WAKE_UP_CFG, 0x1, 0x1, 0},
	{"top wdt reset", tx5336_SMU_WDT_RESET_CTL, 0x1, 0x1, 4},
	//{"rtc clk enable", tx5336_SMU_RTC_CLK_CFG, 0x3, 0x3, 0},
};

static const struct tsm_smu_info tx5336_smu __initconst = {
	.values = tx5336_defaults,
	.num_values = ARRAY_SIZE(tx5336_defaults),
};

static const struct of_device_id tsm_smu_dt_match[] __initconst = {
	{
		.compatible = "ts,tx5336-smu",
		.data = (void *)&tx5336_smu,
	},
	{ /* sentinel */ },
};
static int ts_restart_handler(struct notifier_block *this,
				unsigned long mode, void *cmd)
{
	regmap_write(g_smu, tx5336_SMU_WDT_RESET_CTL, 0x1);

	while (1)
		cpu_do_idle();

	return NOTIFY_DONE;
}

static struct notifier_block ts_restart_nb = {
	.notifier_call = ts_restart_handler,
	.priority = 128,
};

static int __init tsm_smu_init(void)
{
	const struct tsm_smu_info *smu_info;
	const struct of_device_id *match;
	struct device_node *np;
	struct regmap *smu;
	int ret, i;
	unsigned int temp;

	np = of_find_matching_node_and_match(NULL, tsm_smu_dt_match,
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
	g_smu = smu;
	for (i = 0; i < smu_info->num_values; i++) {
		const struct tsm_smu_value *val = &smu_info->values[i];

		pr_debug("%s: adjusting %s in %#6x to %#10x\n", __func__,
			val->desc, val->reg, val->val<<val->shift);

		regmap_read(smu, val->reg, &temp);
		//pr_err("%s: read to 0x%x temp 0x%x\n", __func__, val->reg, temp);
		temp = (temp & (~(val->mask<<val->shift))) | (val->val<<val->shift);
		ret = regmap_write(smu, val->reg, temp);
		if (ret < 0)
			pr_err("%s: write to %#6x failed with %d\n", __func__, val->reg, ret);

		//pr_err("%s: write to 0x%x temp 0x%x\n", __func__, val->reg, temp);
	}

	ret = register_restart_handler(&ts_restart_nb);
	if (ret)
		pr_err("cannot register restart handler (err=%d)\n", ret);

	return 0;
}
postcore_initcall(tsm_smu_init);
