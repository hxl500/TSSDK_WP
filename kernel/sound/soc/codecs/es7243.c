/*
 * ALSA SoC ES7243l adc driver
 *
 * Author:		David Yang, <yangxiaohua@everest-semi.com>
 *		or
 *		<info@everest-semi.com>
 * Copyright:	(C) 2017 Everest Semiconductor Co Ltd.,
 *
 * Based on sound/soc/codecs/es7243.c by DavidYang
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Notes:
 *	ES7243l is a stereo Audio ADC for Microphone Array
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/tlv.h>
#include <sound/initval.h>
#include <linux/regmap.h>

#include "es7243.h"

#define AVDD_3V3 0

static struct snd_soc_codec *es7243l_codec;
struct i2c_client *i2c_clt[(ES7243l_CHANNELS_MAX) / 2];

/* codec private data */
struct es7243l_priv {
	struct regmap *regmap;
	struct i2c_client *i2c;
	unsigned int sysclk;
	struct snd_pcm_hw_constraint_list *sysclk_constraints;
	bool dmic;
	u8 mclksrc;
	bool mclkinv;
	bool bclkinv;
	u8 tdm;
	u8 vdda;
};
struct snd_soc_codec *tron_codec[8];
int es7243l_init_reg = 0;
static int es7243l_codec_num = 0;

static const struct regmap_config es7243l_regmap_config = {
	.reg_bits = 8,		//Number of bits in a register address
	.val_bits = 8,		//Number of bits in a register value
	.max_register	= 0xff,
};

/*
* ES7243 register cache
*/
static const u8 es7243_reg[] = {
	0x00, 0x00, 0x10, 0x04,	/* 0 */
	0x02, 0x13, 0x00, 0x3f,	/* 4 */
	0x11, 0x00, 0xc0, 0xc0,	/* 8 */
	0x12, 0xa0, 0x40,	/* 12 */
};

static const struct reg_default es7243_reg_defaults[] = {
	{0x00, 0x00}, {0x01, 0x00}, {0x02, 0x10}, {0x03, 0x04},	/* 0 */
	{0x04, 0x02}, {0x05, 0x13}, {0x06, 0x00}, {0x07, 0x3f},	/* 4 */
	{0x08, 0x11}, {0x09, 0x00}, {0x0a, 0xc0}, {0x0b, 0xc0},	/* 8 */
	{0x0c, 0x12}, {0x0d, 0xa0}, {0x0e, 0x40},	/* 12 */
};

static int es7243l_read(u8 reg, u8 * rt_value, struct i2c_client *client)
{
	int ret;
	u8 read_cmd[3] = { 0 };
	u8 cmd_len = 0;

	read_cmd[0] = reg;
	cmd_len = 1;

	if (client->adapter == NULL)
		pr_err("es7243_read client->adapter==NULL\n");

	ret = i2c_master_send(client, read_cmd, cmd_len);
	if (ret != cmd_len) {
		pr_err("es7243_read error1, ret = %d\n", ret);
		return -1;
	}

	ret = i2c_master_recv(client, rt_value, 1);
	if (ret != 1) {
		pr_err("es7243_read error2, ret = %d.\n", ret);
		return -1;
	}

	return 0;
}

static int
es7243l_write(u8 reg, unsigned char value, struct i2c_client *client)
{
	int ret = 0;
	u8 write_cmd[2] = { 0 };

	write_cmd[0] = reg;
	write_cmd[1] = value;

	ret = i2c_master_send(client, write_cmd, 2);
	if (ret != 2) {
		pr_err("es7243_write error->[REG-0x%02x,val-0x%02x]\n",
			   reg, value);
		return -1;
	}

	return 0;
}

static int
es7243l_update_bits(u8 reg, u8 mask, u8 value, struct i2c_client *client)
{
	u8 val_old, val_new;

	es7243l_read(reg, &val_old, client);
	val_new = (val_old & ~mask) | (value & mask);
	if (val_new != val_old) {
		es7243l_write(reg, val_new, client);
	}

	return 0;
}

struct _coeff_div {
	u32 mclk;		//mclk frequency
	u32 sr_rate;		//sample rate
	u8 osr;			//adc over sample rate
	u8 prediv_premulti;	//adcclk and dacclk divider
	u8 cf_dsp_div;		//adclrck divider and daclrck divider
	u8 scale;
	u8 lrckdiv_h;
	u8 lrckdiv_l;
	u8 bclkdiv;		//sclk divider
};

