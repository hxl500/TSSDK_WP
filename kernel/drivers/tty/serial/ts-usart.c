/*
 *  Driver for Tsingmicro serial ports (USART)
 *
 * Author: Shi <captain@tsm.co.il>
 *
 * Copyright (C) 2022 Tsingmicro Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/console.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>

#define TS_USART_NR		4
#define DEV_NAME "ttyts"
#define DRIVER_NAME "ts-usart"

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

#define USI_RX_FIFO_EN		(0x1 << 3)
#define USI_TX_FIFO_EN		(0x1 << 2)
#define USI_FM_EN			(0x1 << 1)
#define USI_USI_EN			(0x1 << 0)

#define CTRL_DBIT_BITS		0x3c
#define CTRL_DBIT_5			0x0
#define CTRL_DBIT_6			0x1
#define CTRL_DBIT_7			0x2
#define CTRL_DBIT_8			0x3

#define CTRL_STOP_BITS		0x33
#define CTRL_STOP_1			(0x0 << 2)
#define CTRL_STOP_1_5		(0x1 << 2)
#define CTRL_STOP_2			(0x2 << 2)

#define CTRL_PARITY_BITS	0x0f
#define CTRL_PARITY_ENABLE	(0x1 << 4)
#define CTRL_PARITY_ODD		(0x0 << 5)
#define CTRL_PARITY_EVEN	(0x1 << 5)

#define CK_UART_INT_ENABLE_DEFAUL (USI_INT_UART_PERR | \
			USI_INT_UART_STOP | USI_INT_RX_WERR | \
			USI_INT_RX_RERR | USI_INT_RX_FULL | \
			USI_INT_TX_WERR | USI_INT_TX_RERR | \
			USI_INT_RX_THOLD)

/*----- USART Control Codes: Mode Parameters: Data Bits -----*/
enum uart_data_bits_e {
	UART_DATA_BITS_5	= 0,	///< 5 Data bits
	UART_DATA_BITS_6,			///< 6 Data bit
	UART_DATA_BITS_7,			///< 7 Data bits
	UART_DATA_BITS_8,			///< 8 Data bits (default)
	UART_DATA_BITS_9			///< 9 Data bits
};

/*----- USART Control Codes: Mode Parameters: Parity -----*/
enum uart_parity_e {
	UART_PARITY_NONE	= 0,	///< No Parity (default)
	UART_PARITY_EVEN,			///< Even Parity
	UART_PARITY_ODD,			///< Odd Parity
	UART_PARITY_1,				///< Parity forced to 1
	UART_PARITY_0				///< Parity forced to 0
};

/*----- USART Control Codes: Mode Parameters: Stop Bits -----*/
enum uart_stop_bits_e {
	UART_STOP_BITS_1	= 0,	///< 1 Stop bit (default)
	UART_STOP_BITS_2,			///< 2 Stop bits
	UART_STOP_BITS_1_5,			///< 1.5 Stop bits
	UART_STOP_BITS_0_5			///< 0.5 Stop bits
};

struct ts_port {
	struct uart_port port;
};

static struct uart_port *ts_ports[TS_USART_NR];

static bool ts_uart_tx_full(struct uart_port *port)
{
	return !!(readl_relaxed(port->membase + USI_FIFO_STA) &
		  USI_FIFO_STA_TX_FULL);
}

static bool ts_uart_rx_empty(struct uart_port *port)
{
	return !!(readl_relaxed(port->membase + USI_FIFO_STA) &
		  USI_FIFO_STA_RX_EMPTY);
}

static void ts_uart_stop_tx(struct uart_port *port)
{
	u32 val;

	val = readl_relaxed(port->membase + USI_INTR_EN);
	writel_relaxed(val & (~USI_INT_TX_EMPTY), port->membase + USI_INTR_EN);
}

