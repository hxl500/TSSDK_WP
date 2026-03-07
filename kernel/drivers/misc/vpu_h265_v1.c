/*
 * Copyright (C) 2023 TsingMicro.
 * vpu h265 version 1.0 driver.
 */
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/of_device.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/reset.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/gpio.h>
#include <asm/siginfo.h>
#include <linux/pid.h>
#include <linux/sched/signal.h>
#include <linux/pid_namespace.h>
#include <linux/of_reserved_mem.h>
#include <linux/reset.h>
#include <net/sock.h>
#include <linux/netlink.h>


#define H265_NAME			"h265_codec_v1"

#define VPU_MEM_DEVICE_NAME "vpu_mem"
#define ITF_MEM_MAX_SIZE  0x400000  /* 4MB */

#define TOP_MSK_IRQ_BGN						8
#define MASK_INT_KNL_BEGIN			BIT(0)
#define MASK_INT_ITF_BEGIN			BIT(1)

#define TOP_MSK_IRQ_END						9
#define MASK_INT_KNL_END			BIT(0)
#define MASK_INT_ITF_END			BIT(1)

#define TOP_CLR_ERR							10
#define CLEAR_ERR_LCU_TIMEOUT		BIT(0)
#define CLEAR_ERR_FRAME_LOST		BIT(1)

#define TOP_CLR_IRQ_BGN						11
#define CLEAR_INT_ITF_BEGIN			BIT(1)
#define CLEAR_INT_KNL_BEGIN			BIT(0)

#define TOP_CLR_IRQ_END						12
#define CLEAR_INT_ITF_END			BIT(1)
#define CLEAR_INT_KNL_END			BIT(0)

#define TOP_VAL_ERR							20
#define ERR_STATUS_LCU_TIMEOUT		BIT(0)
#define ERR_STATUS_FRAME_LOST		BIT(1)

#define TOP_VAL_IRQ_BGN						21
#define INT_STATUS_ITF_BEGIN		BIT(1)
#define INT_STATUS_KNL_BEGIN		BIT(0)

#define TOP_VAL_IRQ_END						22
#define INT_STATUS_ITF_END			BIT(1)
#define INT_STATUS_KNL_END			BIT(0)

#define ITF_NUM_BS_ENC						128

#define DEV_TYPE			'c'
#define DEV_CMD				_IO(DEV_TYPE, 0)
#define DEV_SET_PID			_IOW(DEV_TYPE, 8, int)
#define DEV_GET_MEM_REGION	_IOR(DEV_TYPE, 9, struct h265_cdc_mem_region)
#define DEV_RESET			_IO(DEV_TYPE, 10)
#define DEV_CLK_ENABLE		_IO(DEV_TYPE, 11)
#define DEV_CLK_DISABLE		_IO(DEV_TYPE, 12)

#define IRQ_DONE_NOTIFY_BY_NETLINK	2
#define IRQ_DONE_NOTIFY_BY_SIGNAL	1
#define IRQ_DONE_NOTIFY_BY_SYSFS	0

#define NETLINK_TEST				30
#define MSG_LEN						125
#define USER_PORT					100

extern struct net init_net;

#define VPU_H265_CLK_NUM			4
#define VPU_CLK_COMMON				0
#define VPU_CLK_PRIVATE				1

struct h265_cdc_mem_region {
	void *reg_addr;
	u32 reg_size;

	void *itf_addr;
	u32 itf_size;
};

struct h265_cdc_clk {
	char name[32];
	u32 type;
	struct clk *curr_clk;
};

struct vpu_h265_st {
	struct platform_device *pdev;
	struct h265_cdc_clk clks[VPU_H265_CLK_NUM];

	struct reset_control *rstc;
	u32 rst_cnt;

	dma_addr_t reg_addr;
	resource_size_t reg_size;
	void __iomem *reg_base;

	dma_addr_t itf_addr;
	resource_size_t itf_size;
	void __iomem *itf_base;

	struct work_struct work;

	struct sock *nlsk;

	int target_pid;
	int irq;
	int notify_type;
	u32 notify_cnt;
	int sig_err;
	bool irq_enabled;
	u32 curr_err;
	u32 curr_int_end;
	u32 err_cnt;
	u32 irq_enter;
	u32 irq_done;
	u32 sig_ok;
	u32 sig_fail;
	u32 irq_re_en;
};

static struct platform_device *misc_parent;

