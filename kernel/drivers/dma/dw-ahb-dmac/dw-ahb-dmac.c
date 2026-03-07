#include <linux/module.h>
#include <linux/device.h>
#include <linux/clk.h>
#include <linux/pm_runtime.h>
#include <linux/platform_device.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/of.h>
#include <linux/of_dma.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/dmapool.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/io-64-nonatomic-lo-hi.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/property.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/bitops.h>

#include "dw-ahb-dmac.h"

#include "../dmaengine.h"
#include "../virt-dma.h"

#define DRV_NAME	"dw_ahb_dmac"

/* The set of bus widths supported by the DMA controller */
#define DW_AHB_DMA_BUSWIDTHS			  \
	BIT(DMA_SLAVE_BUSWIDTH_UNDEFINED)	| \
	BIT(DMA_SLAVE_BUSWIDTH_1_BYTE)		| \
	BIT(DMA_SLAVE_BUSWIDTH_2_BYTES)		| \
	BIT(DMA_SLAVE_BUSWIDTH_4_BYTES)

#define MAX_BUF_CNT (16)

struct virtual_mic_cfg {
	unsigned char init_done;
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

	struct dw_ahb_dma_chan *tx_chan;
	struct dw_ahb_dma_chan *rx_chan[2]; //rx0,rx1
	unsigned char play_start_flag;
	unsigned char rx_channels;
	unsigned char rx_start_flag[2]; //rx0, rx1
};

static struct virtual_mic_cfg virt_mic_cfg;
static inline void dw_ahb_dma_chan_enable(struct dw_ahb_dma_chan *chan);
static int dw_ahb_dma_virt_mic_init(struct dw_ahb_dma_chan *chan,
						size_t buf_len, size_t period_len)
{
	int i;
	//chan->virt_mic_type = VIRTUAL_MIC_TYPE_INTER_CODEC_V1;

	//pr_err("chan dir = %d, virt_mic type = %d, pre_buf_cnt = %d\r\n",
	//			chan->direction, chan->virt_mic_type, chan->pre_buf_cnt);

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

static size_t dw_ahb_dma_virt_mic_get_real_period_len(
			struct dw_ahb_dma_chan *chan, size_t period_len)
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

static void dw_ahb_dma_virt_mic_set_cfg(struct dw_ahb_dma_chan *chan,
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
dw_ahb_dma_virt_mic_set_block_ts(struct dw_ahb_dma_chan *chan,
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

static void dw_ahb_dma_virt_mic_tx_start(struct dw_ahb_dma_chan *chan)
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
		dw_ahb_dma_chan_enable(chan);
	}
}

static void dw_ahb_dma_virt_mic_rx_start(struct dw_ahb_dma_chan *chan)
{
	if (!chan->cyclic)
		return;

	if (chan->direction != DMA_DEV_TO_MEM)
		return ;

	if (chan->cyclic && chan->direction == DMA_DEV_TO_MEM) {
		if (virt_mic_cfg.rx_channels == 2) {
			virt_mic_cfg.rx_start_flag[chan->config.rx_chan_id] = 1;
		} else {
			virt_mic_cfg.cap_open = 1;
		}
	}

	/* when rx_channel = 2, cap_open set 1 */
	if (virt_mic_cfg.rx_start_flag[0] == 1 && virt_mic_cfg.rx_start_flag[1] == 1) {
		virt_mic_cfg.cap_open = 1;
	}
}

static void dw_ahb_dma_virt_mic_update(struct dw_ahb_dma_chan *chan,
					struct dw_ahb_dma_desc *desc)
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

		//pr_err("0 r idx = %d\n", virt_mic_cfg.play_r);

		if (virt_mic_cfg.buffer_used[virt_mic_cfg.play_r] &&
			virt_mic_cfg.play_open && virt_mic_cfg.buffer[virt_mic_cfg.play_r]) {
			memcpy(virt_mic_cfg.cap_addr[cap_buf_idx] + virt_mic_cfg.virt_mic_offset,
				virt_mic_cfg.buffer[virt_mic_cfg.play_r],
				virt_mic_cfg.virt_mic_len);
			virt_mic_cfg.buffer_used[virt_mic_cfg.play_r] = 0;
			//pr_err("r idx = %d\n", virt_mic_cfg.play_r);
			virt_mic_cfg.play_r++;
		} else {
			memset(virt_mic_cfg.cap_addr[cap_buf_idx] + virt_mic_cfg.virt_mic_offset,
				0, virt_mic_cfg.virt_mic_len);
		}

		if (virt_mic_cfg.play_open == 0 && virt_mic_cfg.play_start_flag == 1)
			dw_ahb_dma_virt_mic_tx_start(virt_mic_cfg.tx_chan);

		if (!virt_mic_cfg.play_open) {
			memset(&virt_mic_cfg.buffer_used, 0, sizeof(virt_mic_cfg.buffer_used));
			virt_mic_cfg.play_r = 0;
		}
	}
}

static void dw_ahb_dma_virt_mic_close(struct dw_ahb_dma_chan *chan)
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

static void dw_ahb_dma_enable(struct dw_ahb_dma_chip *chip)
{
	dma_writel(chip, CFG, DW_CFG_DMA_EN);
}

static void dw_ahb_dma_disable(struct dw_ahb_dma_chip *chip)
{
	dma_writel(chip, CFG, 0);

	channel_clear_bit(chip, MASK.XFER, chip->dw->all_chan_mask);
	channel_clear_bit(chip, MASK.BLOCK, chip->dw->all_chan_mask);
	channel_clear_bit(chip, MASK.SRC_TRAN, chip->dw->all_chan_mask);
	channel_clear_bit(chip, MASK.DST_TRAN, chip->dw->all_chan_mask);
	channel_clear_bit(chip, MASK.ERROR, chip->dw->all_chan_mask);

	while (dma_readl(chip, CFG) & DW_CFG_DMA_EN)
		cpu_relax();
}

static inline void dw_ahb_dma_chan_enable(struct dw_ahb_dma_chan *chan)
{
	struct dw_ahb_dma_chip *chip = chan->chip;

	channel_set_bit(chip, CH_EN, chan->mask);
}

static inline void dw_ahb_dma_chan_disable(struct dw_ahb_dma_chan *chan)
{
	struct dw_ahb_dma_chip *chip = chan->chip;

	channel_clear_bit(chip, CH_EN, chan->mask);
	while (dma_readl(chip, CH_EN) & chan->mask)
		cpu_relax();
}

static inline void
dw_ahb_chan_config_write(struct dw_ahb_dma_chan *chan,
							struct dw_ahb_dma_chan_config *config)
{
	u32 cfghi = DWC_CFGH_FIFO_MODE;
	u32 cfglo = DWC_CFGL_CH_PRIOR(config->prior);

	cfghi |= DWC_CFGH_DST_PER(config->dst_per);
	cfghi |= DWC_CFGH_SRC_PER(config->src_per);

	channel_writel(chan, CFG_LO, cfglo);
	channel_writel(chan, CFG_HI, cfghi);
}

/* Called in chan locked context */
static void
dw_ahb_dma_chan_block_xfer_start(struct dw_ahb_dma_chan *chan,
					struct dw_ahb_dma_desc *first)
{
	struct dw_ahb_dma_chip *chip = chan->chip;
	u8 lms = DWC_LLP_LMS(chan->chip->dw->hdata->mem_master);
	struct dw_ahb_dma_chan_config config = {};

	/* ASSERT:  channel is idle */
	if (dma_readl(chip, CH_EN) & chan->mask) {
		dev_err(chan2dev(chan), "%s %s is not idle?\n",
						__func__, dw_ahb_chan_name(chan));
		return;
	}

	dw_ahb_dma_enable(chip);

	config.prior = chan->priority;
	switch (chan->direction) {
	case DMA_MEM_TO_DEV:
		config.dst_per = chan->hw_handshake_num;
		if (chan->cyclic)
			virt_mic_cfg.tx_chan = chan;
		break;
	case DMA_DEV_TO_MEM:
		config.src_per = chan->hw_handshake_num;
		if (chan->cyclic) {
			virt_mic_cfg.rx_chan[chan->config.rx_chan_id] = chan;
		}

		break;
	default:
		break;
	}

