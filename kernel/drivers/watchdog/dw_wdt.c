/*
 * Copyright 2010-2011 Picochip Ltd., Jamie Iles
 * http://www.picochip.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * This file implements a driver for the Synopsys DesignWare watchdog device
 * in the many subsystems. The watchdog has 16 different timeout periods
 * and these are a function of the input clock frequency.
 *
 * The DesignWare watchdog cannot be stopped once it has been started so we
 * do not implement a stop function. The watchdog core will continue to send
 * heartbeat requests after the watchdog device has been closed.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/of.h>
#include <linux/pm.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/watchdog.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/uaccess.h>

#define WDOG_CONTROL_REG_OFFSET				0x00
#define WDOG_CONTROL_REG_WDT_EN_MASK		0x01
#define WDOG_CONTROL_REG_RESP_MODE_MASK		0x02
#define WDOG_CONTROL_REG_RESP_PULSE_LENGTH_MASK		0x07
#define WDOG_CONTROL_REG_RESP_PULSE_LENGTH_POS		(2)
#define WDOG_CONTROL_REG_RESET_MODE_MASK		0x1
#define WDOG_CONTROL_REG_RESET_MODE_POS			(1)


#define WDOG_TIMEOUT_RANGE_REG_OFFSET		0x04
#define WDOG_TIMEOUT_RANGE_TOPINIT_SHIFT	4

#define WDOG_CURRENT_COUNT_REG_OFFSET		0x08
#define WDOG_COUNTER_RESTART_REG_OFFSET		0x0c
#define WDOG_COUNTER_RESTART_KICK_VALUE		0x76

#define WDOG_CONTROL_REG_CLEAR_INT			0x14

typedef enum {
	WDT_ResetPulseLength_2_PCLK_CYCLES = 0,
	WDT_ResetPulseLength_4_PCLK_CYCLES,
	WDT_ResetPulseLength_8_PCLK_CYCLES,
	WDT_ResetPulseLength_16_PCLK_CYCLES,
	WDT_ResetPulseLength_32_PCLK_CYCLES,
	WDT_ResetPulseLength_64_PCLK_CYCLES,
	WDT_ResetPulseLength_128_PCLK_CYCLES,
	WDT_ResetPulseLength_256_PCLK_CYCLES,
} eWDT_ResetPulseLength_t;

typedef enum {
	WDT_SYSTEM_RESET = 0,
	WDT_INTERRUPT,
} eWDT_ResponseMode_t;

static unsigned int g_CpuWdtUnfoodCnt = 0;

/* The maximum TOP (timeout period) value that can be set in the watchdog. */
#define DW_WDT_MAX_TOP		15

#define DW_WDT_DEFAULT_SECONDS	5
#define DW_WDT_DEFAULT_RESET_PULSE_LENGTH	WDT_ResetPulseLength_64_PCLK_CYCLES
#define DW_WDT_DEFAULT_RESET_MODE	WDT_INTERRUPT

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started "
		 "(default=" __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

struct dw_wdt {
	void __iomem		*regs;
	struct clk		*pclk;
	struct clk		*clk;
	unsigned long		rate;
	struct watchdog_device	wdd;
	struct reset_control *rst_apb;
	struct reset_control *rst_tick;
	struct reset_control *rst_glb;
	unsigned int uboot_work;
	unsigned int reset_mode;
};

#define to_dw_wdt(wdd)	container_of(wdd, struct dw_wdt, wdd)

#define	 WATCHDOG_IOCTL_BASE	'W'

#define WATCHDOG_ENABLE_MULTICORE	    _IOR(WATCHDOG_IOCTL_BASE, 11, int)
#define WATCHDOG_DISABLE_MULTICORE	    _IOR(WATCHDOG_IOCTL_BASE, 12, int)

struct watchdog_thread {
	struct task_struct *thread;
	atomic_t count;
	int cpu;
	unsigned int timeout;
};

static struct watchdog_thread per_cpu_threads[2];

static bool thread_flag = false;

static bool first_flag = true;

static int watchdog_thread_fn(void *data)
{
    struct watchdog_thread *wdt = (struct watchdog_thread *)data;

    while (!kthread_should_stop()) {
        atomic_inc(&wdt->count);
		//pr_info("timeout = %d, cpu:%d, count = %d\n", wdt->timeout, wdt->cpu, atomic_read(&wdt->count));
        msleep(wdt->timeout * 1000 / 10); //睡眠timeout的十分之一时间
    }

    return 0;
}

