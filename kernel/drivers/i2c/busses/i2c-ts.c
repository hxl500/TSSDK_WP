// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Tsingmicro Co., Ltd.
 * Copyright 2022 Linaro Ltd.
 *
 * Author: Shi yc <yc.shi@tsingmicro.org>
 */
// #define DEBUG
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>


#define I2C_FIFO_MAX			16

#define I2C_TIMEOUT			msecs_to_jiffies(1000)

#define DEV(i2c)			((i2c)->adap.dev.parent)

#define I2C_WFIFO_RESET			BIT(2)
#define I2C_RFIFO_RESET			BIT(3)

#define USI_FIFO_STA_TX_EMPTY		BIT(0)
#define USI_FIFO_STA_TX_FULL		BIT(1)
#define USI_FIFO_STA_RX_EMPTY		BIT(2)
#define USI_FIFO_STA_RX_FULL		BIT(3)

#define USI_EN				0x000
#define USI_MODE_SEL		0x004
#define USI_TX_RX_FIFO		0x008
#define USI_FIFO_STA		0x00c
#define USI_CLK_DIV0		0x010
#define USI_CLK_DIV1		0x014
#define USI_UART_CTRL		0x018
#define USI_UART_STA		0x01c
#define USI_I2C_MODE		0x020
#define USI_I2C_ADDR		0x024
#define USI_I2CM_CTRL		0x028
#define USI_I2CM_CODE		0x02c
#define USI_I2CS_CTRL		0x030
#define USI_I2C_FM_DIV		0x034
#define USI_I2C_HOLD		0x038
#define USI_I2C_STA			0x03c
#define USI_SPI_MODE		0x040
#define USI_SPI_CTRL		0x044
#define USI_SPI_STA			0x048
#define USI_INTR_CTRL		0x04c
#define USI_INTR_EN			0x050
#define USI_INTR_STA		0x054
#define USI_RAW_INTR_STA	0x058
#define USI_INTR_UNMASK		0x05c
#define USI_INTR_CLR		0x060
#define USI_DMA_CTRL		0x064
#define USI_DMA_THRESHOLD	0x068

#define USI_INT_SPI_STOP		(0x1 << 17)
#define USI_INT_I2C_AERR		(0x1 << 16)
#define USI_INT_I2CS_GCALL		(0x1 << 15)
#define USI_INT_I2CM_LOSE_ARBI	(0x1 << 14)
#define USI_INT_I2C_NACK		(0x1 << 13)
#define USI_INT_I2C_STOP		(0x1 << 12)
#define USI_INT_UART_PERR		(0x1 << 11)
#define USI_INT_UART_STOP		(0x1 << 10)
#define USI_INT_RX_WERR			(0x1 << 9)
#define USI_INT_RX_RERR			(0x1 << 8)
#define USI_INT_RX_FULL			(0x1 << 7)
#define USI_INT_RX_EMPTY		(0x1 << 6)
#define USI_INT_RX_THOLD		(0x1 << 5)
#define USI_INT_TX_WERR			(0x1 << 4)
#define USI_INT_TX_RERR			(0x1 << 3)
#define USI_INT_TX_FULL			(0x1 << 2)
#define USI_INT_TX_EMPTY		(0x1 << 1)
#define USI_INT_TX_THOLD		(0x1 << 0)

#define USI_MODE_UART		0x0
#define USI_MODE_I2C		0x1
#define USI_MODE_SPI		0x2

#define CK_USI_I2C_MODE_SLAVE   0x00
#define CK_USI_I2C_MODE_MATER   0x01

#define USI_I2CM_CTRL_HS_MODE        0x04

#define USI_I2CM_CTRL_ADDR_MODE      0x1e
#define USI_I2CM_CTRL_ADDR7_MODE     0x00
#define USI_I2CM_CTRL_ADDR10_MODE    0x01

#define USI_I2CM_CTRL_SBYTE_EN       0x08

#define USI_I2C_STA_SLAVE_WORK       0x100
#define USI_I2C_STA_MASTER_WORK      0x1

enum ENUM_IIC_SPEED {
	IIC_STANDARDSPEED	= 100000,
	IIC_FASTSPEED		= 400000,
	IIC_HIGHSPEED		= 3400000
};

#define I2C_MODE_STANDARD	0
#define I2C_MODE_FAST		1

