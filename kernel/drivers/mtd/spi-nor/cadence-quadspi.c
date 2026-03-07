/*
 * Driver for Cadence QSPI Controller
 *
 * Copyright Altera Corporation (C) 2012-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <linux/clk.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/spi-nor.h>
#include <linux/of_device.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/spi/spi.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/reset.h>
#include <linux/clk-provider.h>
#include <asm/set_memory.h>

#define CQSPI_NAME			"cadence-qspi"
#define CQSPI_MAX_CHIPSELECT		16

/* Quirks */
#define CQSPI_NEEDS_WR_DELAY		BIT(0)

struct cqspi_st;

struct cqspi_flash_pdata {
	struct spi_nor	nor;
	struct cqspi_st	*cqspi;
	u32		clk_rate;
	u32		read_delay;
	u32		tshsl_ns; /* cs# High Time */
	u32		tsd2d_ns;
	u32		tchsh_ns; /* cs# Active Hold Time*/
	u32		tslch_ns; /* cs# Active Setup Time*/
	u8		inst_width;
	u8		addr_width;
	u8		data_width;
	u8		cs;
	bool		registered;
};

struct cqspi_st {
	struct platform_device	*pdev;

	struct clk		*clk;
	struct clk		*hclk;
	struct clk		*pclk;
	unsigned int		sclk;

	void __iomem		*iobase;
	void __iomem		*ahb_base;
	struct completion	transfer_complete;
	struct mutex		bus_mutex;

	int			current_cs;
	int			current_page_size;
	int			current_erase_size;
	int			current_addr_width;
	unsigned long		master_ref_clk_hz;
	bool			is_decoded_cs;
	u32			fifo_depth;
	u32			fifo_width;
	u32			fifo_rx_depth;
	u32			fifo_tx_depth;
	u32			trigger_address;
	u32			trig_addr_off;
	u32			wr_delay;
	u32         irq_status;
	struct cqspi_flash_pdata f_pdata[CQSPI_MAX_CHIPSELECT];

	bool			wr_completion;
	bool			wr_en;
};

/* Operation timeout value */
#define CQSPI_TIMEOUT_MS				500
#define CQSPI_READ_TIMEOUT_MS			10

/* Instruction type */
#define CQSPI_INST_TYPE_SINGLE			0
#define CQSPI_INST_TYPE_DUAL			1
#define CQSPI_INST_TYPE_QUAD			2
#define CQSPI_INST_TYPE_OCTAL			3

#define CQSPI_DUMMY_CLKS_PER_BYTE		8
#define CQSPI_DUMMY_BYTES_MAX			4
#define CQSPI_DUMMY_CLKS_MAX			31

#define CQSPI_STIG_DATA_LEN_MAX			8

/* Register map */
#define CQSPI_REG_CONFIG			0x00
#define CQSPI_REG_CONFIG_ENABLE_MASK		BIT(0)
#define CQSPI_REG_CONFIG_ENB_DIR_ACC_CTRL	BIT(7)
#define CQSPI_REG_CONFIG_DECODE_MASK		BIT(9)
#define CQSPI_REG_CONFIG_CHIPSELECT_LSB		10
#define CQSPI_REG_CONFIG_DMA_MASK		BIT(15)
#define CQSPI_REG_CONFIG_BAUD_LSB		19
#define CQSPI_REG_CONFIG_IDLE_LSB		31
#define CQSPI_REG_CONFIG_CHIPSELECT_MASK	0xF
#define CQSPI_REG_CONFIG_BAUD_MASK		0xF

#define CQSPI_REG_RD_INSTR			0x04
#define CQSPI_REG_RD_INSTR_OPCODE_LSB		0
#define CQSPI_REG_RD_INSTR_TYPE_INSTR_LSB	8
#define CQSPI_REG_RD_INSTR_TYPE_ADDR_LSB	12
#define CQSPI_REG_RD_INSTR_TYPE_DATA_LSB	16
#define CQSPI_REG_RD_INSTR_MODE_EN_LSB		20
#define CQSPI_REG_RD_INSTR_DUMMY_LSB		24
#define CQSPI_REG_RD_INSTR_TYPE_INSTR_MASK	0x3
#define CQSPI_REG_RD_INSTR_TYPE_ADDR_MASK	0x3
#define CQSPI_REG_RD_INSTR_TYPE_DATA_MASK	0x3
#define CQSPI_REG_RD_INSTR_DUMMY_MASK		0x1F

#define CQSPI_REG_WR_INSTR			0x08
#define CQSPI_REG_WR_INSTR_OPCODE_LSB		0
#define CQSPI_REG_WR_INSTR_TYPE_ADDR_LSB	12
#define CQSPI_REG_WR_INSTR_TYPE_DATA_LSB	16
#define CQSPI_REG_WR_INSTR_WEL_DIS_MASK     BIT(1)

#define CQSPI_REG_DELAY				0x0C
#define CQSPI_REG_DELAY_TSLCH_LSB		0
#define CQSPI_REG_DELAY_TCHSH_LSB		8
#define CQSPI_REG_DELAY_TSD2D_LSB		16
#define CQSPI_REG_DELAY_TSHSL_LSB		24
#define CQSPI_REG_DELAY_TSLCH_MASK		0xFF
#define CQSPI_REG_DELAY_TCHSH_MASK		0xFF
#define CQSPI_REG_DELAY_TSD2D_MASK		0xFF
#define CQSPI_REG_DELAY_TSHSL_MASK		0xFF

#define CQSPI_REG_READCAPTURE			0x10
#define CQSPI_REG_READCAPTURE_BYPASS_LSB	0
#define CQSPI_REG_READCAPTURE_DELAY_LSB		1
#define CQSPI_REG_READCAPTURE_DELAY_MASK	0xF

#define CQSPI_REG_SIZE				0x14
#define CQSPI_REG_SIZE_ADDRESS_LSB		0
#define CQSPI_REG_SIZE_PAGE_LSB			4
#define CQSPI_REG_SIZE_BLOCK_LSB		16
#define CQSPI_REG_SIZE_ADDRESS_MASK		0xF
#define CQSPI_REG_SIZE_PAGE_MASK		0xFFF
#define CQSPI_REG_SIZE_BLOCK_MASK		0x3F

#define CQSPI_REG_SRAMPARTITION			0x18
#define CQSPI_REG_INDIRECTTRIGGER		0x1C

#define CQSPI_REG_DMA				0x20
#define CQSPI_REG_DMA_SINGLE_LSB		0
#define CQSPI_REG_DMA_BURST_LSB			8
#define CQSPI_REG_DMA_SINGLE_MASK		0xFF
#define CQSPI_REG_DMA_BURST_MASK		0xFF

#define CQSPI_REG_REMAP				0x24
#define CQSPI_REG_MODE_BIT			0x28

#define CQSPI_REG_SDRAMLEVEL			0x2C
#define CQSPI_REG_SDRAMLEVEL_RD_LSB		0
#define CQSPI_REG_SDRAMLEVEL_WR_LSB		16
#define CQSPI_REG_SDRAMLEVEL_RD_MASK		0xFFFF
#define CQSPI_REG_SDRAMLEVEL_WR_MASK		0xFFFF

#define CQSPI_REG_WR_COMPLETION_CTRL		0x38
#define CQSPI_REG_WR_DISABLE_AUTO_POLL		BIT(14)

#define CQSPI_REG_IRQSTATUS			0x40
#define CQSPI_REG_IRQMASK			0x44

#define CQSPI_REG_INDIRECTRD			0x60
#define CQSPI_REG_INDIRECTRD_START_MASK		BIT(0)
#define CQSPI_REG_INDIRECTRD_CANCEL_MASK	BIT(1)
#define CQSPI_REG_INDIRECTRD_DONE_MASK		BIT(5)

#define CQSPI_REG_INDIRECTRDWATERMARK		0x64
#define CQSPI_REG_INDIRECTRDSTARTADDR		0x68
#define CQSPI_REG_INDIRECTRDBYTES		0x6C

#define CQSPI_REG_CMDCTRL			0x90
#define CQSPI_REG_CMDCTRL_EXECUTE_MASK		BIT(0)
#define CQSPI_REG_CMDCTRL_INPROGRESS_MASK	BIT(1)
#define CQSPI_REG_CMDCTRL_WR_BYTES_LSB		12
#define CQSPI_REG_CMDCTRL_WR_EN_LSB		15
#define CQSPI_REG_CMDCTRL_ADD_BYTES_LSB		16
#define CQSPI_REG_CMDCTRL_ADDR_EN_LSB		19
#define CQSPI_REG_CMDCTRL_RD_BYTES_LSB		20
#define CQSPI_REG_CMDCTRL_RD_EN_LSB		23
#define CQSPI_REG_CMDCTRL_OPCODE_LSB		24
#define CQSPI_REG_CMDCTRL_WR_BYTES_MASK		0x7
#define CQSPI_REG_CMDCTRL_ADD_BYTES_MASK	0x3
#define CQSPI_REG_CMDCTRL_RD_BYTES_MASK		0x7

