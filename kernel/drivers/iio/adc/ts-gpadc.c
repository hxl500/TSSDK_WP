/*
 * Tsingmicro GPADC driver
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

#define TS_GPADC_MAX_CHANNELS		4
#define TS_GPADC_5336_MAX_CHANNELS		7

#define QW_ADC_VREF_OUT 1.8

#define ts_gpadc_readl(adc, reg) \
	(readl_relaxed(adc->base + reg))
#define ts_gpadc_writel(adc, reg, val) \
	(writel_relaxed(val, adc->base + reg))

/*
 * TS-GPADC registers definitions
 */
#define TS_GPADC_MAIN_CTL				(0x00) /*offset 0x00*/
#define TS_GPADC_CHAN_CTL				(0x04) /*offset 0x04*/
#define TS_GPADC_CHN_SMP_NUM			(0x0C) /*offset 0x0c*/
#define TS_GPADC_INT					(0x10) /*offset 0x10*/
#define TS_GPADC_INT_RAW				(0x14) /*offset 0x14*/
#define TS_GPADC_INT_EN						(0x18) /*offset 0x18*/
#define TS_GPADC_FIFO					(0x1C) /*offset 0x1c*/
#define TS_GPADC_SMP_CLK				(0x24) /*offset 0x24*/
#define TS_GPADC_DUTY_CYCLE				(0x30) /*offset 0x30*/
#define TS_GPADC_WAIT_CNT				(0x34) /*offset 0x34*/

/*TS_GPADC_MAIN_CTL register bit definition*/
#define TS_GPADC_GSEL_SHIFT				24
#define TS_GPADC_IREF_TRIM_SHIFT		21
#define TS_GPADC_IREF_TRIM_MASK			0x7
#define TS_GPADC_BUFF_EN_SHIFT			20
#define TS_GPADC_CLK_EN_SHIFT			19
#define TS_GPADC_VREF_TRIM_SHIFT		15
#define TS_GPADC_VREF_TRIM_MASK			0xF
#define TS_GPADC_AZ_SHIFT				14
#define TS_GPADC_FIFO_FLUSH_SHIFT		11
#define TS_GPADC_READ_REQ_THRESH_SHIFT	8
#define TS_GPADC_READ_REQ_THRESH_MASK	0x7
#define TS_GPADC_AUTO_SW_SHIFT			2
#define TS_GPADC_DMAS_ON_SHIFT			1
#define TS_GPADC_ALG_ADC_EN_SHIFT		0

/* TS_GPADC_CHAN_CTL register bit definition*/
#define TS_GPADC_CHAN_AUTO_SHIFT		8
#define TS_GPADC_CHAN_AUTO_MASK			0xF
#define TS_GPADC_CHAN_SW_SHIFT			0
#define TS_GPADC_CHAN_SW_MASK			0xF


/* TS_GPADC_INT register bit definition*/
#define TS_GPADC_FIFO_AF_SHIFT			6
#define TS_GPADC_FIFO_AE_SHIFT			5
#define TS_GPADC_FIFO_FULL_SHIFT			4
#define TS_GPADC_FIFO_EMPTY_SHIFT			3
#define TS_GPADC_FIFO_UF_SHIFT			2
#define TS_GPADC_FIFO_OF_SHIFT			1
#define TS_GPADC_DATA_RDY_SHIFT			0

/* TS_GPADC_INT_RAW register bit definition*/
#define TS_GPADC_FIFO_AF_RAW_SHIFT		6
#define TS_GPADC_FIFO_AE_RAW_SHIFT		5
#define TS_GPADC_FIFO_FULL_RAW_SHIFT		4
#define TS_GPADC_FIFO_EMPTY_RAW_SHIFT		3
#define TS_GPADC_FIFO_UF_RAW_SHIFT		2
#define TS_GPADC_FIFO_OF_RAW_SHIFT		1
#define TS_GPADC_DATA_RDY_RAW_SHIFT		0

/* TS_GPADC_INT_EN register bit definition*/
#define TS_GPADC_FIFO_UF_EN_SHIFT			2
#define TS_GPADC_FIFO_OF_EN_SHIFT			1
#define TS_GPADC_DATA_RDY_EN_SHIFT		0


