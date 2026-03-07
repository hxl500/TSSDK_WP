/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2023 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/
#define pr_fmt(fmt)	"sharemsg_drv: " fmt

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/time.h>
#include <linux/timekeeping.h>
#include <linux/sched/signal.h>
#include <linux/miscdevice.h>
#include <linux/math64.h>
#include <linux/cache.h>
#include <uapi/linux/sched/types.h>

#include "sharemsg.h"
#include "sharemsg_uapi.h"

// #define SHAREMSG_MEM_DEBUG
// #define SHAREMSG_DT_DEBUG

#ifdef SHAREMSG_MEM_DEBUG
	#define VIDEO_DUMP_LEN (32)
#endif

#define MODULE_NAME "sharemsg"
#define SHAREMSG_MEM_DEVICE_NAME "sharemsg"


static struct sharemsg_mem_dev sharemsg_mem_dev;

static int sharemsg_free_mem(struct sharemsg_mem_dev *sharemsg_mem_dev)
{
	void *start, *end;

	if (sharemsg_mem_dev->phy_addr) {
		start = phys_to_virt(sharemsg_mem_dev->phy_addr);
		end = start + sharemsg_mem_dev->mem_len;
		free_reserved_area(start, end, -1, "sharemsg mem");
		sharemsg_mem_dev->phy_addr = 0;
	}

	return 0;
}

static int sharemsg_get_status(struct sharemsg_mem_dev *sharemsg_mem_dev,
												unsigned long arg)
{
	int ret;
	struct sharemsg_status sharemsg_status;

#ifdef SHAREMSG_MEM_DEBUG
	int i;
	venc_para_t *venc_para;
	rne_alg_para_t *alg_para;
	bare_paras_t *bare_paras;
#endif

	memset(&sharemsg_status, 0, sizeof(struct sharemsg_status));

	sharemsg_status.mem_len = sharemsg_mem_dev->mem_len;
	sharemsg_status.phy_addr = sharemsg_mem_dev->phy_addr;

#ifdef SHAREMSG_MEM_DEBUG
	bare_paras = (bare_paras_t *)phys_to_virt(sharemsg_mem_dev->phy_addr);
	for (i = 0; i < sharemsg_mem_dev->fbs_num; i++) {
		venc_para = &bare_paras->venc_paras[i];
		venc_para->fbs_base = (char *)(sharemsg_mem_dev->phy_addr +
				sizeof(bare_paras_t) + i * sharemsg_mem_dev->fbs_maxlen);
		venc_para->fbs_len = sharemsg_mem_dev->fbs_maxlen;
		venc_para->fbs_eof = true;
	}
	for (i = 0; i < sharemsg_mem_dev->det_chnnum; i++) {
		alg_para = &bare_paras->alg_paras[i];
		alg_para->curr_perdestrain_det = true;
	}

	pr_alert("sharemsg_status.mem_len:0x%x\n", sharemsg_status.mem_len);
	pr_alert("sharemsg_status.phy_addr:0x%x\n", sharemsg_status.phy_addr);

	pr_alert("=================================\n");
	pr_alert("setup param values:\n");
	pr_alert("=================================\n");
	for (i = 0; i < sharemsg_mem_dev->fbs_num; i++) {
		pr_alert("venc_para[%d]->fbs_base:0x%x\n", i,
			sharemsg_mem_dev->phy_addr + sizeof(bare_paras_t) + i * sharemsg_mem_dev->fbs_maxlen);
		pr_alert("venc_para[%d]->fbs_len:0x%x\n", i, sharemsg_mem_dev->fbs_maxlen);
		pr_alert("venc_para[%d]->fbs_eof:0x%x\n", i, true);
	}
	for (i = 0; i < sharemsg_mem_dev->det_chnnum; i++) {
		pr_alert("alg_para[%d]->curr_perdestrain_det:0x%x\n", i, true);
	}

	pr_alert("=================================\n");
	pr_alert("dump param values:\n");
	pr_alert("=================================\n");
	for (i = 0; i < sharemsg_mem_dev->fbs_num; i++) {
		venc_para = &bare_paras->venc_paras[i];
		pr_alert("venc_para[%d]->fbs_base:0x%p\n", i, venc_para->fbs_base);
		pr_alert("venc_para[%d]->fbs_len:0x%x\n", i, venc_para->fbs_len);
		pr_alert("venc_para[%d]->fbs_eof:0x%x\n", i, venc_para->fbs_eof);
	}
	for (i = 0; i < sharemsg_mem_dev->det_chnnum; i++) {
		alg_para = &bare_paras->alg_paras[i];
		pr_alert("alg_para[%d]->curr_perdestrain_det:0x%x\n", i, alg_para->curr_perdestrain_det);
	}
	pr_alert("=================================\n");
#endif
	ret = copy_to_user((void __user *) arg, (char *) &sharemsg_status,
			 sizeof(struct sharemsg_status));
	if (ret) {
		pr_err("%s %d copy_to_user error\n", __func__, __LINE__);
		return -EFAULT;
	}

	return 0;

}

