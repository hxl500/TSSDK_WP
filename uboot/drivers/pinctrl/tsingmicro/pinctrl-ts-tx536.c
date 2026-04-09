// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 */

// #define DEBUG

#include <common.h>
#include <dm.h>
#include <log.h>
#include <dm/pinctrl.h>
#include <dm/of_access.h>
#include <dm/ofnode.h>
#include <dm/devres.h>
#include <linux/bitops.h>
#include <asm/io.h>

#define TS_PINMUX_MUX_SHIFT	0
#define TS_PINMUX_MUX_MASK	(0x3 << TS_PINMUX_MUX_SHIFT)

struct pinctrl_pin_desc {
	unsigned int number;
	const char *name;
	void *drv_data;
};

struct ts_pctrl_group {
	const char *name; /* group name */
	unsigned int *pins; /* ts pins */
	unsigned int npins;
	unsigned int mux_val; /* function */
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
	struct ts_pctrl_group *groups; /* all groups of one pinctrl */
	unsigned int ngroups;
	struct ts_pinmux_function *funcs;
	unsigned int nfuncs;
	struct pinctrl_desc desc;
	bool init_flag;
};

static const char * const ts_tx536_pins[127] = {
	"SCL",
	"SDA",
	"TX",
	"RX",
	"W1",
	"GPIO0",
	"GPIO1",
	"GPIO2",
	"GPIO3",
};

#if 0
static const char * const ts_tx536_pins_muxing[] = {
	"I2C SCL",
	"I2C SDA",
	"Uart TX",
	"Uart RX",
	"1-wire gpio",
	"gpio",
	"gpio",
	"gpio",
	"gpio",
};
#endif

static const struct pinconf_param ts_tx536_conf_params[] = {
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
static unsigned int ts_tx536_pins_param[ARRAY_SIZE(ts_tx536_pins)];
static unsigned int ts_tx536_pins_value[ARRAY_SIZE(ts_tx536_pins)];

static int ts_tx536_get_pins_count(struct udevice *dev)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);

	return pctrl->desc.npins;
}

static const char *ts_tx536_get_pin_name(struct udevice *dev, unsigned int selector)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);

	return pctrl->desc.pins[selector].name;
}

static int ts_tx536_get_pin_muxing(struct udevice *dev,
				  unsigned int selector,
				  char *buf, int size)
{
	//const struct pinconf_param *p;
	int i;
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);
	const struct ts_pctrl_group *pgrp;
	//const struct ts_pinmux_function *func = &pctrl->funcs[func_selector];
	uint32_t mux_val, reg_offset, found = 0;

	reg_offset = 4 * selector;
	mux_val = readl(pctrl->reg_base + reg_offset);
	mux_val &= TS_PINMUX_MUX_MASK;

	for (i = 0; i < pctrl->ngroups; i++) {
		pgrp = &pctrl->groups[i];

		for (int j = 0; j < pgrp->npins; j++) {
			if (pgrp->pins[j] == selector) {
				if (pgrp->mux_val == mux_val) {
					sprintf(buf, "%d %s", mux_val, pgrp->name);
					//debug("pin %d: %s mux:%d\n", pgrp->pins[j], pgrp->name, pgrp->mux_val);
					found = 1;
					break;
				}
			}
		}
	}

	if (found == 0)
		sprintf(buf, "%d ", mux_val);
	//debug("%s: %s\n", __func__, buf);

	return 0;
}

static int ts_tx536_get_groups_count(struct udevice *dev)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);

	return pctrl->ngroups;
}

static const char *ts_tx536_get_group_name(struct udevice *dev,
					  unsigned int selector)
{
	debug("%s:selector=%d\n", __func__, selector);
	debug("%s: dev= %p\n", __func__, dev);

	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);
	//debug(">>>get group[%d] %s\n", selector, pctrl->groups[selector].name);
	return pctrl->groups[selector].name;
}

static int ts_tx536_get_functions_count(struct udevice *dev)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);

	return pctrl->nfuncs;
}

static const char *ts_tx536_get_function_name(struct udevice *dev,
					     unsigned int selector)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);
	//debug(">>>get function[%d] %s\n", selector, pctrl->funcs[selector].name);
	return pctrl->funcs[selector].name;
}

