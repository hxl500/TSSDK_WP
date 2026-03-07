/*
 * Tsingmicro GPADC driver,implement not in iio frame
 *
 * Copyright (C) 2022-2022 Tsingmicro, Inc.
 *
 */
#include <linux/clk.h>
#include <linux/err.h>
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
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/major.h>

#define QW_ADC_VREF_OUT 1.8
#define TS_GPADC_CHANNEL_NUM 4

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

#define TS_GPADC_DEV_CNT		1			/* 设备号长度*/
#define TS_GPADC_DEV_NAME		"ts_gpadc"	/* 设备名字*/
/* ts_gpadc dev 设备结构体 */
struct ts_gpadc_dev {
	dev_t devid;
	struct cdev cdev;
	struct class *class;
	struct device *device;
	int major;
	struct device_node *node;

	void __iomem *base;
	struct clk *clk;
	int			irq;
};

struct ts_gpadc_dev *ts_gpadc_dev;

static int ts_gpadc_channel_switch(struct ts_gpadc_dev *adc, unsigned int ch)
{
	int reg;

	reg = ts_gpadc_readl(adc, TS_GPADC_CHAN_CTL);
	reg &= ~(TS_GPADC_CHAN_SW_MASK << TS_GPADC_CHAN_SW_SHIFT);
	reg |= (ch << TS_GPADC_CHAN_SW_SHIFT);
	ts_gpadc_writel(adc, TS_GPADC_CHAN_CTL, reg);

	return 0;
}


static int ts_gpadc_get_fifo_data(struct ts_gpadc_dev *adc)
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
static ssize_t ts_gpadc_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;
	int fifo_value;
	int val = 0;
	int channel = 0;
	struct ts_gpadc_dev *adc = (struct ts_gpadc_dev *)(file->private_data);

	err = copy_from_user(&channel, buf, size);

	if (channel >= TS_GPADC_CHANNEL_NUM) {
		// printk("ts_gpadc channel=%d is exceed %d", channel, TS_GPADC_CHANNEL_NUM);
		return -1;
	}

	// printk("channel = %d",channel);

	/*channel sw*/
	ts_gpadc_channel_switch(adc, channel);
	//wait_for_completion(&adc->completion); /* set by ISR */

	mdelay(1);
	fifo_value = ts_gpadc_get_fifo_data(adc);
	//printk("channel = %d,fifo data:0x%08x",chan->channel, fifo_value);

	if (((fifo_value >> 12) & 0xF) == channel)
		val = (fifo_value & 0x3FF);
	else
		val = -1;

	err = copy_to_user(buf, &val, size);

	return size;
}

static int ts_gpadc_open(struct inode *inode, struct file *file)
{
	// printk("ts_gpadc_open");
	file->private_data = ts_gpadc_dev; /* 设置私有数据  */
	return 0;
}

static int ts_gpadc_close(struct inode *inode, struct file *filp)
{
	return 0;
}


static int ts_gpadc_hw_init(struct ts_gpadc_dev *adc)
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

static irqreturn_t ts_gpadc_isr(int irq, void *data)
{
	struct ts_gpadc_dev *adc = (struct ts_gpadc_dev *)data;
	u32 int_status = 0;
	u32 value = 0;

	/* Read int status */
	int_status = ts_gpadc_readl(adc, TS_GPADC_INT);

	/*Read FIFO data*/
	if (int_status & 0x1)
		value = ts_gpadc_readl(adc, TS_GPADC_FIFO);

	/*clear int*/
	ts_gpadc_writel(adc, TS_GPADC_MAIN_CTL, int_status);

	return IRQ_HANDLED;
}

/* 2. 定义自己的file_operations结构体                                              */
static const struct file_operations ts_gpadc_fops = {
	.owner	 = THIS_MODULE,
	.open    = ts_gpadc_open,
	.read    = ts_gpadc_read,
	//.write   = adc_drv_write,
	.release = ts_gpadc_close,
};

