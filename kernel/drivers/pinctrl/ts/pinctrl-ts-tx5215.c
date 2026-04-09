/*
 * Zynq pin controller
 *
 *  Copyright (C) 2014 Xilinx
 *
 *  Sören Brinkmann <soren.brinkmann@xilinx.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/io.h>
#include <linux/log2.h>
#include <linux/mfd/syscon.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <dt-bindings/pinctrl/pinctrl-ts-tx5215.h>
#include <linux/regmap.h>
#include "../pinctrl-utils.h"
#include "../core.h"

#define TS_NUM_MIOS	PINMAX

#define TS_PINMUX_MUX_SHIFT	0
#define TS_PINMUX_MUX_MASK	(0x3 << TS_PINMUX_MUX_SHIFT)
#define TS_PINMUX_GPIO_MUX_VAL 8

/*gpio 2-mA driver select*/
#define SCD_GPIO_R2R_REG		0x500
/*gpio 4-mA driver select*/
#define SCD_GPIO_R4R_REG		0x504
/*gpio 8-mA driver select*/
#define SCD_GPIO_R8R_REG		0x508
/*gpio open drain select*/
#define SCD_GPIO_ODR_REG		0x50c
/*gpio pull up select*/
#define SCD_GPIO_PUR_REG		0x510
/*gpio pull down select*/
#define SCD_GPIO_PDR_REG		0x514
/*gpio rate control select*/
#define SCD_GPIO_SLR_REG		0x518
/*gpio digital enabel*/
#define SCD_GPIO_DEN_REG		0x51c
/*gpio analog mode select*/
#define SCD_GPIO_AMSEL_REG		0x528
/*gpio port control*/
#define SCD_GPIO_PCTL_REG		0x52c

/*refer to the kindsof(gpio driver select) */
#define DRV_STREN_KINDS			3

#define PAD_GROUP_A		0x0
#define PAD_GROUP_B		0x10000
#define PAD_GROUP_C		0x20000
#define PAD_GROUP_D		0x30000
#define PAD_GROUP_E		0x40000
#define PAD_GROUP_F		0x50000
#define PAD_GROUP_G		0x60000
#define PAD_GROUP_J		0x80000
#define PAD_GROUP_K		0x90000

static void ts_tx5215_pin_to_offset(unsigned int pin,
	unsigned int *pad_offset, unsigned int *reg_offset)
{

	switch (pin) {
	case PA0 ... PA7:
		*pad_offset = PAD_GROUP_A;
		*reg_offset = pin - PA0;
		break;
	case PB0 ... PB7:
		*pad_offset = PAD_GROUP_B;
		*reg_offset = pin - PB0;
		break;
	case PC0 ... PC7:
		*pad_offset = PAD_GROUP_C;
		*reg_offset = pin - PC0;
		break;
	case PD0 ... PD7:
		*pad_offset = PAD_GROUP_D;
		*reg_offset = pin - PD0;
		break;
	case PE0 ... PE7:
		*pad_offset = PAD_GROUP_E;
		*reg_offset = pin - PE0;
		break;
	case PF0 ... PF7:
		*pad_offset = PAD_GROUP_F;
		*reg_offset = pin - PF0;
		break;
	case PG0 ... PG2:
		*pad_offset = PAD_GROUP_G;
		*reg_offset = pin - PG0;
		break;
	case PJ0 ... PJ5:
		*pad_offset = PAD_GROUP_J;
		*reg_offset = pin - PJ0;
		break;
	case PK0 ... PK6:
		*pad_offset = PAD_GROUP_K;
		*reg_offset = pin - PK0;
		break;
	default:
		pr_info("err: %s pin %d,failed\n", __func__, pin);
	}

}

/**
 * struct ts_pinctrl - driver data
 * @pctrl:		Pinctrl device
 * @reg_base:		Pinctrl reg base
 * @groups:		Pingroups
 * @ngroups:		Number of @groups
 * @funcs:		Pinmux functions
 * @nfuncs:		Number of @funcs
 */
