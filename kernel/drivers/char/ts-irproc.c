#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/of_gpio.h>
#include <linux/clk.h>
#include <linux/poll.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/major.h>

/* register summary */
#define IR_RX_EVENT                0x000 /* offset: 0x000 (R/ ) ir rx event */
#define IR_RX_CLK_DIV              0x004 /* offset: 0x004 (R/W) ir rx clock div */
#define IR_CONFIG                  0x008 /* offset: 0x008 (R/W) ir config register */
#define IR_IRQ_MASK                0x00c /* offset: 0x00c (R/W) ir interrupt mask */
#define IR_IRQ_STATUS              0x010 /* offset: 0x010 (R/ ) ir interrupt status */

#ifdef CONFIG_ARCH_TS_TX5336
#define SMU_BASE                   0xF0517000UL //0xF24E0000UL
#else
#define SMU_BASE                   0xF24E0000UL
#endif
#define SMU_IR_CTL                 0x088

void __iomem *smu_base;

/* register operation */
#define IR_ENABLE_EDGE_FALLING                    (1 << 0)
#define IR_ENABLE_EDGE_RISING                     (1 << 1)
#define IR_TIME_OUT_VALUE_H100                    (0 << 4)
#define IR_TIME_OUT_VALUE_H180                    (1 << 4)
#define IR_TIME_OUT_VALUE_H200                    (1 << 5)
#define IR_TIME_OUT_VALUE_H300                    (3 << 4)
#define IR_TIME_OUT_FLAG_CLEAR                    (1 << 6)

#define IR_IRQ_RX_MASK_NOT_EMPTY                  0x1
#define IR_IRQ_RX_MASK_HALF                       0x2
#define IR_IRQ_RX_MASK_FULL                       0x4

struct ts_irproc_dev {
	void __iomem *iobase;
	int	irq;
	struct class *class;
	struct device *dev;
	struct cdev cdev;
	dev_t devno;
	struct gpio_desc *irie_gpio;
	struct clk *clk;
	unsigned long rate;
};

int ts_irproc_open(struct inode *inode, struct file *file)
{
	return 0;
}

