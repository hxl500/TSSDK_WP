// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2009-2013 ADVANSEE
 * Benoît Thébaudeau <benoit.thebaudeau@advansee.com>
 *
 * Based on the mpc512x iim code:
 * Copyright 2008 Silicon Turnkey Express, Inc.
 * Martha Marx <mmarx@silicontkx.com>
 */

#include <common.h>
#include <command.h>
#include <console.h>
#include <errno.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <power/pmic.h>
#include <linux/io.h>
#include "tx536_power_test.h"

static struct udevice *currdev;

static int failure(int ret)
{
	printf("Error: %d (%s)\n", ret, errno_str(ret));

	return CMD_RET_FAILURE;
}

static int bind_pmic(void)
{
	char *name;
	int ret = -ENODEV;

	if (currdev)
		return 0;

	name = "ip6103@30";
	ret = pmic_get(name, &currdev);
	if (ret) {
		printf("Can't get PMIC: %s!\n", name);
		return failure(ret);
	}
	printf("dev: %d @ %s\n", currdev->seq, currdev->name);

	return CMD_RET_SUCCESS;
}

static uint8_t rd_lp_rtc_reg(uint8_t addr)
{
	lp_rtc_reg_t lp_rtc_reg;
	uint8_t rd_rdata8;
	int timeout = TIMEOUT;

	lp_rtc_reg.lp_rtc_ctl.u32 = readl(SMU_BASE + LP_RTC_CTL);
	pr_debug("wr wait rtc_cfg_busy %x!", lp_rtc_reg.lp_rtc_ctl.u32);

	while (lp_rtc_reg.lp_rtc_ctl.rtc_cfg_busy != 0x0) {
		if (timeout <= 0) {
			pr_err("rd wait rtc_cfg_busy %x!", lp_rtc_reg.lp_rtc_ctl.u32);
			return -ETIMEDOUT;
		}

		lp_rtc_reg.lp_rtc_ctl.u32 = readl(SMU_BASE + LP_RTC_CTL);

		timeout--;
		udelay(1000);
	}

	lp_rtc_reg.lp_rtc_ctl.rtc_reg_rd = 0x1;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_wr = 0x0;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_addr = addr;
	lp_rtc_reg.lp_rtc_ctl.we_rtc_reg_addr = 0x1;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_wdata = 0x0;
	lp_rtc_reg.lp_rtc_ctl.we_rtc_reg_wdata = 0x0;
	writel(lp_rtc_reg.lp_rtc_ctl.u32, SMU_BASE + LP_RTC_CTL);
	pr_debug("wait smu_lp_rtc_ctl %x!", lp_rtc_reg.lp_rtc_ctl.u32);

	lp_rtc_reg.lp_rtc_rdat.u32 = readl(SMU_BASE + LP_RTC_RDAT);
	timeout = TIMEOUT;
	/* waiting rtc read data be ready */
	while (lp_rtc_reg.lp_rtc_rdat.rtc_rdat_rdy != 0x1) {
		if (timeout <= 0) {
			pr_err("waiting for rtc_rdat ready %x!", lp_rtc_reg.lp_rtc_rdat.u32);
			return -ETIMEDOUT;
		}

		lp_rtc_reg.lp_rtc_rdat.u32 = readl(SMU_BASE + LP_RTC_RDAT);

		udelay(1000);
	}

	rd_rdata8 = lp_rtc_reg.lp_rtc_rdat.rtc_reg_rdata;

	return rd_rdata8;
}

