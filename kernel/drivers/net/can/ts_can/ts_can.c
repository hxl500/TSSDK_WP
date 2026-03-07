#include "ts_can.h"

static const struct can_bittiming_const tscan_bittiming_const = {
	.name = DRV_NAME,
	.tseg1_min = 4,
	.tseg1_max = 16,
	.tseg2_min = 2,
	.tseg2_max = 8,
	.sjw_max = 4,
	.brp_min = 1,
	.brp_max = 256,
	.brp_inc = 1,
};

/*
 * set_reset_mode
 */
static void tscan_set_reset_mode(struct net_device *dev)
{
	struct tscan_priv *priv = netdev_priv(dev);
	struct tscan_regs __iomem *regs = priv->regs;
	union tscan_cdr CDR;

	/* reset can and enter configuration mode */
	if (regs->MOD.reset_mode == 0)
		regs->MOD.reset_mode = 1;
	// 1 clock divider
	CDR.cdr6 = 1;
	CDR.cdr7 = 1;
	CDR.cdr = 0x03;
	regs->CDR.u32 = CDR.u32;
	// 2 out control
	regs->OCR.ocmode = 0x02;
	// 3 ier enable
	regs->IER.u32 = 0xff;
	// 4
	regs->MOD.appeptance_filter_mode = 1;
	// recv filter
	regs->ACR[0] = 0;
	regs->ACR[1] = 0;
	regs->ACR[2] = 0;
	regs->ACR[3] = 0;
	regs->AMR[0] = 0xff;
	regs->AMR[1] = 0xff;
	regs->AMR[2] = 0xff;
	regs->AMR[3] = 0xff;
	// 6 mode
	// regs->MOD.sleep_mode = 1;
	priv->can.state = CAN_STATE_STOPPED;
	return;
#if 0
	union tscan_cdr CDR;
	union tscan_ier IER;
	union tscan_btr1 BTR1;
	// union tscan_mod MOD;

	// mode reset mode
	regs->MOD.reset_mode = 1;
	//1 clock divider
	CDR.cdr6 = 1;
	CDR.cdr7 = 1;
	CDR.cdr = 0x03;
	regs->CDR.u32 = CDR.u32;
	//2 out control
	regs->OCR.ocmode = 0x02;
	//3 ier enable --> receive interrupt enable
	IER.rie = 1;
	IER.wuie = 1;
	IER.tie = 1;
	regs->IER.u32 = IER.u32;
	// 4 
	regs->MOD.appeptance_filter_mode = 1;
	//can_config_filter(hcan, filter_config);
	// recv filter
	regs->ACR[0] = 0;
	regs->ACR[1] = 0;
	regs->ACR[2] = 0;
	regs->ACR[3] = 0;

	regs->AMR[0] = 0xff;
	regs->AMR[1] = 0xff;
	regs->AMR[2] = 0xff;
	regs->AMR[3] = 0xff;

	//5 set bit rate 1mbps
	regs->BTR0.brp = 0x04;
	BTR1.tseg1 = 0x05;
	BTR1.tseg2 = 0x02;
	regs->BTR1.u32 = BTR1.u32;
	//6 mode
	//regs->MOD.selftest_mode = 1;
	regs->MOD.reset_mode = 0;

	// hcan->MOD.sleep_mode = 1;
	priv->can.state = CAN_STATE_STOPPED;
#endif
}

/*
 * set_normal_mode
 */
static void tscan_set_normal_mode(struct net_device *dev)
{
	struct tscan_priv *priv = netdev_priv(dev);
	struct tscan_regs __iomem *regs = priv->regs;

	/*
	 * leave configuration mode
	 */
	regs->MOD.reset_mode = 0;
	//	regs->MOD.sleep_mode = 1;
	return;
}

/*
 * set_bittiming
 */