	dw_ahb_chan_config_write(chan, &config);

	/* Enable interrupts */
	channel_set_bit(chip, MASK.XFER, chan->mask);
	channel_set_bit(chip, MASK.ERROR, chan->mask);

	channel_writel(chan, LLP, first->hw_desc[0].llp | lms);
	// for scatter/gather mode, use ctllo & ctlhi in lli
	if (first->hw_desc[0].lli->ctllo & (DWC_CTLL_S_GATH_EN | DWC_CTLL_D_SCAT_EN)) {
		channel_writel(chan, CTL_LO, first->hw_desc[0].lli->ctllo);
		channel_writel(chan, CTL_HI, first->hw_desc[0].lli->ctlhi);
	} else {
		channel_writel(chan, CTL_LO, DWC_CTLL_LLP_D_EN | DWC_CTLL_LLP_S_EN);
		channel_writel(chan, CTL_HI, 0);
	}

	dw_ahb_dma_virt_mic_rx_start(chan);

	/* cyclic rx, rx_channels != 2 */
	if ((virt_mic_cfg.rx_channels != 2) &&
			chan->cyclic && chan->direction == DMA_DEV_TO_MEM) {
		dw_ahb_dma_chan_enable(chan);
	}

	/* cyclic rx, rx_channels == 2 */
	if ((virt_mic_cfg.rx_channels == 2) &&
			chan->cyclic && chan->direction == DMA_DEV_TO_MEM &&
			virt_mic_cfg.cap_open) {
		dw_ahb_dma_chan_enable(virt_mic_cfg.rx_chan[0]);
		dw_ahb_dma_chan_enable(virt_mic_cfg.rx_chan[1]);
	}

	if (virt_mic_cfg.cap_open) { /* cyclic tx, but cyclic rx must is working */
		if (chan->cyclic && chan->direction == DMA_MEM_TO_DEV)
			virt_mic_cfg.play_start_flag = 1;
	} else { /* cyclic tx, but cyclic rx not work */
		if (chan->cyclic && chan->direction == DMA_MEM_TO_DEV)
			dw_ahb_dma_chan_enable(chan);
	}

	/* not cyclic, tx/rx */
	if (!chan->cyclic)
		dw_ahb_dma_chan_enable(chan);

}

static void dw_ahb_chan_start_first_queued(struct dw_ahb_dma_chan *chan)
{
	struct dw_ahb_dma_desc *desc;
	struct virt_dma_desc *vd;

	vd = vchan_next_desc(&chan->vc);
	if (!vd)
		return;

	if (chan->cyclic) {
		/* Enable interrupts to perform cyclic transfer */
		channel_set_bit(chan->chip, MASK.BLOCK, chan->mask);
	}

	//pr_err("start chan dir = %d\r\n", chan->direction);

	desc = vd_to_ahb_dma_desc(vd);
	dev_vdbg(chan2dev(chan), "%s: started %u\n", dw_ahb_chan_name(chan),
		vd->tx.cookie);
	dw_ahb_dma_chan_block_xfer_start(chan, desc);
}

static inline void
dw_ahb_dma_dump_lli(struct dw_ahb_dma_chan *chan,
				struct dw_ahb_dma_hw_desc *desc)
{
	dev_err(dchan2dev(&chan->vc.chan), "  desc: s0x%x d0x%x l0x%x c0x%x:%x\n",
		 lli_read(desc, sar),
		 lli_read(desc, dar),
		 lli_read(desc, llp),
		 lli_read(desc, ctlhi),
		 lli_read(desc, ctllo));
}

static void dw_ahb_dma_chan_list_dump_lli(struct dw_ahb_dma_chan *chan,
						struct dw_ahb_dma_desc *desc_head)
{
	int count = atomic_read(&chan->descs_allocated);
	int i;

	for (i = 0; i < count; i++) {
		dw_ahb_dma_dump_lli(chan, &desc_head->hw_desc[i]);
	}
}

static noinline void
dw_ahb_dma_chan_handle_err(struct dw_ahb_dma_chan *chan)
{
	struct virt_dma_desc *vd;
	struct dw_ahb_dma_chip *chip = chan->chip;

	unsigned long flags;

	spin_lock_irqsave(&chan->vc.lock, flags);

	dma_writel(chip, CLEAR.ERROR, chan->mask);
	dw_ahb_dma_chan_disable(chan);

	/* The bad descriptor currently is in the head of vc list */
	vd = vchan_next_desc(&chan->vc);
	/* Remove the completed descriptor from issued list */
	list_del(&vd->node);

	/* WARN about bad descriptor */
	dev_err(chan2dev(chan),
		"Bad descriptor submitted for %s, cookie: %d\n",
		dw_ahb_chan_name(chan), vd->tx.cookie);

	dw_ahb_dma_chan_list_dump_lli(chan, vd_to_ahb_dma_desc(vd));

	vchan_cookie_complete(vd);

	/* Try to restart the controller */
	dw_ahb_chan_start_first_queued(chan);

	spin_unlock_irqrestore(&chan->vc.lock, flags);
}

static void
dw_ahb_dma_chan_cyclic_block_xfer_complete(
				struct dw_ahb_dma_chan *chan, u32 status_block)
{
	int count = atomic_read(&chan->descs_allocated);
	struct dw_ahb_dma_chip *chip = chan->chip;
	struct dw_ahb_dma_desc *desc;
	struct dw_ahb_dma_hw_desc *hw_desc;
	struct virt_dma_desc *vd;
	unsigned long flags;
	u8	m_master = chan->chip->dw->hdata->mem_master;
	u8 lms = DWC_LLP_LMS(m_master);
	u32 llp;
	int i;

	spin_lock_irqsave(&chan->vc.lock, flags);

	if (status_block & chan->mask) {
		dma_writel(chip, CLEAR.BLOCK, chan->mask);
		/* The completed descriptor currently is in the head of vc list */
		vd = vchan_next_desc(&chan->vc);
		desc = vd_to_ahb_dma_desc(vd);
		if (desc) {
			llp = channel_readl(chan, LLP);
			for (i = 0; i < count; i++) {
				hw_desc = &desc->hw_desc[i];
				//pr_err("hw_desc->llp = 0x%x, llp = 0x%x, chan = %d\n", hw_desc->llp, llp, chan->id);
				if ((hw_desc->llp | lms) == llp) {
					desc->completed_blocks = i - 1;
					if (desc->completed_blocks < 0)
						desc->completed_blocks = count - 1;
					//pr_err("completed_blocks = %d, chan = %d\n", desc->completed_blocks, chan->id);

					dw_ahb_dma_virt_mic_update(chan, desc);

					vchan_cyclic_callback(vd);
					break;
				}
			}
		}
	}

	/* Re-enable interrupts */
	channel_set_bit(chip, MASK.BLOCK, chan->mask);

	spin_unlock_irqrestore(&chan->vc.lock, flags);
}


static void
dw_ahb_dma_chan_block_xfer_complete(struct dw_ahb_dma_chan *chan)
{
	struct dw_ahb_dma_chip *chip = chan->chip;
	struct virt_dma_desc *vd;
	unsigned long flags;

	dma_writel(chip, CLEAR.XFER, chan->mask);

	spin_lock_irqsave(&chan->vc.lock, flags);

	/* ASSERT:  channel is idle */
	if (dma_readl(chip, CH_EN) & chan->mask) {
		dev_err(chan2dev(chan), "%s DMA channel not idle?\n", __func__);
		dw_ahb_dma_chan_disable(chan);
	}

	/* The completed descriptor currently is in the head of vc list */
	vd = vchan_next_desc(&chan->vc);

	/* Remove the completed descriptor from issued list before completing */
	list_del(&vd->node);
	vchan_cookie_complete(vd);

	/* Submit queued descriptors after processing the completed ones */
	dw_ahb_chan_start_first_queued(chan);

	spin_unlock_irqrestore(&chan->vc.lock, flags);
}

