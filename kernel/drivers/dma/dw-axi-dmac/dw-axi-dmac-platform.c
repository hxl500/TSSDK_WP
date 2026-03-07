// SPDX-License-Identifier:  GPL-2.0
// (C) 2017-2018 Synopsys, Inc. (www.synopsys.com)

/*
 * Synopsys DesignWare AXI DMA Controller driver.
 *
 * Author: Eugeniy Paltsev <Eugeniy.Paltsev@synopsys.com>
 */

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dmaengine.h>
#include <linux/dmapool.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/io-64-nonatomic-lo-hi.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_dma.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/property.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <sound/pcm.h>

#include "dw-axi-dmac.h"
#include "../dmaengine.h"
#include "../virt-dma.h"

static volatile int g_dbg_flag = 0;

/*
 * The set of bus widths supported by the DMA controller. DW AXI DMAC supports
 * master data bus width up to 512 bits (for both AXI master interfaces), but
 * it depends on IP block configurarion.
 */
#define AXI_DMA_BUSWIDTHS		  \
	(DMA_SLAVE_BUSWIDTH_1_BYTE	| \
	DMA_SLAVE_BUSWIDTH_2_BYTES	| \
	DMA_SLAVE_BUSWIDTH_4_BYTES	| \
	DMA_SLAVE_BUSWIDTH_8_BYTES	| \
	DMA_SLAVE_BUSWIDTH_16_BYTES	| \
	DMA_SLAVE_BUSWIDTH_32_BYTES	| \
	DMA_SLAVE_BUSWIDTH_64_BYTES)


#define MAX_BUF_CNT (16)
struct virtual_mic_cfg {
	unsigned char play_open;
	unsigned char cap_open;
	unsigned char play_pre_buf_cnt;
	unsigned char play_r;
	unsigned char play_w;
	unsigned char buf_cnt;
	unsigned char *buffer[MAX_BUF_CNT];
	unsigned int virt_mic_offset;
	unsigned int virt_mic_len;
	unsigned char buffer_used[MAX_BUF_CNT];
	void *play_addr[MAX_BUF_CNT];
	void *cap_addr[MAX_BUF_CNT];
	unsigned int cap_block_ts_div;

	struct axi_dma_chan *tx_chan;
	unsigned char play_start_flag;
};

static struct virtual_mic_cfg virt_mic_cfg;
static inline void axi_chan_enable(struct axi_dma_chan *chan);
static int axi_dma_virt_mic_init(struct axi_dma_chan *chan,
						size_t buf_len, size_t period_len)
{
	int i;

	if (chan->virt_mic_type == VIRTUAL_MIC_TYPE_NORMAL)
		return 0;

	if (chan->cyclic && chan->direction == DMA_MEM_TO_DEV) {
		virt_mic_cfg.play_pre_buf_cnt = chan->pre_buf_cnt;
		virt_mic_cfg.play_open = 0;
		virt_mic_cfg.play_w = 0;
		virt_mic_cfg.play_r = 0;
	} else if (chan->cyclic && chan->direction == DMA_DEV_TO_MEM) {
		switch (chan->virt_mic_type) {
		case VIRTUAL_MIC_TYPE_INTER_CODEC_V1:
			virt_mic_cfg.buf_cnt = buf_len / period_len;
			virt_mic_cfg.virt_mic_offset  = period_len / 2;
			virt_mic_cfg.virt_mic_len	  = period_len / 2;
			break;
		case VIRTUAL_MIC_TYPE_I2S:
			virt_mic_cfg.buf_cnt = buf_len / period_len;
			virt_mic_cfg.virt_mic_offset  = period_len / 2;
			virt_mic_cfg.virt_mic_len	  = period_len / 2;
			break;
		case VIRTUAL_MIC_TYPE_PDM:
			virt_mic_cfg.buf_cnt = buf_len / period_len;
			virt_mic_cfg.virt_mic_offset  = period_len / 2;
			virt_mic_cfg.virt_mic_len	  = period_len / 2;
			break;
		default:
			break;
		}

		if (virt_mic_cfg.buf_cnt > MAX_BUF_CNT) {
			pr_err("virt_mic_cfg.buf_cnt max is %d\r\n", MAX_BUF_CNT);
			return -1;
		}

		for (i = 0; i < virt_mic_cfg.buf_cnt; i++) {
			if (virt_mic_cfg.buffer[i]) {
				kfree(virt_mic_cfg.buffer[i]);
			}

			virt_mic_cfg.buffer[i] = (unsigned char *)kzalloc(virt_mic_cfg.virt_mic_len, GFP_KERNEL);
			if (!virt_mic_cfg.buffer[i]) {
				pr_err("virt_mic_cfg.buffer[%d] malloc failed\r\n", i);
				return -1;
			}
		}
		memset(&virt_mic_cfg.cap_addr, 0, sizeof(virt_mic_cfg.cap_addr));
	}

	return 0;
}

static size_t axi_dma_virt_mic_get_real_period_len(struct axi_dma_chan *chan,
				size_t period_len)
{
	size_t real_period_len;

	if (chan->virt_mic_type == VIRTUAL_MIC_TYPE_NORMAL)
		return period_len;

	real_period_len = period_len;

	if (chan->cyclic && chan->direction == DMA_DEV_TO_MEM) {
		switch (chan->virt_mic_type) {
		case VIRTUAL_MIC_TYPE_INTER_CODEC_V1:
			real_period_len = period_len / 2;
			break;
		case VIRTUAL_MIC_TYPE_I2S:
			real_period_len = period_len / 2;
			break;
		case VIRTUAL_MIC_TYPE_PDM:
			real_period_len = period_len / 2;
			break;
		default:
			break;
		}
	}
	return real_period_len;
}

static void axi_dma_virt_mic_set_cfg(struct axi_dma_chan *chan,
				struct snd_pcm_substream *subtream, int addr_idx, u32 buf_len)
{
	if (chan->virt_mic_type == VIRTUAL_MIC_TYPE_NORMAL)
		return;

	if (chan->cyclic && chan->direction == DMA_MEM_TO_DEV) {
		virt_mic_cfg.play_addr[addr_idx] =
					subtream->runtime->dma_area + addr_idx * buf_len;
		//memset(virt_mic_cfg.play_addr[addr_idx], 0, virt_mic_cfg.virt_mic_len);
	} else if (chan->cyclic && chan->direction == DMA_DEV_TO_MEM) {
		virt_mic_cfg.cap_addr[addr_idx] =
					subtream->runtime->dma_area + addr_idx * buf_len;
	}
}

static void
axi_dma_virt_mic_set_block_ts(struct axi_dma_chan *chan,
												size_t *block_ts)
{
	if (chan->virt_mic_type == VIRTUAL_MIC_TYPE_NORMAL)
		return;

	if (chan->cyclic && chan->direction == DMA_DEV_TO_MEM) {
		switch (chan->virt_mic_type) {
		case VIRTUAL_MIC_TYPE_INTER_CODEC_V1:
			*block_ts = *block_ts / 2;
			break;
		case VIRTUAL_MIC_TYPE_I2S:
			*block_ts = *block_ts / 2;
			break;
		case VIRTUAL_MIC_TYPE_PDM:
			*block_ts = *block_ts / 2;
			break;
		default:
			break;
		}
	}
}

static void axi_dma_virt_mic_tx_start(struct axi_dma_chan *chan)
{
	int i;

	if (chan->virt_mic_type == VIRTUAL_MIC_TYPE_NORMAL)
		return;

	if (chan->cyclic && chan->direction == DMA_MEM_TO_DEV) {
		if (!virt_mic_cfg.cap_open)
			return;

		for (i = 0; i < virt_mic_cfg.play_pre_buf_cnt; i++) {
			if (virt_mic_cfg.buffer[virt_mic_cfg.play_w]) {
				memcpy(virt_mic_cfg.buffer[virt_mic_cfg.play_w],
						virt_mic_cfg.play_addr[i],
						virt_mic_cfg.virt_mic_len);
				virt_mic_cfg.buffer_used[virt_mic_cfg.play_w] = 1;
				//pr_err("s w idx = %d\n", virt_mic_cfg.play_w);
				virt_mic_cfg.play_w++;
			} else {
				pr_err("%s: virt mic buffer[%d] is null\r\n",
									__func__, virt_mic_cfg.play_w);
			}
		}
		virt_mic_cfg.play_open = 1;
		axi_chan_enable(chan);
	}
}

static void axi_dma_virt_mic_rx_start(struct axi_dma_chan *chan)
{
	if (chan->virt_mic_type == VIRTUAL_MIC_TYPE_NORMAL)
		return;

	if (chan->cyclic && chan->direction == DMA_DEV_TO_MEM) {
		virt_mic_cfg.cap_open = 1;
	}
}

static void axi_dma_virt_mic_update(struct axi_dma_chan *chan,
					struct axi_dma_desc *desc)
{
	int play_buf_idx, cap_buf_idx;
	int play_real_idx;

	if (chan->virt_mic_type == VIRTUAL_MIC_TYPE_NORMAL)
		return;

	if (!virt_mic_cfg.cap_open)
		return;

