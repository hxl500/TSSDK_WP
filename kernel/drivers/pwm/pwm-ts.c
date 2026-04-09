#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/export.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/slab.h>

#define PWMCFG		  0x000	 /* Offset: 0x000 (R/W)  PWM configure register */
#define PWMINVERTTRIG 0x004	 /* Offset: 0x004 (R/W)  PWM signal is inverted register */
#define PWM01TRIG	  0x008	 /* Offset: 0x008 (R/W)  contain the trigger generate compare value */
#define PWM23TRIG	  0x00C	 /* Offset: 0x00C (R/W)  contain the trigger generate compare value */
#define PWM45TRIG	  0x010	 /* Offset: 0x010 (N/A)  contain the trigger generate compare value */
#define PWMINTEN1	  0x014	 /* Offset: 0x014 (R/W)  interrupt enable */
#define PWMINTEN2	  0x018	 /* Offset: 0x018 (N/A)  interrupt enable */
#define PWMRIS1 	  0x01C	 /* Offset: 0x01C (R/ )  raw interrupt status */
#define PWMRIS2 	  0x020	 /* Offset: 0x020 (N/A)  raw interrupt status */
#define PWMIC1		  0x024	 /* Offset: 0x024 (R/W)  interrupt clear */
#define PWMIC2		  0x028	 /* Offset: 0x028 (R/W)  interrupt clear */
#define PWMIS1		  0x02C	 /* Offset: 0x02C (R/W)  interrupt status */
#define PWMIS2		  0x030	 /* Offset: 0x030 (R/W)  interrupt status */
#define PWMCTL		  0x034	 /* Offset: 0x034 (R/W)  configure the pwm generation blocks */
#define PWM01LOAD	  0x038	 /* Offset: 0x038 (R/W)  contain the load value of the PWM count */
#define PWM23LOAD	  0x03C	 /* Offset: 0x03C (R/W)  contain the load value of the PWM count */
#define PWM45LOAD	  0x040	 /* Offset: 0x040 (N/A)  contain the load value of the PWM count */
#define PWM01COUNT	  0x044	 /* Offset: 0x044 (R/ )  contain the current value of the PWM count */
#define PWM23COUNT	  0x048	 /* Offset: 0x048 (R/ )  contain the current value of the PWM count */
#define PWM45COUNT	  0x04C	 /* Offset: 0x04C (N/A)  contain the current value of the PWM count */
#define PWM0CMP 	  0x050	 /* Offset: 0x050 (R/W)  contain a value to be compared against the counter */
#define PWM1CMP 	  0x054	 /* Offset: 0x054 (R/W)  contain a value to be compared against the counter */
#define PWM2CMP 	  0x058	 /* Offset: 0x058 (R/W)  contain a value to be compared against the counter */
#define PWM3CMP 	  0x05C	 /* Offset: 0x05C (N/A)  contain a value to be compared against the counter */
#define PWM4CMP 	  0x060	 /* Offset: 0x060 (N/A)  contain a value to be compared against the counter */
#define PWM5CMP 	  0x064	 /* Offset: 0x064 (N/A)  contain a value to be compared against the counter */
#define PWM01DB 	  0x068	 /* Offset: 0x068 (R/W)  contain the number of clock ticks to delay */
#define PWM23DB 	  0x06C	 /* Offset: 0x06C (R/W)  contain the number of clock ticks to delayr */
#define PWM45DB 	  0x070	 /* Offset: 0x070 (N/A)  contain the number of clock ticks to delay */
#define CAPCTL		  0x074	 /* Offset: 0x074 (R/W)  input capture control */
#define CAPINTEN	  0x078	 /* Offset: 0x078 (R/W)  input capture interrupt enable */
#define CAPRIS		  0x07C	 /* Offset: 0x07C (R/ )  input capture raw interrupt status */
#define CAPIC		  0x080	 /* Offset: 0x080 (R/W)  input capture interrupt clear */
#define CAPIS		  0x084	 /* Offset: 0x084 (R/ )  input capture interrupt status */
#define CAP01T		  0x088	 /* Offset: 0x088 (R/ )  input capture count value	*/
#define CAP23T		  0x08C	 /* Offset: 0x08C (R/ )  input capture count value	*/
#define CAP45T		  0x090	 /* Offset: 0x090 (N/A)  input capture count value	*/
#define CAP01MATCH	  0x094	 /* Offset: 0x094 (R/W)  input capture match value	*/
#define CAP23MATCH	  0x098	 /* Offset: 0x098 (R/W)  input capture match value	*/
#define CAP45MATCH	  0x09C	 /* Offset: 0x09C (N/A)  input capture match value	*/
#define TIMINTEN	  0x0A0	 /* Offset: 0x0A0 (R/W)  time interrupt enable */
#define TIMRIS		  0x0A4	 /* Offset: 0x0A4 (R/ )  time raw interrupt stats */
#define TIMIC		  0x0A8	 /* Offset: 0x0A8 (R/W)  time interrupt clear */
#define TIMIS		  0x0AC	 /* Offset: 0x0AC (R/ )  time interrupt status */
#define TIM01LOAD	  0x0B0	 /* Offset: 0x0B0 (R/W)  time load value */
#define TIM23LOAD	  0x0B4	 /* Offset: 0x0B4 (R/W)  time load value */
#define TIM45LOAD	  0x0B8	 /* Offset: 0x0B8 (N/A)  time load value */
#define TIM01COUNT	  0x0BC	 /* Offset: 0x0BC (R/W)  time current count time */
#define TIM23COUNT	  0x0C0	 /* Offset: 0x0C0 (R/W)  time current count time */
#define TIM45COUNT	  0x0C4	 /* Offset: 0x0C4 (R/W)  time current count time */