static int wr_lp_rtc_reg(uint8_t addr, uint8_t wdata)
{
	lp_rtc_reg_t lp_rtc_reg;
	int timeout = TIMEOUT;

	/*waiting for lp_rtc be idle*/
	lp_rtc_reg.lp_rtc_ctl.u32 = readl(SMU_BASE + LP_RTC_CTL);
	while (lp_rtc_reg.lp_rtc_ctl.rtc_cfg_busy != 0x0) {
		if (timeout <= 0) {
			pr_err("lp_rtc wr wait rtc_cfg_busy %x!", lp_rtc_reg.lp_rtc_ctl.u32);
			return -ETIMEDOUT;
		}

		lp_rtc_reg.lp_rtc_ctl.u32 = readl(SMU_BASE + LP_RTC_CTL);

		timeout--;
	}

	lp_rtc_reg.lp_rtc_ctl.u32 = readl(SMU_BASE + LP_RTC_CTL);
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_rd = 0x0;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_wr = 0x1;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_addr = addr;
	lp_rtc_reg.lp_rtc_ctl.we_rtc_reg_addr = 0x1;
	lp_rtc_reg.lp_rtc_ctl.rtc_reg_wdata = wdata;
	lp_rtc_reg.lp_rtc_ctl.we_rtc_reg_wdata = 0x1;
	writel(lp_rtc_reg.lp_rtc_ctl.u32, SMU_BASE + LP_RTC_CTL);

	return 0;
}
static void lp_rtc_read_time(void)
{
	uint8_t time[4] = {0};

	wr_lp_rtc_reg(0x00, 0x03);

	/*Time Register Address Grow Up From Index 0 To TIME_REG_LEN */
	time[0] = rd_lp_rtc_reg(TIME_REG_7_0);
	time[1] = rd_lp_rtc_reg(TIME_REG_15_8);
	time[2] = rd_lp_rtc_reg(TIME_REG_23_16);
	time[3] = rd_lp_rtc_reg(TIME_REG_31_24);

	printf("time_reg:[3][2][1][0]=%d_%d_%d_%d\n", time[3], time[2], time[1], time[0]);
}

/*
 * Reset LP_RTC Should Follow Initial Steps In The Design Manual
 */
static void lp_rtc_init(void)
{
	lp_rtc_read_time();
	mdelay(2000);
	lp_rtc_read_time();

	printf("lp_rtc init\n");
	/* enable rtc_pclk_en */
	unsigned int temp = readl(SMU_BASE + RTC_CLK_CFG);

	temp |= 0x3;
	writel(temp, SMU_BASE + RTC_CLK_CFG);

	temp = readl(SMU_BASE + RTC_RST_CTL);
	temp |= 0x6;
	writel(temp, SMU_BASE + RTC_RST_CTL);

	wr_lp_rtc_reg(0x2, 0x2);

	/* step 2 reset procedure */
	wr_lp_rtc_reg(TEST_REG, 0x0);//reset test register
	wait_us(500);
	wait_us(500);
	wr_lp_rtc_reg(TIME_REG_31_24, 0x0);//reset time register
	wr_lp_rtc_reg(TIME_REG_23_16, 0x0);//reset time register
	wr_lp_rtc_reg(TIME_REG_15_8, 0x0);//reset time register
	wr_lp_rtc_reg(TIME_REG_7_0, 0x0); //reset time register
	wait_us(62);
	wait_us(62);
	wr_lp_rtc_reg(STATUS_REG, 0x3); // reset status register
	wait_us(62);
	wait_us(62);

	wr_lp_rtc_reg(INTERRUPT1_REG, 0x0);// reset interrupt1 enable register
	wr_lp_rtc_reg(INTERRUPT2_REG, 0x0);// reset interrupt2 enable register
	wr_lp_rtc_reg(ALARM1_REG_31_24, 0x0);// reset alarm1 register
	wr_lp_rtc_reg(ALARM1_REG_23_16, 0x0);// reset alarm1 register
	wr_lp_rtc_reg(ALARM1_REG_15_8, 0x0);// reset alarm1 register
	wr_lp_rtc_reg(ALARM1_REG_7_0, 0x0);// reset alarm1 register
	wr_lp_rtc_reg(ALARM2_REG_31_24, 0x0);// reset alarm2 register
	wr_lp_rtc_reg(ALARM2_REG_23_16, 0x0);// reset alarm2 register
	wr_lp_rtc_reg(ALARM2_REG_15_8, 0x0);// reset alarm2 register
	wr_lp_rtc_reg(ALARM2_REG_7_0, 0x0);// reset alarm2 register
	wr_lp_rtc_reg(CORRECT_REG2, 0x0);// reset correct register
	wr_lp_rtc_reg(CORRECT_REG1, 0x0);// reset correct register
	wr_lp_rtc_reg(TIME_REG_31_24, 0x0);// reset time register
	wr_lp_rtc_reg(TIME_REG_23_16, 0x0);// reset time register
	wr_lp_rtc_reg(TIME_REG_15_8, 0x0); // reset time register
	wr_lp_rtc_reg(TIME_REG_7_0, 0x0) ;// reset time register

	wait_us(62);
	wait_us(62);
	wr_lp_rtc_reg(STATUS_REG, 0x3);// reset status register
	wait_us(52);
	wait_us(52);

	lp_rtc_read_time();
	mdelay(2000);
	lp_rtc_read_time();
}