static int tscan_set_bittiming(struct net_device *dev)
{
	struct tscan_priv *priv = netdev_priv(dev);
	struct tscan_regs __iomem *regs = priv->regs;
	struct can_bittiming *bt = &priv->can.bittiming;

	union tscan_btr0 BTR0; // 06H bus timing 0
	union tscan_btr1 BTR1; // 07H bus timing 1
	//	u16 clk, timing;

	/* reset can and enter configuration mode */
	if (regs->MOD.reset_mode == 0)
		regs->MOD.reset_mode = 1;

	BTR0.brp = bt->brp - 1;
	BTR0.sjw = bt->sjw - 1;
	regs->BTR0.u32 = BTR0.u32;

	if (bt->phase_seg1 > 2)
	{
		if (bt->phase_seg2 > 2)
		{
			BTR1.tseg1 = bt->phase_seg1 - 2;
			BTR1.tseg2 = bt->phase_seg2 - 2;
		}
		else
		{
			BTR1.tseg1 = bt->phase_seg1 - 3;
			BTR1.tseg2 = bt->phase_seg2 - 1;
		}
	}
	else if (bt->phase_seg2 > 2)
	{
		if (bt->phase_seg1 > 2)
		{
			BTR1.tseg1 = bt->phase_seg1 - 2;
			BTR1.tseg2 = bt->phase_seg2 - 2;
		}
		else
		{
			BTR1.tseg1 = bt->phase_seg1 - 1;
			BTR1.tseg2 = bt->phase_seg2 - 3;
		}
	}
	regs->BTR1.u32 = BTR1.u32;

	if (priv->can.ctrlmode & CAN_CTRLMODE_LOOPBACK)
	{
		printk("---selftest_mode\r\n");
		regs->MOD.selftest_mode = 1;
	}

	if (priv->can.ctrlmode & CAN_CTRLMODE_LISTENONLY)
	{
		printk("---listen_only_mode\r\n");
		regs->MOD.listen_only_mode = 1;
	}

	return 0;
}
/*
 * can_start
 */
static void tscan_can_start(struct net_device *dev)
{
	struct tscan_priv *priv = netdev_priv(dev);

	/* enter reset mode */
	if (priv->can.state != CAN_STATE_STOPPED)
		tscan_set_reset_mode(dev);

	tscan_set_bittiming(dev);
	/* leave reset mode */
	tscan_set_normal_mode(dev);
	return;
}

/*
 * can_err
 */
static int tscan_can_err(struct net_device *dev)
{
	struct tscan_priv *priv = netdev_priv(dev);
	struct net_device_stats *stats = &dev->stats;
	struct tscan_regs __iomem *regs = priv->regs;
	struct can_frame *cf;
	struct sk_buff *skb;
	enum can_state state = priv->can.state;

	skb = alloc_can_err_skb(dev, &cf);
	if (skb == NULL)
		return -ENOMEM;

	/* data overrun interrupt */
	if (regs->IR.doi)
	{
		cf->can_id |= CAN_ERR_CRTL;
		cf->data[1] = CAN_ERR_CRTL_RX_OVERFLOW;
		stats->rx_over_errors++;
		stats->rx_errors++;
	}

	/* bus error interrupt */
	if (regs->IR.bei)
	{
		state = CAN_STATE_BUS_OFF;
		cf->can_id |= CAN_ERR_BUSOFF;
		priv->can.can_stats.bus_off++;
		can_bus_off(dev);
	}

	/* error passive interrupt */
	if (regs->IR.epi)
		state = CAN_STATE_ERROR_PASSIVE;

	/* error warning interrupt */
	if (regs->IR.ei)
		state = CAN_STATE_ERROR_WARNING;

	if (state != priv->can.state && (state == CAN_STATE_ERROR_WARNING || state == CAN_STATE_ERROR_PASSIVE))
	{
		cf->can_id |= CAN_ERR_CRTL;
		if (state == CAN_STATE_ERROR_WARNING)
		{
			priv->can.can_stats.error_warning++;
			cf->data[1] = CAN_ERR_CRTL_RX_WARNING;
		}
		else
		{
			priv->can.can_stats.error_passive++;
			cf->data[1] = CAN_ERR_CRTL_RX_PASSIVE;
		}

		priv->can.can_stats.bus_error++;
		cf->can_id |= CAN_ERR_PROT | CAN_ERR_BUSERROR;
		cf->data[2] |= CAN_ERR_PROT_BIT;
	}

	priv->can.state = state;

	stats->rx_packets++;
	stats->rx_bytes += cf->can_dlc;
	netif_rx(skb);

	return 0;
}

/*
 * can_rx
 */
