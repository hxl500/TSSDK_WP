/*
 * Tsingmicro ADC driver
 *
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 *
 */

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/iio/iio.h>
#include <linux/iio/driver.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/interrupt.h>
#include <linux/reset.h>

#define ts_adc_readl(adc, reg) \
	(readl_relaxed(adc->base + reg))
#define ts_adc_writel(adc, reg, val) \
	(writel_relaxed(val, adc->base + reg))

#define QW_ADC_CHANNEL_NUM          6
#define ADC_BUF_CAPABILITY  16
#define QW_ADC_DATA_WIDTH 0xfff

/*
 * TS-ADC registers definitions
 */
#define TS_ADC_CTRL          (0x00) /* offset: 0x00 (R/W) A/D Control Register */
#define TS_ADC_BAUDR         (0x04) /* offset: 0x04 (R/W) A/D Clock baudr */
#define TS_ADC_PD_SEOC_TIME  (0x08) /* offset: 0x08 (R/W) A/D Power down clock set */
#define TS_ADC_SOC_WIDTH     (0x0C) /* offset: 0x0c (R/W) A/D Soc width set */
#define TS_ADC_SCAN_TIME     (0x10) /* offset: 0x10 (R/W) A/D Scan time interval set (only PD use) */
#define TS_ADC_CH_VLD        (0x14) /* offset: 0x14 (R/W) A/D Channel valid register */
#define TS_ADC_INTR_EN       (0x18) /* offset: 0x18 (R/W) A/D Interrupt enable */
#define TS_ADC_INTR_CONFIG   (0x1C) /* offset: 0x1c (R/W) A/D Interrupt config */
#define TS_ADC_INTR_STA      (0x20) /* offset: 0x20 (R/W) A/D Interrupt state */
#define TS_ADC_CTRL_START    (0x24) /* offset: 0x24 ( /W) A/D Start register */
#define TS_ADC_CTRL_STOP     (0x28) /* offset: 0x28 ( /W) A/D Stop register */
#define TS_ADC_DATA_AB       (0x2C) /* offset: 0x2c (R/ ) A/D A/B channel data */
#define TS_ADC_DATA_CD       (0x30) /* offset: 0x30 (R/ ) A/D C/D channel data */
#define TS_ADC_DATA_EF       (0x34) /* offset: 0x34 (R/ ) A/D E/F channel data */
#define TS_ADC_DATA_GH       (0x38) /* offset: 0x38 (R/ ) A/D G/H channel data */
#define TS_ADC_CTRL_STATE    (0x3C) /* offset: 0x3c (R/W) A/D Control state */
#define TS_ADC_CTRL_VERID    (0x40) /* offset: 0x40 (R/ ) A/D Control verid */
#define TS_ADC_DIR_DAT_OP    (0x44) /* offset: 0x44 (R/ ) A/D Driect data */

/****************************************************/
/*registers bit definition*/
/*ADC CTRL register*/
#define QW_ADC_CTRL_MODE_SEL_BIT          1
#define QW_ADC_CTRL_RESET_BIT             3
#define QW_ADC_CTRL_ADC_CLK_ADJUST_BIT    6

/* ADC_SOC_WIDTH register */
#define QW_ADC_SOC_WIDTH_ENADC_CTRL_BIT   8
#define QW_ADC_SOC_WIDTH_MASK             0x7F
#define QW_ADC_SOC_WIDTH_RESET_BIT        10
#define QW_ADC_SOC_WIDTH_SEL_BG_BIT       9
#define QW_ADC_SOC_WIDTH_SEL_REF_BIT      7

/* ADC_INTR_CONFIG register */
#define QW_ADC_INTR_CFG_BIT             16

/* channel enable */
#define QW_ADC_CHA_VLD_BIT                0
#define QW_ADC_CHB_VLD_BIT                1
#define QW_ADC_CHC_VLD_BIT                2
#define QW_ADC_CHD_VLD_BIT                3
#define QW_ADC_CHE_VLD_BIT                4
#define QW_ADC_CHF_VLD_BIT                5


/* interrupt enable */
#define QW_ADC_IC_EN_CHA_BIT              0
#define QW_ADC_IC_EN_CHB_BIT              1
#define QW_ADC_IC_EN_CHC_BIT              2
#define QW_ADC_IC_EN_CHD_BIT              3
#define QW_ADC_IC_EN_CHE_BIT              4
#define QW_ADC_IC_EN_CHF_BIT              5

