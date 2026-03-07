// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */

/* #define DEBUG */

#include <common.h>
#include <dm.h>
#include <log.h>
#include <dm/pinctrl.h>
#include <dm/of_access.h>
#include <dm/ofnode.h>
#include <dm/devres.h>
#include <linux/bitops.h>
#include <asm/io.h>
#include <dt-bindings/pinctrl/pinctrl-ts-tx5112.h>

#define SCD_GPIO_PCTL_REG			0x52c		/*gpio port control*/

#define PAD_GROUP_A		0x0
#define PAD_GROUP_B		0x10000
#define PAD_GROUP_C		0x20000
#define PAD_GROUP_D		0x30000
#define PAD_GROUP_E		0x40000
#define PAD_GROUP_F		0x50000
/* tx5112 does not has following pin groups */
// #define PAD_GROUP_G		0x60000
// #define PAD_GROUP_J		0x80000
// #define PAD_GROUP_K		0x90000

static int ts_tx5112_gpio_func_sel(struct udevice *dev, unsigned pin, 
	unsigned func_selector);

struct pinctrl_pin_desc {
	unsigned number;
	const char *name;
	void *drv_data;
};

struct ts_pctrl_group {
	const char *name;
	unsigned int *pins;
	unsigned int npins;
	unsigned int mux_val;
};

struct ts_pinmux_function {
	const char *name;
	const char **groups;
	unsigned int ngroups;
};

struct pinctrl_desc {
	const char *name;
	const struct pinctrl_pin_desc *pins;
	unsigned int npins;
	const struct pinctrl_ops *pctlops;
	const struct pinmux_ops *pmxops;
	const struct pinconf_ops *confops;
	struct module *owner;
#ifdef CONFIG_GENERIC_PINCONF
	unsigned int num_custom_params;
	const struct pinconf_generic_params *custom_params;
	const struct pin_config_item *custom_conf_items;
#endif
};

struct ts_pinctrl {
	void __iomem *reg_base;
	struct ts_pctrl_group *groups;
	unsigned int ngroups;
	struct ts_pinmux_function *funcs;
	unsigned int nfuncs;
	struct pinctrl_desc desc;
};

static const char * const ts_tx5112_pins[PINMAX] = {

};

static const char * const ts_tx5112_pins_muxing[PINMAX] = {

};

static const struct pinconf_param ts_tx5112_conf_params[] = {
	{ "bias-disable", PIN_CONFIG_BIAS_DISABLE, 0 },
	{ "bias-high-impedance", PIN_CONFIG_BIAS_HIGH_IMPEDANCE, 0 },
	{ "bias-bus-hold", PIN_CONFIG_BIAS_BUS_HOLD, 0 },
	{ "bias-pull-up", PIN_CONFIG_BIAS_PULL_UP, 1 },
	{ "bias-pull-down", PIN_CONFIG_BIAS_PULL_DOWN, 1 },
	{ "bias-pull-pin-default", PIN_CONFIG_BIAS_PULL_PIN_DEFAULT, 1 },
	{ "drive-open-drain", PIN_CONFIG_DRIVE_OPEN_DRAIN, 0 },
	{ "drive-open-source", PIN_CONFIG_DRIVE_OPEN_SOURCE, 0 },
	{ "drive-strength", PIN_CONFIG_DRIVE_STRENGTH, 0 },
	{ "input-enable", PIN_CONFIG_INPUT_ENABLE, 1 },
	{ "input-disable", PIN_CONFIG_INPUT_ENABLE, 0 },
};

/* bitfield used to save param and value of each pin/selector */
static unsigned int ts_tx5112_pins_param[ARRAY_SIZE(ts_tx5112_pins)];
static unsigned int ts_tx5112_pins_value[ARRAY_SIZE(ts_tx5112_pins)];

static int ts_tx5112_get_pins_count(struct udevice *dev)
{
	return ARRAY_SIZE(ts_tx5112_pins);
}

static const char *ts_tx5112_get_pin_name(struct udevice *dev, unsigned selector)
{
	return ts_tx5112_pins[selector];
}