	if (chan->cyclic && chan->direction == DMA_MEM_TO_DEV) {
		play_buf_idx = desc->completed_blocks - 1;
		if (play_buf_idx < 0)
			play_buf_idx = virt_mic_cfg.buf_cnt - 1;

		play_real_idx = play_buf_idx + virt_mic_cfg.play_pre_buf_cnt;
		if (play_real_idx >= virt_mic_cfg.buf_cnt)
			play_real_idx = 0;
		//pr_err("0 w idx = %d\n", virt_mic_cfg.play_w);
		if (virt_mic_cfg.buffer[virt_mic_cfg.play_w]) {
			memcpy(virt_mic_cfg.buffer[virt_mic_cfg.play_w],
					virt_mic_cfg.play_addr[play_real_idx],
					virt_mic_cfg.virt_mic_len);
			virt_mic_cfg.buffer_used[virt_mic_cfg.play_w] = 1;
			//pr_err("w idx = %d\n", virt_mic_cfg.play_w);
			virt_mic_cfg.play_w++;
		} else {
			pr_err("%s: virt mic buffer[%d] is null\r\n",
								__func__, virt_mic_cfg.play_w);
		}

		if (virt_mic_cfg.play_w >= virt_mic_cfg.buf_cnt)
			virt_mic_cfg.play_w = 0;
	} else if (chan->cyclic && chan->direction == DMA_DEV_TO_MEM) {
		cap_buf_idx = desc->completed_blocks - 1;
		if (cap_buf_idx < 0)
			cap_buf_idx = virt_mic_cfg.buf_cnt - 1;

		if (virt_mic_cfg.play_r >= virt_mic_cfg.buf_cnt)
			virt_mic_cfg.play_r = 0;

		if (virt_mic_cfg.buffer_used[virt_mic_cfg.play_r] &&
			virt_mic_cfg.play_open && virt_mic_cfg.buffer[virt_mic_cfg.play_r]) {

			memcpy(virt_mic_cfg.cap_addr[cap_buf_idx] + virt_mic_cfg.virt_mic_offset,
				virt_mic_cfg.buffer[virt_mic_cfg.play_r],
				virt_mic_cfg.virt_mic_len);
			virt_mic_cfg.buffer_used[virt_mic_cfg.play_r] = 0;
			virt_mic_cfg.play_r++;
		} else {
			memset(virt_mic_cfg.cap_addr[cap_buf_idx] + virt_mic_cfg.virt_mic_offset,
					0x0, virt_mic_cfg.virt_mic_len);
		}

		if (virt_mic_cfg.play_open == 0 && virt_mic_cfg.play_start_flag == 1)
			axi_dma_virt_mic_tx_start(virt_mic_cfg.tx_chan);

		if (!virt_mic_cfg.play_open) {
			memset(&virt_mic_cfg.buffer_used, 0, sizeof(virt_mic_cfg.buffer_used));
			virt_mic_cfg.play_r = 0;
		}
	}
}

static void axi_dma_virt_mic_close(struct axi_dma_chan *chan)
{
	if (chan->virt_mic_type == VIRTUAL_MIC_TYPE_NORMAL)
		return;

	if (chan->cyclic && chan->direction == DMA_MEM_TO_DEV) {
		virt_mic_cfg.play_open = 0;
		virt_mic_cfg.play_start_flag = 0;
	} else if (chan->cyclic && chan->direction == DMA_DEV_TO_MEM) {
		memset(&virt_mic_cfg.cap_addr, 0, sizeof(virt_mic_cfg.cap_addr));
		virt_mic_cfg.cap_open = 0;
	}
}

static inline void
axi_dma_iowrite32(struct axi_dma_chip *chip, u32 reg, u32 val)
{
	iowrite32(val, chip->regs + reg);
}

static inline u32 axi_dma_ioread32(struct axi_dma_chip *chip, u32 reg)
{
	return ioread32(chip->regs + reg);
}

static inline void
axi_chan_iowrite32(struct axi_dma_chan *chan, u32 reg, u32 val)
{
	iowrite32(val, chan->chan_regs + reg);
}

static inline u32 axi_chan_ioread32(struct axi_dma_chan *chan, u32 reg)
{
	return ioread32(chan->chan_regs + reg);
}

static inline void
axi_chan_iowrite64(struct axi_dma_chan *chan, u32 reg, u64 val)
{
	/*
	 * We split one 64 bit write for two 32 bit write as some HW doesn't
	 * support 64 bit access.
	 */
	iowrite32(lower_32_bits(val), chan->chan_regs + reg);
	iowrite32(upper_32_bits(val), chan->chan_regs + reg + 4);
}

static inline void axi_chan_config_write(struct axi_dma_chan *chan,
					 struct axi_dma_chan_config *config)
{
	u32 cfg_lo, cfg_hi;

	cfg_lo = (config->dst_multblk_type << CH_CFG_L_DST_MULTBLK_TYPE_POS |
		  config->src_multblk_type << CH_CFG_L_SRC_MULTBLK_TYPE_POS);
	if (chan->chip->dw->hdata->reg_map_8_channels) {
		cfg_hi = config->tt_fc << CH_CFG_H_TT_FC_POS |
			 config->hs_sel_src << CH_CFG_H_HS_SEL_SRC_POS |
			 config->hs_sel_dst << CH_CFG_H_HS_SEL_DST_POS |
			 config->src_per << CH_CFG_H_SRC_PER_POS |
			 config->dst_per << CH_CFG_H_DST_PER_POS |
			 config->prior << CH_CFG_H_PRIORITY_POS;
	} else {
		cfg_lo |= config->src_per << CH_CFG2_L_SRC_PER_POS |
			  config->dst_per << CH_CFG2_L_DST_PER_POS;
		cfg_hi = config->tt_fc << CH_CFG2_H_TT_FC_POS |
			 config->hs_sel_src << CH_CFG2_H_HS_SEL_SRC_POS |
			 config->hs_sel_dst << CH_CFG2_H_HS_SEL_DST_POS |
			 config->prior << CH_CFG2_H_PRIORITY_POS;
	}
	axi_chan_iowrite32(chan, CH_CFG_L, cfg_lo);
	axi_chan_iowrite32(chan, CH_CFG_H, cfg_hi);
}

static inline void axi_dma_disable(struct axi_dma_chip *chip)
{
	u32 val;

	val = axi_dma_ioread32(chip, DMAC_CFG);
	val &= ~DMAC_EN_MASK;
	axi_dma_iowrite32(chip, DMAC_CFG, val);
}

static inline void axi_dma_enable(struct axi_dma_chip *chip)
{
	u32 val;

	val = axi_dma_ioread32(chip, DMAC_CFG);
	val |= DMAC_EN_MASK;
	axi_dma_iowrite32(chip, DMAC_CFG, val);
}

static inline void axi_dma_irq_disable(struct axi_dma_chip *chip)
{
	u32 val;

	val = axi_dma_ioread32(chip, DMAC_CFG);
	val &= ~INT_EN_MASK;
	axi_dma_iowrite32(chip, DMAC_CFG, val);
}

static inline void axi_dma_irq_enable(struct axi_dma_chip *chip)
{
	u32 val;

	val = axi_dma_ioread32(chip, DMAC_CFG);
	val |= INT_EN_MASK;
	axi_dma_iowrite32(chip, DMAC_CFG, val);
}

static inline void axi_chan_irq_disable(struct axi_dma_chan *chan, u32 irq_mask)
{
	u32 val;

	if (likely(irq_mask == DWAXIDMAC_IRQ_ALL)) {
		axi_chan_iowrite32(chan, CH_INTSTATUS_ENA, DWAXIDMAC_IRQ_NONE);
	} else {
		val = axi_chan_ioread32(chan, CH_INTSTATUS_ENA);
		val &= ~irq_mask;
		axi_chan_iowrite32(chan, CH_INTSTATUS_ENA, val);
	}
}

static inline void axi_chan_irq_set(struct axi_dma_chan *chan, u32 irq_mask)
{
	axi_chan_iowrite32(chan, CH_INTSTATUS_ENA, irq_mask);
}

static inline void axi_chan_irq_sig_set(struct axi_dma_chan *chan, u32 irq_mask)
{
	axi_chan_iowrite32(chan, CH_INTSIGNAL_ENA, irq_mask);
}

static inline void axi_chan_irq_clear(struct axi_dma_chan *chan, u32 irq_mask)
{
	axi_chan_iowrite32(chan, CH_INTCLEAR, irq_mask);
}

static inline u32 axi_chan_irq_read(struct axi_dma_chan *chan)
{
	return axi_chan_ioread32(chan, CH_INTSTATUS);
}

static inline void axi_chan_disable(struct axi_dma_chan *chan)
{
	u32 val;

	if (chan->chip->dw->hdata->reg_map_8_channels) {
		val = axi_dma_ioread32(chan->chip, DMAC_CHEN_L);
		val &= ~(BIT(chan->id) << DMAC_CHAN_L_EN_SHIFT);
		val |= BIT(chan->id) << DMAC_CHAN_L_EN_WE_SHIFT;
		axi_dma_iowrite32(chan->chip, DMAC_CHEN_L, val);
	} else {
		if (chan->id > 16) {
			val = axi_dma_ioread32(chan->chip, DMAC_CHEN2_H);
			val &= ~(BIT(chan->id - 16) << DMAC_CHAN2_H_EN_SHIFT);
			val |= BIT(chan->id - 16) << DMAC_CHAN2_H_EN_WE_SHIFT;
			axi_dma_iowrite32(chan->chip, DMAC_CHEN2_H, val);
		} else {
			val = axi_dma_ioread32(chan->chip, DMAC_CHEN2_L);
			val &= ~(BIT(chan->id) << DMAC_CHAN2_L_EN_SHIFT);
			val |= BIT(chan->id) << DMAC_CHAN2_L_EN_WE_SHIFT;
			axi_dma_iowrite32(chan->chip, DMAC_CHEN2_L, val);
		}
	}
}

static inline void axi_chan_enable(struct axi_dma_chan *chan)
{
	u32 val;

	if (chan->chip->dw->hdata->reg_map_8_channels) {
		val = axi_dma_ioread32(chan->chip, DMAC_CHEN_L);
		val |= BIT(chan->id) << DMAC_CHAN_L_EN_SHIFT |
			BIT(chan->id) << DMAC_CHAN_L_EN_WE_SHIFT;
		axi_dma_iowrite32(chan->chip, DMAC_CHEN_L, val);
	} else {
		if (chan->id > 16) {
			val = axi_dma_ioread32(chan->chip, DMAC_CHEN2_H);
			val |= BIT(chan->id - 16) << DMAC_CHAN2_H_EN_SHIFT |
				BIT(chan->id - 16) << DMAC_CHAN2_H_EN_WE_SHIFT;
			axi_dma_iowrite32(chan->chip, DMAC_CHEN2_H, val);
		} else {
			val = axi_dma_ioread32(chan->chip, DMAC_CHEN2_L);
			val |= BIT(chan->id) << DMAC_CHAN2_L_EN_SHIFT |
				BIT(chan->id) << DMAC_CHAN2_L_EN_WE_SHIFT;
			axi_dma_iowrite32(chan->chip, DMAC_CHEN2_L, val);
		}
	}
}

static inline bool axi_chan_is_hw_enable(struct axi_dma_chan *chan)
{
	u32 val;

	if (chan->chip->dw->hdata->reg_map_8_channels) {
		val = axi_dma_ioread32(chan->chip, DMAC_CHEN_L);
		return !!(val & (BIT(chan->id) << DMAC_CHAN_L_EN_SHIFT));
	} else {
		if(chan->id > 16) {
			val = axi_dma_ioread32(chan->chip, DMAC_CHEN2_H);
			return !!(val & (BIT(chan->id - 16) << DMAC_CHAN2_H_EN_SHIFT));
		} else {
			val = axi_dma_ioread32(chan->chip, DMAC_CHEN2_L);
			return !!(val & (BIT(chan->id) << DMAC_CHAN2_L_EN_SHIFT));
		}
	}
}