static void ts_uart_start_tx(struct uart_port *port)
{
	u32 val;

	val = readl_relaxed(port->membase + USI_INTR_EN);
	writel_relaxed(val | USI_INT_TX_EMPTY, port->membase + USI_INTR_EN);
	val = readl_relaxed(port->membase + USI_INTR_UNMASK);
	writel_relaxed(val | USI_INT_TX_EMPTY, port->membase + USI_INTR_UNMASK);
}

static void ts_uart_stop_rx(struct uart_port *port)
{
	u32 val;

	val = readl_relaxed(port->membase + USI_INTR_EN);
	writel_relaxed(val & (~USI_INT_RX_THOLD), port->membase + USI_INTR_EN);
}

static void ts_uart_rx(struct uart_port *port)
{
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);

	while (1) {
		u32 status;
		u8 ch;
		unsigned int ch_flag;

		if (ts_uart_rx_empty(port))
			break;

		ch = readl_relaxed(port->membase + USI_TX_RX_FIFO);
		status = readl_relaxed(port->membase + USI_INTR_STA);

		port->icount.rx++;
		ch_flag = TTY_NORMAL;

		if (status) {
			if (status & USI_INT_UART_PERR)
				port->icount.parity++;
			else if (status & USI_INT_TX_WERR)
				port->icount.overrun++;

			status &= port->read_status_mask;

			if (status & USI_INT_UART_PERR)
				ch_flag = TTY_PARITY;
			else if (status & USI_INT_TX_WERR)
				ch_flag = TTY_OVERRUN;
		}

		if (status & port->ignore_status_mask)
			continue;

		uart_insert_char(port, status, USI_INT_TX_WERR, ch, ch_flag);
	}

	spin_unlock_irqrestore(&port->lock, flags);

	tty_flip_buffer_push(&port->state->port);
}

static void ts_uart_tx(struct uart_port *port)
{
	struct circ_buf *xmit = &port->state->xmit;
	unsigned long flags;

	if (ts_uart_tx_full(port))
		return;

	spin_lock_irqsave(&port->lock, flags);

	if (port->x_char) {
		writel_relaxed(port->x_char, port->membase + USI_TX_RX_FIFO);
		port->icount.tx++;
		port->x_char = 0;
		goto out;
	}

	if (uart_circ_empty(xmit) || uart_tx_stopped(port)) {
		ts_uart_stop_tx(port);
		goto out;
	}

	while (!uart_circ_empty(xmit)) {
		writeb(xmit->buf[xmit->tail], port->membase + USI_TX_RX_FIFO);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;

		if (ts_uart_tx_full(port))
			break;
	}

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);

out:
	spin_unlock_irqrestore(&port->lock, flags);
}

static irqreturn_t ts_uart_int(int irq, void *dev_id)
{
	u32 int_status, val;
	struct uart_port *port = dev_id;

	int_status = readl_relaxed(port->membase + USI_INTR_STA);

	if (int_status & USI_INT_RX_THOLD) {
		writel_relaxed(USI_INT_RX_THOLD, port->membase + USI_INTR_CLR);
		ts_uart_rx(port);
	}

	if (int_status & USI_INT_TX_EMPTY) {
		writel_relaxed(USI_INT_TX_EMPTY, port->membase + USI_INTR_CLR);
		ts_uart_tx(port);
	}

	int_status = readl_relaxed(port->membase + USI_INTR_STA);
	if (int_status & USI_INT_UART_STOP) {
		writel_relaxed(USI_INT_UART_STOP, port->membase + USI_INTR_CLR);
		ts_uart_rx(port);
	}

	int_status = readl_relaxed(port->membase + USI_INTR_STA);
	if (int_status & USI_INT_UART_PERR) {
		writel_relaxed(USI_INT_TX_EMPTY,
					 port->membase + USI_INT_UART_PERR);
		val = readl_relaxed(port->membase + USI_EN);
		writel_relaxed(val & ~USI_RX_FIFO_EN, port->membase + USI_EN);
		val = readl_relaxed(port->membase + USI_EN);
		writel_relaxed(val | USI_RX_FIFO_EN, port->membase + USI_EN);
	}

	writel_relaxed(int_status, port->membase + USI_INTR_CLR);

	return IRQ_HANDLED;
}