/* codec hifi mclk clock divider coefficients */
static const struct _coeff_div coeff_div[] = {
	//mclk	   lrck,  osr, pre, div  ,scale, lrckdiv_h, lrckdiv_l, bclkdiv
	/* 24.576MHZ */
	{24576000, 8000, 0x20, 0x50, 0x00, 0x00, 0x0b, 0xff, 0x2f},
	{24576000, 12000, 0x20, 0x30, 0x00, 0x00, 0x07, 0xff, 0x1f},
	{24576000, 16000, 0x20, 0x20, 0x00, 0x00, 0x05, 0xff, 0x17},
	{24576000, 24000, 0x20, 0x10, 0x00, 0x00, 0x03, 0xff, 0x0f},
	{24576000, 32000, 0x20, 0x21, 0x00, 0x00, 0x02, 0xff, 0x0b},
	{24576000, 48000, 0x20, 0x00, 0x00, 0x00, 0x01, 0xff, 0x07},
	/* 12.288MHZ */
	{12288000, 8000, 0x20, 0x20, 0x00, 0x00, 0x05, 0xff, 0x17},
	{12288000, 12000, 0x20, 0x10, 0x00, 0x00, 0x03, 0xff, 0x0f},
	{12288000, 16000, 0x20, 0x21, 0x00, 0x00, 0x02, 0xff, 0x0b},
	{12288000, 24000, 0x20, 0x00, 0x00, 0x00, 0x01, 0xff, 0x07},
	{12288000, 32000, 0x20, 0x22, 0x00, 0x00, 0x01, 0x7f, 0x05},
	{12288000, 48000, 0x20, 0x01, 0x00, 0x00, 0x00, 0xff, 0x03},
	/* 6.144MHZ */
	{6144000, 8000, 0x20, 0x21, 0x00, 0x00, 0x02, 0xff, 0x0b},
	{6144000, 12000, 0x20, 0x00, 0x00, 0x00, 0x01, 0xff, 0x07},
	{6144000, 16000, 0x20, 0x22, 0x00, 0x00, 0x01, 0x7f, 0x05},
	{6144000, 24000, 0x20, 0x01, 0x00, 0x00, 0x00, 0xff, 0x03},
	{6144000, 32000, 0x20, 0x23, 0x00, 0x00, 0x00, 0xbf, 0x02},
	{6144000, 48000, 0x20, 0x02, 0x00, 0x00, 0x00, 0x7f, 0x01},
	/* 3.072MHZ */
	{3072000, 8000, 0x20, 0x22, 0x00, 0x00, 0x01, 0x7f, 0x05},
	{3072000, 12000, 0x20, 0x01, 0x00, 0x00, 0x00, 0xff, 0x03},
	{3072000, 16000, 0x20, 0x23, 0x00, 0x00, 0x00, 0xbf, 0x02},
	{3072000, 24000, 0x20, 0x02, 0x00, 0x00, 0x00, 0x7f, 0x01},
	{3072000, 32000, 0x10, 0x03, 0x20, 0x04, 0x00, 0x5f, 0x02},
	{3072000, 48000, 0x20, 0x03, 0x00, 0x00, 0x00, 0x3f, 0x00},
	/* 1.536MHZ */
	{1536000, 8000, 0x20, 0x23, 0x00, 0x00, 0x00, 0xbf, 0x02},
	{1536000, 12000, 0x20, 0x02, 0x00, 0x00, 0x00, 0x7f, 0x01},
	{1536000, 16000, 0x10, 0x03, 0x20, 0x04, 0x00, 0x5f, 0x00},
	{1536000, 24000, 0x20, 0x03, 0x00, 0x00, 0x00, 0x3f, 0x00},
	/* 32.768MHZ */
	{32768000, 8000, 0x20, 0x70, 0x00, 0x00, 0x0f, 0xff, 0x3f},
	{32768000, 16000, 0x20, 0x30, 0x00, 0x00, 0x07, 0xff, 0x1f},
	{32768000, 32000, 0x20, 0x10, 0x00, 0x00, 0x03, 0xff, 0x0f},
	/* 16.384MHZ */
	{16384000, 8000, 0x20, 0x30, 0x00, 0x00, 0x07, 0xff, 0x1f},
	{16384000, 16000, 0x20, 0x10, 0x00, 0x00, 0x03, 0xff, 0x0f},
	{16384000, 32000, 0x20, 0x00, 0x00, 0x00, 0x01, 0xff, 0x07},
	/* 8.192MHZ */
	{8192000, 8000, 0x20, 0x10, 0x00, 0x00, 0x03, 0xff, 0x0f},
	{8192000, 16000, 0x20, 0x00, 0x00, 0x00, 0x01, 0xff, 0x07},
	{8192000, 32000, 0x20, 0x01, 0x00, 0x00, 0x00, 0xff, 0x03},
	/* 4.096MHZ */
	{4096000, 8000, 0x20, 0x00, 0x00, 0x00, 0x01, 0xff, 0x07},
	{4096000, 16000, 0x20, 0x01, 0x00, 0x00, 0x00, 0xff, 0x03},
	{4096000, 32000, 0x20, 0x02, 0x00, 0x00, 0x00, 0x7f, 0x01},
	/* 2.048MHZ */
	{2048000, 8000, 0x20, 0x01, 0x00, 0x00, 0x00, 0xff, 0x03},
	{2048000, 16000, 0x20, 0x02, 0x00, 0x00, 0x00, 0x7f, 0x01},
	{2048000, 32000, 0x20, 0x03, 0x00, 0x00, 0x00, 0x3f, 0x00},
	/* 1.024MHZ */
	{1024000, 8000, 0x20, 0x02, 0x00, 0x00, 0x00, 0x7f, 0x01},
	{1024000, 16000, 0x20, 0x03, 0x00, 0x00, 0x00, 0x3f, 0x00},
	/* 22.5792MHz */
	{22579200, 11025, 0x20, 0x30, 0x00, 0x00, 0x07, 0xff, 0x1f},
	{22579200, 22050, 0x20, 0x10, 0x00, 0x00, 0x03, 0xff, 0x0f},
	{22579200, 44100, 0x20, 0x00, 0x00, 0x00, 0x01, 0xff, 0x07},
	/* 11.2896MHz */
	{11289600, 11025, 0x20, 0x10, 0x00, 0x00, 0x03, 0xff, 0x0f},
	{11289600, 22050, 0x20, 0x00, 0x00, 0x00, 0x01, 0xff, 0x07},
	{11289600, 44100, 0x20, 0x01, 0x00, 0x00, 0x00, 0xff, 0x03},
	/* 5.6448MHz */
	{56448000, 11025, 0x20, 0x00, 0x00, 0x00, 0x01, 0xff, 0x07},
	{56448000, 22050, 0x20, 0x01, 0x00, 0x00, 0x00, 0xff, 0x03},
	{56448000, 44100, 0x20, 0x02, 0x00, 0x00, 0x00, 0x7f, 0x01},
	/* 2.8224MHz */
	{28224000, 11025, 0x20, 0x01, 0x00, 0x00, 0x00, 0xff, 0x03},
	{28224000, 22050, 0x20, 0x02, 0x00, 0x00, 0x00, 0x7f, 0x01},
	{28224000, 44100, 0x20, 0x03, 0x00, 0x00, 0x00, 0x3f, 0x00},
	/* 1.4112MHz */
	{14112000, 11025, 0x20, 0x02, 0x00, 0x00, 0x00, 0x7f, 0x01},
	{14112000, 22050, 0x20, 0x03, 0x00, 0x00, 0x00, 0x3f, 0x00},
};

static inline int get_coeff(int mclk, int rate)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(coeff_div); i++) {
		if (coeff_div[i].sr_rate == rate && coeff_div[i].mclk == mclk)
			return i;
	}

	return -EINVAL;
}

/* The set of rates we can generate from the above for each SYSCLK */

static unsigned int rates_12288[] = {
	8000, 12000, 16000, 24000, 32000, 48000, 64000, 96000, 128000, 192000,
};

static unsigned int rates_8192[] = {
	8000, 16000, 32000, 64000, 128000,
};

static struct snd_pcm_hw_constraint_list constraints_12288 = {
	.count = ARRAY_SIZE(rates_12288),
	.list = rates_12288,
};

static struct snd_pcm_hw_constraint_list constraints_8192 = {
	.count = ARRAY_SIZE(rates_8192),
	.list = rates_8192,
};

static unsigned int rates_112896[] = {
	8000, 11025, 22050, 44100,
};

static struct snd_pcm_hw_constraint_list constraints_112896 = {
	.count = ARRAY_SIZE(rates_112896),
	.list = rates_112896,
};