static void create_watchdog_threads(struct watchdog_device *wdd)
{
    int cpu;

	thread_flag = true;

    for_each_online_cpu(cpu) {
		struct watchdog_thread *wdt = &per_cpu_threads[cpu];
		//pr_info("[%s], %d, cpu = %d\n", __func__, __LINE__, cpu);
		wdt->cpu = cpu;
		wdt->timeout = wdd->timeout;
        atomic_set(&wdt->count, 0);
        wdt->thread = kthread_create(watchdog_thread_fn, wdt, "wdt_thread/%d", cpu);
        if (!IS_ERR(wdt->thread)) {
            kthread_bind(wdt->thread, cpu);
            wake_up_process(wdt->thread);
        }
    }
}

static void stop_watchdog_threads(void)
{
    int cpu;

    for_each_online_cpu(cpu) {
        struct watchdog_thread *wdt = &per_cpu_threads[cpu];
		//pr_info("[%s], %d, cpu = %d\n", __func__, __LINE__, cpu);
        if (wdt->thread) {
            kthread_stop(wdt->thread);
            wdt->thread = NULL;
        }
    }
}

static void stop_watchdog_thread(int cpu)
{
    struct watchdog_thread *wdt = &per_cpu_threads[cpu];
	//pr_info("[%s], %d, cpu = %d\n", __func__, __LINE__, cpu);
    if (wdt->thread) {
        kthread_stop(wdt->thread);
        wdt->thread = NULL;
    }

}

static int check_watchdog_counts(void)
{
    int cpu;
    static atomic_t last_counts[2];

	if (first_flag == true) {
		for_each_online_cpu(cpu) {
			struct watchdog_thread *wdt = &per_cpu_threads[cpu];
	        int current_count = atomic_read(&wdt->count);
			//pr_info("###current_count = %d, last_counts[%d] = %d\n", current_count, cpu, atomic_read(&last_counts[cpu]));
	        atomic_set(&last_counts[cpu], current_count);
		}
		first_flag = false;
	} else {
		for_each_online_cpu(cpu) {
	        struct watchdog_thread *wdt = &per_cpu_threads[cpu];
	        int current_count = atomic_read(&wdt->count);
			//pr_info("***current_count = %d, last_counts[%d] = %d\n", current_count, cpu, atomic_read(&last_counts[cpu]));
	        if (current_count == atomic_read(&last_counts[cpu])) {
	            // Count did not increase, trigger watchdog
	            return -1;
	        }
	        atomic_set(&last_counts[cpu], current_count);
		}
    }

    return 0;
}

static long dw_wdt_ioctl(struct watchdog_device *wdd, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	unsigned int cpu = 0;

	switch (cmd) {
	case WATCHDOG_ENABLE_MULTICORE:
		create_watchdog_threads(wdd);
		break;
	case WATCHDOG_DISABLE_MULTICORE:
		if (copy_from_user(&cpu, (void __user *)arg, sizeof(cpu))) {
			pr_info("copy from user failed\n");
			return -EFAULT;
		}
		stop_watchdog_thread(cpu);
		break;
	default:
		ret = -ENOIOCTLCMD;
		break;
	}

	return ret;
}

static inline int dw_wdt_is_enabled(struct dw_wdt *dw_wdt)
{
	return readl(dw_wdt->regs + WDOG_CONTROL_REG_OFFSET) &
		WDOG_CONTROL_REG_WDT_EN_MASK;
}

static void dw_wdt_set_reset_pulse_length(struct watchdog_device *wdd, eWDT_ResetPulseLength_t value)
{
	struct dw_wdt *dw_wdt = to_dw_wdt(wdd);

	u32 val = readl(dw_wdt->regs + WDOG_CONTROL_REG_OFFSET);

	val &= ~(WDOG_CONTROL_REG_RESP_PULSE_LENGTH_MASK << WDOG_CONTROL_REG_RESP_PULSE_LENGTH_POS);
	val |= (value << WDOG_CONTROL_REG_RESP_PULSE_LENGTH_POS);
	writel(val, dw_wdt->regs + WDOG_CONTROL_REG_OFFSET);

}

static int dw_wdt_hardware_init(struct watchdog_device *wdd)
{
	dw_wdt_set_reset_pulse_length(wdd, DW_WDT_DEFAULT_RESET_PULSE_LENGTH);
	return 0;
}

