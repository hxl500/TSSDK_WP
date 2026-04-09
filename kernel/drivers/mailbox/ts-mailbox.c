/*
 * Copyright (c) 2015, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/mailbox_controller.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/stat.h>

/* Interrupt Generate Register (R/W)
 * Write 1 into it, enable interrupt to client immediately.
 * Write 0 into it, disable interrupt to client immediately.
 */
#define TS_MBOX_INTGR		0x00

/* Interrupt Clear Register (W1C)
 * Write 1 into it, clear the interrupt
 */
#define TS_MBOX_INTCR		0x04

/* Interrupt Mask Register (R/W)
 * 1 means mask interrupt bit
 * 0 means unmask interrupt bit
 */
#define TS_MBOX_INTMR		0x08

/* Interrupt Register Status Register (RO)
 * Read 1 means has interrupt
 * Read 0 means no interrupt
 */
#define TS_MBOX_INTRSR		0x0C

/* Interrupt Masked Status Register (RO)
 * Read 1 means has interrupt on masked bit
 * Read 0 means no interrupt on masked bit
 */
#define TS_MBOX_INTMSR		0x10

/* Interrupt Enable (R/W)
 * 1 means interrupt enabled
 * 0 means interrupt disabled
 */
#define TS_MBOX_INTENB		0x14

#define TS_MBOX_SEND_MAX_MESSAGE_LENGTH   60
#define TS_MBOX_MAX_MESSAGE_LENGTH        64

#define MBOX_INTGR_ADDR(mbox)	 \
	(0x18*(mbox->idx ? 0 : 1) + TS_MBOX_INTGR)
#define MBOX_INTCR_ADDR(mbox)	 \
	(0x18*(mbox->idx) + TS_MBOX_INTCR)
#define MBOX_INTMR_ADDR(mbox)	 \
	(0x18*(mbox->idx) + TS_MBOX_INTMR)
#define MBOX_INTRSR_ADDR(mbox)	 \
	(0x18*(mbox->idx) + TS_MBOX_INTRSR)
#define MBOX_INTMSR_ADDR(mbox)	 \
	(0x18*(mbox->idx) + TS_MBOX_INTMSR)
#define MBOX_INTENB_ADDR(mbox)	 \
	(0x18*(mbox->idx) + TS_MBOX_INTENB)

#define MBOX_TX_ACK_ADDR(mbox)	(0x18*2 + (mbox->idx ? 124 : 60))
#define MBOX_RX_ACK_ADDR(mbox)	(0x18*2 + (mbox->idx ? 60 : 124))
#define MBOX_TX_MSSG_ADDR(mbox)	(0x18*2 + (mbox->idx ? 64 : 0))
#define MBOX_RX_MSSG_ADDR(mbox)	(0x18*2 + (mbox->idx ? 0 : 64))

#define MBOX_ACK	0x4B6765
#define ACK_BIT		15

#define MBOX_CHAN_MAX		16

/* Mailbox message length: 15 words */
#define TS_MBOX_MSG_LEN		15

struct ts_mbox_msg {
	u32 len;
	u32 data[TS_MBOX_MSG_LEN];
};

#define MBOX_TX				0x1

struct ts_mbox_chan {

	/*
	 * Description for channel's hardware info:
	 *  - direction: tx or rx
	 *  - dst irq: peer core's irq number
	 *  - ack irq: local irq number
	 *  - slot number
	 */
	unsigned int dir;
	unsigned int slot;

	struct ts_mbox *parent;
};

struct ts_mbox {
	struct device *dev;

	int irq;
	int idx;

	/* flag of enabling tx's irq mode */
	bool tx_irq_mode;

	/* region for mailbox */
	void __iomem *base;

	struct clk *pclk;

	unsigned int chan_num;
	struct ts_mbox_chan *mchan;

	void *irq_map_chan[MBOX_CHAN_MAX];
	struct mbox_chan *chan;
	struct mbox_controller controller;
};


static int ts_mbox_send_data(struct mbox_chan *chan, void *msg)
{
	struct ts_mbox_chan *mchan = chan->con_priv;
	struct ts_mbox *mbox = mchan->parent;
	u32 *buf = msg;
	int i;

	for (i = 0; i < TS_MBOX_MSG_LEN; i++)
		writel(buf[i], mbox->base + MBOX_TX_MSSG_ADDR(mbox) + i * 4);

	/* Generate tx interrupt for other side receive */
	writel_relaxed(BIT(TS_MBOX_MSG_LEN) - 1,
					 mbox->base + MBOX_INTGR_ADDR(mbox));

	return 0;
}