#if 0
static unsigned int rates_12[] = {
	8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000,
	48000, 88235, 96000,
};

static struct snd_pcm_hw_constraint_list constraints_12 = {
	.count = ARRAY_SIZE(rates_12),
	.list = rates_12,
};
#endif
/*
* Note that this should be called from init rather than from hw_params.
*/
static int
es7243l_set_dai_sysclk(struct snd_soc_dai *codec_dai,
			   int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct es7243l_priv *es7243l = snd_soc_codec_get_drvdata(codec);
	pr_err("Enter into %s(), freq = %d\n", __func__, freq);
	switch (freq) {
	case 11289600:
	case 22579200:
		es7243l->sysclk_constraints = &constraints_112896;
		es7243l->sysclk = freq;
		return 0;
	case 12288000:
	case 24576000:
		es7243l->sysclk_constraints = &constraints_12288;
		es7243l->sysclk = freq;
		return 0;
	case 4096000:
	case 8192000:
		es7243l->sysclk_constraints = &constraints_8192;
		es7243l->sysclk = freq;
		return 0;
/*
	case 12000000:
	case 24000000:
		es7243l->sysclk_constraints = &constraints_12;
		es7243l->sysclk = freq;
		return 0;
*/
	}
//		return -EINVAL;
	return 0;
}

static int es7243l_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
#if 0
	u8 iface = 0;
	u8 adciface = 0;
	u8 clksel = 0;
	u8 i;
	pr_err("Enter into %s()\n", __func__);
	for (i = 0; i < (ES7243l_CHANNELS_MAX) / 2; i++) {
		es7243l_read(0x0b, &adciface, i2c_clt[i]);	//get interface format
		es7243l_read(0x00, &iface, i2c_clt[i]);	//get spd interface
		es7243l_read(0x02, &clksel, i2c_clt[i]);	//get spd interface

		/* set master/slave audio interface */
		switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
		case SND_SOC_DAIFMT_CBM_CFM:	// MASTER MODE
			iface |= 0x40;
			break;
		case SND_SOC_DAIFMT_CBS_CFS:	// SLAVE MODE
			iface &= 0xbf;
			break;
		default:
			return -EINVAL;
		}
		/* interface format */
		switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
		case SND_SOC_DAIFMT_I2S:
			adciface &= 0xFC;
			break;
		case SND_SOC_DAIFMT_RIGHT_J:
			return -EINVAL;
		case SND_SOC_DAIFMT_LEFT_J:
			adciface &= 0xFC;
			adciface |= 0x01;
			break;
		case SND_SOC_DAIFMT_DSP_A:
			adciface &= 0xDC;
			adciface |= 0x03;
			break;
		case SND_SOC_DAIFMT_DSP_B:
			adciface &= 0xDC;
			adciface |= 0x23;
			break;
		default:
			return -EINVAL;
		}

		/* clock inversion */
		switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
		case SND_SOC_DAIFMT_NB_NF:
			adciface &= 0xdf;
			clksel &= 0xfe;
			break;
		case SND_SOC_DAIFMT_IB_IF:
			adciface |= 0x20;
			clksel |= 0x01;
			break;
		case SND_SOC_DAIFMT_IB_NF:
			adciface &= 0xdf;
			clksel |= 0x01;
			break;
		case SND_SOC_DAIFMT_NB_IF:
			adciface |= 0x20;
			clksel &= 0xfe;
			break;
		default:
			return -EINVAL;
		}

		es7243l_write(0x00, iface, i2c_clt[i]);
		es7243l_write(0x02, clksel, i2c_clt[i]);
		es7243l_write(0x0b, adciface, i2c_clt[i]);
	}
#endif
	return 0;
}

static int
es7243l_pcm_startup(struct snd_pcm_substream *substream,
			struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct es7243l_priv *es7243l = snd_soc_codec_get_drvdata(codec);

	return 0;

	pr_err("%s **** %d **** \n", __func__, __LINE__);
	/* The set of sample rates that can be supported depends on the
	 * MCLK supplied to the CODEC - enforce this.
	 */
	if (!es7243l->sysclk) {
		dev_err(codec->dev,
			"No MCLK configured, call set_sysclk() on init\n");
		return -EINVAL;
	}

	snd_pcm_hw_constraint_list(substream->runtime, 0,
					SNDRV_PCM_HW_PARAM_RATE, es7243l->sysclk_constraints);

	return 0;
}

static int
es7243l_pcm_hw_params(struct snd_pcm_substream *substream,
			  struct snd_pcm_hw_params *params,
			  struct snd_soc_dai *dai)
{
//		struct snd_soc_pcm_runtime *rtd = substream->private_data;
//		struct snd_soc_codec *codec = rtd->codec;
//		struct es7243l_priv *es7243l = snd_soc_codec_get_drvdata(codec);
	u8 index, regv;
//		int coeff;
	pr_err("%s **** %d **** \n", __func__, __LINE__);
#if 0
	coeff = get_coeff(es7243l->sysclk, params_rate(params));
	if (coeff < 0) {
		pr_err("Unable to configure sample rate %dHz with %dHz MCLK",
			   params_rate(params), es7243l->sysclk);
		return coeff;
	}
	/*
	 * set clock parameters
	 */
	if (coeff >= 0) {
		for (index = 0; index < (ES7243l_CHANNELS_MAX) / 2; index++) {
			es7243l_write(0x03, coeff_div[coeff].osr,
					  i2c_clt[index]);
			es7243l_write(0x04,
					  coeff_div[coeff].prediv_premulti,
					  i2c_clt[index]);
			es7243l_write(0x05, coeff_div[coeff].cf_dsp_div,
					  i2c_clt[index]);
			es7243l_write(0x0d, coeff_div[coeff].scale,
					  i2c_clt[index]);
			es7243l_write(0x03, coeff_div[coeff].osr,
					  i2c_clt[index]);

			es7243l_read(0x07, &regv, i2c_clt[index]);
			regv &= 0xf0;
			regv |= (coeff_div[coeff].lrckdiv_h & 0x0f);
			es7243l_write(0x07, regv, i2c_clt[index]);
			es7243l_write(0x06, coeff_div[coeff].bclkdiv,
					  i2c_clt[index]);

		}
	}
#endif
	/*
	 * set data length
	 */
	for (index = 0; index < (ES7243l_CHANNELS_MAX) / 2; index++) {
		es7243l_read(0x0b, &regv, i2c_clt[index]);
		regv &= 0xe3;
		switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_S16_LE:
			regv |= 0x0c;
			break;
		case SNDRV_PCM_FORMAT_S20_3LE:
			regv |= 0x04;
			break;
		case SNDRV_PCM_FORMAT_S24_LE:
			break;
		case SNDRV_PCM_FORMAT_S32_LE:
			regv |= 0x10;
			break;
		default:
			regv |= 0x0c;
			break;
		}
		es7243l_write(0x0b, regv, i2c_clt[index]);
	}
	msleep(50);
	for (index = 0; index < (ES7243l_CHANNELS_MAX) / 2; index++) {
		es7243l_read(0x0b, &regv, i2c_clt[index]);
		regv &= 0x3f;
		es7243l_write(0x0b, regv, i2c_clt[index]);
	}
	return 0;
}