static int pmic_comunication_test(void)
{
	uint reg;
	int ret;

	reg = 0x1;
	if (!currdev) {
		printf("First, set the PMIC device!\n");
		return CMD_RET_USAGE;
	}

	ret = pmic_reg_read(currdev, reg);
	if (ret < 0) {
		printf("Can't read PMIC register: %d!\n", reg);
		return failure(ret);
	}

	printf("reg=0x%02x, val=0x%02x", reg, ret);

	return 0;
}

static void set_sgrf(unsigned int exp_data, unsigned char lshift)
{
	unsigned int wdata;
	unsigned int leftdat;

	printf("set sgrf start\n");
	wdata = readl(SGRF_BADDR + 0x54);
	leftdat = exp_data << lshift;
	wdata = wdata | leftdat;
	writel(wdata, SGRF_BADDR + 0x54);
}

#define CX400_POWERDOWN
// #define DDR_POWRDOWN
#define AI_POWERDOWN
#define VPU_POWERDOWN

#ifdef CX400_POWERDOWN
static void cx400_mn_pd(void)
{
	unsigned int temp;

	printf("cx400 pd: start\n");

	temp = readl(SMU_BASE + SMU_CX400_BUS_IF_STAT);
	printf("cx400 pd:rd SMU_CX400_BUS_IF_STAT(0x%x),0x%x\n", SMU_CX400_BUS_IF_STAT, temp);
	while ((temp & 0x3) != 0x3) { //bit1:cx400_dbg_apb_idle  bit0:cx400_sys_ahb_idle  //0x1:idle
		temp = readl(SMU_BASE + SMU_CX400_BUS_IF_STAT);
		printf("cx400 pd:rd SMU_CX400_BUS_IF_STAT,%x\n", temp);
	}

	temp = readl(SMU_BASE + SMU_CX400_BUS_LP_CTL);
	temp |= (0x1 << 0); //cx400_bus_go_idle
	writel(temp, SMU_BASE + SMU_CX400_BUS_LP_CTL);

	udelay(1);
	////check go idle
	temp = readl(SMU_BASE + SMU_CX400_BUS_LP_CTL);
	printf("cx400 pd:rd SMU_CX400_BUS_LP_CTL(0x%x),0x%x\n", SMU_CX400_BUS_LP_CTL, temp);
	while (((temp >> 31) & 0x1) != 0x1) { //cx400_sys_bus_idle 0x0:active  0x1:idle
		temp = readl(SMU_BASE + SMU_CX400_BUS_LP_CTL);
		printf("cx400 pd:rd cx400_sys_bus_idle,%x\n", (temp >> 31) & 0x1);
	}

	temp = readl(SMU_BASE + SMU_CX400_PWR_CTL);
	// temp = ((temp | 0x2)) & (~0x1);
	temp |= (0x1 << 1); //bit1:cx400_go_slp
	temp &= ~(0x1 << 0); //bit0:cx400_wak_up
	writel(temp, SMU_BASE + SMU_CX400_PWR_CTL);

	// udelay(3);
	// temp = readl(SMU_BASE + SMU_CX400_PWR_CTL);
	// while(((temp >> 28) & 0x7) != 0) { //cx400_pwr_stat 0x0:sleep state  0x2: transition to sleep 0x3:work state
	// printf("cx_mpd:cx400_pwr_stat not sleep %x\n",(temp >> 31) & 0x1);
	// }

	temp = readl(SMU_BASE + SMU_CX400_PWR_CTL);
	printf("cx400 pd:rd SMU_CX400_PWR_CTL(0x%x),0x%x\n", SMU_CX400_PWR_CTL, temp);
	printf("cx400 pd: finish\n\n");
}
#endif

