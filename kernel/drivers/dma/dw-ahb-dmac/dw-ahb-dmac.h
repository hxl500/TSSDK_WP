#ifndef _DW_AHB_DMA_H
#define _DW_AHB_DMA_H

/*
 * Driver for the Synopsys DesignWare AHB DMA Controller
 *
 * Copyright (C) 2005-2007 Atmel Corporation
 * Copyright (C) 2010-2011 ST Microelectronics
 * Copyright (C) 2016 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/dmaengine.h>
#include <linux/reset.h>

#include <linux/io-64-nonatomic-hi-lo.h>

#include "../virt-dma.h"

#define DW_AHB_DMA_MAX_NR_REQUESTS	16
#define DW_AHB_DMA_MAX_NR_MASTERS	4
#define DW_AHB_DMA_MAX_NR_CHANNELS	8
#define DW_AHB_DMA_MAX_BLK_SIZE		(4095)
#define MAX_BLOCK_SIZE				0x1000 /* 1024 blocks * 4 bytes data width */

/* flow controller */
enum dw_dma_fc {
	DW_DMA_FC_D_M2M,
	DW_DMA_FC_D_M2P,
	DW_DMA_FC_D_P2M,
	DW_DMA_FC_D_P2P,
	DW_DMA_FC_P_P2M,
	DW_DMA_FC_SP_P2P,
	DW_DMA_FC_P_M2P,
	DW_DMA_FC_DP_P2P,
};

/*
 * Redefine this macro to handle differences between 32- and 64-bit
 * addressing, big vs. little endian, etc.
 */
#define DW_REG(name)		u32 name; u32 __pad_##name

/* Hardware register definitions. */
struct dw_dma_chan_regs {
	DW_REG(SAR);		/* Source Address Register */
	DW_REG(DAR);		/* Destination Address Register */
	DW_REG(LLP);		/* Linked List Pointer */
	u32	CTL_LO;		/* Control Register Low */
	u32	CTL_HI;		/* Control Register High */
	DW_REG(SSTAT);
	DW_REG(DSTAT);
	DW_REG(SSTATAR);
	DW_REG(DSTATAR);
	u32	CFG_LO;		/* Configuration Register Low */
	u32	CFG_HI;		/* Configuration Register High */
	DW_REG(SGR);
	DW_REG(DSR);
};

struct dw_dma_irq_regs {
	DW_REG(XFER);
	DW_REG(BLOCK);
	DW_REG(SRC_TRAN);
	DW_REG(DST_TRAN);
	DW_REG(ERROR);
};

struct dw_ahb_dma_regs {
	/* per-channel registers */
	struct dw_dma_chan_regs	CHAN[DW_AHB_DMA_MAX_NR_CHANNELS];

	/* irq handling */
	struct dw_dma_irq_regs	RAW;		/* r */
	struct dw_dma_irq_regs	STATUS;		/* r (raw & mask) */
	struct dw_dma_irq_regs	MASK;		/* rw (set = irq enabled) */
	struct dw_dma_irq_regs	CLEAR;		/* w (ack, affects "raw") */

	DW_REG(STATUS_INT);			/* r */

	/* software handshaking */
	DW_REG(REQ_SRC);
	DW_REG(REQ_DST);
	DW_REG(SGL_REQ_SRC);
	DW_REG(SGL_REQ_DST);
	DW_REG(LAST_SRC);
	DW_REG(LAST_DST);

	/* miscellaneous */
	DW_REG(CFG);
	DW_REG(CH_EN);
	DW_REG(ID);
	DW_REG(TEST);

	/* iDMA 32-bit support */
	DW_REG(CLASS_PRIORITY0);
	DW_REG(CLASS_PRIORITY1);

	/* optional encoded params, 0x3c8..0x3f7 */
	u32	__reserved;

	/* per-channel configuration registers */
	u32	DWC_PARAMS[DW_AHB_DMA_MAX_NR_CHANNELS];
	u32	MULTI_BLK_TYPE;
	u32	MAX_BLK_SIZE;

	/* top-level parameters */
	u32	DW_PARAMS;

	/* component ID */
	u32	COMP_TYPE;
	u32	COMP_VERSION;

	/* iDMA 32-bit support */
	DW_REG(FIFO_PARTITION0);
	DW_REG(FIFO_PARTITION1);