static int es7243l_mute(struct snd_soc_dai *dai, int mute)
{
	//struct snd_soc_codec *codec = dai->codec;
	u8 i;
	pr_err("%s **** %d **** \n", __func__, __LINE__);
	for (i = 0; i < (ES7243l_CHANNELS_MAX) / 2; i++) {
		if (mute)
			es7243l_update_bits(0x0b, 0xc0, 0xc0, i2c_clt[i]);
		else
			es7243l_update_bits(0x0b, 0xc0, 0x00, i2c_clt[i]);
	}
	return 0;
}

static int
es7243l_set_bias_level(struct snd_soc_codec *codec,
			   enum snd_soc_bias_level level)
{
	u8 i, regv;
	switch (level) {
	case SND_SOC_BIAS_ON:
		pr_err("%s on\n", __func__);
		break;
	case SND_SOC_BIAS_PREPARE:
		pr_err("%s prepare\n", __func__);
		break;
	case SND_SOC_BIAS_STANDBY:
		pr_err("%s standby\n", __func__);
		for (i = 0; i < (ES7243l_CHANNELS_MAX) / 2; i++) {
			/*
			 * enable clock
			 */
			es7243l_read(0x01, &regv, i2c_clt[i]);
			regv |= 0x0A;
			es7243l_write(0x01, regv, i2c_clt[i]);
			es7243l_write(0x16, 0x00, i2c_clt[i]);	//power up analog
			/*
			 * enable mic input 1
			 */
			es7243l_read(0x20, &regv, i2c_clt[i]);
			regv |= 0x10;
			es7243l_write(0x20, regv, i2c_clt[i]);
			/*
			 * enable mic input 2
			 */
			es7243l_read(0x21, &regv, i2c_clt[i]);
			regv |= 0x10;
			es7243l_write(0x21, regv, i2c_clt[i]);
			msleep(100);
			es7243l_update_bits(0x0b, 0xc0, 0x00, i2c_clt[i]);	//mute SDP
		}
		break;
	case SND_SOC_BIAS_OFF:
		pr_err("%s off\n", __func__);
		for (i = 0; i < (ES7243l_CHANNELS_MAX) / 2; i++) {
			es7243l_update_bits(0x0b, 0xc0, 0xc0, i2c_clt[i]);	//mute SDP
			/*
			 * disable mic input 1
			 */
			es7243l_read(0x20, &regv, i2c_clt[i]);
			regv &= 0xef;
			es7243l_write(0x20, regv, i2c_clt[i]);
			/*
			 * disable mic input 2
			 */
			es7243l_read(0x21, &regv, i2c_clt[i]);
			regv &= 0xef;
			es7243l_write(0x21, regv, i2c_clt[i]);

			es7243l_write(0x16, 0xff, i2c_clt[i]);	//power down analog

			/*
			 * disable clock
			 */
			es7243l_read(0x01, &regv, i2c_clt[i]);
			regv &= 0xf5;
			es7243l_write(0x01, regv, i2c_clt[i]);
		}
		break;
	}
	//codec->dapm.bias_level = level;
	return 0;
}

/*
* snd_controls for PGA gain, Mute, suspend and resume
*/
static const DECLARE_TLV_DB_SCALE(mic_boost_tlv, 0, 300, 0);

#if ES7243l_CHANNELS_MAX > 0
static int
es7243l_micboost1_setting_set(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_value *ucontrol)
{
	es7243l_update_bits(0x20, 0x0F,
				ucontrol->value.integer.value[0] & 0x0f,
				i2c_clt[0]);
	return 0;
}

static int
es7243l_micboost1_setting_get(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_value *ucontrol)
{
	u8 val;
	es7243l_read(0x20, &val, i2c_clt[0]);
	ucontrol->value.integer.value[0] = val & 0x0f;
	return 0;
}

static int
es7243l_micboost2_setting_set(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_value *ucontrol)
{
	es7243l_update_bits(0x21, 0x0F,
				ucontrol->value.integer.value[0] & 0x0f,
				i2c_clt[0]);
	return 0;
}

static int
es7243l_micboost2_setting_get(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_value *ucontrol)
{
	u8 val;
	es7243l_read(0x21, &val, i2c_clt[0]);
	ucontrol->value.integer.value[0] = val & 0x0f;
	return 0;
}

static int
es7243l_adc1_mute_set(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	es7243l_update_bits(0x0B, 0x40,
				(ucontrol->value.integer.value[0] & 0x01) << 6,
				i2c_clt[0]);
	return 0;
}

static int
es7243l_adc1_mute_get(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	u8 val;
	es7243l_read(0x0B, &val, i2c_clt[0]);
	ucontrol->value.integer.value[0] = (val & 0x40) >> 6;
	return 0;
}

static int
es7243l_adc2_mute_set(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	es7243l_update_bits(0x0B, 0x80,
				(ucontrol->value.integer.value[0] & 0x01) << 7,
				i2c_clt[0]);
	return 0;
}

static int
es7243l_adc2_mute_get(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	u8 val;
	es7243l_read(0x0B, &val, i2c_clt[0]);
	ucontrol->value.integer.value[0] = (val & 0x80) >> 7;
	return 0;
}

static int
es7243l_adc1adc2_suspend_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	u8 val;
	es7243l_read(0x17, &val, i2c_clt[0]);
	ucontrol->value.integer.value[0] = (val & 0x02) >> 2;
	return 0;
}

