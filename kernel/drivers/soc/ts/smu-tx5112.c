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

#define TX5112_SMU_CHIP_WAKE_UP_CFG		0x40
#define TX5112_SMU_WDT_RESET_CTL		0x10
#define TX5112_SMU_RTC_CLK_CFG			0x80

struct regmap *g_smu;
static const struct tsm_smu_value tx5112_defaults[] __initconst = {
	{"chip wake up", TX5112_SMU_CHIP_WAKE_UP_CFG, 0x1, 0x3, 0},
	{"top wdt reset", TX5112_SMU_WDT_RESET_CTL, 0x1, 0x1, 4},
	{"rtc clk enable", TX5112_SMU_RTC_CLK_CFG, 0x3, 0x3, 0},
};

static const struct tsm_smu_info tx5112_smu __initconst = {
	.values = tx5112_defaults,
	.num_values = ARRAY_SIZE(tx5112_defaults),
};

static const struct of_device_id tsm_smu_dt_match[] __initconst = {
	{
		.compatible = "ts,tx5112-smu",
		.data = (void *)&tx5112_smu,
	},
	{ /* sentinel */ },
};	
static int ts_restart_handler(struct notifier_block *this,
				unsigned long mode, void *cmd)
{
	regmap_write(g_smu, TX5112_SMU_WDT_RESET_CTL, 0x1);

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
		temp = (temp & (~(val->mask<<val->shift))) | (val->val<<val->shift);
		ret = regmap_write(smu, val->reg, temp);
		if (ret < 0)
			pr_err("%s: write to %#6x failed with %d\n", __func__, val->reg, ret);
	}
	
	ret = register_restart_handler(&ts_restart_nb);
	if (ret)
		pr_err("cannot register restart handler (err=%d)\n", ret);

	return 0;
}
postcore_initcall(tsm_smu_init);