static irqreturn_t dw_ahb_dma_interrupt(int irq, void *dev_id)
{
	struct dw_ahb_dma_chip *chip = dev_id;
	struct dw_ahb_dma_chan	*chan;
	u32 status_xfer;
	u32 status_block;
	u32 status_err;
	u32 status;
	unsigned int i;

	/*
	 * bit[0]: statusXfr
	 * bit[1]: statusBlock
	 * bit[2]: statusSrcTran
	 * bit[3]: statusDstTran
	 * bit[4]: statusErr
	 */
	status = dma_readl(chip, STATUS_INT);
	//dev_err(chip->dw->dma.dev, "%s: status=0x%x\n", __func__, status);

	/* Check if we have any interrupt from the DMAC */
	if (!status)
		return IRQ_NONE;

	/*
	 * Just disable the interrupts. We'll turn them back on in the
	 * softirq handler.
	 */
	channel_clear_bit(chip , MASK.XFER, chip->dw->all_chan_mask);
	channel_clear_bit(chip, MASK.BLOCK, chip->dw->all_chan_mask);
	channel_clear_bit(chip , MASK.ERROR, chip->dw->all_chan_mask);

	status_xfer = dma_readl(chip, RAW.XFER);
	status_err = dma_readl(chip, RAW.ERROR);
	status_block = dma_readl(chip, RAW.BLOCK);

	/* Poll, clear and process every chanel interrupt status */
	for (i = 0; i < chip->dw->hdata->nr_channels; i++) {
		chan = &chip->dw->chan[i];
		if (chan->cyclic)
			dw_ahb_dma_chan_cyclic_block_xfer_complete(chan, status_block);
		else if (status_err & (1 << i))
			dw_ahb_dma_chan_handle_err(chan);
		else if (status_xfer & (1 << i))
			dw_ahb_dma_chan_block_xfer_complete(chan);
	}

	/* Re-enable interrupts */
	channel_set_bit(chip, MASK.XFER, chip->dw->all_chan_mask);
	channel_set_bit(chip, MASK.ERROR, chip->dw->all_chan_mask);

	return IRQ_HANDLED;
}

static struct dma_chan *
			dw_ahb_dma_of_xlate(struct of_phandle_args *dma_spec,
						struct of_dma *ofdma)
{
	struct dw_ahb_dma *dw = ofdma->of_dma_data;
	struct dw_ahb_dma_chan *chan;
	struct dma_chan *dchan;

	dchan = dma_get_any_slave_channel(&dw->dma);
	if (!dchan)
		return NULL;

	chan = dchan_to_dw_ahb_dma_chan(dchan);
	chan->hw_handshake_num = dma_spec->args[0];
	chan->virt_mic_type = dma_spec->args[1];
	chan->pre_buf_cnt = dma_spec->args[2];

	if (chan->virt_mic_type > VIRTUAL_TYPE_MAX) {
		pr_err("virt_mic_type[%d] failed \r\n", chan->virt_mic_type);
		return NULL;
	}

	return dchan;
}

static struct dw_ahb_dma_desc *dw_ahb_dma_desc_alloc(u32 num)
{
	struct dw_ahb_dma_desc *desc;

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

static struct dw_ahb_dma_lli *
dw_ahb_dma_desc_get(struct dw_ahb_dma_chan *chan, dma_addr_t *addr)
{
	struct dw_ahb_dma_lli *lli;
	dma_addr_t phys;

	lli = dma_pool_zalloc(chan->desc_pool, GFP_NOWAIT, &phys);
	if (unlikely(!lli)) {
		dev_err(chan2dev(chan), "%s: not enough descriptors available\n",
			dw_ahb_chan_name(chan));
		return NULL;
	}

	atomic_inc(&chan->descs_allocated);
	*addr = phys;

	return lli;
}

static void dw_ahb_dma_desc_put(struct dw_ahb_dma_desc *desc)
{
	struct dw_ahb_dma_chan *chan = desc->chan;
	int count = atomic_read(&chan->descs_allocated);
	struct dw_ahb_dma_hw_desc *hw_desc;
	int descs_put;

	for (descs_put = 0; descs_put < count; descs_put++) {
		hw_desc = &desc->hw_desc[descs_put];
		dma_pool_free(chan->desc_pool, hw_desc->lli, hw_desc->llp);
	}

	kfree(desc->hw_desc);
	kfree(desc);
	atomic_sub(descs_put, &chan->descs_allocated);
	dev_dbg(chan2dev(chan), "%s: %d descs put, %d still allocated\n",
		dw_ahb_chan_name(chan), descs_put,
		atomic_read(&chan->descs_allocated));
}

static void vchan_desc_put(struct virt_dma_desc *vdesc)
{
	dw_ahb_dma_desc_put(vd_to_ahb_dma_desc(vdesc));
}

static int dw_ahb_dma_set_desc_dest_master(
		struct dw_ahb_dma_hw_desc *hw_desc, struct dw_ahb_dma_chan *chan)
{
	u32 val;

	val = lli_read(hw_desc, ctllo);
	switch (chan->direction) {
	case DMA_MEM_TO_DEV:
		val |= DWC_CTLL_DMS(chan->chip->dw->hdata->dev_master);
		break;
	case DMA_DEV_TO_MEM:
		val |= DWC_CTLL_DMS(chan->chip->dw->hdata->mem_master);
		break;
	case DMA_MEM_TO_MEM:
		val |= DWC_CTLL_DMS(chan->chip->dw->hdata->mem_master);
		break;
	default:
		return -EINVAL;
	}

	lli_write(hw_desc, ctllo, val);

	return 0;
}

static int dw_ahb_dma_set_desc_src_master(
		struct dw_ahb_dma_hw_desc *hw_desc, struct dw_ahb_dma_chan *chan)
{
	u32 val;

	val = lli_read(hw_desc, ctllo);
	switch (chan->direction) {
	case DMA_MEM_TO_DEV:
		val |= DWC_CTLL_SMS(chan->chip->dw->hdata->mem_master);
		break;
	case DMA_DEV_TO_MEM:
		val |= DWC_CTLL_SMS(chan->chip->dw->hdata->dev_master);
		break;
	case DMA_MEM_TO_MEM:
		val |= DWC_CTLL_SMS(chan->chip->dw->hdata->mem_master);
		break;
	default:
		return -EINVAL;
	}

	lli_write(hw_desc, ctllo, val);

	return 0;
}

static void dw_ahb_dma_synchronize(struct dma_chan *dchan)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);

	vchan_synchronize(&chan->vc);
}

static int dw_ahb_dma_alloc_chan_resources(struct dma_chan *dchan)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	struct dw_ahb_dma_chip *chip = chan->chip;

	/* ASSERT:  channel is idle */
	if (dma_readl(chip, CH_EN) & chan->mask) {
		dev_err(chan2dev(chan), "%s DMA channel not idle?\n", __func__);
		return -EIO;
	}

	/* LLI address must be aligned to a 32-byte boundary */
	chan->desc_pool = dma_pool_create(dev_name(chan2dev(chan)),
					chan->chip->dev,
					sizeof(struct dw_ahb_dma_lli),
					32, 0);
	if (!chan->desc_pool) {
		dev_err(chan2dev(chan), "No memory for descriptors\n");
		return -ENOMEM;
	}

	dev_vdbg(dchan2dev(dchan), "%s: allocating\n", dw_ahb_chan_name(chan));

	pm_runtime_get(chan->chip->dev);

	return 0;
}

static void dw_ahb_dma_free_chan_resources(struct dma_chan *dchan)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	struct dw_ahb_dma_chip *chip = chan->chip;

	/* ASSERT:  channel is idle */
	if (dma_readl(chip, CH_EN) & chan->mask) {
		dev_err(chan2dev(chan), "%s DMA channel not idle?\n", __func__);
	}

	/* Disable interrupts */
	channel_clear_bit(chip, MASK.XFER, chan->mask);
	channel_clear_bit(chip, MASK.BLOCK, chan->mask);
	channel_clear_bit(chip, MASK.XFER, chan->mask);
	chan->cyclic = false;

	vchan_free_chan_resources(&chan->vc);

	dma_pool_destroy(chan->desc_pool);
	chan->desc_pool = NULL;
	dev_vdbg(dchan2dev(dchan),
		 "%s: free resources, descriptor still allocated: %u\n",
		 dw_ahb_chan_name(chan), atomic_read(&chan->descs_allocated));

	pm_runtime_put(chan->chip->dev);

}