static long sharemsg_mem_ioctl(struct file *file, unsigned int cmd,
			  unsigned long arg)
{
	int ret = 0;

	switch (cmd) {
	case SHAREMSG_IOC_GET_STATUS:
		ret = sharemsg_get_status(&sharemsg_mem_dev, arg);
		break;
	case SHAREMSG_IOC_FREE_MEM:
		ret = sharemsg_free_mem(&sharemsg_mem_dev);
		break;
	default:
		pr_info("%s %d unknow cmd %x\n", __func__, __LINE__, cmd);
		break;
	}

	return ret;
}

static ssize_t sharemsg_mem_read(struct file *file, char __user *buf, size_t size,
				loff_t *ppos)
{
	return 0;
}

#ifdef SHAREMSG_MEM_DEBUG
void sharemsg_dump(char *str, char *data, uint32_t len)
{
    unsigned int i;
    unsigned int byte_per_line = BYTE_PER_LINE;

    pr_cont("%s:\n", str);
    for (i = 0; i < len; ++i) {
        if (i % byte_per_line == 0)
            pr_cont("%x: ", i);
        pr_cont("%x", *(data + i));
        if (i % byte_per_line == byte_per_line - 1)
            pr_info("\n");
        else
            pr_cont(" ");
    }
}
#endif

static int sharemsg_mem_mmap(struct file *filp, struct vm_area_struct *vma)
{
	unsigned long start = vma->vm_start;
	unsigned long size = vma->vm_end - vma->vm_start;
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long page, pos;

#ifdef SHAREMSG_MEM_DEBUG
	int i = 0;
	int j = 0;
	char *p = NULL;
	char *p_tmp = NULL;
	uint32_t off = 0;
	venc_para_t *venc_para;
	bare_paras_t *bare_paras;
#endif

	printk( "start=0x%08x offset=0x%08x\n", (unsigned int)start, (unsigned int)offset);

	pos = (unsigned long)sharemsg_mem_dev.phy_addr + offset;
	page = pos >> PAGE_SHIFT ;
	if(remap_pfn_range(vma, start, page, size, PAGE_SHARED)) {
		return -EAGAIN;
	} else {
		printk("remap_pfn_range %u\n success\n", (unsigned int)page);
	}
#ifdef SHAREMSG_MEM_DEBUG
	bare_paras = (bare_paras_t *)phys_to_virt(sharemsg_mem_dev.phy_addr);
	for (i = 0; i < sharemsg_mem_dev.fbs_num; i++) {
		venc_para = &bare_paras->venc_paras[i];
		off = (uint32_t)venc_para->fbs_base - sharemsg_mem_dev.phy_addr;
		p = (char *)bare_paras + off;
		for(j = 0; j < sharemsg_mem_dev.fbs_maxlen; j++) {
			*p++ = i + j;
		}
		pr_info("=============fbs_num[%d]==============\n", i);
		p_tmp = p - sharemsg_mem_dev.fbs_maxlen;
		sharemsg_dump("test fbs head", p_tmp, VIDEO_DUMP_LEN);
		p_tmp = p - VIDEO_DUMP_LEN;
		sharemsg_dump("test fbs tail", p_tmp, VIDEO_DUMP_LEN);
		pr_info("======================================\n");
	}
#endif

	vma->vm_flags &= ~VM_IO;
	vma->vm_flags |=  (VM_DONTEXPAND | VM_DONTDUMP);

	return 0;
}

static int sharemsg_mem_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int sharemsg_mem_close(struct inode *inode, struct file *file)
{
	return 0;
}


const static struct file_operations sharemsg_dev_fops = {
	.owner = THIS_MODULE,
	.open = sharemsg_mem_open,
	.release = sharemsg_mem_close,
	.read = sharemsg_mem_read,
	.mmap = sharemsg_mem_mmap,
	.unlocked_ioctl = sharemsg_mem_ioctl,
};

