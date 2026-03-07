/*
 * An RTC driver for the Tsingmicro Tx511 processor series.
 *
 * Copyright (C) 2007 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/rtc.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/delay.h>

 #define RTC_CDR    0x00	/* Offset: 0x00 (R/ )  rtc_cnt, read value */
 #define RTC_CLR    0x04	/* Offset: 0x04 (R/W)  rtc_cnt, set value */
 #define RTC_CR     0x08	/* Offset: 0x08 (R/W)  rtc_cnt control register */
 #define RTC_STAT   0x0c	/* Offset: 0x0c (R/ )  mask interrupt register */
 #define RTC_RSTAT  0x10	/* Offset: 0x10 (R/ )  raw mask interrupt register */
 #define RTC_VID    0x14	/* Offset: 0x14 (R/ )  version register */
 #define RTC_TR_L   0x18	/* Offset: 0x18 (R/ )  time register, low 16 bits */
 #define RTC_TR_H   0x1c	/* Offset: 0x1c (R/ )  time register, high 16 bits */
 #define RTC_DR_L   0x20	/* Offset: 0x20 (R/ )  date register, low 16 bits */
 #define RTC_DR_H   0x24	/* Offset: 0x24 (R/ )  date register, high 16 bits */
 #define RTC_TLR_L  0x28	/* Offset: 0x28 (R/W)  time load register, low 16 bits */
 #define RTC_TLR_H  0x2c	/* Offset: 0x2c (R/W)  time load register, high 16 bits */
 #define RTC_DLR_L  0x30	/* Offset: 0x30 (R/W)  date load register, low 16 bits */
 #define RTC_DLR_H  0x34	/* Offset: 0x34 (R/W)  date load register, high 16 bits */
 #define RTC_TMR_L  0x38	/* Offset: 0x38 (R/W)  time interrupt match reg, low 16 bits */
 #define RTC_TMR_H  0x3c	/* Offset: 0x3c (R/W)  time interrupt match reg, high 16 bits */
 #define RTC_DTCR_L 0x40	/* Offset: 0x40 (R/W)  data/time intc, low 16 bits */
 #define RTC_DTCR_H 0x44	/* Offset: 0x44 (R/ )  data/time intc, high 16 bits */
 #define RTC_CAL_CR 0x48	/* Offset: 0x48 (R/W)  cal control register */
 #define RTC_CAL_LD 0x4c	/* Offset: 0x4c (R/W)  calibration value set register */
 #define RTC_CAL_RD 0x50	/* Offset: 0x50 (R/ )  calibration value get register */

struct rtc_ts {
	struct rtc_device	*rtc;
	void __iomem		*regs;
	unsigned long		alarm_time;
	unsigned long		irq;
	/* Protect against concurrent register access. */
	spinlock_t		lock;
};

