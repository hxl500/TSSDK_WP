#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/delay.h>
#include <linux/bitmap.h>
#include <linux/device.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/gpio.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <sound/core.h>
#include <sound/initval.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/memalloc.h>
#include <linux/platform_data/dma-dw.h>
#include <linux/dma/dw.h>
#include <linux/regmap.h>
#include <sound/tlv.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include <linux/gpio/consumer.h>

#include "ts-pdmic.h"

#define TS_PDMIC_FORMATS (SNDRV_PCM_FMTBIT_S16_LE)

#define TS_PDMIC_RATES (SNDRV_PCM_RATE_16000 | \
						SNDRV_PCM_RATE_32000 | \
						SNDRV_PCM_RATE_48000)

enum {
	DMA_RX_READY,
	DMA_RX_CHAN_PRESENT,
};

static DEFINE_MUTEX(opened_mutex);

struct ts_pdmic_pdata {
	const char *card_name;

	struct clk *pclk;
	struct clk *mclk;

	unsigned int mic_min_freq;
	unsigned int mic_max_freq;

	struct dw_dma_slave	rx_dws;
	struct dw_dma_slave	tx_dws;
};

struct ts_pdmic_dma {
	struct dma_chan		*rx_chan;
	struct dma_chan		*tx_chan;

	struct dma_slave_config	rx_conf;
	struct dma_slave_config	tx_conf;
};

struct ts_pdmic {
	struct platform_device	*pdev;
	int irq;
	resource_size_t io_phy_base;
	void __iomem *regs;
	struct regmap *regmap;

	struct ts_pdmic_dma	dma;

	spinlock_t			lock;
	struct snd_card		*card;
	struct snd_pcm		*pcm;
	int	opened;

	u64					cur_format;
	unsigned int		cur_rate;
	unsigned long		flags;

	struct snd_pcm_substream	*capture_substream;
	int capture_period;

	const struct ts_pdmic_pdata *pdata;
	struct snd_pcm_substream *substream;
};

#define TS_PDMIC_DELAY_US 10
struct ts_pdmic *g_ts_pdmic_chip;

static void ts_pdmic_unprepare_clk(struct ts_pdmic_pdata *pdata);
static int ts_pdmic_prepare_clk(struct ts_pdmic_pdata *pdata);

struct ts_pdmic *ts_pdmic_get_chip_from_card(struct snd_card	*card)
{
	return (struct ts_pdmic *)(card->private_data);
}

/* adc */
static void ts_pdmic_rx_enable(struct ts_pdmic *ts_pdmic_chip,
											bool on)
{
	unsigned int val;

	val = readl(ts_pdmic_chip->regs + PDMIC_ENABLE);
	if (on) {
		val &= ~PDMIC_ENABLE_PDMEN_MASK;
		val |= PDMIC_ENABLE_PDMEN_EN << PDMIC_ENABLE_PDMEN_SHIFT;
	} else {
		val &= ~PDMIC_ENABLE_PDMEN_MASK;
		val |= PDMIC_ENABLE_PDMEN_DIS << PDMIC_ENABLE_PDMEN_SHIFT;
	}

	writel(val, ts_pdmic_chip->regs + PDMIC_ENABLE);
	udelay(TS_PDMIC_DELAY_US);
}

static struct snd_pcm_hardware ts_pdmic_hw = {
	.info			= (SNDRV_PCM_INFO_MMAP
				  | SNDRV_PCM_INFO_MMAP_VALID
				  | SNDRV_PCM_INFO_INTERLEAVED
				  | SNDRV_PCM_INFO_BLOCK_TRANSFER
				  | SNDRV_PCM_INFO_JOINT_DUPLEX
				  | SNDRV_PCM_INFO_RESUME
				  | SNDRV_PCM_INFO_PAUSE),
	.formats		= TS_PDMIC_FORMATS,
	.rates			= TS_PDMIC_RATES,
	.rate_min		= 16000,
	.rate_max		= 48000,
	.channels_min		= 1,
	.channels_max		= 2,
	.buffer_bytes_max	= 1024*4*16,
	.period_bytes_min	= 80*4,
	.period_bytes_max	= 1024*4,
	.periods_min		= 2,
	.periods_max		= 16,
};

