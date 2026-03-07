#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/i2c.h>
#include <linux/of_gpio.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>

#include <soc/ts/wrap/platform.h>

#include "../ts_i2s/ts-i2s.h"

#define DAI_NAME_SIZE 32

enum ts_codec_mode {
	TS_CODEC_TX536,
	TS_CODEC_TX5336,
	TS_CODEC_TX5112,
	TS_CODEC_TX5215,
};

struct ts_codec_res {
	enum ts_codec_mode mode;
};

struct ts_es8388_data {
	struct device *dev;
	struct snd_soc_dai_link dai;
	struct snd_soc_card card;
	struct ts_codec_res ts_codec_chip;
	bool is_codec_master;
	char codec_dai_name[DAI_NAME_SIZE];
	char platform_name[DAI_NAME_SIZE];
};

struct codec_master_clk_div_param {
	enum ts_codec_mode mode;
	unsigned long rate;
	unsigned int oclk_freq;
	unsigned int codec_clk_freq;
};

struct codec_slave_clk_div_param {
	enum ts_codec_mode mode;
	unsigned long rate;
	int sample_bits;
	unsigned int channels;
	unsigned int oclk_freq;		//I2S_MCLK out for es8388 work
	unsigned int mclk_freq;		//I2S_SCK
	unsigned int codec_clk_freq;
};

enum ts_codec_mode g_mode;

static const struct codec_slave_clk_div_param codec_slave_div_param[] = {
	/* ch = 2 , bit = 16*/
	/* 8k */
	{TS_CODEC_TX536, 8000, 16, 2, 2049180, 256147, 12288000},  // 8000*16*2=256000
	/* 16k */
	{TS_CODEC_TX536, 16000, 16, 2, 2049180, 512300, 12288000},  // 16000*16*2=512000
	/* 32k */
	{TS_CODEC_TX536, 32000, 16, 2, 4096000, 1024000, 12288000},   // 32000*16*2=1024000
	/* 44.1k */
	{TS_CODEC_TX536, 44100, 16, 2, 5681818, 1420000, 12288000},   // 44100*16*2=1411200
	/* 48k */
	{TS_CODEC_TX536, 48000, 16, 2, 6250000, 1562000, 12288000},   // 48000*16*2=1536000
	/* 64k */
	{TS_CODEC_TX536, 64000, 16, 2, 38333333, 4200000, 12288000},   // 64000*16*2=2048000
	/* 96k */
	{TS_CODEC_TX536, 96000, 16, 2, 12500000, 3125000, 12288000},   // 96000*16*2=3072000

	/* ch = 2 , bit = 32 */
	{TS_CODEC_TX536, 8000, 32, 2, 2049180, 512300, 12000000},  // 8000*32*2=512000
	/* 16k */
	{TS_CODEC_TX536, 16000, 32, 2, 4096000, 1024000, 12000000},  // 16000*32*2=1024000
	/* 32k */
	{TS_CODEC_TX536, 32000, 32, 2, 4096000, 2048000, 12000000},   // 32000*32*2=2048000
	/* 44.1k */
	{TS_CODEC_TX536, 44100, 32, 2, 5681818, 2840910, 12000000},   // 44100*32*2=2822400
	/* 48k */
	{TS_CODEC_TX536, 48000, 32, 2, 6250000, 3125000, 12000000},   // 48000*32*2=3072000
	/* 64k */
	{TS_CODEC_TX536, 64000, 32, 2, 8333333, 4200000, 12000000},   // 64000*32*2=4096000
	/* 96k */
	{TS_CODEC_TX536, 96000, 32, 2, 12500000, 6250000, 12000000},   // 96000*32*2=6144000

	/* ch = 2 , bit = 16*/
	/* 16k */
	{TS_CODEC_TX5112, 16000, 16, 2, 8192000, 512000, 8192000},  // 16000*16*2=512000
	/* 32k */
	{TS_CODEC_TX5112, 32000, 16, 2, 8192000, 1024000, 8192000},   // 32000*16*2=1024000

	/* ch = 2 , bit = 16*/
	/* 16k */
	{TS_CODEC_TX5215, 16000, 16, 2, 8192000, 512000, 8192000},  // 16000*16*2=512000
	/* 32k */
	{TS_CODEC_TX5215, 32000, 16, 2, 8192000, 1024000, 8192000},   // 32000*16*2=1024000

	/* ch = 2 , bit = 16*/
	/* 8k */
	{TS_CODEC_TX5336, 8000, 16, 2, 8192000, 256000, 8192000},  // 8000*16*2=256000
	/* 16k */
	{TS_CODEC_TX5336, 16000, 16, 2, 8192000, 512000, 8192000},  // 16000*16*2=512000
	/* 32k */
	{TS_CODEC_TX5336, 32000, 16, 2, 8192000, 1024000, 8192000},   // 32000*16*2=1024000
	/* 44.1k */
	{TS_CODEC_TX5336, 44100, 16, 2, 5681818, 1411200, 12288000},   // 44100*16*2=1411200
	/* 48k */
	{TS_CODEC_TX5336, 48000, 16, 2, 8192000, 1536000, 12288000},   // 48000*16*2=1536000
	/* 96k */
	{TS_CODEC_TX5336, 96000, 16, 2, 12500000, 3072000, 12288000},   // 96000*16*2=3072000

	/* ch = 2 , bit = 32*/
	/* 8k */
	{TS_CODEC_TX5336, 8000, 32, 2, 8192000, 512000, 12000000},  // 8000*32*2=512000
	/* 16k */
	{TS_CODEC_TX5336, 16000, 32, 2, 8192000, 1024000, 12000000},  // 16000*32*2=1024000
	/* 32k */
	{TS_CODEC_TX5336, 32000, 32, 2, 8192000, 2048000, 12000000},   // 32000*32*2=2048000
	/* 44.1k */
	{TS_CODEC_TX5336, 44100, 32, 2, 5681818, 2822400, 12000000},   // 44100*32*2=2822400
	/* 48k */
	{TS_CODEC_TX5336, 48000, 32, 2, 8192000, 3072000, 12000000},   // 48000*32*2=3072000
	/* 96k */
	{TS_CODEC_TX5336, 96000, 32, 2, 12500000, 6144000, 12000000},   // 96000*32*2=6144000
};