#ifdef DDR_POWRDOWN
static void ddr_pd(void)
{
	unsigned int temp;

	printf("ddr pd: start\n");

	temp = readl(SMU_BASE + SMU_DDR_BUS_IF_STAT);
	while ((temp & 0x8000007F) != 0x8000007F) {
		temp = readl(SMU_BASE + SMU_DDR_BUS_IF_STAT);
		printf("ddr pd:rd SMU_DDR_BUS_IF_STAT,%x\n", temp);
	}

	temp = readl(SMU_BASE + SMU_DDR_BUS_LP_CTL);
	temp |= (0x1 << 0); //ddr_bus_go_idle
	writel(temp, SMU_BASE + SMU_DDR_BUS_LP_CTL);

	udelay(1);
	//check go idle
	temp = readl(SMU_BASE + SMU_DDR_BUS_LP_CTL);
	printf("ddr pd:rd ddr_sys_bus_idle,%x\n", (temp >> 31)&0x1);
	while (((temp >> 31)&0x1) != 0x1) { //ddr_sys_bus_idle 0x0:actitive  0x1:idle
		temp = readl(SMU_BASE + SMU_DDR_BUS_LP_CTL);
		printf("ddr pd:rd ddr_sys_bus_idle,%x\n", (temp >> 31)&0x1);
	}

	temp = readl(SMU_BASE + SMU_DDR_PWR_CTL);
	temp |= (0x1 << 0); //ddr_domain_iso_en
	temp |= (0x1 << 1); //ddr_domain_resets
	writel(temp, SMU_BASE + SMU_DDR_PWR_CTL);

	udelay(1);
	temp = readl(SMU_BASE + SMU_DDR_PWR_CTL);
	while ((temp & 0x3) != 0x3) {
		temp = readl(SMU_BASE + SMU_DDR_PWR_CTL);
		printf("ddr pd:rd SMU_DDR_PWR_CTL,%x\n", temp & 0x3);
	}

	temp = readl(SMU_BASE + SMU_DDR_PWR_CTL);
	temp &= ~(0x1 << 31); //ddr_sys_pwr_stat
	writel(temp, SMU_BASE + SMU_DDR_PWR_CTL);

	udelay(1);
	temp = readl(SMU_BASE + SMU_DDR_PWR_CTL);
	while (((temp >> 31) & 0x1) != 0) { //ddr_sys_pwr_stat 0x0:power off  0x1:power supply ready
		printf("ddr pd:ddr_sys_pwr_stat not power off %x\n", (temp >> 31) & 0x1);
	}

	printf("ddr pd finish\n");
	temp = readl(SMU_BASE + SMU_DDR_PWR_CTL);
	printf("ddr pd:rd SMU_DDR_PWR_CTL(0x%x),0x%x\n\n", SMU_DDR_PWR_CTL, temp);
}
#endif