// void ts_pdmic_init(struct ts_pdmic *ts_pdmic_chip)
// {
//	int val;

//	writel(0, ts_pdmic_chip->regs + TSING_CODEC_INTR_CTRL);

//	val = (CODEC_INTR_CTRL_AUDIO_CODEC_INTR_EN_MASK) |
//			(CODEC_INTR_CTRL_ADJ_ERR_FLG_EN_MASK);

//	writel(val, ts_pdmic_chip->regs + TSING_CODEC_INTR_CTRL);
// }

/* pdmic end*/

/* This function is called by the DMA driver. */
static void ts_pdmic_dma_capture_period_done(void *arg)
{
	struct ts_pdmic *ts_pdmic_chip = (struct ts_pdmic *)arg;

	snd_pcm_period_elapsed(ts_pdmic_chip->capture_substream);
}

static int ts_pdmic_prepare_dma(struct ts_pdmic *ts_pdmic_chip,
		struct snd_pcm_substream *substream,
		enum dma_transfer_direction direction)
{
	struct dma_chan	*chan = NULL;
	struct dw_cyclic_desc	*cdesc;
	struct snd_pcm_runtime	*runtime = substream->runtime;
	unsigned long	buffer_len, period_len;

	/*
	 * We don't do DMA on "complex" transfers, i.e. with
	 * non-halfword-aligned buffers or lengths.
	 */
	if (runtime->dma_addr & 1 || runtime->buffer_size & 1) {
		dev_dbg(&ts_pdmic_chip->pdev->dev, "too complex transfer\n");
		return -EINVAL;
	}

	if (direction == DMA_MEM_TO_DEV)
		chan = ts_pdmic_chip->dma.tx_chan;
	else
		chan = ts_pdmic_chip->dma.rx_chan;

	buffer_len = frames_to_bytes(runtime, runtime->buffer_size);
	period_len = frames_to_bytes(runtime, runtime->period_size);

	cdesc = dw_dma_cyclic_prep(chan, runtime->dma_addr, buffer_len,
			period_len, direction);
	if (IS_ERR(cdesc)) {
		dev_dbg(&ts_pdmic_chip->pdev->dev, "could not prepare cyclic DMA\n");
		return PTR_ERR(cdesc);
	}

	if (direction == DMA_MEM_TO_DEV) {
		pr_err("ts pdmic do not support playback!\n");
	} else {
		cdesc->period_callback = ts_pdmic_dma_capture_period_done;
		set_bit(DMA_RX_READY, &ts_pdmic_chip->flags);
	}

	cdesc->period_callback_param = ts_pdmic_chip;

	return 0;
}

static int ts_pdmic_capture_open(struct snd_pcm_substream *substream)
{
	struct ts_pdmic *ts_pdmic_chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;

	mutex_lock(&opened_mutex);
	ts_pdmic_chip->opened++;
	runtime->hw = ts_pdmic_hw;
	if (ts_pdmic_chip->cur_rate) {
		runtime->hw.rate_min = ts_pdmic_chip->cur_rate;
		runtime->hw.rate_max = ts_pdmic_chip->cur_rate;
	}
	if (ts_pdmic_chip->cur_format)
		runtime->hw.formats = pcm_format_to_bits(ts_pdmic_chip->cur_format);
	mutex_unlock(&opened_mutex);
	ts_pdmic_chip->capture_substream = substream;
	return 0;
}