static int ts_tx5112_get_pin_muxing(struct udevice *dev,
				  unsigned int selector,
				  char *buf, int size)
{
	const struct pinconf_param *p;
	int i;

	snprintf(buf, size, "%s", ts_tx5112_pins_muxing[selector]);

	if (ts_tx5112_pins_param[selector]) {
		for (i = 0, p = ts_tx5112_conf_params;
		     i < ARRAY_SIZE(ts_tx5112_conf_params);
		     i++, p++) {
			if ((ts_tx5112_pins_param[selector] & BIT(p->param)) &&
			    (!!(ts_tx5112_pins_value[selector] & BIT(p->param)) ==
			     p->default_value)) {
				strncat(buf, " ", size);
				strncat(buf, p->property, size);
			}
		}
	}
	strncat(buf, ".", size);

	return 0;
}

static int ts_tx5112_get_groups_count(struct udevice *dev)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);
	return pctrl->ngroups;
}

static const char *ts_tx5112_get_group_name(struct udevice *dev,
					  unsigned selector)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);
	debug(">>>get group[%d] %s\n", selector, pctrl->groups[selector].name);
	return pctrl->groups[selector].name;
}

static int ts_tx5112_get_functions_count(struct udevice *dev)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);
	return pctrl->nfuncs;
}

static const char *ts_tx5112_get_function_name(struct udevice *dev,
					     unsigned selector)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);
	//debug(">>>get function[%d] %s\n", selector, pctrl->funcs[selector].name);
	return pctrl->funcs[selector].name;
}

static int ts_tx5112_pinmux_set(struct udevice *dev, unsigned pin_selector,
			      unsigned func_selector)
{
	debug("ts_tx5112 pinmux: pin = %d (%s), function = %d (%s)\n",
	      pin_selector, ts_tx5112_get_pin_name(dev, pin_selector),
	      func_selector, ts_tx5112_get_function_name(dev, func_selector));

	ts_tx5112_pins_param[pin_selector] = 0;
	ts_tx5112_pins_value[pin_selector] = 0;

    ts_tx5112_gpio_func_sel(dev, pin_selector, func_selector);
	return 0;
}

static int ts_tx5112_gpio_func_sel(struct udevice *dev, unsigned pin, 
	unsigned func_selector)
{
	u32 pad_offset;
	u32 reg_offset;
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
	case PD0 ... PD4:
		pad_offset = PAD_GROUP_D;
		reg_offset = pin - PD0;
		break;
	case PE0 ... PE5:
		pad_offset = PAD_GROUP_E;
		reg_offset = pin - PE0;
		break;
	case PF0 ... PF6:
		pad_offset = PAD_GROUP_F;
		reg_offset = pin - PF0;
		break;
	default:
		printf("err: %s pin %d, func %d failed\n", __func__, pin, func_selector);
		return -1;
	}

	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);
	void __iomem *addr = pctrl->reg_base + pad_offset + SCD_GPIO_PCTL_REG;
	u32 val;
	val = readl(addr);
	reg_offset *= 4;
	val &= ~(0xf << reg_offset);
	val |= (func_selector << reg_offset);
	writel(val, addr);
	//debug("pin%d, wr 0x%p, val 0x%x\n", pin, addr, val);
    return 0;
}

static int ts_tx5112_pinmux_group_set(struct udevice *dev,
				    unsigned group_selector,
				    unsigned func_selector)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);
	const struct ts_pctrl_group *pgrp = &pctrl->groups[group_selector];
	const struct ts_pinmux_function *func = &pctrl->funcs[func_selector];

	debug("set_mux, function:%u, group:%u\n", func_selector,
		group_selector);
	debug("group name:%s, func name:%s ", pgrp->name, func->name);
	debug("mux_val:%x\n", pgrp->mux_val);

	for (u32 i = 0; i < pgrp->npins; i++) {
		ts_tx5112_gpio_func_sel(dev, pgrp->pins[i], pgrp->mux_val);
	}
	return 0;
}

static int ts_tx5112_pinconf_set(struct udevice *dev, unsigned pin_selector,
			       unsigned param, unsigned argument)
{
	debug("ts_tx5112 pinconf: pin = %d (%s), param = %d, arg = %d\n",
	      pin_selector, ts_tx5112_get_pin_name(dev, pin_selector),
	      param, argument);

	ts_tx5112_pins_param[pin_selector] |= BIT(param);
	if (argument)
		ts_tx5112_pins_value[pin_selector] |= BIT(param);
	else
		ts_tx5112_pins_value[pin_selector] &= ~BIT(param);

	return 0;
}