#define QW_ADC_VREF_INTER 1.2
#define QW_ADC_VREF_OUT 1.8
/* ADC state */
#define QW_ADC_SR_BUSY_BIT                0


struct ts_adc_state {
	struct regulator *vref;
	void __iomem *base;
	struct device *dev;
	struct mutex lock;
	struct clk *clk;
	int			irq;
};

#define TS_ADC_CHAN(_idx) {				\
	.type = IIO_VOLTAGE,					\
	.indexed = 1,						\
	.channel = _idx,					\
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),		\
	.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),	\
}

static const struct iio_chan_spec ts_adc_iio_channels[] = {
	TS_ADC_CHAN(0),
	TS_ADC_CHAN(1),
	TS_ADC_CHAN(2),
	TS_ADC_CHAN(3),
	TS_ADC_CHAN(4),
	TS_ADC_CHAN(5),
};

static int ts_adc_read_chan(struct ts_adc_state *adc, unsigned int ch)
{
	int reg;
	int data;

	switch (ch) {
	case 0:
		reg = ts_adc_readl(adc, TS_ADC_DATA_AB);
		data = reg & 0xfff;
		break;
	case 1:
		reg = ts_adc_readl(adc, TS_ADC_DATA_AB);
		data = (reg >> 16) & 0xfff;
		break;
	case 2:
		reg = ts_adc_readl(adc, TS_ADC_DATA_CD);
		data = reg & 0xfff;
		break;
	case 3:
		reg = ts_adc_readl(adc, TS_ADC_DATA_CD);
		data = (reg >> 16) & 0xfff;
		break;
	case 4:
		reg = ts_adc_readl(adc, TS_ADC_DATA_EF);
		data = reg & 0xfff;
		break;
	case 5:
		reg = ts_adc_readl(adc, TS_ADC_DATA_EF);
		data = (reg >> 16) & 0xfff;
		break;
	}

	return data;

}

static int ts_adc_read_raw(struct iio_dev *indio_dev,
				struct iio_chan_spec const *chan,
				int *val, int *val2, long mask)
{
	struct ts_adc_state *adc = iio_priv(indio_dev);

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		mutex_lock(&adc->lock);
		*val = ts_adc_read_chan(adc, chan->channel);
		mutex_unlock(&adc->lock);
		if (*val < 0)
			return *val;

		return IIO_VAL_INT;

	case IIO_CHAN_INFO_SCALE:
		*val = QW_ADC_VREF_OUT * 1000; //mv
		*val2 = 12;

		return IIO_VAL_FRACTIONAL_LOG2;
	}

	return -EINVAL;
}

static int ts_adc_hw_init(struct ts_adc_state *adc)
{
	u32 reg = 0;

	/*reset adc ctrl*/
	reg = ts_adc_readl(adc, TS_ADC_CTRL);
	reg |= (1 << QW_ADC_CTRL_RESET_BIT);
	ts_adc_writel(adc, TS_ADC_SOC_WIDTH, reg);
	mdelay(1);
	/*release reset adc ctrl*/
	reg = ts_adc_readl(adc, TS_ADC_CTRL);
	reg &= ~(1 << QW_ADC_CTRL_RESET_BIT);
	ts_adc_writel(adc, TS_ADC_SOC_WIDTH, reg);

	/* set adc clk div */
	ts_adc_writel(adc, TS_ADC_BAUDR, 0x64);

	/*reset adc*/
	reg = ts_adc_readl(adc, TS_ADC_SOC_WIDTH);
	reg |= (1 << QW_ADC_SOC_WIDTH_RESET_BIT);
	ts_adc_writel(adc, TS_ADC_SOC_WIDTH, reg);

	/*set normal work mode*/
	reg = ts_adc_readl(adc, TS_ADC_SOC_WIDTH);
	reg |= (1 << QW_ADC_SOC_WIDTH_ENADC_CTRL_BIT);
	ts_adc_writel(adc, TS_ADC_SOC_WIDTH, reg);

	/*adc soc width is 1*/
	reg = ts_adc_readl(adc, TS_ADC_SOC_WIDTH);
	reg &= ~QW_ADC_SOC_WIDTH_MASK;
	reg |= 0x1;
	ts_adc_writel(adc, TS_ADC_SOC_WIDTH, reg);

	/*choose out ref*/
	reg = ts_adc_readl(adc, TS_ADC_SOC_WIDTH);
	reg &= ~(1 << QW_ADC_SOC_WIDTH_SEL_REF_BIT);
	reg &= ~(1 << QW_ADC_SOC_WIDTH_SEL_BG_BIT);
	ts_adc_writel(adc, TS_ADC_SOC_WIDTH, reg);

	/* disable all intr */
	ts_adc_writel(adc, TS_ADC_INTR_EN, 0x0);

	/* close adc PD_FORCE_HIGH */
	ts_adc_writel(adc, TS_ADC_CTRL, 0x0);

	/*adc clk adjust*/
	reg = ts_adc_readl(adc, TS_ADC_CTRL);
	reg |= (0x1 << QW_ADC_CTRL_ADC_CLK_ADJUST_BIT);
	ts_adc_writel(adc, TS_ADC_CTRL, reg);

	/*disable reset adc*/
	reg = ts_adc_readl(adc, TS_ADC_SOC_WIDTH);
	reg &= ~(1 << QW_ADC_SOC_WIDTH_RESET_BIT);
	ts_adc_writel(adc, TS_ADC_SOC_WIDTH, reg);

	return 0;
}

