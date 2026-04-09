/*
 * TsingMicro SPI controller driver (master mode only)
 *
 * Copyright (C) 2020 - TsingMicro, Inc.
 *
 * based on Blackfin On-Chip SPI Driver (spi_bfin5xx.c)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/spi/spi.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>

/* Name of this driver */
#define TS_SPI_NAME		"ts-spi"
#define USI_FIFO_STA_TX_EMPTY		BIT(0)
#define USI_FIFO_STA_TX_FULL		BIT(1)
#define USI_FIFO_STA_RX_EMPTY		BIT(2)
#define USI_FIFO_STA_RX_FULL		BIT(3)

#define USI_SPI_MODE_MASTER		0x1
#define USI_SPI_MODE_SLAVE		0x0

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

#define USI_INTR_CTRL_TH_MODE		(1 << 16)
#define USI_INTR_CTRL_RXFIFO_TH		0xfffffcff
#define USI_INTR_CTRL_RXFIFO_TH_4	(0x1 << 8)	// 4-byte data
#define USI_INTR_CTRL_RXFIFO_TH_8	(0x2 << 8)	// 8-byte data
#define USI_INTR_CTRL_RXFIFO_TH_12	(0x3 << 8)	// 12-byte data
#define USI_INTR_CTRL_TXFIFO_TH		0xfffffffc
#define USI_INTR_CTRL_TXFIFO_TH_4	(0x1 << 0)	// 4-byte data
#define USI_INTR_CTRL_TXFIFO_TH_8	(0x2 << 0)	// 8-byte data
#define USI_INTR_CTRL_TXFIFO_TH_12	(0x3 << 0)	// 12-byte data

#define USI_TX_MAX_FIFO		0x10
#define USI_RX_MAX_FIFO		0x10

#define USI_RX_FIFO_EN		(0x1 << 3)
#define USI_TX_FIFO_EN		(0x1 << 2)
#define USI_FM_EN			(0x1 << 1)
#define USI_USI_EN			(0x1 << 0)

#define USI_SPI_CTRL_CPHA	(0x1 << 6) /* Clock Phase Control */
#define USI_SPI_CTRL_CPOL	(0x1 << 7) /* Clock Polarity Control */
#define USI_SPI_CTRL_TMODE		 (3 << 4)
#define USI_SPI_CTRL_TMODE_TXRX  (0 << 4)
#define USI_SPI_CTRL_TMODE_TX	 (1 << 4)
#define USI_SPI_CTRL_TMODE_RX	 (2 << 4)

#define USI_SPI_CTRL_DATA_FRAME_SIZE (0xf << 0)

enum SPI_DATAWIDTH {
	SPI_DATASIZE_4	= 3,
	SPI_DATASIZE_5	= 4,
	SPI_DATASIZE_6	= 5,
	SPI_DATASIZE_7	= 6,
	SPI_DATASIZE_8	= 7,
	SPI_DATASIZE_9	= 8,
	SPI_DATASIZE_10	= 9,
	SPI_DATASIZE_11	= 10,
	SPI_DATASIZE_12	= 11,
	SPI_DATASIZE_13	= 12,
	SPI_DATASIZE_14	= 13,
	SPI_DATASIZE_15	= 14,
	SPI_DATASIZE_16	= 15
};

enum SPI_MODE {
	SPI_TXRX = 0,
	SPI_TX	 = 1,
	SPI_RX	 = 2,
	SPI_EERX = 3
};

struct ts_spi {
	void __iomem *regs;
	struct clk *clk;
	unsigned int clk_rate;
	struct gpio_desc *cs_gpio;
	u32 speed_hz;
	struct spi_transfer *cur_tranfer;
	int cur_cnt;
	enum SPI_MODE mode;
	struct spi_master *master;
};

void ts_spi_usi_enable(struct ts_spi *ts_spi)
{
	u32 usi_en_reg;

	usi_en_reg = readl(ts_spi->regs + USI_EN);
	usi_en_reg |= USI_USI_EN;
	writel(usi_en_reg, ts_spi->regs + USI_EN);
}

void ts_spi_usi_disable(struct ts_spi *ts_spi)
{
	u32 usi_en_reg;

	usi_en_reg = readl(ts_spi->regs + USI_EN);
	usi_en_reg &= ~USI_USI_EN;
	writel(usi_en_reg, ts_spi->regs + USI_EN);

}