/*T-sensor config*/
#define TS_TSENSOR_CFG 0xB0
#define TS_TSNSOR_0_RES_SHORT_LV_SHIFT	0
#define TS_TSNSOR_0_TSN_ENP_LV_SHIFT	1
#define TS_TSNSOR_1_RES_SHORT_LV_SHIFT	2
#define TS_TSNSOR_1_TSN_ENP_LV_SHIFT	3
#define TS_TSNSOR_2_RES_SHORT_LV_SHIFT	4
#define TS_TSNSOR_2_TSN_ENP_LV_SHIFT	5

#define TS_TSNSOR_0_TSN_COEF_SEL_LV_SHIFT	8
#define TS_TSNSOR_0_TSN_COEF_SEL_LV_MASK	0xF
#define TS_TSNSOR_0_TSN_TRM_LV_SHIFT		12
#define TS_TSNSOR_0_TSN_TRM_LV_MASK			0xF

#define TS_TSNSOR_1_TSN_COEF_SEL_LV_SHIFT	16
#define TS_TSNSOR_1_TSN_COEF_SEL_LV_MASK	0xF
#define TS_TSNSOR_1_TSN_TRM_LV_SHIFT		20
#define TS_TSNSOR_1_TSN_TRM_LV_MASK			0xF

#define TS_TSNSOR_2_TSN_COEF_SEL_LV_SHIFT	24
#define TS_TSNSOR_2_TSN_COEF_SEL_LV_MASK	0xF
#define TS_TSNSOR_2_TSN_TRM_LV_SHIFT		28
#define TS_TSNSOR_2_TSN_TRM_LV_MASK			0xF


/**
 * struct ts_gpadc_platform_data - platform specific data
 * @nchannels:		number of GPADC channels
 * @iio_channels:	iio channels
 * @tsensor_hw_init:	pointer to T-sensor hw init function
 */
struct ts_gpadc_platform_data {
	const int nchannels;
	const struct iio_chan_spec *iio_channels;
	int (*tsensor_hw_init)(void __iomem *sysreg_base);
};

struct ts_gpadc_state {
	struct regulator *vref;
	void __iomem *base;
	struct device *dev;
	struct mutex lock;
	struct clk *clk;
	int			irq;
	u32 value;
	struct completion completion;
	const struct ts_gpadc_platform_data *pdata;
};

#define TS_GPADC_CHAN(_idx) {				\
	.type = IIO_VOLTAGE,					\
	.indexed = 1,						\
	.channel = _idx,					\
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),		\
	.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),	\
}

#define TS_GPADC_TEMPERATURE_CHAN(_idx) {	\
	.type = IIO_TEMP,	\
	.indexed = 1,		\
	.channel = _idx,		\
	.info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED),	\
}
//IIO_CHAN_INFO_PROCESSED

static const struct iio_chan_spec ts_gpadc_iio_channels[] = {
	TS_GPADC_CHAN(0),
	TS_GPADC_CHAN(1),
	TS_GPADC_CHAN(2),
	TS_GPADC_CHAN(3),
};

static const struct iio_chan_spec ts_gpadc_5336_iio_channels[] = {
	TS_GPADC_CHAN(0),
	TS_GPADC_CHAN(1),
	TS_GPADC_CHAN(2),
	TS_GPADC_CHAN(3),
	TS_GPADC_TEMPERATURE_CHAN(4),
	TS_GPADC_TEMPERATURE_CHAN(5),
	TS_GPADC_TEMPERATURE_CHAN(6),
};

static int ts_gpadc_channel_switch(struct ts_gpadc_state *adc, unsigned int ch)
{
	int reg;

	reg = ts_gpadc_readl(adc, TS_GPADC_CHAN_CTL);
	reg &= ~(TS_GPADC_CHAN_SW_MASK << TS_GPADC_CHAN_SW_SHIFT);
	reg |= (ch << TS_GPADC_CHAN_SW_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_CHAN_CTL, reg);

	return 0;
}


