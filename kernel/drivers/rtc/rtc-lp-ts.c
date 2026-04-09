/*
 * A Low Power RTC driver for the Tsingmicro Tx511 processor series.
 *
 * Copyright (C) 2007 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

//#define DEBUG
#include "linux/of.h"
#include "linux/time64.h"
#include "linux/types.h"
#include <linux/rtc/rtc-lp-ts.h>

static int wr_lp_rtc_reg(struct lp_rtc_ts *lp_rtc, uint8_t addr, uint8_t wdata);
static uint8_t rd_lp_rtc_reg(struct lp_rtc_ts *lp_rtc, uint8_t addr);
static void rd_lp_read_check(struct lp_rtc_ts *lp_rtc,uint8_t addr, uint8_t expdata);

/**
 * @brief: write time value into the time register of lp_rtc
 * @param {lp_rtc_ts* } lp_rtc: lp_rtc_ts structure contained all private info of lp rtc
 * @param {uint8_t* } time: time value pointor. lp rtc has four time registers and every one takes up one byte
 * @return {void}
 */
static void write_time(struct lp_rtc_ts *lp_rtc,uint8_t* time)
{
	/* write time rigister must operator following steps*/
	/* step 1:write any value into status register*/
	wr_lp_rtc_reg(lp_rtc, 0x00, 0x03);
	/* step 2:wait 30 us */
	wait_us(30);
	/* step 3:write time register */
	wr_lp_rtc_reg(lp_rtc,TIME_REG_7_0,time[0]);
	wr_lp_rtc_reg(lp_rtc,TIME_REG_15_8,time[1]);
	wr_lp_rtc_reg(lp_rtc,TIME_REG_23_16,time[2]);
	wr_lp_rtc_reg(lp_rtc,TIME_REG_31_24,time[3]);
}

static void read_time(struct lp_rtc_ts *lp_rtc,uint8_t* time)
{
	wr_lp_rtc_reg(lp_rtc, 0x00, 0x03);
	wait_us(30);
	/*Time Register Address Grow Up From Index 0 To TIME_REG_LEN */
	time[0] = rd_lp_rtc_reg(lp_rtc, TIME_REG_7_0);
	time[1] = rd_lp_rtc_reg(lp_rtc, TIME_REG_15_8);
	time[2] = rd_lp_rtc_reg(lp_rtc, TIME_REG_23_16);
	time[3] = rd_lp_rtc_reg(lp_rtc, TIME_REG_31_24);
}

static int wr_lp_rtc_reg(struct lp_rtc_ts *lp_rtc, uint8_t addr, uint8_t wdata)
{
	lp_rtc_reg_t lp_rtc_reg;
	int timeout = TIMEOUT;

	/*waiting for lp_rtc be idle*/
	lp_rtc_reg.lp_rtc_ctl.u32 = readl(lp_rtc->regs + LP_RTC_CTL );
	while(lp_rtc_reg.lp_rtc_ctl.rtc_cfg_busy != 0x0)
	{
		if(timeout <=0)
		{
			pr_debug("lp_rtc wr wait rtc_cfg_busy %x!",lp_rtc_reg.lp_rtc_ctl.u32);
			return -ETIMEDOUT;
		}

		lp_rtc_reg.lp_rtc_ctl.u32 = readl(lp_rtc->regs + LP_RTC_CTL );

		timeout--;
		//usleep_range(1000,1100);
		udelay(100);
	}

	lp_rtc_reg.lp_rtc_ctl.u32 = readl(lp_rtc->regs + LP_RTC_CTL );
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_rd = 0x0;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_wr = 0x1;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_addr = addr;
	lp_rtc_reg.lp_rtc_ctl.we_rtc_reg_addr = 0x1;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_wdata = wdata;
	lp_rtc_reg.lp_rtc_ctl.we_rtc_reg_wdata = 0x1;
	writel(lp_rtc_reg.lp_rtc_ctl.u32, lp_rtc->regs + LP_RTC_CTL);

	return 0;
}