static int32_t ts_spi_set_mode(struct ts_spi *ts_spi, enum SPI_MODE mode)
{
	u32 spi_ctrl_reg;

	spi_ctrl_reg = readl(ts_spi->regs + USI_SPI_CTRL);

	switch (mode) {
	case SPI_TXRX:
		spi_ctrl_reg &= ~USI_SPI_CTRL_TMODE;
		spi_ctrl_reg |= USI_SPI_CTRL_TMODE_TXRX;
		break;

	case SPI_TX:
		spi_ctrl_reg &= ~USI_SPI_CTRL_TMODE;
		spi_ctrl_reg |= USI_SPI_CTRL_TMODE_TX;
		break;

	case SPI_RX:
		spi_ctrl_reg &= ~USI_SPI_CTRL_TMODE;
		spi_ctrl_reg |= USI_SPI_CTRL_TMODE_RX;
		break;

	default:
		spi_ctrl_reg &= ~USI_SPI_CTRL_TMODE;
		spi_ctrl_reg |= USI_SPI_CTRL_TMODE_TXRX;
		break;
	}

	ts_spi->mode = mode;
	return 0;
}

static u32 ts_spi_get_rx_num(struct ts_spi *ts_spi)
{
	u32 rxnum = readl(ts_spi->regs + USI_FIFO_STA);

	return rxnum >> 16;
}
static irqreturn_t ts_spi_irq(int irq, void *dev_id)
{
	struct spi_master *master = dev_id;
	struct ts_spi *ts_spi = spi_master_get_devdata(master);
	struct spi_transfer *t = ts_spi->cur_tranfer;
	u32 intr_status, status;
	u32 intr_en_reg, rx_num;
	u32 tmp_cur_cnt;
	u32 rx_cur_cnt;
	u32 i;

	status = IRQ_NONE;

	if (!t) {// False trigger
		intr_status = readl(ts_spi->regs + USI_INTR_STA) & 0x3ffff;
		writel(intr_status, ts_spi->regs + USI_INTR_CLR);
		return IRQ_HANDLED;
	}

	intr_status = readl(ts_spi->regs + USI_INTR_STA) & 0x3ffff;

	if (t->tx_buf && t->rx_buf) {
		if (ts_spi->cur_cnt < t->len) {
			rx_num = ts_spi_get_rx_num(ts_spi);
			while (rx_num) { // Fetch data from rx fifo
				readb(ts_spi->regs + USI_TX_RX_FIFO);
				rx_num--;
			}

			if ((ts_spi->cur_cnt + USI_TX_MAX_FIFO) < t->len)
				tmp_cur_cnt = USI_TX_MAX_FIFO;
			else
				tmp_cur_cnt = t->len - ts_spi->cur_cnt;

			writesb(ts_spi->regs + USI_TX_RX_FIFO, t->tx_buf + ts_spi->cur_cnt,
				tmp_cur_cnt);

			/* read rx fifo */
			rx_cur_cnt = 0;
			rx_num = 0;
			while (rx_cur_cnt < tmp_cur_cnt) {
				rx_num = ts_spi_get_rx_num(ts_spi);
				//printk("[%s], %d, rx_num = %d, tmp_cur_cnt = %d\n", __func__, __LINE__, rx_num, tmp_cur_cnt);
				for(i = 0; i < rx_num; i++) {
					((unsigned char *)t->rx_buf)[ts_spi->cur_cnt + rx_cur_cnt] =
										readb(ts_spi->regs + USI_TX_RX_FIFO);
					rx_cur_cnt++;
				}
			}
			ts_spi->cur_cnt += tmp_cur_cnt;
		} else {
			intr_en_reg = readl(ts_spi->regs + USI_INTR_EN);
			intr_en_reg &= ~USI_INT_TX_EMPTY;
			writel(intr_en_reg, ts_spi->regs + USI_INTR_EN);
			writel(intr_status, ts_spi->regs + USI_INTR_CLR);

			rx_num = ts_spi_get_rx_num(ts_spi);
			while (rx_num) { // Fetch data from rx fifo
				readb(ts_spi->regs + USI_TX_RX_FIFO);
				rx_num--;
			}
			spi_finalize_current_transfer(ts_spi->master);
			return IRQ_HANDLED;
		}
	} else if (t->tx_buf) {
		if (ts_spi->cur_cnt < t->len) {
			rx_num = ts_spi_get_rx_num(ts_spi);
			while (rx_num) { // Fetch data from rx fifo
				readb(ts_spi->regs + USI_TX_RX_FIFO);
				rx_num--;
			}

			if ((ts_spi->cur_cnt + USI_TX_MAX_FIFO) < t->len)
				tmp_cur_cnt = USI_TX_MAX_FIFO;
			else
				tmp_cur_cnt = t->len - ts_spi->cur_cnt;

			writesb(ts_spi->regs + USI_TX_RX_FIFO, t->tx_buf + ts_spi->cur_cnt,
				tmp_cur_cnt);
			ts_spi->cur_cnt += tmp_cur_cnt;
		} else {
			intr_en_reg = readl(ts_spi->regs + USI_INTR_EN);
			intr_en_reg &= ~USI_INT_TX_EMPTY;
			writel(intr_en_reg, ts_spi->regs + USI_INTR_EN);
			writel(intr_status, ts_spi->regs + USI_INTR_CLR);

			rx_num = ts_spi_get_rx_num(ts_spi);
			while (rx_num) { // Fetch data from rx fifo
				readb(ts_spi->regs + USI_TX_RX_FIFO);
				rx_num--;
			}
			spi_finalize_current_transfer(ts_spi->master);
			return IRQ_HANDLED;
		}
	} else {

		rx_num = ts_spi_get_rx_num(ts_spi);
		while (rx_num) {
			if (ts_spi->cur_cnt < t->len) {
				((unsigned char *)t->rx_buf)[ts_spi->cur_cnt] =
					readb(ts_spi->regs + USI_TX_RX_FIFO);
				ts_spi->cur_cnt++;
			} else {
				intr_en_reg = readl(ts_spi->regs + USI_INTR_EN);
				intr_en_reg &= ~USI_INT_TX_EMPTY;
				writel(intr_en_reg, ts_spi->regs + USI_INTR_EN);
				writel(intr_status, ts_spi->regs + USI_INTR_CLR);
				spi_finalize_current_transfer(ts_spi->master);
				return IRQ_HANDLED;
			}
			rx_num--;
		}

		if (rx_num == 0)  /* generate clock to get RT data */
			writeb(0xff, ts_spi->regs + USI_TX_RX_FIFO);

	}

	writel(intr_status, ts_spi->regs + USI_INTR_CLR);

	return IRQ_HANDLED;

}