static int ts_gpadc_get_fifo_data(struct ts_gpadc_state *adc)
{
	int data;

	data = ts_gpadc_readl(adc, TS_GPADC_FIFO);

	return data;
}

/**
 * fifo data
 * 31..28           25..16        15..12              9..0
 * chan_sel[3:0]   din[9:0]    chan_sel[3:0]     din[9:0]
 **/
static int ts_gpadc_read_raw(struct iio_dev *indio_dev,
				struct iio_chan_spec const *chan,
				int *val, int *val2, long mask)
{
	struct ts_gpadc_state *adc = iio_priv(indio_dev);
	int fifo_value;
	int adc_value;

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
	case IIO_CHAN_INFO_PROCESSED:
		mutex_lock(&adc->lock);
		/*channel sw*/
		ts_gpadc_channel_switch(adc, chan->channel);
		//wait_for_completion(&adc->completion); /* set by ISR */

		mdelay(1);
		fifo_value = ts_gpadc_get_fifo_data(adc);
		//printk("channel = %d,fifo data:0x%08x",chan->channel, fifo_value);

		if (((fifo_value >> 12) & 0xF) == chan->channel)
			adc_value = (fifo_value & 0x3FF);
		else
			adc_value = -1;

		switch (chan->type) {
		case IIO_VOLTAGE:
			*val = adc_value;
			break;
		case IIO_TEMP:
			/*
			 * value2 = 577 <---> Celsius2 = 48°C
			 * value1 = 559 <---> Celsius1 = 40°C
			 *
			 * scale = 1.7578125mv = 17578125/10000000 mv
			 * K = (value2*scale - value1*scale) / Celsius2 -Celsius1
			 * K =  3.955 mV/°C = 3955 mv/1000°C =  3955 mv/1000000 m°C
			 *
			 * and V=value1*scale at Celsius1
			 * Celsius = Celsius1*1000 + (value*sacle - value1*sacle) / K
			 *         = Celsius1*1000 + (value - value1)*sacle / K
			 *
			 * val = 40000 + (adc_value-559) * 17578125/10000000 * 1000000/3955
			 */

			*val = 40000 + (adc_value-559)*(17578125/39550); //m°C

			break;
		default:
			mutex_unlock(&adc->lock);
			return -EINVAL;
		}

		mutex_unlock(&adc->lock);
		return IIO_VAL_INT;

	case IIO_CHAN_INFO_SCALE:
		*val = QW_ADC_VREF_OUT * 1000; //mv
		*val2 = 10;

		return IIO_VAL_FRACTIONAL_LOG2;
	}

	return -EINVAL;
}

static int ts_gpadc_tsensor_hw_init(void __iomem *sysreg_base)
{
	u32 reg = 0;

	reg = readl(sysreg_base + TS_TSENSOR_CFG);
	reg |= (1 << TS_TSNSOR_0_TSN_ENP_LV_SHIFT);
	reg |= (1 << TS_TSNSOR_1_TSN_ENP_LV_SHIFT);
	reg |= (1 << TS_TSNSOR_2_TSN_ENP_LV_SHIFT);
	writel(reg, sysreg_base + TS_TSENSOR_CFG);
	return 0;
}