static void tscan_can_rx(struct net_device *dev)
{
	struct tscan_priv *priv = netdev_priv(dev);
	struct net_device_stats *stats = &dev->stats;
	struct tscan_regs __iomem *regs = priv->regs;
	struct can_frame *cf;
	struct sk_buff *skb;
	u32 i;

	skb = alloc_can_skb(dev, &cf);
	if (skb == NULL)
		return;

	/* get id */
	if (regs->frame_info.ff & TSCAN_ID_EXT) // extended frame
	{
		cf->can_id = (regs->extended_frame.id[3] >> 3) | (regs->extended_frame.id[2] << 5) |
					 (regs->extended_frame.id[1] << 13) | (regs->extended_frame.id[0] << 21);
		cf->can_id |= CAN_EFF_FLAG;

		for (i = 0; i < regs->frame_info.dlc; i++)
			cf->data[i] = regs->extended_frame.data[i];
	}
	else // standard frame
	{
		cf->can_id = (regs->standard_frame.id[1] >> 5) | (regs->standard_frame.id[0] << 3);

		for (i = 0; i < regs->frame_info.dlc; i++)
			cf->data[i] = regs->standard_frame.data[i];
	}

	if (regs->frame_info.rtr & TSCAN_RTR_REMOTE) // data frame or remote frame
		cf->can_id |= CAN_RTR_FLAG;

	/* get data length code */
	cf->can_dlc = regs->frame_info.dlc;

	stats->rx_packets++;
	stats->rx_bytes += cf->can_dlc;
	netif_rx(skb);
	return;
}

/*
 * irq
 */
static irqreturn_t tscan_irq(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct net_device_stats *stats = &dev->stats;
	struct tscan_priv *priv = netdev_priv(dev);
	struct tscan_regs __iomem *regs = priv->regs;
	union tscan_ir IR;
	irqreturn_t handled = IRQ_NONE;

	IR.u32 = regs->IR.u32;
	/* reception interrupt */
	if (IR.ri)
	{
		tscan_can_rx(dev);
		regs->CMR.rrb = 1; // release receive buffer
		handled = IRQ_HANDLED;
	}
	/* transmission complete interrupt */
	if (IR.ti)
	{
		stats->tx_packets++;
		stats->tx_bytes += can_get_echo_skb(dev, 0);
		netif_wake_queue(dev);

		handled = IRQ_HANDLED;
	}
	/* error interrupt */
	if (IR.u32 >> 2)
		tscan_can_err(dev);
	else
		return IRQ_NONE;

	return handled;
}

/*
 * open
 */
static int tscan_open(struct net_device *dev)
{
	// struct tscan_priv* priv = netdev_priv(dev);
	int err;

	/* set chip into reset mode */
	tscan_set_reset_mode(dev);

	/* common open */
	err = open_candev(dev);
	if (err)
		goto exit_open;

	tscan_can_start(dev);

	/* register interrupt handler */
	err = request_irq(dev->irq, tscan_irq, IRQF_SHARED, dev->name, dev);
	if (err)
		goto exit_irq;

	netif_start_queue(dev);

	return 0;
exit_irq:
	free_irq(dev->irq, dev);
exit_open:
	return err;
}

/*
 * close
 */
static int tscan_close(struct net_device *dev)
{
	// struct tscan_priv* priv = netdev_priv(dev);

	netif_stop_queue(dev);
	tscan_set_reset_mode(dev);
	close_candev(dev);
	// can_rx_offload_disable(&priv->offload);
	// tsan_chip_stop_disable_on_error(dev);
	free_irq(dev->irq, dev);

	return 0;
}

/*
 * start xmit
 */