#define USI_INTR_CTRL_TH_MODE		(1 << 16)
#define USI_INTR_CTRL_RXFIFO_TH		0xfffffcff
#define USI_INTR_CTRL_RXFIFO_TH_4	(0x1 << 8)  // 4-byte data
#define USI_INTR_CTRL_RXFIFO_TH_8	(0x2 << 8)  // 8-byte data
#define USI_INTR_CTRL_RXFIFO_TH_12	(0x3 << 8)  // 12-byte data
#define USI_INTR_CTRL_TXFIFO_TH		0xfffffffc
#define USI_INTR_CTRL_TXFIFO_TH_4	(0x1 << 0)  // 4-byte data
#define USI_INTR_CTRL_TXFIFO_TH_8	(0x2 << 0)  // 8-byte data
#define USI_INTR_CTRL_TXFIFO_TH_12	(0x3 << 0)  // 12-byte data

#define USI_TX_MAX_FIFO		0x10
#define USI_RX_MAX_FIFO		0x10

#define TS_I2C_SLAVE_ADDR_MASK	0x3ff

#define TS_I2C_TIMEOUT		(msecs_to_jiffies(10000))

struct ts_i2c {
	struct i2c_adapter	adap;
	struct clk		*clk;
	struct completion	complete;
	u32			clk_freq;
	void __iomem		*reg_base;
	size_t			residue;
	int			irq;
	int			msg_rd;
	u8			*cur_trans;
	int			error;
};

static void ts_i2c_writel(struct ts_i2c *i2c,
			      u32 val, unsigned long reg)
{
	writel(val, i2c->reg_base + reg);
}

static u32 ts_i2c_readl(struct ts_i2c *i2c, unsigned long reg)
{
	return readl(i2c->reg_base + reg);
}

static void ts_i2c_writesb(struct ts_i2c *i2c,
			       void *data, unsigned long reg, int len)
{
	unsigned int count = len;

	if (count) {
		const u8 *buf = data;

		do {
			writel(*buf++, i2c->reg_base + reg);
		} while (--count);
	}
}

static void ts_i2c_readsb(struct ts_i2c *i2c,
			      void *data, unsigned long reg, int len)
{
	unsigned int count = len;

	if (count) {
		u8 *buf = data;

		do {
			u8 x = readl(i2c->reg_base + reg);
			*buf++ = x;
		} while (--count);
	}
}

void ts_i2c_set_rxfifo_th(struct ts_i2c *i2c, u32 length)
{
	u32 val;

	val = ts_i2c_readl(i2c, USI_INTR_CTRL);
	ts_i2c_writel(i2c, val & (~USI_INTR_CTRL_TH_MODE),
				 USI_INTR_CTRL);
	val = ts_i2c_readl(i2c, USI_INTR_CTRL);
	ts_i2c_writel(i2c, val & USI_INTR_CTRL_RXFIFO_TH,
				 USI_INTR_CTRL);
	val = ts_i2c_readl(i2c, USI_INTR_CTRL);

	if (length >= USI_RX_MAX_FIFO) {
		ts_i2c_writel(i2c, val | USI_INTR_CTRL_RXFIFO_TH_12 |
			 USI_INTR_CTRL_TH_MODE, USI_INTR_CTRL);
	} else if (length >= USI_RX_MAX_FIFO - 4) {
		ts_i2c_writel(i2c, val | USI_INTR_CTRL_RXFIFO_TH_8 |
			 USI_INTR_CTRL_TH_MODE, USI_INTR_CTRL);
	} else if (length >= 4) {
		ts_i2c_writel(i2c, val | USI_INTR_CTRL_RXFIFO_TH_4 |
			 USI_INTR_CTRL_TH_MODE, USI_INTR_CTRL);
	} else {
		ts_i2c_writel(i2c, val | USI_INTR_CTRL_RXFIFO_TH_4,
			 USI_INTR_CTRL);
	}
	val = ts_i2c_readl(i2c, USI_INTR_CTRL);
	pr_debug("%s:%d, length:0x%x, val:0x%x\n", __func__, __LINE__, length, val);

}

