#ifndef _TX536_POWER_TEST_H
#define _TX536_POWER_TEST_H

#include <linux/delay.h>

#define IP6103_CONTROL_0           0x00
#define IP6103_CONTROL_1           0x01
#define IP6103_CONTROL_2           0x02
#define IP6103_CONTROL_3           0x03
#define IP6103_CONTROL_4           0x04
#define IP6103_CONTROL_5           0x05
#define IP6103_CONTROL_6           0x06
#define IP6103_CONTROL_7           0x07
#define IP6103_CONTROL_8           0x08
#define IP6103_CONTROL_9           0x11

#define IP6103_DCDC_CTL            0x20    /*DCDC Control*/
#define IP6103_DCDC0_0             0x21    /*DCDC0 Voltage Setting*/
#define IP6103_DCDC0_1             0x21    /*DCDC0 Stepwize Interval*/
#define IP6103_DCDC1_0             0x28    /*DCDC1 Voltage Setting*/
#define IP6103_DCDC1_1             0x29    /*DCDC1 Stepwize Interval*/
#define IP6103_DCDC2_0             0x2F    /*DCDC2 Voltage Setting*/
#define IP6103_DCDC2_1             0x30    /*DCDC2 Stepwize Interval*/
#define IP6103_DCDC3_0             0x36    /*DCDC3 Voltage Setting*/
#define IP6103_DCDC3_1             0x37    /*DCDC3 Stepwize Interval*/

#define IP6103_SW_LDO_CTL_0        0x40
#define IP6103_SW_LDO_CTL_1        0x41    /*LDOx Enable*/
#define IP6103_LDO0                0x42    /*LDO0 Voltage Setting*/
#define IP6103_LDO1                0x44    /*LDO1 Voltage Setting*/
#define IP6103_LDO2                0x46    /*LDO2 Voltage Setting*/
#define IP6103_LDO4                0x4A    /*LDO4 Voltage Setting*/
#define IP6103_LDO5                0x4C    /*LDO5 Voltage Setting*/
#define IP6103_LDO6                0x4E    /*LDO6 Voltage Setting*/


/*********************************************/
#define SMU_BASE 0xf12a2000
#define wait_us udelay
#define LP_RTC_EN                      0x118
#define LP_RTC_CFG_TIM                 0x124
#define RTC_RST_CTL                    0X0F4
#define LP_RTC_CTL                     0x11C
#define LP_RTC_RDAT                    0x120
#define RTC_CLK_CFG                    0X0F0

#define STATUS_REG                 0x0
#define TEST_REG                   0x1c
#define INTERRUPT1_REG             0x04
#define INTERRUPT2_REG             0x08
#define ALARM1_REG_7_0             0x10
#define ALARM1_REG_15_8            0x11
#define ALARM1_REG_23_16           0x12
#define ALARM1_REG_31_24           0x13
#define ALARM2_REG_7_0             0x14
#define ALARM2_REG_15_8            0x15
#define ALARM2_REG_23_16           0x16
#define ALARM2_REG_31_24           0x17
#define TIME_REG_7_0               0X0C
#define TIME_REG_15_8              0X0D
#define TIME_REG_23_16             0X0E
#define TIME_REG_31_24             0X0F
#define CORRECT_REG1               0X18
#define CORRECT_REG2               0X19

#define TIMEOUT                        20           /*20 MS*/
#define INITIAL_YEAR                1970

/* MIN RTC Work Status*/
typedef enum {
	RTC_STATUS_BUSY = 0,
	RTC_STATUS_OK
} rtc_status_e;

/* low power rtc private data struct */
struct lp_rtc_ts {
	struct rtc_device	*rtc;
	void __iomem		*regs;
	unsigned long		alarm_time;
	unsigned long		irq;
	/* Protect against concurrent register access. */
	// spinlock_t		lock;
};

/* LP_RTC_EN   0x118*/
union LP_RTC_EN_U {
	struct {
		unsigned int lp_rtc_en:1;
		unsigned int reserved0:3;
		unsigned int chip_pwr_off:1;
		unsigned int reserved1:27;
	};
	unsigned int u32;
};

/*LP_RTC_CTL  0x11c*/
union LP_RTC_CTL_U {
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
union LP_RTC_RDAT_U {
	struct {
		unsigned int rtc_reg_rdata:8;
		unsigned int rtc_rdat_rdy:1;
		unsigned int reserved0:23;
	};
	unsigned int u32;
};

/* LP_RTC_CFG_TIM 0x124 */
union LP_RTC_CFG_TIM_U {
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
	union LP_RTC_EN_U lp_rtc_en;
	union LP_RTC_CTL_U lp_rtc_ctl;
	union LP_RTC_RDAT_U lp_rtc_rdat;
	union LP_RTC_CFG_TIM_U lp_rtc_cfg_tim;
} lp_rtc_reg_t;

/********************************************************************/
#define SMU_SMU_IRQ_MASK2	0x18
#define SMU_CHIP_PWR_MODE_CFG	0x30
#define SMU_CPU_BOOT_RST_CTL	0x34

#define SMU_CHIP_WAKE_UP_CFG	0xE0
#define SMU_CX400_PWR_CTL		0xC0
#define SMU_CX400_BUS_LP_CTL	0xCC
#define SMU_CX400_BUS_IF_STAT	0xC8
#define SMU_DDR_PWR_CTL			0x40
#define SMU_DDR_BUS_IF_STAT		0x44
#define SMU_DDR_BUS_LP_CTL		0x48

#define SMU_A53_CPU0_PWR_CTL	0xA0
#define SMU_A53_CPU1_PWR_CTL	0xA4
#define SMU_A53_SCU_PWR_CTL		0xA8

#define SMU_AI_PWR_CTL			0x50
#define SMU_AI_BUS_IF_STAT		0x54
#define SMU_AI_BUS_LP_CTL		0x58

#define SMU_VPU_PWR_CTL			0x60
#define SMU_VPU_BUS_IF_STAT		0x64
#define SMU_VPU_BUS_LP_CTL		0x68

#define SMU_SMU_USR_GRP0		0x100
#define SMU_SMU_USR_GRP1		0x104
#define SMU_SMU_USR_GRP2		0x108
#define SMU_SMU_USR_GRP3		0x10C

#define SGRF_BADDR	0xF128D000
#endif