static inline int dw_wdt_top_in_seconds(struct dw_wdt *dw_wdt, unsigned top)
{
	/*
	 * There are 16 possible timeout values in 0..15 where the number of
	 * cycles is 2 ^ (16 + i) and the watchdog counts down.
	 */
	return (1U << (16 + top)) / dw_wdt->rate;
}

static int dw_wdt_get_top(struct dw_wdt *dw_wdt)
{
	unsigned int top = readl(dw_wdt->regs + WDOG_TIMEOUT_RANGE_REG_OFFSET) & 0xF;

	return dw_wdt_top_in_seconds(dw_wdt, top);
}

static int dw_wdt_ping(struct watchdog_device *wdd)
{
	struct dw_wdt *dw_wdt = to_dw_wdt(wdd);

	if (thread_flag == true) {
		//pr_info("[%s], %d\n", __func__, __LINE__);
		if (check_watchdog_counts() < 0) {
	        return 0;
	    }
	}

	writel(WDOG_COUNTER_RESTART_KICK_VALUE, dw_wdt->regs +
		   WDOG_COUNTER_RESTART_REG_OFFSET);

	g_CpuWdtUnfoodCnt = 0;

	return 0;
}

static int dw_wdt_set_timeout(struct watchdog_device *wdd, unsigned int top_s)
{
	struct dw_wdt *dw_wdt = to_dw_wdt(wdd);
	int i, top_val = DW_WDT_MAX_TOP;

	/*
	 * Iterate over the timeout values until we find the closest match. We
	 * always look for >=.
	 */
	for (i = 0; i <= DW_WDT_MAX_TOP; ++i)
		if (dw_wdt_top_in_seconds(dw_wdt, i) >= top_s) {
			top_val = i;
			break;
		}

	/*
	 * Set the new value in the watchdog.  Some versions of dw_wdt
	 * have have TOPINIT in the TIMEOUT_RANGE register (as per
	 * CP_WDT_DUAL_TOP in WDT_COMP_PARAMS_1).  On those we
	 * effectively get a pat of the watchdog right here.
	 */
	writel(top_val | top_val << WDOG_TIMEOUT_RANGE_TOPINIT_SHIFT,
		   dw_wdt->regs + WDOG_TIMEOUT_RANGE_REG_OFFSET);

	wdd->timeout = dw_wdt_top_in_seconds(dw_wdt, top_val);

	return 0;
}

static void dw_wdt_set_reset_mode(struct dw_wdt *dw_wdt,
							eWDT_ResponseMode_t value)
{
	u32 val = readl(dw_wdt->regs + WDOG_CONTROL_REG_OFFSET);

	val &= ~(WDOG_CONTROL_REG_RESET_MODE_MASK << WDOG_CONTROL_REG_RESET_MODE_POS);
	val |= (value << WDOG_CONTROL_REG_RESET_MODE_POS);
	writel(val, dw_wdt->regs + WDOG_CONTROL_REG_OFFSET);
}

static void dw_wdt_arm_system_reset(struct dw_wdt *dw_wdt)
{
	u32 val;

	if (dw_wdt->reset_mode == WDT_INTERRUPT) {
		dw_wdt_set_reset_mode(dw_wdt, WDT_INTERRUPT);
	} else {
		dw_wdt_set_reset_mode(dw_wdt, WDT_SYSTEM_RESET);
	}

 	val = readl(dw_wdt->regs + WDOG_CONTROL_REG_OFFSET);
	/* Enable watchdog. */
	val |= WDOG_CONTROL_REG_WDT_EN_MASK;
	writel(val, dw_wdt->regs + WDOG_CONTROL_REG_OFFSET);
}

static int dw_wdt_start(struct watchdog_device *wdd)
{
	struct dw_wdt *dw_wdt = to_dw_wdt(wdd);

	dw_wdt_set_timeout(wdd, wdd->timeout);
	dw_wdt_arm_system_reset(dw_wdt);

	return 0;
}