static int ts_prepare_transfer_hardware(struct spi_master *master)
{
	return 0;
}

static void ts_spi_config_clock_mode(struct spi_device *spi)
{
	struct ts_spi *ts_spi = spi_master_get_devdata(spi->master);
	u32 new_spi_ctrl, ctrl_reg;

	new_spi_ctrl = readl(ts_spi->regs + USI_SPI_CTRL);
	ctrl_reg = new_spi_ctrl;

	/* Set the SPI clock phase and clock polarity */
	new_spi_ctrl &= ~(USI_SPI_CTRL_CPHA | USI_SPI_CTRL_CPOL);
	if (spi->mode & SPI_CPHA)
		new_spi_ctrl |= USI_SPI_CTRL_CPHA;
	if (spi->mode & SPI_CPOL)
		new_spi_ctrl |= USI_SPI_CTRL_CPOL;

	if (new_spi_ctrl != ctrl_reg) {
		ts_spi_usi_disable(ts_spi);
		writel(new_spi_ctrl, ts_spi->regs + USI_SPI_CTRL);
		ts_spi_usi_enable(ts_spi);
	}

}

static int ts_prepare_message(struct spi_master *master,
				struct spi_message *msg)
{

	ts_spi_config_clock_mode(msg->spi);

	return 0;
}

static void ts_spi_config_clock_freq(struct spi_device *spi,
					struct spi_transfer *transfer)
{
	struct ts_spi *ts_spi = spi_master_get_devdata(spi->master);
	u32 ctrl_reg, baud_rate_val = 0x10;
	unsigned long frequency;

	frequency = ts_spi->clk_rate;
	ctrl_reg = readl(ts_spi->regs + USI_CLK_DIV0);

	/* Set the clock frequency */
	if (ts_spi->speed_hz != transfer->speed_hz) {
		baud_rate_val = frequency / transfer->speed_hz;

		/*according to datasheet, spi clk_div0 min value is 0x10 */
		if (baud_rate_val < 0x10)
			baud_rate_val = 0x10;
	}

	writel(baud_rate_val, ts_spi->regs + USI_CLK_DIV0);

}

static int ts_spi_setup_transfer(struct spi_device *spi,
				   struct spi_transfer *transfer)
{
#if 0
	struct ts_spi *ts_spi = spi_master_get_devdata(spi->master);
#endif

	ts_spi_config_clock_freq(spi, transfer);

#if 0
	dev_dbg(&spi->dev, "%s, mode %d, %u bits/w, %u clock speed\n",
		__func__, spi->mode, spi->bits_per_word,
		ts_spi->speed_hz);
#endif
	return 0;

}