static unsigned int ts_uart_tx_empty(struct uart_port *port)
{
	u32 status = readl_relaxed(port->membase + USI_FIFO_STA);

	return (status & USI_FIFO_STA_TX_EMPTY) ? TIOCSER_TEMT : 0;
}

static unsigned int ts_uart_get_mctrl(struct uart_port *port)
{
	return TIOCM_CTS;
}

static void ts_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

static void ts_uart_break_ctl(struct uart_port *port, int state)
{
}

void ts_usi_set_rxfifo_th(struct uart_port *port, u32 length)
{
	u32 val;

	val = readl_relaxed(port->membase + USI_INTR_CTRL);
	writel_relaxed(val & USI_INTR_CTRL_RXFIFO_TH,
		port->membase + USI_INTR_CTRL);
	val = readl_relaxed(port->membase + USI_INTR_CTRL);

	if (length >= USI_RX_MAX_FIFO) {
		writel_relaxed(val | USI_INTR_CTRL_RXFIFO_TH_12 |
		 USI_INTR_CTRL_TH_MODE, port->membase + USI_INTR_CTRL);
	} else if (length >= USI_RX_MAX_FIFO - 4) {
		writel_relaxed(val | USI_INTR_CTRL_RXFIFO_TH_8 |
		 USI_INTR_CTRL_TH_MODE, port->membase + USI_INTR_CTRL);
	} else if (length >= 4) {
		writel_relaxed(val | USI_INTR_CTRL_RXFIFO_TH_4 |
		 USI_INTR_CTRL_TH_MODE, port->membase + USI_INTR_CTRL);
	} else {
		writel_relaxed(val | USI_INTR_CTRL_RXFIFO_TH_4,
		 port->membase + USI_INTR_CTRL);
	}

}

static int ts_uart_startup(struct uart_port *port)
{
	u32 val;

	writel_relaxed(0x0, port->membase + USI_EN);
	writel_relaxed(0x0, port->membase + USI_INTR_UNMASK);
	writel_relaxed(0x0, port->membase + USI_INTR_EN);

	val = readl_relaxed(port->membase + USI_INTR_CTRL);
	writel_relaxed(val | USI_INTR_CTRL_TXFIFO_TH_12,
			   port->membase + USI_INTR_CTRL);
	/*need mask interupt before config rx fifo */
	ts_usi_set_rxfifo_th(port, USI_RX_MAX_FIFO);
	writel_relaxed(USI_MODE_UART, port->membase + USI_MODE_SEL);
	writel_relaxed(CK_UART_INT_ENABLE_DEFAUL,
				 port->membase + USI_INTR_UNMASK);
	writel_relaxed(CK_UART_INT_ENABLE_DEFAUL,
				 port->membase + USI_INTR_EN);

	writel_relaxed(0xf, port->membase + USI_EN);

	return 0;
}

static void ts_uart_shutdown(struct uart_port *port)
{

	writel_relaxed(0, port->membase + USI_EN);
}

static void ts_uart_config_baudrate(struct uart_port *port, u32 baudrate)
{
	u32 divisor, rate;

	rate = port->uartclk;
	writel_relaxed(0x0, port->membase + USI_EN);

	divisor = ((rate * 10) / baudrate) >> 4;

	if ((divisor % 10) >= 5)
		divisor = (divisor / 10);
	else
		divisor = divisor / 10 - 1;

	writel_relaxed(divisor, port->membase + USI_CLK_DIV0);
	writel_relaxed(0xf, port->membase + USI_EN);
}

static int ts_uart_config_parity(struct uart_port *port,
					 enum uart_parity_e parity)
{
	u32 val;

	writel_relaxed(0x0, port->membase + USI_EN);

	switch (parity) {
	case UART_PARITY_NONE:
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val & (~CTRL_PARITY_ENABLE),
						 port->membase + USI_UART_CTRL);
		break;