static void dw_wdt_reset_control_assert_and_deassert(struct dw_wdt *dw_wdt)
{
	if (!IS_ERR(dw_wdt->rst_tick)) {
		reset_control_assert(dw_wdt->rst_tick);
	}
	if (!IS_ERR(dw_wdt->rst_apb)) {
		reset_control_assert(dw_wdt->rst_apb);
	}
	if (!IS_ERR(dw_wdt->rst_glb)) {
		reset_control_assert(dw_wdt->rst_glb);
	}
	ndelay(100);
	if (!IS_ERR(dw_wdt->rst_glb)) {
		reset_control_deassert(dw_wdt->rst_glb);
	}
	if (!IS_ERR(dw_wdt->rst_apb)) {
		reset_control_deassert(dw_wdt->rst_apb);
	}
	if (!IS_ERR(dw_wdt->rst_tick)) {
		reset_control_deassert(dw_wdt->rst_tick);
	}
}

static int dw_wdt_stop(struct watchdog_device *wdd)
{
	struct dw_wdt *dw_wdt = to_dw_wdt(wdd);

	//pr_info("[%s], %d\n", __func__, __LINE__);

	if (thread_flag == true) {
		stop_watchdog_threads();
		thread_flag = false;
	}

	if (!dw_wdt->rst_apb) {
		set_bit(WDOG_HW_RUNNING, &wdd->status);
		return 0;
	}

	dw_wdt_reset_control_assert_and_deassert(dw_wdt);
	dw_wdt_hardware_init(wdd);

	return 0;
}

static irqreturn_t dw_wdt_handler(int irq, void *dev_id)
{
	struct dw_wdt *dw_wdt = dev_id;

	if ( g_CpuWdtUnfoodCnt == 0) {
		readl(dw_wdt->regs + WDOG_CONTROL_REG_CLEAR_INT);
		pr_err("%s **** %d ***** g_CpuWdtIntCnt = %d\n",
						__func__, __LINE__, g_CpuWdtUnfoodCnt);
	} else {
		//pr_err("%s **** %d ***** g_CpuWdtIntCnt = %d\n",
			//		__func__, __LINE__, g_CpuWdtUnfoodCnt);
	}
	g_CpuWdtUnfoodCnt++;

	return IRQ_HANDLED;
}

static int dw_wdt_restart(struct watchdog_device *wdd,
			  unsigned long action, void *data)
{
	struct dw_wdt *dw_wdt = to_dw_wdt(wdd);

	writel(0, dw_wdt->regs + WDOG_TIMEOUT_RANGE_REG_OFFSET);
	if (dw_wdt_is_enabled(dw_wdt))
		writel(WDOG_COUNTER_RESTART_KICK_VALUE,
			   dw_wdt->regs + WDOG_COUNTER_RESTART_REG_OFFSET);
	else
		dw_wdt_arm_system_reset(dw_wdt);

	/* wait for reset to assert... */
	mdelay(500);

	return 0;
}

static unsigned int dw_wdt_get_timeleft(struct watchdog_device *wdd)
{
	struct dw_wdt *dw_wdt = to_dw_wdt(wdd);

	return readl(dw_wdt->regs + WDOG_CURRENT_COUNT_REG_OFFSET) /
		dw_wdt->rate;
}

static const struct watchdog_info dw_wdt_ident = {
	.options	= WDIOF_KEEPALIVEPING | WDIOF_SETTIMEOUT |
			  WDIOF_MAGICCLOSE,
	.identity	= "Synopsys DesignWare Watchdog",
};

static const struct watchdog_ops dw_wdt_ops = {
	.owner		= THIS_MODULE,
	.start		= dw_wdt_start,
	.stop		= dw_wdt_stop,
	.ping		= dw_wdt_ping,
	.set_timeout	= dw_wdt_set_timeout,
	.get_timeleft	= dw_wdt_get_timeleft,
	.restart	= dw_wdt_restart,
	.ioctl      = dw_wdt_ioctl,
};

#ifdef CONFIG_PM_SLEEP
static int dw_wdt_suspend(struct device *dev)
{
	struct dw_wdt *dw_wdt = dev_get_drvdata(dev);

	clk_disable_unprepare(dw_wdt->clk);

	return 0;
}

static int dw_wdt_resume(struct device *dev)
{
	struct dw_wdt *dw_wdt = dev_get_drvdata(dev);
	int err = clk_prepare_enable(dw_wdt->clk);

	if (err)
		return err;

	dw_wdt_ping(&dw_wdt->wdd);

	return 0;
}
#endif /* CONFIG_PM_SLEEP */

static SIMPLE_DEV_PM_OPS(dw_wdt_pm_ops, dw_wdt_suspend, dw_wdt_resume);

