// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023-2023 Tsingmicro, Inc.
 *
 * gpio driver for ts aon gpio
 */
// #define DEBUG

#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/gpio.h>
#include <linux/gpio/driver.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/spinlock.h>

struct aon_gpio_reg_cfg {
	uint32_t ctl_reg;
	uint32_t int_mode_reg;
	uint8_t data_in;
	uint8_t out_en;
	uint8_t data_out;
	uint8_t int_en;
	uint8_t int_pol;
	uint8_t int_typ;
	uint8_t irq_shift;
	uint8_t pull_up_shift;

};

struct aon_gpios_cfg {
	const struct aon_gpio_reg_cfg *reg_cfg;
	unsigned int num_reg_cfg;
	unsigned int smu_irq_raw_status_reg;
	unsigned int smu_irq_mask_reg;
	unsigned int smu_irq_status_reg;
	unsigned int smu_pad_ctrl_reg;
	int (*init)(struct regmap *regmap);
};

struct ts_aon_gpio {
	struct aon_gpios_cfg *cfg;
	struct regmap *regmap;
	struct gpio_chip gc;
	struct device *dev;
	struct device_node *smu_of_node;
	struct irq_domain *domain;
	unsigned int irq;

};

#define REG_CFG(_ctl_reg, _int_reg, _di, _oe, _do,\
					_int_en, _int_pol, _int_typ, _irq_shift, _pull_up_shift) \
	{ \
		.ctl_reg = (_ctl_reg),\
		.int_mode_reg = (_int_reg),\
		.data_in = (_di),\
		.out_en = (_oe),\
		.data_out = (_do),\
		.int_en = (_int_en),\
		.int_pol = (_int_pol),\
		.int_typ = (_int_typ),\
		.irq_shift = (_irq_shift),\
		.pull_up_shift = (_pull_up_shift)\
	}

static const struct aon_gpio_reg_cfg tx536_aon_gpio_reg_cfg[] = {
	REG_CFG(0xe8, 0xe4, 2, 1, 0, 2, 1, 0, 6, 0),
	REG_CFG(0xe8, 0xe4, 6, 5, 4, 6, 5, 4, 7, 1),
};

static const struct aon_gpios_cfg tx536_aon_gpios_cfg = {
	.reg_cfg = tx536_aon_gpio_reg_cfg,
	.num_reg_cfg = ARRAY_SIZE(tx536_aon_gpio_reg_cfg),
	.smu_irq_raw_status_reg = 0x08,
	.smu_irq_mask_reg = 0x18,
	.smu_irq_status_reg = 0x28,
	.smu_pad_ctrl_reg = 0x130,

};

static int tx5112_init(struct regmap *regmap)
{
	/* cfg iomux to gpio */
	return regmap_write_bits(regmap, 0x60, BIT(3)|BIT(11), BIT(3)|BIT(11));
}

static const struct aon_gpio_reg_cfg tx5112_aon_gpio_reg_cfg[] = {
	REG_CFG(0x60, 0x50, 2, 1, 0, 2, 1, 0, 4, 0),
	REG_CFG(0x60, 0x50, 10, 9, 8, 10, 9, 8, 5, 1),
};

static const struct aon_gpios_cfg tx5112_aon_gpios_cfg = {
	.reg_cfg = tx5112_aon_gpio_reg_cfg,
	.num_reg_cfg = ARRAY_SIZE(tx5112_aon_gpio_reg_cfg),
	.smu_irq_raw_status_reg = 0x00,
	.smu_irq_mask_reg = 0x04,
	.smu_irq_status_reg = 0x08,
	.init = tx5112_init,
};

static int ts_aon_gpio_direction_input(struct gpio_chip *chip,
				unsigned int offset)
{
	struct ts_aon_gpio *ts_aon_gpio = gpiochip_get_data(chip);
	const struct aon_gpio_reg_cfg *cfg = &ts_aon_gpio->cfg->reg_cfg[offset];

	if (offset >= chip->ngpio)
		return -EINVAL;

	return regmap_write_bits(ts_aon_gpio->regmap, cfg->ctl_reg,
				BIT(cfg->out_en), 0);
}