struct ts_pinctrl {
	struct pinctrl_dev *pctrl;
	void __iomem *reg_base;
	struct ts_pctrl_group *groups;
	unsigned int ngroups;
	struct ts_pinmux_function *funcs;
	unsigned int nfuncs;
	struct pinctrl_desc desc;
};

struct ts_pctrl_group {
	const char *name;
	unsigned int *pins;
	unsigned int npins;
	unsigned int mux_val;
};

/**
 * struct ts_pinmux_function - a pinmux function
 * @name:	Name of the pinmux function.
 * @groups:	List of pingroups for this function.
 * @ngroups:	Number of entries in @groups.
 */
struct ts_pinmux_function {
	const char *name;
	const char **groups;
	unsigned int ngroups;
};

/* pinctrl */
static int ts_pctrl_get_groups_count(struct pinctrl_dev *pctldev)
{
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);

	return pctrl->ngroups;
}

static const char *ts_pctrl_get_group_name(struct pinctrl_dev *pctldev,
						unsigned int selector)
{
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);

	return pctrl->groups[selector].name;
}

static int ts_pctrl_get_group_pins(struct pinctrl_dev *pctldev,
					unsigned int selector,
					const unsigned int **pins,
					unsigned int *num_pins)
{
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);

	*pins = pctrl->groups[selector].pins;
	*num_pins = pctrl->groups[selector].npins;

	return 0;
}

static const struct pinctrl_ops ts_pctrl_ops = {
	.get_groups_count = ts_pctrl_get_groups_count,
	.get_group_name = ts_pctrl_get_group_name,
	.get_group_pins = ts_pctrl_get_group_pins,
	.dt_node_to_map = pinconf_generic_dt_node_to_map_all,
	.dt_free_map = pinctrl_utils_free_map,
};

/* pinmux */
static int ts_pmux_get_functions_count(struct pinctrl_dev *pctldev)
{
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);

	return pctrl->nfuncs;
}

static const char *ts_pmux_get_function_name(struct pinctrl_dev *pctldev,
						  unsigned int selector)
{
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);

	return pctrl->funcs[selector].name;
}

static int ts_pmux_get_function_groups(struct pinctrl_dev *pctldev,
					unsigned int selector,
					const char *const **groups,
					unsigned *const num_groups)
{
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);

	*groups = pctrl->funcs[selector].groups;
	*num_groups = pctrl->funcs[selector].ngroups;
	return 0;
}

static void ts_tx5215_gpio_func_sel(struct pinctrl_dev *pctldev, unsigned int pin,
	unsigned int func_selector)
{
	u32 pad_offset;
	u32 reg_offset;
	u32 val;
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);

	switch (pin) {
	case PA0 ... PA7:
		pad_offset = PAD_GROUP_A;
		reg_offset = pin - PA0;
		break;
	case PB0 ... PB7:
		pad_offset = PAD_GROUP_B;
		reg_offset = pin - PB0;
		break;
	case PC0 ... PC7:
		pad_offset = PAD_GROUP_C;
		reg_offset = pin - PC0;
		break;
	case PD0 ... PD7:
		pad_offset = PAD_GROUP_D;
		reg_offset = pin - PD0;
		break;
	case PE0 ... PE7:
		pad_offset = PAD_GROUP_E;
		reg_offset = pin - PE0;
		break;
	case PF0 ... PF3:
	case PF5 ... PF7:
		pad_offset = PAD_GROUP_F;
		reg_offset = pin - PF0;
		break;
	case PG0 ... PG2:
		pad_offset = PAD_GROUP_G;
		reg_offset = pin - PG0;
		break;
	case PJ0 ... PJ5:
		pad_offset = PAD_GROUP_J;
		reg_offset = pin - PJ0;
		break;
	case PJ6:
		pad_offset = PAD_GROUP_J;
		reg_offset = 6;
		break;
	case PK0 ... PK6:
		pad_offset = PAD_GROUP_K;
		reg_offset = pin - PK0;
		break;
	default:
		pr_info("err: %s pin %d, func %d failed\n",
			__func__, pin, func_selector);
		return;
	}

	val = readl(pctrl->reg_base + pad_offset + SCD_GPIO_PCTL_REG);
	reg_offset *= 4;
	val &= ~(0xf << reg_offset);
	val |= (func_selector << reg_offset);
	writel(val, pctrl->reg_base + pad_offset + SCD_GPIO_PCTL_REG);
	// pr_err("debug_info:pin%d, wr 0x%x, val 0x%x\n", pin,
	// pad_offset + SCD_GPIO_PCTL_REG , val);
}