static const struct codec_master_clk_div_param codec_master_div_param[] = {
	{TS_CODEC_TX536, 22050, 11363637, 11289600},
	{TS_CODEC_TX536, 16000, 12500000, 12288000},

	{TS_CODEC_TX5112, 8000, 8192000, 8192000},
	{TS_CODEC_TX5112, 16000, 8192000, 8192000},
	{TS_CODEC_TX5112, 32000, 8192000, 8192000},
};

static int ts_es8388_startup(struct snd_pcm_substream *substream)
{
	return 0;
}

static void ts_es8388_shutdown(struct snd_pcm_substream *substream)
{

}

static int ts_es8388_get_codec_slave_div_param(enum ts_codec_mode mode,
		unsigned long rate,	unsigned int channels, int sample_bits)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(codec_slave_div_param); i++) {
		if (codec_slave_div_param[i].mode == mode &&
			codec_slave_div_param[i].rate == rate &&
			codec_slave_div_param[i].channels == channels &&
			codec_slave_div_param[i].sample_bits == sample_bits)
			return i;
	}
	return -EINVAL;
}

static int ts_es8388_get_codec_master_div_param(enum ts_codec_mode mode,
						unsigned long rate)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(codec_master_div_param); i++) {
		if (codec_master_div_param[i].mode == mode &&
			codec_master_div_param[i].rate == rate)
			return i;
	}
	return -EINVAL;
}