static int ts_adc_intr_warning_config(struct ts_adc_state *adc)
{
	float verf = 0.0;
	uint32_t delta = 0;
	uint32_t threshold = 0;
	u32		reg = 0;

	reg = ts_adc_readl(adc, TS_ADC_SOC_WIDTH);

	if ((reg >> QW_ADC_SOC_WIDTH_SEL_REF_BIT) & 0x1)
		verf = QW_ADC_VREF_INTER;
	else
		verf = QW_ADC_VREF_OUT;

	threshold = (u32)(1.8 * QW_ADC_DATA_WIDTH / verf) & QW_ADC_DATA_WIDTH;
	delta = (u32)(0.1 * QW_ADC_DATA_WIDTH / verf) & QW_ADC_DATA_WIDTH;

	reg = ts_adc_readl(adc, TS_ADC_INTR_CONFIG);
	reg = (threshold << QW_ADC_INTR_CFG_BIT) | delta;
	ts_adc_writel(adc, TS_ADC_INTR_CONFIG, reg);

	return 0;
}

static int ts_adc_config(struct ts_adc_state *adc)
{
	u32		i = 0;
	u32		reg = 0;

	for (i = 0; i < QW_ADC_CHANNEL_NUM; i++) {
		/* enable all channel */
		reg = ts_adc_readl(adc, TS_ADC_CH_VLD);
		reg |= (0x1 << i);
		ts_adc_writel(adc, TS_ADC_CH_VLD, reg);

		/* disable all channel interrupt */
		reg = ts_adc_readl(adc, TS_ADC_INTR_EN);
		reg &= ~(0x1 << i);
		ts_adc_writel(adc, TS_ADC_INTR_EN, reg);
	}

	ts_adc_intr_warning_config(adc);

	/*set continue mode*/
	reg = ts_adc_readl(adc, TS_ADC_CTRL);
	reg |= (0x1 << QW_ADC_CTRL_MODE_SEL_BIT);
	ts_adc_writel(adc, TS_ADC_CTRL, reg);

	/*start adc*/
	ts_adc_writel(adc, TS_ADC_CTRL_START, 1);

	return 0;
}

static int ts_adc_reg_access(struct iio_dev *indio_dev,
			unsigned int reg, unsigned int writeval,
			unsigned int *readval)
{
	struct ts_adc_state *adc = iio_priv(indio_dev);

	if ((readval == NULL) ||
		((reg % 4) || (reg > TS_ADC_DIR_DAT_OP)))
		return -EINVAL;

	*readval = readl(adc->base + reg);

	return 0;
}
static const struct iio_info ts_adc_info = {
	.read_raw = ts_adc_read_raw,
	.debugfs_reg_access = &ts_adc_reg_access,
	.driver_module = THIS_MODULE,
};