static int tscan_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	const struct tscan_priv *priv = netdev_priv(dev);
	struct can_frame *cf = (struct can_frame *)skb->data;
	struct tscan_regs __iomem *regs = priv->regs;
	union tscan_frame tscan_frame_u;
	u32 can_id;
	u8 i;

	if (can_dropped_invalid_skb(dev, skb))
		return NETDEV_TX_OK;

	netif_stop_queue(dev);

	while (regs->SR.tbs != 1)
		;
	// standard frame or extended frame
	if (cf->can_id & CAN_EFF_FLAG) // extended frame
	{
		can_id = cf->can_id & CAN_EFF_MASK;
		tscan_frame_u.ff = TSCAN_ID_EXT;

		regs->extended_frame.id[0] = can_id >> 21;
		regs->extended_frame.id[1] = can_id >> 13;
		regs->extended_frame.id[2] = can_id >> 5;
		regs->extended_frame.id[3] = can_id << 3;
		for (i = 0; i < cf->can_dlc; i++)
			regs->extended_frame.data[i] = cf->data[i];
	}
	else // standard frame
	{
		can_id = cf->can_id & CAN_SFF_MASK;
		tscan_frame_u.ff = TSCAN_ID_EXT;

		regs->standard_frame.id[0] = can_id >> 3;
		regs->standard_frame.id[1] = can_id << 5;
		for (i = 0; i < cf->can_dlc; i++)
			regs->extended_frame.data[i] = cf->data[i];
	}
	// data frame or remote frame
	if (cf->can_id & CAN_RTR_FLAG)
		tscan_frame_u.rtr = TSCAN_RTR_REMOTE;
	else
		tscan_frame_u.rtr = TSCAN_RTR_DATA;
	// data len
	tscan_frame_u.dlc = cf->can_dlc;
	regs->frame_info.u32 = tscan_frame_u.u32;

	can_put_echo_skb(skb, dev, 0);

	// todo
	if (regs->MOD.selftest_mode == 0)
		regs->CMR.tr = 1;
	else
		regs->CMR.srr = 1;

	return NETDEV_TX_OK;
}