	DW_REG(SAI_ERR);
	DW_REG(GLOBAL_CFG);
};

/*
 * Big endian I/O access when reading and writing to the DMA controller
 * registers.  This is needed on some platforms, like the Atmel AVR32
 * architecture.
 */

#ifdef CONFIG_DW_DMAC_BIG_ENDIAN_IO
#define dma_readl_native ioread32be
#define dma_writel_native iowrite32be
#else
#define dma_readl_native readl
#define dma_writel_native writel
#endif

/* Bitfields in DW_PARAMS */
#define DW_PARAMS_NR_CHAN	8		/* number of channels */
#define DW_PARAMS_NR_MASTER	11		/* number of AHB masters */
#define DW_PARAMS_DATA_WIDTH(n)	(15 + 2 * (n))
#define DW_PARAMS_DATA_WIDTH1	15		/* master 1 data width */
#define DW_PARAMS_DATA_WIDTH2	17		/* master 2 data width */
#define DW_PARAMS_DATA_WIDTH3	19		/* master 3 data width */
#define DW_PARAMS_DATA_WIDTH4	21		/* master 4 data width */
#define DW_PARAMS_EN		28		/* encoded parameters */

/* Bitfields in DWC_PARAMS */
#define DWC_PARAMS_MBLK_EN	11		/* multi block transfer */

/* bursts size */
enum dw_dma_msize {
	DW_DMA_MSIZE_1,
	DW_DMA_MSIZE_4,
	DW_DMA_MSIZE_8,
	DW_DMA_MSIZE_16,
	DW_DMA_MSIZE_32,
	DW_DMA_MSIZE_64,
	DW_DMA_MSIZE_128,
	DW_DMA_MSIZE_256,
};

/* Bitfields in LLP */
#define DWC_LLP_LMS(x)		((x) & 3)	/* list master select */
#define DWC_LLP_LOC(x)		((x) & ~3)	/* next lli */

/* Bitfields in CTL_LO */
enum {
	DW_DMA_TRANS_WIDTH_8		= 0,
	DW_DMA_TRANS_WIDTH_16,
	DW_DMA_TRANS_WIDTH_32,
	DW_DMA_TRANS_WIDTH_64,
	DW_DMA_TRANS_WIDTH_128,
	DW_DMA_TRANS_WIDTH_256,
	DW_DMA_TRANS_WIDTH_MAX	= DW_DMA_TRANS_WIDTH_256
};

#define DWC_CTLL_INT_EN		(1 << 0)	/* irqs enabled? */
#define DWC_CTLL_DST_WIDTH(n)	((n)<<1)	/* bytes per element */
#define DWC_CTLL_SRC_WIDTH(n)	((n)<<4)
#define DWC_CTLL_DST_INC	(0<<7)		/* DAR update/not */
#define DWC_CTLL_DST_DEC	(1<<7)
#define DWC_CTLL_DST_FIX	(2<<7)
#define DWC_CTLL_SRC_INC	(0<<9)		/* SAR update/not */
#define DWC_CTLL_SRC_DEC	(1<<9)
#define DWC_CTLL_SRC_FIX	(2<<9)
#define DWC_CTLL_DST_MSIZE(n)	((n)<<11)	/* burst, #elements */
#define DWC_CTLL_SRC_MSIZE(n)	((n)<<14)
#define DWC_CTLL_S_GATH_EN	(1 << 17)	/* src gather, !FIX */
#define DWC_CTLL_D_SCAT_EN	(1 << 18)	/* dst scatter, !FIX */
#define DWC_CTLL_FC(n)		((n) << 20)
#define DWC_CTLL_FC_M2M		(0 << 20)	/* mem-to-mem */
#define DWC_CTLL_FC_M2P		(1 << 20)	/* mem-to-periph */
#define DWC_CTLL_FC_P2M		(2 << 20)	/* periph-to-mem */
#define DWC_CTLL_FC_P2P		(3 << 20)	/* periph-to-periph */
/* plus 4 transfer types for peripheral-as-flow-controller */
#define DWC_CTLL_DMS(n)		((n)<<23)	/* dst master select */
#define DWC_CTLL_SMS(n)		((n)<<25)	/* src master select */
#define DWC_CTLL_LLP_D_EN	(1 << 27)	/* dest block chain */
#define DWC_CTLL_LLP_S_EN	(1 << 28)	/* src block chain */