static const uint8_t g_leap_year[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const uint8_t g_noleap_year[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int ts_clock_isleapyear(int year)
{
	return (year % 400) ? ((year % 100) ? ((year % 4) ? 0 : 1) : 0) : 1;
}

static int ts_check_tm_ok(struct rtc_time *rtctime)
{
	int32_t leap = 1;

	if (rtctime->tm_year < 70 || rtctime->tm_year >= 2000) {
		goto error_time;
	}

	leap = ts_clock_isleapyear(rtctime->tm_year + 1900);

	if (rtctime->tm_sec < 0 || rtctime->tm_sec >= 60) {
		goto error_time;
	}

	if (rtctime->tm_min < 0 || rtctime->tm_min >= 60) {
		goto error_time;
	}

	if (rtctime->tm_hour < 0 || rtctime->tm_hour >= 24) {
		goto error_time;
	}

	if (rtctime->tm_mon < 0 || rtctime->tm_mon >= 12) {
		goto error_time;
	}

	if (leap) {
		if (rtctime->tm_mday < 1 || rtctime->tm_mday > g_leap_year[rtctime->tm_mon]) {
			goto error_time;
		}
	} else {
		if (rtctime->tm_mday < 1 || rtctime->tm_mday > g_noleap_year[rtctime->tm_mon]) {
			goto error_time;
		}
	}


	return 0;
error_time:
	return -1;
}

static uint32_t ts_rtc_get_week_by_date(struct rtc_time *rtctime)
{
	/* week:				 tm_wday:
	 * 0 is Monday			 0 is Sunday
	 * 1 is Tuesday			 1 is Monday
	 * 2 is Wednesday		 2 is Tuesday
	 * 3 is Thursday		 3 is Wednesday
	 * 4 is Friday			 4 is Thursday
	 * 5 is Saturday		 5 is Friday
	 * 6 is Sunday			 6 is Saturday
	 */
	uint32_t year, mon, day, week;

	year = rtctime->tm_year + 1900;
	mon = rtctime->tm_mon + 1;
	day = rtctime->tm_mday;
	week = (day + 2 * mon + 3 * (mon + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;

	if (week == 6)
		week = 0;
	else
		week++;

	return week;
}

static void ts_rtc_enable(struct device *dev)
{
	struct rtc_ts *rtc = dev_get_drvdata(dev);

	writel((1 << 2), rtc->regs + RTC_CR);
}

static void ts_rtc_disable(struct device *dev)
{
	struct rtc_ts *rtc = dev_get_drvdata(dev);

	writel(0, rtc->regs + RTC_CR);
}

static int ts_rtc_settime(struct device *dev, struct rtc_time *tm)
{
	struct rtc_ts *rtc = dev_get_drvdata(dev);
	uint32_t temp = 0;
	uint32_t temp_wday = 0;
	int32_t ret = -1;

	spin_lock_irq(&rtc->lock);

	ret = ts_check_tm_ok(tm);
	if (ret != 0) {
		spin_unlock_irq(&rtc->lock);
		return ret;
	}

	ts_rtc_disable(dev);
	temp_wday = ts_rtc_get_week_by_date(tm);

	/* set sec TLR_L[7:0] */
	temp |= (tm->tm_sec & 0xff);
	/* set min TLR_L[15:8] */
	temp |= (tm->tm_min & 0xff) << 8;
	writel(temp, rtc->regs + RTC_TLR_L);

	temp = 0;
	/* set hour TLR_H[7:0] */
	temp |= (tm->tm_hour & 0xff);
	/* set week TLR_H[15:8] */
	temp |= (temp_wday & 0xff) << 8;
	writel(temp, rtc->regs + RTC_TLR_H);

	temp = 0;
	/* set day DLR_L[7:0] */
	temp |= (tm->tm_mday & 0xff);
	/* set mon DLR_L[15:8], tm_mon is (0-11), but in register is (1-12) */
	temp |= ((tm->tm_mon + 1) & 0xff) << 8;
	writel(temp, rtc->regs + RTC_DLR_L);

	/* set year DLR_H[11:0] */
	temp = (tm->tm_year & 0xfff) + 1900;
	writel(temp, rtc->regs + RTC_DLR_H);

	ts_rtc_enable(dev);
	spin_unlock_irq(&rtc->lock);

	return 0;
}
/*note！！！ after set rtc timer, update rtc timer maybe need 200us???*/
static int ts_rtc_readtime(struct device *dev, struct rtc_time *tm)
{
	struct rtc_ts *rtc = dev_get_drvdata(dev);

	spin_lock_irq(&rtc->lock);
	/* sec TR_L[7:0] */
	tm->tm_sec = readl(rtc->regs + RTC_TR_L) & 0xff;
	/* min TR_L[15:8] */
	tm->tm_min = (readl(rtc->regs + RTC_TR_L) >> 8) & 0xff;
	/* hour TR_H[7:0] */
	tm->tm_hour = readl(rtc->regs + RTC_TR_H) & 0xff;
	/* week TR_H[15:8] */
	tm->tm_wday = (readl(rtc->regs + RTC_TR_H) >> 8) & 0xff;
	/* day DR_L[7:0] */
	tm->tm_mday = readl(rtc->regs + RTC_DR_L) & 0xff;
	/* mon DR_L[15:8], tm_mon is (0-11), but in register is (1-12) */
	tm->tm_mon = ((readl(rtc->regs + RTC_DR_L) >> 8) & 0xff) - 1;
	/* year DR_H[11:0] */
	tm->tm_year = (readl(rtc->regs + RTC_DR_H) & 0xfff) - 1900;

	spin_unlock_irq(&rtc->lock);

	return 0;
}

static int ts_rtc_readalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rtc_ts *rtc = dev_get_drvdata(dev);
	struct rtc_time *rtc_time = &alrm->time;
	uint32_t temp = 0;

	spin_lock_irq(&rtc->lock);

	temp = 0;
	temp = readl(rtc->regs + RTC_TMR_L);
	/* march_sec TMR_L[7:0] */
	rtc_time->tm_sec = temp & 0xff;
	/* march_min TMR_L[15:8] */
	rtc_time->tm_min = (temp >> 8) & 0xff;

	temp = 0;
	temp = readl(rtc->regs + RTC_TMR_H);
	/* march_hour TMR_H[7:0] */
	rtc_time->tm_hour = temp & 0xff;
	/* march_week */
	if (temp & (1 << 14))
		rtc_time->tm_wday = 0;	/* when sunday*/
	else if (temp & (1 << 8))
		rtc_time->tm_wday = 1;
	else if (temp & (1 << 9))
		rtc_time->tm_wday = 2;
	else if (temp & (1 << 10))
		rtc_time->tm_wday = 3;
	else if (temp & (1 << 11))
		rtc_time->tm_wday = 4;
	else if (temp & (1 << 12))
		rtc_time->tm_wday = 5;
	else if (temp & (1 << 13))
		rtc_time->tm_wday = 6;

	spin_unlock_irq(&rtc->lock);

	return 0;
}

static int ts_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rtc_ts *rtc = dev_get_drvdata(dev);
	uint32_t temp = 0;
	uint32_t temp_wday = 0;

	spin_lock_irq(&rtc->lock);

	temp_wday = ts_rtc_get_week_by_date(&alrm->time);

	/* march_sec TMR_L[7:0] */
	temp |= (alrm->time.tm_sec & 0xff);
	/* march_min TMR_L[15:8] */
	temp |= (alrm->time.tm_min & 0xff) << 8;
	writel(temp, rtc->regs + RTC_TMR_L);

	temp = 0;
	/* march_hour TMR_H[7:0] */
	temp |= (alrm->time.tm_hour & 0xff);

	/* march_week */
	if (temp_wday == 0) {
		/* when sunday*/
		temp |= 1 << 14;
	} else {
		temp |= 1 << (temp_wday + 7);
	}
	writel(temp, rtc->regs + RTC_TMR_H);

	spin_unlock_irq(&rtc->lock);

	return 0;
}

static int ts_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct rtc_ts *rtc = dev_get_drvdata(dev);
	int ret = 0;
	uint32_t temp;

	spin_lock_irq(&rtc->lock);

	if (enabled) {
		temp  = readl(rtc->regs + RTC_DTCR_L);
		/* unmask int */
		temp &= ~(1 << 15);
		/* enable int */
		temp |= 1 << 7;
		writel(temp, rtc->regs + RTC_DTCR_L);
	} else {
		temp  = readl(rtc->regs + RTC_DTCR_L);
		/* unmask int */
		temp |= (1 << 15);
		/* enable int */
		temp &= ~(1 << 7);
		writel(temp, rtc->regs + RTC_DTCR_L);
	}

	spin_unlock_irq(&rtc->lock);

	return ret;
}

static irqreturn_t ts_rtc_interrupt(int irq, void *dev_id)
{
	struct rtc_ts *rtc = (struct rtc_ts *)dev_id;
	int ret = IRQ_HANDLED;

	spin_lock_irq(&rtc->lock);
	writel(0xff, rtc->regs + RTC_DTCR_H);
	spin_unlock_irq(&rtc->lock);

	return ret;
}

static int ts_rtc_init(struct device *dev)
{
	struct rtc_ts *rtc = dev_get_drvdata(dev);

	/* set auto calibration */
	writel(0x2, rtc->regs + RTC_CAL_CR);
	/* clear the int */
	writel(0xff, rtc->regs + RTC_DTCR_H);

	/* enable count */
	ts_rtc_enable(dev);


	return 0;
}

static const struct rtc_class_ops ts_rtc_ops = {
	.read_time	= ts_rtc_readtime,
	.set_time	= ts_rtc_settime,
	.read_alarm	= ts_rtc_readalarm,
	.set_alarm	= ts_rtc_setalarm,
	.alarm_irq_enable = ts_rtc_alarm_irq_enable,
};

enum reg_sysfs_cmd {
	SYSFS_CMD_READ_REG    = 0,
	SYSFS_CMD_WRITE_REG   = 1,
	SYSFS_CMD_MAX
};
struct rtc_ts_sysfs_cmd {
	enum reg_sysfs_cmd cmd;
	const char *key_str;
};

static ssize_t ts_rtc_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct rtc_ts *ts_rtc_chip =
		(struct rtc_ts *)dev->driver_data;

	void __iomem *iobase = NULL;
	u32 val;
	u32 i;
	int off = 0;

	if (buf == NULL) {
		dev_err(dev, "buf is NULL\n");
		return 0;
	}

	iobase = ts_rtc_chip->regs;

	off += sprintf(buf + off, "------dump regs begin------\n");
	for (i = 0; i <= 0x50; i += 4) {
		val = readl(iobase + i);
		off += sprintf(buf + off, "[%02x] 0x%x\n", i, val);
	}

	off += sprintf(buf + off, "------dump regs end	------\n");

	return off;

}