static int ts_pinmux_set_mux(struct pinctrl_dev *pctldev,
							  unsigned int function, unsigned int group)
{
		u32 i;
		struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);
		const struct ts_pctrl_group *pgrp = &pctrl->groups[group];
		const struct ts_pinmux_function *func = &pctrl->funcs[function];

		dev_dbg(pctldev->dev, "%s\n", __func__);
		dev_dbg(pctldev->dev, "set_mux,function:%u,group:%u\n", function,
				group);
		dev_dbg(pctldev->dev, "group name:%s,func name:%s", pgrp->name,
				func->name);
		dev_dbg(pctldev->dev, "pgrp->mux_val:%x\n", pgrp->mux_val);

		for (i = 0; i < pgrp->npins; i++)
			ts_tx5215_gpio_func_sel(pctldev, pgrp->pins[i], pgrp->mux_val);

		return 0;
}

static int ts_gpio_request_enable(struct pinctrl_dev *pctldev,
					struct pinctrl_gpio_range *range,
					unsigned int offset)
{
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);
	struct pinctrl_desc *ts_desc = &pctrl->desc;

	dev_dbg(pctldev->dev, "%s\n", __func__);

	if (offset >= ts_desc->npins) {
		dev_err(pctldev->dev, "error pin %d\n", offset);
		return -EINVAL;
	}

	ts_tx5215_gpio_func_sel(pctldev, offset, TS_PINMUX_GPIO_MUX_VAL);

	return 0;
}

static const struct pinmux_ops ts_pinmux_ops = {
	.get_functions_count = ts_pmux_get_functions_count,
	.get_function_name = ts_pmux_get_function_name,
	.get_function_groups = ts_pmux_get_function_groups,
	.set_mux = ts_pinmux_set_mux,
	.gpio_request_enable = ts_gpio_request_enable,
	// .strict = true,
};

static int ts_pinconf_cfg_get(struct pinctrl_dev *pctldev,
				   unsigned int pin, unsigned long *config)
{
	u32 val, mode_offset, pad_offset, reg_offset;
	unsigned int arg = 0;
	unsigned int param = pinconf_to_config_param(*config);
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);

	if (pin >= TS_NUM_MIOS)
		return -ENOTSUPP;

	ts_tx5215_pin_to_offset(pin, &pad_offset, &reg_offset);
	dev_dbg(pctldev->dev, "cfg_get,pin:%u\n", pin);

	switch (param) {
	case PIN_CONFIG_BIAS_PULL_UP:
		mode_offset = SCD_GPIO_PUR_REG;
		break;
	case PIN_CONFIG_BIAS_PULL_DOWN:
		mode_offset = SCD_GPIO_PDR_REG;
		break;
	case PIN_CONFIG_DRIVE_STRENGTH:
		mode_offset = arg * 0x4 + SCD_GPIO_R2R_REG;
		break;
	default:
		dev_dbg(pctldev->dev,
			"unsupported configuration parameter '%u'\n", param);
		return -ENOTSUPP;
	}

	val = readl(pctrl->reg_base + mode_offset + pad_offset);
	arg = (val >> reg_offset) & 0x1;

	*config = pinconf_to_config_packed(param, arg);
	return 0;
}