static void h265_gbl_reset(struct vpu_h265_st *h265_st)
{
	reset_control_assert(h265_st->rstc);
	udelay(500);
	reset_control_deassert(h265_st->rstc);
}

static int vpu_h265_clk_enable(struct vpu_h265_st *h265_st, int enable)
{
	int ret;
	int i;
	struct h265_cdc_clk *vpu_clk = NULL;

	for (i = 0; i < VPU_H265_CLK_NUM; i++) {
		vpu_clk = &h265_st->clks[i];
		//pr_info("[clk%d] %s, type %d, en %d\n", i, __clk_get_name(vpu_clk->curr_clk), vpu_clk->type, enable);
		if (vpu_clk->type == VPU_CLK_PRIVATE) {
			if (enable) {
				ret = clk_prepare_enable(vpu_clk->curr_clk);
				//pr_info("clk_prepare_enable(%s), ret %d\n", vpu_clk->name, ret);
				if (ret)
					pr_err("Failed to enable clk '%s': %d\n",
						vpu_clk->name, ret);
			} else {
				clk_disable(vpu_clk->curr_clk);
				//pr_info("clk_disable(%s)\n", vpu_clk->name);
			}
		}
	}

	return 0;
}

static int vpu_misc_open(struct inode *inode, struct file *file)
{
	if (!misc_parent)
		return -1;
	else
		return 0;
}

static long vpu_misc_ioctl(struct file *flip, unsigned int cmd,
	unsigned long arg)
{
	struct vpu_h265_st *h265_st = platform_get_drvdata(misc_parent);
	void __user *argp = (void __user *)arg;
	struct h265_cdc_mem_region mem_region;
	unsigned long ret;
	int err;
	int irq;
	int pid = 0;

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_READ, argp, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_WRITE, argp, _IOC_SIZE(cmd));
	if (err)
		return -EFAULT;

	switch (cmd) {
	case DEV_SET_PID:
		ret = copy_from_user(&pid, argp, _IOC_SIZE(cmd));
		h265_st->target_pid = pid;
		irq = h265_st->irq;
		if (pid == 0) {
			if (h265_st->irq_enabled) {
				disable_irq_nosync(irq);
				h265_st->irq_enabled = false;
			}
		} else {
			if (!h265_st->irq_enabled) {
				enable_irq(irq);
				h265_st->irq_enabled = true;
			}
		}
		break;

	case DEV_GET_MEM_REGION:
		ret = copy_from_user(&mem_region, argp, _IOC_SIZE(cmd));
		mem_region.reg_addr = (void *)h265_st->reg_addr;
		mem_region.reg_size = h265_st->reg_size;
		mem_region.itf_addr = (void *)h265_st->itf_addr;
		mem_region.itf_size = h265_st->itf_size;
		ret = copy_to_user(argp, &mem_region, _IOC_SIZE(cmd));
		break;

	case DEV_RESET:
		h265_gbl_reset(h265_st);
		h265_st->rst_cnt++;
		break;

	case DEV_CLK_ENABLE:
		vpu_h265_clk_enable(h265_st, true);
		break;

	case DEV_CLK_DISABLE:
		vpu_h265_clk_enable(h265_st, false);
		break;

	default:
		pr_err("unsupported cmd 0x%x\n", cmd);
		return -EINVAL;
	}

	return 0;
}

static unsigned int itf_alloc_type = 1;
module_param(itf_alloc_type, uint, 0644);
MODULE_PARM_DESC(itf_alloc_type, "itf memory alloc type: 1 - by platform, 0 - by driver");

static unsigned int notify_en;
module_param(notify_en, uint, 0644);
MODULE_PARM_DESC(notify_en, "notify bs size enable");

const static struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.open = vpu_misc_open,
	.unlocked_ioctl = vpu_misc_ioctl,
};

static struct miscdevice misc = {
#if defined(TS_VPU_MEM_MINOR)
	.minor = TS_VPU_MEM_MINOR,
#else
	.minor = MISC_DYNAMIC_MINOR,
#endif
	.name = VPU_MEM_DEVICE_NAME,
	.fops = &dev_fops,
};