static int
es7243l_adc1adc2_suspend_set(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	//u8 val;
	if ((ucontrol->value.integer.value[0] & 0x01) == 1) {	//suspend
		es7243l_write(0x04, 0x02, i2c_clt[0]);
		es7243l_write(0x04, 0x01, i2c_clt[0]);
		es7243l_write(0xf7, 0x30, i2c_clt[0]);
		es7243l_write(0xf9, 0x01, i2c_clt[0]);
		es7243l_write(0x16, 0xff, i2c_clt[0]);
		es7243l_write(0x17, 0x00, i2c_clt[0]);
		es7243l_write(0x01, 0x38, i2c_clt[0]);
		es7243l_update_bits(0x20, 0x10, 0x00, i2c_clt[0]);
		es7243l_update_bits(0x21, 0x10, 0x00, i2c_clt[0]);
		es7243l_write(0x00, 0x8e, i2c_clt[0]);
		es7243l_write(0x01, 0x30, i2c_clt[0]);
	} else {		//resume
		es7243l_write(0x01, 0x3a, i2c_clt[0]);
		es7243l_write(0x00, 0x80, i2c_clt[0]);
		es7243l_update_bits(0x20, 0x10, 0x10, i2c_clt[0]);
		es7243l_update_bits(0x21, 0x10, 0x10, i2c_clt[0]);
		es7243l_write(0x16, 0x00, i2c_clt[0]);
		es7243l_write(0x17, 0x02, i2c_clt[0]);
	}
	return 0;
}
#endif

static const struct snd_kcontrol_new es7243l_snd_controls[] = {
#if ES7243l_CHANNELS_MAX > 0
	SOC_SINGLE_EXT_TLV("PGA1_setting", 0x20, 0, 0x0F, 0,
			   es7243l_micboost1_setting_get,
			   es7243l_micboost1_setting_set,
			   mic_boost_tlv),
	SOC_SINGLE_EXT_TLV("PGA2_setting", 0x21, 0, 0x0F, 0,
			   es7243l_micboost2_setting_get,
			   es7243l_micboost2_setting_set,
			   mic_boost_tlv),
	SOC_SINGLE_EXT("ADC1_MUTE", 0x0B, 1, 0x40, 0,
			   es7243l_adc1_mute_get, es7243l_adc1_mute_set),
	SOC_SINGLE_EXT("ADC2_MUTE", 0x0B, 1, 0x80, 0,
			   es7243l_adc2_mute_get, es7243l_adc2_mute_set),
	SOC_SINGLE_EXT("ADC1ADC2_SUSPEND", 0x17, 1, 1, 0,
			   es7243l_adc1adc2_suspend_get,
			   es7243l_adc1adc2_suspend_set),
#endif
};

#define es7243l_RATES SNDRV_PCM_RATE_8000_48000

#define es7243l_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
	SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE)

static struct snd_soc_dai_ops es7243l_ops = {
	.startup = es7243l_pcm_startup,
	.hw_params = es7243l_pcm_hw_params,
	.set_fmt = es7243l_set_dai_fmt,
	.set_sysclk = es7243l_set_dai_sysclk,
	.digital_mute = es7243l_mute,
};

#if ES7243l_CHANNELS_MAX > 0
static struct snd_soc_dai_driver es7243l_dai0 = {
	.name = "ES7243l HiFi 0",
	.capture = {
			.stream_name = "Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = es7243l_RATES,
			.formats = es7243l_FORMATS,
			},
	.ops = &es7243l_ops,
	.symmetric_rates = 1,
};
#endif

static struct snd_soc_dai_driver *es7243l_dai[] = {
#if ES7243l_CHANNELS_MAX > 0
	&es7243l_dai0,
#endif
};

static int es7243l_suspend(struct snd_soc_codec *codec)
{
	es7243l_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int es7243l_resume(struct snd_soc_codec *codec)
{
	es7243l_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	return 0;
}

struct _mclk_lrck_ratio {
	u16 ratio;		//ratio between mclk and lrck
	u8 nfs;			//nfs mode, =0, nfs mode disabled
	u8 osr;			//adc over sample rate
	u8 prediv_premulti;	//adcclk and dacclk divider
	u8 cf_dsp_div;		//adclrck divider and daclrck divider
	u8 scale;
};

/* codec hifi mclk clock divider coefficients */
static const struct _mclk_lrck_ratio ratio_div[] = {
	//ratio 	nfs,  osr,	pre,  div  ,scale
	{3072, 0, 0x20, 0x50, 0x00, 0x00},
	{3072, 2, 0x20, 0xb0, 0x00, 0x00},

	{2048, 0, 0x20, 0x30, 0x00, 0x00},
	{2048, 2, 0x20, 0x70, 0x00, 0x00},
	{2048, 3, 0x20, 0xb0, 0x00, 0x00},
	{2048, 4, 0x20, 0xf0, 0x00, 0x00},

	{1536, 0, 0x20, 0x20, 0x00, 0x00},
	{1536, 2, 0x20, 0x50, 0x00, 0x00},
	{1536, 3, 0x20, 0x80, 0x00, 0x00},
	{1536, 4, 0x20, 0xb0, 0x00, 0x00},

	{1024, 0, 0x20, 0x10, 0x00, 0x00},
	{1024, 2, 0x20, 0x30, 0x00, 0x00},
	{1024, 3, 0x20, 0x50, 0x00, 0x00},
	{1024, 4, 0x20, 0x70, 0x00, 0x00},
	{1024, 5, 0x20, 0x90, 0x00, 0x00},
	{1024, 6, 0x20, 0xb0, 0x00, 0x00},
	{1024, 7, 0x20, 0xd0, 0x00, 0x00},
	{1024, 8, 0x20, 0xf0, 0x00, 0x00},

	{768, 0, 0x20, 0x21, 0x00, 0x00},
	{768, 2, 0x20, 0x20, 0x00, 0x00},
	{768, 3, 0x20, 0x81, 0x00, 0x00},
	{768, 4, 0x20, 0x50, 0x00, 0x00},
	{768, 5, 0x20, 0xe1, 0x00, 0x00},
	{768, 6, 0x20, 0x80, 0x00, 0x00},
	{768, 8, 0x20, 0xb0, 0x00, 0x00},

	{512, 0, 0x20, 0x00, 0x00, 0x00},
	{512, 2, 0x20, 0x10, 0x00, 0x00},
	{512, 3, 0x20, 0x20, 0x00, 0x00},
	{512, 4, 0x20, 0x30, 0x00, 0x00},
	{512, 5, 0x20, 0x40, 0x00, 0x00},
	{512, 6, 0x20, 0x50, 0x00, 0x00},
	{512, 7, 0x20, 0x60, 0x00, 0x00},
	{512, 8, 0x20, 0x70, 0x00, 0x00},

	{384, 0, 0x20, 0x22, 0x00, 0x00},
	{384, 2, 0x20, 0x21, 0x00, 0x00},
	{384, 3, 0x20, 0x82, 0x00, 0x00},
	{384, 4, 0x20, 0x20, 0x00, 0x00},
	{384, 5, 0x20, 0xe2, 0x00, 0x00},
	{384, 6, 0x20, 0x81, 0x00, 0x00},
	{384, 8, 0x20, 0x50, 0x00, 0x00},

	{256, 0, 0x20, 0x01, 0x00, 0x00},
	{256, 2, 0x20, 0x00, 0x00, 0x00},
	{256, 3, 0x20, 0x21, 0x00, 0x00},
	{256, 4, 0x20, 0x10, 0x00, 0x00},
	{256, 5, 0x20, 0x41, 0x00, 0x00},
	{256, 6, 0x20, 0x20, 0x00, 0x00},
	{256, 7, 0x20, 0x61, 0x00, 0x00},
	{256, 8, 0x20, 0x30, 0x00, 0x00},

	{192, 0, 0x20, 0x23, 0x00, 0x00},
	{192, 2, 0x20, 0x22, 0x00, 0x00},
	{192, 3, 0x20, 0x83, 0x00, 0x00},
	{192, 4, 0x20, 0x21, 0x00, 0x00},
	{192, 5, 0x20, 0xe3, 0x00, 0x00},
	{192, 6, 0x20, 0x82, 0x00, 0x00},
	{192, 8, 0x20, 0x20, 0x00, 0x00},

	{128, 0, 0x20, 0x02, 0x00, 0x00},
	{128, 2, 0x20, 0x01, 0x00, 0x00},
	{128, 3, 0x20, 0x22, 0x00, 0x00},
	{128, 4, 0x20, 0x00, 0x00, 0x00},
	{128, 5, 0x20, 0x42, 0x00, 0x00},
	{128, 6, 0x20, 0x21, 0x00, 0x00},
	{128, 7, 0x20, 0x62, 0x00, 0x00},
	{128, 8, 0x20, 0x10, 0x00, 0x00},

	{64, 0, 0x20, 0x03, 0x00, 0x00},
	{64, 2, 0x20, 0x02, 0x00, 0x00},
	{64, 3, 0x20, 0x23, 0x00, 0x00},
	{64, 4, 0x20, 0x01, 0x00, 0x00},
	{64, 5, 0x20, 0x43, 0x00, 0x00},
	{64, 6, 0x20, 0x22, 0x00, 0x00},
	{64, 7, 0x20, 0x63, 0x00, 0x00},
	{64, 8, 0x20, 0x00, 0x00, 0x00},
};

static inline int get_mclk_lrck_ratio(int clk_ratio, int n_fs)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ratio_div); i++) {
		if (ratio_div[i].ratio == clk_ratio
			&& ratio_div[i].nfs == n_fs)

			return i;
	}

	return -EINVAL;
}