static int ts_pinconf_cfg_set(struct pinctrl_dev *pctldev,
				   unsigned int pin,
				   unsigned long *configs, unsigned int num_configs)
{
	int i;
	int drv_stren_idx;
	u32 val, mode_offset, pad_offset, reg_offset;
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);

	if (pin >= TS_NUM_MIOS)
		return -ENOTSUPP;

	dev_dbg(pctldev->dev, "cfg_set,pin:%u,num_configs:%u\n", pin,
		num_configs);

	ts_tx5215_pin_to_offset(pin, &pad_offset, &reg_offset);
	for (i = 0; i < num_configs; i++) {
		unsigned int param = pinconf_to_config_param(configs[i]);
		unsigned int arg = pinconf_to_config_argument(configs[i]);

		switch (param) {
		case PIN_CONFIG_BIAS_PULL_UP:
			mode_offset = SCD_GPIO_PUR_REG;
			/*in order to config pull-up, pull-down should be cleared first*/
			val = readl(pctrl->reg_base + SCD_GPIO_PDR_REG + pad_offset);
			val &= ~(1 << reg_offset);
			writel(val, pctrl->reg_base + SCD_GPIO_PDR_REG + pad_offset);

			break;
		case PIN_CONFIG_BIAS_PULL_DOWN:
			mode_offset = SCD_GPIO_PDR_REG;
			 /*in order to config pull-down, pull-up should be cleared first*/
			val = readl(pctrl->reg_base + SCD_GPIO_PUR_REG + pad_offset);
			val &= ~(1 << reg_offset);
			writel(val, pctrl->reg_base + SCD_GPIO_PUR_REG + pad_offset);

			break;
		case PIN_CONFIG_DRIVE_STRENGTH:
			/* need firstly clear all kinds of driver strength */
			for (drv_stren_idx = 0; drv_stren_idx < DRV_STREN_KINDS; drv_stren_idx++) {
				mode_offset = drv_stren_idx * 0x4 + SCD_GPIO_R2R_REG;
				val = readl(pctrl->reg_base + mode_offset + pad_offset);
				val &= ~(1 << reg_offset);
				writel(val, pctrl->reg_base + mode_offset + pad_offset);
			}

			/* formula to convert target driver strength: arg: 2 4 8 --> drv_stren_idx: 0 1 2 */
			drv_stren_idx =  ilog2(arg) - 1;
			pr_debug("arg:%d drv_stren_idx:%d \r\n", arg, drv_stren_idx);
			mode_offset = drv_stren_idx * 0x4 + SCD_GPIO_R2R_REG;
			break;
		default:
			dev_warn(pctldev->dev,
				 "unsupported configuration parameter '%u'\n",
				 param);
			continue;
		}
		val = readl(pctrl->reg_base + mode_offset + pad_offset);
		val |= 1 << reg_offset;
		writel(val, pctrl->reg_base + mode_offset + pad_offset);
	}

	return 0;
}

static int ts_pinconf_group_set(struct pinctrl_dev *pctldev,
				 unsigned int selector,
				 unsigned long *configs,
				 unsigned int num_configs)
{
	int i, ret;
	struct ts_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);
	const struct ts_pctrl_group *pgrp = &pctrl->groups[selector];

	for (i = 0; i < pgrp->npins; i++) {
		ret = ts_pinconf_cfg_set(pctldev, pgrp->pins[i], configs,
					  num_configs);
		if (ret)
			return ret;
	}

	return 0;
}

static const struct pinconf_ops ts_pinconf_ops = {
	.is_generic = true,
	.pin_config_get = ts_pinconf_cfg_get,
	.pin_config_set = ts_pinconf_cfg_set,
	.pin_config_group_set = ts_pinconf_group_set,
};

static int ts_pinctrl_parse_groups(struct platform_device *pdev,
					struct device_node *np,
					struct ts_pctrl_group *grp,
					struct ts_pinctrl *pctrl, u32 index)
{
	struct device *dev = &pdev->dev;
	int ret;

	dev_dbg(dev, "group(%d): %s\n", index, np->name);

	/* Initialise group */
	ret = of_property_read_u32(np, "mux,val", &grp->mux_val);
	if (ret) {
		dev_err(dev, "read mux,val error\n");
		return ret;
	}

