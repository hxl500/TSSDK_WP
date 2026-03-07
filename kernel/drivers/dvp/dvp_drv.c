/*****************************************************************************
 * Copyright (C) 2022,Tsing Micro Technology Co., Ltd,All rights reserved.
 * Description:dvp driver;
 *
 *****************************************************************************/
 
#define pr_fmt(fmt)	"dvp_drv: " fmt

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/of_device.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/io.h>
#include <linux/reset.h>
#include <linux/delay.h>
#include <linux/clk.h>

//#include "../sys/sys_drv.h"

#include "dvp_drv.h"
#include "dvp_reg.h"


#define MODULE_NAME "DVP"

#define MAX_DVP_DEVICE       3

#define SRC_ADDR             0x80000000
#define DST_ADDR             0x20000000

#define DVP_WIDTH            1920
#define DVP_HIGH             1080

unsigned int *src_addr;
unsigned int *dst_addr;

unsigned int ddr_address;

unsigned int *tmp_addr;

struct dvp_dev *g_dvp;

static int i = 0;
static int j = 0;

int dvp_open(struct inode *inode, struct file *file)
{
	pr_err("[%s]: %d---\n", __func__, __LINE__);
	
	return 0;
}

int dvp_close(struct inode *inode, struct file *file)
{
	pr_err("[%s]: %d---\n", __func__, __LINE__);
	
	return 0;
}

static ssize_t dvp_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	unsigned int val;
	int ret;
	
	ret = copy_from_user(&val, buf, count);
	if (ret) {
 		pr_err("[%s]: %d---copy from user failed!\n", __func__, __LINE__);
 		return ret;
 	}
	
	ddr_address = val;
	pr_err("[%s]: %d---ddr at : 0x%x\n", __func__, __LINE__, ddr_address);
	
	return 0;
}

static ssize_t dvp_read(struct file *file, char __user *buf, size_t size,
				loff_t *ppos)
{

	return 0;
}

/*
static u32 dvp_poll(struct file *file, struct poll_table_struct *wait)
{
	return 0;
}
*/

// static int dvp_write_ddr_address(unsigned long arg)
// {
// 	unsigned int tmpval=arg;
// 	unsigned int kbuf;
// 	int ret = 0;
	
// 	pr_err("[%s]: %d---\n", __func__, __LINE__);

// 	ret = copy_from_user(&kbuf, (char __user *)tmpval, 4);
// 	pr_err("[%s]: %d---ret=%d\n", __func__, __LINE__,ret);
// 	if (ret) {
// 		pr_err("[%s]: %d---copy from user failed!\n", __func__, __LINE__);
// 		return ret;
// 	}

// 	ddr_address = kbuf;
// 	pr_err("[%s]: %d--- 0x%x\n", __func__, __LINE__, ddr_address);

// 	return ret;
// }

static long dvp_ioctl(struct file *file, unsigned int cmd,
			unsigned long arg)
{
	int ret = 0;
	struct dvp_dev *dvp = g_dvp;

	switch (cmd) {
	// case DVP_WRITE_DDR_ADDR:
	// 	ret = dvp_write_ddr_address(arg);
	// 	if (ret)
	// 		return ret;
	// 	break;
	case DVP_ENABLE:
		dvp_cfg_register_os02g10(dvp);
		break;
	default:
		break;
	}
	
	return ret;
}

static const struct file_operations dvp_fops = {
	.owner = THIS_MODULE,
	.open = dvp_open,
	.release = dvp_close,
	.write = dvp_write,
	.read = dvp_read,
	//.poll = dvp_poll,
	.unlocked_ioctl = dvp_ioctl,
	.compat_ioctl = dvp_ioctl,
};

unsigned int dvp_ddr_address(void)
{
	return ddr_address;
}

EXPORT_SYMBOL(dvp_ddr_address);


static void swap_data_os02g10(void)
{
	//In order to look at the pciture
#if 1
	unsigned int i, value;
	unsigned int base_addr = (unsigned int)src_addr;
	unsigned int result_addr = (unsigned int)dst_addr;
	unsigned int hvalue, lvalue;

	pr_err("[%s]: %d---enter!\n", __func__, __LINE__);
	
	for(i = 0; i < DVP_WIDTH * 2 * DVP_HIGH; i += 4) {
		value = *(unsigned int *)(base_addr + i);
		lvalue = value & 0xffff;
		hvalue = value & 0xffff0000;
		lvalue <<= 6;
		hvalue <<= 6;
		*(unsigned int *)result_addr = hvalue | lvalue;

		result_addr += 4;
	}
	pr_err("[%s]: %d---swap data ok!!\n", __func__, __LINE__);

	memcpy(tmp_addr, dst_addr, DVP_WIDTH * DVP_HIGH * 2);
	pr_err("[%s]: %d---data memcpy ok!!\n", __func__, __LINE__);
#endif
	return;
}