static int ts_tx5112_pinconf_group_set(struct udevice *dev,
				     unsigned group_selector,
				     unsigned param, unsigned argument)
{
	debug("ts_tx5112 pinconf: group = %d (%s), param = %d, arg = %d\n",
	      group_selector, ts_tx5112_get_group_name(dev, group_selector),
	      param, argument);

	return 0;
}

const struct pinctrl_ops ts_tx5112_pinctrl_ops = {
	.get_pins_count = ts_tx5112_get_pins_count,
	.get_pin_name = ts_tx5112_get_pin_name,
	.get_pin_muxing = ts_tx5112_get_pin_muxing,
	.get_groups_count = ts_tx5112_get_groups_count,
	.get_group_name = ts_tx5112_get_group_name,
	.get_functions_count = ts_tx5112_get_functions_count,
	.get_function_name = ts_tx5112_get_function_name,
	.pinmux_set = ts_tx5112_pinmux_set,
	.pinmux_group_set = ts_tx5112_pinmux_group_set,
	.pinconf_num_params = ARRAY_SIZE(ts_tx5112_conf_params),
	.pinconf_params = ts_tx5112_conf_params,
	.pinconf_set = ts_tx5112_pinconf_set,
	.pinconf_group_set = ts_tx5112_pinconf_group_set,
	.set_state = pinctrl_generic_set_state,
};

static int of_property_count_elems_of_size(ofnode node,
				const char *propname, int elem_size)
{
	int len = 0;
	const void *tmp = ofnode_get_property(node, propname, &len);
	//debug("len %d\n", len);

	if (len % elem_size != 0) {
		pr_err("size of %s in node OF%d is not a multiple of %d\n",
		       propname, len, elem_size);
		return -EINVAL;
	}

	(void)tmp;
	return len / elem_size;
}

static int ts_pinctrl_parse_groups(struct udevice *dev,
				    ofnode node,
				    struct ts_pctrl_group *grp,
				    struct ts_pinctrl *pctrl, u32 index)
{
	int ret;

	/* Initialise group */
	ret = ofnode_read_u32(node, "mux,val", &grp->mux_val);
	if (ret) {
		debug("read mux,val error\n");
		return ret;
	}
	debug("mux_val: 0x%x\n", grp->mux_val);
	ret = of_property_count_elems_of_size(node, "ts,pins", sizeof(u32));
	if (ret < 0) {
		debug("failed to get the count of ts,pins in %pOF node\n", node.np);
		return ret;
	}
	grp->npins = ret;
	grp->pins = devm_kzalloc(dev, grp->npins * sizeof(unsigned int),
				 GFP_KERNEL);
	if (!grp->pins)
		return -ENOMEM;
	ret = ofnode_read_u32_array(node, "ts,pins", grp->pins, grp->npins);
	if (ret) {
		debug("read ts,pins error\n");
		return ret;
	}
	debug("npins: %u\n", grp->npins);

	for (int j = 0; j < grp->npins; j++) {
		debug("%u ", grp->pins[j]);
	}
	grp->name = ofnode_get_name(node);
	debug("\n");
	return 0;
}

static int ts_pinctrl_parse_functions(struct udevice *dev,
				       struct ts_pinctrl *pctrl,
				       ofnode node, u32 index)
{
	ofnode subnode;
	struct ts_pinmux_function *func;
	struct ts_pctrl_group *grp;
	int ret;
	static u32 grp_index = 0;

	func = &pctrl->funcs[index];

	/* Initialise function */
	func->name = ofnode_get_name(node);
	func->ngroups = ofnode_get_child_count(node);
	debug("\nfunc:%s, ngroups = %d\n", func->name, func->ngroups);
	debug("grp_index = %d\n", grp_index);
	if (func->ngroups <= 0)
		return 0;

	func->groups = devm_kzalloc(dev,
				    func->ngroups * sizeof(char *), GFP_KERNEL);
	if (!func->groups)
		return -ENOMEM;

	u32 i = 0;
	ofnode_for_each_subnode(subnode, node) {
		func->groups[i] = ofnode_get_name(subnode);
		debug("group[%d] name %s\n", i, func->groups[i]);
		grp = &pctrl->groups[grp_index++];
		ret = ts_pinctrl_parse_groups(dev, subnode, grp, pctrl, i++);
		if (ret) {
			debug("failed to parse groups\n");
			return ret;
		}
	}

	return 0;
}