#define PWM_CFG_CNTDIV_128	(0x6 << 24)
#define PWM_CFG_CNTDIV_64	(0x5 << 24)
#define PWM_CFG_CNTDIV_32	(0x4 << 24)
#define PWM_CFG_CNTDIV_16	(0x3 << 24)
#define PWM_CFG_CNTDIV_8	(0x2 << 24)
#define PWM_CFG_CNTDIV_4	(0x1 << 24)
#define PWM_CFG_CNTDIV_2	(0x0 << 24)
#define PWM_CFG_CNTDIV_EN	(0x1 << 27)

#define PWM_CFG_CNTDIV_Msk_POS	(24)
#define PWM_CFG_CNTDIV_Msk		(0x7 << 24)

#define PWM_PWMCFG_OUTPUT_Msk(ch)			(1U << (ch))
#define PWM_PWMCFG_OUTPUT_ENABLE(ch)		(1U << (ch))
#define PWM_PWMCFG_OUTPUT_DISABLE(ch)		(0U)

#define PWM_PWMCFG_CAPTURE_Msk(ch)			(1U << ((ch) + 12))
#define PWM_PWMCFG_CAPTURE_ENABLE(ch)		(1U << ((ch) + 12))
#define PWM_PWMCFG_CAPTURE_DISABLE(ch)		(0U)

#define PWM_PWMCFG_COMPARE_Msk(ch)			(1U << ((ch) + 18))
#define PWM_PWMCFG_COMPARE_ENABLE(ch)		(1U << ((ch) + 18))
#define PWM_PWMCFG_COMPARE_DISABLE(ch)		(0U)

#define PWM_CAPCTL_EDGE_MODE_Msk(ch)		(1U << (ch))
#define PWM_CAPCTL_EDGE_COUNT(ch)			(1U << (ch))
#define PWM_CAPCTL_EDGE_TIME(ch)			(0U)

#define PWM_CAPCTL_EVENT_MODE_Msk(ch)		(0x3U << ((ch * 2) + 6))
#define PWM_CAPCTL_POSEDGE(ch)				(0U)
#define PWM_CAPCTL_NEGEDGE(ch)				(0x1U << ((ch * 2) + 6))
#define PWM_CAPCTL_BOTH_EDGE(ch)			(0x3U << ((ch * 2) + 6))

#define PWM_CAPINTEN_EDGE_COUNT_IRQ_Msk(ch) (1U << (ch))
#define PWM_CAPINTEN_EDGE_COUNT_IRQ_EN(ch)	(1U << (ch))
#define PWM_CAPINTEN_EDGE_COUNT_IRQ_DIS(ch) (0U)