static int tscan_set_mode(struct net_device *dev, enum can_mode mode)
{
	switch (mode)
	{
	case CAN_MODE_START:
		tscan_can_start(dev);
		if (netif_queue_stopped(dev))
			netif_wake_queue(dev);
		break;

	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

static int __tscan_get_berr_counter(const struct net_device *dev, struct can_berr_counter *bec)
{
	const struct tscan_priv *priv = netdev_priv(dev);
	struct tscan_regs __iomem *regs = priv->regs;

	bec->rxerr = regs->RXERR;
	bec->txerr = regs->TXERR;

	return 0;
}

static int tscan_get_berr_counter(const struct net_device *dev, struct can_berr_counter *bec)
{
	const struct tscan_priv *priv = netdev_priv(dev);
	int err;
	err = clk_prepare_enable(priv->clk_ipg);
	if (err)
		return err;

	err = clk_prepare_enable(priv->clk_per);
	if (err)
		goto out_disable_ipg;

	err = __tscan_get_berr_counter(dev, bec);

	clk_disable_unprepare(priv->clk_per);
out_disable_ipg:
	clk_disable_unprepare(priv->clk_ipg);

	return err;
}

static const struct net_device_ops tscan_netdev_ops = {
	.ndo_open = tscan_open,
	.ndo_stop = tscan_close,
	.ndo_start_xmit = tscan_start_xmit,
	.ndo_change_mtu = can_change_mtu,
};
/*
 * probe
 */
static int tscan_probe(struct platform_device *pdev)
{
	struct net_device *dev;
	struct tscan_priv *priv;
	// struct regulator* reg_xceiver;
	struct resource *mem;
	// struct clk* clk_ipg = NULL, * clk_per = NULL;
	struct clk* pclk;
	void __iomem *regs;
	int irq, ret;
	u32 clock_freq ;
	// reg_xceiver = devm_regulator_get(&pdev->dev, "xceiver");
	// if (PTR_ERR(reg_xceiver) == -EPROBE_DEFER)
	// 	return -EPROBE_DEFER;
	// else if (IS_ERR(reg_xceiver))
	// 	reg_xceiver = NULL;
	// if (pdev->dev.of_node)
	// 	of_property_read_u32(pdev->dev.of_node, "clock-frequency", &clock_freq);
	// if (!clock_freq)
	// {
	// 	clk_ipg = devm_clk_get(&pdev->dev, "ipg");
	// 	if (IS_ERR(clk_ipg))
	// 	{
	// 		dev_err(&pdev->dev, "no ipg clock defined\n");
	// 		return PTR_ERR(clk_ipg);
	// 	}
	// 	clk_per = devm_clk_get(&pdev->dev, "per");
	// 	if (IS_ERR(clk_per))
	// 	{
	// 		dev_err(&pdev->dev, "no per clock defined\n");
	// 		return PTR_ERR(clk_per);
	// 	}
	// 	clock_freq = clk_get_rate(clk_per);
	// }
	pclk = devm_clk_get(&pdev->dev, "can_pclk");
	if (IS_ERR(pclk))
	{
		ret = PTR_ERR(pclk);
		dev_err(&pdev->dev, "Unable to get cve pclk clk: %d\n", ret);
		goto err_pclk;
	}
	ret = clk_prepare_enable(pclk);
	if (ret)
	{
		dev_err(&pdev->dev, "Cannot enable cve pclk clock: %d\n", ret);
		goto err_pclk;
	}
	clock_freq = clk_get_rate(pclk);

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	irq = platform_get_irq(pdev, 0);
	if (irq <= 0)
		return -ENODEV;

	regs = devm_ioremap_resource(&pdev->dev, mem);
	if (IS_ERR(regs))
		return PTR_ERR(regs);

	// of_id = of_match_device(tscan_of_match, &pdev->dev);
	// if (of_id)
	// 	devtype_data = of_id->data;
	// else if (platform_get_device_id(pdev)->driver_data)
	// 	devtype_data = (struct flexcan_devtype_data*)platform_get_device_id(pdev)->driver_data;
	// else
	// 	return -ENODEV;

	dev = alloc_candev(sizeof(struct tscan_priv), 1);
	if (!dev)
		return -ENOMEM;

	platform_set_drvdata(pdev, dev);
	SET_NETDEV_DEV(dev, &pdev->dev);

	dev->netdev_ops = &tscan_netdev_ops;
	dev->irq = irq;
	dev->flags |= IFF_ECHO;

	priv = netdev_priv(dev);



	priv->can.bittiming_const = &tscan_bittiming_const;
	priv->can.do_set_bittiming = tscan_set_bittiming;
	priv->can.do_set_mode = tscan_set_mode;
	priv->can.do_get_berr_counter = tscan_get_berr_counter;
	priv->can.ctrlmode_supported = CAN_CTRLMODE_LOOPBACK | CAN_CTRLMODE_LISTENONLY | CAN_CTRLMODE_3_SAMPLES | CAN_CTRLMODE_BERR_REPORTING;

	priv->regs = regs;
	priv->can.clock.freq = clock_freq;
	priv->pclk = pclk;
	// priv->clk_per = clk_per;
	// priv->pdata = dev_get_platdata(&pdev->dev);
	// priv->can.clock.freq = clock_freq;
	// err = register_tscandev(dev);

	ret = register_candev(dev);
	if (ret)
	{
		dev_err(&pdev->dev, "registering netdev failed\n");
		goto failed_register;
	}

	return 0;
	// failed_offload:
err_pclk:
failed_register:
	clk_disable_unprepare(pclk);
	free_candev(dev);
	return ret;
}

static int tscan_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);
	struct tscan_priv* priv = netdev_priv(dev);

	clk_disable_unprepare(priv->pclk);
	tscan_set_reset_mode(dev);
	unregister_candev(dev);
	free_candev(dev);
	return 0;
}

/*
 * suspend
 */
static int tscan_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	struct net_device *dev = platform_get_drvdata(pdev);
	struct tscan_priv *priv = netdev_priv(dev);
	struct tscan_regs __iomem *regs = priv->regs;

	if (netif_running(dev))
		/* enter sleep mode */
		regs->MOD.sleep_mode = 1;

	return 0;
}

/*
 * resume
 */
static int tscan_resume(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);
	struct tscan_priv *priv = netdev_priv(dev);
	struct tscan_regs __iomem *regs = priv->regs;

	if (netif_running(dev))
		/* leave sleep mode */
		regs->MOD.sleep_mode = 0;

	return 0;
}

/*
 * match
 */
static const struct of_device_id tscan_of_match[] = {
	{.compatible = "ts,ts_can"},
	{/* Sentinel */},
};

static struct platform_driver tscan_driver = {
	.driver = {
		.name = DRV_NAME,
		.of_match_table = tscan_of_match,
	},
	.probe = tscan_probe,
	.remove = tscan_remove,
	.suspend = tscan_suspend,
	.resume = tscan_resume,
};

module_platform_driver(tscan_driver);
MODULE_AUTHOR("zhaohancheng");
MODULE_LICENSE("GPL v2");