static int ts_pdmic_capture_close(struct snd_pcm_substream *substream)
{
	struct ts_pdmic *ts_pdmic_chip = snd_pcm_substream_chip(substream);

	mutex_lock(&opened_mutex);
	ts_pdmic_chip->opened--;
	if (!ts_pdmic_chip->opened) {
		ts_pdmic_chip->cur_rate = 0;
		ts_pdmic_chip->cur_format = 0;
	}

	mutex_unlock(&opened_mutex);
	ts_pdmic_chip->capture_substream = NULL;

	return 0;
}

static int ts_pdmic_capture_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *hw_params)
{
	int retval;
	struct ts_pdmic *ts_pdmic_chip = snd_pcm_substream_chip(substream);

	retval = snd_pcm_lib_malloc_pages(substream,
					params_buffer_bytes(hw_params));
	if (retval < 0)
		return retval;

	/* snd_pcm_lib_malloc_pages returns 1 if buffer is changed. */
	if (retval == 1)
		if (test_and_clear_bit(DMA_RX_READY, &ts_pdmic_chip->flags))
			dw_dma_cyclic_free(ts_pdmic_chip->dma.rx_chan);

	/* Set restrictions to params. */
	mutex_lock(&opened_mutex);
	ts_pdmic_chip->cur_rate = params_rate(hw_params);
	ts_pdmic_chip->cur_format = params_format(hw_params);
	mutex_unlock(&opened_mutex);

	return retval;
}

static int ts_pdmic_capture_hw_free(struct snd_pcm_substream *substream)
{
	struct ts_pdmic *ts_pdmic_chip = snd_pcm_substream_chip(substream);

	if (test_and_clear_bit(DMA_RX_READY, &ts_pdmic_chip->flags))
		dw_dma_cyclic_free(ts_pdmic_chip->dma.rx_chan);

	return snd_pcm_lib_free_pages(substream);
}

// void ts_pdmic_reg_read(struct ts_pdmic *ts_pdmic_chip)
// {
//	unsigned int val;

//	val = readl(ts_pdmic_chip->regs + PDMIC_CFG);
//	pr_err("[0x%x] = 0x%x\n", PDMIC_CFG, val);

//	val = readl(ts_pdmic_chip->regs + PDMIC_ENABLE);
//	pr_err("[0x%x] = 0x%x\n", PDMIC_ENABLE, val);

//	val = readl(ts_pdmic_chip->regs + PDMIC_FIFO_CFG);
//	pr_err("[0x%x] = 0x%x\n", PDMIC_FIFO_CFG, val);

//	val = readl(ts_pdmic_chip->regs + PDMIC_FIFO_INTEN);
//	pr_err("[0x%x] = 0x%x\n", PDMIC_FIFO_INTEN, val);

//	val = readl(ts_pdmic_chip->regs + PDMIC_FIFO_STAT);
//	pr_err("[0x%x] = 0x%x\n", PDMIC_FIFO_STAT, val);

//	val = readl(ts_pdmic_chip->regs + PDMIC_FIFO_LVL);
//	pr_err("[0x%x] = 0x%x\n", PDMIC_FIFO_LVL, val);
// }