static uint8_t rd_lp_rtc_reg(struct lp_rtc_ts *lp_rtc, uint8_t addr)
{
	lp_rtc_reg_t lp_rtc_reg;
	uint8_t rd_rdata8;
	int timeout = TIMEOUT;

	lp_rtc_reg.lp_rtc_ctl.u32 = readl(lp_rtc->regs + LP_RTC_CTL );

	while(lp_rtc_reg.lp_rtc_ctl.rtc_cfg_busy != 0x0)
	{
		if(timeout <=0) {
			pr_err("rd wait rtc_cfg_busy %x!",lp_rtc_reg.lp_rtc_ctl.u32);
			return -ETIMEDOUT;
		}

		lp_rtc_reg.lp_rtc_ctl.u32 = readl(lp_rtc->regs + LP_RTC_CTL );

		timeout--;
		//usleep_range(1000,1100);
		udelay(100);
	}

	lp_rtc_reg.lp_rtc_ctl.rtc_reg_rd = 0x1;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_wr = 0x0;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_addr = addr;
	lp_rtc_reg.lp_rtc_ctl.we_rtc_reg_addr = 0x1;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_wdata = 0x0;
	lp_rtc_reg.lp_rtc_ctl.we_rtc_reg_wdata = 0x0;
	writel(lp_rtc_reg.lp_rtc_ctl.u32, lp_rtc->regs + LP_RTC_CTL);

	lp_rtc_reg.lp_rtc_rdat.u32 = readl(lp_rtc->regs + LP_RTC_RDAT);
	timeout = TIMEOUT;
	/* waiting rtc read data be ready */
	while(lp_rtc_reg.lp_rtc_rdat.rtc_rdat_rdy != 0x1)
	{
		if(timeout <=0) {
			pr_err("waiting for rtc_rdat ready %x!",lp_rtc_reg.lp_rtc_rdat.u32);
			return -ETIMEDOUT;
		}

		lp_rtc_reg.lp_rtc_rdat.u32 = readl(lp_rtc->regs + LP_RTC_RDAT);

		timeout--;
		//usleep_range(1000,1100);
		udelay(100);
	}

	rd_rdata8 = lp_rtc_reg.lp_rtc_rdat.rtc_reg_rdata;

	return rd_rdata8;
}

/**
 * @brief: Check whether lp_rtc write value to the register is successful
 * @param {lp_rtc_ts *} lp_rtc: lp_rtc private info structure
 * @param {uint8_t} addr: address of the purpose register
 * @param {uint8_t} expdata: expects right data of purpose register
 * @return {*} void
 */
__attribute__((unused))
static void rd_lp_read_check(struct lp_rtc_ts *lp_rtc,uint8_t addr, uint8_t expdata)
{
	uint8_t ck_rdata8;

	ck_rdata8 = rd_lp_rtc_reg(lp_rtc,addr);

	if(ck_rdata8 != expdata)
		pr_debug("Err! read rtc check failed with addr:%x, read:%x, exp:%x \r\n",addr, ck_rdata8, expdata);
	else
		pr_debug("Ok! read rtc check okay with addr:%x, read:%x, exp:%x \r\n",addr, ck_rdata8, expdata);
}

static void lp_rtc_open(struct lp_rtc_ts *lp_rtc, bool enable)
{
	uint32_t lp_rtc_en = 0;

	lp_rtc_en = readl(lp_rtc->regs + LP_RTC_EN);
	pr_debug("lp rtc enable status:%d\n", lp_rtc_en);
	if(enable) {
		lp_rtc_en |= 1 << LP_RTC_EN_OFFSET ;
	} else {
		lp_rtc_en &= ~(1 << LP_RTC_EN_OFFSET );
	}
	writel(lp_rtc_en, lp_rtc->regs + LP_RTC_EN);

	return;
}

/*
 * Reset LP_RTC Should Follow Initial Steps In The Design Manual
 */
