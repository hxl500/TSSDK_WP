/**
 * @copyright Copyright (c) 2022  Tsingmicro Intelligent Technology Co., Limited
 * @file tx_timer.c
 * @brief
 *
 * @author ymc (yangmengchao@tsingmicro.com)
 * @version 1.0
 * @date 2022-10-14
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <asm/uaccess.h>
//#include <asm/system.h>
#include <asm/io.h>
#include <asm/pgtable.h>
#include <asm/cacheflush.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/irq.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
//#include <mach/platform.h>
//#include <mach/irqs.h>
#include "tx_timer.h"
#include "tx_motor.h"
#include "motor_internal_struct.h"

struct ts_timer_t {
    int init;          /* timer initialize times */
    int index;         /* timer index */
    char *name;
    unsigned int addr; /* timer control base address */
    unsigned int irq;
    timer_handler handler;
    void *priv;
	struct clk *pclk;
	struct clk *timer;
};

#define TS_TIMER(_index, _irq, _addr) { \
    .init = 0, \
    .index = _index, \
    .name = "timer"#_index, \
    .addr = _addr, \
    .priv = NULL, \
    .handler = NULL, \
    .irq  = _irq,\
}

static struct ts_timer_t ts_timerlist[] = {
    TS_TIMER(0, INTNR_TIMER_0, TIMER0_REG_BASE),
    TS_TIMER(1, INTNR_TIMER_1, TIMER1_REG_BASE),
    TS_TIMER(2, INTNR_TIMER_2, TIMER2_REG_BASE),
    TS_TIMER(3, INTNR_TIMER_3, TIMER3_REG_BASE),
    TS_TIMER(4, INTNR_TIMER_4, TIMER4_REG_BASE),
    //TS_TIMER(5, INTNR_TIMER_5, TIMER5_REG_BASE),
    //TS_TIMER(6, INTNR_TIMER_6, TIMER6_REG_BASE),
    //TS_TIMER(7, INTNR_TIMER_7, TIMER7_REG_BASE),
};

#define TIMER(_index)        (&ts_timerlist[_index])

static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
    int status;
    int handled = 0;
    struct ts_timer_t *ts_timer = (struct ts_timer_t *)dev_id;
    status = timer_readl(ts_timer->addr, REG_TIMER_MIS);
    if (status) {
        pr_debug("\033[1;35m%s[%d]\n\033[m", __FUNCTION__, __LINE__);
        ts_timer->handler(ts_timer->priv);
        timer_readl(ts_timer->addr, REG_TIMER_INTCLR);
        handled = 1;
    }
    return IRQ_RETVAL(handled);
}

void *tx_timer_probe(struct platform_device *pdev, int which_timer, int timer_irq)
{
    struct ts_timer_t *ts_timer = NULL;
	struct device *dev = &pdev->dev;
	int ret;
	unsigned long pclk, timer;

    if (which_timer > 6 || which_timer < 2) {
        pr_debug("timer index input error.\n");
        return NULL;
    }
    ts_timer = TIMER(which_timer);

	ts_timer->pclk = devm_clk_get(dev, "pclk");
	if (IS_ERR(ts_timer->pclk)) {
		dev_err(dev, "Cannot claim timer pclock.\n");
		return NULL;
	}

	ret = clk_prepare_enable(ts_timer->pclk);
	if (ret) {
		dev_err(dev, "Cannot enable timer pclock.\n");
		return NULL;
	}

	ts_timer->timer = devm_clk_get(dev, "timer");
	if (IS_ERR(ts_timer->timer)) {
		dev_err(dev, "Cannot claim timer timer_clock.\n");
		return NULL;
	}

	ret = clk_prepare_enable(ts_timer->timer);
	if (ret) {
		dev_err(dev, "Cannot enable timer timer_clock.\n");
		return NULL;
	}

	pclk = clk_get_rate(ts_timer->pclk);
	pr_info("clock %s, %lu\n", __clk_get_name(ts_timer->pclk), pclk);

	timer = clk_get_rate(ts_timer->timer);
	pr_info("clock %s, %lu\n", __clk_get_name(ts_timer->timer), timer);

	ts_timer->irq = timer_irq;
	ts_timer->addr = (unsigned int)ioremap(ts_timer->addr, 0x14);
    if (ts_timer->init) {
        pr_debug("error, timer busy.\n");
        return NULL;
    }

    if (request_irq(ts_timer->irq, timer_interrupt, IRQ_TYPE_LEVEL_HIGH, ts_timer->name, ts_timer) < 0) {
        printk("request irq %d failed\n", ts_timer->irq);
        return NULL;
    }
    ts_timer->init = 1;
    pr_debug("%s %s(index:%d)\n", __func__, ts_timer->name, which_timer);

    return ts_timer;
}

int tx_timer_remove(void *priv)
{
    struct ts_timer_t *ts_timer = (struct ts_timer_t *)priv;

    free_irq(ts_timer->irq, ts_timer);
    ts_timer->init = 0;
	iounmap((void __iomem *)ts_timer->addr);
    pr_debug("%s %d\n", __func__, __LINE__);

    return 0;
}

int tx_timer_start(timer_handler handler, void *data, void *priv, int hz)
{
    //int value;
    int counter;
    struct ts_timer_t *ts_timer = (struct ts_timer_t *)priv;

    pr_debug("%s %d\n", __func__, __LINE__);
    counter = TIMER_CLK / hz;               /* maybe hz means s, not ms? */
    ts_timer->handler = handler;
    ts_timer->priv = data;
    pr_debug("%s hz %d counter %d\n", __func__, hz, counter);
	timer_writel(ts_timer->addr, counter, REG_TIMER_RELOAD);
    timer_writel(ts_timer->addr, 0x3, REG_TIMER_CONTROL);
    //timer_writel(ts_timer->addr, counter, REG_TIMER_BGLOAD);
    //value = timer_readl(ts_timer->addr, REG_TIMER_CONTROL);
    //value |= 1 << 7;
    //timer_writel(ts_timer->addr, value, REG_TIMER_CONTROL);
    pr_debug("%s %s start\n", __func__, ts_timer->name);

    return 0;
}

int tx_timer_stop(void *priv)
{
    int value;
    struct ts_timer_t *ts_timer = (struct ts_timer_t *)priv;

    value = timer_readl(ts_timer->addr, REG_TIMER_CONTROL);
    value &= ~(1 << 0);
    timer_writel(ts_timer->addr, value, REG_TIMER_CONTROL);
    ts_timer->handler = NULL;
    ts_timer->priv = NULL;
    pr_debug("%s %d\n", __func__, __LINE__);

    return 0;
}