static int ts_mbox_startup(struct mbox_chan *chan)
{
	struct ts_mbox_chan *mchan = chan->con_priv;
	struct ts_mbox *mbox = mchan->parent;

	/* Enable all rx interrupts */
	writel_relaxed(BIT(TS_MBOX_MAX_MESSAGE_LENGTH >> 2) - 1,
		       mbox->base + MBOX_INTENB_ADDR(mbox));

	/*rx unmasked interrupts*/
	writel_relaxed(0, mbox->base + MBOX_INTMR_ADDR(mbox));

	/*clear rx interrupts*/
	writel_relaxed(BIT(TS_MBOX_MAX_MESSAGE_LENGTH >> 2) - 1,
		       mbox->base + MBOX_INTCR_ADDR(mbox));

	return 0;
}

static void ts_mbox_shutdown(struct mbox_chan *chan)
{
	struct ts_mbox_chan *mchan = chan->con_priv;
	struct ts_mbox *mbox = mchan->parent;

	/* Disable all B2A interrupts */
	writel_relaxed(0, mbox->base + MBOX_INTENB_ADDR(mbox));

}

static const struct mbox_chan_ops ts_mbox_ops = {
	.send_data	= ts_mbox_send_data,
	.startup	= ts_mbox_startup,
	.shutdown	= ts_mbox_shutdown,
};

static irqreturn_t ts_mbox_isr(int irq, void *p)
{
	struct ts_mbox *mbox = p;
	struct ts_mbox_chan *mchan;
	struct mbox_chan *chan;
	unsigned int state, i;
	u32 msg[TS_MBOX_MSG_LEN];

	state = readl_relaxed(mbox->base + MBOX_INTMSR_ADDR(mbox));
	if (!state) {
		dev_warn(mbox->dev, "%s: spurious interrupt\n",
			 __func__);
		return IRQ_HANDLED;
	}

	chan = &mbox->chan[0];
	if (!chan) {
		dev_warn(mbox->dev, "%s: unexpected irq vectoc\n",
			 __func__);
	}

	mchan = chan->con_priv;

	if (state & BIT(ACK_BIT))
		mbox_chan_txdone(chan, 0);
	else {
		for (i = 0; i < TS_MBOX_MSG_LEN; i++)
			msg[i] = readl_relaxed(mbox->base +
					 MBOX_RX_MSSG_ADDR(mbox) + i * 4);

		mbox_chan_received_data(chan, (void *)msg);
		/*send ack to the sender*/
		writel_relaxed(MBOX_ACK,
				 mbox->base + MBOX_TX_ACK_ADDR(mbox));
		/* Generate tx interrupt for other side receive */
		writel_relaxed(BIT(ACK_BIT),
				 mbox->base + MBOX_INTGR_ADDR(mbox));
	}
	/* clear IRQ source */
	writel_relaxed(state,
			   mbox->base + MBOX_INTCR_ADDR(mbox));


	return IRQ_HANDLED;
}

enum cdns_sysfs_cmd {
	SYSFS_CMD_READ_REG = 0,
	SYSFS_CMD_WRITE_REG,
	SYSFS_CMD_RECV_DATA,
	SYSFS_CMD_SEND_DATA,
	SYSFS_CMD_SEND_DATA_SAMPLE,
	SYSFS_CMD_MAX
};
struct mbox_sysfs_cmd {
	enum cdns_sysfs_cmd cmd;
	const char *key_str;
};

#define SYSFS_MBOX_TEST_BUFFER_SIZE		0x100
#define DCC_CMD_HEAD       0xAA
#define DCC_CMD_VER        1           /* Get a53 soft version */
#define DCC_CMD_LEN        8

static int mbox_dump_regs(struct ts_mbox *mbox, char *buf)
{
	void __iomem *iobase = NULL;
	u32 val;
	u32 i;
	int off = 0;

	if (buf == NULL) {
		dev_info(mbox->dev, "buf is NULL\n");
		return 0;
	}

	if (mbox == NULL) {
		off = sprintf(buf, "mb is NULL\n");
		return off;
	}
	iobase = mbox->base;

	off += sprintf(buf + off, "------dump regs begin------\n");
	for (i = 0; i < 0xB0; i += 4) {
		val = readl(iobase + i);
		off += sprintf(buf + off, "[%02x] 0x%2x\n", i, val);
	}
	off += sprintf(buf + off, "------dump regs end  ------\n");

	return off;
}