static int es7243l_probe(struct snd_soc_codec *codec)
{
	struct es7243l_priv *es7243l = snd_soc_codec_get_drvdata(codec);
	int ret = 0;
	u8 index, regv, chn, work_mode, ratio_index, datbits;
	u16 ratio;
	u8 digital_vol[16], pga_gain[16];


	es7243l_codec = codec;

	codec->control_data = devm_regmap_init_i2c(es7243l->i2c,
						   &es7243l_regmap_config);
	ret = PTR_RET(codec->control_data);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set cache I/O: %d\n", ret);
		return ret;
	}

	tron_codec[es7243l_codec_num++] = codec;
	index = 0;
#if ES7243l_CHANNELS_MAX > 0
	digital_vol[index] = ES7243l_DIGITAL_VOLUME_1;
	pga_gain[index] = ES7243l_MIC_ARRAY_AIN1_PGA;
	index++;
	digital_vol[index] = ES7243l_DIGITAL_VOLUME_2;
	pga_gain[index] = ES7243l_MIC_ARRAY_AIN2_PGA;
	index++;
#endif

	for (index = 0; index < (ES7243l_CHANNELS_MAX) / 2; index++) {
		es7243l_read(0x02, &regv, i2c_clt[index]);
		if (es7243l->mclksrc == FROM_MCLK_PIN)
			regv &= 0x7f;
		else
			regv |= 0x80;
		regv &= 0xfd;
		if (es7243l->mclkinv == true) {
			regv |= 0x20;
		}
		regv &= 0xfe;
		if (es7243l->bclkinv == true) {
			regv |= 0x01;
		}
		es7243l_write(0x02, regv, i2c_clt[index]);
		/*
		 *		set data bits
		 */
		es7243l_read(0x0b, &regv, i2c_clt[index]);
		regv &= 0xe3;
		datbits = ES7243l_DATA_LENGTH;
		switch (datbits) {
		case DATA_16BITS:
			regv |= 0x0c;
			break;
		case DATA_24BITS:
			break;
		case DATA_32BITS:
			regv |= 0x10;
			break;
		default:
			regv |= 0x0c;
			break;
		}
		es7243l_write(0x0b, regv, i2c_clt[index]);
		/*
		 *		set sdp format and tdm mode
		 */
		chn = ES7243l_CHANNELS_MAX / 2;
		switch (es7243l->tdm) {
		case ES7243l_NORMAL_I2S:
			es7243l_read(0x0b, &regv, i2c_clt[index]);
			regv &= 0xfc;
			es7243l_write(0x0b, regv, i2c_clt[index]);
			es7243l_read(0x0c, &regv, i2c_clt[index]);
			regv &= 0xc0;
			es7243l_write(0x0c, regv, i2c_clt[index]);
			work_mode = 0;
			break;
		case ES7243l_NORMAL_LJ:
			es7243l_read(0x0b, &regv, i2c_clt[index]);
			regv &= 0xfc;
			regv |= 0x01;
			es7243l_write(0x0b, regv, i2c_clt[index]);
			es7243l_read(0x0c, &regv, i2c_clt[index]);
			regv &= 0xc0;
			es7243l_write(0x0c, regv, i2c_clt[index]);
			work_mode = 0;
			break;

		case ES7243l_NORMAL_DSPA:
			es7243l_read(0x0b, &regv, i2c_clt[index]);
			regv &= 0xdc;
			regv |= 0x03;
			es7243l_write(0x0b, regv, i2c_clt[index]);
			es7243l_read(0x0c, &regv, i2c_clt[index]);
			regv &= 0xc0;
			es7243l_write(0x0c, regv, i2c_clt[index]);
			work_mode = 0;
			break;
		case ES7243l_NORMAL_DSPB:
			es7243l_read(0x0b, &regv, i2c_clt[index]);
			regv &= 0xdc;
			regv |= 0x23;
			es7243l_write(0x0b, regv, i2c_clt[index]);
			es7243l_read(0x0c, &regv, i2c_clt[index]);
			regv &= 0xc0;
			es7243l_write(0x0c, regv, i2c_clt[index]);
			work_mode = 0;
			break;
		case ES7243l_TDM_A:
			es7243l_read(0x0b, &regv, i2c_clt[index]);
			regv &= 0xdc;
			regv |= 0x03;
			es7243l_write(0x0b, regv, i2c_clt[index]);
			es7243l_read(0x0c, &regv, i2c_clt[index]);
			regv &= 0xc0;
			regv |= 0x08;
			es7243l_write(0x0c, regv, i2c_clt[index]);
			work_mode = 0;
			break;
		case ES7243l_NFS_I2S:
			es7243l_read(0x0b, &regv, i2c_clt[index]);
			regv &= 0xfc;
			es7243l_write(0x0b, regv, i2c_clt[index]);
			es7243l_read(0x0c, &regv, i2c_clt[index]);
			regv &= 0xc0;
			switch (chn) {
			case 2:
				regv |= 0x01;
				work_mode = 2;
				break;
			case 3:
				regv |= 0x02;
				work_mode = 3;
				break;
			case 4:
				regv |= 0x03;
				work_mode = 4;
				break;
			case 5:
				regv |= 0x04;
				work_mode = 5;
				break;
			case 6:
				regv |= 0x05;
				work_mode = 6;
				break;
			case 7:
				regv |= 0x06;
				work_mode = 7;
				break;
			case 8:
				regv |= 0x07;
				work_mode = 8;
				break;
			default:
				work_mode = 0;
				break;
			}
			/*
			 * the last chip generate flag bits, others chip in sync mode
			 */
			if (index == ((ES7243l_CHANNELS_MAX / 2) - 1))
				regv |= 0x10;
			else
				regv |= 0x20;
			es7243l_write(0x0c, regv, i2c_clt[index]);
			break;
		default:
			work_mode = 0;
			break;
		}

		/*
		 *		set clock divider and multiplexer according clock ratio and nfs mode
		 */
		ratio = ES7243l_MCLK_LRCK_RATIO;
		ratio_index = get_mclk_lrck_ratio(ratio, work_mode);
		if (ratio_index < 0) {
			pr_err
				("can't get configuration for %d ratio with %d es7243l",
				 ratio, work_mode);
			es7243l_write(0x03, 0x20, i2c_clt[index]);
			es7243l_write(0x0d, 0x00, i2c_clt[index]);
			es7243l_write(0x04, 0x00, i2c_clt[index]);
			es7243l_write(0x05, 0x00, i2c_clt[index]);
		} else {
			es7243l_write(0x03, ratio_div[ratio_index].osr,
					  i2c_clt[index]);
			es7243l_write(0x0d, ratio_div[ratio_index].scale,
					  i2c_clt[index]);
			es7243l_write(0x04,
					  ratio_div
					  [ratio_index].prediv_premulti,
					  i2c_clt[index]);
			es7243l_write(0x05,
					  ratio_div[ratio_index].cf_dsp_div,
					  i2c_clt[index]);
		}

		es7243l_write(0x09, 0xe0, i2c_clt[index]);
		es7243l_write(0x0a, 0xa0, i2c_clt[index]);

		es7243l_write(0x0e, digital_vol[index * 2], i2c_clt[index]);

		es7243l_write(0x0f, 0x80, i2c_clt[index]);
		es7243l_write(0x14, 0x0c, i2c_clt[index]);
		es7243l_write(0x15, 0x0c, i2c_clt[index]);
		/*
			we read chip ID here to differentiate 7243L and 7243E
		*/
		es7243l_read(0xfd, &regv, i2c_clt[index]);
		if(0x72==regv){
			//7243l
			es7243l_write(0x16, 0x00, i2c_clt[index]);
			es7243l_write(0x18, 0x24, i2c_clt[index]);
			es7243l_write(0x17, 0x02, i2c_clt[index]);
			es7243l_write(0x19, 0x88, i2c_clt[index]);
			es7243l_write(0x1a, 0x84, i2c_clt[index]);
			es7243l_write(0x1b, 0x66, i2c_clt[index]);
			es7243l_write(0x1c, 0x44, i2c_clt[index]);
			es7243l_write(0x1d, 0xf0, i2c_clt[index]);
			es7243l_write(0x1e, 0x0f, i2c_clt[index]);
			es7243l_write(0x1f, 0x07, i2c_clt[index]);
		}else if(0x7a==regv){
			//7243e
#if AVDD_3V3
			es7243l_write(0x18, 0x26, i2c_clt[index]);
			es7243l_write(0x17, 0x02, i2c_clt[index]);
			es7243l_write(0x19, 0x77, i2c_clt[index]);
			es7243l_write(0x1a, 0xf4, i2c_clt[index]);
			es7243l_write(0x1b, 0x66, i2c_clt[index]);
			es7243l_write(0x1c, 0x44, i2c_clt[index]);
			es7243l_write(0x1d, 0x3c, i2c_clt[index]);
			es7243l_write(0x1e, 0x00, i2c_clt[index]);
			es7243l_write(0x1f, 0x0c, i2c_clt[index]);
#else
			es7243l_write(0x16, 0x00, i2c_clt[index]);
			es7243l_write(0x18, 0x26, i2c_clt[index]);
			es7243l_write(0x17, 0x02, i2c_clt[index]);
			es7243l_write(0x19, 0x66, i2c_clt[index]);
			es7243l_write(0x1a, 0x44, i2c_clt[index]);
			es7243l_write(0x1b, 0x44, i2c_clt[index]);
			es7243l_write(0x1c, 0x44, i2c_clt[index]);
			es7243l_write(0x1d, 0x3c, i2c_clt[index]);
			es7243l_write(0x1e, 0x0f, i2c_clt[index]);
			es7243l_write(0x1f, 0x07, i2c_clt[index]);
#endif
		}
		es7243l_write(0x00, 0x80, i2c_clt[index]);
		es7243l_write(0x01, 0x3a, i2c_clt[index]);
		es7243l_write(0x16, 0x00, i2c_clt[index]);

		es7243l_write(0x20, (0x10 | pga_gain[index * 2]),
				  i2c_clt[index]);
		es7243l_write(0x21, (0x10 | pga_gain[index * 2 + 1]),
				  i2c_clt[index]);

		//es7243l_write(0x1f, 0x03, i2c_clt[index]);
		/*
		 * reset PGA
		 */
		msleep(100);
		es7243l_write(0x16, 0x03, i2c_clt[index]);
		msleep(100);
		es7243l_write(0x16, 0x00, i2c_clt[index]);
	}

	return 0;
}