static int ts_pdmic_capture_prepare(struct snd_pcm_substream *substream)
{
	struct ts_pdmic *ts_pdmic_chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	unsigned int rate_min;
	unsigned int rate_max;
	unsigned int cfg_val;
	int retval;

	ts_pdmic_chip->capture_period = 0;

	/*depend on pdm_mclk*/
	rate_min = ts_pdmic_chip->pdata->mic_min_freq >> 6; // 1.024M/64=16k
	rate_max = ts_pdmic_chip->pdata->mic_max_freq >> 6; // 3.072M/64=48k

	if (runtime->channels > 2) {
		dev_err(&ts_pdmic_chip->pdev->dev,
			"max supports two channels\n");
		return -EINVAL;
	}

	if ((runtime->rate < rate_min) || (runtime->rate > rate_max)) {
		dev_err(&ts_pdmic_chip->pdev->dev,
			"sample rate is %dHz, min rate is %dHz, max rate is %dHz\n",
			runtime->rate, rate_min, rate_max);

		return -EINVAL;
	}


	cfg_val = readl(ts_pdmic_chip->regs + PDMIC_CFG);

	/* assign channels to AC97C channel A */
	switch (runtime->channels) {
	case 1:
		cfg_val = (PDMIC_CFG_STEREO_MONO
				<< PDMIC_CFG_STEREO_SHIFT);
		break;
	case 2:
		cfg_val = (PDMIC_CFG_STEREO_STEREO
				<< PDMIC_CFG_STEREO_SHIFT);
		break;
	default:
		/* TODO: support more than two channels */
		return -EINVAL;
	}

	switch (runtime->format) {
	case SNDRV_PCM_FORMAT_S16_LE:
		cfg_val |= (PDMIC_CFG_PCMWD_16BITS
			     << PDMIC_CFG_PCMWD_SHIFT);
		break;
	default:
		return -EINVAL;
	}

	// cfg_val |= (PDMIC_CFG_STOREMODE_NOT_INTERLEAVING
	//			<< PDMIC_CFG_STOREMODE_SHIFT);

	cfg_val |= (PDMIC_CFG_STOREMODE_INTERLEAVING
				<< PDMIC_CFG_STOREMODE_SHIFT); //DMA transfer at 32bit,pdmic need interleaveing to 32bit

	cfg_val |= (PDMIC_CFG_SMPLEDGE_RISE
				<< PDMIC_CFG_SMPLEDGE_SHIFT);

	clk_set_rate(ts_pdmic_chip->pdata->mclk, runtime->rate * 64 * 4); //set pdm_mclk

	writel(cfg_val, ts_pdmic_chip->regs + PDMIC_CFG);

	// ts_pdmic_reg_read(ts_pdmic_chip);

	if (!test_bit(DMA_RX_READY, &ts_pdmic_chip->flags))
		retval = ts_pdmic_prepare_dma(ts_pdmic_chip, substream,
				DMA_DEV_TO_MEM);

	return retval;
}

static int
ts_pdmic_capture_trigger(struct snd_pcm_substream *substream, int cmd)
{
	int retval = 0;
	struct ts_pdmic *ts_pdmic_chip = snd_pcm_substream_chip(substream);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE: /* fall through */
	case SNDRV_PCM_TRIGGER_RESUME: /* fall through */
	case SNDRV_PCM_TRIGGER_START:
		retval = dw_dma_cyclic_start(ts_pdmic_chip->dma.rx_chan);
		if (retval)
			goto out;
		ts_pdmic_rx_enable(ts_pdmic_chip, true);
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH: /* fall through */
	case SNDRV_PCM_TRIGGER_SUSPEND: /* fall through */
	case SNDRV_PCM_TRIGGER_STOP:
		ts_pdmic_rx_enable(ts_pdmic_chip, false);
		dw_dma_cyclic_stop(ts_pdmic_chip->dma.rx_chan);
		break;
	default:
		retval = -EINVAL;
		break;
	}
out:
	return retval;
}

static snd_pcm_uframes_t
ts_pdmic_capture_pointer(struct snd_pcm_substream *substream)
{
	struct ts_pdmic *ts_pdmic_chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime	*runtime = substream->runtime;
	snd_pcm_uframes_t	frames;
	unsigned long		bytes;

	bytes = dw_dma_get_dst_addr(ts_pdmic_chip->dma.rx_chan);
	bytes -= runtime->dma_addr;

	frames = bytes_to_frames(runtime, bytes);
	if (frames >= runtime->buffer_size)
		frames -= runtime->buffer_size;
	return frames;
}

static struct snd_pcm_ops ts_pdmic_capture_ops = {
	.open		= ts_pdmic_capture_open,
	.close		= ts_pdmic_capture_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= ts_pdmic_capture_hw_params,
	.hw_free	= ts_pdmic_capture_hw_free,
	.prepare	= ts_pdmic_capture_prepare,
	.trigger	= ts_pdmic_capture_trigger,
	.pointer	= ts_pdmic_capture_pointer,
};