#ifdef AI_POWERDOWN
static void ai_pd(void)
{
	unsigned int temp;

	printf("ai pd:start\n");

	udelay(1);
	temp = readl(SMU_BASE + SMU_AI_BUS_IF_STAT);
	if ((temp >> 31) != 0x1) {
		while ((temp & 0x3F) != 0x3F) {
			temp = readl(SMU_BASE + SMU_AI_BUS_IF_STAT);
			printf("ai pd:rd SMU_AI_BUS_IF_STAT(%x),%x\n", SMU_AI_BUS_IF_STAT, temp);
		}
	}

	temp = readl(SMU_BASE + SMU_AI_BUS_LP_CTL);
	temp |= (0x1 << 0); //ai_bus_go_idle
	writel(temp, SMU_BASE + SMU_AI_BUS_LP_CTL);

	udelay(1);
	//check go idle
	temp = readl(SMU_BASE + SMU_AI_BUS_LP_CTL);
	printf("ai pd:rd ai_sys_bus_idle,%x\n", (temp >> 31)&0x1);
	printf("ai pd:wait ai_sys_bus_idle......%x\n", (temp >> 31)&0x1);
	while (((temp >> 31)&0x1) != 0x1) { //ai_sys_bus_idle 0x0:actitive  0x1:idle
		temp = readl(SMU_BASE + SMU_AI_BUS_LP_CTL);
		printf("ai pd:rd ai_sys_bus_idle,%x\n", (temp >> 31)&0x1);
	}

	temp = readl(SMU_BASE + SMU_AI_PWR_CTL);
	temp |= (0x1 << 0); //ai_domain_iso_en
	temp |= (0x1 << 1); //ai_domain_reset
	writel(temp, SMU_BASE + SMU_AI_PWR_CTL);

	udelay(1);
	temp = readl(SMU_BASE + SMU_AI_PWR_CTL);
	while ((temp & 0x3) != 0x3) {
		temp = readl(SMU_BASE + SMU_AI_PWR_CTL);
		printf("ai pd:rd SMU_AI_PWR_CTL,%x\n", temp & 0x3);
	}

	// ip6103_dcdcx_enable(DCDC2,IP6103_DISABLE);//VDD_AE_0V9

	temp = readl(SMU_BASE + SMU_AI_PWR_CTL);
	temp &= ~(0x1 << 31); //ai_sys_pwr_stat
	writel(temp, SMU_BASE + SMU_AI_PWR_CTL);

	udelay(1);
	temp = readl(SMU_BASE + SMU_AI_PWR_CTL);
	while (((temp >> 31) & 0x1) != 0) { //ai_sys_pwr_stat 0x0:power off  0x1:power supply ready
		printf("ai pd:ai_sys_pwr_stat not power off %x\n", (temp >> 31) & 0x1);
	}

	printf("ai pd finish\n");

	temp = readl(SMU_BASE + SMU_AI_PWR_CTL);
	printf("ai pd:rd SMU_AI_PWR_CTL(0x%x),0x%x\n\n", SMU_AI_PWR_CTL, temp);
}
#endif

#ifdef VPU_POWERDOWN
static void vpu_pd(void)
{
	unsigned int temp;

	printf("vpu pd start\n");

	set_sgrf(0x7, 12);

	udelay(1);
	temp = readl(SMU_BASE + SMU_VPU_BUS_IF_STAT);
	while ((temp & 0x7) != 0x7) {
		temp = readl(SMU_BASE + SMU_VPU_BUS_IF_STAT);
		printf("vpu pd:rd SMU_VPU_BUS_IF_STAT,%x\n", temp);
	}

	temp = readl(SMU_BASE + SMU_VPU_BUS_LP_CTL);
	temp |= (0x1 << 0); //vpu_bus_go_idle
	writel(temp, SMU_BASE + SMU_VPU_BUS_LP_CTL);

	udelay(1);
	//check go idle
	temp = readl(SMU_BASE + SMU_VPU_BUS_LP_CTL);
	printf("vpu pd:rd vpu_sys_bus_idle,%x\n", (temp >> 31)&0x1);
	while (((temp >> 31)&0x1) != 0x1) { //vpu_sys_bus_idle 0x0:actitive  0x1:idle
		temp = readl(SMU_BASE + SMU_VPU_BUS_LP_CTL);
		printf("vpu pd:rd vpu_sys_bus_idle,%x\n", (temp >> 31)&0x1);
	}

	temp = readl(SMU_BASE + SMU_VPU_PWR_CTL);
	temp |= (0x1 << 0); //vpu_domain_iso_en
	temp |= (0x1 << 1); //vpu_domain_reset
	writel(temp, SMU_BASE + SMU_VPU_PWR_CTL);

	udelay(1);
	temp = readl(SMU_BASE + SMU_VPU_PWR_CTL);
	while ((temp & 0x3) != 0x3) {
		temp = readl(SMU_BASE + SMU_VPU_PWR_CTL);
		printf("vpu pd:rd SMU_VPU_PWR_CTL,%x\n", temp & 0x3);
	}

	temp = readl(SMU_BASE + SMU_VPU_PWR_CTL);
	temp &= ~(0x1 << 31); //vpu_sys_pwr_stat
	writel(temp, SMU_BASE + SMU_VPU_PWR_CTL);

	udelay(1);
	temp = readl(SMU_BASE + SMU_VPU_PWR_CTL);
	while (((temp >> 31) & 0x1) != 0) { //vpu_sys_pwr_stat 0x0:power off  0x1:power supply ready
		printf("vpu pd:vpu_sys_pwr_stat not power off %x\n", (temp >> 31) & 0x1);
	}

	printf("vpu pd finish\n");

	temp = readl(SMU_BASE + SMU_VPU_PWR_CTL);
	printf("vpu pd:rd SMU_VPU_PWR_CTL(0x%x),0x%x\n\n", SMU_VPU_PWR_CTL, temp);
}
#endif