static void axi_dma_hw_init(struct axi_dma_chip *chip)
{
	int ret;
	u32 i;

	for (i = 0; i < chip->dw->hdata->nr_channels; i++) {
		axi_chan_irq_disable(&chip->dw->chan[i], DWAXIDMAC_IRQ_ALL);
		axi_chan_disable(&chip->dw->chan[i]);
	}
	ret = dma_set_mask_and_coherent(chip->dev, DMA_BIT_MASK(64));
	if (ret)
		dev_warn(chip->dev, "Unable to set coherent mask\n");
}

static u32 axi_chan_get_xfer_width(struct axi_dma_chan *chan, dma_addr_t src,
				   dma_addr_t dst, size_t len)
{
	u32 max_width = chan->chip->dw->hdata->m_data_width;

	return __ffs(src | dst | len | BIT(max_width));
}

static inline const char *axi_chan_name(struct axi_dma_chan *chan)
{
	return dma_chan_name(&chan->vc.chan);
}

static struct axi_dma_desc *axi_desc_alloc(u32 num)
{
	struct axi_dma_desc *desc;

	desc = kzalloc(sizeof(*desc), GFP_NOWAIT);
	if (!desc)
		return NULL;

	desc->hw_desc = kcalloc(num, sizeof(*desc->hw_desc), GFP_NOWAIT);
	if (!desc->hw_desc) {
		kfree(desc);
		return NULL;
	}

	return desc;
}

static struct axi_dma_lli *axi_desc_get(struct axi_dma_chan *chan,
					dma_addr_t *addr)
{
	struct axi_dma_lli *lli;
	dma_addr_t phys;

	lli = dma_pool_zalloc(chan->desc_pool, GFP_NOWAIT, &phys);
	if (unlikely(!lli)) {
		dev_err(chan2dev(chan), "%s: not enough descriptors available\n",
			axi_chan_name(chan));
		return NULL;
	}

	atomic_inc(&chan->descs_allocated);
	*addr = phys;

	return lli;
}

static void axi_desc_put(struct axi_dma_desc *desc)
{
	struct axi_dma_chan *chan = desc->chan;
	int count = atomic_read(&chan->descs_allocated);
	struct axi_dma_hw_desc *hw_desc;
	int descs_put;

	for (descs_put = 0; descs_put < count; descs_put++) {
		hw_desc = &desc->hw_desc[descs_put];
		dma_pool_free(chan->desc_pool, hw_desc->lli, hw_desc->llp);
	}

	kfree(desc->hw_desc);
	kfree(desc);
	atomic_sub(descs_put, &chan->descs_allocated);
	dev_vdbg(chan2dev(chan), "%s: %d descs put, %d still allocated\n",
		axi_chan_name(chan), descs_put,
		atomic_read(&chan->descs_allocated));
}

static void vchan_desc_put(struct virt_dma_desc *vdesc)
{
	axi_desc_put(vd_to_axi_desc(vdesc));
}

static enum dma_status
dma_chan_tx_status(struct dma_chan *dchan, dma_cookie_t cookie,
		  struct dma_tx_state *txstate)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);
	struct virt_dma_desc *vdesc;
	enum dma_status status;
	u32 completed_length;
	unsigned long flags;
	u32 completed_blocks;
	size_t bytes = 0;
	u32 length;
	u32 len;

	status = dma_cookie_status(dchan, cookie, txstate);
	if (status == DMA_COMPLETE || !txstate)
		return status;

	spin_lock_irqsave(&chan->vc.lock, flags);

	vdesc = vchan_find_desc(&chan->vc, cookie);
	if (vdesc) {
		length = vd_to_axi_desc(vdesc)->length;
		completed_blocks = vd_to_axi_desc(vdesc)->completed_blocks;
		len = vd_to_axi_desc(vdesc)->hw_desc[0].len;
		completed_length = completed_blocks * len;
		bytes = length - completed_length;
	} else {
		bytes = vd_to_axi_desc(vdesc)->length;
	}

	spin_unlock_irqrestore(&chan->vc.lock, flags);
	dma_set_residue(txstate, bytes);

	return status;
}

static void write_desc_llp(struct axi_dma_hw_desc *desc, dma_addr_t adr)
{
	desc->lli->llp = cpu_to_le64(adr);
}

static void write_chan_llp(struct axi_dma_chan *chan, dma_addr_t adr)
{
	axi_chan_iowrite64(chan, CH_LLP, adr);
}

static void dw_axi_dma_set_byte_halfword(struct axi_dma_chan *chan, bool set)
{
	u32 offset = DMAC_APB_BYTE_WR_CH_EN;
	u32 reg_width, val;

	if (!chan->chip->apb_regs) {
		dev_dbg(chan->chip->dev, "apb_regs not initialized\n");
		return;
	}

	reg_width = __ffs(chan->config.dst_addr_width);
	if (reg_width == DWAXIDMAC_TRANS_WIDTH_16)
		offset = DMAC_APB_HALFWORD_WR_CH_EN;

	val = ioread32(chan->chip->apb_regs + offset);

	if (set)
		val |= BIT(chan->id);
	else
		val &= ~BIT(chan->id);

	iowrite32(val, chan->chip->apb_regs + offset);
}
/* Called in chan locked context */
static void axi_chan_block_xfer_start(struct axi_dma_chan *chan,
				      struct axi_dma_desc *first)
{
	u32 priority = chan->chip->dw->hdata->priority[chan->id];
	struct axi_dma_chan_config config = {};
	u32 irq_mask;
	u8 lms = 0; /* Select AXI0 master for LLI fetching */

	if (unlikely(axi_chan_is_hw_enable(chan))) {
		dev_err(chan2dev(chan), "%s is non-idle!\n",
			axi_chan_name(chan));

		return;
	}

	axi_dma_enable(chan->chip);

	config.dst_multblk_type = DWAXIDMAC_MBLK_TYPE_LL;
	config.src_multblk_type = DWAXIDMAC_MBLK_TYPE_LL;
	config.tt_fc = DWAXIDMAC_TT_FC_MEM_TO_MEM_DMAC;
	config.prior = priority;
	config.hs_sel_dst = DWAXIDMAC_HS_SEL_HW;
	config.hs_sel_src = DWAXIDMAC_HS_SEL_HW;
	switch (chan->direction) {
	case DMA_MEM_TO_DEV:
		dw_axi_dma_set_byte_halfword(chan, true);
		config.tt_fc = chan->config.device_fc ?
				DWAXIDMAC_TT_FC_MEM_TO_PER_DST :
				DWAXIDMAC_TT_FC_MEM_TO_PER_DMAC;
		if (chan->chip->apb_regs)
			config.dst_per = chan->id;
		else
			config.dst_per = chan->hw_handshake_num;

		if (chan->cyclic)
			virt_mic_cfg.tx_chan = chan;

		break;
	case DMA_DEV_TO_MEM:
		config.tt_fc = chan->config.device_fc ?
				DWAXIDMAC_TT_FC_PER_TO_MEM_SRC :
				DWAXIDMAC_TT_FC_PER_TO_MEM_DMAC;
		if (chan->chip->apb_regs)
			config.src_per = chan->id;
		else
			config.src_per = chan->hw_handshake_num;
		break;
	default:
		break;
	}
	axi_chan_config_write(chan, &config);

	write_chan_llp(chan, first->hw_desc[0].llp | lms);

	irq_mask = DWAXIDMAC_IRQ_DMA_TRF | DWAXIDMAC_IRQ_ALL_ERR;
	axi_chan_irq_sig_set(chan, irq_mask);

	/* Generate 'suspend' status but don't generate interrupt */
	irq_mask |= DWAXIDMAC_IRQ_SUSPENDED;
	axi_chan_irq_set(chan, irq_mask);

	axi_dma_virt_mic_rx_start(chan);

	/* cyclic rx */
	if (chan->cyclic && chan->direction == DMA_DEV_TO_MEM) {
		axi_chan_enable(chan);
	}

	if (virt_mic_cfg.cap_open) { /* cyclic tx, but cyclic rx must is working */
		if (chan->cyclic && chan->direction == DMA_MEM_TO_DEV)
			virt_mic_cfg.play_start_flag = 1;
	} else { /* cyclic tx, but cyclic rx not work */
		if (chan->cyclic && chan->direction == DMA_MEM_TO_DEV)
			axi_chan_enable(chan);
	}

	/* not cyclic, tx/rx */
	if (!chan->cyclic)
		axi_chan_enable(chan);

}

static void axi_chan_start_first_queued(struct axi_dma_chan *chan)
{
	struct axi_dma_desc *desc;
	struct virt_dma_desc *vd;

	vd = vchan_next_desc(&chan->vc);
	if (!vd)
		return;

	desc = vd_to_axi_desc(vd);
	dev_vdbg(chan2dev(chan), "%s: started %u\n", axi_chan_name(chan),
		vd->tx.cookie);
	axi_chan_block_xfer_start(chan, desc);
}

static void dma_chan_issue_pending(struct dma_chan *dchan)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);
	unsigned long flags;

	spin_lock_irqsave(&chan->vc.lock, flags);
	if (vchan_issue_pending(&chan->vc))
		axi_chan_start_first_queued(chan);
	spin_unlock_irqrestore(&chan->vc.lock, flags);
}

static void dw_axi_dma_synchronize(struct dma_chan *dchan)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);

	vchan_synchronize(&chan->vc);
}

static int dma_chan_alloc_chan_resources(struct dma_chan *dchan)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);

	/* ASSERT: channel is idle */
	if (axi_chan_is_hw_enable(chan)) {
		dev_err(chan2dev(chan), "%s is non-idle!\n",
			axi_chan_name(chan));
		return -EBUSY;
	}

	/* LLI address must be aligned to a 64-byte boundary */
	chan->desc_pool = dma_pool_create(dev_name(chan2dev(chan)),
					  chan->chip->dev,
					  sizeof(struct axi_dma_lli),
					  64, 0);
	if (!chan->desc_pool) {
		dev_err(chan2dev(chan), "No memory for descriptors\n");
		return -ENOMEM;
	}
	dev_vdbg(dchan2dev(dchan), "%s: allocating\n", axi_chan_name(chan));

	pm_runtime_get(chan->chip->dev);

	return 0;
}