static void ts_i2c_rx_start_ctrl(struct ts_i2c *i2c)
{
	u32 val;

	pr_debug("%s:%d\n", __func__, __LINE__);
	/*diable usi en*/
	ts_i2c_writel(i2c, 0, USI_EN);
	ts_i2c_writel(i2c, 0, USI_INTR_EN);

	/*config module and enable inter mask*/
	ts_i2c_writel(i2c, USI_INT_I2C_STOP | USI_INT_RX_THOLD,
		 USI_INTR_UNMASK);
	ts_i2c_writel(i2c, 0xf, USI_EN);

	/*low than 4 create inter*/
	ts_i2c_set_rxfifo_th(i2c, 1);
	/*config scl low*/
	val = ts_i2c_readl(i2c, USI_I2CM_CTRL);
	ts_i2c_writel(i2c, val & ~(1 << 1), USI_I2CM_CTRL);

	//recv restart
	ts_i2c_writel(i2c, 3 << 8, USI_TX_RX_FIFO);

	ts_i2c_writel(i2c, USI_INT_RX_THOLD, USI_INTR_EN);
}

static void ts_i2c_tx_start_ctrl(struct ts_i2c *i2c)
{
	u32 val;
	u8 cmd;

	pr_debug("%s:%d\n", __func__, __LINE__);
	/*diasble usi module and inter en*/
	ts_i2c_writel(i2c, 0, USI_EN);
	ts_i2c_writel(i2c, 0, USI_INTR_EN);

	/*config module and enable inter mask*/
	ts_i2c_writel(i2c, USI_INT_I2C_STOP | USI_INT_TX_EMPTY |
			 USI_INT_I2C_NACK, USI_INTR_UNMASK);
	ts_i2c_writel(i2c, 0xf, USI_EN);

	/*config scl low*/
	val = ts_i2c_readl(i2c, USI_I2CM_CTRL);
	ts_i2c_writel(i2c, val & ~(1 << 1), USI_I2CM_CTRL);
	ts_i2c_writel(i2c, 5, USI_I2C_HOLD);

	//send start
	cmd = *i2c->cur_trans;
	ts_i2c_writel(i2c, cmd | 0x200, USI_TX_RX_FIFO);
	i2c->cur_trans++;
	i2c->residue--;

	/*config inter enable*/
	ts_i2c_writel(i2c, USI_INT_I2C_STOP | USI_INT_TX_EMPTY
			 | USI_INT_I2C_NACK, USI_INTR_EN);

}

static void ts_i2c_flush_fifos(struct ts_i2c *i2c)
{
	u32 val, tmp;

	val = ts_i2c_readl(i2c, USI_EN);

	if (i2c->msg_rd)
		tmp = I2C_RFIFO_RESET;
	else
		tmp = I2C_WFIFO_RESET;

	ts_i2c_writel(i2c, val & ~tmp, USI_EN);
	val = ts_i2c_readl(i2c, USI_EN);
	ts_i2c_writel(i2c, val | tmp, USI_EN);
}

static int ts_i2c_empty_rx_fifo(struct ts_i2c *i2c, u32 status)
{
	u8 tmp[I2C_FIFO_MAX] = {0};
	int i;
	u32 num, val, emptyfifo;

	if (status & USI_INT_RX_THOLD) {
		num = ts_i2c_readl(i2c, USI_FIFO_STA);
		pr_debug("%s:%d, USI_FIFO_STA:0x%x\n", __func__, __LINE__, num);
		num &= (0x1F << 16);
		num >>= 16;
		ts_i2c_readsb(i2c, tmp, USI_TX_RX_FIFO, num);
		num = num > i2c->residue ? i2c->residue : num;
		for (i = 0; i < num; i++)
			*i2c->cur_trans++ = tmp[i];

		i2c->residue -= num;
		pr_debug("%s:%d, num:0x%x, residue:%zu\n", __func__, __LINE__, num, i2c->residue);

		if (i2c->residue != 0) {
			num =  ts_i2c_readl(i2c, USI_FIFO_STA);
			num &= (0x1F << 8);
			num >>= 8;

			emptyfifo = (i2c->residue > (I2C_FIFO_MAX - num)) ?
						(I2C_FIFO_MAX - num) : i2c->residue;

			for (i = 0; i < emptyfifo; i++)
				ts_i2c_writel(i2c, 0x100, USI_TX_RX_FIFO);

			ts_i2c_set_rxfifo_th(i2c, emptyfifo);
		} else {
			val = ts_i2c_readl(i2c, USI_INTR_EN);
			ts_i2c_writel(i2c, val & ~USI_INT_RX_THOLD, USI_INTR_EN);
			val = ts_i2c_readl(i2c, USI_INTR_EN);
			ts_i2c_writel(i2c, val | USI_INT_I2C_STOP,
					USI_INTR_EN);
			val = ts_i2c_readl(i2c, USI_I2CM_CTRL);
			/*create stop*/
			ts_i2c_writel(i2c, val | (1 << 1), USI_I2CM_CTRL);
		}
	}

	if ((status & USI_INT_I2C_STOP) || (status & USI_INT_RX_WERR)
		 || (status & USI_INT_RX_RERR)) {
		val = ts_i2c_readl(i2c, USI_INTR_EN);
		ts_i2c_writel(i2c, val & ~USI_INT_RX_THOLD, USI_INTR_EN);
		ts_i2c_writel(i2c, 0, USI_EN);
		pr_debug("%s:%d\n", __func__, __LINE__);
		complete(&i2c->complete);
		return 0;
	}

	return 0;
}