	case UART_PARITY_ODD:
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val & CTRL_PARITY_BITS,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_PARITY_ENABLE,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_PARITY_ODD,
						 port->membase + USI_UART_CTRL);
		break;

	case UART_PARITY_EVEN:
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val & CTRL_PARITY_BITS,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_PARITY_ENABLE,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_PARITY_EVEN,
						 port->membase + USI_UART_CTRL);
		break;

	default:
		return -1;
	}

	writel_relaxed(0xf, port->membase + USI_EN);

	return 0;
}

static int ts_uart_config_stopbits(struct uart_port *port,
					 enum uart_stop_bits_e stopbit)
{
	u32 val;

	writel_relaxed(0x0, port->membase + USI_EN);

	switch (stopbit) {
	case UART_STOP_BITS_1:
		/* Clear the STOP bit to set 1 stop bit*/
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val & CTRL_STOP_BITS,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_STOP_1,
						 port->membase + USI_UART_CTRL);
		break;

	case UART_STOP_BITS_1_5:
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val & CTRL_STOP_BITS,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_STOP_1_5,
						 port->membase + USI_UART_CTRL);
		break;

	case UART_STOP_BITS_2:
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val & CTRL_STOP_BITS,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_STOP_2,
						 port->membase + USI_UART_CTRL);
		break;

	default:
		return -1;
	}

	writel_relaxed(0xf, port->membase + USI_EN);

	return 0;
}

static int ts_uart_config_databits(struct uart_port *port,
					 enum uart_data_bits_e databits)
{
	u32 val;

	writel_relaxed(0x0, port->membase + USI_EN);

	switch (databits) {
	case UART_DATA_BITS_5:
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val & CTRL_DBIT_BITS,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_DBIT_5,
						 port->membase + USI_UART_CTRL);
		break;

	case UART_DATA_BITS_6:
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val & CTRL_DBIT_BITS,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_DBIT_6,
						 port->membase + USI_UART_CTRL);
		break;

	case UART_DATA_BITS_7:
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val & CTRL_DBIT_BITS,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_DBIT_7,
						 port->membase + USI_UART_CTRL);
		break;

	case UART_DATA_BITS_8:
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val & CTRL_DBIT_BITS,
						 port->membase + USI_UART_CTRL);
		val = readl_relaxed(port->membase + USI_UART_CTRL);
		writel_relaxed(val | CTRL_DBIT_8,
						 port->membase + USI_UART_CTRL);
		break;

	default:
		return -1;
	}

	writel_relaxed(0xf, port->membase + USI_EN);

	return 0;
}

static int ts_uart_config(struct uart_port *port,
					 u32 baud,
					 enum uart_parity_e parity,
					 enum uart_stop_bits_e stopbits,
					 enum uart_data_bits_e bits)
{
	int ret;
	/* control the data_bit of the usart*/
	ts_uart_config_baudrate(port, baud);

	/* control the parity of the usart*/
	ret = ts_uart_config_parity(port, parity);
	if (ret < 0)
		return ret;

	/* control the stopbit of the usart*/
	ret = ts_uart_config_stopbits(port, stopbits);
	if (ret < 0)
		return ret;

	ret = ts_uart_config_databits(port, bits);
	if (ret < 0)
		return ret;

	return 0;
}

static void ts_uart_set_termios(struct uart_port *port,
		struct ktermios *new, struct ktermios *old)
{
	unsigned long flags;
	unsigned int baud;
	enum uart_data_bits_e databits;
	enum uart_stop_bits_e stopbit;
	enum uart_parity_e parity;

	 /* Mask termios capabilities we don't support */
	new->c_cflag &= ~CMSPAR;
	new->c_iflag &= ~(BRKINT | IGNBRK);

	baud = uart_get_baud_rate(port, new, old,
		DIV_ROUND_CLOSEST(port->uartclk, 16 * 8192),
		DIV_ROUND_CLOSEST(port->uartclk, 16));