/* Bitfields in CTL_HI */
#define DWC_CTLH_BLOCK_TS_MASK	GENMASK(11, 0)
#define DWC_CTLH_BLOCK_TS(x)	((x) & DWC_CTLH_BLOCK_TS_MASK)
#define DWC_CTLH_DONE		(1 << 12)

/* Bitfields in CFG_LO */
#define DWC_CFGL_CH_PRIOR_MASK	(0x7 << 5)	/* priority mask */
#define DWC_CFGL_CH_PRIOR(x)	((x) << 5)	/* priority */
#define DWC_CFGL_CH_SUSP	(1 << 8)	/* pause xfer */
#define DWC_CFGL_FIFO_EMPTY	(1 << 9)	/* pause xfer */
#define DWC_CFGL_HS_DST		(1 << 10)	/* handshake w/dst */
#define DWC_CFGL_HS_SRC		(1 << 11)	/* handshake w/src */
#define DWC_CFGL_LOCK_CH_XFER	(0 << 12)	/* scope of LOCK_CH */
#define DWC_CFGL_LOCK_CH_BLOCK	(1 << 12)
#define DWC_CFGL_LOCK_CH_XACT	(2 << 12)
#define DWC_CFGL_LOCK_BUS_XFER	(0 << 14)	/* scope of LOCK_BUS */
#define DWC_CFGL_LOCK_BUS_BLOCK	(1 << 14)
#define DWC_CFGL_LOCK_BUS_XACT	(2 << 14)
#define DWC_CFGL_LOCK_CH	(1 << 15)	/* channel lockout */
#define DWC_CFGL_LOCK_BUS	(1 << 16)	/* busmaster lockout */
#define DWC_CFGL_HS_DST_POL	(1 << 18)	/* dst handshake active low */
#define DWC_CFGL_HS_SRC_POL	(1 << 19)	/* src handshake active low */
#define DWC_CFGL_MAX_BURST(x)	((x) << 20)
#define DWC_CFGL_RELOAD_SAR	(1 << 30)
#define DWC_CFGL_RELOAD_DAR	(1 << 31)

/* Bitfields in CFG_HI */
#define DWC_CFGH_FCMODE		(1 << 0)
#define DWC_CFGH_FIFO_MODE	(1 << 1)
#define DWC_CFGH_PROTCTL(x)	((x) << 2)
#define DWC_CFGH_PROTCTL_DATA	(0 << 2)	/* data access - always set */
#define DWC_CFGH_PROTCTL_PRIV	(1 << 2)	/* privileged -> AHB HPROT[1] */
#define DWC_CFGH_PROTCTL_BUFFER	(2 << 2)	/* bufferable -> AHB HPROT[2] */
#define DWC_CFGH_PROTCTL_CACHE	(4 << 2)	/* cacheable  -> AHB HPROT[3] */
#define DWC_CFGH_DS_UPD_EN	(1 << 5)
#define DWC_CFGH_SS_UPD_EN	(1 << 6)
#define DWC_CFGH_SRC_PER(x)	((x) << 7)
#define DWC_CFGH_DST_PER(x)	((x) << 11)

/* Bitfields in SGR */
#define DWC_SGR_SGI(x)		((x) << 0)
#define DWC_SGR_SGC(x)		((x) << 20)

/* Bitfields in DSR */
#define DWC_DSR_DSI(x)		((x) << 0)
#define DWC_DSR_DSC(x)		((x) << 20)

/* Bitfields in CFG */
#define DW_CFG_DMA_EN		(1 << 0)

enum virtual_mic_type {
	VIRTUAL_MIC_TYPE_NORMAL 			= 0,
	VIRTUAL_MIC_TYPE_INTER_CODEC_V1 	= 1,
	VIRTUAL_MIC_TYPE_I2S				= 2,
	VIRTUAL_MIC_TYPE_PDM				= 3,
};

#define VIRTUAL_TYPE_MAX VIRTUAL_MIC_TYPE_PDM