static irqreturn_t ts_adc_isr(int irq, void *dev_id)
{
	/*intterupt handle*/

	return IRQ_HANDLED;
}

static int ts_adc_probe(struct platform_device *pdev)
{
	struct iio_dev *indio_dev;
	struct ts_adc_state *adc;
	struct resource *res;
	struct reset_control *rstc;
	int ret;
	struct regulator *regulator;

	indio_dev = devm_iio_device_alloc(&pdev->dev, sizeof(*adc));
	if (!indio_dev) {
		dev_err(&pdev->dev, "Failed allocating iio device\n");
		return -ENOMEM;
	}

	adc = iio_priv(indio_dev);
	adc->dev = &pdev->dev;
	mutex_init(&adc->lock);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "failed to get platform I/O memory\n");
		return -ENXIO;
	}

	adc->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(adc->base))
		return PTR_ERR(adc->base);

	adc->irq = platform_get_irq(pdev, 0);
	if (adc->irq <= 0) {
		dev_err(&pdev->dev, "failed getting interrupt resource\n");
		return -ENXIO;
	}

	ret = devm_request_irq(adc->dev, adc->irq,
			ts_adc_isr, 0,
			dev_name(&pdev->dev), indio_dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed requesting irq, irq = %d\n", adc->irq);
		return ret;
	}

	adc->clk = devm_clk_get(&pdev->dev, "adc_pclk");
	if (IS_ERR(adc->clk)) {
		dev_err(&pdev->dev, "failed getting clock, err = %ld\n",
						PTR_ERR(adc->clk));
		return PTR_ERR(adc->clk);
	}

	/*power operation*/
	regulator = devm_regulator_get_optional(&pdev->dev, "name");
	if (IS_ERR(regulator)) {
		ret = PTR_ERR(regulator);
		dev_err(&pdev->dev, "Failed to request %d\n", ret);
	} else{
		ret = regulator_set_voltage(regulator, 1800000, 1800000);
		if (ret)
			dev_err(&pdev->dev, "regulator_set_voltage error\n");

		ret = regulator_enable(regulator);
		if (ret)
			dev_err(&pdev->dev, "Couldn't enable regulator\n");
	}

	platform_set_drvdata(pdev, indio_dev);

	indio_dev->name = dev_name(&pdev->dev);
	indio_dev->dev.parent = &pdev->dev;
	indio_dev->info = &ts_adc_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = ts_adc_iio_channels;
	indio_dev->num_channels = ARRAY_SIZE(ts_adc_iio_channels);

	ret = clk_prepare_enable(adc->clk);
	if (ret) {
		dev_err(&pdev->dev,
			"Could not prepare or enable the clock.\n");
		// goto error_adc_clk_enable;
		return ret;
	}

	rstc = devm_reset_control_get_optional_exclusive(&pdev->dev, "adc");
	if (IS_ERR(rstc)) {
		ret = PTR_ERR(rstc);
		dev_err(&pdev->dev, "Cannot get adc reset.\n");
		return -1;
	}
	reset_control_deassert(rstc);

	ts_adc_hw_init(adc);
	ts_adc_config(adc);

	ret = iio_device_register(indio_dev);
	if (ret) {
		dev_err(&pdev->dev, "unable to register device\n");
		return ret;
	}
	dev_info(&pdev->dev, "TS ADC driver loaded, IRQ %d\n", adc->irq);

	return 0;
}

static int ts_adc_remove(struct platform_device *pdev)
{
	struct iio_dev *indio_dev = platform_get_drvdata(pdev);
	struct ts_adc_state *adc = iio_priv(indio_dev);

	iio_device_unregister(indio_dev);
	clk_disable_unprepare(adc->clk);

	return 0;
}

static const struct of_device_id ts_adc_match[] = {
	{ .compatible = "tsingmicro,ts-adc" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ts_adc_match);

static struct platform_driver ts_adc_driver = {
	.probe          = ts_adc_probe,
	.remove         = ts_adc_remove,
	.driver         = {
		.name   = "ts-adc",
		.of_match_table = ts_adc_match,
	},
};

module_platform_driver(ts_adc_driver);

MODULE_AUTHOR("Sheng Yang <yangsheng@tsingmicro.com>");
MODULE_DESCRIPTION("Tsingmicro ADC driver");
MODULE_LICENSE("GPL v2");