static ssize_t vpu_h265_stat_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct vpu_h265_st *h265_st = platform_get_drvdata(misc_parent);
	int len = 0;

	len += sprintf(buf + len, "reg_addr    : 0x%x\n", h265_st->reg_addr);
	len += sprintf(buf + len, "reg_size    : 0x%x\n", h265_st->reg_size);
	len += sprintf(buf + len, "reg_base    : 0x%p\n", h265_st->reg_base);
	len += sprintf(buf + len, "itf_addr    : 0x%x\n", h265_st->itf_addr);
	len += sprintf(buf + len, "itf_size    : 0x%x\n", h265_st->itf_size);
	len += sprintf(buf + len, "itf_base    : 0x%p\n", h265_st->itf_base);
	len += sprintf(buf + len, "------------------------\n");
	len += sprintf(buf + len, "rst_cnt     : %d\n", h265_st->rst_cnt);
	len += sprintf(buf + len, "target_pid  : %d\n", h265_st->target_pid);
	len += sprintf(buf + len, "irq         : %d\n", h265_st->irq);
	len += sprintf(buf + len, "notify_type : %d\n", h265_st->notify_type);
	len += sprintf(buf + len, "notify_cnt  : %d\n", h265_st->notify_cnt);
	len += sprintf(buf + len, "irq_enabled : %d\n", h265_st->irq_enabled);
	len += sprintf(buf + len, "status_err  : 0x%x\n",
		h265_st->curr_err);
	len += sprintf(buf + len, "int_end     : 0x%x\n",
		h265_st->curr_int_end);
	len += sprintf(buf + len, "irq_enter   : %d\n", h265_st->irq_enter);
	len += sprintf(buf + len, "irq_done    : %d\n", h265_st->irq_done);
	len += sprintf(buf + len, "sig_ok      : %d\n", h265_st->sig_ok);
	len += sprintf(buf + len, "sig_fail    : %d\n", h265_st->sig_fail);
	len += sprintf(buf + len, "sig_err     : %d\n", h265_st->sig_err);
	len += sprintf(buf + len, "irq_re_en   : %d\n", h265_st->irq_re_en);
	len += sprintf(buf + len, "clken added\n");	
	return len;
}

static ssize_t vpu_h265_stat_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct vpu_h265_st *h265_st = platform_get_drvdata(misc_parent);
	unsigned long val;
	int err;

	err = kstrtoul(buf, 0, &val);
	if (err)
		return err;

	if (val == IRQ_DONE_NOTIFY_BY_SYSFS) {
		pr_info("vpu h265 set notify type sysfs\n");
		h265_st->notify_type = IRQ_DONE_NOTIFY_BY_SYSFS;
	} else if (val == IRQ_DONE_NOTIFY_BY_SIGNAL) {
		pr_info("vpu h265 set notify type signal\n");
		h265_st->notify_type = IRQ_DONE_NOTIFY_BY_SIGNAL;
	} else if (val == IRQ_DONE_NOTIFY_BY_NETLINK) {
		pr_info("vpu h265 set notify type netlink\n");
		h265_st->notify_type = IRQ_DONE_NOTIFY_BY_NETLINK;
	} else {
		h265_st->rst_cnt = 0;
		h265_st->curr_err = 0;
		h265_st->curr_int_end = 0;
		h265_st->err_cnt = 0;
		h265_st->irq_enter = 0;
		h265_st->irq_done = 0;
		h265_st->sig_ok = 0;
		h265_st->sig_fail = 0;
		h265_st->sig_err = 0;
		h265_st->irq_re_en = 0;
		pr_info("vpu h265 stat cleared\n");
	}

	return count;
}

static DEVICE_ATTR(vpu_stat, 0644,
	vpu_h265_stat_show, vpu_h265_stat_store);

static ssize_t vpu_h265_notify_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	ssize_t rc = 0;

	rc = sprintf(buf, "this is notify show func\n");
	return rc;
}

static ssize_t vpu_h265_notify_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	return count;
}

static DEVICE_ATTR(vpu_notify, 0644,
	vpu_h265_notify_show, vpu_h265_notify_store);