struct dw_ahb_dma_chip;
struct dw_ahb_dma_chan {
	struct dw_ahb_dma_chip	*chip;
	struct dma_chan	 chan;
	void __iomem *chan_regs;
	u8	mask;
	u8	id;
	u8	priority;

	enum virtual_mic_type virt_mic_type;
	u8 pre_buf_cnt;

	enum dma_transfer_direction	direction;
	u8	hw_handshake_num;

	struct dma_pool	*desc_pool;
	struct virt_dma_chan vc;

	atomic_t descs_allocated;
	bool cyclic;
	bool is_paused;

	struct dma_slave_config config;
};

static inline struct dw_dma_chan_regs __iomem *
__dwc_regs(struct dw_ahb_dma_chan *chan)
{
	return chan->chan_regs;
}

#define channel_readl(dwc, name) \
	dma_readl_native(&(__dwc_regs(dwc)->name))
#define channel_writel(dwc, name, val) \
	dma_writel_native((val), &(__dwc_regs(dwc)->name))

/**
 * struct dw_dma_platform_data - Controller configuration parameters
 * @nr_channels: Number of channels supported by hardware (max 8)
 * @is_private: The device channels should be marked as private and not for
 *	by the general purpose DMA channel allocator.
 * @is_memcpy: The device channels do support memory-to-memory transfers.
 * @is_idma32: The type of the DMA controller is iDMA32
 * @chan_allocation_order: Allocate channels starting from 0 or 7
 * @chan_priority: Set channel priority increasing from 0 to 7 or 7 to 0.
 * @block_size: Maximum block size supported by the controller
 * @nr_masters: Number of AHB masters supported by the controller
 * @data_width: Maximum data width supported by hardware per AHB master
 *		(in bytes, power of 2)
 * @multi_block: Multi block transfers supported by hardware per channel.
 * @protctl: Protection control signals setting per channel.
 */
struct dw_ahb_dma_hcfg {
	unsigned int	nr_channels;
	bool		is_private;
	bool		is_memcpy;
#define CHAN_ALLOCATION_ASCENDING	0	/* zero to seven */
#define CHAN_ALLOCATION_DESCENDING	1	/* seven to zero */
	unsigned char	chan_allocation_order;
#define CHAN_PRIORITY_ASCENDING		0	/* chan0 highest */
#define CHAN_PRIORITY_DESCENDING	1	/* chan7 highest */
	unsigned char	chan_priority;
	unsigned int	block_size;
	unsigned char	nr_masters;
	unsigned char	mem_master;
	unsigned char	dev_master;
	unsigned char	data_width[DW_AHB_DMA_MAX_NR_MASTERS];
	unsigned char	multi_block[DW_AHB_DMA_MAX_NR_CHANNELS];
#define CHAN_PROTCTL_PRIVILEGED		BIT(0)
#define CHAN_PROTCTL_BUFFERABLE		BIT(1)
#define CHAN_PROTCTL_CACHEABLE		BIT(2)
#define CHAN_PROTCTL_MASK		GENMASK(2, 0)
	unsigned char	protctl;
};

struct dw_ahb_dma_chan_config {
	u8 dst_multblk_type;
	u8 src_multblk_type;
	u8 dst_per;
	u8 src_per;
	u8 tt_fc;
	u8 prior;
	u8 hs_sel_dst;
	u8 hs_sel_src;
};

struct dw_ahb_dma {
	struct dma_device	dma;
	struct dw_ahb_dma_hcfg	*hdata;
	struct device_dma_parameters	dma_parms;
	/* channels */
	struct dw_ahb_dma_chan	*chan;

	u8	all_chan_mask;
	u8	in_use;
};

struct dw_ahb_dma_chip {
	struct device	*dev;
	int		id;
	int		irq;
	void __iomem *regs;
	struct clk	*clk;
	struct dw_ahb_dma *dw;
	struct reset_control *rstc;
};

static inline struct dw_ahb_dma_regs __iomem *__dw_regs(struct dw_ahb_dma_chip *dw)
{
	return dw->regs;
}

#define dma_readl(dw, name) \
	dma_readl_native(&(__dw_regs(dw)->name))
#define dma_writel(dw, name, val) \
	dma_writel_native((val), &(__dw_regs(dw)->name))

#define idma32_readq(dw, name)				\
	hi_lo_readq(&(__dw_regs(dw)->name))