static void dma_chan_free_chan_resources(struct dma_chan *dchan)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);

	/* ASSERT: channel is idle */
	if (axi_chan_is_hw_enable(chan))
		dev_err(dchan2dev(dchan), "%s is non-idle!\n",
			axi_chan_name(chan));

	axi_chan_disable(chan);
	axi_chan_irq_disable(chan, DWAXIDMAC_IRQ_ALL);

	vchan_free_chan_resources(&chan->vc);

	dma_pool_destroy(chan->desc_pool);
	chan->desc_pool = NULL;
	dev_vdbg(dchan2dev(dchan),
		 "%s: free resources, descriptor still allocated: %u\n",
		 axi_chan_name(chan), atomic_read(&chan->descs_allocated));

	pm_runtime_put(chan->chip->dev);
}

static void axi_chan_dump_lli(struct axi_dma_chan *chan,
			      struct axi_dma_hw_desc *desc)
{
	dev_err(dchan2dev(&chan->vc.chan),
		"SAR: 0x%llx DAR: 0x%llx LLP: 0x%llx BTS 0x%x CTL: 0x%x:%08x",
		le64_to_cpu(desc->lli->sar),
		le64_to_cpu(desc->lli->dar),
		le64_to_cpu(desc->lli->llp),
		le32_to_cpu(desc->lli->block_ts_lo),
		le32_to_cpu(desc->lli->ctl_hi),
		le32_to_cpu(desc->lli->ctl_lo));
}

static void axi_chan_list_dump_lli(struct axi_dma_chan *chan,
				   struct axi_dma_desc *desc_head)
{
	int count = atomic_read(&chan->descs_allocated);
	int i;

	for (i = 0; i < count; i++)
		axi_chan_dump_lli(chan, &desc_head->hw_desc[i]);

}

static void dw_axi_dma_set_hw_channel(struct axi_dma_chan *chan, bool set)
{
	struct axi_dma_chip *chip = chan->chip;
	unsigned long reg_value, val;

	if (!chip->apb_regs) {
		dev_dbg(chip->dev, "apb_regs not initialized\n");
		return;
	}

	/*
	 * An unused DMA channel has a default value of 0x3F.
	 * Lock the DMA channel by assign a handshake number to the channel.
	 * Unlock the DMA channel by assign 0x3F to the channel.
	 */
	if (set)
		val = chan->hw_handshake_num;
	else
		val = UNUSED_CHANNEL;

	reg_value = lo_hi_readq(chip->apb_regs + DMAC_APB_HW_HS_SEL_0);

	/* Channel is already allocated, set handshake as per channel ID */
	/* 64 bit write should handle for 8 channels */

	reg_value &= ~(DMA_APB_HS_SEL_MASK <<
			(chan->id * DMA_APB_HS_SEL_BIT_SIZE));
	reg_value |= (val << (chan->id * DMA_APB_HS_SEL_BIT_SIZE));
	lo_hi_writeq(reg_value, chip->apb_regs + DMAC_APB_HW_HS_SEL_0);

	return;
}

/*
 * If DW_axi_dmac sees CHx_CTL.ShadowReg_Or_LLI_Last bit of the fetched LLI
 * as 1, it understands that the current block is the final block in the
 * transfer and completes the DMA transfer operation at the end of current
 * block transfer.
 */
static void set_desc_last(struct axi_dma_hw_desc *desc)
{
	u32 val;

	val = le32_to_cpu(desc->lli->ctl_hi);
	val |= CH_CTL_H_LLI_LAST;
	desc->lli->ctl_hi = cpu_to_le32(val);
}

static void write_desc_sar(struct axi_dma_hw_desc *desc, dma_addr_t adr)
{
	desc->lli->sar = cpu_to_le64(adr);
}

static void write_desc_dar(struct axi_dma_hw_desc *desc, dma_addr_t adr)
{
	desc->lli->dar = cpu_to_le64(adr);
}

static int set_desc_src_master(struct axi_dma_hw_desc *hw_desc,
				 struct axi_dma_chan *chan)
{
	u32 val;

	val = le32_to_cpu(hw_desc->lli->ctl_lo);
	switch (chan->direction) {
	case DMA_MEM_TO_DEV:
		val |= (chan->chip->dw->hdata->mem_master << CH_CTL_L_SRC_MAST_POS);
		break;
	case DMA_DEV_TO_MEM:
		val |= (chan->chip->dw->hdata->dev_master << CH_CTL_L_SRC_MAST_POS);
		break;
	case DMA_MEM_TO_MEM:
		val |= (chan->chip->dw->hdata->mem_master << CH_CTL_L_SRC_MAST_POS);
		break;
	default:
		return -EINVAL;
	}

	hw_desc->lli->ctl_lo = cpu_to_le32(val);
	return 0;
}

static int set_desc_dest_master(struct axi_dma_hw_desc *hw_desc,
				 struct axi_dma_chan *chan)
{
	u32 val;

	val = le32_to_cpu(hw_desc->lli->ctl_lo);
	switch (chan->direction) {
	case DMA_MEM_TO_DEV:
		val |= (chan->chip->dw->hdata->dev_master << CH_CTL_L_DST_MAST_POS);
		break;
	case DMA_DEV_TO_MEM:
		val |= (chan->chip->dw->hdata->mem_master << CH_CTL_L_DST_MAST_POS);
		break;
	case DMA_MEM_TO_MEM:
		val |= (chan->chip->dw->hdata->mem_master << CH_CTL_L_DST_MAST_POS);
		break;
	default:
		return -EINVAL;
	}

	hw_desc->lli->ctl_lo = cpu_to_le32(val);

	return 0;
}

static int dw_axi_dma_set_hw_desc(struct axi_dma_chan *chan,
				  struct axi_dma_hw_desc *hw_desc,
				  dma_addr_t mem_addr, size_t len)
{
	unsigned int data_width = BIT(chan->chip->dw->hdata->m_data_width);
	unsigned int reg_width;
	unsigned int mem_width;
	dma_addr_t device_addr;
	size_t axi_block_ts;
	size_t block_ts;
	u32 ctllo, ctlhi;
	u32 burst_len;

	axi_block_ts = chan->chip->dw->hdata->block_size[chan->id];

	mem_width = __ffs(data_width | mem_addr | len);

	if (mem_width > DWAXIDMAC_TRANS_WIDTH_32)
		mem_width = DWAXIDMAC_TRANS_WIDTH_32;

	if (!IS_ALIGNED(mem_addr, 4)) {
		dev_err(chan->chip->dev, "invalid buffer alignment\n");
		return -EINVAL;
	}

	switch (chan->direction) {
	case DMA_MEM_TO_DEV:
		reg_width = __ffs(chan->config.dst_addr_width);
		device_addr = chan->config.dst_addr;
		ctllo = reg_width << CH_CTL_L_DST_WIDTH_POS |
			mem_width << CH_CTL_L_SRC_WIDTH_POS |
			DWAXIDMAC_CH_CTL_L_NOINC << CH_CTL_L_DST_INC_POS |
			DWAXIDMAC_CH_CTL_L_INC << CH_CTL_L_SRC_INC_POS;
		block_ts = len >> mem_width;
		break;
	case DMA_DEV_TO_MEM:
		reg_width = __ffs(chan->config.src_addr_width);
		device_addr = chan->config.src_addr;
		ctllo = reg_width << CH_CTL_L_SRC_WIDTH_POS |
			mem_width << CH_CTL_L_DST_WIDTH_POS |
			DWAXIDMAC_CH_CTL_L_INC << CH_CTL_L_DST_INC_POS |
			DWAXIDMAC_CH_CTL_L_NOINC << CH_CTL_L_SRC_INC_POS;
		block_ts = len >> reg_width;
		axi_dma_virt_mic_set_block_ts(chan, &block_ts);
		break;
	default:
		return -EINVAL;
	}

	if (block_ts > axi_block_ts)
		return -EINVAL;

	hw_desc->lli = axi_desc_get(chan, &hw_desc->llp);
	if (unlikely(!hw_desc->lli))
		return -ENOMEM;

	ctlhi = CH_CTL_H_LLI_VALID;

	if (chan->chip->dw->hdata->restrict_axi_burst_len) {
		burst_len = chan->chip->dw->hdata->axi_rw_burst_len;
		ctlhi |= CH_CTL_H_ARLEN_EN | CH_CTL_H_AWLEN_EN |
			 burst_len << CH_CTL_H_ARLEN_POS |
			 burst_len << CH_CTL_H_AWLEN_POS;
	}

	hw_desc->lli->ctl_hi = cpu_to_le32(ctlhi);

	if (chan->direction == DMA_MEM_TO_DEV) {
		write_desc_sar(hw_desc, mem_addr);
		write_desc_dar(hw_desc, device_addr);
	} else {
		write_desc_sar(hw_desc, device_addr);
		write_desc_dar(hw_desc, mem_addr);
	}

	hw_desc->lli->block_ts_lo = cpu_to_le32(block_ts - 1);

	ctllo |= DWAXIDMAC_BURST_TRANS_LEN_4 << CH_CTL_L_DST_MSIZE_POS |
		 DWAXIDMAC_BURST_TRANS_LEN_4 << CH_CTL_L_SRC_MSIZE_POS;
	hw_desc->lli->ctl_lo = cpu_to_le32(ctllo);

	set_desc_src_master(hw_desc, chan);
	set_desc_dest_master(hw_desc, chan);

	hw_desc->len = len;
	return 0;
}

static size_t calculate_block_len(struct axi_dma_chan *chan,
				  dma_addr_t dma_addr, size_t buf_len,
				  enum dma_transfer_direction direction)
{
	u32 data_width, reg_width, mem_width;
	size_t axi_block_ts, block_len;

	axi_block_ts = chan->chip->dw->hdata->block_size[chan->id];

	switch (direction) {
	case DMA_MEM_TO_DEV:
		data_width = BIT(chan->chip->dw->hdata->m_data_width);
		mem_width = __ffs(data_width | dma_addr | buf_len);
		if (mem_width > DWAXIDMAC_TRANS_WIDTH_32)
			mem_width = DWAXIDMAC_TRANS_WIDTH_32;

		block_len = axi_block_ts << mem_width;
		break;
	case DMA_DEV_TO_MEM:
		reg_width = __ffs(chan->config.src_addr_width);
		block_len = axi_block_ts << reg_width;
		break;
	default:
		block_len = 0;
	}