#define PWM_CAPINTEN_EDGE_TIME_IRQ_Msk(ch)	(1U << ((ch) + 6))
#define PWM_CAPINTEN_EDGE_TIME_IRQ_EN(ch)	(1U << ((ch) + 6))
#define PWM_CAPINTEN_EDGE_TIME_IRQ_DIS(ch)	(0U)

#define PWM_CAPIC_EDGE_COUNT_IRQ_Msk(ch)	(1U << (ch))
#define PWM_CAPIC_EDGE_COUNT_IRQ_CLR(ch)	(1U << (ch))

#define PWM_CAPIC_EDGE_TIME_IRQ_Msk(ch) 	(1U << ((ch) + 6))
#define PWM_CAPIC_EDGE_TIME_IRQ_CLR(ch) 	(1U << ((ch) + 6))

typedef enum {
	CKENUM_PWM_CH0	  = 0,
	CKENUM_PWM_CH1	  = 1,
	CKENUM_PWM_CH2	  = 2,
	CKENUM_PWM_CH3	  = 3,
	CKENUM_PWM_CH4	  = 4,
	CKENUM_PWM_CH5	  = 5,
	CKENUM_PWM_CH6	  = 6,
	CKENUM_PWM_CH7	  = 7,
	CKENUM_PWM_CH8	  = 8,
	CKENUM_PWM_CH9	  = 9,
	CKENUM_PWM_CH10   = 10,
	CKENUM_PWM_CH11   = 11
} CKENUM_PWM_CHANNEL;

typedef enum {
	CKENUM_PWM_COUNT_UP 		 = 0,
	CKENUM_PWM_COUNT_UP_DOWN	 = 1
} CKENUM_PWM_COUNTER_MODE;


typedef enum {
	PWM_CAPTURE_EVENT_COUNT = 0,				   ///< Capture Count Event
	PWM_CAPTURE_EVENT_TIME	= 1,				   ///< Capture Time Event
	PWM_TIMER_EVENT_TIMEOUT = 2 				   ///< Timer Timeout Event
} pwm_event_e;

struct ts_pwm_chip {
	struct pwm_chip chip;
	void __iomem *base;
	int	irq;
	int pwm_channels;
	u32 period_us_max;
	struct clk *clk;
	spinlock_t ts_pwm_spinlock;
	unsigned long rate;
};

static inline struct ts_pwm_chip *to_ts_pwm_chip(struct pwm_chip *chip)
{
	return container_of(chip, struct ts_pwm_chip, chip);
}

static int pwm_ts_request(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct ts_pwm_chip *ts_chip = to_ts_pwm_chip(chip);

	spin_lock_irq(&ts_chip->ts_pwm_spinlock);
	spin_unlock_irq(&ts_chip->ts_pwm_spinlock);

	return 0;
}

static void pwm_ts_free(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct ts_pwm_chip *ts_chip = to_ts_pwm_chip(chip);

	spin_lock_irq(&ts_chip->ts_pwm_spinlock);
	spin_unlock_irq(&ts_chip->ts_pwm_spinlock);
}

static int pwm_ts_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	unsigned int channel = pwm->hwpwm;
	u32 pwmConfig;
	struct ts_pwm_chip *ts_chip = to_ts_pwm_chip(chip);

	spin_lock_irq(&ts_chip->ts_pwm_spinlock);

	pwmConfig = readl(ts_chip->base + PWMCFG);
	if (channel == CKENUM_PWM_CH0 || channel == CKENUM_PWM_CH1) {
		pwmConfig |= 0x00000003;		/* PWM0 output enable */
	}

	if (channel == CKENUM_PWM_CH2 || channel == CKENUM_PWM_CH3) {
		pwmConfig |= 0x0000000C;		/* PWM1 output enable */
	}

	if (channel == CKENUM_PWM_CH4 || channel == CKENUM_PWM_CH5) {
		pwmConfig |= 0x00000030;		/* PWM2 output enable */
	}

	if (channel == CKENUM_PWM_CH6 || channel == CKENUM_PWM_CH7) {
		pwmConfig |= 0x000000C0;		/* PWM3 output enable */
	}

	if (channel == CKENUM_PWM_CH8 || channel == CKENUM_PWM_CH9) {
		pwmConfig |= 0x00000300;		/* PWM4 output enable */
	}

	if (channel == CKENUM_PWM_CH10 || channel == CKENUM_PWM_CH11) {
		pwmConfig |= 0x00000C00;		/* PWM5 output enable */
	}

	writel(pwmConfig, ts_chip->base + PWMCFG);

	spin_unlock_irq(&ts_chip->ts_pwm_spinlock);

	return 0;
}