#define CQSPI_REG_INDIRECTWR			0x70
#define CQSPI_REG_INDIRECTWR_START_MASK		BIT(0)
#define CQSPI_REG_INDIRECTWR_CANCEL_MASK	BIT(1)
#define CQSPI_REG_INDIRECTWR_DONE_MASK		BIT(5)

#define CQSPI_REG_INDIRECTWRWATERMARK		0x74
#define CQSPI_REG_INDIRECTWRSTARTADDR		0x78
#define CQSPI_REG_INDIRECTWRBYTES		0x7C
#define CQSPI_REG_INDIRECT_TRIGGER_ADDR_RANGE		0x80

#define CQSPI_REG_CMDADDRESS			0x94
#define CQSPI_REG_CMDREADDATALOWER		0xA0
#define CQSPI_REG_CMDREADDATAUPPER		0xA4
#define CQSPI_REG_CMDWRITEDATALOWER		0xA8
#define CQSPI_REG_CMDWRITEDATAUPPER		0xAC

/* Interrupt status bits */
#define CQSPI_REG_IRQ_MODE_ERR			BIT(0)
#define CQSPI_REG_IRQ_UNDERFLOW			BIT(1)
#define CQSPI_REG_IRQ_IND_COMP			BIT(2)
#define CQSPI_REG_IRQ_IND_RD_REJECT		BIT(3)
#define CQSPI_REG_IRQ_WR_PROTECTED_ERR		BIT(4)
#define CQSPI_REG_IRQ_ILLEGAL_AHB_ERR		BIT(5)
#define CQSPI_REG_IRQ_WATERMARK			BIT(6)
#define CQSPI_REG_IRQ_IND_SRAM_FULL		BIT(12)

#define CQSPI_IRQ_MASK_RD		(CQSPI_REG_IRQ_WATERMARK	| \
					 CQSPI_REG_IRQ_IND_SRAM_FULL	| \
					 CQSPI_REG_IRQ_IND_COMP)

#define CQSPI_IRQ_MASK_WR		(CQSPI_REG_IRQ_IND_COMP		| \
					 CQSPI_REG_IRQ_WATERMARK	| \
					 CQSPI_REG_IRQ_UNDERFLOW)

#define CQSPI_IRQ_STATUS_MASK		0x1FFFF

#define SPINOR_DEFAULT_PAGE_SIZE        0x100    /* 256 bytes */
#define SPINOR_DEFAULT_ADDRESS_BYTES    3
#define SPINOR_DEFAULT_BLOCK_SIZE       0x10000  /* 64K bytes */

#define SYSFS_TEST_BUFFER_SIZE          0x100000
enum cdns_sysfs_cmd {
	SYSFS_CMD_READ_REG    = 0,
	SYSFS_CMD_WRITE_REG   = 1,
	SYSFS_CMD_READ_AHB    = 2,
	SYSFS_CMD_READ_ID     = 3,
	SYSFS_CMD_READ_STATUS = 4,
	SYSFS_CMD_ERASE_DATA  = 5,
	SYSFS_CMD_READ_DATA   = 6,
	SYSFS_CMD_WRITE_DATA  = 7,
	SYSFS_CMD_MAX
};
struct cqspi_sysfs_cmd {
	enum cdns_sysfs_cmd cmd;
	const char *key_str;
};
struct cqspi_st *g_cqspi;

static uint32_t __maybe_unused __glb_current_us(void)
{
	unsigned int now;

	asm volatile ("mrc p15, 0, %0, c9, c13, 0" : "=r" (now));

	//now = ((uint64_t)now * 1000)/ 1000000000;
	now = (uint64_t)now >> 10;
	//printf("now=%lld\n", now);
	return now;
}

static int cqspi_wait_for_bit(void __iomem *reg, const u32 mask, bool clear)
{
	u32 val;

	return readl_poll_timeout_atomic(reg, val,
					  (((clear ? ~val : val) & mask) == mask),
					  10, CQSPI_TIMEOUT_MS * 1000);
}

static bool cqspi_is_idle(struct cqspi_st *cqspi)
{
	u32 reg = readl(cqspi->iobase + CQSPI_REG_CONFIG);

	return reg & (1 << CQSPI_REG_CONFIG_IDLE_LSB);
}

static u32 cqspi_get_rd_sram_level(struct cqspi_st *cqspi)
{
	u32 reg = readl(cqspi->iobase + CQSPI_REG_SDRAMLEVEL);

	reg >>= CQSPI_REG_SDRAMLEVEL_RD_LSB;
	return reg & CQSPI_REG_SDRAMLEVEL_RD_MASK;
}

static unsigned int irq_enter_cnt;
static unsigned int irq_complete_cnt;

module_param(irq_enter_cnt, uint, 0644);
module_param(irq_complete_cnt, uint, 0644);

static irqreturn_t cqspi_irq_handler(int this_irq, void *dev)
{
	struct cqspi_st *cqspi = dev;
	unsigned int irq_status;

	irq_enter_cnt++;

	/* Read interrupt status */
	irq_status = readl(cqspi->iobase + CQSPI_REG_IRQSTATUS);
	pr_debug("irq_status: %08x\n", irq_status);
	cqspi->irq_status |= irq_status;

	/* Clear interrupt */
	writel(irq_status, cqspi->iobase + CQSPI_REG_IRQSTATUS);

	irq_status &= (CQSPI_IRQ_MASK_RD | CQSPI_IRQ_MASK_WR);

	if (irq_status) {
		irq_complete_cnt++;
		complete(&cqspi->transfer_complete);
	}

	return IRQ_HANDLED;
}

static unsigned int cqspi_calc_rdreg(struct spi_nor *nor, const u8 opcode)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	u32 rdreg = 0;

	rdreg |= f_pdata->inst_width << CQSPI_REG_RD_INSTR_TYPE_INSTR_LSB;
	rdreg |= f_pdata->addr_width << CQSPI_REG_RD_INSTR_TYPE_ADDR_LSB;
	rdreg |= f_pdata->data_width << CQSPI_REG_RD_INSTR_TYPE_DATA_LSB;

	return rdreg;
}

static int cqspi_wait_idle(struct cqspi_st *cqspi)
{
	const unsigned int poll_idle_retry = 3;
	unsigned int count = 0;
	unsigned long timeout;

	timeout = jiffies + msecs_to_jiffies(CQSPI_TIMEOUT_MS);
	while (1) {
		/*
		 * Read few times in succession to ensure the controller
		 * is indeed idle, that is, the bit does not transition
		 * low again.
		 */
		if (cqspi_is_idle(cqspi))
			count++;
		else
			count = 0;

		if (count >= poll_idle_retry)
			return 0;

		if (time_after(jiffies, timeout)) {
			/* Timeout, in busy mode. */
			dev_err(&cqspi->pdev->dev,
				"QSPI is still busy after %dms timeout.\n",
				CQSPI_TIMEOUT_MS);
			return -ETIMEDOUT;
		}

		cpu_relax();
	}
}

static int cqspi_exec_flash_cmd(struct cqspi_st *cqspi, unsigned int reg)
{
	void __iomem *reg_base = cqspi->iobase;
	int ret;

	/* Write the CMDCTRL without start execution. */
	writel(reg, reg_base + CQSPI_REG_CMDCTRL);
	/* Start execute */
	reg |= CQSPI_REG_CMDCTRL_EXECUTE_MASK;
	writel(reg, reg_base + CQSPI_REG_CMDCTRL);

	/* Polling for completion. */
	ret = cqspi_wait_for_bit(reg_base + CQSPI_REG_CMDCTRL,
				 CQSPI_REG_CMDCTRL_INPROGRESS_MASK, 1);
	if (ret) {
		dev_err(&cqspi->pdev->dev,
			"Flash command execution timed out.\n");

		if (ret < 0)
			return ret;
	}

	/* Polling QSPI idle status. */
	return cqspi_wait_idle(cqspi);
}

static int cqspi_command_read(struct spi_nor *nor,
			const u8 *txbuf, const unsigned int n_tx,
			u8 *rxbuf, const unsigned int n_rx)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *reg_base = cqspi->iobase;
	unsigned int rdreg;
	unsigned int reg;
	unsigned int read_len;
	int status;

	pr_debug("%s opcode:%x\n", __func__, txbuf[0]);
	if (!n_rx || n_rx > CQSPI_STIG_DATA_LEN_MAX || !rxbuf) {
		dev_err(nor->dev, "Invalid input argument, len %d rxbuf 0x%p\n",
			n_rx, rxbuf);
		return -EINVAL;
	}

	reg = txbuf[0] << CQSPI_REG_CMDCTRL_OPCODE_LSB;

	rdreg = cqspi_calc_rdreg(nor, txbuf[0]);
	writel(rdreg, reg_base + CQSPI_REG_RD_INSTR);

	reg |= (0x1 << CQSPI_REG_CMDCTRL_RD_EN_LSB);

	/* 0 means 1 byte. */
	reg |= (((n_rx - 1) & CQSPI_REG_CMDCTRL_RD_BYTES_MASK)
		<< CQSPI_REG_CMDCTRL_RD_BYTES_LSB);
	status = cqspi_exec_flash_cmd(cqspi, reg);
	if (status)
		return status;

	reg = readl(reg_base + CQSPI_REG_CMDREADDATALOWER);

	/* Put the read value into rx_buf */
	read_len = (n_rx > 4) ? 4 : n_rx;
	memcpy(rxbuf, &reg, read_len);
	rxbuf += read_len;
	pr_debug("reg0:%x\n", reg);

	if (n_rx > 4) {
		reg = readl(reg_base + CQSPI_REG_CMDREADDATAUPPER);

		read_len = n_rx - read_len;
		memcpy(rxbuf, &reg, read_len);
		pr_debug("reg1:%x\n", reg);
	}

	return 0;
}