	return block_len;
}


static struct dma_async_tx_descriptor *
dw_axi_dma_chan_prep_cyclic(struct dma_chan *dchan,
				struct snd_pcm_substream *subtream,
			    size_t buf_len, size_t period_len,
			    enum dma_transfer_direction direction,
			    unsigned long flags)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);
	struct axi_dma_hw_desc *hw_desc = NULL;
	struct axi_dma_desc *desc = NULL;
	dma_addr_t dma_addr = subtream->runtime->dma_addr;
	dma_addr_t src_addr = dma_addr;
	u32 num_periods, num_segments;
	size_t axi_block_len;
	u32 total_segments;
	u32 segment_len;
	size_t real_period_len;
	unsigned int i;
	int status;
	u64 llp = 0;
	u8 lms = 0; /* Select AXI0 master for LLI fetching */
	int ret;

	chan->cyclic = true;
	chan->direction = direction;

	num_periods = buf_len / period_len;

	axi_block_len = calculate_block_len(chan, dma_addr, buf_len, direction);
	if (axi_block_len == 0)
		return NULL;

	real_period_len = axi_dma_virt_mic_get_real_period_len(chan, period_len);

	num_segments = DIV_ROUND_UP(real_period_len, axi_block_len);
	segment_len = DIV_ROUND_UP(period_len, num_segments);
	total_segments = num_periods * num_segments;

	desc = axi_desc_alloc(total_segments);
	if (unlikely(!desc))
		goto err_desc_get;

	desc->chan = chan;
	desc->length = 0;
	desc->period_len = period_len;

	//pr_err("chan dir = %d, dma_addr = 0x%llx, buf_len = %zu, period_len = %zu\r\n",
	//					chan->direction, dma_addr, buf_len, period_len);

	ret = axi_dma_virt_mic_init(chan, buf_len, period_len);
	if (ret) {
		pr_err("dw_ahb_dma_virt_mic_init failed with %d\r\n", ret);
		return NULL;
	}

	for (i = 0; i < total_segments; i++) {
		hw_desc = &desc->hw_desc[i];

		status = dw_axi_dma_set_hw_desc(chan, hw_desc, src_addr,
						segment_len);
		if (status < 0)
			goto err_desc_get;

		axi_dma_virt_mic_set_cfg(chan, subtream, i, segment_len);

		desc->length += hw_desc->len;
		/* Set end-of-link to the linked descriptor, so that cyclic
		 * callback function can be triggered during interrupt.
		 */
		set_desc_last(hw_desc);
		src_addr += segment_len;
	}

	llp = desc->hw_desc[0].llp;

	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--total_segments];
		write_desc_llp(hw_desc, llp | lms);
		llp = hw_desc->llp;
	} while (total_segments);

	dw_axi_dma_set_hw_channel(chan, true);

	//axi_chan_list_dump_lli(chan, desc);

	return vchan_tx_prep(&chan->vc, &desc->vd, flags);

err_desc_get:
	if (desc)
		axi_desc_put(desc);

	return NULL;
}

static struct dma_async_tx_descriptor *
dw_axi_dma_chan_prep_slave_sg(struct dma_chan *dchan, struct scatterlist *sgl,
			      unsigned int sg_len,
			      enum dma_transfer_direction direction,
			      unsigned long flags, void *context)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);
	struct axi_dma_hw_desc *hw_desc = NULL;
	struct axi_dma_desc *desc = NULL;
	u32 num_segments, segment_len;
	unsigned int loop = 0;
	struct scatterlist *sg;
	size_t axi_block_len;
	u32 len, num_sgs = 0;
	unsigned int i;
	dma_addr_t mem;
	int status;
	u64 llp = 0;
	u8 lms = 0; /* Select AXI0 master for LLI fetching */

	if (unlikely(!is_slave_direction(direction) || !sg_len))
		return NULL;

	mem = sg_dma_address(sgl);
	len = sg_dma_len(sgl);

	axi_block_len = calculate_block_len(chan, mem, len, direction);
	if (axi_block_len == 0)
		return NULL;

	for_each_sg(sgl, sg, sg_len, i)
		num_sgs += DIV_ROUND_UP(sg_dma_len(sg), axi_block_len);

	desc = axi_desc_alloc(num_sgs);
	if (unlikely(!desc))
		goto err_desc_get;

	desc->chan = chan;
	desc->length = 0;
	chan->direction = direction;

	for_each_sg(sgl, sg, sg_len, i) {
		mem = sg_dma_address(sg);
		len = sg_dma_len(sg);
		num_segments = DIV_ROUND_UP(sg_dma_len(sg), axi_block_len);
		segment_len = DIV_ROUND_UP(sg_dma_len(sg), num_segments);

		do {
			hw_desc = &desc->hw_desc[loop++];
			status = dw_axi_dma_set_hw_desc(chan, hw_desc, mem, segment_len);
			if (status < 0)
				goto err_desc_get;

			desc->length += hw_desc->len;
			len -= segment_len;
			mem += segment_len;
		} while (len >= segment_len);
	}

	/* Set end-of-link to the last link descriptor of list */
	set_desc_last(&desc->hw_desc[num_sgs - 1]);

	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--num_sgs];
		write_desc_llp(hw_desc, llp | lms);
		llp = hw_desc->llp;
	} while (num_sgs);

	dw_axi_dma_set_hw_channel(chan, true);

	return vchan_tx_prep(&chan->vc, &desc->vd, flags);

err_desc_get:
	if (desc)
		axi_desc_put(desc);

	return NULL;
}

static struct dma_async_tx_descriptor *
dma_chan_prep_dma_memcpy(struct dma_chan *dchan, dma_addr_t dst_adr,
			 dma_addr_t src_adr, size_t len, unsigned long flags)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);
	size_t block_ts, max_block_ts, xfer_len;
	struct axi_dma_hw_desc *hw_desc = NULL;
	struct axi_dma_desc *desc = NULL;
	u32 xfer_width, reg, num;
	u64 llp = 0;
	u8 lms = 0; /* Select AXI0 master for LLI fetching */

	dev_dbg(chan2dev(chan), "%s: memcpy: src: %pad dst: %pad length: %zd flags: %#lx",
		axi_chan_name(chan), &src_adr, &dst_adr, len, flags);

	max_block_ts = chan->chip->dw->hdata->block_size[chan->id];
	xfer_width = axi_chan_get_xfer_width(chan, src_adr, dst_adr, len);
	num = DIV_ROUND_UP(len, max_block_ts << xfer_width);
	desc = axi_desc_alloc(num);
	if (unlikely(!desc))
		goto err_desc_get;

	desc->chan = chan;
	num = 0;
	desc->length = 0;
	while (len) {
		xfer_len = len;

		hw_desc = &desc->hw_desc[num];
		/*
		 * Take care for the alignment.
		 * Actually source and destination widths can be different, but
		 * make them same to be simpler.
		 */
		xfer_width = axi_chan_get_xfer_width(chan, src_adr, dst_adr, xfer_len);

		/*
		 * block_ts indicates the total number of data of width
		 * to be transferred in a DMA block transfer.
		 * BLOCK_TS register should be set to block_ts - 1
		 */
		block_ts = xfer_len >> xfer_width;
		if (block_ts > max_block_ts) {
			block_ts = max_block_ts;
			xfer_len = max_block_ts << xfer_width;
		}

		hw_desc->lli = axi_desc_get(chan, &hw_desc->llp);
		if (unlikely(!hw_desc->lli))
			goto err_desc_get;

		write_desc_sar(hw_desc, src_adr);
		write_desc_dar(hw_desc, dst_adr);
		hw_desc->lli->block_ts_lo = cpu_to_le32(block_ts - 1);

		reg = CH_CTL_H_LLI_VALID;
		if (chan->chip->dw->hdata->restrict_axi_burst_len) {
			u32 burst_len = chan->chip->dw->hdata->axi_rw_burst_len;

			reg |= (CH_CTL_H_ARLEN_EN |
				burst_len << CH_CTL_H_ARLEN_POS |
				CH_CTL_H_AWLEN_EN |
				burst_len << CH_CTL_H_AWLEN_POS);
		}
		hw_desc->lli->ctl_hi = cpu_to_le32(reg);

		reg = (DWAXIDMAC_BURST_TRANS_LEN_4 << CH_CTL_L_DST_MSIZE_POS |
		       DWAXIDMAC_BURST_TRANS_LEN_4 << CH_CTL_L_SRC_MSIZE_POS |
		       xfer_width << CH_CTL_L_DST_WIDTH_POS |
		       xfer_width << CH_CTL_L_SRC_WIDTH_POS |
		       DWAXIDMAC_CH_CTL_L_INC << CH_CTL_L_DST_INC_POS |
		       DWAXIDMAC_CH_CTL_L_INC << CH_CTL_L_SRC_INC_POS);
		hw_desc->lli->ctl_lo = cpu_to_le32(reg);

		set_desc_src_master(hw_desc, chan);
		set_desc_dest_master(hw_desc, chan);

		hw_desc->len = xfer_len;
		desc->length += hw_desc->len;
		/* update the length and addresses for the next loop cycle */
		len -= xfer_len;
		dst_adr += xfer_len;
		src_adr += xfer_len;
		num++;
	}

	/* Set end-of-link to the last link descriptor of list */
	set_desc_last(&desc->hw_desc[num - 1]);
	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--num];
		write_desc_llp(hw_desc, llp | lms);
		llp = hw_desc->llp;
	} while (num);

	if (g_dbg_flag)
		axi_chan_list_dump_lli(chan, desc);

	return vchan_tx_prep(&chan->vc, &desc->vd, flags);

err_desc_get:
	if (desc)
		axi_desc_put(desc);
	return NULL;
}