static void dvp_event_cb(int idx, dvp_event_e event, void *user_data)
{
	pr_err("*****dvp_event_cb*****\n");
	pr_err("[%s]: %d---interrupt handle %d\n", __func__, __LINE__, j);
	j++;

	swap_data_os02g10();

	return;
}

static void dvp_cfg_gctrl(unsigned int cr_cfg)
{
	struct DVP_REG *dvp_reg = (struct DVP_REG *)(g_dvp->hw_res.base_reg);

	dvp_reg->dvp_gctrl.u32 = cr_cfg;
}

static unsigned int dvp_get_gctrl(void)
{
	struct DVP_REG *dvp_reg = (struct DVP_REG *)(g_dvp->hw_res.base_reg);
	
	return (dvp_reg->dvp_ctrl_stat.u32);
}

static void dvp_interrupt_config(unsigned int DVP_IT, FunctionalState NewState)
{
	struct DVP_REG *dvp_reg = (struct DVP_REG *)(g_dvp->hw_res.base_reg);
	
	if (NewState != DISABLE) {
		/* Enable the Interrupt sources */
		//pr_err("[%s]: %d---ier = 0x%x, dvp_it = 0x%x\n", __func__, __LINE__, dvp_reg->dvp_ier.u32, DVP_IT);
		dvp_reg->dvp_ier.u32 |= DVP_IT;
		//pr_err("[%s]: %d---ier = 0x%x, dvp_it = 0x%x\n", __func__, __LINE__, dvp_reg->dvp_ier.u32, DVP_IT);
	} else {
		/* Disable the Interrupt sources */
		dvp_reg->dvp_ier.u32 &= (unsigned int)(~DVP_IT);
	}
}

static void dvp_hvpara_cfg(dvp_hpara Hpara, dvp_vpara Vpara)
{
	struct DVP_REG *dvp_reg = (struct DVP_REG *)(g_dvp->hw_res.base_reg);
	unsigned int temp;

	temp = dvp_reg->dvp_hparm.u32;
	temp &= ~(DVP_HPARA_STRIDE | DVP_HPARA_WIDTH);
	temp |= (Hpara.dvp_strdie << 16) | (Hpara.dvp_width);
	dvp_reg->dvp_hparm.u32 = temp;

	temp = dvp_reg->dvp_vparm.u32;
	temp &= (~DVP_VPARA_HEIGHT);
	temp |= Vpara.dvp_height;
	dvp_reg->dvp_vparm.u32 = temp;
}

static void dvp_addr_cfg(dvp_image_addr addr)
{
	struct DVP_REG *dvp_reg = (struct DVP_REG *)(g_dvp->hw_res.base_reg);

	dvp_reg->dvp_imgaddr1.img_addr1 = addr.dvp_addr1;
	dvp_reg->dvp_imgaddr2.img_addr2 = addr.dvp_addr2;
	dvp_reg->dvp_imgaddr3.img_addr3 = addr.dvp_addr3;
}

static void dvp_clear_flag(unsigned int dvp_flag)
{
	struct DVP_REG *dvp_reg = (struct DVP_REG *)(g_dvp->hw_res.base_reg);

	pr_err("[%s]: %d--- 0x%x\n", __func__, __LINE__, dvp_reg->dvp_icr.u32);
	
	dvp_reg->dvp_icr.u32 = dvp_flag;
	dvp_reg->dvp_icr.u32 = 0;
}

static void dvp_dma_write(FunctionalState NewState)
{
	struct DVP_REG *dvp_reg = (struct DVP_REG *)(g_dvp->hw_res.base_reg);
	
	if (NewState != DISABLE) {
		/* Enable the DVP Capture */
		dvp_reg->dvp_gctrl.u32 |= DVP_CR_DMA_WR_EN;
	} else {
		/* Disable the DVP Capture */
		dvp_reg->dvp_gctrl.u32 &= ~DVP_CR_DMA_WR_EN;
	}
}

static unsigned int dvp_get_flag_status(void)
{
	struct DVP_REG *dvp_reg = (struct DVP_REG *)(g_dvp->hw_res.base_reg);
	unsigned int status;

	status = dvp_reg->dvp_ifr.u32;
	if (status & 0x8)
		pr_err("[%s]---fifo error!", __func__);

	return status;
}

static irqreturn_t dvp_isr(int irq, void *data)
{
	struct dvp_dev *dvp = (struct dvp_dev *)data;
	struct DVP_REG *dvp_reg = (struct DVP_REG *)(g_dvp->hw_res.base_reg);

	pr_err("\n");
	pr_err("[%s]: %d---enter!---count:%d\n", __func__, __LINE__, i);
	
	dvp->irq_status = dvp_get_flag_status();
	pr_err("[%s]---IFR_status=0x%x,ICR_status = 0x%x\r\n", __func__, dvp->irq_status, dvp_reg->dvp_icr.u32);

	if (!(dvp->irq_status & DVP_IFR_FIFO_ERR)) {
		dvp_dma_write(DISABLE);
		dvp->hw_res.cb_event(0, 0, NULL);
		dvp_dma_write(ENABLE);
	}

	dvp_clear_flag(dvp->irq_status);
	i++;
	
	return IRQ_HANDLED;
}

