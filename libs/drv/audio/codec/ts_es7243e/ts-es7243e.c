#include <linux/module.h>
#include <linux/fsl/guts.h>
#include <linux/interrupt.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <sound/soc.h>

#include "../ts_i2s/ts-i2s.h"

struct ts_es7243_data {
	struct snd_soc_dai_link dai;
	struct snd_soc_card card;
	bool is_codec_master;
};

struct codec_master_clk_div_param {
	unsigned long rate;
	unsigned int oclk_freq;
	unsigned int codec_clk_freq; // div from oclk;
};

struct codec_slave_clk_div_param {
	unsigned long rate;
	int sample_bits;
	unsigned int channels;
	unsigned int oclk_freq;
	unsigned int mclk_freq;
	unsigned int codec_clk_freq; // div from oclk;
};

static const struct codec_slave_clk_div_param codec_slave_div_param[] = {
	/* ch = 2 , bit = 16*/
	/* 16k */
	{16000, 16, 2, 8192000, 512000, 8192000},  // 16000*16*2=512000
	/* 32k */
	{32000, 16, 2, 8192000, 1024000, 8192000},   // 32000*16*2=1024000
};

static const struct codec_master_clk_div_param codec_master_div_param[] = {

};

static int ts_es7243_get_codec_slave_div_param(
		unsigned long rate,	unsigned int channels, int sample_bits)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(codec_slave_div_param); i++) {
		if (codec_slave_div_param[i].rate == rate &&
			codec_slave_div_param[i].channels == channels &&
			codec_slave_div_param[i].sample_bits == sample_bits)
			return i;
	}
	return -EINVAL;
}

static int ts_es7243_get_codec_master_div_param( unsigned long rate)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(codec_master_div_param); i++) {
		if (codec_master_div_param[i].rate == rate)
			return i;
	}
	return -EINVAL;
}

static int ts_es7243_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	unsigned long rate = params_rate(params);
	unsigned int channels = params_channels(params);
	int sample_bits = params_width(params);
	unsigned int clk = 0;
	int ret = 0;
	int param_index = -1;
	unsigned int oclk_freq, mclk_freq;
	unsigned int fmt;

	fmt = rtd->dai_link->dai_fmt;
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:	/* MASTER MODE */
		param_index = ts_es7243_get_codec_master_div_param(rate);
		if(param_index < 0) {
			pr_err("codec master cannot configure rate %ld\n", rate);
			return param_index;
		}

		oclk_freq = codec_master_div_param[param_index].oclk_freq;
		ret = snd_soc_dai_set_clkdiv(cpu_dai, TDM_DIV_OCLK, oclk_freq);
		if (ret < 0)
			return ret;

		clk = codec_master_div_param[param_index].codec_clk_freq;
		/* set the codec system clock for DAC and ADC */
		ret = snd_soc_dai_set_sysclk(codec_dai, 0, clk,
				SND_SOC_CLOCK_OUT);
		if (ret < 0)
			return ret;

		break;
	case SND_SOC_DAIFMT_CBS_CFS:	/* SLAVE MODE */
		param_index = ts_es7243_get_codec_slave_div_param(rate,
								channels, sample_bits);
		if(param_index < 0) {
			pr_err("codec slave cannot configure rate %ld, "
					"sample bits %d, channels = %d\n",
					rate, sample_bits, channels);
			return param_index;
		}

		oclk_freq = codec_slave_div_param[param_index].oclk_freq;
		ret = snd_soc_dai_set_clkdiv(cpu_dai, TDM_DIV_OCLK, oclk_freq);
		if (ret < 0)
			return ret;

		mclk_freq = codec_slave_div_param[param_index].mclk_freq;
		ret = snd_soc_dai_set_clkdiv(cpu_dai, TDM_DIV_MCLK, mclk_freq);
		if (ret < 0)
			return ret;

		clk = codec_slave_div_param[param_index].codec_clk_freq;
		/* set the codec system clock for DAC and ADC */
		ret = snd_soc_dai_set_sysclk(codec_dai, 0, clk,
				SND_SOC_CLOCK_OUT);
		if (ret < 0)
			return ret;

		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static const struct snd_soc_ops ts_es7243_ops = {
	.hw_params		= ts_es7243_hw_params,
};