static int ts_tx536_pinmux_set(struct udevice *dev, unsigned int pin_selector,
			      unsigned int func_selector)
{
	debug("ts_tx536 pinmux: pin = %d (%s), function = %d (%s)\n",
	      pin_selector, ts_tx536_get_pin_name(dev, pin_selector),
	      func_selector, ts_tx536_get_function_name(dev, func_selector));

	ts_tx536_pins_param[pin_selector] = 0;
	ts_tx536_pins_value[pin_selector] = 0;

	return 0;
}

static int ts_tx536_pinmux_group_set(struct udevice *dev,
				    unsigned int group_selector,
				    unsigned int func_selector)
{
	struct ts_pinctrl *pctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);
	const struct ts_pctrl_group *pgrp = &pctrl->groups[group_selector];
	const struct ts_pinmux_function *func = &pctrl->funcs[func_selector];
	u32 reg, i;

	debug("set_mux, function:%u, group:%u\n", func_selector,
		group_selector);
	debug("group name:%s, func name:%s", pgrp->name, func->name);
	debug("mux_val:%x\n", pgrp->mux_val);

	for (i = 0; i < pgrp->npins; i++) {
		u32 reg_offset = 4 * pgrp->pins[i];

		reg = readl(pctrl->reg_base + reg_offset);
		reg &= ~TS_PINMUX_MUX_MASK;
		reg |= pgrp->mux_val;
		writel(reg, pctrl->reg_base + reg_offset);
	}
	return 0;
}

static int ts_tx536_pinconf_set(struct udevice *dev, unsigned int pin_selector,
			       unsigned int param, unsigned int argument)
{
	debug("ts_tx536 pinconf: pin = %d (%s), param = %d, arg = %d\n",
	      pin_selector, ts_tx536_get_pin_name(dev, pin_selector),
	      param, argument);

	ts_tx536_pins_param[pin_selector] |= BIT(param);
	if (argument)
		ts_tx536_pins_value[pin_selector] |= BIT(param);
	else
		ts_tx536_pins_value[pin_selector] &= ~BIT(param);

	return 0;
}

static int ts_tx536_pinconf_group_set(struct udevice *dev,
				     unsigned int group_selector,
				     unsigned int param, unsigned int argument)
{
	debug("ts_tx536 pinconf: group = %d (%s), param = %d, arg = %d\n",
	      group_selector, ts_tx536_get_group_name(dev, group_selector),
	      param, argument);

	return 0;
}

const struct pinctrl_ops ts_tx536_pinctrl_ops = {
	.get_pins_count = ts_tx536_get_pins_count,
	.get_pin_name = ts_tx536_get_pin_name,
	.get_pin_muxing = ts_tx536_get_pin_muxing,
	.get_groups_count = ts_tx536_get_groups_count,
	.get_group_name = ts_tx536_get_group_name,
	.get_functions_count = ts_tx536_get_functions_count,
	.get_function_name = ts_tx536_get_function_name,
	.pinmux_set = ts_tx536_pinmux_set,
	.pinmux_group_set = ts_tx536_pinmux_group_set,
	.pinconf_num_params = ARRAY_SIZE(ts_tx536_conf_params),
	.pinconf_params = ts_tx536_conf_params,
	.pinconf_set = ts_tx536_pinconf_set,
	.pinconf_group_set = ts_tx536_pinconf_group_set,
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

	debug("group(%d): %s\n", index, ofnode_get_name(node));

	/* Initialise group */
	ret = ofnode_read_u32(node, "mux,val", &grp->mux_val);
	if (ret) {
		pr_err("read mux,val error\n");
		return ret;
	}
	debug("mux_val: 0x%x\n", grp->mux_val);
	ret = of_property_count_elems_of_size(node, "ts,pins", sizeof(u32));
	if (ret < 0) {
		pr_err("failed to get the count of ts,pins in %pOF node\n", node.np);
		return ret;
	}
	grp->npins = ret;
	grp->pins = devm_kzalloc(dev, grp->npins * sizeof(unsigned int *),
				 GFP_KERNEL);
	if (!grp->pins)
		return -ENOMEM;
	ret = ofnode_read_u32_array(node, "ts,pins", grp->pins, grp->npins);
	if (ret) {
		pr_err("read ts,pins error\n");
		return ret;
	}
	debug("npins: %u\n", grp->npins);

	for (int j = 0; j < grp->npins; j++)
		debug("%u ", grp->pins[j]);

	grp->name = ofnode_get_name(node);
	debug("\n");
	return 0;
}