static int dvp_cfg_register_os02g10(struct dvp_dev* dvp)
{
	/* config 0x3c,0x40 register */
	dvp->hpara.dvp_strdie = DVP_WIDTH * 2;
	dvp->hpara.dvp_width = DVP_WIDTH;
	dvp->vpara.dvp_height = DVP_HIGH;
	dvp_hvpara_cfg(dvp->hpara, dvp->vpara);

	/* config 0x08 register */
	dvp->addr.dvp_addr1 = ddr_address;
	dvp_addr_cfg(dvp->addr);

	/* config 0x44 register */
	dvp_interrupt_config(DVP_IER_ADDR1_IE | DVP_IER_FIFO_ERR, ENABLE);

	/* config 0x00 register */
	dvp->gcr.u32 = dvp_get_gctrl();
	dvp->gcr.dvp_capture_en = 1;
	dvp->gcr.dvp_addr_sel1 = 1;
	dvp->gcr.dvp_h_polor = 0;
	dvp->gcr.dvp_v_polor = 1;
	dvp->gcr.dvp_clk_polor = 0;
	dvp->gcr.dvp_dma_wr_en = 1;
	dvp->gcr.dvp_snesor_mode = 2;
	dvp->gcr.dvp_byte_sel = 1;
	dvp_cfg_gctrl(dvp->gcr.u32);
	
	pr_err("[%s]: %d---ok!\n", __func__, __LINE__);
	
	return 0;
}

static int dvp_init(dvp_event_cb_t cb_event, struct dvp_dev* dvp)
{
	dvp->hw_res.cb_event = cb_event;

	dvp_cfg_register_os02g10(dvp);

	pr_err("[%s] %d---ok!\n", __func__, __LINE__);

	return 0;
}

static int dvp_operation_reset(struct device *dev)
{
	struct reset_control *rstc[3];
	int ret = 0;
	int i;

	for (i = 0; i < sizeof(rstc) / sizeof(rstc[0]); i++) {
		rstc[i] = devm_reset_control_get_optional_exclusive(dev, dvp_reset_id[i]);
		if (IS_ERR(rstc[i])) {
			ret = PTR_ERR(rstc[i]);
			pr_err("[%s]: %d---cannot get dvp reset!\n", __func__, __LINE__);
			return -1;
	    }		
		reset_control_reset(rstc[i]);
		pr_err("[%s]: %d---count = %d, dvp_reset_id = %s\n", __func__, __LINE__, i, dvp_reset_id[i]);
	}
	
	pr_err("[%s]: %d---operation reset success! ret = %d\n", __func__, __LINE__, ret);

	return ret;
}

static int dvp_device_node_init(struct dvp_dev *dvp)
{
	int ret = 0;
	
	ret = alloc_chrdev_region(&dvp->devno, 0, MAX_DVP_DEVICE, "dvp");
	if (ret < 0) {
		pr_err("Error %d while alloc chrdev dvp\n", ret);
		goto err_req_cdev;
	}

	cdev_init(&dvp->cdev, &dvp_fops);
	dvp->cdev.owner = THIS_MODULE;
	ret = cdev_add(&dvp->cdev, dvp->devno, MAX_DVP_DEVICE);
	if (ret) {
		pr_err("Error %d while adding dvp cdev\n", ret);
		goto err_req_cdev;
	}

	dvp->class = class_create(THIS_MODULE, "dvp_class");
	if (IS_ERR(dvp->class)) {
		pr_err("dvp class create error\n");
		goto err_cr_cls;
	}

	dvp->dev = device_create(dvp->class, NULL, MKDEV(MAJOR(dvp->devno), 0), NULL, "dvp_dev");
	if (IS_ERR(dvp->dev)) {
		ret = -EINVAL;
		pr_err("dvp_dev_in create fail\n");
		goto err_dev;
	}
	pr_err("[%s]: %d---ok!\n", __func__, __LINE__);
	
	return ret;

err_dev:
	device_destroy(dvp->class, MKDEV(MAJOR(dvp->devno), 0));
	class_destroy(dvp->class);
err_cr_cls:
	cdev_del(&dvp->cdev);
err_req_cdev:
	unregister_chrdev_region(dvp->devno, MAX_DVP_DEVICE);
	
	return ret;
}