static int cqspi_command_write(struct spi_nor *nor, const u8 opcode,
			const u8 *txbuf, const unsigned int n_tx)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *reg_base = cqspi->iobase;
	unsigned int reg;
	unsigned int data;
	int ret;

	pr_debug("%s opcode:%x", __func__, opcode);

	if (n_tx > 4 || (n_tx && !txbuf)) {
		dev_err(nor->dev,
			"Invalid input argument, cmdlen %d txbuf 0x%p\n",
			n_tx, txbuf);
		return -EINVAL;
	}

	reg = opcode << CQSPI_REG_CMDCTRL_OPCODE_LSB;
	if (n_tx) {
		reg |= (0x1 << CQSPI_REG_CMDCTRL_WR_EN_LSB);
		reg |= ((n_tx - 1) & CQSPI_REG_CMDCTRL_WR_BYTES_MASK)
			<< CQSPI_REG_CMDCTRL_WR_BYTES_LSB;
		data = 0;
		memcpy(&data, txbuf, n_tx);
		writel(data, reg_base + CQSPI_REG_CMDWRITEDATALOWER);
	}

	ret = cqspi_exec_flash_cmd(cqspi, reg);
	return ret;
}

static int cqspi_command_write_addr(struct spi_nor *nor,
				    const u8 opcode, const unsigned int addr)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *reg_base = cqspi->iobase;
	unsigned int reg;

	reg = opcode << CQSPI_REG_CMDCTRL_OPCODE_LSB;
	reg |= (0x1 << CQSPI_REG_CMDCTRL_ADDR_EN_LSB);
	reg |= ((nor->addr_width - 1) & CQSPI_REG_CMDCTRL_ADD_BYTES_MASK)
		<< CQSPI_REG_CMDCTRL_ADD_BYTES_LSB;

	writel(addr, reg_base + CQSPI_REG_CMDADDRESS);

	return cqspi_exec_flash_cmd(cqspi, reg);
}

static int cqspi_indirect_read_setup(struct spi_nor *nor,
				     const unsigned int from_addr)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *reg_base = cqspi->iobase;
	unsigned int dummy_clk = 0;
	unsigned int reg;

	writel(from_addr, reg_base + CQSPI_REG_INDIRECTRDSTARTADDR);

	reg = nor->read_opcode << CQSPI_REG_RD_INSTR_OPCODE_LSB;
	reg |= cqspi_calc_rdreg(nor, nor->read_opcode);

	/* Setup dummy clock cycles */
	dummy_clk = nor->read_dummy;
	if (dummy_clk > CQSPI_DUMMY_CLKS_MAX)
		return -EOPNOTSUPP;

	if (dummy_clk / 8) {
		reg |= (1 << CQSPI_REG_RD_INSTR_MODE_EN_LSB);
		/* Set mode bits high to ensure chip doesn't enter XIP */
		writel(0xFF, reg_base + CQSPI_REG_MODE_BIT);

		/* Need to subtract the mode byte (8 clocks). */
		if (f_pdata->inst_width != CQSPI_INST_TYPE_QUAD)
			dummy_clk -= 8;

		if (dummy_clk)
			reg |= (dummy_clk & CQSPI_REG_RD_INSTR_DUMMY_MASK)
			       << CQSPI_REG_RD_INSTR_DUMMY_LSB;
	}

	writel(reg, reg_base + CQSPI_REG_RD_INSTR);

	/* Set address width */
	reg = readl(reg_base + CQSPI_REG_SIZE);
	reg &= ~CQSPI_REG_SIZE_ADDRESS_MASK;
	reg |= (nor->addr_width - 1);
	writel(reg, reg_base + CQSPI_REG_SIZE);
	return 0;
}

static int cqspi_indirect_read_execute(struct spi_nor *nor,
			u8 *rxbuf, const unsigned int n_rx)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *reg_base = cqspi->iobase;
	void __iomem *ahb_base = cqspi->ahb_base + cqspi->trig_addr_off;
	unsigned int remaining = n_rx;
	unsigned int words;
	unsigned int bytes;
	unsigned int bytes_remain;
	unsigned int entries;
	int ret = 0;
	unsigned int temp;

	writel(remaining, reg_base + CQSPI_REG_INDIRECTRDBYTES);
	writel(CQSPI_REG_INDIRECTRD_START_MASK, reg_base + CQSPI_REG_INDIRECTRD);
	/* Clear all interrupts. */
	writel(CQSPI_IRQ_STATUS_MASK, reg_base + CQSPI_REG_IRQSTATUS);

	while (remaining > 0) {
#if 0
		/* enable interrupts */
		writel(CQSPI_IRQ_MASK_RD, reg_base + CQSPI_REG_IRQMASK);
		ret = wait_for_completion_timeout(&cqspi->transfer_complete,
				msecs_to_jiffies(CQSPI_READ_TIMEOUT_MS));
		if (!ret) {
			dev_err(nor->dev, "Indirect read timeout, no bytes\n");
			ret = -ETIMEDOUT;
			goto failrd;
		}
#endif
		entries = cqspi_get_rd_sram_level(cqspi);
		do {
			bytes = entries * cqspi->fifo_width;
			bytes = bytes > remaining ? remaining : bytes;
			pr_debug("bytes: %d, entries: %d\n", bytes, entries);
			words = bytes >> 2;
			bytes_remain = bytes - (words << 2);
			if (words) {
				/* Read 4 byte word chunks then single bytes */
				ioread32_rep(ahb_base, rxbuf, words);
				rxbuf += (words << 2);
			}

			if (bytes_remain) {
				temp = ioread32(ahb_base);
				memcpy(rxbuf, &temp, bytes_remain);
				rxbuf += bytes_remain;
			}

			remaining -= bytes;
			entries = cqspi_get_rd_sram_level(cqspi);
		} while (entries >= cqspi->fifo_rx_depth / 8);
	}

	/* Check indirect done status */
	ret = cqspi_wait_for_bit(reg_base + CQSPI_REG_INDIRECTRD,
				 CQSPI_REG_INDIRECTRD_DONE_MASK, 0);
	if (ret) {
		dev_err(nor->dev,
			"Indirect read completion error (%i)\n", ret);

		if (ret < 0)
			goto failrd;
		else
			ret = 0;
	}

	/* Disable interrupt */
	writel(0, reg_base + CQSPI_REG_IRQMASK);
	/* Clear indirect completion status */
	writel(CQSPI_REG_INDIRECTRD_DONE_MASK, reg_base + CQSPI_REG_INDIRECTRD);

	return 0;

failrd:
	/* Disable interrupt */
	writel(0, reg_base + CQSPI_REG_IRQMASK);
	/* Cancel the indirect read */
	writel(CQSPI_REG_INDIRECTWR_CANCEL_MASK,
	       reg_base + CQSPI_REG_INDIRECTRD);
	return ret;
}

static int cqspi_indirect_write_setup(struct spi_nor *nor,
				      const unsigned int to_addr)
{
	unsigned int reg;
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *reg_base = cqspi->iobase;

	/* Set opcode. */
	reg = nor->program_opcode << CQSPI_REG_WR_INSTR_OPCODE_LSB;
	if (cqspi->wr_en)
		reg |= CQSPI_REG_WR_INSTR_WEL_DIS_MASK;

	writel(reg, reg_base + CQSPI_REG_WR_INSTR);
	reg = cqspi_calc_rdreg(nor, nor->program_opcode);
	writel(reg, reg_base + CQSPI_REG_RD_INSTR);

	writel(to_addr, reg_base + CQSPI_REG_INDIRECTWRSTARTADDR);

	reg = readl(reg_base + CQSPI_REG_SIZE);
	reg &= ~CQSPI_REG_SIZE_ADDRESS_MASK;
	reg |= (nor->addr_width - 1);
	writel(reg, reg_base + CQSPI_REG_SIZE);

	if (cqspi->wr_completion) {
		reg = readl(reg_base + CQSPI_REG_WR_COMPLETION_CTRL);
		reg |= CQSPI_REG_WR_DISABLE_AUTO_POLL;
		writel(reg, reg_base + CQSPI_REG_WR_COMPLETION_CTRL);
	}
	return 0;
}