static int ts_transfer_one(struct spi_master *master,
				 struct spi_device *spi,
				 struct spi_transfer *transfer)
{
	struct ts_spi *ts_spi = spi_master_get_devdata(master);
	u32 intr_mask_reg, intr_en_reg;

	if (!transfer)
		return 0;

	intr_mask_reg = readl(ts_spi->regs + USI_INTR_UNMASK);
	intr_en_reg = readl(ts_spi->regs + USI_INTR_EN);

	ts_spi_setup_transfer(spi, transfer);
	ts_spi_set_mode(ts_spi, SPI_TXRX);

	ts_spi->cur_tranfer = transfer;
	ts_spi->cur_cnt	 = 0;

	if (transfer->tx_buf) {
		writel(USI_FM_EN | USI_USI_EN,
					ts_spi->regs + USI_EN);
		writel(USI_FM_EN | USI_USI_EN |
				USI_RX_FIFO_EN | USI_TX_FIFO_EN,
					ts_spi->regs + USI_EN);
	}

	/* enable transmit FIFO empty interrupt */
	intr_mask_reg |= USI_INT_TX_EMPTY | USI_INT_TX_WERR | USI_INT_TX_RERR;
	intr_en_reg |= USI_INT_TX_EMPTY | USI_INT_TX_WERR | USI_INT_TX_RERR;

	writel(intr_mask_reg, ts_spi->regs + USI_INTR_UNMASK);
	writel(intr_en_reg, ts_spi->regs + USI_INTR_EN);

	return transfer->len;
}


static int ts_unprepare_transfer_hardware(struct spi_master *master)
{
	return 0;
}

static void ts_spi_chipselect(struct spi_device *spi, bool is_high)
{
	struct ts_spi *ts_spi = spi_master_get_devdata(spi->master);

	if (!ts_spi->cs_gpio) {
		dev_err(&spi->master->dev, "ts_spi->cs_gpio is null.\n");
		return;
	}

	gpiod_set_value(ts_spi->cs_gpio, is_high);

}

static int ts_spi_setup(struct spi_device *spi)
{
	return 0;
}

static void ts_spi_cleanup(struct spi_device *spi)
{

}

static int ts_parse_dt(struct platform_device *pdev,
				struct ts_spi *ts_spi)
{
	ts_spi->cs_gpio = devm_gpiod_get(&pdev->dev, "uspics", GPIOD_OUT_HIGH);
	if (!ts_spi->cs_gpio) {
		dev_err(&pdev->dev, "devm_gpiod_get failed.\n");
		return -1;
	}

	return 0;
}

static void ts_spi_config_data_width(struct ts_spi *ts_spi)
{
	u32 spi_ctrl_reg;

	spi_ctrl_reg = readl(ts_spi->regs + USI_SPI_CTRL);
	spi_ctrl_reg &= ~USI_SPI_CTRL_DATA_FRAME_SIZE;
	spi_ctrl_reg |= SPI_DATASIZE_8;
	writel(spi_ctrl_reg, ts_spi->regs + USI_SPI_CTRL);
}

static void ts_spi_config_mode(struct ts_spi *ts_spi)
{
	writel(USI_SPI_MODE_MASTER, ts_spi->regs + USI_SPI_MODE);
}

static void ts_spi_hw_init(struct ts_spi *ts_spi)
{
	writel(USI_MODE_SPI, ts_spi->regs + USI_MODE_SEL);
	ts_spi_config_mode(ts_spi);
	ts_spi_config_data_width(ts_spi);
}

static ssize_t ts_spi_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct spi_master *master;
	struct ts_spi *ts_spi;
	u32 val;
	void __iomem *iobase = NULL;
	u32 i;
	int off = 0;

	master = (struct spi_master *)dev->driver_data;
	ts_spi = spi_master_get_devdata(master);

	if (buf == NULL) {
		dev_err(dev, "buf is NULL\n");
		return 0;
	}

	iobase = ts_spi->regs;

	off += sprintf(buf + off, "------dump regs begin------\n");
	for (i = 0; i < 0x068; i += 4) {
		val = readl(iobase + i);
		off += sprintf(buf + off, "[%02x] 0x%x\n", i, val);
	}

	off += sprintf(buf + off, "------dump regs end	------\n");

	return off;

}
static ssize_t ts_spi_set(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	struct spi_master *master;
	struct ts_spi *ts_spi;

	master = (struct spi_master *)dev->driver_data;
	ts_spi = spi_master_get_devdata(master);

	return count;
}