static int ts_i2c_fill_tx_fifo(struct ts_i2c *i2c, u32 status)
{
	u32 val, num, emptyfifo;
	u8 *buf = i2c->cur_trans;

	if ((status & USI_INT_I2C_STOP) || (status & USI_INT_TX_WERR)) {
		val = ts_i2c_readl(i2c, USI_INTR_EN);
		ts_i2c_writel(i2c, val & ~USI_INT_TX_EMPTY, USI_INTR_EN);
		ts_i2c_writel(i2c, 0, USI_EN);
		pr_debug("%s:%d\n", __func__, __LINE__);
		complete(&i2c->complete);
		return 0;
	}

	if (status & USI_INT_TX_EMPTY) {
		pr_debug("%s:%d\n", __func__, __LINE__);
		num = ts_i2c_readl(i2c, USI_FIFO_STA);
		num &= (0x1F << 8);
		num >>= 8;
		emptyfifo = (i2c->residue > (I2C_FIFO_MAX - num)) ?
				(I2C_FIFO_MAX - num) : i2c->residue;

		ts_i2c_writesb(i2c, buf, USI_TX_RX_FIFO, emptyfifo);
		i2c->residue -= emptyfifo;
		i2c->cur_trans += emptyfifo;
	}

	if (i2c->residue == 0) {
		i2c->cur_trans = NULL;
		val = ts_i2c_readl(i2c, USI_I2CM_CTRL);
		/*create stop*/
		ts_i2c_writel(i2c, val | (1 << 1), USI_I2CM_CTRL);
		val = ts_i2c_readl(i2c, USI_INTR_EN);
		ts_i2c_writel(i2c, val & ~USI_INT_TX_EMPTY, USI_INTR_EN);
	}

	return 0;
}

static int ts_i2c_reset_hardware(struct ts_i2c *i2c)
{
	int err;
	u32 val, speed;

	err = clk_prepare_enable(i2c->clk);
	if (err) {
		dev_err(DEV(i2c), "failed to enable clock\n");
		return err;
	}

	ts_i2c_writel(i2c, 0, USI_EN);
	ts_i2c_writel(i2c, 0, USI_INTR_EN);
	ts_i2c_writel(i2c, 0, USI_I2CM_CTRL);

	ts_i2c_writel(i2c, USI_MODE_I2C, USI_MODE_SEL);
	ts_i2c_writel(i2c, CK_USI_I2C_MODE_MATER, USI_I2C_MODE);

	speed = (clk_get_rate(i2c->clk) / i2c->clk_freq - 2) / 2;

	val = ts_i2c_readl(i2c, USI_I2CM_CTRL);

	if (speed >= IIC_HIGHSPEED)
		ts_i2c_writel(i2c, val | USI_I2CM_CTRL_HS_MODE, USI_I2CM_CTRL);
	else
		ts_i2c_writel(i2c, val & ~USI_I2CM_CTRL_HS_MODE, USI_I2CM_CTRL);

	ts_i2c_writel(i2c, speed, USI_CLK_DIV0);
	ts_i2c_writel(i2c, speed, USI_CLK_DIV1);

	val = ts_i2c_readl(i2c, USI_I2CM_CTRL);
	ts_i2c_writel(i2c, val & ~USI_I2CM_CTRL_ADDR_MODE, USI_I2CM_CTRL);
	val = ts_i2c_readl(i2c, USI_I2CM_CTRL);
	ts_i2c_writel(i2c, val | USI_I2CM_CTRL_ADDR7_MODE, USI_I2CM_CTRL);

	val = ts_i2c_readl(i2c, USI_I2CM_CTRL);
	ts_i2c_writel(i2c, val & ~(1 << 1), USI_I2CM_CTRL);

	return 0;
}