static void pmic_test_cpu_enter_wfi(void)
{
	unsigned int temp;

	printf("pmic test cpu enter wfi.\n");

	printf("config SMU_SMU_IRQ_MASK2\n");
	//writel(0, SMU_BASE + SMU_SMU_IRQ_MASK2); //wakup1 need clean
	temp = readl(SMU_BASE + SMU_SMU_IRQ_MASK2);
	temp |= (0x1 << 16);	//rtc_irq_mask;
	temp |= (0x1 << 17);	//smu_irq_mask;
	temp |= (0x1 << 18);	//gic_irq_mask;
	temp |= 0xFF;
	writel(temp, SMU_BASE + SMU_SMU_IRQ_MASK2);

	temp = readl(SMU_BASE + SMU_SMU_IRQ_MASK2);
	printf("SMU_SMU_IRQ_MASK2(0x%x)=0x%x\n", SMU_SMU_IRQ_MASK2, temp);

	printf("config SMU_CHIP_PWR_MODE_CFG\n");
	temp = readl(SMU_BASE + SMU_CHIP_PWR_MODE_CFG);
	temp |= (0x1 << 4);	//auto_MCU_en
	temp |= (0x1 << 5);	//EXT_mode_en
	temp |= (0x1 << 6);	//SLP_mode_en
	temp |= (0x1 << 2);	//bak_to_run
	writel(temp, SMU_BASE + SMU_CHIP_PWR_MODE_CFG);

	//wake mode cfg
	//printf("config SMU_CHIP_WAKE_UP_CFG\n");
	//temp = readl(SMU_BASE + SMU_CHIP_WAKE_UP_CFG);
	//temp &= ~(0x1 << 0);	//wakeup_src0_en=0
	//temp &= ~(0x1 << 1);	//wakeup0_pol=0
	//temp |= (0x1 << 4);	//wakeup_src1_en=0
	//temp |= (0x1 << 5);	//wakeup1_pol=0
	//writel(temp, SMU_BASE + SMU_CHIP_WAKE_UP_CFG);

#ifdef CX400_POWERDOWN
    /*powerdown CX400*/
	temp = readl(SMU_BASE + SMU_CX400_PWR_CTL);
	printf("\nSMU_CX400_PWR_CTL(0x%x)=0x%x\n", SMU_CX400_PWR_CTL, temp);
	if (((temp >> 28) & 0x7) != 0) { //cx_pwr_stat  0x3: work state  0x0:sleep state
		printf("cx_pwr_stat = 0x%x\n", (temp >> 28) & 0x7);
		cx400_mn_pd();
	} else {
		printf("cx_pwr_stat has power off\n\n");
	}
#endif

#ifdef DDR_POWRDOWN
	/*powerdown DDR*/
	temp = readl(SMU_BASE + SMU_DDR_PWR_CTL);
	printf("SMU_DDR_PWR_CTL(0x%x)=0x%x\n", SMU_DDR_PWR_CTL, temp);
	if (((temp >> 31) & 0x1) != 0) { //ddr_sys_pwr_stat 0x0:power off  0x1:power supply ready
		printf("ddr_sys_pwr_stat = 0x%x\n", (temp >> 31) & 0x1);
		ddr_pd();
	} else {
		printf("ddr_sys_pwr_stat has power off\n\n");
	}
#endif

#ifdef AI_POWERDOWN
	/*powerdown AI*/
	temp = readl(SMU_BASE + SMU_AI_PWR_CTL);
	printf("SMU_AI_PWR_CTL(0x%x)=0x%x\n", SMU_AI_PWR_CTL, temp);
	if (((temp >> 31) & 0x1) != 0) { //ai_sys_pwr_stat 0x0:power off  0x1:power supply ready
		printf("ai_sys_pwr_stat = 0x%x\n", (temp >> 31) & 0x1);
		ai_pd();
	} else {
		printf("ai_sys_pwr_stat has power off\n\n");
	}
#endif

#ifdef VPU_POWERDOWN
	/*powerdown VPU*/
	temp = readl(SMU_BASE + SMU_VPU_PWR_CTL);
	printf("SMU_VPU_PWR_CTL(0x%x)=0x%x\n", SMU_VPU_PWR_CTL, temp);
	if (((temp >> 31) & 0x1) != 0) { //vpu_sys_pwr_stat 0x0:power off  0x1:power supply ready
		printf("vpu_sys_pwr_stat = 0x%x\n", (temp >> 31) & 0x1);
		vpu_pd();
	} else {
		printf("vpu_sys_pwr_stat has power off\n\n");
	}
#endif

	temp = readl(SMU_BASE + SMU_A53_CPU0_PWR_CTL);
	temp &= ~(0x1 << 0);	//a53_cpu0_pwr_up
	temp &= ~(0x1 << 1);	//a53_cpu0_pwr_dwn
	temp |= (0x1 << 4);		//cpu0_auto_PD_en
	writel(temp, SMU_BASE + SMU_A53_CPU0_PWR_CTL);

	temp = readl(SMU_BASE + SMU_A53_CPU1_PWR_CTL);
	temp &= ~(0x1 << 0);	//a53_cpu1_pwr_up
	temp &= ~(0x1 << 1);	//a53_cpu1_pwr_dwn
	temp |= (0x1 << 4);		//cpu1_auto_PD_en
	writel(temp, SMU_BASE + SMU_A53_CPU1_PWR_CTL);

	temp = readl(SMU_BASE + SMU_A53_SCU_PWR_CTL);
	temp &= ~(0x1 << 0);	//a53_scu_pwr_up
	temp &= ~(0x1 << 1);	//a53_scu_pwr_dwn
	temp |= (0x1 << 4);		//scu_auto_PD_en
	writel(temp, SMU_BASE + SMU_A53_SCU_PWR_CTL);

	// temp = readl(SMU_BASE + SMU_CPU_BOOT_RST_CTL);
	// temp |= (0x3 << 16);
	// writel(temp, SMU_BASE + SMU_CPU_BOOT_RST_CTL);
	set_sgrf(0x7f, 15);

	printf("core0 enter wfi\n");

	printf("CX400 AI VPU A53 have power down,please exe ck804 now.\n");
	__asm("wfi;");
}