static int cqspi_indirect_write_execute(struct spi_nor *nor,
		const u8 *txbuf, const unsigned int n_tx)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *reg_base = cqspi->iobase;
	unsigned int remaining = n_tx;
	unsigned int write_bytes;
	int ret;

	writel(remaining, reg_base + CQSPI_REG_INDIRECTWRBYTES);

	/* Clear all interrupts. */
	writel(CQSPI_IRQ_STATUS_MASK, reg_base + CQSPI_REG_IRQSTATUS);

	writel(CQSPI_IRQ_MASK_WR, reg_base + CQSPI_REG_IRQMASK);

	if (remaining <= cqspi->fifo_tx_depth * cqspi->fifo_width)
		/*watermark disable*/
		writel(0x0, cqspi->iobase + CQSPI_REG_INDIRECTWRWATERMARK);
	else
		/* Program write watermark -- 1/4 of the TX FIFO. */
		writel(cqspi->fifo_tx_depth * cqspi->fifo_width / 4,
	       cqspi->iobase + CQSPI_REG_INDIRECTWRWATERMARK);

	reinit_completion(&cqspi->transfer_complete);
	writel(CQSPI_REG_INDIRECTWR_START_MASK,
	       reg_base + CQSPI_REG_INDIRECTWR);
	/*
	 * As per 66AK2G02 TRM SPRUHY8F section 11.15.5.3 Indirect Access
	 * Controller programming sequence, couple of cycles of
	 * QSPI_REF_CLK delay is required for the above bit to
	 * be internally synchronized by the QSPI module. Provide 5
	 * cycles of delay.
	 */
	if (cqspi->wr_delay)
		ndelay(cqspi->wr_delay);

	while (remaining > 0) {
		size_t write_words, mod_bytes;
		size_t max_size;

		max_size = cqspi->fifo_tx_depth * cqspi->fifo_width;
		write_bytes = (remaining > max_size)?max_size:remaining;
		write_words = write_bytes / 4;
		mod_bytes = write_bytes % 4;

		/* Write 4 bytes at a time then single bytes. */
		if (write_words) {
			iowrite32_rep(cqspi->ahb_base + cqspi->trig_addr_off,
				txbuf, write_words);
			txbuf += (write_words * 4);
		}
		if (mod_bytes) {
			unsigned int temp = 0xFFFFFFFF;

			memcpy(&temp, txbuf, mod_bytes);
			iowrite32(temp, cqspi->ahb_base + cqspi->trig_addr_off);
			txbuf += mod_bytes;
		}

		ret = wait_for_completion_timeout(&cqspi->transfer_complete,
						  msecs_to_jiffies
						  (CQSPI_TIMEOUT_MS));
		if (!ret) {
			dev_err(nor->dev, "Indirect write timeout\n");
			ret = -ETIMEDOUT;
			goto failwr;
		}

		remaining -= write_bytes;

		if (remaining > 0)
			reinit_completion(&cqspi->transfer_complete);
	}

	/* Check indirect done status */
	ret = cqspi_wait_for_bit(reg_base + CQSPI_REG_INDIRECTWR,
				 CQSPI_REG_INDIRECTWR_DONE_MASK, 0);
	if (ret) {
		dev_err(nor->dev,
			"Indirect write completion error (%i)\n", ret);

		if (ret < 0)
			goto failwr;
		else
			ret = 0;
	}

	/* Disable interrupt. */
	writel(0, reg_base + CQSPI_REG_IRQMASK);

	/* Clear indirect completion status */
	writel(CQSPI_REG_INDIRECTWR_DONE_MASK, reg_base + CQSPI_REG_INDIRECTWR);

	cqspi_wait_idle(cqspi);
	//printk("t2-t1=%d, t3-t2=%d, t4-t3=%d, t5-t4=%d, t6-t5=%d\n", t2-t1, t3-t2, t4-t3, t5-t4, t6-t5);
	return 0;

failwr:
	/* Disable interrupt. */
	writel(0, reg_base + CQSPI_REG_IRQMASK);

	/* Cancel the indirect write */
	writel(CQSPI_REG_INDIRECTWR_CANCEL_MASK,
	       reg_base + CQSPI_REG_INDIRECTWR);
	return ret;
}

static void cqspi_chipselect(struct spi_nor *nor)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *reg_base = cqspi->iobase;
	unsigned int chip_select = f_pdata->cs;
	unsigned int reg;

	reg = readl(reg_base + CQSPI_REG_CONFIG);
	if (cqspi->is_decoded_cs) {
		reg |= CQSPI_REG_CONFIG_DECODE_MASK;
	} else {
		reg &= ~CQSPI_REG_CONFIG_DECODE_MASK;

		/* Convert CS if without decoder.
		 * CS0 to 4b'1110
		 * CS1 to 4b'1101
		 * CS2 to 4b'1011
		 * CS3 to 4b'0111
		 */
		chip_select = 0xF & ~(1 << chip_select);
	}

	reg &= ~(CQSPI_REG_CONFIG_CHIPSELECT_MASK
		 << CQSPI_REG_CONFIG_CHIPSELECT_LSB);
	reg |= (chip_select & CQSPI_REG_CONFIG_CHIPSELECT_MASK)
	    << CQSPI_REG_CONFIG_CHIPSELECT_LSB;
	writel(reg, reg_base + CQSPI_REG_CONFIG);
}

static void cqspi_configure_cs_and_sizes(struct spi_nor *nor)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *iobase = cqspi->iobase;
	unsigned int reg;

	/* check paras, update to default value for illegal paras */
	unsigned int page_size = (nor->page_size != 0) ?
			nor->page_size : SPINOR_DEFAULT_PAGE_SIZE;
	unsigned int erasesize = (nor->mtd.erasesize != 0) ?
			nor->mtd.erasesize : SPINOR_DEFAULT_BLOCK_SIZE;
	uint8_t addr_width = (nor->addr_width != 0) ?
			nor->addr_width : SPINOR_DEFAULT_ADDRESS_BYTES;

	/* configure page size and block size. */
	reg = readl(iobase + CQSPI_REG_SIZE);
	reg &= ~(CQSPI_REG_SIZE_PAGE_MASK << CQSPI_REG_SIZE_PAGE_LSB);
	reg &= ~(CQSPI_REG_SIZE_BLOCK_MASK << CQSPI_REG_SIZE_BLOCK_LSB);
	reg &= ~CQSPI_REG_SIZE_ADDRESS_MASK;
	reg |= (page_size << CQSPI_REG_SIZE_PAGE_LSB);
	reg |= (ilog2(erasesize) << CQSPI_REG_SIZE_BLOCK_LSB);
	reg |= (addr_width - 1);
	writel(reg, iobase + CQSPI_REG_SIZE);

	/* configure the chip select */
	cqspi_chipselect(nor);

	/* Store the new configuration of the controller */
	cqspi->current_page_size = page_size;
	cqspi->current_erase_size = erasesize;
	cqspi->current_addr_width = addr_width;
}

static unsigned int calculate_ticks_for_ns(const unsigned int ref_clk_hz,
					   const unsigned int ns_val)
{
	unsigned int ticks;

	ticks = ref_clk_hz / 1000;	/* kHz */
	ticks = DIV_ROUND_UP(ticks * ns_val, 1000000);

	return ticks;
}

static void cqspi_delay(struct spi_nor *nor)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *iobase = cqspi->iobase;
	const unsigned int ref_clk_hz = cqspi->master_ref_clk_hz;
	unsigned int tshsl, tchsh, tslch, tsd2d;
	unsigned int reg;
	unsigned int tsclk;

	/* calculate the number of ref ticks for one sclk tick */
	tsclk = DIV_ROUND_UP(ref_clk_hz, cqspi->sclk);

	tshsl = calculate_ticks_for_ns(ref_clk_hz, f_pdata->tshsl_ns);
	/* this particular value must be at least one sclk */
	if (tshsl < tsclk)
		tshsl = tsclk;

	tchsh = calculate_ticks_for_ns(ref_clk_hz, f_pdata->tchsh_ns);
	tslch = calculate_ticks_for_ns(ref_clk_hz, f_pdata->tslch_ns);
	tsd2d = calculate_ticks_for_ns(ref_clk_hz, f_pdata->tsd2d_ns);

	reg = (tshsl & CQSPI_REG_DELAY_TSHSL_MASK)
	       << CQSPI_REG_DELAY_TSHSL_LSB;
	reg |= (tchsh & CQSPI_REG_DELAY_TCHSH_MASK)
		<< CQSPI_REG_DELAY_TCHSH_LSB;
	reg |= (tslch & CQSPI_REG_DELAY_TSLCH_MASK)
		<< CQSPI_REG_DELAY_TSLCH_LSB;
	reg |= (tsd2d & CQSPI_REG_DELAY_TSD2D_MASK)
		<< CQSPI_REG_DELAY_TSD2D_LSB;
	writel(reg, iobase + CQSPI_REG_DELAY);
}

static void cqspi_config_baudrate_div(struct cqspi_st *cqspi)
{
	const unsigned int ref_clk_hz = cqspi->master_ref_clk_hz;
	void __iomem *reg_base = cqspi->iobase;
	u32 reg, div;

	/* Recalculate the baudrate divisor based on QSPI specification. */
	div = DIV_ROUND_UP(ref_clk_hz, 2 * cqspi->sclk) - 1;

	reg = readl(reg_base + CQSPI_REG_CONFIG);
	reg &= ~(CQSPI_REG_CONFIG_BAUD_MASK << CQSPI_REG_CONFIG_BAUD_LSB);
	reg |= (div & CQSPI_REG_CONFIG_BAUD_MASK) << CQSPI_REG_CONFIG_BAUD_LSB;
	writel(reg, reg_base + CQSPI_REG_CONFIG);
}