static int dvp_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct dvp_dev *dvp;
	struct resource *mem_res;

	pr_err("[%s]: %d---enter---8.24-19:40!\n", __func__, __LINE__);

	dvp = kzalloc(sizeof(struct dvp_dev), GFP_KERNEL);
	if (!dvp) {
		ret = -ENOMEM;
		pr_err("kzmalloc dvp err\n");
		goto p_err;
	}
	g_dvp = dvp;

	tmp_addr = kzalloc(DVP_WIDTH * DVP_HIGH * 2, GFP_KERNEL);
	pr_err("[%s]: %d---0x%p!\n", __func__, __LINE__, tmp_addr);
	
	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem_res) {
		pr_err("Failed to get io memory region(%p)\n", mem_res);
		ret = -EBUSY;
		goto err_get_resource;
	}
	dvp->hw_res.regs_start = mem_res->start;
	dvp->hw_res.regs_end = mem_res->end;
	
	dvp->hw_res.base_reg = devm_ioremap_nocache(&pdev->dev, mem_res->start, resource_size(mem_res));
	if (!dvp->hw_res.base_reg) {
		pr_err("Failed to remap io region(%p)\n", dvp->hw_res.base_reg);
		ret = -ENOMEM;
		goto err_get_resource;
	}

	dvp->hclk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(dvp->hclk))
		return PTR_ERR(dvp->hclk);

	ret = clk_prepare_enable(dvp->hclk);
	if (ret)
		return ret;

	dvp->aclk = devm_clk_get(&pdev->dev, "dvp_aclk");
	if (IS_ERR(dvp->aclk))
		return PTR_ERR(dvp->aclk);

	ret = clk_prepare_enable(dvp->aclk);
	if (ret)
		return ret;

	/* Get IRQ SPI number */
	dvp->hw_res.irq = platform_get_irq(pdev, 0);
	if (dvp->hw_res.irq < 0) {
		pr_err("Failed to get dvp irq(%d)\n", dvp->hw_res.irq);
		ret = -EBUSY;
		goto err_get_irq;
	}
	
	ret = request_irq(dvp->hw_res.irq, dvp_isr, 0, "dvp", dvp);
	if (ret) {
		pr_err("request_irq %d err %d\n", dvp->hw_res.irq, ret);
		goto err_get_irq;
	}
#if 1
	src_addr = devm_ioremap_nocache(&pdev->dev, SRC_ADDR, DVP_WIDTH * DVP_HIGH * 2);
	dst_addr = devm_ioremap_nocache(&pdev->dev, DST_ADDR, DVP_WIDTH * DVP_HIGH * 2);
	pr_err("[%s]: %d---0x%p, 0x%p!\n", __func__, __LINE__, src_addr, dst_addr);
#endif	
	dvp_device_node_init(dvp);

	dvp_operation_reset(&pdev->dev);

	dvp_init(dvp_event_cb, dvp);

	platform_set_drvdata(pdev, dvp);
	
	pr_err("[%s]: %d---probe ok!\n", __func__, __LINE__);

	return ret;
	
err_get_irq:
	iounmap(dvp->hw_res.base_reg);
err_get_resource:
	kfree(dvp);
p_err:
	return ret;
}

static int dvp_remove(struct platform_device *pdev)
{
	int ret = 0;
	struct dvp_dev *dvp;

	BUG_ON(!pdev);
	
	dvp = platform_get_drvdata(pdev);

	//iounmap(src_addr);
	//iounmap(dst_addr);
	free_irq(dvp->hw_res.irq, dvp);
	device_destroy(dvp->class, MKDEV(MAJOR(dvp->devno), 0));
	class_destroy(dvp->class);
	cdev_del(&dvp->cdev);
	unregister_chrdev_region(dvp->devno, MAX_DVP_DEVICE);
	kfree(tmp_addr);
	kfree(dvp);
	
	pr_err("[%s]: %d---remove ok!\n", __func__, __LINE__);
	
	return ret;
}

static const struct of_device_id dvp_of_match[] = {
	{.compatible = "ts,dvp"},
	{/* Sentinel */},
};

static struct platform_driver dvp_driver = {
	.probe = dvp_probe,
	.remove = dvp_remove,
	.driver = {
		.name = MODULE_NAME,
		.owner = THIS_MODULE,
		.of_match_table = dvp_of_match,
	},
};

static int __init dvp_module_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&dvp_driver);
	if (ret)
		pr_err("platform_driver_register failed: %d\n", ret);

	pr_err("[%s]: %d---init ok!!\n", __func__, __LINE__);
	
	return ret;
}

static void __exit dvp_module_exit(void)
{
	platform_driver_unregister(&dvp_driver);

	pr_err("[%s]: %d---exit ok!\n", __func__, __LINE__);
}

module_init(dvp_module_init);
module_exit(dvp_module_exit);

MODULE_DESCRIPTION("TS dvp module driver");
MODULE_LICENSE("GPL v2");