static ssize_t wdt_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct dw_wdt *dw_wdt;
	u32 val;

	dw_wdt = (struct dw_wdt *)dev->driver_data;

	val = readl(dw_wdt->regs + WDOG_CONTROL_REG_OFFSET);
	dev_info(dev, "[0x00]: 0x%x\n", val);

	val = readl(dw_wdt->regs + WDOG_TIMEOUT_RANGE_REG_OFFSET);
	dev_info(dev, "[0x04]: 0x%x\n", val);

	val = readl(dw_wdt->regs + WDOG_CURRENT_COUNT_REG_OFFSET);
	dev_info(dev, "[0x08]: 0x%x\n", val);

	val = readl(dw_wdt->regs + WDOG_COUNTER_RESTART_REG_OFFSET);
	dev_info(dev, "[0x0C]: 0x%x\n", val);

	return 0;
}
static ssize_t wdt_set(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	struct dw_wdt *dw_wdt;
	struct watchdog_device *wdd;

	dw_wdt = (struct dw_wdt *)dev->driver_data;
	wdd	   = &(dw_wdt->wdd);

	if (strncmp(buf, "set", 3) == 0) {
		dw_wdt_set_timeout(wdd, 1);
		dev_info(dev, "wdt0 set\n");
	} else if (strncmp(buf, "start", 5) == 0) {
		dev_info(dev, "wdt0 11 start\n");
		dw_wdt_start(wdd);
		dev_info(dev, "wdt0 22 start\n");
	} else if (strncmp(buf, "reboot", 6) == 0) {
		dev_info(dev, "wdt0 reboot\n");
		dw_wdt_restart(wdd, 0, NULL);
	} else if (strncmp(buf, "ping", 4) == 0) {
		dev_info(dev, "wdt0 reboot\n");
		dw_wdt_restart(wdd, 0, NULL);
	} else if (strncmp(buf, "stop", 4) == 0) {
		dev_err(dev, "stop\n");
		dw_wdt_stop(wdd);
	}

	return count;
}

static DEVICE_ATTR(wdt_test, S_IWUSR | S_IRUSR, wdt_show, wdt_set);

static struct attribute *wdt_attributes[] = {
	&dev_attr_wdt_test.attr,
	NULL
};

static const struct attribute_group wdt_attr_group = {
	.attrs = wdt_attributes,
};

static void dw_wdt_prase_dt(struct device *dev, struct dw_wdt *dw_wdt)
{
	unsigned int tmp;

	if(!of_property_read_u32(dev->of_node, "reset-mode", &tmp)) {
		if (tmp == 1) {
			dw_wdt->reset_mode = WDT_INTERRUPT;
		} else {
			dw_wdt->reset_mode = WDT_SYSTEM_RESET;
		}
	} else {
		dw_wdt->reset_mode = WDT_SYSTEM_RESET;
	}

	if(!of_property_read_u32(dev->of_node, "uboot-work", &tmp)) {
		if (tmp == 1) {
			dw_wdt->uboot_work = true;
		} else {
			dw_wdt->uboot_work = false;
		}
	} else {
		dw_wdt->uboot_work = false;
	}
}