	switch (new->c_cflag & CSIZE) {
	case CS5:
		databits = UART_DATA_BITS_5;
		break;
	case CS6:
		databits = UART_DATA_BITS_6;
		break;
	case CS7:
		databits = UART_DATA_BITS_7;
		break;
	case CS8:
		databits = UART_DATA_BITS_8;
		break;
	}

	if (new->c_cflag & CSTOPB)
		/* the receiver only verifies the first stop bit */
		stopbit = UART_STOP_BITS_2;
	else
		stopbit = UART_STOP_BITS_1;

	if (new->c_cflag & PARENB) {
		if (new->c_cflag & PARODD)
			parity = UART_PARITY_ODD;
		else
			parity = UART_PARITY_EVEN;
	} else
		parity = UART_PARITY_NONE;

	spin_lock_irqsave(&port->lock, flags);

	/* Set read status mask */
	port->read_status_mask = USI_INT_TX_WERR;
	if (new->c_iflag & INPCK)
		port->read_status_mask |= USI_INT_UART_PERR;

	/* Set status ignore mask */
	port->ignore_status_mask = 0;
	if (!(new->c_cflag & CREAD))
		port->ignore_status_mask |= USI_INT_TX_WERR | USI_INT_UART_PERR;

	uart_update_timeout(port, new->c_cflag, baud);

	ts_uart_config(port, baud, parity, stopbit, databits);

	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *ts_uart_type(struct uart_port *port)
{
	return (port->type == PORT_TS) ? "TS USART" : NULL;
}

static void ts_uart_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE)
		port->type = PORT_TS;
}

static void ts_uart_release_port(struct uart_port *port)
{
}

static int ts_uart_request_port(struct uart_port *port)
{
	return 0;
}

static const struct uart_ops ts_uart_ops = {
	.tx_empty	= ts_uart_tx_empty,
	.set_mctrl	= ts_uart_set_mctrl,
	.get_mctrl	= ts_uart_get_mctrl,
	.stop_tx	= ts_uart_stop_tx,
	.start_tx	= ts_uart_start_tx,
	.stop_rx	= ts_uart_stop_rx,
	.break_ctl	= ts_uart_break_ctl,
	.startup	= ts_uart_startup,
	.shutdown	= ts_uart_shutdown,
	.set_termios	= ts_uart_set_termios,
	.type		= ts_uart_type,
	.config_port	= ts_uart_config_port,
	.release_port	= ts_uart_release_port,
	.request_port	= ts_uart_request_port,
};

static void ts_uart_console_putchar(struct uart_port *port, int ch)
{
	while (ts_uart_tx_full(port))
		cpu_relax();

	writel_relaxed(ch, port->membase + USI_TX_RX_FIFO);
}

static void ts_uart_console_write(struct console *co, const char *c,
					 unsigned int n)
{
	struct uart_port *port = ts_ports[co->index];
	u32 status;
	unsigned long flags;
	int locked = 1;

	if (oops_in_progress)
		locked = spin_trylock_irqsave(&port->lock, flags);
	else
		spin_lock_irqsave(&port->lock, flags);

	uart_console_write(port, c, n, ts_uart_console_putchar);

	if (locked)
		spin_unlock_irqrestore(&port->lock, flags);

	/* Wait for transmitter to become empty */
	do {
		status = readl_relaxed(port->membase + USI_FIFO_STA);
	} while ((status & USI_FIFO_STA_TX_EMPTY) == 0);
}