static int ts_gpadc_probe(struct platform_device *pdev)
{
	struct ts_gpadc_dev *adc;
	struct resource *res;
	struct reset_control *rstc;
	int ret;

	adc = kzalloc(sizeof(struct ts_gpadc_dev), GFP_KERNEL);
	if (!adc) {
		// pr_err("kzmalloc adc err\n");
		return -ENOMEM;
	}
	ts_gpadc_dev = adc;

	// printk("ts_gpadc_probe driver and device was matched!\r\n");
	/* 1、设置设备号 */
#if defined(TS_GPADC_MAJOR)
	ts_gpadc_dev->major = TS_GPADC_MAJOR;
#endif

	if (ts_gpadc_dev->major) {
		ts_gpadc_dev->devid = MKDEV(ts_gpadc_dev->major, 0);
		register_chrdev_region(ts_gpadc_dev->devid, TS_GPADC_DEV_CNT, TS_GPADC_DEV_NAME);
	} else {
		alloc_chrdev_region(&ts_gpadc_dev->devid, 0, TS_GPADC_DEV_CNT, TS_GPADC_DEV_NAME);
		ts_gpadc_dev->major = MAJOR(ts_gpadc_dev->devid);
	}

	/* 2、注册设备      */
	cdev_init(&ts_gpadc_dev->cdev, &ts_gpadc_fops);
	cdev_add(&ts_gpadc_dev->cdev, ts_gpadc_dev->devid, TS_GPADC_DEV_CNT);

	/* 3、创建类      */
	ts_gpadc_dev->class = class_create(THIS_MODULE, TS_GPADC_DEV_NAME);
	if (IS_ERR(ts_gpadc_dev->class))
		return PTR_ERR(ts_gpadc_dev->class);

	/* 4、创建设备 */
	ts_gpadc_dev->device = device_create(ts_gpadc_dev->class, NULL, ts_gpadc_dev->devid, NULL, TS_GPADC_DEV_NAME);
	if (IS_ERR(ts_gpadc_dev->device))
		return PTR_ERR(ts_gpadc_dev->device);

	/*get base*/
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "failed to get platform I/O memory\n");
		return -ENXIO;
	}

	adc->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(adc->base))
		return PTR_ERR(adc->base);

	/*get irq*/
	adc->irq = platform_get_irq(pdev, 0);
	if (adc->irq <= 0) {
		dev_err(&pdev->dev, "failed getting interrupt resource\n");
		return -ENXIO;
	}

	ret = request_irq(adc->irq, ts_gpadc_isr, IRQF_TRIGGER_RISING,
			  "ts_gpadc", adc);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed requesting irq, irq = %d\n", adc->irq);
		return ret;
	}

	/*get irq*/
	adc->clk = devm_clk_get(&pdev->dev, "adc_pclk");
	if (IS_ERR(adc->clk)) {
		dev_err(&pdev->dev, "failed getting clock, err = %ld\n",
						PTR_ERR(adc->clk));
		return PTR_ERR(adc->clk);
	}


	ret = clk_prepare_enable(adc->clk);
	if (ret) {
		dev_err(&pdev->dev,
			"Could not prepare or enable the clock.\n");

		return ret;
	}

	rstc = devm_reset_control_get_optional_exclusive(&pdev->dev, "adc");
	if (IS_ERR(rstc)) {
		ret = PTR_ERR(rstc);
		dev_err(&pdev->dev, "Cannot get adc reset.\n");
		return -1;
	}
	reset_control_deassert(rstc);

	ts_gpadc_hw_init(adc);

	platform_set_drvdata(pdev, adc);

	dev_info(&pdev->dev, "TS GPADC driver2 loaded, IRQ %d\n", adc->irq);

	return 0;
}

static int ts_gpadc_remove(struct platform_device *pdev)
{
	struct ts_gpadc_dev *adc;

	adc = platform_get_drvdata(pdev);

	clk_disable_unprepare(adc->clk);

	return 0;
}

static const struct of_device_id ts_gpadc_match[] = {
	{ .compatible = "tsingmicro,ts-gpadc" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ts_gpadc_match);

static struct platform_driver ts_gpadc_driver = {
	.probe          = ts_gpadc_probe,
	.remove         = ts_gpadc_remove,
	.driver         = {
		.name   = "ts-gpadc",
		.of_match_table = ts_gpadc_match,
	},
};

module_platform_driver(ts_gpadc_driver);

MODULE_AUTHOR("tsingmicro.com>");
MODULE_DESCRIPTION("Tsingmicro GPADC driver Ver2");
MODULE_LICENSE("GPL v2");