static int pwm_ts_config_clockdiv(struct ts_pwm_chip *ts_chip, u32 period_us_max)
{
	u32 period_us[7];
	u8 g_div[7]	= {2, 4, 8, 16, 32, 64, 128};
	u8 div_reg[7] = { 0, 1, 2, 3, 4, 5, 6};
	int i;
	u32 pwmConfig;
	u8 max_idx, min_idx, sel_idx;

	pwmConfig = readl(ts_chip->base + PWMCFG);
	pwmConfig &= ~(7 << 24);
	writel(pwmConfig, ts_chip->base + PWMCFG);

	max_idx = min_idx = 0xFF;
	for (i = 0; i < 7; i++) {
		period_us[i] = ts_chip->rate / 1000000;
		period_us[i] = (0xffff * g_div[i]) / period_us[i];
	}

	for (i = 6; i >= 0; i--) {
		if (period_us_max <= period_us[i])
			max_idx = i;
		else
			break;
	}

	if (max_idx == 0xFF) {
		dev_err(ts_chip->chip.dev, "input period %d err.\n", period_us_max);
		return -1;
	}

	sel_idx = max_idx;

	pwmConfig = readl(ts_chip->base + PWMCFG);
	pwmConfig |= PWM_CFG_CNTDIV_EN | (div_reg[sel_idx] << PWM_CFG_CNTDIV_Msk_POS);
	writel(pwmConfig, ts_chip->base + PWMCFG);

	ts_chip->period_us_max = period_us_max;
	return 0;
}

static u32 pwm_ts_get_clockdiv(struct ts_pwm_chip *ts_chip, uint8_t channel)
{
	u32 pwmConfig, div_bits, clockdiv, div_enabled;

	pwmConfig = readl(ts_chip->base + PWMCFG);

	div_enabled = pwmConfig & PWM_CFG_CNTDIV_EN;
	if (!div_enabled)
		return 1;

	div_bits = pwmConfig & PWM_CFG_CNTDIV_Msk;

	switch (div_bits) {
	case PWM_CFG_CNTDIV_2:
		clockdiv = 2;
		break;

	case PWM_CFG_CNTDIV_4:
		clockdiv = 4;
		break;

	case PWM_CFG_CNTDIV_8:
		clockdiv = 8;
		break;

	case PWM_CFG_CNTDIV_16:
		clockdiv = 16;
		break;

	case PWM_CFG_CNTDIV_32:
		clockdiv = 32;
		break;

	case PWM_CFG_CNTDIV_64:
		clockdiv = 64;
		break;

	case PWM_CFG_CNTDIV_128:
		clockdiv = 128;
		break;

	default:
		clockdiv = 1;
	}

	return clockdiv;
}