static void lp_rtc_init(struct lp_rtc_ts *lp_rtc)
{

	// wr_lp_rtc_reg(lp_rtc,0x2, 0x2);
	lp_rtc_open(lp_rtc, true);
	/* step 2 reset procedure */
	wr_lp_rtc_reg(lp_rtc,TEST_REG, 0x0);//reset test register
	wait_us(500);
	wr_lp_rtc_reg(lp_rtc,STATUS_REG, 0x3); // reset status register
	wait_us(62);

	wr_lp_rtc_reg(lp_rtc,INTERRUPT1_REG, 0x0);// reset interrupt1 enable register
	wr_lp_rtc_reg(lp_rtc,INTERRUPT2_REG, 0x0);// reset interrupt2 enable register
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_31_24, 0x0);// reset alarm1 register
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_23_16, 0x0);// reset alarm1 register
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_15_8, 0x0);// reset alarm1 register
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_7_0, 0x0);// reset alarm1 register
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_31_24, 0x0);// reset alarm2 register
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_23_16, 0x0);// reset alarm2 register
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_15_8, 0x0);// reset alarm2 register
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_7_0, 0x0);// reset alarm2 register
	wr_lp_rtc_reg(lp_rtc,CORRECT_REG2, 0x0);// reset correct register
	wr_lp_rtc_reg(lp_rtc,CORRECT_REG1, 0x0);// reset correct register

	wait_us(62);

	wr_lp_rtc_reg(lp_rtc,STATUS_REG, 0x3);// reset status register
	wait_us(52);
}


/* Implements of the rtc_class_ops */
static int ts_lp_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct lp_rtc_ts *lp_rtc = dev_get_drvdata(dev);
    struct rtc_time* tm = &(alrm->time);
	uint8_t   time_buf[4] = {0};
    timeu64_t temp_time;

	pr_debug("get lprtc alarm input:%d %d %d %d %d %d \r\n", tm->tm_year,tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    spin_lock_irq(&lp_rtc->lock);

    time_buf[3] = rd_lp_rtc_reg(lp_rtc, ALARM1_REG_31_24);
    time_buf[2] = rd_lp_rtc_reg(lp_rtc, ALARM1_REG_23_16);
    time_buf[1] = rd_lp_rtc_reg(lp_rtc, ALARM1_REG_15_8);
    time_buf[0] = rd_lp_rtc_reg(lp_rtc, ALARM1_REG_7_0);

    temp_time = (timeu64_t) (time_buf[3]*256*256*256ULL + time_buf[2]*256*256ULL + time_buf[1]*256ULL + time_buf[0]*1ULL);
	rtc_time64_to_tm(temp_time, tm);
	spin_unlock_irq(&lp_rtc->lock);
	
    return 0;
}

static int ts_lp_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct lp_rtc_ts *lp_rtc = dev_get_drvdata(dev);
	uint8_t time_buf[4] = {0};
	//get alarm set value from applay
	struct rtc_time* tm= &(alrm->time);

	timeu64_t time_convd = mktime(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
									   tm->tm_hour, tm->tm_min, tm->tm_sec);
    time_buf[0] = time_convd % 256;
	time_buf[1] = time_convd /256 % 256;
	time_buf[2] = time_convd /256/256 % 256;
	time_buf[3] = time_convd /256/256/256 % 256;

	pr_debug("set lprtc alarm input:%d %d %d %d %d %d \r\n", tm->tm_year,tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	pr_debug("%s  Time_reg:[Dec]%d_%d_%d_%d =%llu(s) \r\n",__func__, time_buf[3],time_buf[2],time_buf[1], time_buf[0],time_convd);

	//step3 applay alarml enable setting
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_31_24, time_buf[3]);
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_23_16, time_buf[2]);
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_15_8, time_buf[1]);
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_7_0, time_buf[0]);//set alarm1 register to 4
	wr_lp_rtc_reg(lp_rtc,INTERRUPT1_REG,  0x1);//(interrrupt 1 register)interrupt 1 enable for alarm
#if 0
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_31_24,time_buf[3]);
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_23_16, time_buf[2]);
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_15_8, time_buf[1]);
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_7_0, time_buf[0]);//set alarm2 register to 8
	wr_lp_rtc_reg(lp_rtc,INTERRUPT2_REG,  0x1);//(interrrupt 2 register) .interrupt 2 enable for alarm 
#endif
	wait_us(62);
	wait_us(62);

	pr_debug("alarm set okay!\r\n");
    return 0;
}