int ts_irproc_close(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t ts_irproc_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t ts_irproc_read(struct file *file, char __user *buf, size_t size,
				loff_t *ppos)
{
	return 0;
}


static const struct file_operations irproc_fops = {
	.owner = THIS_MODULE,
	.open = ts_irproc_open,
	.release = ts_irproc_close,
	.write = ts_irproc_write,
	.read = ts_irproc_read,
};

static irqreturn_t ts_irproc_isr(int irq, void *dev_id)
{
	struct ts_irproc_dev *ts_irproc = (struct ts_irproc_dev *)dev_id;
	void __iomem *addr = ts_irproc->iobase;
	unsigned char temp_data[125] = {0};
	int i, j, temp;
	unsigned int data;

	//pr_info("[%s]: %d---data = 0x%x\n", __func__, __LINE__, readl(addr + IR_RX_EVENT));

	//writel(0x0, addr + IR_IRQ_MASK);
#if 1
	/* store data */
	for (i = 0; i < 125; i++) {
		temp_data[i] = readl(addr + IR_RX_EVENT);
		//pr_info("[%s]: %d---data = 0x%x\n", __func__, __LINE__, temp_data[i]);
	}
	
	/* analytic data */
	for (i = 0; i < 93; i++) {
		if (temp_data[i] >= 0x7 && temp_data[i] <= 0x9) {
			temp = i;
			data = 0;
			for (j = 0; j < 32; j++, i++) {
				if (temp_data[i + 1] >= 0x13 && temp_data[i + 1] <= 0x18)
					data |= 1 << j;
			}
			
			i = temp;
		}
	}
	
	/* the parsed data */
	pr_info("[%s]: %d---data = 0x%08x\n", __func__, __LINE__, data);

	//writel(0x1, addr + IR_IRQ_MASK);
#endif	
	return IRQ_HANDLED;
}

static void ts_irproc_div_config(struct ts_irproc_dev *ts_irproc)
{
	void __iomem *addr;
	unsigned long temp = 0;

	addr = ts_irproc->iobase;
	addr += IR_RX_CLK_DIV;
	
	temp = temp | 0x3e7;
	writel(temp, addr);
}


static void ts_irproc_irq_config(struct ts_irproc_dev *ts_irproc)
{
	void __iomem *addr;
	unsigned long temp = 0;

	addr = ts_irproc->iobase;
	addr += IR_CONFIG;
	
	temp = temp | IR_ENABLE_EDGE_FALLING;
	writel(temp, addr);
}

static void ts_irproc_irq_mask_config(struct ts_irproc_dev *ts_irproc)
{
	void __iomem *addr;
	unsigned long temp = 0;

	addr = ts_irproc->iobase;
	addr += IR_IRQ_MASK;
	
	//temp = readl(addr);
	//temp |= IR_IRQ_RX_MASK_NOT_EMPTY;
	temp |= IR_IRQ_RX_MASK_HALF;
	//temp = IR_IRQ_RX_MASK_FULL;
	writel(temp, addr);
}

static void ts_irproc_init(struct ts_irproc_dev *ts_irproc)
{
	void __iomem *addr = ts_irproc->iobase;
	unsigned char temp_data[96] = {0};
	int i;

	/* IR_RX_DIV config */
	ts_irproc_div_config(ts_irproc);

	/* IR_CONFIG config */
	ts_irproc_irq_config(ts_irproc);
	
	/* IR_IRQ_MASK config, enable all status interrupt */
	ts_irproc_irq_mask_config(ts_irproc);

	/* clear fifo */
	for (i = 0; i < 96; i++) {
		temp_data[i] = readl(addr + IR_RX_EVENT);
		//pr_info("[%s]: %d---data = 0x%x\n", __func__, __LINE__, temp_data[i]);
	}
}

static void ts_irproc_open_smu(void)
{
  	unsigned long temp;
	
  	smu_base = ioremap(SMU_BASE, 0x1000);
  	temp = readl(smu_base + SMU_IR_CTL);
  	temp &= (~(0x1 << 0));
	/* use 32K clock, via ir wake up from ext to run need 32K clock */
	//temp |= (0x1 << 16);
	writel(temp, smu_base + SMU_IR_CTL);
}

static int ts_irproc_device_node_init(struct ts_irproc_dev *ts_irproc)
{
	int ret = 0;

#if defined(TS_IR_MAJOR)
	ts_irproc->devno = MKDEV(TS_IR_MAJOR, 0);
#endif

	if (ts_irproc->devno) {
		ret = register_chrdev_region(ts_irproc->devno, 1, "irproc");
	} else {
		ret = alloc_chrdev_region(&ts_irproc->devno, 0, 1, "irproc");
	}	
	if (ret < 0) {
		pr_err("Error %d while alloc chrdev irproc\n", ret);
		goto err_req_cdev;
	}

	cdev_init(&ts_irproc->cdev, &irproc_fops);
	ts_irproc->cdev.owner = THIS_MODULE;
	ret = cdev_add(&ts_irproc->cdev, ts_irproc->devno, 0);
	if (ret) {
		pr_err("Error %d while adding irproc cdev\n", ret);
		goto err_req_cdev;
	}

	ts_irproc->class = class_create(THIS_MODULE, "irproc_class");
	if (IS_ERR(ts_irproc->class)) {
		pr_err("irproc class create fail\n");
		goto err_cr_cls;
	}

	ts_irproc->dev = device_create(ts_irproc->class, NULL, MKDEV(MAJOR(ts_irproc->devno), 0), NULL, "irproc_dev");
	if (IS_ERR(ts_irproc->dev)) {
		ret = -EINVAL;
		pr_err("irproc device create fail\n");
		goto err_dev;
	}
	
	//pr_err("[%s]: %d---ok!\n", __func__, __LINE__);
	
	return ret;

err_dev:
	device_destroy(ts_irproc->class, MKDEV(MAJOR(ts_irproc->devno), 0));
	class_destroy(ts_irproc->class);
err_cr_cls:
	cdev_del(&ts_irproc->cdev);
err_req_cdev:
	unregister_chrdev_region(ts_irproc->devno, 0);
	
	return ret;
}

static int ts_irproc_probe(struct platform_device *pdev)
{
	struct ts_irproc_dev *ts_irproc;
	struct resource *mem_res;
	int ret = 0;
	
	//pr_info("%s [%d]\n", __func__, __LINE__);

	ts_irproc = devm_kzalloc(&pdev->dev, sizeof(*ts_irproc), GFP_KERNEL);
	if (!ts_irproc) {
		ret = -ENOMEM;
		pr_err("kzmalloc irproc err\n");
		return ret;
	}
	
	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ts_irproc->iobase = devm_ioremap_resource(&pdev->dev, mem_res);
	if (!ts_irproc->iobase) {
		pr_err("Failed to remap io region(%p)\n", ts_irproc->iobase);
		ret = -ENOMEM;
		return ret;
	}
	//pr_err("[%s]: %d---iobase = 0x%p\n", __func__, __LINE__, ts_irproc->iobase);
#if 0	
	ts_irproc->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(ts_irproc->clk)) {
		dev_err(&pdev->dev, "Cannot claim ts ir clock.\n");
		return PTR_ERR(ts_irproc->clk);
	}

	ret = clk_prepare_enable(ts_irproc->clk);
	if (ret) {
		dev_err(&pdev->dev, "Cannot enable ts ir clock.\n");
		return ret;
	}

	ts_irproc->rate = clk_get_rate(ts_irproc->clk);
	if (ts_irproc->rate == 0) {
		ret = -EINVAL;
		return ret;
	}
	pr_info("irproc_clock_rate = %ld\n", ts_irproc->rate);
#endif
	ts_irproc->irq = platform_get_irq(pdev, 0);
	if (ts_irproc->irq < 0) {
		pr_err("Failed to get irproc irq(%d)\n", ts_irproc->irq);
		ret = -EBUSY;
		return ret;
	}

	ret = devm_request_irq(&pdev->dev, ts_irproc->irq, ts_irproc_isr, 0, "ts-irproc", ts_irproc);
	if (ret) {
		pr_err("request_irq %d err %d\n", ts_irproc->irq, ret);
		return ret;
	}

	ret = ts_irproc_device_node_init(ts_irproc);
	if (ret) {
		pr_err("device_node_init fail %d\n", ret);
		return ret;
	}

	ts_irproc_open_smu();
	
	ts_irproc_init(ts_irproc);

	platform_set_drvdata(pdev, ts_irproc);
	
	//pr_info("%s [%d]\n", __func__, __LINE__);
	
	return ret;
}

static int ts_irproc_remove(struct platform_device *pdev)
{
	struct ts_irproc_dev *ts_irproc;
	
	//pr_info("%s [%d]\n", __func__, __LINE__);

	ts_irproc = platform_get_drvdata(pdev);

	device_destroy(ts_irproc->class, MKDEV(MAJOR(ts_irproc->devno), 0));
	class_destroy(ts_irproc->class);
	cdev_del(&ts_irproc->cdev);
	unregister_chrdev_region(ts_irproc->devno, 0);

	iounmap(smu_base);
	//pr_info("%s [%d]\n", __func__, __LINE__);
	
	return 0;
}


static const struct of_device_id irproc_of_match[] = {
	{.compatible = "ts,ts-irproc"},
	{ },
};

static struct platform_driver ts_irproc_driver = {
	.driver             = {
		.name           = "ts-irproc",
		.of_match_table = irproc_of_match,
	},
	.probe              = ts_irproc_probe,
	.remove             = ts_irproc_remove,
};

module_platform_driver(ts_irproc_driver);

MODULE_LICENSE("GPL");