static void pwm_ts_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	unsigned int channel = pwm->hwpwm;
	u32 pwmConfig;
	struct ts_pwm_chip *ts_chip = to_ts_pwm_chip(chip);

	spin_lock_irq(&ts_chip->ts_pwm_spinlock);

	pwmConfig = readl(ts_chip->base + PWMCFG);
	if (channel == CKENUM_PWM_CH0 || channel == CKENUM_PWM_CH1) {
		pwmConfig &= ~0x00000003;		 /* PWM0 output disable */
	}

	if (channel == CKENUM_PWM_CH2 || channel == CKENUM_PWM_CH3) {
		pwmConfig &= ~0x0000000C;		 /* PWM1 output disable */
	}

	if (channel == CKENUM_PWM_CH4 || channel == CKENUM_PWM_CH5) {
		pwmConfig &= ~0x00000030;		 /* PWM2 output disable */
	}

	if (channel == CKENUM_PWM_CH6 || channel == CKENUM_PWM_CH7) {
		pwmConfig &= ~0x000000C0;		 /* PWM3 output disable */
	}

	if (channel == CKENUM_PWM_CH8 || channel == CKENUM_PWM_CH9) {
		pwmConfig &= ~0x00000300;		 /* PWM4 output disable */
	}

	if (channel == CKENUM_PWM_CH10 || channel == CKENUM_PWM_CH11) {
		pwmConfig &= ~0x00000C00;		 /* PWM5 output disable */
	}

	writel(pwmConfig, ts_chip->base + PWMCFG);

	spin_unlock_irq(&ts_chip->ts_pwm_spinlock);

}