static ssize_t ts_rtc_set(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	struct rtc_ts *ts_rtc_chip =
		(struct rtc_ts *)dev->driver_data;
	void __iomem *iobase = NULL;
	u32 i;
	char *p = NULL;
	char *endp = NULL;
	bool found = false;
	u32 op_reg, rd_val = 0, wr_val = 0;

	struct rtc_ts_sysfs_cmd cmd_arr[] = {
		{SYSFS_CMD_READ_REG, "show"},
		{SYSFS_CMD_WRITE_REG, "set"},
	};


	if (buf == NULL) {
		dev_err(dev, "buf is NULL\n");
		return 0;
	}

	iobase = ts_rtc_chip->regs;

	for (i = 0; i < ARRAY_SIZE(cmd_arr); i++) {
		if (cmd_arr[i].key_str == NULL)
			break;
		p = strstr(buf, cmd_arr[i].key_str);
		if (p) {
			p += strlen(cmd_arr[i].key_str);
			found = true;
			break;
		}
	}

	if (!found) {
		pr_err("show reg\n");
		pr_err("set reg value\n");
		return count;
	}

	op_reg = (u32)simple_strtoul(p + 1, &endp, 0);
	if (op_reg < 0 || op_reg > 0x50) {
		pr_err("op_reg is error\n");
		return count;
	}

	switch (cmd_arr[i].cmd) {
	case SYSFS_CMD_READ_REG:
		rd_val = readl(iobase + op_reg);
		pr_err("\nrd reg[%02d][0x%02x] 0x%x\n", op_reg, op_reg, rd_val);
		break;

	case SYSFS_CMD_WRITE_REG:
		if (endp == NULL) {
			pr_err("not enough para, wr_val needed\n");
			return count;
		}

		wr_val = (u32)simple_strtoul(endp + 1, &endp, 0);
		writel(wr_val, iobase + op_reg);
		pr_err("\nwr reg [%02d][0x%02x], val = 0x%x\n", op_reg, op_reg, wr_val);
		break;
	default:
		break;

	}

	return count;
}