static size_t
dw_ahb_dma_calculate_block_len(struct dw_ahb_dma_chan *chan,
					dma_addr_t dma_addr, size_t buf_len,
					enum dma_transfer_direction direction)
{
	u32 data_width, reg_width, mem_width;
	size_t dw_ahb_block_ts, block_len;
	u8	m_master = chan->chip->dw->hdata->mem_master;

	dw_ahb_block_ts = chan->chip->dw->hdata->block_size;

	switch (direction) {
	case DMA_MEM_TO_DEV:
		data_width = BIT(chan->chip->dw->hdata->data_width[m_master]);
		mem_width = __ffs(data_width | dma_addr | buf_len);
		if (mem_width > DW_DMA_TRANS_WIDTH_32)
			mem_width = DW_DMA_TRANS_WIDTH_32;

		block_len = dw_ahb_block_ts << mem_width;
		break;
	case DMA_DEV_TO_MEM:
		reg_width = __ffs(chan->config.src_addr_width);
		block_len = dw_ahb_block_ts << reg_width;
		break;
	default:
		block_len = 0;
	}

	return block_len;
}

static int dw_ahb_dma_set_hw_desc(struct dw_ahb_dma_chan *chan,
					struct dw_ahb_dma_hw_desc *hw_desc,
					dma_addr_t mem_addr, size_t len)
{
	u8	m_master = chan->chip->dw->hdata->mem_master;

	unsigned int data_width = BIT(chan->chip->dw->hdata->data_width[m_master]);
	struct dma_slave_config	*sconfig = &chan->config;
	unsigned int reg_width;
	unsigned int mem_width;
	dma_addr_t device_addr;
	size_t dw_ahb_block_ts;
	size_t block_ts;
	u32 ctllo;

	dw_ahb_block_ts = chan->chip->dw->hdata->block_size;

	mem_width = __ffs(data_width | mem_addr | len);
	if (mem_width > DW_DMA_TRANS_WIDTH_32)
		mem_width = DW_DMA_TRANS_WIDTH_32;

	if (!IS_ALIGNED(mem_addr, 4)) {
		dev_err(chan->chip->dev, "invalid buffer alignment\n");
		return -EINVAL;
	}

	switch (chan->direction) {
	case DMA_MEM_TO_DEV:
		reg_width = __ffs(chan->config.dst_addr_width);
		device_addr = chan->config.dst_addr;

		ctllo = DWC_CTLL_DST_WIDTH(reg_width)
				| DWC_CTLL_SRC_WIDTH(mem_width)
				| DWC_CTLL_DST_FIX
				| DWC_CTLL_SRC_INC
				| DWC_CTLL_FC_M2P
				| DWC_CTLL_INT_EN
				| DWC_CTLL_LLP_D_EN
				| DWC_CTLL_LLP_S_EN
				| DWC_CTLL_DST_MSIZE(sconfig->src_maxburst)
				| DWC_CTLL_SRC_MSIZE(sconfig->dst_maxburst);
		ctllo |= sconfig->device_fc ? DWC_CTLL_FC(DW_DMA_FC_P_M2P) :
							DWC_CTLL_FC(DW_DMA_FC_D_M2P);
		block_ts = len >> mem_width;
		break;
	case DMA_DEV_TO_MEM:
		reg_width = __ffs(chan->config.src_addr_width);
		device_addr = chan->config.src_addr;
		ctllo = DWC_CTLL_DST_WIDTH(mem_width)
				| DWC_CTLL_SRC_WIDTH(reg_width)
				| DWC_CTLL_DST_INC
				| DWC_CTLL_SRC_FIX
				| DWC_CTLL_FC_P2M
				| DWC_CTLL_INT_EN
				| DWC_CTLL_LLP_D_EN
				| DWC_CTLL_LLP_S_EN
				| DWC_CTLL_DST_MSIZE(sconfig->src_maxburst)
				| DWC_CTLL_SRC_MSIZE(sconfig->dst_maxburst);
		ctllo |= sconfig->device_fc ? DWC_CTLL_FC(DW_DMA_FC_P_P2M) :
			DWC_CTLL_FC(DW_DMA_FC_D_P2M);
		block_ts = len >> reg_width;
		dw_ahb_dma_virt_mic_set_block_ts(chan, &block_ts);
		break;
	default:
		return -EINVAL;
	}

	if (block_ts > dw_ahb_block_ts)
		return -EINVAL;

	hw_desc->lli = dw_ahb_dma_desc_get(chan, &hw_desc->llp);
	if (unlikely(!hw_desc->lli))
		return -ENOMEM;

	if (chan->direction == DMA_MEM_TO_DEV) {
		lli_write(hw_desc, sar, mem_addr);
		lli_write(hw_desc, dar, device_addr);

	} else {
		lli_write(hw_desc, sar, device_addr);
		lli_write(hw_desc, dar, mem_addr);
	}

	lli_write(hw_desc, ctlhi, block_ts);
	lli_write(hw_desc, ctllo, ctllo);

	dw_ahb_dma_set_desc_src_master(hw_desc, chan);
	dw_ahb_dma_set_desc_dest_master(hw_desc, chan);
	hw_desc->len = len;

	return 0;
}

static struct dma_async_tx_descriptor *
dw_ahb_dma_prep_dma_memcpy(struct dma_chan *dchan, dma_addr_t dest,
					dma_addr_t src, size_t len, unsigned long flags)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	size_t block_ts, max_block_ts, xfer_len = 0;
	struct dw_ahb_dma_hw_desc *hw_desc = NULL;
	struct dw_ahb_dma_desc *desc = NULL;
	u8	m_master = chan->chip->dw->hdata->mem_master;
	u8 lms = DWC_LLP_LMS(m_master);
	u32 xfer_width, num;
	u32	ctllo;
	u32 llp = 0;
	unsigned int data_width = chan->chip->dw->hdata->data_width[m_master];
	unsigned int src_width;
	unsigned int dst_width;

	max_block_ts = chan->chip->dw->hdata->block_size;
	xfer_width = __ffs(data_width | src | dest | xfer_len);
	num = DIV_ROUND_UP(len, max_block_ts << xfer_width);
	desc = dw_ahb_dma_desc_alloc(num);
	if (unlikely(!desc))
		goto err_desc_get;

	desc->chan = chan;
	num = 0;
	desc->length = 0;
	chan->direction = DMA_MEM_TO_MEM;

	src_width = dst_width = __ffs(data_width | src | dest | len);

	while (len) {
		xfer_len = len;
		hw_desc = &desc->hw_desc[num];
		/*
		 * Take care for the alignment.
		 * Actually source and destination widths can be different, but
		 * make them same to be simpler.
		 */
		xfer_width = __ffs(data_width | src | dest | xfer_len);
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

		hw_desc->lli = dw_ahb_dma_desc_get(chan, &hw_desc->llp);
		if (unlikely(!hw_desc->lli))
			goto err_desc_get;

		ctllo = DWC_CTLL_DST_WIDTH(dst_width)
				| DWC_CTLL_SRC_WIDTH(src_width)
				| DWC_CTLL_DST_INC
				| DWC_CTLL_SRC_INC
				| DWC_CTLL_FC_M2M
				| DWC_CTLL_INT_EN
				| DWC_CTLL_LLP_D_EN
				| DWC_CTLL_LLP_S_EN
				| DWC_CTLL_DST_MSIZE(DW_DMA_MSIZE_16)
				| DWC_CTLL_SRC_MSIZE(DW_DMA_MSIZE_16);

		lli_write(hw_desc, sar, src);
		lli_write(hw_desc, dar, dest);
		lli_write(hw_desc, ctllo, ctllo);
		lli_write(hw_desc, ctlhi, block_ts);

		dw_ahb_dma_set_desc_dest_master(hw_desc, chan);
		dw_ahb_dma_set_desc_src_master(hw_desc, chan);

		hw_desc->len = xfer_len;
		desc->length += hw_desc->len;
		/* update the length and addresses for the next loop cycle */
		len -= xfer_len;
		dest += xfer_len;
		src += xfer_len;
		num++;
	}

	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--num];
		lli_write(hw_desc, llp, llp | lms);
		llp = hw_desc->llp;
	} while (num);

	//dw_ahb_dma_chan_list_dump_lli(chan, desc);

	return vchan_tx_prep(&chan->vc, &desc->vd, flags);