static int ts_es8388_hw_params(struct snd_pcm_substream *substream,
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

	if (channels > 2 || channels == 1)
		channels = 2;

	fmt = rtd->dai_link->dai_fmt;
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:	/* MASTER MODE */
		param_index = ts_es8388_get_codec_master_div_param(g_mode, rate);
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
		param_index = ts_es8388_get_codec_slave_div_param(g_mode, rate,
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

static const struct snd_soc_ops ts_es8388_ops = {
	.startup		= ts_es8388_startup,
	.shutdown		= ts_es8388_shutdown,
	.hw_params		= ts_es8388_hw_params,
};

static int ts_es8388_probe(struct platform_device *pdev)
{
	struct device_node *i2s_np = NULL, *codec_np = NULL;
	struct device_node *node = pdev->dev.of_node;
	struct platform_device *i2s_pdev;
	struct device *dev = &pdev->dev;
	struct ts_es8388_data *data;
	int ret;

	i2s_np = of_parse_phandle(node, "i2s-controller", 0);
	codec_np = of_parse_phandle(node, "audio-codec", 0);
	if (!i2s_np || !codec_np) {
		dev_err(dev, "phandle missing or invalid\n");
		ret = -EINVAL;
		goto fail;
	}

	i2s_pdev = of_find_device_by_node(i2s_np);
	if (!i2s_pdev) {
		dev_err(dev, "failed to find I2S platform device\n");
		ret = -EINVAL;
		goto fail;
	}

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data) {
		ret = -ENOMEM;
		goto put_device;
	}

	if (of_property_read_bool(node, "codec-master"))
		data->is_codec_master = true;


	memcpy(&data->ts_codec_chip, (void *)of_device_get_match_data(&pdev->dev),
			sizeof(struct ts_codec_res));

	g_mode = data->ts_codec_chip.mode;

	data->dev 					= dev;
	data->dai.name 				= "ES8388";
	data->dai.stream_name 		= "ES8388";
	data->dai.codec_dai_name 	= "ES8388 HiFi";
	data->dai.codec_of_node 	= codec_np;
	data->dai.cpu_of_node 		= i2s_np;
	data->dai.platform_of_node 	= i2s_np;
	data->dai.ops				= &ts_es8388_ops;

	if(data->is_codec_master)
		data->dai.dai_fmt 	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
						SND_SOC_DAIFMT_CBM_CFM;
	else
		data->dai.dai_fmt	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
						SND_SOC_DAIFMT_CBS_CFS;

	data->card.dev = dev;
	ret = snd_soc_of_parse_card_name(&data->card, "model");
	if (ret) {
		dev_err(dev, "Unable to parse card name\n");
		goto put_device;
	}

	data->card.num_links	= 1;
	data->card.owner		= THIS_MODULE;
	data->card.dai_link		= &data->dai;
	data->card.card_number  = TS_CODEC_I2S0_CARD_IDX;

	ret = devm_snd_soc_register_card(&pdev->dev, &data->card);
	if (ret) {
		dev_err(dev, "Unable to register: %d\n", ret);
		goto put_device;
	}

	platform_set_drvdata(pdev, data);
put_device:
	put_device(&i2s_pdev->dev);
fail:
	of_node_put(i2s_np);
	of_node_put(codec_np);

	return 0;
}

struct ts_codec_res ts_codec_tx536_res = {
	.mode = TS_CODEC_TX536,
};

struct ts_codec_res ts_codec_tx5112_res = {
	.mode = TS_CODEC_TX5112,
};

struct ts_codec_res ts_codec_tx5215_res = {
	.mode = TS_CODEC_TX5215,
};

struct ts_codec_res ts_codec_tx5336_res = {
	.mode = TS_CODEC_TX5336,
};

static const struct of_device_id ts_es8388_dt_ids[] = {
	{	.compatible = "ts-es8388,tx5112",
		.data = &ts_codec_tx5112_res,
	}, {
		.compatible = "ts-es8388,tx536",
		.data = &ts_codec_tx536_res,
	}, {
		.compatible = "ts-es8388,tx5215",
		.data = &ts_codec_tx5215_res,
	}, {
		.compatible = "ts-es8388,tx5336",
		.data = &ts_codec_tx5336_res,
	}, {
		/* sentinel */
	}
};

MODULE_DEVICE_TABLE(of, ts_es8388_dt_ids);

static struct platform_driver ts_es8388_driver = {
	.driver = {
		.name = "ts-es8388",
		.of_match_table = ts_es8388_dt_ids,
	},
	.probe = ts_es8388_probe,
};

module_platform_driver(ts_es8388_driver);

MODULE_AUTHOR("yangguoyu");
MODULE_DESCRIPTION("Tsingmicro ES8388 ASoC machine driver");
MODULE_LICENSE("GPL v2");