static void cqspi_readdata_capture(struct cqspi_st *cqspi,
				   const unsigned int bypass,
				   const unsigned int delay)
{
	void __iomem *reg_base = cqspi->iobase;
	unsigned int reg;

	reg = readl(reg_base + CQSPI_REG_READCAPTURE);

	if (bypass)
		reg |= (1 << CQSPI_REG_READCAPTURE_BYPASS_LSB);
	else
		reg &= ~(1 << CQSPI_REG_READCAPTURE_BYPASS_LSB);

	reg &= ~(CQSPI_REG_READCAPTURE_DELAY_MASK
		 << CQSPI_REG_READCAPTURE_DELAY_LSB);

	reg |= (delay & CQSPI_REG_READCAPTURE_DELAY_MASK)
		<< CQSPI_REG_READCAPTURE_DELAY_LSB;

	writel(reg, reg_base + CQSPI_REG_READCAPTURE);
}

static void cqspi_controller_enable(struct cqspi_st *cqspi, bool enable)
{
	void __iomem *reg_base = cqspi->iobase;
	unsigned int reg;

	reg = readl(reg_base + CQSPI_REG_CONFIG);

	if (enable)
		reg |= CQSPI_REG_CONFIG_ENABLE_MASK;
	else
		reg &= ~CQSPI_REG_CONFIG_ENABLE_MASK;

	writel(reg, reg_base + CQSPI_REG_CONFIG);
}

static void cqspi_configure(struct spi_nor *nor)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	const unsigned int sclk = f_pdata->clk_rate;
	int switch_cs = (cqspi->current_cs != f_pdata->cs);
	int switch_ck = (cqspi->sclk != sclk);

	if ((cqspi->current_page_size != nor->page_size) ||
	    (cqspi->current_erase_size != nor->mtd.erasesize) ||
	    (cqspi->current_addr_width != nor->addr_width))
		switch_cs = 1;

	if (switch_cs || switch_ck)
		cqspi_controller_enable(cqspi, 0);

	/* Switch chip select. */
	if (switch_cs) {
		cqspi->current_cs = f_pdata->cs;
		cqspi_configure_cs_and_sizes(nor);
	}

	/* Setup baudrate divisor and delays */
	if (switch_ck) {
		cqspi->sclk = sclk;
		cqspi_config_baudrate_div(cqspi);
		cqspi_delay(nor);
		cqspi_readdata_capture(cqspi, 1, f_pdata->read_delay);
	}

	if (switch_cs || switch_ck)
		cqspi_controller_enable(cqspi, 1);
}

static int cqspi_set_protocol(struct spi_nor *nor, const int read)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;

	f_pdata->inst_width = CQSPI_INST_TYPE_SINGLE;
	f_pdata->addr_width = CQSPI_INST_TYPE_SINGLE;
	f_pdata->data_width = CQSPI_INST_TYPE_SINGLE;

	if (read) {
		switch (nor->read_proto) {
		case SNOR_PROTO_1_1_1:
			f_pdata->data_width = CQSPI_INST_TYPE_SINGLE;
			break;
		case SNOR_PROTO_1_1_2:
			f_pdata->data_width = CQSPI_INST_TYPE_DUAL;
			break;
		case SNOR_PROTO_1_1_4:
			f_pdata->data_width = CQSPI_INST_TYPE_QUAD;
			break;
		case SNOR_PROTO_1_1_8:
			f_pdata->data_width = CQSPI_INST_TYPE_OCTAL;
			break;
		default:
			return -EINVAL;
		}
	}

	cqspi_configure(nor);

	return 0;
}

static ssize_t cqspi_write(struct spi_nor *nor, loff_t to,
			   size_t len, const u_char *buf)
{
	int ret;

	pr_debug("%s %llx, len:%ld\n", __func__, to, (uintptr_t)len);

	ret = cqspi_set_protocol(nor, 0);
	if (ret)
		return ret;

	ret = cqspi_indirect_write_setup(nor, to);
	if (ret)
		return ret;

	ret = cqspi_indirect_write_execute(nor, buf, len);
	if (ret)
		return ret;

	return len;
}

static ssize_t cqspi_read(struct spi_nor *nor, loff_t from,
			  size_t len, u_char *buf)
{
	int ret;

	pr_debug("%s %llx, len:%ld\n", __func__, from, (uintptr_t)len);

	ret = cqspi_set_protocol(nor, 1);
	if (ret)
		return ret;

	ret = cqspi_indirect_read_setup(nor, from);
	if (ret)
		return ret;

	ret = cqspi_indirect_read_execute(nor, buf, len);
	if (ret)
		return ret;

	return len;
}

static int cqspi_erase(struct spi_nor *nor, loff_t offs)
{
	int ret;

	pr_debug("%s %llx\n", __func__, offs);

	ret = cqspi_set_protocol(nor, 0);
	if (ret)
		return ret;

	/* Send write enable, then erase commands. */
	ret = nor->write_reg(nor, SPINOR_OP_WREN, NULL, 0);
	if (ret)
		return ret;

	/* Set up command buffer. */
	ret = cqspi_command_write_addr(nor, nor->erase_opcode, offs);
	if (ret)
		return ret;

	return 0;
}

static int cqspi_prep(struct spi_nor *nor, enum spi_nor_ops ops)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;

	mutex_lock(&cqspi->bus_mutex);

	return 0;
}

static void cqspi_unprep(struct spi_nor *nor, enum spi_nor_ops ops)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;

	mutex_unlock(&cqspi->bus_mutex);
}

static int cqspi_read_reg(struct spi_nor *nor, u8 opcode, u8 *buf, int len)
{
	int ret;

	ret = cqspi_set_protocol(nor, 0);
	if (!ret)
		ret = cqspi_command_read(nor, &opcode, 1, buf, len);

	return ret;
}

static int cqspi_write_reg(struct spi_nor *nor, u8 opcode, u8 *buf, int len)
{
	int ret;

	ret = cqspi_set_protocol(nor, 0);
	if (!ret)
		ret = cqspi_command_write(nor, opcode, buf, len);

	return ret;
}

static int cqspi_of_get_flash_pdata(struct platform_device *pdev,
				    struct cqspi_flash_pdata *f_pdata,
				    struct device_node *np)
{
	if (of_property_read_u32(np, "cdns,read-delay", &f_pdata->read_delay)) {
		dev_err(&pdev->dev, "couldn't determine read-delay\n");
		return -ENXIO;
	}

	if (of_property_read_u32(np, "cdns,tshsl-ns", &f_pdata->tshsl_ns)) {
		dev_err(&pdev->dev, "couldn't determine tshsl-ns\n");
		return -ENXIO;
	}

	if (of_property_read_u32(np, "cdns,tsd2d-ns", &f_pdata->tsd2d_ns)) {
		dev_err(&pdev->dev, "couldn't determine tsd2d-ns\n");
		return -ENXIO;
	}

	if (of_property_read_u32(np, "cdns,tchsh-ns", &f_pdata->tchsh_ns)) {
		dev_err(&pdev->dev, "couldn't determine tchsh-ns\n");
		return -ENXIO;
	}

	if (of_property_read_u32(np, "cdns,tslch-ns", &f_pdata->tslch_ns)) {
		dev_err(&pdev->dev, "couldn't determine tslch-ns\n");
		return -ENXIO;
	}

	if (of_property_read_u32(np, "spi-max-frequency", &f_pdata->clk_rate)) {
		dev_err(&pdev->dev, "couldn't determine spi-max-frequency\n");
		return -ENXIO;
	}

	return 0;
}

static int cqspi_of_get_pdata(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct cqspi_st *cqspi = platform_get_drvdata(pdev);

	cqspi->is_decoded_cs = of_property_read_bool(np, "cdns,is-decoded-cs");

	if (of_property_read_u32(np, "cdns,fifo-depth", &cqspi->fifo_depth)) {
		dev_err(&pdev->dev, "couldn't determine fifo-depth\n");
		return -ENXIO;
	}

	if (of_property_read_u32(np, "cdns,fifo-width", &cqspi->fifo_width)) {
		dev_err(&pdev->dev, "couldn't determine fifo-width\n");
		return -ENXIO;
	}

	if (of_property_read_u32(np, "cdns,trigger-address",
				 &cqspi->trigger_address)) {
		dev_err(&pdev->dev, "couldn't determine trigger-address\n");
		return -ENXIO;
	}

	return 0;
}