static int dw_wdt_drv_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct watchdog_device *wdd;
	struct dw_wdt *dw_wdt;
	struct resource *mem;
	int ret;
	int irq;

	dw_wdt = devm_kzalloc(dev, sizeof(*dw_wdt), GFP_KERNEL);
	if (!dw_wdt)
		return -ENOMEM;

	dw_wdt_prase_dt(dev, dw_wdt);

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	dw_wdt->regs = devm_ioremap_resource(dev, mem);
	if (IS_ERR(dw_wdt->regs))
		return PTR_ERR(dw_wdt->regs);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_info(dev, "No irq %d in DT\n", irq);
		return -ENODEV;
	}

	ret = devm_request_irq(&pdev->dev, irq,
				   dw_wdt_handler,
				   0, dev_name(&pdev->dev), dw_wdt);
	if (ret)
		return -ENODEV;

	dw_wdt->pclk = devm_clk_get(dev, "pclk");
	if (IS_ERR(dw_wdt->clk))
		return PTR_ERR(dw_wdt->clk);

	dw_wdt->clk = devm_clk_get(dev, "work_clk");
	if (IS_ERR(dw_wdt->clk))
		return PTR_ERR(dw_wdt->clk);

	ret = clk_prepare_enable(dw_wdt->pclk);
	if (ret)
		return ret;

	ret = clk_prepare_enable(dw_wdt->clk);
	if (ret)
		return ret;

	dw_wdt->rate = clk_get_rate(dw_wdt->clk);
	if (dw_wdt->rate == 0) {
		ret = -EINVAL;
		goto out_disable_clk;
	}

	//dev_info(dev, "dw_wdt->rate = %ld!\n", dw_wdt->rate);
	dw_wdt->rst_tick = devm_reset_control_get_optional_exclusive(&pdev->dev, "wdt_tick");
	if (IS_ERR(dw_wdt->rst_tick)) {
		ret = PTR_ERR(dw_wdt->rst_tick);
		goto out_disable_clk;
	} else {
		reset_control_deassert(dw_wdt->rst_tick);
	}

	dw_wdt->rst_apb = devm_reset_control_get_optional_exclusive(&pdev->dev, "wdt_apb");
	if (IS_ERR(dw_wdt->rst_apb)) {
		ret = PTR_ERR(dw_wdt->rst_apb);
		goto out_disable_clk;
	} else {
		reset_control_deassert(dw_wdt->rst_apb);
	}

	dw_wdt->rst_glb = devm_reset_control_get_optional_exclusive(&pdev->dev, "wdt_glb");
	if (IS_ERR(dw_wdt->rst_glb)) {

	} else {
		reset_control_deassert(dw_wdt->rst_glb);
	}

	wdd = &dw_wdt->wdd;
	wdd->info = &dw_wdt_ident;
	wdd->ops = &dw_wdt_ops;
	wdd->min_timeout = 1;
	wdd->max_hw_heartbeat_ms =
		dw_wdt_top_in_seconds(dw_wdt, DW_WDT_MAX_TOP) * 1000;
	wdd->parent = dev;

	watchdog_set_drvdata(wdd, dw_wdt);
	watchdog_set_nowayout(wdd, nowayout);
	watchdog_init_timeout(wdd, 0, dev);

	dw_wdt_hardware_init(wdd);

	/*
	 * If the watchdog is already running, use its already configured
	 * timeout. Otherwise use the default or the value provided through
	 * devicetree.
	 */
	if (dw_wdt_is_enabled(dw_wdt)) {
		wdd->timeout = dw_wdt_get_top(dw_wdt);
		if (dw_wdt->uboot_work) {
			/*
			 * wdt is start on uboot, otherwise open this.
			 */
			//set_bit(WDOG_HW_RUNNING, &wdd->status);
		} else {
			set_bit(WDOG_HW_RUNNING, &wdd->status);
		}
	} else {
		wdd->timeout = DW_WDT_DEFAULT_SECONDS;
		watchdog_init_timeout(wdd, 0, dev);
	}

	platform_set_drvdata(pdev, dw_wdt);
	watchdog_set_restart_priority(wdd, 128);

	ret = watchdog_register_device(wdd);
	if (ret)
		goto out_disable_clk;


	ret = sysfs_create_group(&dev->kobj, &wdt_attr_group);
	if (ret < 0)
		dev_warn(dev, "attr group create failed\n");

	return 0;

out_disable_clk:
	clk_disable_unprepare(dw_wdt->clk);
	return ret;
}

static int dw_wdt_drv_remove(struct platform_device *pdev)
{
	struct dw_wdt *dw_wdt = platform_get_drvdata(pdev);

	sysfs_remove_group(&pdev->dev.kobj, &wdt_attr_group);
	watchdog_unregister_device(&dw_wdt->wdd);
	dw_wdt_reset_control_assert_and_deassert(dw_wdt);
	clk_disable_unprepare(dw_wdt->clk);
	clk_disable_unprepare(dw_wdt->pclk);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id dw_wdt_of_match[] = {
	{ .compatible = "snps,dw-wdt", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, dw_wdt_of_match);
#endif

static struct platform_driver dw_wdt_driver = {
	.probe		= dw_wdt_drv_probe,
	.remove		= dw_wdt_drv_remove,
	.driver		= {
		.name	= "dw_wdt",
		.of_match_table = of_match_ptr(dw_wdt_of_match),
		.pm	= &dw_wdt_pm_ops,
	},
};

module_platform_driver(dw_wdt_driver);

MODULE_AUTHOR("Jamie Iles");
MODULE_DESCRIPTION("Synopsys DesignWare Watchdog Driver");
MODULE_LICENSE("GPL");