static ssize_t mbox_dbg_show(struct device *dev,
			 struct device_attribute *attr, char *buf)
{
	struct ts_mbox *mbox;
	ssize_t rc;

	mbox = (struct ts_mbox *)dev->driver_data;

	rc = mbox_dump_regs(mbox, buf);

	return rc;
}

static ssize_t mbox_dbg_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct ts_mbox *mbox;
	struct mbox_sysfs_cmd cmd_arr[] = {
		{SYSFS_CMD_READ_REG, "readreg"},
		{SYSFS_CMD_WRITE_REG, "writereg"},
		{SYSFS_CMD_RECV_DATA, "recvdata"},
		{SYSFS_CMD_SEND_DATA, "senddata"},
		{SYSFS_CMD_SEND_DATA_SAMPLE, "sample"} };
	char *p = NULL;
	char *endp = NULL;
	u32 op_offset, rd_val, wr_val;
	bool found = false;
	int ret, i, len;
	struct ts_mbox_msg msg;
	char ck804_data[DCC_CMD_LEN] = {0};

	mbox = (struct ts_mbox *)dev->driver_data;

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
		dev_info(dev, "Current commands supported:\n");
		dev_info(dev, "readreg <offset>\n");
		dev_info(dev, "writereg <offset> <value>\n");
		dev_info(dev, "recvdata <offset> <length>\n");
		dev_info(dev, "senddata <buffer>\n");
		dev_info(dev, "sample\n");
		return count;
	}

	if (mbox->controller.ops->startup != NULL) {
		ret = mbox->controller.ops->startup(&mbox->chan[0]);
		if (ret != 0)
			dev_info(dev, "startup %d error\n", ret);
	}

	switch (cmd_arr[i].cmd) {
	case SYSFS_CMD_READ_REG:
		op_offset = (u32)simple_strtoul(p + 1, &endp, 0);
		if (op_offset & 0x3) {
			dev_info(dev, "offset 0x%x not align to 0x4\n",
								 op_offset);
			return count;
		}
		rd_val = readl(mbox->base + op_offset);
		dev_info(dev, "reg [0x%02x] 0x%x\n", op_offset, rd_val);
		break;

	case SYSFS_CMD_WRITE_REG:
		op_offset = (u32)simple_strtoul(p + 1, &endp, 0);
		if (op_offset & 0x3) {
			dev_info(dev, "offset 0x%x not align to 0x4\n",
								 op_offset);
			return count;
		}

		if (endp == NULL) {
			dev_info(dev, "not enough para, wr_val needed\n");
			return count;
		}

		wr_val = (u32)simple_strtoul(endp + 1, &endp, 0);
		writel(wr_val, mbox->base + op_offset);
		dev_info(dev, "wr reg [0x%02x] 0x%x\n", op_offset, wr_val);
		break;

	case SYSFS_CMD_RECV_DATA:
		break;

	case SYSFS_CMD_SEND_DATA:
		/*delete the last '\n'*/
		len = strlen(p + 1) - 1;
		memcpy((char *)msg.data, p+1, len);
		dev_info(dev, "len 0x%x count 0x%lx\n", len, count);
		if (mbox->controller.ops->send_data != NULL) {
			ret = mbox->controller.ops->send_data
					(&mbox->chan[0], (void *)(msg.data));
			if (ret != 0)
				dev_info(dev, "error %d write data\n", ret);
		}
		break;

	case SYSFS_CMD_SEND_DATA_SAMPLE:
		ck804_data[0] = DCC_CMD_HEAD;
		ck804_data[1] = DCC_CMD_VER;

		memcpy((char *)msg.data, ck804_data, sizeof(ck804_data));
		if (mbox->controller.ops->send_data != NULL) {
			ret = mbox->controller.ops->send_data
					(&mbox->chan[0], (void *)(msg.data));
			if (ret != 0)
				dev_info(dev, "error %d write data\n", ret);
		}
		break;

	default:
		break;
	}

	return count;
}

static DEVICE_ATTR(mboxdbg, S_IRUGO | S_IWUSR, mbox_dbg_show, mbox_dbg_store);