static irqreturn_t ts_pdmic_interrupt(int irq, void *dev_id)
{
	struct ts_pdmic *ts_pdmic_chip = (struct ts_pdmic *)dev_id;
	unsigned int val;

	val = readl(ts_pdmic_chip->regs + PDMIC_FIFO_STAT);

	//clear int
	writel(val, ts_pdmic_chip->regs + PDMIC_FIFO_STAT);

	return IRQ_NONE;
}

static struct ts_pdmic_pdata *ts_pdmic_probe_dt(struct device *dev)
{
	struct device_node *np = dev->of_node;
	struct ts_pdmic_pdata *pdata;

	if (!np) {
		dev_err(dev, "device node not found\n");
		return ERR_PTR(-EINVAL);
	}

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return ERR_PTR(-ENOMEM);

	if (of_property_read_string(np, "ts,model", &pdata->card_name))
		pdata->card_name = "ts_pdmic";

	pdata->pclk = devm_clk_get(dev, "pdm_pclk");
	if (IS_ERR(pdata->pclk))
		return ERR_PTR(-ENODEV);

	pdata->mclk = devm_clk_get(dev, "pdm_mclk");
	if (IS_ERR(pdata->mclk))
		return ERR_PTR(-ENODEV);

	if (of_property_read_u32(np, "ts,mic-max-freq",
				 &pdata->mic_max_freq)) {
		dev_err(dev, "failed to get mic-max-freq\n");
		return ERR_PTR(-EINVAL);
	}

	if (pdata->mic_max_freq < pdata->mic_min_freq) {
		dev_err(dev,
			"mic-max-freq should not be less than mic-min-freq\n");
		return ERR_PTR(-EINVAL);
	}

	return pdata;
}

static int ts_pdmic_prepare_clk(struct ts_pdmic_pdata *pdata)
{
	int ret;

	ret = clk_prepare_enable(pdata->pclk);
	if (ret)
		return ret;

	ret = clk_prepare_enable(pdata->mclk);
	if (ret)
		return ret;

	return 0;
}

static void ts_pdmic_unprepare_clk(struct ts_pdmic_pdata *pdata)
{
	if (!IS_ERR(pdata->pclk))
		clk_disable_unprepare(pdata->pclk);

	if (!IS_ERR(pdata->mclk))
		clk_disable_unprepare(pdata->mclk);
}

static int ts_pdmic_dma_init(struct ts_pdmic *ts_pdmic_chip)
{
	int ret;
	struct device *dev = &ts_pdmic_chip->pdev->dev;
	struct dma_slave_config	*rx_conf = &ts_pdmic_chip->dma.rx_conf;

	ts_pdmic_chip->dma.rx_chan = dma_request_slave_channel_reason(dev, "rx");
	if (IS_ERR(ts_pdmic_chip->dma.rx_chan))
		return PTR_ERR(ts_pdmic_chip->dma.rx_chan);

	rx_conf->direction = DMA_DEV_TO_MEM;
	rx_conf->device_fc = false;
	rx_conf->src_addr = ts_pdmic_chip->io_phy_base + PDMIC_FIFO_RD;
	rx_conf->src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES,
	rx_conf->src_maxburst = 1;
	rx_conf->dst_maxburst = 1;

	ret = dmaengine_slave_config(ts_pdmic_chip->dma.rx_chan, rx_conf);
	if (ret) {
		dev_err(dev, "failed to configure RX channel\n");
		return -1;
	}

	set_bit(DMA_RX_CHAN_PRESENT, &ts_pdmic_chip->flags);

	return 0;
}