static int pwm_ts_config(struct pwm_chip *chip, struct pwm_device *pwm,
				  int duty_ns, int period_ns)
{
	unsigned int channel = pwm->hwpwm;
	u32 counter, div, cmp_counter, ctl_tmp, temp;
	struct ts_pwm_chip *ts_chip = to_ts_pwm_chip(chip);
	int period_us, duty_us;

	period_us = period_ns / 1000;
	duty_us   = duty_ns / 1000;

	spin_lock_irq(&ts_chip->ts_pwm_spinlock);

	if (period_us > ts_chip->period_us_max) {
		dev_err(ts_chip->chip.dev, "period_ns = %d more than the biggest\n",
								period_ns);
		spin_unlock_irq(&ts_chip->ts_pwm_spinlock);
		return -EINVAL;
	}

	if (duty_ns > period_ns) {
		dev_err(ts_chip->chip.dev, "duty_ns = %d, period_ns = %d error\n",
								duty_ns, period_ns);
		spin_unlock_irq(&ts_chip->ts_pwm_spinlock);
		return -EINVAL;
	}

	div = pwm_ts_get_clockdiv(ts_chip, channel);
	counter = (ts_chip->rate / 1000000 * period_us / div);
	if ((counter > 0xffff)) {
		dev_err(ts_chip->chip.dev, "err, ch%d counter %x, div %d\n", channel, counter, div);
		spin_unlock_irq(&ts_chip->ts_pwm_spinlock);
		return -EINVAL;
	}

	cmp_counter = (ts_chip->rate / 1000000 * duty_us / div);

	ctl_tmp = readl(ts_chip->base + PWMCTL);

	if (channel == CKENUM_PWM_CH0 || channel == CKENUM_PWM_CH1) {
		ctl_tmp &= 0xfffffffe;
		ctl_tmp |= (u32)CKENUM_PWM_COUNT_UP;
		writel(ctl_tmp, ts_chip->base + PWMCTL);

		temp = readl(ts_chip->base + PWM01LOAD);
		temp &= 0xffff0000;
		temp |= counter;
		writel(temp, ts_chip->base + PWM01LOAD);

		temp = readl(ts_chip->base + PWM0CMP);

		if (channel == CKENUM_PWM_CH0) {
			temp &= 0xffff0000;
			temp |= cmp_counter;
			writel(temp, ts_chip->base + PWM0CMP);
		} else {
			temp &= 0x0000ffff;
			temp |= cmp_counter << 16;
			writel(temp, ts_chip->base + PWM0CMP);
		}
	}

	if (channel == CKENUM_PWM_CH2 || channel == CKENUM_PWM_CH3) {
		ctl_tmp &= 0xfffffffd;
		ctl_tmp |= (u32)CKENUM_PWM_COUNT_UP << 1;
		writel(ctl_tmp, ts_chip->base + PWMCTL);

		temp = readl(ts_chip->base + PWM01LOAD);
		temp &= 0x0000ffff;
		temp |= counter << 16;
		writel(temp, ts_chip->base + PWM01LOAD);

		temp = readl(ts_chip->base + PWM1CMP);

		if (channel == CKENUM_PWM_CH2) {
			temp &= 0xffff0000;
			temp |= cmp_counter;
			writel(temp, ts_chip->base + PWM1CMP);
		} else {
			temp &= 0x0000ffff;
			temp |= cmp_counter << 16;
			writel(temp, ts_chip->base + PWM1CMP);
		}
	}

	if (channel == CKENUM_PWM_CH4 || channel == CKENUM_PWM_CH5) {
		ctl_tmp &= 0xfffffffb;
		ctl_tmp |= (u32)CKENUM_PWM_COUNT_UP << 2;
		writel(ctl_tmp, ts_chip->base + PWMCTL);

		temp = readl(ts_chip->base + PWM23LOAD);
		temp &= 0xffff0000;
		temp |= counter;
		writel(temp, ts_chip->base + PWM23LOAD);

		temp = readl(ts_chip->base + PWM2CMP);

		if (channel == CKENUM_PWM_CH4) {
			temp &= 0xffff0000;
			temp |= cmp_counter;
			writel(temp, ts_chip->base + PWM2CMP);
		} else {
			temp &= 0x0000ffff;
			temp |= cmp_counter << 16;
			writel(temp, ts_chip->base + PWM2CMP);
		}
	}

	if (channel == CKENUM_PWM_CH6 || channel == CKENUM_PWM_CH7) {
		ctl_tmp &= 0xfffffff7;
		ctl_tmp |= (u32)CKENUM_PWM_COUNT_UP << 3;
		writel(ctl_tmp, ts_chip->base + PWMCTL);

		temp = readl(ts_chip->base + PWM23LOAD);
		temp &= 0x0000ffff;
		temp |= counter << 16;
		writel(temp, ts_chip->base + PWM23LOAD);

		temp = readl(ts_chip->base + PWM3CMP);

		if (channel == CKENUM_PWM_CH6) {
			temp &= 0xffff0000;
			temp |= cmp_counter;
			writel(temp, ts_chip->base + PWM3CMP);
		} else {
			temp &= 0x0000ffff;
			temp |= cmp_counter << 16;
			writel(temp, ts_chip->base + PWM3CMP);
		}
	}

	if (channel == CKENUM_PWM_CH8 || channel == CKENUM_PWM_CH9) {
		ctl_tmp &= 0xffffffef;
		ctl_tmp |= (u32)CKENUM_PWM_COUNT_UP << 4;
		writel(ctl_tmp, ts_chip->base + PWMCTL);

		temp = readl(ts_chip->base + PWM45LOAD);
		temp &= 0xffff0000;
		temp |= counter;
		writel(temp, ts_chip->base + PWM45LOAD);

		temp = readl(ts_chip->base + PWM4CMP);

		if (channel == CKENUM_PWM_CH8) {
			temp &= 0xffff0000;
			temp |= cmp_counter;
			writel(temp, ts_chip->base + PWM4CMP);
		} else {
			temp &= 0x0000ffff;
			temp |= cmp_counter << 16;
			writel(temp, ts_chip->base + PWM4CMP);
		}
	}

	if (channel == CKENUM_PWM_CH10 || channel == CKENUM_PWM_CH11) {
		ctl_tmp &= 0xffffffdf;
		ctl_tmp |= (u32)CKENUM_PWM_COUNT_UP << 5;
		writel(ctl_tmp, ts_chip->base + PWMCTL);

		temp = readl(ts_chip->base + PWM45LOAD);
		temp &= 0x0000ffff;
		temp |= counter << 16;
		writel(temp, ts_chip->base + PWM45LOAD);

		temp = readl(ts_chip->base + PWM5CMP);

		if (channel == CKENUM_PWM_CH10) {
			temp &= 0xffff0000;
			temp |= cmp_counter;
			writel(temp, ts_chip->base + PWM5CMP);
		} else {
			temp &= 0x0000ffff;
			temp |= cmp_counter << 16;
			writel(temp, ts_chip->base + PWM5CMP);
		}
	}

	spin_unlock_irq(&ts_chip->ts_pwm_spinlock);

	return 0;
}

static int pwm_ts_set_polarity(struct pwm_chip *chip,
					struct pwm_device *pwm,
					enum pwm_polarity polarity)
{
	struct ts_pwm_chip *ts_chip = to_ts_pwm_chip(chip);