int power_test_config_for_lprtc_mode(void)
{
	int ret;
	unsigned int adr;
	unsigned char val;

	printf("power test config for lprtc mode\n");

	/***init lp_rtc***/
	lp_rtc_init();

	/****close all power****/
	//set key press time
	adr = IP6103_CONTROL_2;
	ret = pmic_read(currdev, adr, &val, 1);
	val |= (0x3 << 6);
	ret = pmic_write(currdev, adr, &val, 1);

	//set long press is only wakeup source
	adr = IP6103_CONTROL_1;
	val = (0x1 << 5);
	ret = pmic_write(currdev, adr, &val, 1);

	//set dcdc whether keep on when light sleep
	adr = IP6103_CONTROL_7;
	ret = pmic_read(currdev, adr, &val, 1);
	val &= ~(1 << 0);	//DC0
	val &= ~(1 << 1);	//DC1
	val &= ~(1 << 2);	//DC2
	val &= ~(1 << 3);	//DC3
	ret = pmic_write(currdev, adr, &val, 1);

	//set ldo whether keep on when light sleep
	adr = IP6103_CONTROL_8;
	ret = pmic_read(currdev, adr, &val, 1);
	val &= ~(1 << 0);	//LDO0
	val &= ~(1 << 1);	//LDO1
	val &= ~(1 << 2);	//LDO2
	val &= ~(1 << 4);	//LDO4
	val &= ~(1 << 5);	//LDO5
	val &= ~(1 << 6);	//LDO6
	ret = pmic_write(currdev, adr, &val, 1);

	//set light sleep
	adr = IP6103_CONTROL_8;
	ret = pmic_read(currdev, adr, &val, 1);
	val |= (1 << 7);
	ret = pmic_write(currdev, adr, &val, 1);

	//enter sleep
	printf("enter sleep,close all ldo and dc\n");
	adr = IP6103_CONTROL_2;
	ret = pmic_read(currdev, adr, &val, 1);
	val &= ~(1 << 0);
	ret = pmic_write(currdev, adr, &val, 1);
	printf("after enter sleep\n");

	return 0;
}