err_desc_get:
	if (desc)
		dw_ahb_dma_desc_put(desc);
	return NULL;
}

static struct dma_async_tx_descriptor *
dw_ahb_dma_prep_dma_memcpy_gather(struct dma_chan *dchan, dma_addr_t dest,
					dma_addr_t src, size_t len, size_t src_cnt, size_t incr_num,  unsigned long flags)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	size_t block_ts, max_block_ts, xfer_len = 0;
	struct dw_ahb_dma_hw_desc *hw_desc = NULL;
	struct dw_ahb_dma_desc *desc = NULL;
	u8 m_master = chan->chip->dw->hdata->mem_master;
	u8 lms = DWC_LLP_LMS(m_master);
	u32 xfer_width, num;
	u32 ctllo;
	u32 llp = 0;
	u32 data_width = chan->chip->dw->hdata->data_width[m_master];
	u32 src_width;
	u32 dst_width;
	u32 gather_num;
	u32 sgr = 0;

	//pr_info("[%s] enter, chan id %d\r\n", __func__, chan->id);
#ifdef CONFIG_ARCH_TS_TX5336
	// only chn0 support gather
	if (chan->id != 0) {
		return NULL;
	}
#endif

	max_block_ts = chan->chip->dw->hdata->block_size;
	xfer_width = __ffs(data_width | src | dest | xfer_len);
	num = DIV_ROUND_UP(len, max_block_ts << xfer_width);
	//pr_info("max_block_ts: 0x%lx, xfer_width: %d, num: %d\r\n", max_block_ts, xfer_width, num);
	desc = dw_ahb_dma_desc_alloc(num);
	if (unlikely(!desc))
		goto err_desc_get;

	desc->chan = chan;
	num = 0;
	desc->length = 0;
	chan->direction = DMA_MEM_TO_MEM;

	src_width = dst_width = __ffs(data_width | src | dest | len);

	while (len) {
		xfer_len = len;
		hw_desc = &desc->hw_desc[num];
		/*
		 * Take care for the alignment.
		 * Actually source and destination widths can be different, but
		 * make them same to be simpler.
		 */
		xfer_width = __ffs(data_width | src | dest | xfer_len);
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

		/* resize xfer_len to ingeter times of src_cnt */
		gather_num = xfer_len / src_cnt;
		xfer_len = gather_num * src_cnt;
		block_ts = xfer_len >> xfer_width;
		//pr_info("[num%d] xfer_len: 0x%lx, block_ts: 0x%lx\r\n", num, xfer_len, block_ts);

		hw_desc->lli = dw_ahb_dma_desc_get(chan, &hw_desc->llp);
		if (unlikely(!hw_desc->lli))
			goto err_desc_get;

		ctllo = DWC_CTLL_DST_WIDTH(dst_width)
				| DWC_CTLL_SRC_WIDTH(src_width)
				| DWC_CTLL_DST_INC
				| DWC_CTLL_SRC_INC
				| DWC_CTLL_FC_M2M
				| DWC_CTLL_INT_EN
				| DWC_CTLL_LLP_D_EN
				| DWC_CTLL_LLP_S_EN
				| DWC_CTLL_S_GATH_EN  // gather enable
				| DWC_CTLL_DST_MSIZE(DW_DMA_MSIZE_8)
				| DWC_CTLL_SRC_MSIZE(DW_DMA_MSIZE_8);

		sgr = DWC_SGR_SGC(src_cnt>>src_width) | DWC_SGR_SGI(incr_num>>src_width);

		lli_write(hw_desc, sar, src);
		lli_write(hw_desc, dar, dest);
		lli_write(hw_desc, ctllo, ctllo);
		lli_write(hw_desc, ctlhi, block_ts);
		channel_writel(chan, SGR, sgr);
		channel_writel(chan, DSR, 0);
		//pr_info("SGR: 0x%x\r\n", channel_readl(chan, SGR));

		dw_ahb_dma_set_desc_dest_master(hw_desc, chan);
		dw_ahb_dma_set_desc_src_master(hw_desc, chan);

		hw_desc->len = xfer_len;
		desc->length += hw_desc->len;
		/* update the length and addresses for the next loop cycle */
		len -= xfer_len;
		dest += xfer_len;
		src += (xfer_len + gather_num * incr_num);
		num++;
	}

	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--num];
		lli_write(hw_desc, llp, llp | lms);
		llp = hw_desc->llp;
	} while (num);

	//dw_ahb_dma_chan_list_dump_lli(chan, desc);

	return vchan_tx_prep(&chan->vc, &desc->vd, flags);

err_desc_get:
	if (desc)
		dw_ahb_dma_desc_put(desc);
	return NULL;
}

static struct dma_async_tx_descriptor *
dw_ahb_dma_prep_dma_memcpy_scatter(struct dma_chan *dchan, dma_addr_t dest,
					dma_addr_t src, size_t len, size_t dsc_cnt, size_t incr_num,  unsigned long flags)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	size_t block_ts, max_block_ts, xfer_len = 0;
	struct dw_ahb_dma_hw_desc *hw_desc = NULL;
	struct dw_ahb_dma_desc *desc = NULL;
	u8 m_master = chan->chip->dw->hdata->mem_master;
	u8 lms = DWC_LLP_LMS(m_master);
	u32 xfer_width, num;
	u32 ctllo;
	u32 llp = 0;
	u32 data_width = chan->chip->dw->hdata->data_width[m_master];
	u32 src_width;
	u32 dst_width;
	u32 scatter_num;
	u32 dsr = 0;

	//pr_info("[%s] enter, chan id %d\r\n", __func__, chan->id);
#ifdef CONFIG_ARCH_TS_TX5336
	// only chn0 support scatter
	if (chan->id != 0) {
		return NULL;
	}
#endif

	max_block_ts = chan->chip->dw->hdata->block_size;
	xfer_width = __ffs(data_width | src | dest | xfer_len);
	num = DIV_ROUND_UP(len, max_block_ts << xfer_width);
	//pr_info("max_block_ts: 0x%lx, xfer_width: %d, num: %d\r\n", max_block_ts, xfer_width, num);
	desc = dw_ahb_dma_desc_alloc(num);
	if (unlikely(!desc))
		goto err_desc_get;

	desc->chan = chan;
	num = 0;
	desc->length = 0;
	chan->direction = DMA_MEM_TO_MEM;

	src_width = dst_width = __ffs(data_width | src | dest | len);

	while (len) {
		xfer_len = len;
		hw_desc = &desc->hw_desc[num];
		/*
		 * Take care for the alignment.
		 * Actually source and destination widths can be different, but
		 * make them same to be simpler.
		 */
		xfer_width = __ffs(data_width | src | dest | xfer_len);
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

		/* resize xfer_len to ingeter times of src_cnt */
		scatter_num = xfer_len / dsc_cnt;
		xfer_len = scatter_num * dsc_cnt;
		block_ts = xfer_len >> xfer_width;
		//pr_info("[num%d] xfer_len: 0x%lx, block_ts: 0x%lx\r\n", num, xfer_len, block_ts);


		hw_desc->lli = dw_ahb_dma_desc_get(chan, &hw_desc->llp);
		if (unlikely(!hw_desc->lli))
			goto err_desc_get;

		ctllo = DWC_CTLL_DST_WIDTH(dst_width)
				| DWC_CTLL_SRC_WIDTH(src_width)
				| DWC_CTLL_DST_INC
				| DWC_CTLL_SRC_INC
				| DWC_CTLL_FC_M2M
				| DWC_CTLL_INT_EN
				| DWC_CTLL_LLP_D_EN
				| DWC_CTLL_LLP_S_EN
				| DWC_CTLL_D_SCAT_EN  // scatter enable
				| DWC_CTLL_DST_MSIZE(DW_DMA_MSIZE_8)
				| DWC_CTLL_SRC_MSIZE(DW_DMA_MSIZE_8);

		dsr = DWC_DSR_DSC(dsc_cnt>>dst_width) | DWC_DSR_DSI(incr_num>>dst_width);

		lli_write(hw_desc, sar, src);
		lli_write(hw_desc, dar, dest);
		lli_write(hw_desc, ctllo, ctllo);
		lli_write(hw_desc, ctlhi, block_ts);
		channel_writel(chan, SGR, 0);
		channel_writel(chan, DSR, dsr);
		//pr_info("DSR: 0x%x\r\n", channel_readl(chan, DSR));

		dw_ahb_dma_set_desc_dest_master(hw_desc, chan);
		dw_ahb_dma_set_desc_src_master(hw_desc, chan);

		hw_desc->len = xfer_len;
		desc->length += hw_desc->len;
		/* update the length and addresses for the next loop cycle */
		len -= xfer_len;
		dest += (xfer_len + scatter_num * incr_num);
		src += xfer_len;
		num++;
	}

	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--num];
		lli_write(hw_desc, llp, llp | lms);
		llp = hw_desc->llp;
	} while (num);

	//dw_ahb_dma_chan_list_dump_lli(chan, desc);

	return vchan_tx_prep(&chan->vc, &desc->vd, flags);