	ret = of_property_count_elems_of_size(np, "ts,pins", sizeof(u32));

	if (ret < 0) {
		dev_err(dev,
			"failed to get the count of ts,pins in %pOF node\n",
			np);
		return ret;
	}
	grp->npins = ret;
	grp->pins = devm_kzalloc(dev, grp->npins * sizeof(unsigned int),
				 GFP_KERNEL);
	if (!grp->pins)
		return -ENOMEM;
	ret = of_property_read_u32_array(np, "ts,pins", grp->pins, grp->npins);
	if (ret) {
		dev_err(dev, "read ts,pins error\n");
		return ret;
	}

	grp->name = np->name;

	return 0;
}

static int init_func_index = -1;
static int init_grp_index = -1;
const char *init_func_name;
static bool have_init_func;

static int ts_pinctrl_parse_functions(struct platform_device *pdev,
					   struct ts_pinctrl *pctrl,
					   struct device_node *np, u32 index)
{
	struct device *dev = &pdev->dev;
	struct device_node *child;
	struct ts_pinmux_function *func;
	struct ts_pctrl_group *grp;
	int ret;
	static u32 grp_index;
	u32 i = 0;

	func = &pctrl->funcs[index];

	/* Initialise function */
	func->name = np->name;
	func->ngroups = of_get_child_count(np);
	dev_dbg(dev, "func:%s,ngroups = %d\n", func->name, func->ngroups);
	dev_dbg(dev, "grp_index = %d\n", grp_index);
	if (func->ngroups <= 0)
		return 0;

	if (have_init_func && (strcmp(func->name, init_func_name) == 0)) {
		init_func_index = index;
		init_grp_index = grp_index;
		have_init_func = false;
	}

	func->groups = devm_kzalloc(dev,
					func->ngroups * sizeof(char *), GFP_KERNEL);
	if (!func->groups)
		return -ENOMEM;

	for_each_child_of_node(np, child) {
		func->groups[i] = child->name;
		dev_dbg(&pdev->dev, "group[%d] name %s\n", i, child->name);
		grp = &pctrl->groups[grp_index++];
		ret = ts_pinctrl_parse_groups(pdev, child, grp, pctrl, i++);
		if (ret) {
			dev_err(&pdev->dev, "failed to parse groups\n");
			of_node_put(child);
			return ret;
		}
	}

	return 0;
}

static void ts_pinctrl_child_count(struct ts_pinctrl *pctrl,
					struct device_node *np)
{
	struct device_node *child;

	for_each_child_of_node(np, child) {
		pctrl->nfuncs++;
		pctrl->ngroups += of_get_child_count(child);
	}
}

static int ts_pinctrl_parse_dt(struct platform_device *pdev,
				struct ts_pinctrl *pctrl)
{
	struct pinctrl_desc *ts_desc = &pctrl->desc;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *child;
	int ret;
	int i;

	ret = of_property_read_u32(np, "ts,npins", &ts_desc->npins);
	if (ret) {
		dev_err(dev, "read ts,npins error\n");
		return ret;
	}

	ts_pinctrl_child_count(pctrl, np);

	dev_dbg(dev, "ts_desc->npins:%u\n", ts_desc->npins);
	dev_dbg(dev, "nfuncs = %d\n", pctrl->nfuncs);
	dev_dbg(dev, "ngroups = %d\n", pctrl->ngroups);

	pctrl->funcs = devm_kzalloc(dev, pctrl->nfuncs *
					sizeof(struct ts_pinmux_function),
					GFP_KERNEL);
	if (!pctrl->funcs) {
		dev_err(dev, "failed to allocate memory for function list\n");
		return -ENOMEM;
	}

	pctrl->groups = devm_kzalloc(dev, pctrl->ngroups *
					 sizeof(struct ts_pctrl_group),
					 GFP_KERNEL);
	if (!pctrl->groups) {
		dev_err(dev, "failed allocate memory for pin group list\n");
		return -ENOMEM;
	}