static int ts_pdmic_pcm_new(struct ts_pdmic *ts_pdmic_chip)
{
	struct snd_pcm		*pcm;
	struct snd_pcm_hardware	hw = ts_pdmic_hw;
	int	capture, retval;

	capture = test_bit(DMA_RX_CHAN_PRESENT, &ts_pdmic_chip->flags);

	retval = snd_pcm_new(ts_pdmic_chip->card, ts_pdmic_chip->card->shortname,
			0, 0, capture, &pcm);
	if (retval)
		return retval;

	if (capture)
		snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE,
				&ts_pdmic_capture_ops);

	retval = snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_DEV,
			&ts_pdmic_chip->pdev->dev, hw.periods_max * hw.period_bytes_max,
			hw.buffer_bytes_max);
	if (retval)
		return retval;

	pcm->private_data	= ts_pdmic_chip;
	pcm->info_flags		= 0;
	strcpy(pcm->name, ts_pdmic_chip->card->shortname);
	ts_pdmic_chip->pcm = pcm;

	return 0;
}

static void ts_pdmic_hwparam_init(struct ts_pdmic *ts_pdmic_chip)
{
	unsigned int val;

	val = readl(ts_pdmic_chip->regs + PDMIC_FIFO_CFG);

	val &= ~PDMIC_FIFO_CFG_DMAEN_MASK;
	val |= PDMIC_FIFO_CFG_EN << PDMIC_FIFO_CFG_DMAEN_SHIFT;

	val &= ~PDMIC_FIFO_CFG_TRIGLVL_MASK;
	val |= EIGHT << PDMIC_FIFO_CFG_TRIGLVL_SHIFT;


	writel(val, ts_pdmic_chip->regs + PDMIC_FIFO_CFG);

}

/* regmap configuration */
#define TS_PDMIC_REG_MAX	0x18
static const struct regmap_config ts_pdmic_regmap_config = {
	.reg_bits	= 32,
	.reg_stride	= 4,
	.val_bits	= 32,
	.max_register	= TS_PDMIC_REG_MAX,
};