static int probed;
static int vpu_h265_remap_mem(struct platform_device *pdev)
{
	struct vpu_h265_st *h265_st = platform_get_drvdata(pdev);
	struct resource *res;
	struct device *dev = &pdev->dev;
	int ret;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	h265_st->reg_addr = res->start;
	h265_st->reg_size = resource_size(res);
	h265_st->reg_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(h265_st->reg_base)) {
		dev_err(dev, "remap reg space failed\n");
		return PTR_ERR(h265_st->reg_base);
	}
	dev_info(dev, "reg_addr 0x%x, size 0x%x, reg_base 0x%p\n",
		res->start, h265_st->reg_size, h265_st->reg_base);

	if (itf_alloc_type) {
		dev_info(dev, "itf memory allocated by platform\n");
		return 0;
	}

	dev_info(dev, "itf memory allocated by driver\n");

	dma_set_mask_and_coherent(dev, DMA_BIT_MASK(64));
	ret = of_reserved_mem_device_init(dev);
	if (ret) {
		dev_err(dev, "of_reserved_mem_device_init failed, ret %d.\n",
			ret);
		return ret;
	}

	h265_st->itf_size = ITF_MEM_MAX_SIZE;
	h265_st->itf_base = dma_alloc_coherent(dev,
		h265_st->itf_size, &h265_st->itf_addr, GFP_KERNEL);
	if (!h265_st->itf_base) {
		dev_err(dev, "failed to allocate itf mem\n");
		return -ENOMEM;
	}
	dev_info(dev, "itf_base 0x%p, size 0x%x, phy addr 0x%x.\n",
		h265_st->itf_base, h265_st->itf_size, h265_st->itf_addr);

	return 0;
}

static int vpu_h265_reg_misc_dev(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int ret;

	misc_parent = pdev;
	ret = misc_register(&misc);
	if (ret) {
		dev_err(dev, "failed to register misc dev\n");
		return ret;
	}

	ret = device_create_file(misc.this_device, &dev_attr_vpu_stat);
	if (ret) {
		dev_err(dev, "failed to create attr stat\n");
		return ret;
	}

	ret = device_create_file(misc.this_device, &dev_attr_vpu_notify);
	if (ret) {
		dev_err(dev, "failed to create attr notify\n");
		return ret;
	}

	return 0;
}

static int vpu_h265_init_clk(struct platform_device *pdev)
{
	struct vpu_h265_st *h265_st = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;
	int ret;
	int i;
	struct clk *curr_clk;
	struct h265_cdc_clk *vpu_clk = NULL;

	char *clks[VPU_H265_CLK_NUM] = {"vpu_hclk", "h265_cclk",
		"h265_pclk", "h265_aclk"};
	int types[VPU_H265_CLK_NUM] = {VPU_CLK_COMMON, VPU_CLK_PRIVATE,
		VPU_CLK_PRIVATE, VPU_CLK_PRIVATE};

	for (i = 0; i < VPU_H265_CLK_NUM; i++) {
		curr_clk = clk_get(dev, clks[i]);
		if (IS_ERR(curr_clk)) {
			ret = PTR_ERR(curr_clk);
			dev_err(dev, "Failed to get clk '%s': %d\n",
				clks[i], ret);
			return ret;
		}

		vpu_clk = &h265_st->clks[i];
		strcpy(vpu_clk->name, clks[i]);
		vpu_clk->type = types[i];
		vpu_clk->curr_clk = curr_clk;
		//dev_info(dev, "clk%d %s, type %d\n", i, __clk_get_name(curr_clk), vpu_clk->type);

		if (vpu_clk->type == VPU_CLK_COMMON) {
			ret = clk_prepare_enable(vpu_clk->curr_clk);
			//pr_info("clk_prepare_enable(%s), ret %d\n", vpu_clk->name, ret);
			if (ret)
				pr_err("Failed to enable clk '%s': %d\n",
					vpu_clk->name, ret);
		}
	}

	vpu_h265_clk_enable(h265_st, true);
	//vpu_h265_clk_enable(h265_st, false);
	return 0;
}

static int vpu_h265_init_reset(struct platform_device *pdev)
{
	struct vpu_h265_st *h265_st = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;
	struct reset_control *rstc;

	rstc = devm_reset_control_array_get_optional_exclusive(dev);
	pr_info("rstc %p\n", rstc);
	if (IS_ERR(rstc)) {
		dev_err(dev, "Cannot get h265 reset.\n");
		return PTR_ERR(rstc);
	}

	reset_control_assert(rstc);
	udelay(500);
	reset_control_deassert(rstc);

	h265_st->rstc = rstc;
	return 0;
}

static u32 h265_read_reg(struct vpu_h265_st *h265_st, u32 reg_id)
{
	return readl(h265_st->reg_base + (reg_id << 2));
}

static void h265_write_reg(struct vpu_h265_st *h265_st, u32 reg_id,
	u32 val)
{
	writel(val, h265_st->reg_base + (reg_id << 2));
}