static int es7243l_remove(struct snd_soc_codec *codec)
{
	es7243l_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_es7243l = {
	.probe = es7243l_probe,
	.remove = es7243l_remove,
	.suspend = es7243l_suspend,
	.resume = es7243l_resume,
	.set_bias_level = es7243l_set_bias_level,
	.idle_bias_off = true,
	.reg_word_size = sizeof(u8),
#if ES7243l_CODEC_RW_TEST_EN
	.read = es7243l_codec_read,
	.write = es7243l_codec_write,
#endif
	.component_driver = {
		.controls = es7243l_snd_controls,
		.num_controls = ARRAY_SIZE(es7243l_snd_controls),
	},
};

enum reg_sysfs_cmd {
	SYSFS_CMD_READ_REG	  = 0,
	SYSFS_CMD_WRITE_REG   = 1,
	SYSFS_CMD_MAX
};
struct es7243_sysfs_cmd {
	enum reg_sysfs_cmd cmd;
	const char *key_str;
};

static ssize_t es7243l_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	char *p = NULL;
	char *endp = NULL;
	bool found = false;
	int i;
	u8 op_reg, rd_val = 0, wr_val = 0;

	struct es7243_sysfs_cmd cmd_arr[] = {
		{SYSFS_CMD_READ_REG, "show"},
		{SYSFS_CMD_WRITE_REG, "set"},
	};

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
		pr_err("show reg\n");
		pr_err("set reg value\n");
		return count;
	}

	op_reg = (u32)simple_strtoul(p + 1, &endp, 0);
	if(op_reg < 0 || op_reg > 0x21) {
		pr_err("op_reg is error\n");
		return count;
	}

	switch (cmd_arr[i].cmd) {
	case SYSFS_CMD_READ_REG:
		es7243l_read(op_reg, &rd_val, i2c_clt[0]);
		pr_err("\nrd reg[%02d][0x%02x] 0x%x\n", op_reg, op_reg, rd_val);
		break;

	case SYSFS_CMD_WRITE_REG:
		if (endp == NULL) {
			pr_err("not enough para, wr_val needed\n");
			return count;
		}

		wr_val = (u32)simple_strtoul(endp + 1, &endp, 0);
		es7243l_write(op_reg, wr_val, i2c_clt[0]);
		pr_err("\nwr reg [%02d][0x%02x], val = 0x%x\n", op_reg, op_reg, wr_val);
		break;
	default:
		break;

	}

	return count;
}