static u32 grp_index;
static int ts_pinctrl_parse_functions(struct udevice *dev,
				       struct ts_pinctrl *pctrl,
				       ofnode node, u32 index)
{
	ofnode subnode;
	struct ts_pinmux_function *func;
	struct ts_pctrl_group *grp;
	int ret;

	func = &pctrl->funcs[index];

	/* Initialise function */
	func->name = ofnode_get_name(node);

	func->ngroups = ofnode_get_child_count(node);

	debug("func:%s, ngroups = %d\n", func->name, func->ngroups);
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
		grp = &pctrl->groups[grp_index++];
		ret = ts_pinctrl_parse_groups(dev, subnode, grp, pctrl, i++);
		if (ret) {
			pr_err("failed to parse groups\n");
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
		pr_err("read ts,npins error\n");
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
		pr_err("failed to allocate memory for function list\n");
		return -ENOMEM;
	}

	pctrl->groups = devm_kzalloc(dev, pctrl->ngroups *
				     sizeof(struct ts_pctrl_group),
				     GFP_KERNEL);

	if (!pctrl->groups) {
		pr_err("failed allocate memory for pin group list\n");
		return -ENOMEM;
	}

	i = 0;
	grp_index = 0;
	ofnode subnode;

	ofnode_for_each_subnode(subnode, dev_ofnode(dev)) {
		ret = ts_pinctrl_parse_functions(dev, pctrl, subnode, i++);
		if (ret) {
			pr_err("failed to parse function\n");
			return ret;
		}
	}
	pr_debug("%s done\n", __func__);
	return 0;
}

static int ts_pinctrl_register(struct udevice *dev,
				struct ts_pinctrl *pctrl)
{
	int ret = ts_pinctrl_parse_dt(dev, pctrl);

	if (ret)
		return ret;

	struct pinctrl_desc *ts_desc = &pctrl->desc;
	struct pinctrl_pin_desc *pindesc, *pdesc;
	char *pin_names;
	int pin;

	pindesc = devm_kzalloc(dev, sizeof(*pindesc) * ts_desc->npins,
				   GFP_KERNEL);
	if (!pindesc)
		return -ENOMEM;
	pin_names = devm_kzalloc(dev, PINNAME_SIZE * ts_desc->npins,
				   GFP_KERNEL);
	if (!pindesc)
		return -ENOMEM;
	pdesc = pindesc;

	for (pin = 0; pin < ts_desc->npins; pin++) {
		pdesc->number = pin;
		sprintf(pin_names, "PIN%d", pin);
		pdesc->name = pin_names;
		pdesc++;
		pin_names += PINNAME_SIZE;
	}

	ts_desc->pins = pindesc;
	ts_desc->name = "ts_pinctrl";
	ts_desc->owner = THIS_MODULE;

#if 0
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

//bool ts_pinctrl_init_flag = false;
static int ts_pinctrl_probe(struct udevice *dev)
{
	int ret;
	struct ts_pinctrl *pctrl;
	fdt_addr_t addr;

	if (dev->driver_data) {
		struct ts_pinctrl *pinctrl = (struct ts_pinctrl *)dev_get_driver_data(dev);

		if (pinctrl->init_flag)
			return 0;
	}

	debug("pinctrl_probe\n");
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
	pctrl->init_flag = true;

	return 0;
}

static const struct udevice_id ts_tx536_pinctrl_match[] = {
	{ .compatible = "ts,tx536-pinctrl" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(ts_tx536_pinctrl) = {
	.name = "ts_tx536_pinctrl",
	.id = UCLASS_PINCTRL,
	.of_match = ts_tx536_pinctrl_match,
	.ops = &ts_tx536_pinctrl_ops,
	.probe = ts_pinctrl_probe,
};