static int ts_gpadc_hw_init(struct ts_gpadc_state *adc)
{
	u32 reg = 0;

	u32 div = 1000;			//100000000/1000 = 100k
	u32 duty_cycle = 700;	// (div-duty_cycle)/div=30%

	u32 adc_wait_cnt = 100;
	u32 adc_vref_trim = 11;

	/* gpadc mclk needs to be less than 1M */
	ts_gpadc_writel(adc, TS_GPADC_SMP_CLK, div); /*bit15 - bit0*/

	/*set duty cycle*/
	ts_gpadc_writel(adc, TS_GPADC_DUTY_CYCLE, duty_cycle);/*bit15 - bit0*/

	/*set adc_gsel = 6dB*/
	reg = ts_gpadc_readl(adc, TS_GPADC_MAIN_CTL);
	reg &= ~(1 << TS_GPADC_GSEL_SHIFT);
	reg |= (0 << TS_GPADC_GSEL_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, reg);

	/*set vref_trim = 11*/
	reg = ts_gpadc_readl(adc, TS_GPADC_MAIN_CTL);
	reg &= ~(TS_GPADC_VREF_TRIM_MASK << TS_GPADC_VREF_TRIM_SHIFT);
	reg |= (adc_vref_trim << TS_GPADC_VREF_TRIM_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, reg);

	/*set adc_az = 1,enable*/
	reg = ts_gpadc_readl(adc, TS_GPADC_MAIN_CTL);
	reg |= (1 << TS_GPADC_AZ_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, reg);

	/*fifo flush*/
	reg = ts_gpadc_readl(adc, TS_GPADC_MAIN_CTL);
	reg |= (1 << TS_GPADC_FIFO_FLUSH_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, reg);

	reg = ts_gpadc_readl(adc, TS_GPADC_MAIN_CTL);
	reg &= ~(1 << TS_GPADC_FIFO_FLUSH_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, reg);

	/*disable auto sw*/
	reg = ts_gpadc_readl(adc, TS_GPADC_MAIN_CTL);
	reg &= ~(1 << TS_GPADC_AUTO_SW_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, reg);

	/*dmas enable*/
	reg = ts_gpadc_readl(adc, TS_GPADC_MAIN_CTL);
	reg |= (1 << TS_GPADC_DMAS_ON_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, reg);

	/*set adc_wait_cnt*/
	reg = ts_gpadc_readl(adc, TS_GPADC_WAIT_CNT);
	reg = adc_wait_cnt;
	ts_gpadc_writel(adc, TS_GPADC_WAIT_CNT, reg);

	/*enable gp_adc_buff_en*/
	reg = ts_gpadc_readl(adc, TS_GPADC_MAIN_CTL);
	reg |= (1 << TS_GPADC_BUFF_EN_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, reg);

	/*enable adc_clk_en*/
	reg = ts_gpadc_readl(adc, TS_GPADC_MAIN_CTL);
	reg |= (1 << TS_GPADC_CLK_EN_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, reg);

	/*enable adc*/
	reg = ts_gpadc_readl(adc, TS_GPADC_MAIN_CTL);
	reg |= (1 << TS_GPADC_ALG_ADC_EN_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, reg);

	return 0;
}

static const struct iio_info ts_gpadc_info = {
	.read_raw = ts_gpadc_read_raw,
	.driver_module = THIS_MODULE,
};

static const struct ts_gpadc_platform_data ts_gpadc_pdata = {
	.iio_channels = ts_gpadc_iio_channels,
	.nchannels = TS_GPADC_MAX_CHANNELS,
	.tsensor_hw_init = NULL,
};

static const struct ts_gpadc_platform_data ts_gpadc_5336_pdata = {
	.iio_channels = ts_gpadc_5336_iio_channels,
	.nchannels = TS_GPADC_5336_MAX_CHANNELS,
	.tsensor_hw_init = ts_gpadc_tsensor_hw_init,
};


static irqreturn_t ts_gpadc_isr(int irq, void *dev_id)
{
	struct iio_dev *indio_dev = dev_id;
	struct ts_gpadc_state *adc = iio_priv(indio_dev);

	/* Read int status */
	adc->irq = ts_gpadc_readl(adc, TS_GPADC_INT);

	/*Read FIFO data*/
	if (adc->irq & 0x1)
		adc->value = ts_gpadc_readl(adc, TS_GPADC_FIFO);

	/*clear int*/
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, adc->irq);

	return IRQ_HANDLED;
}