static void cqspi_controller_init(struct cqspi_st *cqspi)
{
	u32 reg;

	cqspi_controller_enable(cqspi, 0);

	/* Configure the remap address register, no remap */
	writel(0, cqspi->iobase + CQSPI_REG_REMAP);

	/* Disable all interrupts. */
	writel(0, cqspi->iobase + CQSPI_REG_IRQMASK);

	/* Configure the SRAM split to 1:1 . */
	writel(cqspi->fifo_depth / 2, cqspi->iobase + CQSPI_REG_SRAMPARTITION);
	cqspi->fifo_rx_depth = cqspi->fifo_depth / 2;
	cqspi->fifo_tx_depth = cqspi->fifo_depth / 2;

	/* Load indirect trigger address. */
	writel(cqspi->trigger_address,
	       cqspi->iobase + CQSPI_REG_INDIRECTTRIGGER);

	/* Trigger Range 4096B*/
	writel(0xa, cqspi->iobase + CQSPI_REG_INDIRECT_TRIGGER_ADDR_RANGE);

	/* Program read watermark -- 1/2 of the FIFO. */
	writel(cqspi->fifo_depth * cqspi->fifo_width / 2,
	       cqspi->iobase + CQSPI_REG_INDIRECTRDWATERMARK);
	/* Program write watermark -- 1/8 of the FIFO. */
	writel(cqspi->fifo_depth * cqspi->fifo_width / 8,
	       cqspi->iobase + CQSPI_REG_INDIRECTWRWATERMARK);

	/* disable Direct Access Controller */
	reg = readl(cqspi->iobase + CQSPI_REG_CONFIG);
	reg &= ~CQSPI_REG_CONFIG_ENB_DIR_ACC_CTRL;
	writel(reg, cqspi->iobase + CQSPI_REG_CONFIG);

	cqspi_controller_enable(cqspi, 1);
}

static int cqspi_dump_controller(struct cqspi_st *cqspi, char *buf)
{
	struct cqspi_flash_pdata *flash = NULL;
	struct spi_nor *nor = NULL;
	u32 i;
	int off = 0;

	if (buf == NULL) {
		pr_err("buf is NULL\n");
		return 0;
	}

	if (cqspi == NULL) {
		off = sprintf(buf, "cqspi is NULL\n");
		return off;
	}

	off += sprintf(buf + off, "------dump controller begin------\n");
	off += sprintf(buf + off, "sclk 0x%x\n", cqspi->sclk);
	off += sprintf(buf + off, "iobase %p\n", cqspi->iobase);
	off += sprintf(buf + off, "ahb_base %p\n", cqspi->ahb_base);
	off += sprintf(buf + off,
		"current cs %d, page_size %d, erase_size %d, addr_width %d\n",
		cqspi->current_cs, cqspi->current_page_size,
		cqspi->current_erase_size, cqspi->current_addr_width);
	off += sprintf(buf + off, "master_ref_clk_hz %ld\n",
		cqspi->master_ref_clk_hz);
	off += sprintf(buf + off, "is_decoded_cs %d\n", cqspi->is_decoded_cs);
	off += sprintf(buf + off, "fifo_depth %d\n", cqspi->fifo_depth);
	off += sprintf(buf + off, "fifo_width %d\n", cqspi->fifo_width);
	off += sprintf(buf + off, "trigger_address 0x%x\n",
		cqspi->trigger_address);
	off += sprintf(buf + off, "irq_status 0x%x\n", cqspi->irq_status);
	for (i = 0; i < CQSPI_MAX_CHIPSELECT; i++) {
		flash = &cqspi->f_pdata[i];
		if (flash->registered) {
			off += sprintf(buf + off, "--<cs %d>--\n", flash->cs);
			off += sprintf(buf + off, "clk_rate %d\n",
				flash->clk_rate);
			off += sprintf(buf + off, "read_delay %d\n",
				flash->read_delay);
			off += sprintf(buf + off, "tshsl_ns %d\n",
				flash->tshsl_ns);
			off += sprintf(buf + off, "tsd2d_ns %d\n",
				flash->tsd2d_ns);
			off += sprintf(buf + off, "tchsh_ns %d\n",
				flash->tchsh_ns);
			off += sprintf(buf + off, "tslch_ns %d\n",
				flash->tslch_ns);
			off += sprintf(buf + off, "inst_width %d\n",
				flash->inst_width);
			off += sprintf(buf + off, "addr_width %d\n",
				flash->addr_width);
			off += sprintf(buf + off, "data_width %d\n",
				flash->data_width);

			nor = &flash->nor;
			off += sprintf(buf + off, "--nor info--\n");
			off += sprintf(buf + off, "page_size %d\n",
				nor->page_size);
			off += sprintf(buf + off, "addr_width %d\n",
				nor->addr_width);
			off += sprintf(buf + off, "erase_opcode   0x%x\n",
				nor->erase_opcode);
			off += sprintf(buf + off, "read_opcode    0x%x\n",
				nor->read_opcode);
			off += sprintf(buf + off, "read_dummy %d\n",
				nor->read_dummy);
			off += sprintf(buf + off, "program_opcode 0x%x\n",
				nor->program_opcode);
			off += sprintf(buf + off, "read_proto  0x%x\n",
				nor->read_proto);
			off += sprintf(buf + off, "write_proto 0x%x\n",
				nor->write_proto);
			off += sprintf(buf + off, "reg_proto   0x%x\n",
				nor->reg_proto);
			off += sprintf(buf + off, "flags 0x%x\n", nor->flags);
		}
	}
	off += sprintf(buf + off, "------dump controller end  ------\n");
	return off;
}

static int cqspi_dump_regs(struct cqspi_st *cqspi, char *buf)
{
	void __iomem *iobase = NULL;
	u32 val;
	u32 i;
	int off = 0;

	if (buf == NULL) {
		pr_err("buf is NULL\n");
		return 0;
	}

	if (cqspi == NULL) {
		off = sprintf(buf, "cqspi is NULL\n");
		return off;
	}
	iobase = cqspi->iobase;

	off += sprintf(buf + off, "------dump regs begin------\n");
	for (i = 0; i < 0x30; i += 4) {
		val = readl(iobase + i);
		off += sprintf(buf + off, "[%02x] 0x%x\n", i, val);
	}
	for (i = 0x60; i < 0x84; i += 4) {
		val = readl(iobase + i);
		off += sprintf(buf + off, "[%02x] 0x%x\n", i, val);
	}
	for (i = 0x90; i < 0x98; i += 4) {
		val = readl(iobase + i);
		off += sprintf(buf + off, "[%02x] 0x%x\n", i, val);
	}
	for (i = 0xa0; i < 0xa8; i += 4) {
		val = readl(iobase + i);
		off += sprintf(buf + off, "[%02x] 0x%x\n", i, val);
	}

	off += sprintf(buf + off, "------dump regs end  ------\n");
	return off;
}

static u32 cqspi_dbg_ctrl;
static ssize_t cqspi_dbg_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct cqspi_st *cqspi = g_cqspi;
	ssize_t rc = 0;

	if (cqspi_dbg_ctrl & 0x1)
		rc = cqspi_dump_regs(cqspi, buf);
	else
		rc = cqspi_dump_controller(cqspi, buf);
	cqspi_dbg_ctrl++;
	return rc;
}

static int cqspi_read_data_to_file(struct device *dev, u8 *data, size_t len)
{
	int ret;
	struct file *fp;
	mm_segment_t old_fs;
	loff_t pos;
	char *tmp_file = "/tmp/rd.bin";

	fp = filp_open(tmp_file, O_RDWR | O_CREAT, 0644);
	if (IS_ERR(fp)) {
		ret = PTR_ERR(fp);
		dev_info(dev, "open %s failed,err = %d\n", tmp_file, ret);
		return ret;
	}

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	pos = fp->f_pos;
	ret = kernel_write(fp, data, len, &pos);
	fp->f_pos = pos;

	set_fs(old_fs);
	filp_close(fp, NULL);
	dev_info(dev, "read to file %s ret 0x%x, size 0x%llx\n",
		tmp_file, ret, pos);
	return ret;
}