static int ts_pdmic_probe(struct platform_device *pdev)
{
	struct snd_card	*card;
	struct device *dev = &pdev->dev;
	struct ts_pdmic *ts_pdmic_chip;
	struct ts_pdmic_pdata *pdata;
	struct resource *res;
	struct reset_control *rstc;
	int ret;

	pdata = dev_get_platdata(&pdev->dev);
	if (!pdata) {
		pdata = ts_pdmic_probe_dt(dev);
		if (IS_ERR(pdata))
			return PTR_ERR(pdata);
	}

	ret = ts_pdmic_prepare_clk(pdata);
	if (ret)
		goto get_clk_err;

	rstc = devm_reset_control_get_optional_exclusive(&pdev->dev, "pdm");
	if (IS_ERR(rstc)) {
		ret = PTR_ERR(rstc);
		dev_err(&pdev->dev, "Cannot get pdm reset.\n");
		return -1;
	}
	reset_control_deassert(rstc);

	ret = snd_card_new(&pdev->dev, SNDRV_DEFAULT_IDX1,
					SNDRV_DEFAULT_STR1, THIS_MODULE,
					sizeof(struct ts_pdmic), &card);
	if (ret) {
		dev_dbg(&pdev->dev, "could not create sound card device\n");
		goto err_snd_card_new;
	}

	ts_pdmic_chip = ts_pdmic_get_chip_from_card(card);
	g_ts_pdmic_chip = ts_pdmic_chip;

	ts_pdmic_chip->irq = platform_get_irq(pdev, 0);
	if (ts_pdmic_chip->irq < 0) {
		ret = ts_pdmic_chip->irq;
		dev_err(dev, "failed to get irq: %d\n", ret);
		goto err_snd_card_new;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ts_pdmic_chip->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(ts_pdmic_chip->regs)) {
		ret = PTR_ERR(ts_pdmic_chip->regs);
		dev_err(dev, "failed to remap register memory: %d\n", ret);
		goto err_snd_card_new;
	}

	ts_pdmic_chip->io_phy_base = res->start;

	ts_pdmic_chip->regmap = devm_regmap_init_mmio(dev, ts_pdmic_chip->regs,
					   &ts_pdmic_regmap_config);
	if (IS_ERR(ts_pdmic_chip->regmap)) {
		ret = PTR_ERR(ts_pdmic_chip->regmap);
		dev_err(dev, "failed to init register map: %d\n", ret);
		goto err_snd_card_new;
	}

	ret =  devm_request_irq(dev, ts_pdmic_chip->irq, ts_pdmic_interrupt, 0,
				"ts_pdmic", (void *)ts_pdmic_chip);
	if (ret < 0) {
		dev_err(dev, "can't register ISR for IRQ %u (ret=%i)\n",
			ts_pdmic_chip->irq, ret);
		goto err_snd_card_new;
	}

	spin_lock_init(&ts_pdmic_chip->lock);

	strcpy(card->driver, "ts_pdmic");
	strcpy(card->shortname, "ts_pdmic");
	sprintf(card->longname, "Tsingmicro Codec Control");

	ts_pdmic_chip->card = card;
	ts_pdmic_chip->pdata = pdata;
	ts_pdmic_chip->pdev = pdev;

	ret = ts_pdmic_dma_init(ts_pdmic_chip);
	if (ret) {
		dev_err(dev, "ts_pdmic_dma_init failed with %d\n", ret);
		goto err_snd_card_new;
	}

	ret = ts_pdmic_pcm_new(ts_pdmic_chip);
	if (ret) {
		dev_dbg(&pdev->dev, "could not register ac97 pcm device\n");
		goto err_dma;
	}

	ret = snd_card_register(card);
	if (ret) {
		dev_dbg(&pdev->dev, "could not register sound card\n");
		goto err_dma;
	}

	platform_set_drvdata(pdev, card);

	ts_pdmic_hwparam_init(ts_pdmic_chip);

	return 0;

err_dma:
	if (test_bit(DMA_RX_CHAN_PRESENT, &ts_pdmic_chip->flags))
		dma_release_channel(ts_pdmic_chip->dma.rx_chan);

	clear_bit(DMA_RX_CHAN_PRESENT, &ts_pdmic_chip->flags);
	ts_pdmic_chip->dma.rx_chan = NULL;
	ts_pdmic_chip->dma.tx_chan = NULL;

err_snd_card_new:
	ts_pdmic_unprepare_clk(pdata);
get_clk_err:
	return ret;
}

static int ts_pdmic_remove(struct platform_device *pdev)
{
	struct ts_pdmic_pdata *pdata = dev_get_platdata(&pdev->dev);
	struct snd_card *card = platform_get_drvdata(pdev);
	struct ts_pdmic *ts_pdmic_chip = ts_pdmic_get_chip_from_card(card);

	if (pdata)
		ts_pdmic_unprepare_clk(pdata);

	if (test_bit(DMA_RX_CHAN_PRESENT, &ts_pdmic_chip->flags))
		dma_release_channel(ts_pdmic_chip->dma.rx_chan);

	clear_bit(DMA_RX_CHAN_PRESENT, &ts_pdmic_chip->flags);

	ts_pdmic_chip->dma.rx_chan = NULL;
	ts_pdmic_chip->dma.tx_chan = NULL;

	snd_card_free(card);


	return 0;
}

static const struct of_device_id ts_pdmic_of_match[] = {
	{
		.compatible = "tsingmicro,ts-pdmic",
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, ts_pdmic_of_match);

static struct platform_driver tsingmicro_pdmic_driver = {
	.driver	= {
		.name		= "tsingmicro-pdmic",
		.of_match_table	= of_match_ptr(ts_pdmic_of_match),
	},
	.probe	= ts_pdmic_probe,
	.remove	= ts_pdmic_remove,
};
module_platform_driver(tsingmicro_pdmic_driver);

MODULE_DESCRIPTION("Tsingmicro PDMIC driver under ALSA SoC architecture");
MODULE_AUTHOR("Sheng Yang <yangsheng@tsingmicro.com>");
MODULE_LICENSE("GPL v2");