static const struct of_device_id ts_gpadc_match[] = {
	{
		.compatible = "tsingmicro,ts-gpadc",
		.data = &ts_gpadc_pdata,
	},

	{
		.compatible = "tsingmicro,ts-gpadc-5336",
		.data = &ts_gpadc_5336_pdata,
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ts_gpadc_match);


static int ts_gpadc_probe(struct platform_device *pdev)
{
	struct iio_dev *indio_dev;
	struct ts_gpadc_state *adc;
	struct resource *res;
	struct reset_control *rstc;
	int ret;
	const struct ts_gpadc_platform_data *pdata;
	const struct of_device_id *match;
	struct device *dev = &pdev->dev;
	u32 sysreg_addr = 0;
	void __iomem *sysreg_base;

	match = of_match_device(ts_gpadc_match, dev);
	if (!match)
		return -EINVAL;

	pdata = match->data;

	if (pdata->tsensor_hw_init != NULL) {
		/*get sysreg_addr for T-sensor config*/
		ret = device_property_read_u32(&pdev->dev, "sysreg_addr",
					&sysreg_addr);
		if (ret) {
			pr_err("Failed to read property grf_addr\n");
			return ret;
		}
		//dev_info(&pdev->dev, "sysreg_addr = 0x%X\n", sysreg_addr);

		sysreg_base = ioremap_nocache((unsigned long)sysreg_addr, 0x1000);
		if (!sysreg_base) {
			pr_err("Failed to map sysreg memory\n");
			return -ENOMEM;
		}
	}

	indio_dev = devm_iio_device_alloc(&pdev->dev, sizeof(*adc));
	if (!indio_dev) {
		dev_err(&pdev->dev, "Failed allocating iio device\n");
		return -ENOMEM;
	}

	adc = iio_priv(indio_dev);
	adc->dev = &pdev->dev;
	adc->pdata = pdata;

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

	ret = devm_request_irq(adc->dev, adc->irq, ts_gpadc_isr, 0,
				dev_name(&pdev->dev), indio_dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed requesting irq, irq = %d\n", adc->irq);
		return ret;
	}

#if 1
	adc->clk = devm_clk_get(&pdev->dev, "adc_pclk");
	if (IS_ERR(adc->clk)) {
		dev_err(&pdev->dev, "failed getting clock, err = %ld\n",
						PTR_ERR(adc->clk));
		return PTR_ERR(adc->clk);
	}
#endif

	platform_set_drvdata(pdev, indio_dev);

	init_completion(&adc->completion);

	indio_dev->name = dev_name(&pdev->dev);
	indio_dev->dev.parent = &pdev->dev;
	indio_dev->info = &ts_gpadc_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = pdata->iio_channels;
	indio_dev->num_channels = pdata->nchannels;

#if 1
	ret = clk_prepare_enable(adc->clk);
	if (ret) {
		dev_err(&pdev->dev,
			"Could not prepare or enable the clock.\n");

		return ret;
	}
#endif

	rstc = devm_reset_control_get_optional_exclusive(&pdev->dev, "adc");
	if (IS_ERR(rstc)) {
		ret = PTR_ERR(rstc);
		dev_err(&pdev->dev, "Cannot get adc reset.\n");
		return -1;
	}
	reset_control_deassert(rstc);

	ts_gpadc_hw_init(adc);

	if (pdata->tsensor_hw_init != NULL)
		ts_gpadc_tsensor_hw_init(sysreg_base);

	ret = iio_device_register(indio_dev);
	if (ret) {
		dev_err(&pdev->dev, "unable to register device\n");
		return ret;
	}

	dev_info(&pdev->dev, "TS GPADC driver loaded, IRQ %d\n", adc->irq);

	return 0;
}

static int ts_gpadc_remove(struct platform_device *pdev)
{
	struct iio_dev *indio_dev = platform_get_drvdata(pdev);
	struct ts_gpadc_state *adc = iio_priv(indio_dev);

	iio_device_unregister(indio_dev);
	clk_disable_unprepare(adc->clk);

	return 0;
}

static struct platform_driver ts_gpadc_driver = {
	.probe          = ts_gpadc_probe,
	.remove         = ts_gpadc_remove,
	.driver         = {
		.name   = "ts-gpadc",
		.of_match_table = ts_gpadc_match,
	},
};

module_platform_driver(ts_gpadc_driver);

MODULE_AUTHOR("Sheng Yang <yangsheng@tsingmicro.com>");
MODULE_DESCRIPTION("Tsingmicro GPADC driver");
MODULE_LICENSE("GPL v2");