int vpu_h265_netlink_send_msg(struct vpu_h265_st *h265_st,
	char *pbuf, uint16_t len)
{
	struct sk_buff *nl_skb;
	struct nlmsghdr *nlh;
	int ret;

	// create sk_buff space
	nl_skb = nlmsg_new(len, GFP_ATOMIC);
	if (!nl_skb) {
		pr_err("netlink alloc fail\n");
		return -ENOMEM;
	}

	// set netlink msg header
	nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_TEST, len, 0);
	if (nlh == NULL) {
		pr_err("nlmsg_put fail\n");
		nlmsg_free(nl_skb);
		return -EINVAL;
	}

	// copy and send msg
	memcpy(nlmsg_data(nlh), pbuf, len);
	ret = netlink_unicast(h265_st->nlsk, nl_skb, USER_PORT, MSG_DONTWAIT);

	return ret;
}

static void vpu_h265_netlink_rcv_msg(struct sk_buff *skb)
{
	struct nlmsghdr *nlh = NULL;
	char *umsg = NULL;

	if (skb->len >= nlmsg_total_size(0)) {
		nlh = nlmsg_hdr(skb);
		umsg = NLMSG_DATA(nlh);
		if (umsg) {
			pr_info("kernel recv from user: %s\n", umsg);
		}
	}
}

struct netlink_kernel_cfg h265_nl_cfg = {
	.input  = vpu_h265_netlink_rcv_msg, /* set recv callback */
};

int vpu_h265_netlink_init(struct platform_device *pdev)
{
	struct vpu_h265_st *h265_st = platform_get_drvdata(pdev);

	/* create netlink socket */
	struct sock *nlsk = (struct sock *)netlink_kernel_create(&init_net,
		NETLINK_TEST, &h265_nl_cfg);
	if (nlsk == NULL) {
		pr_err("netlink_kernel_create error\n");
		return -1;
	}
	h265_st->nlsk = nlsk;
	return 0;
}

void vpu_h265_netlink_release(struct platform_device *pdev)
{
	struct vpu_h265_st *h265_st = platform_get_drvdata(pdev);
	if (h265_st->nlsk){
		netlink_kernel_release(h265_st->nlsk); /* release ..*/
		h265_st->nlsk = NULL;
	}
}

static int vpu_h265_send_signal(struct vpu_h265_st *h265_st)
{
	int pid = h265_st->target_pid;
	int ret;
	struct siginfo info;
	struct task_struct *my_task = NULL;

	/* application does not set its pid to driver */
	if (pid == 0) {
		pr_err("pid invalid\n");
		return -EINVAL;
	}

	/* make signal struct */
	memset(&info, 0, sizeof(struct siginfo));
	info.si_signo = SIGUSR1;
	info.si_errno = 0;
	info.si_code  = 200;

	/* get task info */
	rcu_read_lock();
	my_task = pid_task(find_vpid(pid), PIDTYPE_PID);
	rcu_read_unlock();

	if (my_task == NULL) {
		pr_err("get pid_task failed, pid %d\n", pid);
		return -ENOENT;
	}

	/* send signal to application */
	ret = send_sig_info(SIGUSR1, &info, my_task);
	if (ret < 0)
		pr_err("send signal to pid %d failed\n", pid);

	return ret;
}

void vpu_h265_workqueue_fn(struct work_struct *work)
{
	struct vpu_h265_st *h265_st =
			container_of(work, struct vpu_h265_st, work);
	u32 numBs;
	char tmp[128] = {0};

	if (notify_en) {
		numBs = h265_read_reg(h265_st, ITF_NUM_BS_ENC);
		pr_info("irq done, numBs 0x%x\n", numBs);
	}

	if (h265_st->notify_type == IRQ_DONE_NOTIFY_BY_SYSFS) {
		sysfs_notify(&misc.this_device->kobj,
			NULL, "vpu_notify");
		h265_st->notify_cnt++;
	} else if (h265_st->notify_type == IRQ_DONE_NOTIFY_BY_NETLINK) {
		sprintf(tmp, "test notify %d", h265_st->notify_cnt);
		vpu_h265_netlink_send_msg(h265_st, tmp, strlen(tmp));
		h265_st->notify_cnt++;
	} else if (h265_st->notify_type == IRQ_DONE_NOTIFY_BY_SIGNAL) {
		h265_st->sig_err = vpu_h265_send_signal(h265_st);
		if (h265_st->sig_err)
			h265_st->sig_fail++;
		else
			h265_st->sig_ok++;

		h265_st->notify_cnt++;
	}
}