	spin_lock_irq(&ts_chip->ts_pwm_spinlock);
	spin_unlock_irq(&ts_chip->ts_pwm_spinlock);

	return 0;
}

static const struct pwm_ops pwm_ts_ops = {
	.request	= pwm_ts_request,
	.free		= pwm_ts_free,
	.enable		= pwm_ts_enable,
	.disable	= pwm_ts_disable,
	.config		= pwm_ts_config,
	.set_polarity	= pwm_ts_set_polarity,
	.owner		= THIS_MODULE,
};

static irqreturn_t pwm_ts_irq_handler(int irq, void *dev_id)
{
	int i;
	u32 timeout;
	u32 capis, timis, timic, irqflag, val, capic;
	struct ts_pwm_chip *ts_chip = (struct ts_pwm_chip *)dev_id;

	spin_lock_irq(&ts_chip->ts_pwm_spinlock);

	capis = readl(ts_chip->base + CAPIS);
	timis = readl(ts_chip->base + TIMIS);

	for (i = 0; i < ts_chip->pwm_channels  / 2; i++) {
		if (timis & (0x1 << i)) {
			timic = readl(ts_chip->base + TIMIC);
			timic |= (0x1 << i);
			writel(timic, ts_chip->base + TIMIC);

			irqflag = PWM_TIMER_EVENT_TIMEOUT;
			continue;
		}

		if (capis & (0x1 << i)) {
			val = 0;
			timeout = 0;

			while (readl(ts_chip->base + CAPIS) & (0x1 << i)) {
				capic = readl(ts_chip->base + CAPIC);
				capic |= (0x1 << i);
				writel(capic, ts_chip->base + CAPIC);

				if (timeout++ > 10000000)
					break;
			}

			irqflag = PWM_CAPTURE_EVENT_COUNT;
			continue;
		}

		if (capis & (0x40 << i)) {
			val = 0;
			timeout = 0;

			while (readl(ts_chip->base + CAPIS) & (0x40 << i)) {
				capic = readl(ts_chip->base + CAPIC);
				capic |= (0x40 << i);
				writel(capic, ts_chip->base + CAPIC);

				if (timeout++ > 10000000)
					break;
			}

			irqflag = PWM_CAPTURE_EVENT_TIME;
			continue;
		}
	}

	spin_unlock_irq(&ts_chip->ts_pwm_spinlock);
	return IRQ_HANDLED;
}

static int pwm_ts_parse_dt(struct platform_device *pdev, struct ts_pwm_chip *ts_chip)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	int ret;

	ret = of_property_read_u32(np, "pwm,channels", &ts_chip->pwm_channels);
	if (ret) {
		dev_err(dev, "read pwm,channels error\n");
		return ret;
	}

	ret = of_property_read_u32(np, "pwm,max_period_us", &ts_chip->period_us_max);
	if (ret) {
		dev_err(dev, "read pwm,period_us_max error\n");
		return ret;
	}

	return 0;
}

static int pwm_ts_hwInit(struct ts_pwm_chip *ts_chip)
{
	return pwm_ts_config_clockdiv(ts_chip, ts_chip->period_us_max);
}

static ssize_t ts_pwm_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct ts_pwm_chip *ts_chip =
		(struct ts_pwm_chip *)dev->driver_data;

	void __iomem *iobase = NULL;
	u32 val;
	u32 i;
	int off = 0;

	if (buf == NULL) {
		dev_err(dev, "buf is NULL\n");
		return 0;
	}

	iobase = ts_chip->base;

	off += sprintf(buf + off, "------dump regs begin------\n");
	for (i = 0; i < 0x0C4; i += 4) {
		val = readl(iobase + i);
		off += sprintf(buf + off, "[%02x] 0x%x\n", i, val);
	}

	off += sprintf(buf + off, "------dump regs end	------\n");

	off += sprintf(buf + off, "ts_chip->period_us_max = %d\n",
					ts_chip->period_us_max);

	return off;

}