err_desc_get:
	if (desc)
		dw_ahb_dma_desc_put(desc);
	return NULL;
}

static struct dma_async_tx_descriptor *
dw_ahb_dma_prep_slave_sg(struct dma_chan *dchan,
				struct scatterlist *sgl, unsigned int sg_len,
				enum dma_transfer_direction direction,
				unsigned long flags, void *context)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	struct dw_ahb_dma_hw_desc *hw_desc = NULL;
	struct dw_ahb_dma_desc *desc = NULL;
	u32 num_segments, segment_len;
	unsigned int loop = 0;
	struct scatterlist *sg;
	size_t dw_ahb_block_len;
	u32 len, num_sgs = 0;
	unsigned int i;
	dma_addr_t mem;
	int status;
	u32 llp = 0;
	u8 m_master = chan->chip->dw->hdata->mem_master;
	u8 lms = DWC_LLP_LMS(m_master);

	dev_vdbg(chan2dev(chan), "%s\n", __func__);
	if (unlikely(!is_slave_direction(direction) || !sg_len))
		return NULL;

	mem = sg_dma_address(sgl);
	len = sg_dma_len(sgl);

	dw_ahb_block_len = dw_ahb_dma_calculate_block_len(chan, mem,
											len, direction);
	if (dw_ahb_block_len == 0)
		return NULL;

	for_each_sg(sgl, sg, sg_len, i)
		num_sgs += DIV_ROUND_UP(sg_dma_len(sg), dw_ahb_block_len);

	desc = dw_ahb_dma_desc_alloc(num_sgs);
	if (unlikely(!desc))
		goto err_desc_get;

	desc->chan = chan;
	desc->length = 0;
	chan->direction = direction;

	for_each_sg(sgl, sg, sg_len, i) {
		mem = sg_dma_address(sg);
		len = sg_dma_len(sg);
		num_segments = DIV_ROUND_UP(sg_dma_len(sg), dw_ahb_block_len);
		segment_len = DIV_ROUND_UP(sg_dma_len(sg), num_segments);

		do {
			hw_desc = &desc->hw_desc[loop++];
			status = dw_ahb_dma_set_hw_desc(chan, hw_desc, mem, segment_len);
			if (status < 0)
				goto err_desc_get;

			desc->length += hw_desc->len;
			len -= segment_len;
			mem += segment_len;
		} while (len >= segment_len);
	}

	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--num_sgs];
		lli_write(hw_desc, llp, llp | lms);
		llp = hw_desc->llp;
	} while (num_sgs);

	return vchan_tx_prep(&chan->vc, &desc->vd, flags);

err_desc_get:
	if (desc)
		dw_ahb_dma_desc_put(desc);

	return NULL;
}

static struct dma_async_tx_descriptor *
dw_ahb_dma_chan_prep_cyclic(struct dma_chan *dchan,
				struct snd_pcm_substream *subtream,
				size_t buf_len, size_t period_len,
				enum dma_transfer_direction direction,
				unsigned long flags)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	struct dw_ahb_dma_hw_desc *hw_desc = NULL;
	struct dw_ahb_dma_desc *desc = NULL;
	dma_addr_t dma_addr = subtream->runtime->dma_addr;
	dma_addr_t src_addr = dma_addr;
	u32 num_periods, num_segments;
	size_t dw_ahb_block_len;
	u32 total_segments;
	u32 segment_len;
	size_t real_period_len;
	unsigned int i;
	int status;
	u32 llp = 0;
	u8 m_master = chan->chip->dw->hdata->mem_master;
	u8 lms = DWC_LLP_LMS(m_master);
	int ret;

	chan->direction = direction;
	chan->cyclic = true;

	num_periods = buf_len / period_len;

	dw_ahb_block_len = dw_ahb_dma_calculate_block_len(chan, dma_addr,
									buf_len, direction);
	if (dw_ahb_block_len == 0)
		return NULL;

	real_period_len = dw_ahb_dma_virt_mic_get_real_period_len(chan, period_len);

	num_segments = DIV_ROUND_UP(real_period_len, dw_ahb_block_len);
	segment_len = DIV_ROUND_UP(period_len, num_segments);
	total_segments = num_periods * num_segments;

	desc = dw_ahb_dma_desc_alloc(total_segments);
	if (unlikely(!desc))
		goto err_desc_get;

	desc->chan = chan;
	desc->length = 0;
	desc->period_len = period_len;

	//pr_err("chan dir = %d, dma_addr = 0x%x, buf_len = %d, period_len = %d\r\n",
	//					chan->direction, dma_addr, buf_len, period_len);

	ret = dw_ahb_dma_virt_mic_init(chan, buf_len, period_len);
	if (ret) {
		pr_err("dw_ahb_dma_virt_mic_init failed with %d\r\n", ret);
		return NULL;
	}

	for (i = 0; i < total_segments; i++) {
		hw_desc = &desc->hw_desc[i];

		status = dw_ahb_dma_set_hw_desc(chan, hw_desc, src_addr,
						segment_len);
		if (status < 0)
			goto err_desc_get;

		dw_ahb_dma_virt_mic_set_cfg(chan, subtream, i, segment_len);

		desc->length += hw_desc->len;
		src_addr += segment_len;
	}

	llp = desc->hw_desc[0].llp;

	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--total_segments];
		lli_write(hw_desc, llp, llp | lms);
		llp = hw_desc->llp;
	} while (total_segments);

	//dw_ahb_dma_chan_list_dump_lli(chan, desc);

	return vchan_tx_prep(&chan->vc, &desc->vd, flags);

err_desc_get:
	if (desc)
		dw_ahb_dma_desc_put(desc);
	return NULL;
}

static int dw_ahb_dma_slave_config(struct dma_chan *dchan,
							struct dma_slave_config *sconfig)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	memcpy(&chan->config, sconfig, sizeof(*sconfig));

	return 0;
}

static int dw_ahb_dma_chan_pause(struct dma_chan *dchan)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	unsigned long	flags;
	unsigned int count = 20;	/* timeout iterations */
	u32	cfglo;
	//pr_err("%s ***** %d ****** chan dir = %d\r\n", __func__, __LINE__, chan->direction);

	spin_lock_irqsave(&chan->vc.lock, flags);
	cfglo = channel_readl(chan, CFG_LO);

	channel_writel(chan, CFG_LO, cfglo | DWC_CFGL_CH_SUSP);
	while (!(channel_readl(chan, CFG_LO) & DWC_CFGL_FIFO_EMPTY) && count--)
		udelay(2);

	chan->is_paused = true;
	spin_unlock_irqrestore(&chan->vc.lock, flags);

	return 0;
}