static ssize_t cqspi_dbg_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct spi_nor *nor = &g_cqspi->f_pdata[0].nor;
	struct device *curr_dev = nor->dev;
	struct cqspi_sysfs_cmd cmd_arr[] = {
		{SYSFS_CMD_READ_REG, "readreg"},
		{SYSFS_CMD_WRITE_REG, "writereg"},
		{SYSFS_CMD_READ_AHB, "readahb"},
		{SYSFS_CMD_READ_ID, "readid"},
		{SYSFS_CMD_READ_STATUS, "readstatus"},
		{SYSFS_CMD_ERASE_DATA, "erase"},
		{SYSFS_CMD_READ_DATA, "readdata"},
		{SYSFS_CMD_WRITE_DATA, "writedata"} };
	char *p = NULL;
	char *endp = NULL;
	u8 *test_buf = NULL;
	u32 op_offset, op_size, rd_val, wr_val, wr_pattern;
	u32 erasesize, sectors, i;
	bool found = false;
	int ret;
	u8 id[6];
	u8 val;

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
		dev_info(curr_dev, "Current commands supported:\n");
		dev_info(curr_dev, "readreg <offset>\n");
		dev_info(curr_dev, "writereg <offset> <value>\n");
		dev_info(curr_dev, "readahb <offset>\n");
		dev_info(curr_dev, "readid 0\n");
		dev_info(curr_dev, "readstatus 0\n");
		dev_info(curr_dev, "erase <offset> <length>\n");
		dev_info(curr_dev, "readdata <offset> <length>\n");
		dev_info(curr_dev, "writedata <offset> <length> <pattern>\n");
		return count;
	}

	op_offset = (u32)simple_strtoul(p + 1, &endp, 0);
	if (op_offset & 0x3) {
		dev_info(curr_dev, "offset 0x%x not align to 0x4\n", op_offset);
		return count;
	}

	switch (cmd_arr[i].cmd) {
	case SYSFS_CMD_READ_REG:
		rd_val = readl(g_cqspi->iobase + op_offset);
		dev_info(curr_dev, "reg [0x%02x] 0x%x\n", op_offset, rd_val);
		break;

	case SYSFS_CMD_WRITE_REG:
		if (endp == NULL) {
			dev_info(curr_dev, "not enough para, wr_val needed\n");
			return count;
		}

		wr_val = (u32)simple_strtoul(endp + 1, &endp, 0);
		writel(wr_val, g_cqspi->iobase + op_offset);
		dev_info(curr_dev, "wr reg [0x%02x] 0x%x\n", op_offset, wr_val);
		break;

	case SYSFS_CMD_READ_AHB:
		rd_val = readl(g_cqspi->ahb_base + op_offset);
		dev_info(curr_dev, "ahb [0x%02x] 0x%x\n", op_offset, rd_val);
		break;

	case SYSFS_CMD_READ_ID:
		ret = cqspi_read_reg(nor, SPINOR_OP_RDID, id, sizeof(id));
		if (ret < 0) {
			dev_info(curr_dev, "error %d reading ID\n", ret);
		} else {
			dev_info(curr_dev, "ID: %02x %02x %02x %02x %02x %02x\n",
				id[0], id[1], id[2], id[3], id[4], id[5]);
		}
		break;

	case SYSFS_CMD_READ_STATUS:
		ret = cqspi_read_reg(nor, SPINOR_OP_RDSR, &val, 1);
		if (ret < 0)
			dev_info(curr_dev, "error %d reading SR\n", ret);
		else
			dev_info(curr_dev, "SR 0x%x\n", val);
		ret = cqspi_read_reg(nor, SPINOR_OP_RDCR, &val, 1);
		if (ret < 0)
			dev_info(curr_dev, "error %d reading CR\n", ret);
		else
			dev_info(curr_dev, "CR 0x%x\n", val);
		ret = cqspi_read_reg(nor, SPINOR_OP_RDFSR, &val, 1);
		if (ret < 0)
			dev_info(curr_dev, "error %d reading FSR\n", ret);
		else
			dev_info(curr_dev, "FSR 0x%x\n", val);
		break;

	case SYSFS_CMD_ERASE_DATA:
		if (endp == NULL) {
			dev_info(curr_dev, "not enough para, op_size needed\n");
			break;
		}

		op_size = (u32)simple_strtoul(endp + 1, &endp, 0);
		erasesize = nor->mtd.erasesize;
		dev_info(curr_dev, "size = 0x%x, erasesize = 0x%x\n",
			op_size, erasesize);

		sectors = (erasesize != 0) ? (op_size / erasesize) :
			(op_size / SPINOR_DEFAULT_BLOCK_SIZE);
		for (i = 0; i < sectors; i++) {
			ret = cqspi_erase(nor, op_offset + i * erasesize);
			if (ret < 0) {
				dev_info(curr_dev, "error %d erase sector %d\n",
					ret, i);
				break;
			}
			mdelay(200); /* not accurate */
		}
		if (i == sectors)
			dev_info(curr_dev, "erase %d sectors done\n", sectors);
		break;

	case SYSFS_CMD_READ_DATA:
		if (endp == NULL) {
			dev_info(curr_dev, "not enough para, op_size needed\n");
			break;
		}

		op_size = (u32)simple_strtoul(endp + 1, &endp, 0);
		if (op_size > SYSFS_TEST_BUFFER_SIZE) {
			dev_info(curr_dev, "op_size too large\n");
			break;
		}

		test_buf = kzalloc(SYSFS_TEST_BUFFER_SIZE, GFP_KERNEL);
		if (!test_buf)
			break;

		ret = cqspi_read(nor, op_offset, op_size, test_buf);
		if (ret != op_size)
			dev_info(curr_dev, "error %d read data\n", ret);
		else
			cqspi_read_data_to_file(curr_dev, test_buf, op_size);
		kfree(test_buf);
		break;

	case SYSFS_CMD_WRITE_DATA:
		if (endp == NULL) {
			dev_info(curr_dev, "not enough para, op_size & wr_pattern needed\n");
			break;
		}
		op_size = (u32)simple_strtoul(endp + 1, &endp, 0);
		dev_info(curr_dev, "size = 0x%x\n", op_size);
		if (op_size > SYSFS_TEST_BUFFER_SIZE) {
			dev_info(curr_dev, "op_size too large\n");
			break;
		}

		if (endp == NULL) {
			dev_info(dev, "not enough para, wr_pattern needed\n");
			break;
		}
		wr_pattern = (u32)simple_strtoul(endp + 1, &endp, 0);
		dev_info(curr_dev, "pattern = 0x%x\n", wr_pattern);

		test_buf = kmalloc(SYSFS_TEST_BUFFER_SIZE, GFP_KERNEL);
		if (!test_buf)
			break;

		if (wr_pattern == 1) {
			memset(test_buf, 0xa5, op_size);
		} else if (wr_pattern == 2) {
			memset(test_buf, 0x81, op_size);
		} else {
			for (i = 0; i < op_size; i++)
				test_buf[i] = i & 0xff;
		}

		ret = cqspi_write(nor, op_offset, op_size, test_buf);
		if (ret != op_size)
			dev_info(curr_dev, "error %d write data\n", ret);
		kfree(test_buf);
		break;

	default:
		break;
	}

	return count;
}

static DEVICE_ATTR(cqspidbg, S_IRUGO | S_IWUSR,
	cqspi_dbg_show, cqspi_dbg_store);

static struct attribute *cqspidbg_attrs[] = {
	&dev_attr_cqspidbg.attr,
	NULL,
};

static struct attribute_group cqspidbg_attr_group = {
	.attrs = cqspidbg_attrs,
};
struct kobject *cqspi_node_device;

static int cqspi_create_sysfs(struct cqspi_st *cqspi)
{
	struct device *dev = &cqspi->pdev->dev;
	int ret;

	g_cqspi = cqspi;

	cqspi_node_device = kobject_create_and_add("cdns_qspi", NULL);
	if (cqspi_node_device == NULL) {
		dev_err(dev, "create sysfs node failed\n");
		return -ENOMEM;
	}

	ret = sysfs_create_group(cqspi_node_device, &cqspidbg_attr_group);
	if (ret) {
		dev_err(dev, "sysfs_create_group failed, ret %d\n", ret);
		kobject_put(cqspi_node_device);
		cqspi_node_device = NULL;
	}

	return ret;
}

static void cqspi_remove_sysfs(void)
{
	if (cqspi_node_device != NULL) {
		sysfs_remove_group(cqspi_node_device, &cqspidbg_attr_group);
		kobject_put(cqspi_node_device);
		cqspi_node_device = NULL;
	}
}

static int cqspi_setup_flash(struct cqspi_st *cqspi, struct device_node *np)
{
	const struct spi_nor_hwcaps hwcaps = {
		.mask = SNOR_HWCAPS_READ |
			SNOR_HWCAPS_READ_FAST |
			SNOR_HWCAPS_READ_1_1_2 |
			SNOR_HWCAPS_READ_1_1_4 |
			SNOR_HWCAPS_READ_1_1_8 |
			SNOR_HWCAPS_PP,
	};
	struct platform_device *pdev = cqspi->pdev;
	struct device *dev = &pdev->dev;
	struct cqspi_flash_pdata *f_pdata;
	struct spi_nor *nor;
	struct mtd_info *mtd;
	unsigned int cs;
	int i, ret;

	/* Get flash device data */
	for_each_available_child_of_node(dev->of_node, np) {
		ret = of_property_read_u32(np, "reg", &cs);
		if (ret) {
			dev_err(dev, "Couldn't determine chip select.\n");
			goto err;
		}

		if (cs >= CQSPI_MAX_CHIPSELECT) {
			ret = -EINVAL;
			dev_err(dev, "Chip select %d out of range.\n", cs);
			goto err;
		}

		f_pdata = &cqspi->f_pdata[cs];
		f_pdata->cqspi = cqspi;
		f_pdata->cs = cs;

		ret = cqspi_of_get_flash_pdata(pdev, f_pdata, np);
		if (ret)
			goto err;

		nor = &f_pdata->nor;
		mtd = &nor->mtd;

		mtd->priv = nor;

		nor->dev = dev;
		spi_nor_set_flash_node(nor, np);
		nor->priv = f_pdata;

		nor->read_reg = cqspi_read_reg;
		nor->write_reg = cqspi_write_reg;
		nor->read = cqspi_read;
		nor->write = cqspi_write;
		nor->erase = cqspi_erase;
		nor->prepare = cqspi_prep;
		nor->unprepare = cqspi_unprep;

		mtd->name = devm_kasprintf(dev, GFP_KERNEL, "%s.%d",
					   dev_name(dev), cs);
		if (!mtd->name) {
			ret = -ENOMEM;
			goto err;
		}

		ret = spi_nor_scan(nor, NULL, &hwcaps);
		if (ret) {
			dev_err(dev, "spi_nor_scan cs %d, ret %d\n", cs, ret);
			goto err;
		}

		ret = mtd_device_register(mtd, NULL, 0);
		if (ret) {
			dev_err(dev, "mtd_device_register cs %d, ret %d\n",
				cs, ret);
			goto err;
		}
		f_pdata->registered = true;
	}

	return 0;

err:
	for (i = 0; i < CQSPI_MAX_CHIPSELECT; i++)
		if (cqspi->f_pdata[i].registered)
			mtd_device_unregister(&cqspi->f_pdata[i].nor.mtd);
	return ret;
}