static ssize_t ts_pwm_set(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{

	return count;
}

static DEVICE_ATTR(ts_pwm_test, S_IWUSR | S_IRUSR,
			ts_pwm_show, ts_pwm_set);

static struct attribute *ts_pwm_attributes[] = {
	&dev_attr_ts_pwm_test.attr,
	NULL
};

static const struct attribute_group ts_pwm_attr_group = {
	.attrs = ts_pwm_attributes,
};

/* PWM consumed by the tsingmicro vedio device */
static struct pwm_lookup ts_pwm_lookup[] = {
	PWM_LOOKUP("ts_pwm", 0, "0000:00:02.0", "pwm_lcd_backlight", 0, PWM_POLARITY_NORMAL),
};

static int pwm_ts_probe(struct platform_device *pdev)
{
	struct ts_pwm_chip *ts_chip;
	struct resource *res;
	int ret;

	ts_chip = devm_kzalloc(&pdev->dev, sizeof(*ts_chip), GFP_KERNEL);
	if (!ts_chip)
		return -ENOMEM;

	ret = pwm_ts_parse_dt(pdev, ts_chip);
	if (ret)
		return ret;

	ts_chip->chip.dev = &pdev->dev;
	ts_chip->chip.ops = &pwm_ts_ops;
	ts_chip->chip.base = -1;
	ts_chip->chip.npwm = ts_chip->pwm_channels;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "missing IO resource\n");
		return -ENODEV;
	}

	ts_chip->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ts_chip->base))
		return PTR_ERR(ts_chip->base);

	ts_chip->irq = platform_get_irq(pdev, 0);
	if (ts_chip->irq < 0) {
		dev_err(&pdev->dev, "No irq %d in DT.\n", ts_chip->irq);
		return -ENODEV;
	}

	ret = devm_request_irq(&pdev->dev, ts_chip->irq,
				   pwm_ts_irq_handler,
				   IRQF_SHARED, dev_name(&pdev->dev), ts_chip);
	if (ret)
		return -ENODEV;


	ts_chip->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(ts_chip->clk)) {
		dev_err(&pdev->dev, "Cannot claim ts pwm clock.\n");
		return PTR_ERR(ts_chip->clk);
	}

	ret = clk_prepare_enable(ts_chip->clk);
	if (ret) {
		dev_err(&pdev->dev, "Cannot enable ts pwm clock.\n");
		return ret;
	}

	ts_chip->rate = clk_get_rate(ts_chip->clk);
	if (ts_chip->rate == 0) {
		ret = -EINVAL;
		goto out_disable_clk;
	}

	spin_lock_init(&ts_chip->ts_pwm_spinlock);

	ret = pwmchip_add(&ts_chip->chip);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to register PWM chip\n");
		goto out_disable_clk;
	}

	ret = pwm_ts_hwInit(ts_chip);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to pwm_ts_hwInit1\n");
		goto out_disable_clk;
	}
	pwm_add_table(ts_pwm_lookup, ARRAY_SIZE(ts_pwm_lookup));
	platform_set_drvdata(pdev, ts_chip);

	ret = sysfs_create_group(&pdev->dev.kobj, &ts_pwm_attr_group);
	if (ret < 0)
		dev_warn(&pdev->dev, "attr group create failed\n");

	return ret;

out_disable_clk:
	clk_disable_unprepare(ts_chip->clk);
	return ret;
}

static int pwm_ts_remove(struct platform_device *pdev)
{
	struct ts_pwm_chip *ts_chip = platform_get_drvdata(pdev);

	sysfs_remove_group(&pdev->dev.kobj, &ts_pwm_attr_group);
	clk_disable_unprepare(ts_chip->clk);

	return pwmchip_remove(&ts_chip->chip);

}

static const struct of_device_id ts_pwm_matches[] = {
	{ .compatible = "pwm-ts" },
	{ }
};

static struct platform_driver pwm_ts_driver = {
	.driver		= {
		.name	= "ts-pwm",
		.of_match_table = of_match_ptr(ts_pwm_matches),
	},
	.probe		= pwm_ts_probe,
	.remove		= pwm_ts_remove,
};
module_platform_driver(pwm_ts_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ygy");
MODULE_ALIAS("platform:ts-pwm");