static int dw_ahb_dma_chan_resume(struct dma_chan *dchan)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	unsigned long flags;
	u32 cfglo;
	//pr_err("%s ***** %d ****** chan dir = %d\r\n", __func__, __LINE__, chan->direction);

	spin_lock_irqsave(&chan->vc.lock, flags);

	cfglo = channel_readl(chan, CFG_LO);

	if (chan->is_paused) {
		channel_writel(chan, CFG_LO, cfglo & ~DWC_CFGL_CH_SUSP);
	}

	chan->is_paused = false;

	spin_unlock_irqrestore(&chan->vc.lock, flags);

	return 0;
}

static int dw_ahb_dma_terminate_all(struct dma_chan *dchan)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	unsigned long flags;

	LIST_HEAD(head);

	dw_ahb_dma_chan_disable(chan);

	spin_lock_irqsave(&chan->vc.lock, flags);

	vchan_get_all_descriptors(&chan->vc, &head);

	dw_ahb_dma_virt_mic_close(chan);

	chan->cyclic = false;
	virt_mic_cfg.rx_start_flag[chan->config.rx_chan_id] = 0;

	spin_unlock_irqrestore(&chan->vc.lock, flags);

	vchan_dma_desc_free_list(&chan->vc, &head);

	return 0;
}

static enum dma_status
dw_ahb_dma_tx_status(struct dma_chan *dchan, dma_cookie_t cookie,
							struct dma_tx_state *txstate)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
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
		length = vd_to_ahb_dma_desc(vdesc)->length;
		completed_blocks = vd_to_ahb_dma_desc(vdesc)->completed_blocks;
		len = vd_to_ahb_dma_desc(vdesc)->hw_desc[0].len;
		completed_length = completed_blocks * len;
		bytes = length - completed_length;
	} else {
		bytes = vd_to_ahb_dma_desc(vdesc)->length;
	}

	//pr_err("%s **** bytes = 0x%x\n", __func__, bytes);

	spin_unlock_irqrestore(&chan->vc.lock, flags);
	dma_set_residue(txstate, bytes);

	return status;
}

static void dw_ahb_dma_issue_pending(struct dma_chan *dchan)
{
	struct dw_ahb_dma_chan *chan = dchan_to_dw_ahb_dma_chan(dchan);
	unsigned long flags;

	spin_lock_irqsave(&chan->vc.lock, flags);
	if (vchan_issue_pending(&chan->vc))
		dw_ahb_chan_start_first_queued(chan);
	spin_unlock_irqrestore(&chan->vc.lock, flags);
}

static int dw_ahb_dma_resume(struct dw_ahb_dma_chip *chip)
{
	int ret;

	ret = clk_prepare_enable(chip->clk);
	if (ret)
		return ret;

	return 0;
}

static int dw_ahb_dma_suspend(struct dw_ahb_dma_chip *chip)
{
	clk_disable_unprepare(chip->clk);
	return 0;
}

static int dw_ahb_dma_hw_init(struct dw_ahb_dma_chip *chip)
{
	int ret;

	ret = dma_coerce_mask_and_coherent(chip->dev, DMA_BIT_MASK(32));
	if (ret)
		return ret;

	return 0;
}

static ssize_t dw_ahb_dma_app(struct device *dev,
					 struct device_attribute *attr, char *buf)

{
	static int count = 1;
	if (count % 2) {
		virt_mic_cfg.rx_channels = 2;
	} else {
		virt_mic_cfg.rx_channels = 1;
	}
	count++;

	pr_err("virt_mic_cfg.rx_channels = %d\n", virt_mic_cfg.rx_channels);

	return 0;
}
static DEVICE_ATTR(ahb_dma_app, S_IRUSR, dw_ahb_dma_app, NULL);

static struct attribute *ahb_dmac_attributes[] = {
	&dev_attr_ahb_dma_app.attr,
	NULL
};

static const struct attribute_group ahb_dmac_attr_group = {
	.attrs = ahb_dmac_attributes,
};

static int dw_ahb_dma_parse_dt(struct dw_ahb_dma_chip *chip)
{
	struct device *dev = chip->dev;
	u32 tmp, arr[DW_AHB_DMA_MAX_NR_MASTERS];
	int ret;
	struct device_node	*ts_codec_node = NULL;

	ret = device_property_read_u32(dev, "dma-masters", &tmp);
	if (ret)
		return ret;
	if (tmp < 1 || tmp > DW_AHB_DMA_MAX_NR_MASTERS)
		return -EINVAL;
	chip->dw->hdata->nr_masters = tmp;

	ret = device_property_read_u32(dev, "dma-channels", &tmp);
	if (ret)
		return ret;
	if (tmp == 0 || tmp > DW_AHB_DMA_MAX_NR_CHANNELS)
		return -EINVAL;
	chip->dw->hdata->nr_channels = tmp;

	if(device_property_read_bool(dev, "dma-is_private"))
		chip->dw->hdata->is_private = true;


	ret = device_property_read_u32(dev, "mem-master", &tmp);
	if (ret)
		return ret;
	chip->dw->hdata->mem_master = tmp;

	ret = device_property_read_u32(dev, "dev-master", &tmp);
	if (ret)
		return ret;
	chip->dw->hdata->dev_master = tmp;

	chip->dw->hdata->is_memcpy = true;

	ret = device_property_read_u32(dev, "chan_allocation_order", &tmp);
	if (ret)
		return ret;
	chip->dw->hdata->chan_allocation_order = (unsigned char)tmp;

	ret = device_property_read_u32(dev, "chan_priority", &tmp);
	if (ret)
		return ret;
	chip->dw->hdata->chan_priority = (unsigned char)tmp;

	ret = device_property_read_u32(dev, "block_size", &tmp);
	if (ret)
		return ret;
	chip->dw->hdata->block_size = tmp;

	ret = device_property_read_u32_array(dev, "data-width", arr,
					     chip->dw->hdata->nr_masters);
	if (ret)
		return ret;
	for (tmp = 0; tmp < chip->dw->hdata->nr_masters; tmp++) {
		if (arr[tmp] == 0 || arr[tmp] > DW_AHB_DMA_MAX_BLK_SIZE)
			return -EINVAL;
		chip->dw->hdata->data_width[tmp] = arr[tmp];
	}

	for (tmp = 0; tmp < chip->dw->hdata->nr_channels; tmp++)
		chip->dw->hdata->multi_block[tmp] = 1;

	/* get ts_codec rx channels */
	ts_codec_node = of_parse_phandle(dev->of_node, "ts,codec", 0);
	if (ts_codec_node) {
		if (!of_property_read_u32(ts_codec_node, "rx,channels",
					 &tmp)) {
			virt_mic_cfg.rx_channels = tmp;
			pr_err("ts codec, rx_channels = %d\n", virt_mic_cfg.rx_channels);
		}
	}

	return 0;
}