static struct attribute *mboxdbg_attrs[] = {
	&dev_attr_mboxdbg.attr,
	NULL,
};

static struct attribute_group mboxdbg_attr_group = {
	.attrs = mboxdbg_attrs,
};

static const struct of_device_id ts_mbox_of_match[] = {
	{ .compatible = "ts,tx536-mailbox"},
	{ },
};
MODULE_DEVICE_TABLE(of, ts_mbox_of_match);

static int ts_mbox_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ts_mbox *mbox;
	struct resource *res;
	int i, err;

	mbox = devm_kzalloc(dev, sizeof(*mbox), GFP_KERNEL);
	if (!mbox)
		return -ENOMEM;

	mbox->dev = dev;
	mbox->chan_num = MBOX_CHAN_MAX;
	mbox->mchan = devm_kzalloc(dev,
		mbox->chan_num * sizeof(*mbox->mchan), GFP_KERNEL);
	if (!mbox->mchan)
		return -ENOMEM;

	mbox->chan = devm_kzalloc(dev,
		mbox->chan_num * sizeof(*mbox->chan), GFP_KERNEL);
	if (!mbox->chan)
		return -ENOMEM;

	mbox->irq = platform_get_irq(pdev, 0);
	if (mbox->irq < 0)
		return mbox->irq;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	mbox->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(mbox->base)) {
		dev_err(dev, "ioremap buffer failed\n");
		return PTR_ERR(mbox->base);
	}

	err = devm_request_irq(dev, mbox->irq, ts_mbox_isr, 0,
			dev_name(dev), mbox);
	if (err) {
		dev_err(dev, "Failed to register a mailbox IRQ handler: %d\n",
			err);
		return -ENODEV;
	}

	mbox->idx = 0;
	mbox->controller.dev = dev;
	mbox->controller.chans = &mbox->chan[0];
	mbox->controller.num_chans = mbox->chan_num;
	mbox->controller.ops = &ts_mbox_ops;

	for (i = 0; i < mbox->chan_num; i++) {
		mbox->chan[i].con_priv = &mbox->mchan[i];
		mbox->irq_map_chan[i] = NULL;

		mbox->mchan[i].parent = mbox;
	}

	/* use interrupt for tx's ack */
	mbox->tx_irq_mode = true;

	if (mbox->tx_irq_mode)
		mbox->controller.txdone_irq = true;
	else {
		mbox->controller.txdone_poll = true;
		mbox->controller.txpoll_period = 5;
	}

	mbox->pclk = devm_clk_get(&pdev->dev, "pclk_mailbox");
	if (IS_ERR(mbox->pclk)) {
		err = PTR_ERR(mbox->pclk);
		dev_err(&pdev->dev, "failed to get pclk_mailbox clock: %d\n",
			err);
		return err;
	}

	err = clk_prepare_enable(mbox->pclk);
	if (err) {
		dev_err(&pdev->dev, "failed to enable pclk: %d\n", err);
		return err;
	}

	err = mbox_controller_register(&mbox->controller);
	if (err) {
		dev_err(dev, "Failed to register mailbox %d\n", err);
		return err;
	}

	err = sysfs_create_group(&mbox->dev->kobj, &mboxdbg_attr_group);
	if (err < 0)
		dev_warn(dev, "attr group create failed\n");
	else
		dev_info(dev, "attr group create success!\n");

	platform_set_drvdata(pdev, mbox);
	dev_info(dev, "Mailbox enabled\n");
	return 0;
}

static int ts_mbox_remove(struct platform_device *pdev)
{
	struct ts_mbox *mbox = platform_get_drvdata(pdev);

	if (!mbox)
		return -EINVAL;

	mbox_controller_unregister(&mbox->controller);

	sysfs_remove_group(&mbox->dev->kobj, &mboxdbg_attr_group);

	return 0;
}

static struct platform_driver ts_mbox_driver = {
	.probe	= ts_mbox_probe,
	.remove	= ts_mbox_remove,
	.driver = {
		.name = "ts-mailbox",
		.of_match_table = of_match_ptr(ts_mbox_of_match),
	},
};

module_platform_driver(ts_mbox_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Tsingmicro mailbox: communicate between CPU cores and MCU");
MODULE_AUTHOR("Shi <syc@tsingmicor.com>");
