#ifndef __RTC_LP_TS__
#define __RTC_LP_TS__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/rtc.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/irqreturn.h>
#include <asm/io.h> 
#include <linux/time.h>

/* Low Power RTC Was Controled By SMU Module
 * Configing Below Registers(Contained in SMU Module) Will Control 
 * The Parallel Interface Which Attached On Low Power RTC Module
 * This Interface Is A Bright That Could Connect SMU Module And MIN RTC
 * SMU Control Interface To Generate Wrtie/Read Signal Which is Capable to
 * Control The Internal Register Of MIN RTC
 */
#define wait_us  	udelay

/* smu register bit field definition */
#define LP_RTC_EN_OFFSET 0

#define STATUS_REG  		           0x0
#define TEST_REG        		       0x1c
#define INTERRUPT1_REG                 0x04
#define INTERRUPT2_REG  			   0x08
#define ALARM1_REG_7_0  		       0x10
#define ALARM1_REG_15_8                0x11
#define ALARM1_REG_23_16               0x12
#define ALARM1_REG_31_24               0x13
#define ALARM2_REG_7_0  		       0x14
#define ALARM2_REG_15_8                0x15
#define ALARM2_REG_23_16               0x16
#define ALARM2_REG_31_24               0x17
#define TIME_REG_7_0        		   0X0C
#define TIME_REG_15_8                  0X0D
#define TIME_REG_23_16                 0X0E
#define TIME_REG_31_24                 0X0F
#define CORRECT_REG1                   0X18
#define CORRECT_REG2                   0X19

#define TIMEOUT                        20           /*20 MS*/
#define INITIAL_YEAR 		           1970

/* MIN RTC Work Status*/
typedef enum {
	RTC_STATUS_BUSY = 0,
	RTC_STATUS_OK
}rtc_status_e;

/* low power rtc private data struct */
struct lp_rtc_ts{
	struct rtc_device    *rtc;
	void __iomem		 *regs;
	unsigned long		 alarm_time;
	unsigned long		 irq;
	/* Protect against concurrent register access. */
	spinlock_t		     lock;
};

/* default smu private register, get from dts(SoC-Specific) */
static uint16_t LP_RTC_EN;
static uint16_t LP_RTC_CTL;
static uint16_t LP_RTC_RDAT;
static uint16_t LP_RTC_CFG_TIM;

/* LP_RTC_EN   0x118*/
union LP_RTC_EN_U         
{
	struct {
		unsigned int lp_rtc_en:1;
		unsigned int reserved0:3;
		unsigned int chip_pwr_off:1;
		unsigned int reserved1:27;
	};
	unsigned int u32;
};

/*LP_RTC_CTL  0x11c*/
union LP_RTC_CTL_U
{
	struct {
		unsigned int rtc_reg_rd:1;
		unsigned int rtc_reg_wr:1;
		unsigned int reserved0:2;
		unsigned int rtc_reg_addr:5;
		unsigned int reserved1:2;
		unsigned int we_rtc_reg_addr:1;
		unsigned int rtc_reg_wdata:8;
		unsigned int we_rtc_reg_wdata:1;
		unsigned int reserved2:10;
		unsigned int rtc_cfg_busy:1;
	};
	unsigned int u32;
};

/* LP_RTC_RDAT 0x120 */
union LP_RTC_RDAT_U
{
	struct {
		unsigned int rtc_reg_rdata:8;
		unsigned int rtc_rdat_rdy:1;
		unsigned int reserved0:23;
	};
	unsigned int u32;
};

/* LP_RTC_CFG_TIM 0x124 */
union LP_RTC_CFG_TIM_U
{
	struct {
		unsigned int rtc_cfg_clk_cyc:5;
		unsigned int reserved0:3;
		unsigned int tRTC_CLK_H:5;
		unsigned int reserved1:3;
		unsigned int tRTC_CLK_L:5;
		unsigned int reserved2:3;
		unsigned int tRTC_RSTRB:5;
		unsigned int reserved3:3;
	};
	unsigned int u32;
};

typedef struct {
	volatile union LP_RTC_EN_U      lp_rtc_en;
	volatile union LP_RTC_CTL_U     lp_rtc_ctl;
	volatile union LP_RTC_RDAT_U    lp_rtc_rdat;
	volatile union LP_RTC_CFG_TIM_U lp_rtc_cfg_tim;
}lp_rtc_reg_t;

#endif  /*END OF __RTC_LP_TS__ */