static int ts_lp_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct lp_rtc_ts *lp_rtc = dev_get_drvdata(dev);

	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_31_24, 0x0);
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_23_16, 0x0);
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_15_8, 0x0);
	wr_lp_rtc_reg(lp_rtc,ALARM1_REG_7_0, 0x0);//set alarml register to 0
	wr_lp_rtc_reg(lp_rtc,INTERRUPT1_REG,  0x1);//(interrrupt 1 register)interrupt 1 enable for alarm

	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_31_24 ,0x0);
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_23_16, 0x0);
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_15_8, 0x0);
	wr_lp_rtc_reg(lp_rtc,ALARM2_REG_7_0, 0x0);//set alarm2 register to 0
	wr_lp_rtc_reg(lp_rtc,INTERRUPT2_REG,  0x1);//(interrrupt 2 register) .interrupt 2 enable for alarm
	wait_us(62);
	wait_us(62);

	pr_debug("alarm  enable okay!\r\n");
    return 0;
}

static int ts_lp_rtc_gettime(struct device *dev, struct rtc_time *tm)
{
	struct lp_rtc_ts *rtc = dev_get_drvdata(dev);
	time64_t temp_time = 0;
	uint8_t time_buf[4] = {0};

	spin_lock_irq(&rtc->lock);
	/*Get Raw Time From Low Power RTC*/
	read_time(rtc,time_buf);

	/*Convert Raw Time To Second Time*/
	temp_time = (timeu64_t) (time_buf[3]*256*256*256ULL + time_buf[2]*256*256ULL + time_buf[1]*256ULL + time_buf[0]*1ULL);
	rtc_time64_to_tm(temp_time, tm);

	pr_debug("%s Time_reg:[Dec]%d_%d_%d_%d =%llu(s) \r\n",__func__, time_buf[3],time_buf[2],time_buf[1], time_buf[0],temp_time);
	pr_debug("get kernel input:%d %d %d %d %d %d \r\n", tm->tm_year,tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	spin_unlock_irq(&rtc->lock);

	return 0;
}

static int ts_lp_rtc_settime(struct device *dev, struct rtc_time *tm)
{
	struct lp_rtc_ts *lp_rtc = dev_get_drvdata(dev);
	timeu64_t time_convd = rtc_tm_to_time64(tm);
	uint8_t   time_buf[4] = {0};

	pr_debug("set kernel input:%d %d %d %d %d %d \r\n", tm->tm_year,tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	spin_lock_irq(&lp_rtc->lock);
	/* Time(in secs) = time_reg[7:0]*256^3 + time_reg[15:8]*256^2 + time_reg[23:8]*256^1 + time_reg[24:31]*256^0 */
	time_buf[0] = time_convd % 256;
	time_buf[1] = time_convd /256 %256;
	time_buf[2] = time_convd /256 /256 %256;
	time_buf[3] = time_convd /256 /256 /256;

	pr_debug("%s  Time_reg:[Dec]%d_%d_%d_%d =%llu(s) \r\n",__func__, time_buf[3],time_buf[2],time_buf[1], time_buf[0],time_convd);
	/* write time buffer into register */
	write_time(lp_rtc,time_buf);

	spin_unlock_irq(&lp_rtc->lock);
	return 0;
}

static const struct rtc_class_ops ts_rtc_ops = {
    .read_time    = ts_lp_rtc_gettime,
    .set_time    = ts_lp_rtc_settime,
    .read_alarm   = ts_lp_rtc_getalarm,
    .set_alarm   = ts_lp_rtc_setalarm,
    .alarm_irq_enable  = ts_lp_rtc_alarm_irq_enable
};

static irqreturn_t ts_lp_rtc_interrupt(int irq, void *dev_id)
{
	struct lp_rtc_ts *lp_rtc = (struct lp_rtc_ts *)dev_id;
	int ret = IRQ_HANDLED;

	spin_lock(&lp_rtc->lock);
    
	/* reset status register to clear low power rtc interrupt*/
	wr_lp_rtc_reg(lp_rtc,STATUS_REG,0x1);

    /*
     * lp-rtc alarm interrupt triggled with a 1s pulse with will continusely generate
     * many interrupts. fastly clear interrupt we need to config the alarm time registers 
     * with a value that different with currently time conuter.
     *
     * */
    wr_lp_rtc_reg(lp_rtc,ALARM1_REG_31_24, 0x0);
    wr_lp_rtc_reg(lp_rtc,ALARM1_REG_23_16, 0x0);
    wr_lp_rtc_reg(lp_rtc,ALARM1_REG_15_8, 0x0);
    wr_lp_rtc_reg(lp_rtc,ALARM1_REG_7_0, 0x0);//set alarml register to 0
    
    pr_debug("rtc alarm!\n");
	spin_unlock(&lp_rtc->lock);

	return ret;
}

static int __regoff_init_from_dt(struct platform_device *pdev) 
{
    struct device_node *np = pdev->dev.of_node;
    uint32_t regoff_base;

    if(of_property_read_u32(np, "regoff-base", &regoff_base)) {
        return -ENODATA;
    }

    pr_err("lp rtc smu interface regoff from:%u\n", regoff_base);

    LP_RTC_EN = regoff_base;
    LP_RTC_CTL = regoff_base + 0x4;
    LP_RTC_RDAT = regoff_base + 0x8;
    LP_RTC_CFG_TIM = regoff_base + 0x12;

    return 0;
}

static int ts_lp_rtc_drv_probe(struct platform_device *pdev)
{
	struct resource	*regs;
	struct lp_rtc_ts *ts_lp_rtc;
	int irq;
	int ret;

	ts_lp_rtc = devm_kzalloc(&pdev->dev, sizeof(struct lp_rtc_ts),
			   GFP_KERNEL);
	if (!ts_lp_rtc)
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
	ts_lp_rtc->irq = irq;
	ts_lp_rtc->regs = devm_ioremap_resource(&pdev->dev, regs);
	if (!ts_lp_rtc->regs) {
		dev_dbg(&pdev->dev, "could not map I/O memory\n");
		return -ENOMEM;
	}
	spin_lock_init(&ts_lp_rtc->lock);

    ret = __regoff_init_from_dt(pdev);
    if(ret) {
		dev_dbg(&pdev->dev, "could not got smu reg offset\n");   
    	return -ENOMEM;     
    }

	/*
	 * rtc initial must be executed before rtc_device_register()
	 * because this register action will call ops->read_time
	 * which will read from a uninitialized rtc device then
	 * cause read register busy problem
	 */
	lp_rtc_init(ts_lp_rtc);

	ret = devm_request_irq(&pdev->dev, irq, ts_lp_rtc_interrupt, 0,
				"lp-rtc", ts_lp_rtc);
	if (ret) {
		dev_dbg(&pdev->dev, "could not request irq %d\n", irq);
		return ret;
	}

	platform_set_drvdata(pdev, ts_lp_rtc);

	ts_lp_rtc->rtc = devm_rtc_device_register(&pdev->dev, pdev->name,
                    &ts_rtc_ops, THIS_MODULE);

	if (IS_ERR(ts_lp_rtc->rtc)) {
		dev_dbg(&pdev->dev, "could not register rtc device\n");
		return PTR_ERR(ts_lp_rtc->rtc);
	}

    pr_debug("Probe LP_RTC Successufully at line:%d\r\n",__LINE__);
	return 0;
}

static int ts_lp_rtc_drv_remove(struct platform_device *pdev)
{
    struct lp_rtc_ts * lp_rtc = platform_get_drvdata(pdev);

    devm_rtc_device_unregister(&pdev->dev,lp_rtc->rtc);

    return 0;
}

static const struct of_device_id ts_lp_rtc_of_match[] = {
    {.compatible = "ts,lp_rtc"},
    {/* sentinel */}
};
MODULE_DEVICE_TABLE(of, ts_lp_rtc_of_match);

static struct platform_driver ts_lp_rtc_driver = {
    .probe = ts_lp_rtc_drv_probe,
    .remove = ts_lp_rtc_drv_remove,
    .driver = {
        .name = "ts_lp_rtc",
        .of_match_table = of_match_ptr(ts_lp_rtc_of_match),
    }
};
module_platform_driver(ts_lp_rtc_driver);

MODULE_AUTHOR("tsingmicro");
MODULE_DESCRIPTION("Low power real time clock for Tsingmicro");
MODULE_LICENSE("GPL");