static DEVICE_ATTR(ts_rtc_test, S_IWUSR | S_IRUSR,
			ts_rtc_show, ts_rtc_set);

static struct attribute *ts_rtc_attributes[] = {
	&dev_attr_ts_rtc_test.attr,
	NULL
};

static const struct attribute_group ts_rtc_attr_group = {
	.attrs = ts_rtc_attributes,
};

static int ts_rtc_drv_probe(struct platform_device *pdev)
{
	struct resource	*regs;
	struct rtc_ts *ts_rtc;
	int irq;
	int ret;

	ts_rtc = devm_kzalloc(&pdev->dev, sizeof(struct rtc_ts),
			   GFP_KERNEL);
	if (!ts_rtc)
		return -ENOMEM;

	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs) {
		dev_dbg(&pdev->dev, "no mmio resource defined\n");
		return -ENXIO;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0) {
		dev_dbg(&pdev->dev, "could not get irq\n");
		return -ENXIO;
	}

	ts_rtc->irq = irq;
	ts_rtc->regs = devm_ioremap_resource(&pdev->dev, regs);
	if (!ts_rtc->regs) {
		dev_dbg(&pdev->dev, "could not map I/O memory\n");
		return -ENOMEM;
	}
	//disable and mask all interrupts and disable
	writel(0xffff, ts_rtc->regs + RTC_DTCR_L);
	spin_lock_init(&ts_rtc->lock);

	ret = devm_request_irq(&pdev->dev, irq, ts_rtc_interrupt, 0,
				"rtc", ts_rtc);
	if (ret) {
		dev_dbg(&pdev->dev, "could not request irq %d\n", irq);
		return ret;
	}

	platform_set_drvdata(pdev, ts_rtc);

	ts_rtc->rtc = devm_rtc_device_register(&pdev->dev, pdev->name,
				&ts_rtc_ops, THIS_MODULE);
	if (IS_ERR(ts_rtc->rtc)) {
		dev_dbg(&pdev->dev, "could not register rtc device\n");
		return PTR_ERR(ts_rtc->rtc);
	}

	ts_rtc_init(&pdev->dev);

	ret = sysfs_create_group(&pdev->dev.kobj, &ts_rtc_attr_group);
	if (ret < 0)
		dev_warn(&pdev->dev, "attr group create failed\n");

	return 0;
}

static int ts_rtc_drv_remove(struct platform_device *pdev)
{

	struct rtc_ts *ts_rtc = platform_get_drvdata(pdev);

	devm_rtc_device_unregister(&pdev->dev, ts_rtc->rtc);

	sysfs_remove_group(&pdev->dev.kobj, &ts_rtc_attr_group);

	return 0;
}

static const struct of_device_id ts_rtc_of_match[] = {
	{ .compatible = "ts,rtc", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ts_rtc_of_match);

static struct platform_driver ts_rtc_driver = {
	.probe		= ts_rtc_drv_probe,
	.remove		= ts_rtc_drv_remove,
	.driver		= {
		.name	= "ts_rtc",
		.of_match_table = of_match_ptr(ts_rtc_of_match),
	},
};

module_platform_driver(ts_rtc_driver);

MODULE_AUTHOR("tsingmicro");
MODULE_DESCRIPTION("Real time clock for Tsingmicro");
MODULE_LICENSE("GPL");