static irqreturn_t ts_i2c_isr(int irq, void *dev_id)
{
	u32 status;
	struct ts_i2c *i2c = (struct ts_i2c *)dev_id;

	status = ts_i2c_readl(i2c, USI_INTR_STA);
	ts_i2c_writel(i2c, status, USI_INTR_CLR);
	pr_debug("%s:%d, status:0x%x\n", __func__, __LINE__, status);

	if (status & USI_INT_I2CM_LOSE_ARBI) {
		// dev_err(DEV(i2c), "i2c arbi\n");
		i2c->error = -ENXIO;
		goto done;
	} else if (status & USI_INT_I2C_NACK) {
		// dev_err(DEV(i2c), "i2c nak\n");
		i2c->error = -EIO;
		goto done;
	}

	if (i2c->msg_rd)
		ts_i2c_empty_rx_fifo(i2c, status);
	else
		ts_i2c_fill_tx_fifo(i2c, status);

	return IRQ_HANDLED;
done:
	ts_i2c_writel(i2c, 0, USI_EN);
	complete(&i2c->complete);
	return IRQ_HANDLED;
}

static void ts_set_addr(struct ts_i2c *i2c, u16 addr)
{

	ts_i2c_writel(i2c, addr & TS_I2C_SLAVE_ADDR_MASK,
							 USI_I2C_ADDR);

}

static void ts_i2c_over(struct ts_i2c *i2c)
{
	ts_i2c_writel(i2c, 0, USI_EN);
	ts_i2c_writel(i2c, 0, USI_INTR_EN);
	ts_i2c_writel(i2c, 0, USI_INTR_UNMASK);
}

static int ts_i2c_wait_bus_not_busy(struct ts_i2c *i2c)
{
	unsigned long timeout;

	timeout = jiffies + TS_I2C_TIMEOUT;
	while ((ts_i2c_readl(i2c, USI_I2C_STA) & USI_I2C_STA_MASTER_WORK)) {
		if (time_after(jiffies, timeout)) {
			dev_warn(DEV(i2c), "timeout waiting for bus ready\n");
			return -EBUSY;
		}
		msleep(20);
	}

	return 0;
}

static int ts_i2c_xfer_bytes(struct ts_i2c *i2c)
{
	unsigned long time_left;
	int rd = i2c->msg_rd;

	reinit_completion(&i2c->complete);

	if (rd)
		ts_i2c_rx_start_ctrl(i2c);
	else
		ts_i2c_tx_start_ctrl(i2c);

	time_left = wait_for_completion_timeout(&i2c->complete,
						i2c->adap.timeout);
	if (time_left == 0) {
		dev_err(DEV(i2c), "timeout\n");
		return -ETIMEDOUT;
	}

	if (i2c->error) {
		// dev_err(DEV(i2c), "i2c error\n");
		ts_i2c_over(i2c);
		return i2c->error;
	}

	return 0;
}

static int ts_i2c_xfer_msg(struct ts_i2c *i2c,
			       struct i2c_msg *msg)
{
	int ret;

	ts_i2c_flush_fifos(i2c);

	i2c->cur_trans = msg->buf;
	i2c->residue = msg->len;
	i2c->msg_rd = msg->flags & I2C_M_RD;
	i2c->error = 0;

	ret = ts_i2c_wait_bus_not_busy(i2c);
	if (ret < 0)
		return ret;

	ret = ts_i2c_xfer_bytes(i2c);
	if (ret)
		return ret;

	i2c->residue = 0;

	return 0;
}

static int ts_i2c_xfer(struct i2c_adapter *adap,
			   struct i2c_msg *msgs, int num)
{
	struct ts_i2c *i2c = i2c_get_adapdata(adap);
	int ret;
	int i;

	ts_set_addr(i2c, msgs->addr);

	for (i = 0; i < num; i++) {
		ret = ts_i2c_xfer_msg(i2c, &msgs[i]);
		if (ret)
			return ret;
	}

	return num;
}

static u32 ts_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C;
}

static int __maybe_unused ts_i2c_suspend(struct device *dev)
{
	struct ts_i2c *i2c = dev_get_drvdata(dev);

	clk_disable_unprepare(i2c->clk);

	return 0;
}