static struct miscdevice sharemsg_misc = {
#if defined(TS_SHAREMSG_MINOR)
	.minor = TS_SHAREMSG_MINOR,
#else
	.minor = MISC_DYNAMIC_MINOR,
#endif
	.name = SHAREMSG_MEM_DEVICE_NAME,
	.fops = &sharemsg_dev_fops,
};

static int ts_sharemsg_parse_dt(struct platform_device *pdev, struct sharemsg_mem_dev *sharemsg_mem_dev)
{
	int ret = 0;
	struct resource reg;
	struct device_node	*np = pdev->dev.of_node;
	struct device_node	*mem_node;

	ret = of_property_read_u32(np, "fbs,num", &sharemsg_mem_dev->fbs_num);
	if (ret) {
		pr_err("read fbs,num error\n");
		return -ENODEV;
	}
	ret = of_property_read_u32(np, "fbs,maxlen", &sharemsg_mem_dev->fbs_maxlen);
	if (ret) {
		pr_err("read fbs,len error\n");
		return -ENODEV;
	}
	ret = of_property_read_u32(np, "detect,chn-num", &sharemsg_mem_dev->det_chnnum);
	if (ret) {
		pr_err("read detect,chn-num error\n");
		return -ENODEV;
	}
	mem_node = of_parse_phandle(pdev->dev.of_node, "memory-region", 0);
	if (!mem_node) {
		pr_err("missing \"memory-region\" property\n");
		return -ENODEV;
	}
	ret = of_address_to_resource(mem_node, 0, &reg);
	if (ret) {
		pr_err("missing \"reg\" property\n");
		ret = -ENODEV;
		goto free_mem_node;
	}

	sharemsg_mem_dev->phy_addr = reg.start;
	sharemsg_mem_dev->mem_len = resource_size(&reg);

#ifdef SHAREMSG_DT_DEBUG
	pr_alert("sharemsg_mem_dev->fbs_num:0x%x\n", sharemsg_mem_dev->fbs_num);
	pr_alert("sharemsg_mem_dev->fbs_maxlen:0x%x\n", sharemsg_mem_dev->fbs_maxlen);
	pr_alert("sharemsg_mem_dev->det_chnnum:0x%x\n", sharemsg_mem_dev->det_chnnum);
	pr_alert("0x%zx@%pa \n", sharemsg_mem_dev->mem_len, &sharemsg_mem_dev->phy_addr);
#endif

free_mem_node:
	of_node_put(mem_node);

	return ret;
}

static int sharemsg_mem_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct sharemsg_info sharemsg_info;

	pr_info("%s\n", __func__);

	memcpy(&sharemsg_info, (void *)of_device_get_match_data(&pdev->dev),
			sizeof(struct sharemsg_info));
	sharemsg_mem_dev.verson = sharemsg_info.version;
#ifdef SHAREMSG_MEM_DEBUG
	pr_alert("sharemsg_mem_dev.verson:%d\n", sharemsg_mem_dev.verson);
#endif

	ret = ts_sharemsg_parse_dt(pdev, &sharemsg_mem_dev);
	if (ret)
		return ret;
	ret = misc_register(&sharemsg_misc);
	if (ret) {
		pr_err("fail to register sharemsg device\n");
		return -ENODEV;
	}
	pr_info("%s success.\n", __func__);

	return 0;
}


static int sharemsg_mem_remove(struct platform_device *pdev)
{
	misc_deregister(&sharemsg_misc);

	return 0;
}

struct sharemsg_info sharemsg_res = {
	.version = 1,
};


static const struct of_device_id sharemsg_dt_ids[] = {
	{
		.compatible = "ts,sharemsg",
		.data = &sharemsg_res,
	},
	{ /* end of table */ }
};

MODULE_DEVICE_TABLE(of, sharemsg_dt_ids);

static struct platform_driver sharemsg_platform_driver = {
	.probe = sharemsg_mem_probe,
	.remove = sharemsg_mem_remove,
	.driver = {
		.name = MODULE_NAME,
		.pm = NULL,
		.of_match_table = sharemsg_dt_ids,
	},
};


module_platform_driver(sharemsg_platform_driver);


MODULE_DESCRIPTION("Tsingmicro sharemsg module driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("TsingMicro");