static ssize_t
es7243l_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int off = 0;
	int i;
	u8 val;

	if (buf == NULL) {
		dev_err(dev, "buf is NULL\n");
		return 0;
	}

	off += sprintf(buf + off, "------dump regs begin------\n");
	for (i = 0; i < 0x021; i++) {
		es7243l_read(i, &val, i2c_clt[0]);
		off += sprintf(buf + off, "[%02d][0x%02x] 0x%x\n", i, i, val);
	}
	off += sprintf(buf + off, "------dump regs end	------\n");

	return off;
}

static DEVICE_ATTR(es7243l, 0644, es7243l_show, es7243l_store);

static struct attribute *es7243l_debug_attrs[] = {
	&dev_attr_es7243l.attr,
	NULL,
};

static struct attribute_group es7243l_debug_attr_group = {
	.attrs = es7243l_debug_attrs,
};

static int es7243l_i2c_probe(struct i2c_client *i2c,
								const struct i2c_device_id *i2c_id)
{
	struct es7243l_priv *es7243l;
	int ret;

	es7243l = devm_kzalloc(&i2c->dev, sizeof(struct es7243l_priv),
				   GFP_KERNEL);
	if (es7243l == NULL)
		return -ENOMEM;
	es7243l->i2c = i2c;
	es7243l->tdm = ES7243l_WORK_MODE;	//to initialize tdm mode
	es7243l->mclksrc = ES7243l_MCLK_SOURCE;
	es7243l->dmic = DMIC_INTERFACE;
	es7243l->mclkinv = MCLK_INVERTED_OR_NOT;
	es7243l->bclkinv = BCLK_INVERTED_OR_NOT;
	es7243l->vdda = VDDA_VOLTAGE;

	dev_set_drvdata(&i2c->dev, es7243l);
	es7243l->regmap = devm_regmap_init_i2c(i2c, &es7243l_regmap_config);
	if (IS_ERR(es7243l->regmap)) {
		ret = PTR_ERR(es7243l->regmap);
		dev_err(&i2c->dev, "Failed to allocate register map: %d\n", ret);
		return ret;
	}

	if (i2c_id->driver_data < (ES7243l_CHANNELS_MAX) / 2) {
		i2c_clt[i2c_id->driver_data] = i2c;
		ret = snd_soc_register_codec(&i2c->dev,
						 &soc_codec_dev_es7243l,
						 es7243l_dai
						 [i2c_id->driver_data], 1);
		if (ret < 0) {
			kfree(es7243l);
			pr_err("%s(), failed to register codec device\n",
				__func__);
			return ret;
		}
	}

	ret = sysfs_create_group(&i2c->dev.kobj, &es7243l_debug_attr_group);
	if (ret) {
		pr_err("failed to create attr group\n");
	}

	return ret;
}

static int es7243l_i2c_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client->dev.kobj, &es7243l_debug_attr_group);
	snd_soc_unregister_codec(&client->dev);
	return 0;
}

static void es7243l_i2c_shutdown(struct i2c_client *client)
{

}

static const struct i2c_device_id es7243l_i2c_id[] = {
	{"es7243l", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, es7243l_i2c_id);

static const struct of_device_id es7243l_of_match[] = {
	{.compatible = "everest,es7243l",},
	{},
};

MODULE_DEVICE_TABLE(of, es7243l_of_match);

static struct i2c_driver es7243_i2c_driver = {
	.driver = {
		.name		= "es7243",
		.of_match_table = of_match_ptr(es7243l_of_match),
	},
	.probe    = es7243l_i2c_probe,
	.remove   = es7243l_i2c_remove,
	.shutdown = es7243l_i2c_shutdown,
	.id_table = es7243l_i2c_id,
};

module_i2c_driver(es7243_i2c_driver);

MODULE_DESCRIPTION("ASoC es7243 driver");
MODULE_AUTHOR("Will <pengxiaoxin@everset-semi.com>");
MODULE_LICENSE("GPL");