static int ts_es7243_probe(struct platform_device *pdev)
{
	struct device *dev = pdev->dev.parent;
	struct ts_es7243_data *mdata;
	struct device_node *i2s_np = NULL;
	struct device_node *codec_np_capture = NULL;
	struct platform_device *i2s_pdev;
	int ret;

	i2s_np = of_parse_phandle(pdev->dev.of_node, "i2s-controller", 0);

	i2s_pdev = of_find_device_by_node(i2s_np);
	if (!i2s_pdev) {
		dev_err(dev, "failed to find I2S platform device\n");
		ret = -EINVAL;
		goto fail_codec_playback;
	}

	codec_np_capture = of_parse_phandle(pdev->dev.of_node,
									"audio-codec-capture", 0);
	if (!codec_np_capture) {
		dev_err(dev, "phandle missing or invalid\n");
		ret = -EINVAL;
		goto fail_codec_capture;
	}

	mdata = devm_kzalloc(dev, sizeof(struct ts_es7243_data), GFP_KERNEL);
	if (!mdata) {
		ret = -ENOMEM;
		goto put_device;
	}

	/* ASoC core can match codec with device node */
	mdata->dai.codec_of_node = codec_np_capture;

	mdata->dai.cpu_of_node		= i2s_np;
	mdata->dai.platform_of_node	= i2s_np;

	mdata->dai.ops				= &ts_es7243_ops;

	/* The DAI names from the codec (snd_soc_dai_driver.name) */
	mdata->dai.codec_dai_name = "ES7243E HiFi 0";

	if(mdata->is_codec_master)
		mdata->dai.dai_fmt 	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
						SND_SOC_DAIFMT_CBM_CFM;
	else
		mdata->dai.dai_fmt	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
						SND_SOC_DAIFMT_CBS_CFS;

	/* Initialize our DAI data structure.  */
	mdata->dai.stream_name = "capture";
	mdata->dai.name = mdata->dai.stream_name;

	mdata->card.owner = THIS_MODULE;
	mdata->card.dev = &pdev->dev;
	mdata->card.num_links = 1;
	mdata->card.dai_link = &mdata->dai;

	ret = snd_soc_of_parse_card_name(&mdata->card, "model");
	if (ret) {
		dev_err(dev, "Unable to parse card name\n");
		mdata->card.name = pdev->name; /* The platform driver name */
	}

	/* Register with ASoC */
	ret = devm_snd_soc_register_card(&pdev->dev, &mdata->card);
	if (ret) {
		dev_err(&pdev->dev, "could not register card (ret=%i)\n", ret);
		goto put_device;
	}

	return 0;

put_device:
	put_device(&i2s_pdev->dev);

fail_codec_capture:
	of_node_put(codec_np_capture);

fail_codec_playback:
	of_node_put(i2s_np);

	return ret;
}

static int ts_es7243_remove(struct platform_device *pdev)
{
	struct snd_soc_card *card = platform_get_drvdata(pdev);
	struct ts_es7243_data *mdata =
		container_of(card, struct ts_es7243_data, card);


	snd_soc_unregister_card(card);

	kfree(mdata);

	return 0;
}

static const struct of_device_id ts_snd_matches[] = {
	{ .compatible = "ts,es7243e" },
	{ }
};

MODULE_DEVICE_TABLE(of, ts_snd_matches);

static struct platform_driver ts_es7243_driver = {
	.driver		= {
		.name	= "ts-es7243e",
		.of_match_table = of_match_ptr(ts_snd_matches),
	},
	.probe		= ts_es7243_probe,
	.remove		= ts_es7243_remove,
};

module_platform_driver(ts_es7243_driver);
MODULE_LICENSE("GPL");