static struct dma_async_tx_descriptor *
dw_axi_dma_chan_prep_interval_memcpy(struct dma_chan *dchan,
					struct dma_interval *itrl, unsigned long flags)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);
	size_t block_ts, max_block_ts, xfer_len, total_len;
	struct axi_dma_hw_desc *hw_desc = NULL;
	struct axi_dma_desc *desc = NULL;
	u32 xfer_width, reg, num;
	u64 llp = 0;
	u8 lms = 0; /* Select AXI0 master for LLI fetching */

	if (itrl->src_seg_size != itrl->dst_seg_size) {
		dev_err(chan2dev(chan), "%s param is error", __func__);
		return NULL;
	}

	max_block_ts = chan->chip->dw->hdata->block_size[chan->id];
	total_len = itrl->elem_size * itrl->seg_num * itrl->line_num;
	num = itrl->line_num;
	desc = axi_desc_alloc(num);
	if (unlikely(!desc)) {
		pr_err("%s %d ***\n", __func__, __LINE__);
		goto err_desc_get;
	}

	pr_err("seg_num=%lu, line_num=%lu, elem_size=%lu, src_seg_size=%lu, dst_seg_size=%lu\n",
			itrl->seg_num, itrl->line_num, itrl->elem_size, itrl->src_seg_size, itrl->dst_seg_size);

	desc->chan = chan;
	num = 0;
	desc->length = 0;
	while(total_len) {

		xfer_len = itrl->elem_size * itrl->seg_num;
		hw_desc = &desc->hw_desc[num];
		/*
		 * Take care for the alignment.
		 * Actually source and destination widths can be different, but
		 * make them same to be simpler.
		 */
		xfer_width = axi_chan_get_xfer_width(chan, itrl->src_addr, itrl->dst_addr, xfer_len);
		/*
		 * block_ts indicates the total number of data of width
		 * to be transferred in a DMA block transfer.
		 * BLOCK_TS register should be set to block_ts - 1
		 */
		block_ts = xfer_len >> xfer_width;
		if (block_ts > max_block_ts) {
			block_ts = max_block_ts;
			xfer_len = max_block_ts << xfer_width;
		}

		if (xfer_len != itrl->elem_size * itrl->seg_num)
			pr_err("%s %d param error!\n", __func__, __LINE__);

		hw_desc->lli = axi_desc_get(chan, &hw_desc->llp);
		if (unlikely(!hw_desc->lli)) {
			pr_err("%s %d ***\n", __func__, __LINE__);
			goto err_desc_get;
		}

		write_desc_sar(hw_desc, itrl->src_addr);
		write_desc_dar(hw_desc, itrl->dst_addr);
		hw_desc->lli->block_ts_lo = cpu_to_le32(block_ts - 1);

		reg = CH_CTL_H_LLI_VALID;
		if (chan->chip->dw->hdata->restrict_axi_burst_len) {
			u32 burst_len = chan->chip->dw->hdata->axi_rw_burst_len;

			reg |= (CH_CTL_H_ARLEN_EN |
				burst_len << CH_CTL_H_ARLEN_POS |
				CH_CTL_H_AWLEN_EN |
				burst_len << CH_CTL_H_AWLEN_POS);
		}
		hw_desc->lli->ctl_hi = cpu_to_le32(reg);

		reg = (DWAXIDMAC_BURST_TRANS_LEN_4 << CH_CTL_L_DST_MSIZE_POS |
			   DWAXIDMAC_BURST_TRANS_LEN_4 << CH_CTL_L_SRC_MSIZE_POS |
			   xfer_width << CH_CTL_L_DST_WIDTH_POS |
			   xfer_width << CH_CTL_L_SRC_WIDTH_POS |
			   DWAXIDMAC_CH_CTL_L_INC << CH_CTL_L_DST_INC_POS |
			   DWAXIDMAC_CH_CTL_L_INC << CH_CTL_L_SRC_INC_POS);
		hw_desc->lli->ctl_lo = cpu_to_le32(reg);

		set_desc_src_master(hw_desc, chan);
		set_desc_dest_master(hw_desc, chan);

		hw_desc->len = xfer_len;
		desc->length += hw_desc->len;
		/* update the length and addresses for the next loop cycle */
		total_len -= xfer_len;
		itrl->src_addr += itrl->src_line_size;
		itrl->dst_addr += itrl->dst_line_size;
		num++;
	}

	/* Set end-of-link to the last link descriptor of list */
	set_desc_last(&desc->hw_desc[num - 1]);
	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--num];
		write_desc_llp(hw_desc, llp | lms);
		llp = hw_desc->llp;
	} while (num);

	if (g_dbg_flag)
		axi_chan_list_dump_lli(chan, desc);

	return vchan_tx_prep(&chan->vc, &desc->vd, flags);

err_desc_get:
	if (desc)
		axi_desc_put(desc);
	return NULL;
}

static int dw_axi_dma_chan_slave_config(struct dma_chan *dchan,
					struct dma_slave_config *config)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);

	memcpy(&chan->config, config, sizeof(*config));

	return 0;
}

static noinline void axi_chan_handle_err(struct axi_dma_chan *chan, u32 status)
{
	struct virt_dma_desc *vd;
	unsigned long flags;

	spin_lock_irqsave(&chan->vc.lock, flags);

	axi_chan_disable(chan);

	/* The bad descriptor currently is in the head of vc list */
	vd = vchan_next_desc(&chan->vc);
	/* Remove the completed descriptor from issued list */
	list_del(&vd->node);

	/* WARN about bad descriptor */
	dev_err(chan2dev(chan),
		"Bad descriptor submitted for %s, cookie: %d, irq: 0x%08x\n",
		axi_chan_name(chan), vd->tx.cookie, status);
	axi_chan_list_dump_lli(chan, vd_to_axi_desc(vd));

	vchan_cookie_complete(vd);

	/* Try to restart the controller */
	axi_chan_start_first_queued(chan);

	spin_unlock_irqrestore(&chan->vc.lock, flags);
}

static void axi_chan_block_xfer_complete(struct axi_dma_chan *chan)
{
	int count = atomic_read(&chan->descs_allocated);
	struct axi_dma_hw_desc *hw_desc;
	struct axi_dma_desc *desc;
	struct virt_dma_desc *vd;
	unsigned long flags;
	u64 llp;
	int i;

	spin_lock_irqsave(&chan->vc.lock, flags);
	if (unlikely(axi_chan_is_hw_enable(chan))) {
		dev_err(chan2dev(chan), "BUG: %s caught DWAXIDMAC_IRQ_DMA_TRF, but channel not idle!\n",
			axi_chan_name(chan));
		axi_chan_disable(chan);
	}

	/* The completed descriptor currently is in the head of vc list */
	vd = vchan_next_desc(&chan->vc);

	if (chan->cyclic) {
		desc = vd_to_axi_desc(vd);
		if (desc) {
			llp = lo_hi_readq(chan->chan_regs + CH_LLP);
			for (i = 0; i < count; i++) {
				hw_desc = &desc->hw_desc[i];
				if (hw_desc->llp == llp) {
					axi_chan_irq_clear(chan, hw_desc->lli->status_lo);
					hw_desc->lli->ctl_hi |= CH_CTL_H_LLI_VALID;
					desc->completed_blocks = i;

					axi_dma_virt_mic_update(chan, desc);
					vchan_cyclic_callback(vd);

					break;
				}
			}

			axi_chan_enable(chan);
		}
	} else {
		/* Remove the completed descriptor from issued list before completing */
		list_del(&vd->node);
		vchan_cookie_complete(vd);

		/* Submit queued descriptors after processing the completed ones */
		axi_chan_start_first_queued(chan);
	}

	spin_unlock_irqrestore(&chan->vc.lock, flags);
}

static irqreturn_t dw_axi_dma_interrupt(int irq, void *dev_id)
{
	struct axi_dma_chip *chip = dev_id;
	struct dw_axi_dma *dw = chip->dw;
	struct axi_dma_chan *chan;

	u32 status, i;

	/* Disable DMAC inerrupts. We'll enable them after processing chanels */
	axi_dma_irq_disable(chip);

	/* Poll, clear and process every chanel interrupt status */
	for (i = 0; i < dw->hdata->nr_channels; i++) {
		chan = &dw->chan[i];
		status = axi_chan_irq_read(chan);
		axi_chan_irq_clear(chan, status);

		dev_vdbg(chip->dev, "%s %u IRQ status: 0x%08x\n",
			axi_chan_name(chan), i, status);

		if (status & DWAXIDMAC_IRQ_ALL_ERR)
			axi_chan_handle_err(chan, status);
		else if (status & DWAXIDMAC_IRQ_DMA_TRF)
			axi_chan_block_xfer_complete(chan);
	}

	/* Re-enable interrupts */
	axi_dma_irq_enable(chip);

	return IRQ_HANDLED;
}

static int dma_chan_terminate_all(struct dma_chan *dchan)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);
	u32 chan_active, offset;
	unsigned long flags;
	u32 val;
	int ret;
	LIST_HEAD(head);

	if (chan->chip->dw->hdata->reg_map_8_channels) {
		chan_active = BIT(chan->id) << DMAC_CHAN_L_EN_SHIFT;
		offset = DMAC_CHEN_L;
	} else {
		if (chan->id > 16) {
			chan_active = BIT(chan->id - 16) << DMAC_CHAN2_H_EN_SHIFT;
			offset = DMAC_CHEN2_H;
		} else {
			chan_active = BIT(chan->id) << DMAC_CHAN2_L_EN_SHIFT;
			offset = DMAC_CHEN2_L;
		}
	}

	axi_chan_disable(chan);

	ret = readl_poll_timeout_atomic(chan->chip->regs + offset, val,
					!(val & chan_active), 1000, 10000);
	if (ret == -ETIMEDOUT)
		dev_warn(dchan2dev(dchan),
			 "%s failed to stop\n", axi_chan_name(chan));

	if (chan->direction != DMA_MEM_TO_MEM)
		dw_axi_dma_set_hw_channel(chan, false);
	if (chan->direction == DMA_MEM_TO_DEV)
		dw_axi_dma_set_byte_halfword(chan, false);

	spin_lock_irqsave(&chan->vc.lock, flags);

	vchan_get_all_descriptors(&chan->vc, &head);

	axi_dma_virt_mic_close(chan);

	chan->cyclic = false;
	spin_unlock_irqrestore(&chan->vc.lock, flags);

	vchan_dma_desc_free_list(&chan->vc, &head);

	dev_vdbg(dchan2dev(dchan), "terminated: %s\n", axi_chan_name(chan));

	return 0;
}