static int cqspi_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	struct cqspi_st *cqspi;
	struct resource *res;
	struct resource *res_ahb;
	struct reset_control *rstc_apb, *rstc_ahb, *rstc_ref;
	unsigned long data;
	int ret;
	int irq;

	cqspi = devm_kzalloc(dev, sizeof(*cqspi), GFP_KERNEL);
	if (!cqspi)
		return -ENOMEM;

	mutex_init(&cqspi->bus_mutex);
	cqspi->pdev = pdev;
	platform_set_drvdata(pdev, cqspi);

	/* Obtain configuration from OF. */
	ret = cqspi_of_get_pdata(pdev);
	if (ret) {
		dev_err(dev, "Cannot get mandatory OF data.\n");
		return -ENODEV;
	}
#ifndef CONFIG_ON_FPGA
	/* Obtain QSPI clock. */
	cqspi->clk = devm_clk_get(dev, NULL);
	if (IS_ERR(cqspi->clk)) {
		dev_err(dev, "Cannot claim QSPI clock.\n");
		return PTR_ERR(cqspi->clk);
	}

	cqspi->hclk = devm_clk_get(dev, "ospi_hclk");
	if (IS_ERR(cqspi->hclk)) {
		dev_err(dev, "Cannot claim QSPI hclock.\n");
		return PTR_ERR(cqspi->hclk);
	}

	ret = clk_prepare_enable(cqspi->hclk);
	if (ret) {
		dev_err(dev, "Cannot enable QSPI hclock.\n");
		return ret;
	}

	cqspi->pclk = devm_clk_get(dev, "ospi_pclk");
	if (IS_ERR(cqspi->pclk)) {
		dev_err(dev, "Cannot claim QSPI pclock.\n");
		return PTR_ERR(cqspi->pclk);
	}

	ret = clk_prepare_enable(cqspi->pclk);
	if (ret) {
		dev_err(dev, "Cannot enable QSPI pclock.\n");
		return ret;
	}

	pr_info("clock %s.\n", __clk_get_name(cqspi->clk));
#endif

	/* Obtain and remap controller address. */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	cqspi->iobase = devm_ioremap_resource(dev, res);
	if (IS_ERR(cqspi->iobase)) {
		dev_err(dev, "Cannot remap controller address.\n");
		return PTR_ERR(cqspi->iobase);
	}

	/* Obtain and remap AHB address. */
	res_ahb = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	cqspi->ahb_base = devm_ioremap_resource(dev, res_ahb);
	if (IS_ERR(cqspi->ahb_base)) {
		dev_err(dev, "Cannot remap AHB address.\n");
		return PTR_ERR(cqspi->ahb_base);
	}
	cqspi->trig_addr_off = cqspi->trigger_address - res_ahb->start;

	/* set_memory_ro dont use by module*/
#if 0
	/* temp, ro for direct address range */
	set_memory_ro((unsigned long)(cqspi->ahb_base), cqspi->trig_addr_off / PAGE_SIZE);
#endif
	init_completion(&cqspi->transfer_complete);

	/* Obtain IRQ line. */
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(dev, "Cannot obtain IRQ.\n");
		return -ENXIO;
	}

#ifndef CONFIG_ON_FPGA
	ret = clk_prepare_enable(cqspi->clk);
	if (ret) {
		dev_err(dev, "Cannot enable QSPI clock.\n");
		return ret;
	}

	cqspi->master_ref_clk_hz = clk_get_rate(cqspi->clk);
#else
	cqspi->master_ref_clk_hz = 20000000;
#endif
	pr_info("master_ref_clk_hz %lu\n", cqspi->master_ref_clk_hz);

	/* Obtain QSPI reset control */
	rstc_ahb = devm_reset_control_get_optional_exclusive(dev, "ospi_ahb");
	if (IS_ERR(rstc_ahb)) {
		ret = PTR_ERR(rstc_ahb);
		dev_err(dev, "Cannot get QSPI reset.\n");
		goto probe_reset_failed;
	}

	rstc_apb = devm_reset_control_get_optional_exclusive(dev, "ospi_apb");
	if (IS_ERR(rstc_apb)) {
		ret = PTR_ERR(rstc_apb);
		dev_err(dev, "Cannot get QSPI OCP reset.\n");
		goto probe_reset_failed;
	}

	rstc_ref = devm_reset_control_get_optional_exclusive(dev, "ospi_ref");
	if (IS_ERR(rstc_ref)) {
		ret = PTR_ERR(rstc_ref);
		dev_err(dev, "Cannot get QSPI REF clock reset.\n");
		goto probe_reset_failed;
	}

	reset_control_assert(rstc_ahb);
	reset_control_assert(rstc_apb);
	reset_control_assert(rstc_ref);
	ndelay(100);
	reset_control_deassert(rstc_ahb);
	reset_control_deassert(rstc_apb);
	reset_control_deassert(rstc_ref);

	/* write completion is supported by default */
	cqspi->wr_completion = true;
	cqspi->wr_en = true;

	data  = (unsigned long)of_device_get_match_data(dev);
	if (data & CQSPI_NEEDS_WR_DELAY)
		cqspi->wr_delay = 5 * DIV_ROUND_UP(NSEC_PER_SEC,
						   cqspi->master_ref_clk_hz);

	ret = devm_request_irq(dev, irq, cqspi_irq_handler, 0,
			       pdev->name, cqspi);
	if (ret) {
		dev_err(dev, "Cannot request IRQ.\n");
		goto probe_irq_failed;
	}

	cqspi_wait_idle(cqspi);
	cqspi_controller_init(cqspi);
	cqspi->current_cs = -1;
	cqspi->sclk = 0;

	cqspi_create_sysfs(cqspi);

	ret = cqspi_setup_flash(cqspi, np);
	if (ret) {
		dev_err(dev, "Cadence QSPI NOR probe failed %d\n", ret);
		goto probe_setup_failed;
	}

	return ret;
probe_irq_failed:
	cqspi_controller_enable(cqspi, 0);
probe_reset_failed:
probe_setup_failed:
	clk_disable_unprepare(cqspi->clk);
	return ret;
}

static int cqspi_remove(struct platform_device *pdev)
{
	struct cqspi_st *cqspi = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < CQSPI_MAX_CHIPSELECT; i++)
		if (cqspi->f_pdata[i].registered)
			mtd_device_unregister(&cqspi->f_pdata[i].nor.mtd);

	cqspi_controller_enable(cqspi, 0);

	clk_disable_unprepare(cqspi->clk);

	cqspi_remove_sysfs();

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int cqspi_suspend(struct device *dev)
{
	struct cqspi_st *cqspi = dev_get_drvdata(dev);

	cqspi_controller_enable(cqspi, 0);
	return 0;
}

static int cqspi_resume(struct device *dev)
{
	struct cqspi_st *cqspi = dev_get_drvdata(dev);

	cqspi_controller_enable(cqspi, 1);
	return 0;
}

static const struct dev_pm_ops cqspi__dev_pm_ops = {
	.suspend = cqspi_suspend,
	.resume = cqspi_resume,
};

#define CQSPI_DEV_PM_OPS	(&cqspi__dev_pm_ops)
#else
#define CQSPI_DEV_PM_OPS	NULL
#endif

static const struct of_device_id cqspi_dt_ids[] = {
	{
		.compatible = "cdns,qspi-nor",
		.data = (void *)0,
	},
	{
		.compatible = "ti,k2g-qspi",
		.data = (void *)CQSPI_NEEDS_WR_DELAY,
	},
	{ /* end of table */ }
};

MODULE_DEVICE_TABLE(of, cqspi_dt_ids);

static struct platform_driver cqspi_platform_driver = {
	.probe = cqspi_probe,
	.remove = cqspi_remove,
	.driver = {
		.name = CQSPI_NAME,
		.pm = CQSPI_DEV_PM_OPS,
		.of_match_table = cqspi_dt_ids,
	},
};

module_platform_driver(cqspi_platform_driver);

MODULE_DESCRIPTION("Cadence QSPI Controller Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" CQSPI_NAME);
MODULE_AUTHOR("Ley Foon Tan <lftan@altera.com>");
MODULE_AUTHOR("Graham Moore <grmoore@opensource.altera.com>");