static int dw_ahb_dma_probe(struct platform_device *pdev)
{
	struct dw_ahb_dma_chip *chip;
	struct device *dev = &pdev->dev;
	struct resource *mem;
	struct dw_ahb_dma *dw;
	struct dw_ahb_dma_hcfg *hdata;
	int i;
	int err = -1;

	if (!virt_mic_cfg.init_done) {
		memset(&virt_mic_cfg, 0, sizeof(virt_mic_cfg));
		virt_mic_cfg.init_done = 1;
	}

	chip = devm_kzalloc(dev, sizeof(*chip), GFP_KERNEL);
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
	chip->regs = devm_ioremap_resource(dev, mem);
	if (IS_ERR(chip->regs))
		return PTR_ERR(chip->regs);

	chip->rstc = devm_reset_control_get_optional(&pdev->dev, "ahb_dma");
	if (IS_ERR(chip->rstc)) {
		if (PTR_ERR(chip->rstc) != -EPROBE_DEFER)
			dev_err(&pdev->dev, "Failed to get reset!\n");
		return PTR_ERR(chip->rstc);
	} else {
		err = reset_control_reset(chip->rstc);
		if (err) {
			dev_err(&pdev->dev, "Couldn't deassert the device from reset!\n");
			return err;
		}
	}

	chip->clk = devm_clk_get(chip->dev, NULL);
	if (IS_ERR(chip->clk))
		return PTR_ERR(chip->clk);

	err = dw_ahb_dma_parse_dt(chip);
	if (err)
		return err;

	dw->chan = devm_kcalloc(chip->dev, hdata->nr_channels,
				sizeof(*dw->chan), GFP_KERNEL);


	/* Calculate all channel mask before DMA setup */
	chip->dw->all_chan_mask = (1 << hdata->nr_channels) - 1;

	/* Force dma off, just in case */
	dw_ahb_dma_disable(chip);

	err = devm_request_irq(chip->dev, chip->irq, dw_ahb_dma_interrupt,
							IRQF_SHARED, pdev->name, chip);
	if (err)
		return err;

	INIT_LIST_HEAD(&dw->dma.channels);
	for (i = 0; i < hdata->nr_channels; i++) {
		struct dw_ahb_dma_chan *chan = &dw->chan[i];
		chan->mask = 1 << i;
		chan->chip = chip;
		chan->id = i;
		chan->priority = i;
		chan->chan_regs = &__dw_regs(chan->chip)->CHAN[i];;
		atomic_set(&chan->descs_allocated, 0);
		chan->vc.desc_free = vchan_desc_put;
		vchan_init(&chan->vc, &dw->dma);
	}

	/* Clear all interrupts on all channels. */
	dma_writel(chip, CLEAR.XFER, chip->dw->all_chan_mask);
	dma_writel(chip, CLEAR.BLOCK, chip->dw->all_chan_mask);
	dma_writel(chip, CLEAR.SRC_TRAN, chip->dw->all_chan_mask);
	dma_writel(chip, CLEAR.DST_TRAN, chip->dw->all_chan_mask);
	dma_writel(chip, CLEAR.ERROR, chip->dw->all_chan_mask);

	/* Set capabilities */
	dma_cap_set(DMA_SLAVE, dw->dma.cap_mask);
	dma_cap_set(DMA_MEMCPY, dw->dma.cap_mask);
	dma_cap_set(DMA_CYCLIC, dw->dma.cap_mask);
	dma_cap_set(DMA_PRIVATE, dw->dma.cap_mask);

	dw->dma.dev = chip->dev;
	dw->dma.device_tx_status = dw_ahb_dma_tx_status;
	dw->dma.device_issue_pending = dw_ahb_dma_issue_pending;
	dw->dma.device_terminate_all = dw_ahb_dma_terminate_all;
	dw->dma.device_pause = dw_ahb_dma_chan_pause;
	dw->dma.device_resume = dw_ahb_dma_chan_resume;

	dw->dma.device_alloc_chan_resources = dw_ahb_dma_alloc_chan_resources;
	dw->dma.device_free_chan_resources = dw_ahb_dma_free_chan_resources;

	dw->dma.device_prep_dma_memcpy = dw_ahb_dma_prep_dma_memcpy;
	dw->dma.device_synchronize = dw_ahb_dma_synchronize;
	dw->dma.device_config = dw_ahb_dma_slave_config;
	dw->dma.device_prep_slave_sg = dw_ahb_dma_prep_slave_sg;
	dw->dma.device_prep_dma_cyclic = dw_ahb_dma_chan_prep_cyclic;
	dw->dma.device_prep_dma_memcpy_gather = dw_ahb_dma_prep_dma_memcpy_gather;
	dw->dma.device_prep_dma_memcpy_scatter = dw_ahb_dma_prep_dma_memcpy_scatter;

	/* DMA capabilities */
	dw->dma.chancnt = hdata->nr_channels;
	dw->dma.src_addr_widths = DW_AHB_DMA_BUSWIDTHS;
	dw->dma.dst_addr_widths = DW_AHB_DMA_BUSWIDTHS;
	dw->dma.directions = BIT(DMA_DEV_TO_MEM) | BIT(DMA_MEM_TO_DEV) |
						BIT(DMA_MEM_TO_MEM);
	dw->dma.residue_granularity = DMA_RESIDUE_GRANULARITY_BURST;

	dw->dma.dev->dma_parms = &dw->dma_parms;
	dma_set_max_seg_size(&pdev->dev, MAX_BLOCK_SIZE);
	platform_set_drvdata(pdev, chip);

	pm_runtime_enable(chip->dev);

	pm_runtime_get_noresume(chip->dev);
	err = dw_ahb_dma_resume(chip);
	if (err < 0)
		goto err_pm_disable;

	dw_ahb_dma_hw_init(chip);

    dmaenginem_set_dev_type(&(dw->dma), AHB_DMAC_FLAG);

    pr_debug("dw ahb dmac register dma_flag_id:%d\n", dw->dma.dma_flag_id);
	pm_runtime_put(chip->dev);

	err = dmaenginem_async_device_register(&dw->dma);
	if (err)
		goto err_pm_disable;

	/* Register with OF helpers for DMA lookups */
	err = of_dma_controller_register(pdev->dev.of_node,
					 dw_ahb_dma_of_xlate, dw);
	if (err < 0)
		dev_warn(&pdev->dev,
			 "Failed to register OF DMA controller, fallback to MEM_TO_MEM mode\n");

	err = sysfs_create_group(&pdev->dev.kobj, &ahb_dmac_attr_group);
	if (err < 0)
		dev_warn(&pdev->dev, "attr group create failed\n");

	return 0;

err_pm_disable:
	pm_runtime_disable(chip->dev);
	return 0;
}

static int dw_ahb_dma_remove(struct platform_device *pdev)
{
	struct dw_ahb_dma_chip *chip = platform_get_drvdata(pdev);
	struct dw_ahb_dma *dw = chip->dw;
	struct dw_ahb_dma_chan *chan, *_chan;

	/* Enable clk before accessing to registers */
	clk_prepare_enable(chip->clk);

	dw_ahb_dma_disable(chip);

	pm_runtime_disable(chip->dev);
	dw_ahb_dma_suspend(chip);

	devm_free_irq(chip->dev, chip->irq, chip);
	of_dma_controller_free(chip->dev->of_node);

	list_for_each_entry_safe(chan, _chan, &dw->dma.channels,
			vc.chan.device_node) {
		list_del(&chan->vc.chan.device_node);
		tasklet_kill(&chan->vc.task);
	}

	sysfs_remove_group(&pdev->dev.kobj, &ahb_dmac_attr_group);

	return 0;
}

static const struct of_device_id dw_dma_of_id_table[] = {
	{ .compatible = "snps,dma-spear1340-new" },
	{}
};
MODULE_DEVICE_TABLE(of, dw_dma_of_id_table);

#ifdef CONFIG_PM_SLEEP
static int dw_suspend_late(struct device *dev)
{
	struct dw_ahb_dma_chip *chip = dev_get_drvdata(dev);
	return dw_ahb_dma_suspend(chip);
}

static int dw_resume_early(struct device *dev)
{
	struct dw_ahb_dma_chip *chip = dev_get_drvdata(dev);
	return dw_ahb_dma_resume(chip);
}
#endif /* CONFIG_PM_SLEEP */

static const struct dev_pm_ops dw_dev_pm_ops = {
	SET_LATE_SYSTEM_SLEEP_PM_OPS(dw_suspend_late, dw_resume_early)
};

static struct platform_driver dw_ahb_dma_driver = {
	.probe		= dw_ahb_dma_probe,
	.remove		= dw_ahb_dma_remove,
	.driver = {
		.name	= DRV_NAME,
		.pm	= &dw_dev_pm_ops,
		.of_match_table = of_match_ptr(dw_dma_of_id_table),
	},
};
module_platform_driver(dw_ahb_dma_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Synopsys DesignWare DMA Controller platform driver");
MODULE_AUTHOR("Guoyu Yang <yangguoyu@tsingmicro.com>");
MODULE_ALIAS("platform:" DRV_NAME);