static int dma_chan_pause(struct dma_chan *dchan)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);
	unsigned long flags;
	unsigned int timeout = 20; /* timeout iterations */
	u32 val;

	spin_lock_irqsave(&chan->vc.lock, flags);

	if (chan->chip->dw->hdata->reg_map_8_channels) {
		val = axi_dma_ioread32(chan->chip, DMAC_CHEN_L);
		val |= BIT(chan->id) << DMAC_CHAN_L_EN_SUSP_SHIFT |
			BIT(chan->id) << DMAC_CHAN_L_EN_SUSP_WE_SHIFT;
		axi_dma_iowrite32(chan->chip, DMAC_CHEN_L, val);
	} else {
		if (chan->id > 16) {
			val = axi_dma_ioread32(chan->chip, DMAC_CHSUSP_H);
			val |= (BIT(chan->id - 16) << DMAC_CHSUSP_H_EN_SUSP_SHIFT);
			val |= (BIT(chan->id - 16) << DMAC_CHSUSP_H_EN_SUSP_WE_SHIFT);
			axi_dma_iowrite32(chan->chip, DMAC_CHSUSP_H, val);
		} else {
			val = axi_dma_ioread32(chan->chip, DMAC_CHSUSP_L);
			val |=  (BIT(chan->id) << DMAC_CHSUSP_L_EN_SUSP_SHIFT);
			val |=	(BIT(chan->id) << DMAC_CHSUSP_L_EN_SUSP_WE_SHIFT);
			axi_dma_iowrite32(chan->chip, DMAC_CHSUSP_L, val);
		}
	}

	do  {
		if (axi_chan_irq_read(chan) & DWAXIDMAC_IRQ_SUSPENDED)
			break;

		udelay(2);
	} while (--timeout);

	axi_chan_irq_clear(chan, DWAXIDMAC_IRQ_SUSPENDED);

	chan->is_paused = true;

	spin_unlock_irqrestore(&chan->vc.lock, flags);

	return timeout ? 0 : -EAGAIN;
}

/* Called in chan locked context */
static inline void axi_chan_resume(struct axi_dma_chan *chan)
{
	u32 val;

	if (chan->chip->dw->hdata->reg_map_8_channels) {
		val = axi_dma_ioread32(chan->chip, DMAC_CHEN_L);
		val &= ~(BIT(chan->id) << DMAC_CHAN_L_EN_SUSP_SHIFT);
		val |=  (BIT(chan->id) << DMAC_CHAN_L_EN_SUSP_WE_SHIFT);
		axi_dma_iowrite32(chan->chip, DMAC_CHEN_L, val);
	} else {
		if (chan->id > 16) {
			val = axi_dma_ioread32(chan->chip, DMAC_CHSUSP_H);
			val &= ~(BIT(chan->id - 16) << DMAC_CHSUSP_H_EN_SUSP_SHIFT);
			val |=	(BIT(chan->id - 16) << DMAC_CHSUSP_H_EN_SUSP_WE_SHIFT);
			axi_dma_iowrite32(chan->chip, DMAC_CHSUSP_H, val);
		} else {
			val = axi_dma_ioread32(chan->chip, DMAC_CHSUSP_L);
			val &= ~(BIT(chan->id) << DMAC_CHSUSP_L_EN_SUSP_SHIFT);
			val |=  (BIT(chan->id) << DMAC_CHSUSP_L_EN_SUSP_WE_SHIFT);
			axi_dma_iowrite32(chan->chip, DMAC_CHSUSP_L, val);
		}
	}

	chan->is_paused = false;
}

static int dma_chan_resume(struct dma_chan *dchan)
{
	struct axi_dma_chan *chan = dchan_to_axi_dma_chan(dchan);
	unsigned long flags;

	spin_lock_irqsave(&chan->vc.lock, flags);

	if (chan->is_paused)
		axi_chan_resume(chan);

	spin_unlock_irqrestore(&chan->vc.lock, flags);

	return 0;
}

static int axi_dma_suspend(struct axi_dma_chip *chip)
{
	axi_dma_irq_disable(chip);
	axi_dma_disable(chip);

	clk_disable_unprepare(chip->core1_clk);
	clk_disable_unprepare(chip->core2_clk);

	return 0;
}

static int axi_dma_resume(struct axi_dma_chip *chip)
{
	int ret;

	ret = clk_prepare_enable(chip->core2_clk);
	if (ret < 0)
		return ret;

	ret = clk_prepare_enable(chip->core1_clk);
	if (ret < 0)
		return ret;

	axi_dma_enable(chip);
	axi_dma_irq_enable(chip);

	return 0;
}

static int __maybe_unused axi_dma_runtime_suspend(struct device *dev)
{
	struct axi_dma_chip *chip = dev_get_drvdata(dev);

	return axi_dma_suspend(chip);
}

static int __maybe_unused axi_dma_runtime_resume(struct device *dev)
{
	struct axi_dma_chip *chip = dev_get_drvdata(dev);

	return axi_dma_resume(chip);
}

static struct dma_chan *dw_axi_dma_of_xlate(struct of_phandle_args *dma_spec,
					    struct of_dma *ofdma)
{
	struct dw_axi_dma *dw = ofdma->of_dma_data;
	struct axi_dma_chan *chan;
	struct dma_chan *dchan;

	dchan = dma_get_any_slave_channel(&dw->dma);
	if (!dchan)
		return NULL;

	chan = dchan_to_axi_dma_chan(dchan);
	chan->hw_handshake_num = dma_spec->args[0];

	chan->virt_mic_type = dma_spec->args[1];
	chan->pre_buf_cnt = dma_spec->args[2];

	if (chan->virt_mic_type > VIRTUAL_TYPE_MAX) {
		pr_err("virt_mic_type[%d] failed \r\n", chan->virt_mic_type);
		return NULL;
	}

	return dchan;
}

static int parse_device_properties(struct axi_dma_chip *chip)
{
	struct device *dev = chip->dev;
	u32 tmp, carr[DMAC_MAX_CHANNELS];
	int ret;

	ret = device_property_read_u32(dev, "dma-channels", &tmp);
	if (ret)
		return ret;
	if (tmp == 0 || tmp > DMAC_MAX_CHANNELS)
		return -EINVAL;

	chip->dw->hdata->nr_channels = tmp;
	if (tmp <= DMA_REG_MAP_CH_REF)
		chip->dw->hdata->reg_map_8_channels = true;

	ret = device_property_read_u32(dev, "snps,dma-masters", &tmp);
	if (ret)
		return ret;
	if (tmp == 0 || tmp > DMAC_MAX_MASTERS)
		return -EINVAL;

	chip->dw->hdata->nr_masters = tmp;

	ret = device_property_read_u32(dev, "snps,mem-master", &tmp);
	if (ret)
		chip->dw->hdata->mem_master = DWAXIDMAC_MASTER_0;
	else
		chip->dw->hdata->mem_master = tmp;

	if(tmp < 0 || tmp >= chip->dw->hdata->nr_masters)
		chip->dw->hdata->mem_master = DWAXIDMAC_MASTER_0;

	ret = device_property_read_u32(dev, "snps,dev-master", &tmp);
	if (ret)
		chip->dw->hdata->dev_master = DWAXIDMAC_MASTER_0;
	else
		chip->dw->hdata->dev_master = tmp;

	if(tmp < 0 || tmp >= chip->dw->hdata->nr_masters)
		chip->dw->hdata->dev_master = DWAXIDMAC_MASTER_0;

	ret = device_property_read_u32(dev, "snps,data-width", &tmp);
	if (ret)
		return ret;
	if (tmp > DWAXIDMAC_TRANS_WIDTH_MAX)
		return -EINVAL;

	chip->dw->hdata->m_data_width = tmp;

	ret = device_property_read_u32_array(dev, "snps,block-size", carr,
					     chip->dw->hdata->nr_channels);
	if (ret)
		return ret;
	for (tmp = 0; tmp < chip->dw->hdata->nr_channels; tmp++) {
		if (carr[tmp] == 0 || carr[tmp] > DMAC_MAX_BLK_SIZE)
			return -EINVAL;

		chip->dw->hdata->block_size[tmp] = carr[tmp];
	}

	ret = device_property_read_u32_array(dev, "snps,priority", carr,
					     chip->dw->hdata->nr_channels);
	if (ret)
		return ret;
	/* Priority value must be programmed within [0:nr_channels-1] range */
	for (tmp = 0; tmp < chip->dw->hdata->nr_channels; tmp++) {
		if (carr[tmp] >= chip->dw->hdata->nr_channels)
			return -EINVAL;

		chip->dw->hdata->priority[tmp] = carr[tmp];
	}

	/* axi-max-burst-len is optional property */
	ret = device_property_read_u32(dev, "snps,axi-max-burst-len", &tmp);
	if (!ret) {
		if (tmp > DWAXIDMAC_ARWLEN_MAX + 1)
			return -EINVAL;
		if (tmp < DWAXIDMAC_ARWLEN_MIN + 1)
			return -EINVAL;

		chip->dw->hdata->restrict_axi_burst_len = true;
		chip->dw->hdata->axi_rw_burst_len = tmp;
	}

	if (device_property_read_bool(dev, "disable-mem2mem"))
		chip->dw->hdata->disable_mem2mem = true;

	return 0;
}

int my_atoi(const char *str)
{
	int total = 0;
	int isNegative = 0;
	int length = 0;
	const char *p = str;
	char temp = '0';
	int i;

	if (NULL == p) {
		return -1;
	}

	while (*p++ != '\0') {
		length++;
	}

	p = str;

	if (*p == '-') {
		isNegative = 1;
	}

	for (i = 0; i < length; i++) {
		temp = *p++;
		if (temp > '9' || temp < '0') {
			continue;
		}
		if (total != 0 || temp != '0') {
			temp -= '0';
			total = total*10 + temp;
		}
	}

	if (isNegative)
		return (0 - total);
	else
		return total;
}

static ssize_t dmac_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{

	g_dbg_flag = !g_dbg_flag;
	return 0;
}