#define ENTER_EXT_WITHOUT_LPRTC 1
#define ENTER_EXT_WITH_LPRTC 2
#define ENTER_MCU_WITH_LPRTC 3
#define ENTER_SLEEP_WITH_LPRTC 4

//smu_usr_grp record value for ck804 read
static int power_test_set_smu_usr_grp0(uint32_t value)
{
	writel(value, SMU_BASE + SMU_SMU_USR_GRP0);

	return 0;
}

static int power_test_config_for_ext1(void)
{
	printf("power test config for ext1\n");

	power_test_set_smu_usr_grp0(ENTER_EXT_WITHOUT_LPRTC);
	/*lp_rtc not work*/

	pmic_test_cpu_enter_wfi();

	return 0;
}

static int power_test_config_for_ext2(void)
{
	printf("power test config for ext2\n");

	power_test_set_smu_usr_grp0(ENTER_EXT_WITH_LPRTC);

	/*init lp_rtc,lp_rtc work*/
	lp_rtc_init();

	pmic_test_cpu_enter_wfi();

	return 0;
}

static int power_test_config_for_mcu_with_rtc(void)
{
	printf("power test config for ext2\n");

	power_test_set_smu_usr_grp0(ENTER_MCU_WITH_LPRTC);

	/*init lp_rtc,lp_rtc work*/
	lp_rtc_init();

	pmic_test_cpu_enter_wfi();

	return 0;
}

static int power_test_config_for_quiescent(void)
{
	printf(" need not config in software.\n");

	return 0;
}

static int do_power_test_config(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	const char *op = argc >= 2 ? argv[1] : NULL;

	if (argc < 2)
		return CMD_RET_USAGE;

	bind_pmic();

	if (!strcmp(op, "lprtc")) {
		power_test_config_for_lprtc_mode();
		putc('\n');
	} else if (!strcmp(op, "ext1")) {
		power_test_config_for_ext1();
		putc('\n');
	} else if (!strcmp(op, "ext2")) {
		power_test_config_for_ext2();
		putc('\n');
	} else if (!strcmp(op, "mcu_with_rtc")) {
		power_test_config_for_mcu_with_rtc();
		putc('\n');
	} else if (!strcmp(op, "quiescent")) {
		power_test_config_for_quiescent();
		putc('\n');
	} else if (!strcmp(op, "pmic")) {
		pmic_comunication_test();
		putc('\n');
	} else if (!strcmp(op, "wfi")) {
		pmic_test_cpu_enter_wfi();
		putc('\n');
	} else {
		return CMD_RET_USAGE;
	}

	return 0;
}

U_BOOT_CMD(
	powertest, CONFIG_SYS_MAXARGS, 0, do_power_test_config,
	"powertest sub-system",
	"lprtc  - config for test lprtc mode.\n"
	"powertest ext1   - config for test ext1 mode,ext mode,rtc not work.\n"
	"                 - enter ext mode also need config in ck804!!\n"
	"powertest ext2   - config for test ext2 mode,ext mode,rtc work\n"
	"                 - enter ext mode also need config in ck804!!\n"
	"powertest mcu_with_rtc   - config for test mcu mode,rtc work\n"
	"                 - enter mcu mode also need config in ck804!!\n"
	"powertest quiescent   - need not config in software.\n"
	"powertest wfi -config A53 into wfi mode,and powerdown,then need exeute ck804.\n"
	"powertest pmic -test pmic comunication."
);
