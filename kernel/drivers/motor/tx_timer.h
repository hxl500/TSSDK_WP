/**
 * @copyright Copyright (c) 2022  Tsingmicro Intelligent Technology Co., Limited
 * @file tx_timer.h
 * @brief
 *
 * @author ymc (yangmengchao@tsingmicro.com)
 * @version 1.0
 * @date 2022-10-14
 */

#ifndef __TX_TIMER_H__
#define __TX_TIMER_H__

#include <linux/platform_device.h>

#define TIMER_CLK 12000000

#define INTNR_TIMER_0      34
#define INTNR_TIMER_1      69
#define INTNR_TIMER_2      70
#define INTNR_TIMER_3      71
#define INTNR_TIMER_4      81

#define TIMER0_REG_BASE    0xf1008000
#define TIMER1_REG_BASE    0xf1008014
#define TIMER2_REG_BASE    0xf1008028
#define TIMER3_REG_BASE    0xf100803c
#define TIMER4_REG_BASE    0xf1008050

#define REG_TIMER_MIS      0x10
#define REG_TIMER_INTCLR   0xc
#define REG_TIMER_CONTROL  0x8
#define REG_TIMER_RELOAD   0x0

#define timer_readl(base, x)        readl((IOMEM(base)+x))
#define timer_writel(base, v, x)    writel(v, ((IOMEM(base)+x)))

typedef int (*timer_handler)(void *data);
void *tx_timer_probe(struct platform_device *pdev, int which_timer, int timer_irq);
int tx_timer_remove(void *priv);
int tx_timer_start(timer_handler handler, void *data, void *priv, int hz);
int tx_timer_stop(void *priv);

#endif