#define idma32_writeq(dw, name, val)			\
	hi_lo_writeq((val), &(__dw_regs(dw)->name))

#define channel_set_bit(dw, reg, mask) \
	dma_writel(dw, reg, ((mask) << 8) | (mask))
#define channel_clear_bit(dw, reg, mask) \
	dma_writel(dw, reg, ((mask) << 8) | 0)

static inline struct dw_ahb_dma *to_dw_dma(struct dma_device *ddev)
{
	return container_of(ddev, struct dw_ahb_dma, dma);
}

#ifdef CONFIG_DW_DMAC_BIG_ENDIAN_IO
typedef __be32 __dw32;
#else
typedef __le32 __dw32;
#endif

/* LLI == Linked List Item; a.k.a. DMA block descriptor */
struct dw_ahb_dma_lli {
	/* values that are not changed by hardware */
	__dw32		sar;
	__dw32		dar;
	__dw32		llp;		/* chain to next lli */
	__dw32		ctllo;
	/* values that may get written back: */
	__dw32		ctlhi;
	/* sstat and dstat can snapshot peripheral register state.
	 * silicon config may discard either or both...
	 */
	__dw32		sstat;
	__dw32		dstat;
};

#ifdef CONFIG_DW_DMAC_BIG_ENDIAN_IO
#define lli_set(d, reg, v)		((d)->lli->reg |= cpu_to_be32(v))
#define lli_clear(d, reg, v)		((d)->lli->reg &= ~cpu_to_be32(v))
#define lli_read(d, reg)		be32_to_cpu((d)->lli->reg)
#define lli_write(d, reg, v)		((d)->lli->reg = cpu_to_be32(v))
#else
#define lli_set(d, reg, v)		((d)->lli->reg |= cpu_to_le32(v))
#define lli_clear(d, reg, v)		((d)->lli->reg &= ~cpu_to_le32(v))
#define lli_read(d, reg)		le32_to_cpu((d)->lli->reg)
#define lli_write(d, reg, v)		((d)->lli->reg = cpu_to_le32(v))
#endif

struct dw_desc {
	/* FIRST values the hardware uses */
	/* THEN values for driver housekeeping */
	struct list_head		desc_node;
	struct list_head		tx_list;
	struct dma_async_tx_descriptor	txd;
	size_t				len;
	size_t				total_len;
	u32				residue;
};

#define to_dw_desc(h)	list_entry(h, struct dw_desc, desc_node)

static inline struct dw_desc *
txd_to_dw_desc(struct dma_async_tx_descriptor *txd)
{
	return container_of(txd, struct dw_desc, txd);
}

struct dw_ahb_dma_hw_desc {
	struct dw_ahb_dma_lli *lli;
	dma_addr_t	llp;
	u32	len;
};

struct dw_ahb_dma_desc {
	struct dw_ahb_dma_hw_desc	*hw_desc;
	struct virt_dma_desc		vd;
	struct dw_ahb_dma_chan		*chan;
	int	completed_blocks;
	u32	length;
	u32	period_len;
};

static inline struct device *chan2dev(struct dw_ahb_dma_chan *chan)
{
	return &chan->vc.chan.dev->device;
}

static inline struct device *dchan2dev(struct dma_chan *dchan)
{
	return &dchan->dev->device;
}

static inline const char *dw_ahb_chan_name(struct dw_ahb_dma_chan *chan)
{
	return dma_chan_name(&chan->vc.chan);
}

static inline struct dw_ahb_dma_desc *vd_to_ahb_dma_desc(struct virt_dma_desc *vd)
{
	return container_of(vd, struct dw_ahb_dma_desc, vd);
}

static inline struct dw_ahb_dma_chan *
			to_dw_ahb_dma_chan(struct dma_chan *chan)
{
	return container_of(chan, struct dw_ahb_dma_chan, chan);
}

static inline struct dw_ahb_dma_chan *vc_to_dw_ahb_dma_chan(struct virt_dma_chan *vc)
{
	return container_of(vc, struct dw_ahb_dma_chan, vc);
}

static inline struct dw_ahb_dma_chan *dchan_to_dw_ahb_dma_chan(struct dma_chan *dchan)
{
	return vc_to_dw_ahb_dma_chan(to_virt_chan(dchan));
}
#endif