static int ts_uart_console_setup(struct console *co, char *options)
{
	int baud = 115200, bits = 8, parity = 'n', flow = 'n';
	struct uart_port *port;

	if (co->index < 0 || co->index >= TS_USART_NR)
		return -EINVAL;

	port = ts_ports[co->index];
	if (!port)
		return -ENODEV;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct console ts_console = {
	.name	= DEV_NAME,
	.device	= uart_console_device,
	.write	= ts_uart_console_write,
	.setup	= ts_uart_console_setup,
	.flags	= CON_PRINTBUFFER,
	.index	= -1,
};

static struct uart_driver ts_uart = {
	.driver_name	= "ts-usart",
	.dev_name	= DEV_NAME,
	//.major			= TTY_MAJOR,
	//.minor			= 64,
	.nr		= TS_USART_NR,
	.cons = &ts_console,
};

static int ts_uart_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	int irq, ret, index;
	struct ts_port *dp;
	struct resource *res;
	struct clk *uart_clk;

	if (!np) {
		dev_err(&pdev->dev, "Missing device tree node\n");
		return -ENXIO;
	}

	index = of_alias_get_id(np, "serial");
	/* if index = -1; then scan for a free id and use that */
	if (index < 0 || index >= TS_USART_NR) {
		for (index = 0; index < TS_USART_NR; index++)
			if ((ts_ports[index] == NULL)
				|| (ts_ports[index]->mapbase == 0))
				break;
	}

	if (index >= TS_USART_NR)
		return -EINVAL;

	dp = devm_kzalloc(&pdev->dev, sizeof(*dp), GFP_KERNEL);
	if (!dp)
		return -ENOMEM;

	uart_clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(uart_clk)) {
		ret = PTR_ERR(uart_clk);
		dev_err(&pdev->dev, "Unable to get usi clk: %d\n", ret);
		return ret;
	}
	ret = clk_prepare_enable(uart_clk);
	if (ret) {
		dev_err(&pdev->dev, "Cannot enable usi clock: %d\n", ret);
		return ret;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	dp->port.mapbase = res->start;
	dp->port.membase = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(dp->port.membase))
		return PTR_ERR(dp->port.membase);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;
	dp->port.irq = irq;

	dp->port.iotype = UPIO_MEM;
	dp->port.uartclk = clk_get_rate(uart_clk);
	dp->port.fifosize = 16;
	dp->port.dev = &pdev->dev;
	dp->port.ops = &ts_uart_ops;
	dp->port.line = index;
	dp->port.type = PORT_TS;
	spin_lock_init(&dp->port.lock);

	dev_dbg(&pdev->dev, "usi clk rate: %d\n", dp->port.uartclk);
	ts_ports[index] = &dp->port;
	platform_set_drvdata(pdev, &dp->port);

	ret = devm_request_irq(&pdev->dev, dp->port.irq, ts_uart_int, 0,
				   DRIVER_NAME, &dp->port);
	if (ret)
		return ret;

	return uart_add_one_port(&ts_uart, &dp->port);
}

static int ts_uart_remove(struct platform_device *pdev)
{
	struct uart_port *port = platform_get_drvdata(pdev);

	uart_remove_one_port(&ts_uart, port);

	return 0;
}

static const struct of_device_id ts_uart_dt_ids[] = {
	{ .compatible = "ts,ts-usi-uart", },
	{ }
};
MODULE_DEVICE_TABLE(of, ts_uart_dt_ids);

static struct platform_driver ts_uart_platform = {
	.driver = {
		.name		= "ts-usart",
		.of_match_table	= of_match_ptr(ts_uart_dt_ids),
	},
	.probe	= ts_uart_probe,
	.remove	= ts_uart_remove,
};

static int __init ts_uart_init(void)
{
	int ret;

	if (IS_ENABLED(CONFIG_SERIAL_TS_USART_CONSOLE)) {
		ts_uart.cons = &ts_console;
		ts_console.data = &ts_uart;
	}

	ret = uart_register_driver(&ts_uart);
	if (ret)
		return ret;

	ret = platform_driver_register(&ts_uart_platform);
	if (ret)
		uart_unregister_driver(&ts_uart);

	return ret;
}
module_init(ts_uart_init);

static void __exit ts_uart_exit(void)
{
	platform_driver_unregister(&ts_uart_platform);
	uart_unregister_driver(&ts_uart);
}
module_exit(ts_uart_exit);

MODULE_AUTHOR("Shi <captain@tsm.co.il>");
MODULE_DESCRIPTION("Tsingmicro USART serial driver");
MODULE_LICENSE("GPL");