static ssize_t dmac_set(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	int channel = -1;
	struct axi_dma_chip *chip = (struct axi_dma_chip *)dev->driver_data;
	struct dw_axi_dma *dw = chip->dw;
	struct axi_dma_chan *chan;

	channel = my_atoi(buf);

	if (channel < 0 || channel >= DMAC_MAX_CHANNELS) {
		dev_err(dev, "channel err, please ch 0~11\n");
		return count;
	}

	chan = &dw->chan[channel];

	dev_info(chip->dev, "DMAC_ID:	 0x%x\n", readl(chip->regs + DMAC_ID));
	dev_info(chip->dev, "DMAC_CFG:	 0x%x\n", readl(chip->regs + DMAC_CFG));
	dev_info(chip->dev, "DMAC_CHEN_L: 0x%x\n", readl(chip->regs + DMAC_CHEN_L));

	dev_info(chip->dev, "ch%d CH_SAR:	  0x%x\n",
					chan->id, readl(chan->chan_regs + CH_SAR));
	dev_info(chip->dev, "ch%d CH_DAR:	  0x%x\n",
					chan->id, readl(chan->chan_regs + CH_DAR));
	dev_info(chip->dev, "ch%d CH_BLOCK_TS:	  0x%x\n",
					chan->id, readl(chan->chan_regs + CH_BLOCK_TS));
	dev_info(chip->dev, "ch%d CH_CTL_L:	  0x%x\n",
					chan->id, readl(chan->chan_regs + CH_CTL_L));
	dev_info(chip->dev, "ch%d CH_CTL_H:	  0x%x\n",
					chan->id, readl(chan->chan_regs + CH_CTL_H));
	dev_info(chip->dev, "ch%d CH_CFG_L:	  0x%x\n",
					chan->id, readl(chan->chan_regs + CH_CFG_L));
	dev_info(chip->dev, "ch%d CH_CFG_H:	  0x%x\n",
					chan->id, readl(chan->chan_regs + CH_CFG_H));
	dev_info(chip->dev, "ch%d CH_LLP:	  0x%x\n",
					chan->id, readl(chan->chan_regs + CH_LLP));

	return count;
}

static DEVICE_ATTR(dmac_test, S_IWUSR | S_IRUSR, dmac_show, dmac_set);

static struct attribute *dmac_attributes[] = {
	&dev_attr_dmac_test.attr,
	NULL
};

static const struct attribute_group dmac_attr_group = {
	.attrs = dmac_attributes,
};


static int dw_probe(struct platform_device *pdev)
{
	struct axi_dma_chip *chip;
	struct resource *mem;
	struct dw_axi_dma *dw;
	struct dw_axi_dma_hcfg *hdata;
	u32 i;
	int ret;

	chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	dw = devm_kzalloc(&pdev->dev, sizeof(*dw), GFP_KERNEL);
	if (!dw)
		return -ENOMEM;

	hdata = devm_kzalloc(&pdev->dev, sizeof(*hdata), GFP_KERNEL);
	if (!hdata)
		return -ENOMEM;

	chip->dw = dw;
	chip->dev = &pdev->dev;
	chip->dw->hdata = hdata;

	chip->irq = platform_get_irq(pdev, 0);
	if (chip->irq < 0)
		return chip->irq;

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	chip->regs = devm_ioremap_resource(chip->dev, mem);
	if (IS_ERR(chip->regs))
		return PTR_ERR(chip->regs);


#if defined(CONFIG_ARCH_TS_TX5336)
	chip->core1_clk = devm_clk_get(chip->dev, "ahb");
	if (IS_ERR(chip->core1_clk))
		return PTR_ERR(chip->core1_clk);
	chip->core2_clk = devm_clk_get(chip->dev, "axi");
	if (IS_ERR(chip->core2_clk))
		return PTR_ERR(chip->core2_clk);
#else
	chip->core1_clk = devm_clk_get(chip->dev, NULL);
	if (IS_ERR(chip->core1_clk))
		return PTR_ERR(chip->core1_clk);
#endif

	ret = parse_device_properties(chip);
	if (ret)
		return ret;

	dw->chan = devm_kcalloc(chip->dev, hdata->nr_channels,
				sizeof(*dw->chan), GFP_KERNEL);
	if (!dw->chan)
		return -ENOMEM;

	ret = devm_request_irq(chip->dev, chip->irq, dw_axi_dma_interrupt,
			       IRQF_SHARED, KBUILD_MODNAME, chip);
	if (ret)
		return ret;

	INIT_LIST_HEAD(&dw->dma.channels);
	for (i = 0; i < hdata->nr_channels; i++) {
		struct axi_dma_chan *chan = &dw->chan[i];

		chan->chip = chip;
		chan->id = i;
		chan->chan_regs = chip->regs + COMMON_REG_LEN + i * CHAN_REG_LEN;
		atomic_set(&chan->descs_allocated, 0);

		chan->vc.desc_free = vchan_desc_put;
		vchan_init(&chan->vc, &dw->dma);
	}

	/* Set capabilities */
	if(!chip->dw->hdata->disable_mem2mem)
		dma_cap_set(DMA_MEMCPY, dw->dma.cap_mask);

	dma_cap_set(DMA_SLAVE, dw->dma.cap_mask);
	dma_cap_set(DMA_CYCLIC, dw->dma.cap_mask);

	/* DMA capabilities */
	dw->dma.chancnt = hdata->nr_channels;
	dw->dma.max_burst = hdata->axi_rw_burst_len;
	dw->dma.src_addr_widths = AXI_DMA_BUSWIDTHS;
	dw->dma.dst_addr_widths = AXI_DMA_BUSWIDTHS;

	if(!chip->dw->hdata->disable_mem2mem)
		dw->dma.directions = BIT(DMA_MEM_TO_MEM);

	dw->dma.directions |= BIT(DMA_MEM_TO_DEV) | BIT(DMA_DEV_TO_MEM);
	dw->dma.residue_granularity = DMA_RESIDUE_GRANULARITY_BURST;

	dw->dma.dev = chip->dev;
	dw->dma.device_tx_status = dma_chan_tx_status;
	dw->dma.device_issue_pending = dma_chan_issue_pending;
	dw->dma.device_terminate_all = dma_chan_terminate_all;
	dw->dma.device_pause = dma_chan_pause;
	dw->dma.device_resume = dma_chan_resume;

	dw->dma.device_alloc_chan_resources = dma_chan_alloc_chan_resources;
	dw->dma.device_free_chan_resources = dma_chan_free_chan_resources;

	dw->dma.device_prep_dma_memcpy = dma_chan_prep_dma_memcpy;
	dw->dma.device_synchronize = dw_axi_dma_synchronize;
	dw->dma.device_config = dw_axi_dma_chan_slave_config;
	dw->dma.device_prep_slave_sg = dw_axi_dma_chan_prep_slave_sg;
	dw->dma.device_prep_dma_cyclic = dw_axi_dma_chan_prep_cyclic;
	dw->dma.device_prep_dma_interval_memcpy = dw_axi_dma_chan_prep_interval_memcpy;

    dmaenginem_set_dev_type(&dw->dma, AXI_DMAC_FLAG);

    pr_debug("dw axi dmac register dma_flag_id:%d\n", dw->dma.dma_flag_id);
	/*
	 * Synopsis DesignWare AxiDMA datasheet mentioned Maximum
	 * supported blocks is 1024. Device register width is 4 bytes.
	 * Therefore, set constraint to 1024 * 4.
	 */
	dw->dma.dev->dma_parms = &dw->dma_parms;
	dma_set_max_seg_size(&pdev->dev, MAX_BLOCK_SIZE);
	platform_set_drvdata(pdev, chip);

	pm_runtime_enable(chip->dev);

	/*
	 * We can't just call pm_runtime_get here instead of
	 * pm_runtime_get_noresume + axi_dma_resume because we need
	 * driver to work also without Runtime PM.
	 */
	pm_runtime_get_noresume(chip->dev);
	ret = axi_dma_resume(chip);
	if (ret < 0)
		goto err_pm_disable;

	axi_dma_hw_init(chip);

	pm_runtime_put(chip->dev);

	ret = dmaenginem_async_device_register(&dw->dma);
	if (ret)
		goto err_pm_disable;

	/* Register with OF helpers for DMA lookups */
	ret = of_dma_controller_register(pdev->dev.of_node,
					 dw_axi_dma_of_xlate, dw);
	if (ret < 0)
		dev_warn(&pdev->dev,
			 "Failed to register OF DMA controller, fallback to MEM_TO_MEM mode\n");


	ret = sysfs_create_group(&pdev->dev.kobj, &dmac_attr_group);
	if (ret < 0)
		dev_warn(&pdev->dev, "attr group create failed\n");

	return 0;

err_pm_disable:
	pm_runtime_disable(chip->dev);

	return ret;
}

static int dw_remove(struct platform_device *pdev)
{
	struct axi_dma_chip *chip = platform_get_drvdata(pdev);
	struct dw_axi_dma *dw = chip->dw;
	struct axi_dma_chan *chan, *_chan;
	u32 i;

	/* Enable clk before accessing to registers */
	clk_prepare_enable(chip->core2_clk);
	clk_prepare_enable(chip->core1_clk);
	axi_dma_irq_disable(chip);
	for (i = 0; i < dw->hdata->nr_channels; i++) {
		axi_chan_disable(&chip->dw->chan[i]);
		axi_chan_irq_disable(&chip->dw->chan[i], DWAXIDMAC_IRQ_ALL);
	}
	axi_dma_disable(chip);

	pm_runtime_disable(chip->dev);
	axi_dma_suspend(chip);

	devm_free_irq(chip->dev, chip->irq, chip);

	of_dma_controller_free(chip->dev->of_node);

	list_for_each_entry_safe(chan, _chan, &dw->dma.channels,
			vc.chan.device_node) {
		list_del(&chan->vc.chan.device_node);
		tasklet_kill(&chan->vc.task);
	}

	return 0;
}

static const struct dev_pm_ops dw_axi_dma_pm_ops = {
	SET_RUNTIME_PM_OPS(axi_dma_runtime_suspend, axi_dma_runtime_resume, NULL)
};

static const struct of_device_id dw_dma_of_id_table[] = {
	{ .compatible = "snps,axi-dma-1.01a" },
	{ .compatible = "intel,kmb-axi-dma" },
	{}
};
MODULE_DEVICE_TABLE(of, dw_dma_of_id_table);

static struct platform_driver dw_driver = {
	.probe		= dw_probe,
	.remove		= dw_remove,
	.driver = {
		.name	= KBUILD_MODNAME,
		.of_match_table = dw_dma_of_id_table,
		.pm = &dw_axi_dma_pm_ops,
	},
};
module_platform_driver(dw_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Synopsys DesignWare AXI DMA Controller platform driver");
MODULE_AUTHOR("Eugeniy Paltsev <Eugeniy.Paltsev@synopsys.com>");