static int ts_aon_gpio_direction_output(struct gpio_chip *chip,
				unsigned int offset, int value)
{
	struct ts_aon_gpio *ts_aon_gpio = gpiochip_get_data(chip);
	const struct aon_gpio_reg_cfg *cfg = &ts_aon_gpio->cfg->reg_cfg[offset];

	if (offset >= chip->ngpio)
		return -EINVAL;

	dev_dbg(ts_aon_gpio->dev, "offset:%d, value:%d\n", offset, value);
	value = value ? BIT(cfg->data_out) : 0;

	return regmap_write_bits(ts_aon_gpio->regmap, cfg->ctl_reg,
			BIT(cfg->out_en)|BIT(cfg->data_out), BIT(cfg->out_en)|value);
}

static int ts_aon_gpio_get_direction(struct gpio_chip *chip,
				unsigned int offset)
{
	struct ts_aon_gpio *ts_aon_gpio = gpiochip_get_data(chip);
	const struct aon_gpio_reg_cfg *cfg = &ts_aon_gpio->cfg->reg_cfg[offset];
	int ret;
	unsigned int val;

	if (offset >= chip->ngpio)
		return -EINVAL;

	ret = regmap_read(ts_aon_gpio->regmap, cfg->ctl_reg, &val);

	if (ret < 0) {
		dev_err(ts_aon_gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
		return ret;
	}

	dev_dbg(ts_aon_gpio->dev, "v:%x, out_en:%d\n", val, cfg->out_en);

	if (val & BIT(cfg->out_en))
		return 0;

	return 1;
}

static int ts_aon_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	struct ts_aon_gpio *ts_aon_gpio = gpiochip_get_data(chip);
	const struct aon_gpio_reg_cfg *cfg = &ts_aon_gpio->cfg->reg_cfg[offset];
	int ret;
	unsigned int val;

	if (offset >= chip->ngpio)
		return -EINVAL;

	ret = regmap_read(ts_aon_gpio->regmap, cfg->ctl_reg, &val);
	if (ret < 0) {
		dev_err(ts_aon_gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
		return ret;
	}

	return !!(val & BIT(cfg->data_in));
}

static void ts_aon_gpio_set(struct gpio_chip *chip, unsigned int offset,
				int value)
{
	struct ts_aon_gpio *ts_aon_gpio = gpiochip_get_data(chip);
	const struct aon_gpio_reg_cfg *cfg = &ts_aon_gpio->cfg->reg_cfg[offset];
	unsigned int data;
	int ret;

	if (offset >= chip->ngpio)
		return;

	if (value)
		data = BIT(cfg->data_out);
	else
		data = ~BIT(cfg->data_out);

	dev_dbg(ts_aon_gpio->dev, "v:%d, d:%d\n", value, data);

	ret = regmap_write_bits(ts_aon_gpio->regmap, cfg->ctl_reg,
				BIT(cfg->data_out), data);
	if (ret < 0)
		dev_err(ts_aon_gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
}

static int ts_aon_gpio_set_config(struct gpio_chip *gc, unsigned int offset,
				 unsigned long config)
{
	struct ts_aon_gpio *ts_aon_gpio = gpiochip_get_data(gc);
	const struct aon_gpio_reg_cfg *cfg = &ts_aon_gpio->cfg->reg_cfg[offset];
	unsigned int data;
	int ret;
	enum pin_config_param pin_conf = pinconf_to_config_param(config);

	dev_dbg(ts_aon_gpio->dev, "offset:%d, config:%ld\n", offset, config);

	if (offset >= gc->ngpio)
		return -EINVAL;

	if (pin_conf == PIN_CONFIG_BIAS_DISABLE)
		data = BIT(cfg->pull_up_shift);
	else if (pin_conf == PIN_CONFIG_BIAS_PULL_UP)
		data = ~BIT(cfg->pull_up_shift);
	else
		return -ENOTSUPP;

	if (ts_aon_gpio->cfg->smu_pad_ctrl_reg == 0)
		return -ENOTSUPP;

	dev_dbg(ts_aon_gpio->dev, "pin_conf:%d, d:%d\n", pin_conf, data);

	ret = regmap_write_bits(ts_aon_gpio->regmap, ts_aon_gpio->cfg->smu_pad_ctrl_reg,
				BIT(cfg->pull_up_shift), data);
	if (ret < 0) {
		dev_err(ts_aon_gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
		return ret;
	}
	return 0;
}

static void ts_aon_gpio_irq_mask(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct ts_aon_gpio *gpio = igc->private;
	const struct aon_gpio_reg_cfg *cfg = &gpio->cfg->reg_cfg[d->hwirq];
	// struct gpio_chip *gc = &gpio->gc;
	// unsigned long flags;
	int ret;

	dev_dbg(gpio->dev, "%s hwirq: %ld\n", __func__, d->hwirq);
	// spin_lock_irqsave(&gc->bgpio_lock, flags);
	ret = regmap_write_bits(gpio->regmap, gpio->cfg->smu_irq_mask_reg,
				BIT(cfg->irq_shift), BIT(d->hwirq));
	if (ret < 0)
		dev_err(gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
	// spin_unlock_irqrestore(&gc->bgpio_lock, flags);
}

static void ts_aon_gpio_irq_unmask(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct ts_aon_gpio *gpio = igc->private;
	const struct aon_gpio_reg_cfg *cfg = &gpio->cfg->reg_cfg[d->hwirq];
	// struct gpio_chip *gc = &gpio->gc;
	// unsigned long flags;
	int ret;

	dev_dbg(gpio->dev, "%s hwirq: %ld\n", __func__, d->hwirq);
	// spin_lock_irqsave(&gc->bgpio_lock, flags);
	ret = regmap_write_bits(gpio->regmap, gpio->cfg->smu_irq_mask_reg,
				BIT(cfg->irq_shift), 0);
	if (ret < 0)
		dev_err(gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
	// spin_unlock_irqrestore(&gc->bgpio_lock, flags);
}

static void ts_aon_gpio_irq_enable(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct ts_aon_gpio *gpio = igc->private;
	struct gpio_chip *gc = &gpio->gc;
	unsigned long flags;
	int ret;
	const struct aon_gpio_reg_cfg *cfg = &gpio->cfg->reg_cfg[d->hwirq];

	dev_dbg(gpio->dev, "%s hwirq: %ld\n", __func__, d->hwirq);
	spin_lock_irqsave(&gc->bgpio_lock, flags);
	ret = regmap_write_bits(gpio->regmap, cfg->int_mode_reg,
				BIT(cfg->int_en), BIT(cfg->int_en));
	if (ret < 0)
		dev_err(gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
	ts_aon_gpio_irq_unmask(d);
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);
}

static void ts_aon_gpio_irq_disable(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct ts_aon_gpio *gpio = igc->private;
	struct gpio_chip *gc = &gpio->gc;
	unsigned long flags;
	int ret;
	const struct aon_gpio_reg_cfg *cfg = &gpio->cfg->reg_cfg[d->hwirq];

	dev_dbg(gpio->dev, "%s hwirq: %ld\n", __func__, d->hwirq);
	spin_lock_irqsave(&gc->bgpio_lock, flags);
	ret = regmap_write_bits(gpio->regmap, cfg->int_mode_reg,
				BIT(cfg->int_en), 0);
	if (ret < 0)
		dev_err(gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
	ts_aon_gpio_irq_mask(d);
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);
}

static void ts_aon_gpio_irq_ack(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct ts_aon_gpio *gpio = igc->private;
	// struct gpio_chip *gc = &gpio->gc;
	// unsigned long flags;
	int ret;
	const struct aon_gpio_reg_cfg *cfg = &gpio->cfg->reg_cfg[d->hwirq];

	irq_gc_lock(igc);
	dev_dbg(gpio->dev, "%s hwirq: %ld\n", __func__, d->hwirq);
	dev_dbg(gpio->dev, "cfg->irq_shift: %d\n", cfg->irq_shift);

	ret = regmap_write_bits(gpio->regmap, gpio->cfg->smu_irq_raw_status_reg,
				BIT(cfg->irq_shift), BIT(cfg->irq_shift));
	if (ret < 0)
		dev_err(gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
	irq_gc_unlock(igc);
}

static int ts_aon_gpio_irq_reqres(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct ts_aon_gpio *gpio = igc->private;
	struct gpio_chip *gc = &gpio->gc;

	if (gpiochip_lock_as_irq(gc, irqd_to_hwirq(d))) {
		dev_err(gpio->dev, "unable to lock HW IRQ %lu for IRQ\n",
			irqd_to_hwirq(d));
		return -EINVAL;
	}
	return 0;
}

static void ts_aon_gpio_irq_relres(struct irq_data *d)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct ts_aon_gpio *gpio = igc->private;
	struct gpio_chip *gc = &gpio->gc;

	gpiochip_unlock_as_irq(gc, irqd_to_hwirq(d));
}

static void ts_aon_gpio_toggle_trigger(struct ts_aon_gpio *gpio, unsigned int offs)
{
	struct gpio_chip *gc = &gpio->gc;
	const struct aon_gpio_reg_cfg *cfg = &gpio->cfg->reg_cfg[offs];
	u32 v;
	int ret;

	if (gpio_get_value(gc->base + offs))
		v &= ~BIT(cfg->int_pol);
	else
		v |= BIT(cfg->int_pol);

	ret = regmap_write_bits(gpio->regmap, cfg->int_mode_reg,
				BIT(cfg->int_pol), v);
	if (ret < 0)
		dev_err(gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
}

static int ts_aon_gpio_irq_set_type(struct irq_data *d, u32 type)
{
	struct irq_chip_generic *igc = irq_data_get_irq_chip_data(d);
	struct ts_aon_gpio *gpio = igc->private;
	struct gpio_chip *gc = &gpio->gc;
	int bit = d->hwirq;
	unsigned long level, polarity, flags;
	const struct aon_gpio_reg_cfg *cfg = &gpio->cfg->reg_cfg[d->hwirq];
	int ret;

	if (type & ~(IRQ_TYPE_EDGE_RISING | IRQ_TYPE_EDGE_FALLING |
		     IRQ_TYPE_LEVEL_HIGH | IRQ_TYPE_LEVEL_LOW))
		return -EINVAL;

	spin_lock_irqsave(&gc->bgpio_lock, flags);

	switch (type) {
	case IRQ_TYPE_EDGE_BOTH:
		level = BIT(cfg->int_typ);
		ts_aon_gpio_toggle_trigger(gpio, bit);
		break;
	case IRQ_TYPE_EDGE_RISING:
		level = BIT(cfg->int_typ);
		polarity = BIT(cfg->int_pol);
		break;
	case IRQ_TYPE_EDGE_FALLING:
		level = BIT(cfg->int_typ);
		polarity = ~BIT(cfg->int_pol);
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		level = ~BIT(cfg->int_typ);
		polarity = BIT(cfg->int_pol);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		level = ~BIT(cfg->int_typ);
		polarity = ~BIT(cfg->int_pol);
		break;
	}

	irq_setup_alt_chip(d, type);

	ret = regmap_write_bits(gpio->regmap, cfg->int_mode_reg,
				BIT(cfg->int_typ), level);
	if (ret < 0)
		dev_err(gpio->dev, "%s regmap_read error: %d\n", __func__, ret);

	if (type != IRQ_TYPE_EDGE_BOTH) {
		ret = regmap_write_bits(gpio->regmap, cfg->int_mode_reg,
					BIT(cfg->int_pol), polarity);
		if (ret < 0)
			dev_err(gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
	}
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);

	return 0;
}

static int ts_aon_gpio_to_irq(struct gpio_chip *gc, unsigned int offset)
{
	struct ts_aon_gpio *gpio = gpiochip_get_data(gc);

	return irq_find_mapping(gpio->domain, offset);
}

static irqreturn_t ts_aon_gpio_irq_handler(int irq, void *dev_id)
{
	struct ts_aon_gpio *gpio = dev_id;
	unsigned int val;
	int ret, i;
	irqreturn_t worked = IRQ_NONE;

	dev_dbg(gpio->dev, "%s\n", __func__);
	ret = regmap_read(gpio->regmap, gpio->cfg->smu_irq_status_reg, &val);
	if (ret < 0)
		dev_err(gpio->dev, "%s regmap_read error: %d\n", __func__, ret);
	dev_dbg(gpio->dev, "%s reg:%x, irq_status: 0x%x\n", __func__,
				gpio->cfg->smu_irq_status_reg, val);
	for (i = 0; i < gpio->cfg->num_reg_cfg; i++) {
		const struct aon_gpio_reg_cfg *cfg = &gpio->cfg->reg_cfg[i];
		unsigned int irq_mask = BIT(cfg->irq_shift);

		if (val & irq_mask) {
			int hwirq = i;
			int gpio_irq = irq_find_mapping(gpio->domain, hwirq);

			dev_dbg(gpio->dev, "hwirq: %d, gpio_irq: %d\n", hwirq, gpio_irq);
			generic_handle_irq(gpio_irq);
			dev_dbg(gpio->dev, "after generic_handle_irq\n");
			worked = IRQ_HANDLED;
		}
	}

	return worked;
}
static void ts_aon_gpio_configure_irqs(struct ts_aon_gpio *gpio)
{
	struct gpio_chip *gc = &gpio->gc;
	// struct fwnode_handle  *fwnode = &gpio->smu_of_node->fwnode;
	struct fwnode_handle  *fwnode = &gpio->dev->of_node->fwnode;
	struct irq_chip_generic	*irq_gc = NULL;
	unsigned int hwirq, ngpio = gc->ngpio;
	struct irq_chip_type *ct;
	int err, i;

	// gpio->irq = irq_of_parse_and_map(gpio->smu_of_node, 0);
	gpio->irq = irq_of_parse_and_map(gpio->dev->of_node, 0);
	if (!gpio->irq) {
		dev_warn(gpio->dev, "smu no irq\n");
		return;
	}

	gpio->domain = irq_domain_create_linear(fwnode, ngpio,
						 &irq_generic_chip_ops, gpio);
	if (!gpio->domain) {
		dev_info(gpio->dev, "irq_domain_create_linear failed\n");
		return;
	}

	err = irq_alloc_domain_generic_chips(gpio->domain, ngpio, 2,
					     "ts-aon-gpio", handle_level_irq,
					     IRQ_NOAUTOEN|IRQ_NOREQUEST, 0,
					     IRQ_GC_INIT_NESTED_LOCK);
	if (err) {
		dev_info(gpio->dev, "irq_alloc_domain_generic_chips failed\n");
		irq_domain_remove(gpio->domain);
		gpio->domain = NULL;
		return;
	}

	irq_gc = irq_get_domain_generic_chip(gpio->domain, 0);
	if (!irq_gc) {
		dev_info(gpio->dev, "irq_get_domain_generic_chip failed\n");
		irq_domain_remove(gpio->domain);
		gpio->domain = NULL;
		return;
	}

	// irq_gc->reg_base = gpio->regs;
	irq_gc->private = gpio;

	for (i = 0; i < 2; i++) {
		ct = &irq_gc->chip_types[i];
		ct->chip.irq_ack = ts_aon_gpio_irq_ack;
		ct->chip.irq_mask = ts_aon_gpio_irq_mask;
		ct->chip.irq_unmask = ts_aon_gpio_irq_unmask;
		ct->chip.irq_set_type = ts_aon_gpio_irq_set_type;
		ct->chip.irq_enable = ts_aon_gpio_irq_enable;
		ct->chip.irq_disable = ts_aon_gpio_irq_disable;
		ct->chip.irq_request_resources = ts_aon_gpio_irq_reqres;
		ct->chip.irq_release_resources = ts_aon_gpio_irq_relres;
		// ct->regs.ack = gpio_reg_convert(gpio, GPIO_PORTA_EOI);
		// ct->regs.mask = gpio_reg_convert(gpio, GPIO_INTMASK);
		ct->type = IRQ_TYPE_LEVEL_MASK;
	}

	irq_gc->chip_types[0].type = IRQ_TYPE_LEVEL_MASK;
	irq_gc->chip_types[1].type = IRQ_TYPE_EDGE_BOTH;
	irq_gc->chip_types[1].handler = handle_edge_irq;

	/*
	 * Request a shared IRQ since where MFD would have devices
	 * using the same irq pin
	 */
	err = devm_request_irq(gpio->dev, gpio->irq, ts_aon_gpio_irq_handler,
					IRQF_SHARED, "ts-aon-gpio", gpio);
	if (err) {
		dev_err(gpio->dev, "error requesting IRQ\n");
		irq_domain_remove(gpio->domain);
		gpio->domain = NULL;
		return;
	}

	for (hwirq = 0 ; hwirq < ngpio ; hwirq++)
		irq_create_mapping(gpio->domain, hwirq);

	gc->to_irq = ts_aon_gpio_to_irq;
}


static int ts_aon_gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node;
	struct ts_aon_gpio *ts_aon_gpio;
	struct gpio_chip *chip;

	ts_aon_gpio = devm_kzalloc(dev, sizeof(*ts_aon_gpio), GFP_KERNEL);
	if (!ts_aon_gpio)
		return -ENOMEM;

	node = of_parse_phandle(dev->of_node, "ts,smu", 0);
	if (!node) {
		dev_err(dev, "cannot find ts,smu\n");
		return -ENXIO;
	}

	ts_aon_gpio->smu_of_node = node;
	ts_aon_gpio->regmap = syscon_node_to_regmap(node);
	if (IS_ERR(ts_aon_gpio->regmap)) {
		dev_err(dev, "smu regmap error\n");
		return PTR_ERR(ts_aon_gpio->regmap);
	}

	ts_aon_gpio->cfg = (struct aon_gpios_cfg *)of_device_get_match_data(dev);
	if (!ts_aon_gpio->cfg) {
		dev_err(dev, "failed to get reg cfg.\n");
		return -EINVAL;
	}

	if (ts_aon_gpio->cfg->init) {
		if (ts_aon_gpio->cfg->init(ts_aon_gpio->regmap) < 0) {
			dev_err(dev, "cfg init failed.\n");
			return -EINVAL;
		}
	}

	ts_aon_gpio->dev = dev;
	chip = &ts_aon_gpio->gc;

	chip->parent = &pdev->dev;
	chip->of_node = dev->of_node;
	chip->label = "ts-aon-gpio";
	chip->owner = THIS_MODULE;
	chip->get = ts_aon_gpio_get;
	chip->set = ts_aon_gpio_set;
	chip->direction_input = ts_aon_gpio_direction_input;
	chip->direction_output = ts_aon_gpio_direction_output;
	chip->get_direction = ts_aon_gpio_get_direction;
	chip->set_config = ts_aon_gpio_set_config;
	chip->base = -1;
	chip->ngpio = ts_aon_gpio->cfg->num_reg_cfg;

	ts_aon_gpio_configure_irqs(ts_aon_gpio);
	platform_set_drvdata(pdev, ts_aon_gpio);

	pr_debug("%s, irq:%d\n", __func__, ts_aon_gpio->irq);

	return devm_gpiochip_add_data(dev, chip, ts_aon_gpio);
}

static const struct of_device_id ts_aon_gpio_of_match[] = {
	{ .compatible = "ts,aon-gpio-tx536", .data = &tx536_aon_gpios_cfg},
	{ .compatible = "ts,aon-gpio-tx5112", .data = &tx5112_aon_gpios_cfg},
	{ }
};
MODULE_DEVICE_TABLE(of, ts_aon_gpio_of_match);

static struct platform_driver ts_aon_gpio_driver = {
	.probe = ts_aon_gpio_probe,
	.driver = {
		.name = "ts-aon-gpio",
		.owner = THIS_MODULE,
		.of_match_table = ts_aon_gpio_of_match,
	},
};
module_platform_driver(ts_aon_gpio_driver);

MODULE_DESCRIPTION("TS AON GPIO driver");
MODULE_ALIAS("platform:ts-aon-gpio");
MODULE_LICENSE("GPL v2");