static int __maybe_unused ts_i2c_resume(struct device *dev)
{
	struct ts_i2c *i2c = dev_get_drvdata(dev);

	clk_prepare_enable(i2c->clk);

	return 0;
}

static SIMPLE_DEV_PM_OPS(ts_i2c_dev_pm_ops,
			 ts_i2c_suspend, ts_i2c_resume);

static const struct i2c_algorithm ts_i2c_algo = {
	.master_xfer = ts_i2c_xfer,
	.functionality = ts_i2c_func,
};

static const struct of_device_id ts_i2c_of_match[] = {
	{ .compatible = "ts,ts-usi-i2c", },
	{ },
};
MODULE_DEVICE_TABLE(of, ts_i2c_of_match);

static int ts_i2c_probe(struct platform_device *pdev)
{
	struct ts_i2c *i2c;
	void __iomem *reg_base;
	struct resource *res;
	struct clk *clk;
	int ret;

	i2c = devm_kzalloc(&pdev->dev, sizeof(*i2c), GFP_KERNEL);
	if (!i2c)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	reg_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(reg_base))
		return PTR_ERR(reg_base);

	clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(clk)) {
		dev_err(&pdev->dev, "missing controller clock");
		return PTR_ERR(clk);
	}

	ret = clk_prepare_enable(clk);
	if (ret) {
		dev_err(&pdev->dev, "failed to enable i2c_clk\n");
		return ret;
	}

	ret = device_property_read_u32(&pdev->dev, "clock-frequency",
				       &i2c->clk_freq);
	if (ret) {
		dev_err(&pdev->dev, "missing clock-frequency");
		return ret;
	}

	if (i2c->clk_freq != 100000 && i2c->clk_freq != 400000
	    && i2c->clk_freq != 1000000 && i2c->clk_freq != 3400000) {
		dev_err(&pdev->dev,
			"%d Hz is unsupported, only 100kHz, 400kHz, 1MHz and 3.4MHz are supported\n",
			i2c->clk_freq);
		return -EINVAL;
	}

	ret = platform_get_irq(pdev, 0);
	if (ret < 0)
		return ret;

	i2c->irq = ret;
	i2c->reg_base = reg_base;
	i2c->clk = clk;

	init_completion(&i2c->complete);
	platform_set_drvdata(pdev, i2c);

	ret = ts_i2c_reset_hardware(i2c);
	if (ret) {
		dev_err(&pdev->dev, "failed to initialize i2c controller\n");
		goto err_clk_unprepare;
	}

	ret = devm_request_irq(&pdev->dev, i2c->irq,
			ts_i2c_isr, 0, dev_name(&pdev->dev), i2c);
	if (ret) {
		dev_err(&pdev->dev, "failed to request irq %i\n", i2c->irq);
		goto err_clk_unprepare;
	}

	i2c_set_adapdata(&i2c->adap, i2c);
	strlcpy(i2c->adap.name, "ts i2c adapter",
		sizeof(i2c->adap.name));
	i2c->adap.algo = &ts_i2c_algo;
	i2c->adap.nr = pdev->id;
	i2c->adap.dev.parent = &pdev->dev;
	i2c->adap.dev.of_node = pdev->dev.of_node;
	i2c->adap.timeout = I2C_TIMEOUT;

	ret = i2c_add_numbered_adapter(&i2c->adap);
	if (ret)
		goto err_clk_unprepare;

	return 0;

err_clk_unprepare:
	clk_disable_unprepare(i2c->clk);
	return ret;
}

static int ts_i2c_remove(struct platform_device *pdev)
{
	struct ts_i2c *i2c = platform_get_drvdata(pdev);

	i2c_del_adapter(&i2c->adap);
	clk_disable_unprepare(i2c->clk);

	return 0;
}

static struct platform_driver ts_i2c_driver = {
	.probe	= ts_i2c_probe,
	.remove	= ts_i2c_remove,
	.driver	= {
		.name  = "ts_i2c",
		.of_match_table = ts_i2c_of_match,
		.pm = &ts_i2c_dev_pm_ops,
	},
};
module_platform_driver(ts_i2c_driver);

MODULE_AUTHOR("Shi <captain.shi@tsingmicro.org>");
MODULE_DESCRIPTION("TS USI I2C Bus Controller driver");
MODULE_LICENSE("GPL v2");