static DEVICE_ATTR(ts_spi, 0600, ts_spi_show, ts_spi_set);

static struct attribute *ts_spi_attributes[] = {
	&dev_attr_ts_spi.attr,
	NULL
};

static const struct attribute_group ts_spi_attr_group = {
	.attrs = ts_spi_attributes,
};

static int ts_spi_probe(struct platform_device *pdev)
{
	int ret = 0, irq;
	struct spi_master *master;
	struct ts_spi *ts_spi;
	struct resource *res;

	master = spi_alloc_master(&pdev->dev, sizeof(*ts_spi));
	if (!master)
		return -ENOMEM;

	ts_spi = spi_master_get_devdata(master);
	master->dev.of_node = pdev->dev.of_node;
	platform_set_drvdata(pdev, master);

	ts_spi->master = master;
	ret = ts_parse_dt(pdev, ts_spi);
	if (ret)
		return ret;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ts_spi->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ts_spi->regs)) {
		ret = PTR_ERR(ts_spi->regs);
		goto remove_master;
	}

	ts_spi->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(ts_spi->clk)) {
		dev_err(&pdev->dev, "clk clock not found.\n");
		ret = PTR_ERR(ts_spi->clk);
		goto remove_master;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0) {
		ret = -ENXIO;
		dev_err(&pdev->dev, "irq number is invalid\n");
		goto clk_dis_all;
	}

	ret = devm_request_irq(&pdev->dev, irq, ts_spi_irq,
				   0, pdev->name, master);
	if (ret != 0) {
		ret = -ENXIO;
		dev_err(&pdev->dev, "request_irq failed\n");
		goto clk_dis_all;
	}

	ts_spi->clk_rate = clk_get_rate(ts_spi->clk);
	if (ts_spi->clk_rate == 0) {
		ret = -EINVAL;
		goto clk_dis_all;
	}

	ret = clk_prepare_enable(ts_spi->clk);
	if (ret) {
		dev_err(&pdev->dev, "Unable to enable APB clock.\n");
		goto remove_master;
	}

	master->prepare_transfer_hardware = ts_prepare_transfer_hardware;
	master->prepare_message = ts_prepare_message;
	master->transfer_one = ts_transfer_one;
	master->unprepare_transfer_hardware = ts_unprepare_transfer_hardware;
	master->set_cs = ts_spi_chipselect;
	master->setup = ts_spi_setup;
	master->cleanup = ts_spi_cleanup;
	master->mode_bits = SPI_CPOL | SPI_CPHA;

	/* Set to default valid value */
	master->bits_per_word_mask = SPI_BPW_MASK(8);
	master->max_speed_hz = ts_spi->clk_rate;
	ts_spi->speed_hz = master->max_speed_hz;

	ts_spi_hw_init(ts_spi);

	ret = devm_spi_register_controller(&pdev->dev, master);
	if (ret) {
		dev_err(&pdev->dev, "spi_register_master failed\n");
		goto clk_dis_all;
	}

	ret = sysfs_create_group(&pdev->dev.kobj, &ts_spi_attr_group);
	if (ret < 0)
		dev_warn(&pdev->dev, "attr group create failed\n");
	else
		dev_info(&pdev->dev, "attr group create success!\n");

	dev_dbg(&pdev->dev, "%s sucess.\n", __func__);

	return ret;

clk_dis_all:
	clk_disable_unprepare(ts_spi->clk);
remove_master:
	spi_master_put(master);

	return ret;
}

static int ts_spi_remove(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct ts_spi *ts_spi = spi_master_get_devdata(master);

	sysfs_remove_group(&pdev->dev.kobj, &ts_spi_attr_group);
	clk_disable_unprepare(ts_spi->clk);

	return 0;
}


static const struct of_device_id ts_spi_of_match[] = {
	{ .compatible = "ts,ts-spi" },
	{ /* end of table */ }
};
MODULE_DEVICE_TABLE(of, ts_spi_of_match);

/* ts_spi_driver - This structure defines the SPI subsystem platform driver */
static struct platform_driver ts_spi_driver = {
	.probe	= ts_spi_probe,
	.remove	= ts_spi_remove,
	.driver = {
		.name = TS_SPI_NAME,
		.of_match_table = ts_spi_of_match,
	},
};

module_platform_driver(ts_spi_driver);

MODULE_AUTHOR("Tsingmicro, Inc.");
MODULE_DESCRIPTION("Tsingmicro SPI driver");
MODULE_LICENSE("GPL");