static void ts_pinctrl_child_count(struct udevice *dev,
				struct ts_pinctrl *pctrl)
{
	ofnode subnode;
	ofnode_for_each_subnode(subnode, dev_ofnode(dev)) {
		pctrl->nfuncs++;
		pctrl->ngroups += ofnode_get_child_count(subnode);
	}
}

static int ts_pinctrl_parse_dt(struct udevice *dev,
				struct ts_pinctrl *pctrl)
{
	struct pinctrl_desc *ts_desc = &pctrl->desc;
	int ret;
	int i;

	ret = dev_read_u32(dev, "ts,npins", &ts_desc->npins);
	if (ret) {
		debug("read ts,npins error\n");
		return ret;
	}

	ts_pinctrl_child_count(dev, pctrl);

	debug("npins  : %u\n", ts_desc->npins);
	debug("nfuncs : %d\n", pctrl->nfuncs);
	debug("ngroups: %d\n", pctrl->ngroups);

	pctrl->funcs = devm_kzalloc(dev, pctrl->nfuncs *
				    sizeof(struct ts_pinmux_function),
				    GFP_KERNEL);
	if (!pctrl->funcs) {
		debug("failed to allocate memory for function list\n");
		return -ENOMEM;
	}

	pctrl->groups = devm_kzalloc(dev, pctrl->ngroups *
				     sizeof(struct ts_pctrl_group),
				     GFP_KERNEL);
	if (!pctrl->groups) {
		debug("failed allocate memory for pin group list\n");
		return -ENOMEM;
	}

	i = 0;
	ofnode subnode;
	ofnode_for_each_subnode(subnode, dev_ofnode(dev)) {
		ret = ts_pinctrl_parse_functions(dev, pctrl, subnode, i++);
		if (ret) {
			debug("failed to parse function\n");
			return ret;
		}
	}
	printf("ts_pinctrl_parse_dt done\n");
	return 0;
}

static int ts_pinctrl_register(struct udevice *dev,
				struct ts_pinctrl *pctrl)
{

	int ret = ts_pinctrl_parse_dt(dev, pctrl);
	if (ret)
		return ret;

#if 0
	struct pinctrl_desc *ts_desc = &pctrl->desc;
	struct pinctrl_pin_desc *pindesc, *pdesc;
	int pin, ret;

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
#endif
	return 0;
}

bool ts_pinctrl_init_flag = false;
static int ts_pinctrl_probe(struct udevice *dev)
{
	int ret;
	struct ts_pinctrl *pctrl;
	fdt_addr_t addr;

	if (ts_pinctrl_init_flag) {
		return 0;
	}
	
	printf("pinctrl_probe\n");
	pctrl = devm_kzalloc(dev, sizeof(*pctrl), GFP_KERNEL);
	if (!pctrl)
		return -ENOMEM;

	addr = dev_read_addr(dev);
	if (addr == FDT_ADDR_T_NONE) {
		printf("%s get addr0 failed", __func__);
		return -EINVAL;
	}
	pctrl->reg_base = (void *)addr;
	if (IS_ERR(pctrl->reg_base))
		return PTR_ERR(pctrl->reg_base);

	debug("reg_base:%08x\n", (u32)addr);

	ret = ts_pinctrl_register(dev, pctrl);
	if (ret)
		return ret;

	dev->driver_data = (ulong)pctrl;

	dev_info(dev, "ts pinctrl initialized\n");
	ts_pinctrl_init_flag = true;

	return 0;
}

static const struct udevice_id ts_tx5112_pinctrl_match[] = {
	{ .compatible = "ts_tx5112,pinctrl" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(ts_tx5112_pinctrl) = {
	.name = "ts_tx5112_pinctrl",
	.id = UCLASS_PINCTRL,
	.of_match = ts_tx5112_pinctrl_match,
	.ops = &ts_tx5112_pinctrl_ops,
	.probe = ts_pinctrl_probe,
};