	if (!of_property_read_string(np, "init-function", &init_func_name)) {
		have_init_func = true;
		dev_dbg(dev, "init-function = %s\n", init_func_name);
	}

	i = 0;

	for_each_child_of_node(np, child) {
		ret = ts_pinctrl_parse_functions(pdev, pctrl, child, i++);
		if (ret) {
			dev_err(&pdev->dev, "failed to parse function\n");
			of_node_put(child);
			return ret;
		}
	}

	return 0;
}

static int ts_pinctrl_register(struct platform_device *pdev,
				struct ts_pinctrl *pctrl)
{
	struct pinctrl_desc *ts_desc = &pctrl->desc;
	struct pinctrl_pin_desc *pindesc, *pdesc;
	int pin, ret;

	ret = ts_pinctrl_parse_dt(pdev, pctrl);
	if (ret)
		return ret;

	pindesc = devm_kzalloc(&pdev->dev, sizeof(*pindesc) * ts_desc->npins,
				   GFP_KERNEL);
	if (!pindesc)
		return -ENOMEM;

	pdesc = pindesc;

	for (pin = 0; pin < ts_desc->npins; pin++) {
		pdesc->number = pin;
		pdesc->name =
			devm_kasprintf(&pdev->dev, GFP_KERNEL, "PIN%d", pin);
		pdesc++;
	}

	ts_desc->pins = pindesc;
	ts_desc->name = "ts_pinctrl";
	ts_desc->owner = THIS_MODULE;
	ts_desc->pctlops = &ts_pctrl_ops;
	ts_desc->pmxops = &ts_pinmux_ops;
	ts_desc->confops = &ts_pinconf_ops;

	pctrl->pctrl = devm_pinctrl_register(&pdev->dev, ts_desc, pctrl);
	if (IS_ERR(pctrl->pctrl)) {
		dev_err(&pdev->dev, "could not register pinctrl driver\n");
		return PTR_ERR(pctrl->pctrl);
	}

	if ((init_func_index >= 0) && (init_grp_index >= 0)) {
		u32 i;
		const struct ts_pinmux_function *func = &pctrl->funcs[init_func_index];
		u32 end_grp_index = func->ngroups + init_grp_index;

		dev_dbg(&pdev->dev, "init_func_idx %d, grp_idx %d-%d\n",
			init_func_index, init_grp_index, end_grp_index);
		for (i = init_grp_index; i < end_grp_index; i++)
			ts_desc->pmxops->set_mux(pctrl->pctrl, init_func_index, i);

		init_func_index = -1;
		init_grp_index = -1;
	}

	return 0;
}

static int ts_pinctrl_probe(struct platform_device *pdev)
{
	int ret;
	struct resource *res;
	struct ts_pinctrl *pctrl;

	dev_dbg(&pdev->dev, "pinctrl_probe\n");
	pctrl = devm_kzalloc(&pdev->dev, sizeof(*pctrl), GFP_KERNEL);
	if (!pctrl)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "missing IO resource\n");
		return -ENODEV;
	}

	pctrl->reg_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(pctrl->reg_base))
		return PTR_ERR(pctrl->reg_base);

	dev_dbg(&pdev->dev, "res->start:%08x\n", (u32) res->start);

	ret = ts_pinctrl_register(pdev, pctrl);
	if (ret)
		return ret;

	platform_set_drvdata(pdev, pctrl);

	dev_info(&pdev->dev, "ts pinctrl initialized\n");

	return 0;
}

static const struct of_device_id ts_pinctrl_of_match[] = {
	{.compatible = "ts,ts-pinctrl-tx5215"},
	{}
};

static struct platform_driver ts_pinctrl_driver = {
	.driver = {
		   .name = "ts-pinctrl",
		   .of_match_table = ts_pinctrl_of_match,
		   },
	.probe = ts_pinctrl_probe,
};

static int __init ts_pinctrl_init(void)
{
	return platform_driver_register(&ts_pinctrl_driver);
}

arch_initcall(ts_pinctrl_init);