static irqreturn_t vpu_h265_irq_handler(int irq, void *dev)
{
	struct vpu_h265_st *h265_st = (struct vpu_h265_st *)dev;
	u32 rdata_begin;
	u32 rdata_end;
	u32 status_end = INT_STATUS_ITF_END | INT_STATUS_KNL_END;
	u32 status_err;

	h265_st->irq_enter++;

	status_err = h265_read_reg(h265_st, TOP_VAL_ERR);
	rdata_begin = h265_read_reg(h265_st, TOP_VAL_IRQ_BGN);
	rdata_end = h265_read_reg(h265_st, TOP_VAL_IRQ_END);

	//clear irq
	if (rdata_begin)
		h265_write_reg(h265_st, TOP_CLR_IRQ_BGN, rdata_begin);

	if (rdata_end)
		h265_write_reg(h265_st, TOP_CLR_IRQ_END, rdata_end);

	if (status_err) {
		h265_write_reg(h265_st, TOP_CLR_ERR, status_err);
		h265_st->curr_err = status_err;
		h265_st->err_cnt++;
		return IRQ_HANDLED;
	}

	h265_st->curr_int_end |= rdata_end;
	if (h265_st->curr_int_end == status_end) {
		h265_st->curr_int_end = 0;
		h265_st->irq_done++;

		schedule_work(&h265_st->work);
	}

	return IRQ_HANDLED;
}

static int vpu_h265_irq_init(struct platform_device *pdev)
{
	struct vpu_h265_st *h265_st = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;
	int ret;

	INIT_WORK(&h265_st->work, vpu_h265_workqueue_fn);

	/* Obtain IRQ line. */
	h265_st->irq = platform_get_irq(pdev, 0);
	if (h265_st->irq < 0) {
		dev_err(dev, "Cannot obtain IRQ.\n");
		return -ENOENT;
	}

	ret = devm_request_irq(dev, h265_st->irq, vpu_h265_irq_handler, 0,
			pdev->name, h265_st);
	if (ret)
		dev_err(dev, "Cannot request IRQ, ret %d.\n", ret);
	else
		h265_st->irq_enabled = true;

	return ret;
}

static int vpu_h265_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct vpu_h265_st *h265_st;
	int ret;

	if (probed) {
		dev_info(dev, "vpu h265 already probed\n");
		return 0;
	}

	h265_st = devm_kzalloc(dev, sizeof(*h265_st), GFP_KERNEL);
	if (!h265_st)
		return -ENOMEM;

	h265_st->pdev = pdev;
	platform_set_drvdata(pdev, h265_st);

	/* Obtain and remap controller address. */
	ret = vpu_h265_remap_mem(pdev);
	if (ret) {
		dev_err(dev, "failed to remap mem\n");
		goto PROBE_FAILED;
	}

	ret = vpu_h265_reg_misc_dev(pdev);
	if (ret) {
		dev_err(dev, "failed to register misc dev\n");
		goto PROBE_FAILED;
	}

	ret = vpu_h265_init_clk(pdev);
	if (ret) {
		dev_err(dev, "failed to init clock\n");
		goto PROBE_FAILED;
	}

	ret = vpu_h265_irq_init(pdev);
	if (ret) {
		dev_err(dev, "failed to init irq\n");
		goto PROBE_FAILED;
	}

	ret = vpu_h265_init_reset(pdev);
	if (ret) {
		dev_err(dev, "failed to reset\n");
		goto PROBE_FAILED;
	}

	ret = vpu_h265_netlink_init(pdev);
	if (ret) {
		dev_err(dev, "failed to init netlink\n");
		goto PROBE_FAILED;
	}

	probed = true;
	return 0;

PROBE_FAILED:
	return ret;
}

static int vpu_h265_remove(struct platform_device *pdev)
{
	vpu_h265_netlink_release(pdev);

	device_remove_file(misc.this_device, &dev_attr_vpu_stat);
	device_remove_file(misc.this_device, &dev_attr_vpu_notify);
	misc_deregister(&misc);

	return 0;
}

static const struct of_device_id h265_dt_ids[] = {
	{
		.compatible = "ts,h265",
		.data = (void *)0,
	},
	{ /* end of table */ }
};

MODULE_DEVICE_TABLE(of, h265_dt_ids);

static struct platform_driver h265_platform_driver = {
	.probe = vpu_h265_probe,
	.remove = vpu_h265_remove,
	.driver = {
		.name = H265_NAME,
		.pm = NULL,
		.of_match_table = h265_dt_ids,
	},
};

module_platform_driver(h265_platform_driver);

MODULE_DESCRIPTION("VPU H265 Controller Driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("TsingMicro");
