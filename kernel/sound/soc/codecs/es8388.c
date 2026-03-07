/*
 * es8388.c -- es8388 ALSA SoC audio driver
 *
 * Copyright (c) 2016 Rockchip Electronics Co. Ltd.
 *
 * Author: Mark Brown <will@everset-semi.com>
 * Author: Jianqun Xu <jay.xu@rock-chips.com>
 * Author: Nickey Yang <nickey.yang@rock-chips.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of_gpio.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/tlv.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <linux/proc_fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include "es8388.h"

#define ES8388_DEF_VOL	0x1b
static struct snd_soc_codec *es8388_codec;

//#define CODEC_IS_MASTER

static int es8388_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level);

/*
 * es8388 register cache
 * We can't read the es8388 register space when we
 * are using 2 wire for device control, so we cache them instead.
 */
static const struct reg_default es8388_reg_defaults[] = {
	{0x0, 0x06}, {0x1, 0x1C}, {0x2, 0xC3}, {0x3, 0xFC},  /* 0 */
	{0x4, 0xC0}, {0x5, 0x00}, {0x6, 0x00}, {0x7, 0x7C},  /* 4 */
	{0x8, 0x80}, {0x9, 0x00}, {0xa, 0x00}, {0xb, 0x06},  /* 8 */
	{0xc, 0x00}, {0xd, 0x06}, {0xe, 0x30}, {0xf, 0x30},  /* 12 */
	{0x10, 0xC0}, {0x11, 0xC0}, {0x12, 0x38}, {0x13, 0xB0},  /* 16 */
	{0x14, 0x32}, {0x15, 0x06}, {0x16, 0x00}, {0x17, 0x00},  /* 20 */
	{0x18, 0x06}, {0x19, 0x30}, {0x1a, 0xC0}, {0x1b, 0xC0},  /* 24 */
	{0x1c, 0x08}, {0x1d, 0x04}, {0x1e, 0x1F}, {0x1f, 0xF7},  /* 28 */
	{0x20, 0xFD}, {0x21, 0xFF}, {0x22, 0x1F}, {0x23, 0xF7},  /* 32 */
	{0x24, 0xFD}, {0x25, 0xFF}, {0x26, 0x00}, {0x27, 0x38},  /* 36 */
	{0x28, 0x38}, {0x29, 0x38}, {0x2a, 0x38}, {0x2b, 0x38},  /* 40 */
	{0x2c, 0x38}, {0x2d, 0x00}, {0x2e, 0x00}, {0x2f, 0x00},  /* 44 */
	{0x30, 0x00}, {0x31, 0x00}, {0x32, 0x00}, {0x33, 0x00},  /* 48 */
	{0x34, 0x00},											 /* 52 */
};

enum es8388_mode {
	STEREO_L1_R1,
	STEREO_L1_R1_L2_R2,	// differential input
	MONO_L1,
	MONO_L1_R1,			// differential input
};

struct es8388_hw_connect {
	enum es8388_mode workmode;
	bool ladc;
	bool radc;
	bool lout1;
	bool lout2;
	bool rout1;
	bool rout2;
};

/* codec private data */
struct es8388_priv {
	unsigned int sysclk;
	struct regmap *regmap;
	struct clk *mclk;
	struct snd_pcm_hw_constraint_list *sysclk_constraints;
	struct gpio_desc *ce_gpio;
	bool muted;
	struct es8388_hw_connect hw_con;
};

static const DECLARE_TLV_DB_SCALE(adc_tlv, -9600, 50, 1);
static const DECLARE_TLV_DB_SCALE(dac_tlv, -9600, 50, 1);
static const DECLARE_TLV_DB_SCALE(out_tlv, -4500, 150, 0);
static const DECLARE_TLV_DB_SCALE(bypass_tlv, 0, 300, 0);
static const DECLARE_TLV_DB_SCALE(bypass_tlv2, -15, 300, 0);


static const struct snd_kcontrol_new es8388_snd_controls[] = {
	SOC_DOUBLE_R_TLV("Capture Digital Volume", ES8388_ADCCONTROL8,
			 ES8388_ADCCONTROL9, 0, 192, 1, adc_tlv),
	SOC_SINGLE("Capture Mute", ES8388_ADCCONTROL7, 2, 1, 0),
	SOC_SINGLE_TLV("Left Channel Capture Volume", ES8388_ADCCONTROL1, 4,8,
		       0, bypass_tlv),
	SOC_SINGLE_TLV("Right Channel Capture Volume", ES8388_ADCCONTROL1, 0,
		       8, 0, bypass_tlv),
	SOC_DOUBLE_R_TLV("PCM Volume", ES8388_DACCONTROL4, ES8388_DACCONTROL5,
			 0, 192, 1, dac_tlv),
	SOC_DOUBLE_R_TLV("Output 1 Playback Volume", ES8388_DACCONTROL24,
			 ES8388_DACCONTROL25, 0, 33, 0, out_tlv),
	SOC_DOUBLE_R_TLV("Output 2 Playback Volume", ES8388_DACCONTROL26,
			 ES8388_DACCONTROL27, 0, 33, 0, out_tlv),
};

struct _coeff_div {
	u32 mclk;
	u32 rate;
	u16 fs;
	u8 sr:4;
	u8 usb:1;
};

/* codec hifi mclk clock divider coefficients */
#ifdef CODEC_IS_MASTER /* codec is master*/
static const struct _coeff_div coeff_div[] = {
	/* 8k */
	{12288000, 8000, 1536, 0xa, 0x0},
	{11289600, 8000, 1408, 0x9, 0x0},
	{18432000, 8000, 2304, 0xc, 0x0},
	{16934400, 8000, 2112, 0xb, 0x0},
	{12000000, 8000, 1500, 0xb, 0x1},

	/* 11.025k */
	{11289600, 11025, 1024, 0x7, 0x0},
	{16934400, 11025, 1536, 0xa, 0x0},
	{12000000, 11025, 1088, 0x9, 0x1},

	/* 16k */
	{12288000, 16000, 768, 0x6, 0x0},
	{18432000, 16000, 1152, 0x8, 0x0},
	{12000000, 16000, 750, 0x7, 0x1},

	/* 22.05k */
	{11289600, 22050, 512, 0x4, 0x0},
	{16934400, 22050, 768, 0x6, 0x0},
	{12000000, 22050, 544, 0x6, 0x1},

	/* 32k */
	{12288000, 32000, 384, 0x3, 0x0},
	{18432000, 32000, 576, 0x5, 0x0},
	{12000000, 32000, 375, 0x4, 0x1},

	/* 44.1k */
	{11289600, 44100, 768, 0x6, 0x0},
	{16934400, 44100, 384, 0x3, 0x0},
	{12000000, 44100, 272, 0x3, 0x1},

	/* 48k */
	{12288000, 48000, 256, 0x2, 0x0},
	{18432000, 48000, 384, 0x3, 0x0},
	{12000000, 48000, 250, 0x2, 0x1},

	/* 88.2k */
	{11289600, 88200, 128, 0x0, 0x0},
	{16934400, 88200, 192, 0x1, 0x0},
	{12000000, 88200, 136, 0x1, 0x1},

	/* 96k */
	{12288000, 96000, 128, 0x0, 0x0},
	{18432000, 96000, 192, 0x1, 0x0},
	{12000000, 96000, 125, 0x0, 0x1},

};
#else /* codec is salve	*/
static const struct _coeff_div coeff_div[] = {
	/* 8k */
	{12288000, 8000, 256, 0x2, 0x0},
	{12000000, 8000, 256, 0x2, 0x0},

	/* 12K */
	{12288000, 12000, 256, 0x2, 0x0},
	{12000000, 12000, 256, 0x2, 0x0},

	/* 11.025k */
	{12288000, 11025, 192, 0x1, 0x0},
	{11289600, 11025, 1024, 0x7, 0x0},
	{16934400, 11025, 1536, 0xa, 0x0},
	{12000000, 11025, 192, 0x1, 0x0},

	/* 16k */
	{12288000, 16000, 128, 0x0, 0x0},
	{11289600, 16000, 768, 0x6, 0x0},
	{18432000, 16000, 1152, 0x8, 0x0},
	{12000000, 16000, 768, 0x6, 0x0},

	/* 22.05k */
	{12288000, 22050, 256, 0x2, 0x0},
	{11289600, 22050, 256, 0x2, 0x0},
	{16934400, 22050, 768, 0x6, 0x0},
	{12000000, 22050, 256, 0x2, 0x0},

	/* 24k */
	{12288000, 24000, 128, 0x0, 0x0},
	{11289600, 24000, 128, 0x0, 0x0},
	{12000000, 24000, 256, 0x2, 0x0},

	/* 32k */
	{12288000, 32000, 128, 0x0, 0x0},
	{11289600, 32000, 128, 0x0, 0x0},
	{18432000, 32000, 576, 0x5, 0x0},
	{12000000, 32000, 256, 0x2, 0x0},

	/* 44.1k */
	{12288000, 44100, 768, 0x6, 0x0},
	{11289600, 44100, 768, 0x6, 0x0},
	{16934400, 44100, 384, 0x3, 0x0},
	{12000000, 44100, 128, 0x0, 0x0},

	/* 48k */
	{12288000, 48000, 128, 0x0, 0x0},
	{18432000, 48000, 384, 0x3, 0x0},
	{12000000, 48000, 128, 0x0, 0x0},

	/* 64k */
	{12288000, 64000, 128, 0x0, 0x0},
	{12000000, 64000, 128, 0x0, 0x0},

	/* 96k */
	{12288000, 96000, 128, 0x0, 0x0},
	{18432000, 96000, 192, 0x1, 0x0},
	{12000000, 96000, 128, 0x0, 0x0},
};
#endif
static inline int get_coeff(int mclk, int rate)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(coeff_div); i++) {
		if (coeff_div[i].rate == rate && coeff_div[i].mclk == mclk)
			return i;
	}

	return -EINVAL;
}

static unsigned int rates_12288[] = {
	8000, 11025, 12000, 16000, 22050, 24000,
	32000, 44100, 48000, 64000, 96000,
};

static struct snd_pcm_hw_constraint_list constraints_12288 = {
	.count = ARRAY_SIZE(rates_12288),
	.list = rates_12288,
};

static unsigned int rates_112896[] = {
	8000, 11025, 12000, 16000, 22050, 24000,
	32000, 44100, 48000, 64000, 96000,
};

static struct snd_pcm_hw_constraint_list constraints_112896 = {
	.count = ARRAY_SIZE(rates_112896),
	.list = rates_112896,
};

static unsigned int rates_12[] = {
	8000, 11025, 12000, 16000, 22050, 24000,
	32000, 44100, 48000, 64000, 96000,
};

static struct snd_pcm_hw_constraint_list constraints_12 = {
	.count = ARRAY_SIZE(rates_12),
	.list = rates_12,
};

/*
 * Note that this should be called from init rather than from hw_params.
 */
static int es8388_set_dai_sysclk(struct snd_soc_dai *codec_dai,
				 int clk_id, unsigned int freq, int dir)
{

	struct snd_soc_codec *codec = codec_dai->codec;
	struct es8388_priv *es8388 = snd_soc_codec_get_drvdata(codec);

	switch (freq) {
	case 11289600:
	case 18432000:
	case 22579200:
	case 36864000:
		es8388->sysclk_constraints = &constraints_112896;
		es8388->sysclk = freq;
		return 0;

	case 12288000:
	case 16934400:
	case 24576000:
	case 33868800:
		es8388->sysclk_constraints = &constraints_12288;
		es8388->sysclk = freq;
		return 0;

	case 12000000:
	case 24000000:
		es8388->sysclk_constraints = &constraints_12;
		es8388->sysclk = freq;
		return 0;
	}

	return -EINVAL;

}

static int es8388_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	u8 iface = 0;
	u8 adciface = 0;
	u8 daciface = 0;

	/* set master/slave audio interface */
	iface = snd_soc_read(codec, ES8388_IFACE);
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:	/* MASTER MODE */
		iface |= ES8388_MASTERMODE_MSC;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:	/* SLAVE MODE */
		iface &= ~ES8388_MASTERMODE_MSC;
		break;
	default:
		return -EINVAL;
	}

	/* interface format */
	adciface = snd_soc_read(codec, ES8388_ADC_IFACE);
	daciface = snd_soc_read(codec, ES8388_DAC_IFACE);
	adciface &= ~ES8388_ADCCONTROL4_ADCFORMAT_MASK;
	daciface &= ~ES8388_DACCONTROL1_DACFORMAT_MASK;
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		adciface |= ES8388_ADCCONTROL4_ADCFORMAT_I2S;
		daciface |= ES8388_DACCONTROL1_DACFORMAT_I2S;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		break;
	case SND_SOC_DAIFMT_DSP_A:
		break;
	case SND_SOC_DAIFMT_DSP_B:
		break;
	default:
		return -EINVAL;
	}

	/* clock inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		iface		|= ES8388_MASTERMODE_BCLK_NORMAL;
		adciface	|= ES8388_ADCCONTROL4_ADCLRP_I2S_POL_NORMAL;
		daciface	|= ES8388_DACCONTROL1_DACLRP_I2S_POL_NORMAL;
		break;
	case SND_SOC_DAIFMT_IB_IF:
		iface		|= ES8388_MASTERMODE_BCLK_INV;
		adciface	|= ES8388_ADCCONTROL4_ADCLRP_I2S_POL_INV;
		daciface	|= ES8388_DACCONTROL1_DACLRP_I2S_POL_INV;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		iface		|= ES8388_MASTERMODE_BCLK_INV;
		adciface	|= ES8388_ADCCONTROL4_ADCLRP_I2S_POL_NORMAL;
		daciface	|= ES8388_DACCONTROL1_DACLRP_I2S_POL_NORMAL;
		break;
	case SND_SOC_DAIFMT_NB_IF:
		iface		|= ES8388_MASTERMODE_BCLK_NORMAL;
		adciface	|= ES8388_ADCCONTROL4_ADCLRP_I2S_POL_INV;
		daciface	|= ES8388_DACCONTROL1_DACLRP_I2S_POL_INV;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_write(codec, ES8388_IFACE, iface);
	snd_soc_write(codec, ES8388_ADC_IFACE, adciface);
	snd_soc_write(codec, ES8388_DAC_IFACE, daciface);

	return 0;
}

static int es8388_pcm_startup(struct snd_pcm_substream *substream,
			      struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct es8388_priv *es8388 = snd_soc_codec_get_drvdata(codec);

	if(es8388->sysclk_constraints && es8388->sysclk) {
		snd_pcm_hw_constraint_list(substream->runtime, 0,
					SNDRV_PCM_HW_PARAM_RATE, es8388->sysclk_constraints);
	}

	return 0;
}

static int es8388_pcm_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	struct es8388_priv *es8388 = snd_soc_codec_get_drvdata(codec);
	u16 adciface = 0;
	u16 daciface = 0;
	int coeff;
	u16 srate = snd_soc_read(codec, ES8388_IFACE);

	coeff = get_coeff(es8388->sysclk, params_rate(params));
	if (coeff < 0) {
		dev_err(codec->dev,
			"Unable to configure sample rate %dHz with %dHz MCLK\n",
			params_rate(params), es8388->sysclk);
		return coeff;
	}

	/* bit size */
	adciface = snd_soc_read(codec, ES8388_ADC_IFACE);
	daciface = snd_soc_read(codec, ES8388_DAC_IFACE);
	adciface &= ~ES8388_ADCCONTROL4_ADCWL_MASK;
	daciface &= ~ES8388_DACCONTROL1_DACWL_MASK;
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		adciface |= (ADCWL_16BIT << ES8388_ADCCONTROL4_ADCWL_SHIFT);
		daciface |= (DACWL_16BIT << ES8388_DACCONTROL1_DACWL_SHIFT);
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		adciface |= (ADCWL_20BIT << ES8388_ADCCONTROL4_ADCWL_SHIFT);
		daciface |= (DACWL_20BIT << ES8388_DACCONTROL1_DACWL_SHIFT);
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		adciface |= (ADCWL_24BIT << ES8388_ADCCONTROL4_ADCWL_SHIFT);
		daciface |= (DACWL_24BIT << ES8388_DACCONTROL1_DACWL_SHIFT);
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		adciface |= (ADCWL_32BIT << ES8388_ADCCONTROL4_ADCWL_SHIFT);
		daciface |= (DACWL_32BIT << ES8388_DACCONTROL1_DACWL_SHIFT);
		break;
	}

	/* set iface & srate */
	snd_soc_write(codec, ES8388_DAC_IFACE, daciface);
	snd_soc_write(codec, ES8388_ADC_IFACE, adciface);

	if (coeff >= 0) {
		snd_soc_write(codec, ES8388_IFACE, srate);
		snd_soc_write(codec, ES8388_ADCCONTROL5,
					coeff_div[coeff].sr | (coeff_div[coeff].usb) << 5);
		snd_soc_write(codec, ES8388_DACCONTROL2,
					coeff_div[coeff].sr | (coeff_div[coeff].usb) << 5);
	}

	return 0;
}

static int es8388_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	struct es8388_priv *es8388 = snd_soc_codec_get_drvdata(codec);

	es8388->muted = mute;
	if (mute) {
		snd_soc_write(codec, ES8388_DACCONTROL3, ES8388_DACCONTROL3_DACMUTE);
	} else {
		snd_soc_write(codec, ES8388_DACCONTROL3, ES8388_DACCONTROL3_DACUNMUTE);
		msleep(10);
	}
	return 0;
}

static int es8388_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
	struct es8388_priv *es8388 = snd_soc_codec_get_drvdata(codec);
	int ret;
	u16 val;

	switch (level) {
	case SND_SOC_BIAS_ON:
		dev_dbg(codec->dev, "%s on\n", __func__);
		break;
	case SND_SOC_BIAS_PREPARE:
		dev_dbg(codec->dev, "%s prepare\n", __func__);
		if (IS_ERR(es8388->mclk))
			break;
		if (snd_soc_codec_get_bias_level(codec) == SND_SOC_BIAS_ON) {
			clk_disable_unprepare(es8388->mclk);
		} else {
			ret = clk_prepare_enable(es8388->mclk);
			if (ret)
				return ret;
		}

		snd_soc_write(codec, ES8388_ANAVOLMANAG, 0x7C);
		snd_soc_write(codec, ES8388_CHIPLOPOW1, 0x00);
		snd_soc_write(codec, ES8388_CHIPLOPOW2, 0x00);
		snd_soc_write(codec, ES8388_CHIPPOWER, 0x00);
		val = snd_soc_read(codec, ES8388_ADCPOWER);
		if (es8388->hw_con.ladc)
			val &= ~(1 << 5);
		if (es8388->hw_con.radc)
			val &= ~(1 << 4);
		snd_soc_write(codec, ES8388_ADCPOWER, val);
		break;
	case SND_SOC_BIAS_STANDBY:
		dev_dbg(codec->dev, "%s standby\n", __func__);
		snd_soc_write(codec, ES8388_ANAVOLMANAG, 0x7C);
		snd_soc_write(codec, ES8388_CHIPLOPOW1, 0x00);
		snd_soc_write(codec, ES8388_CHIPLOPOW2, 0x00);
		snd_soc_write(codec, ES8388_CHIPPOWER, 0x00);
		val = snd_soc_read(codec, ES8388_ADCPOWER);
		if (es8388->hw_con.ladc)
			val &= ~(1 << 5);
		if (es8388->hw_con.radc)
			val &= ~(1 << 4);
		snd_soc_write(codec, ES8388_ADCPOWER, val);

		val = snd_soc_read(codec, ES8388_DACPOWER);
		if (es8388->hw_con.lout1)
			val |= (1 << 5);
		if (es8388->hw_con.lout2)
			val |= (1 << 3);
		if (es8388->hw_con.rout1)
			val |= (1 << 4);
		if (es8388->hw_con.rout2)
			val |= (1 << 2);
		snd_soc_write(codec, ES8388_DACPOWER, val);
		break;
	case SND_SOC_BIAS_OFF:
		if (es8388->mclk)
			clk_disable_unprepare(es8388->mclk);
		dev_dbg(codec->dev, "%s off\n", __func__);
		snd_soc_write(codec, ES8388_ADCPOWER, 0xFF);
		snd_soc_write(codec, ES8388_DACPOWER, 0xC0);
		snd_soc_write(codec, ES8388_CHIPLOPOW1, 0xFF);
		snd_soc_write(codec, ES8388_CHIPLOPOW2, 0xFF);
		snd_soc_write(codec, ES8388_CHIPPOWER, 0xFF);
		snd_soc_write(codec, ES8388_ANAVOLMANAG, 0x7B);
		break;
	}
		return 0;
}

static struct snd_soc_dai_ops es8388_ops = {
	.startup		= es8388_pcm_startup,
	.hw_params		= es8388_pcm_hw_params,
	.set_fmt		= es8388_set_dai_fmt,
	.set_sysclk		= es8388_set_dai_sysclk,
	.digital_mute	= es8388_mute,
};

static struct snd_soc_dai_driver es8388_dai = {
	.name = "ES8388 HiFi",
	.playback = {
		.stream_name	= "Playback",
		.channels_min	= 1,
		.channels_max	= 2,
		.rates			= (
						SNDRV_PCM_RATE_8000  |
						SNDRV_PCM_RATE_11025 |
						SNDRV_PCM_RATE_12000 |
						SNDRV_PCM_RATE_16000 |
						SNDRV_PCM_RATE_22050 |
						SNDRV_PCM_RATE_24000 |
						SNDRV_PCM_RATE_32000 |
						SNDRV_PCM_RATE_44100 |
						SNDRV_PCM_RATE_48000 |
						SNDRV_PCM_RATE_64000 |
						SNDRV_PCM_RATE_96000),
		.formats		= (
						SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE|
						SNDRV_PCM_FMTBIT_S32_LE),
	},
	.capture = {
		.stream_name	= "Capture",
		.channels_min	= 1,
		.channels_max	= 2,
		.rates			= (
						SNDRV_PCM_RATE_8000  |
						SNDRV_PCM_RATE_11025 |
						SNDRV_PCM_RATE_12000 |
						SNDRV_PCM_RATE_16000 |
						SNDRV_PCM_RATE_22050 |
						SNDRV_PCM_RATE_24000 |
						SNDRV_PCM_RATE_32000 |
						SNDRV_PCM_RATE_44100 |
						SNDRV_PCM_RATE_48000 |
						SNDRV_PCM_RATE_64000 |
						SNDRV_PCM_RATE_96000),
		.formats		= (
						SNDRV_PCM_FMTBIT_S16_LE |
						SNDRV_PCM_FMTBIT_S24_LE |
						SNDRV_PCM_FMTBIT_S24_3LE|
						SNDRV_PCM_FMTBIT_S32_LE),
	},
	.ops = &es8388_ops,
	.symmetric_rates = 1,
};

static int es8388_suspend(struct snd_soc_codec *codec)
{
	snd_soc_write(codec, ES8388_DACCONTROL3, 0x06);
	snd_soc_write(codec, ES8388_DACCONTROL26, 0x00);
	snd_soc_write(codec, ES8388_DACCONTROL27, 0x00);
	snd_soc_write(codec, ES8388_ADCPOWER, 0xFF);
	snd_soc_write(codec, ES8388_DACPOWER, 0xc0);
	snd_soc_write(codec, ES8388_CHIPPOWER, 0xF3);
	snd_soc_write(codec, ES8388_CONTROL1, 0x00);
	snd_soc_write(codec, ES8388_CONTROL2, 0x58);
	snd_soc_write(codec, ES8388_DACCONTROL21, 0x9c);
	usleep_range(18000, 20000);
	return 0;
}

static int es8388_resume(struct snd_soc_codec *codec)
{
	snd_soc_write(codec, ES8388_DACCONTROL21, 0x80);
	snd_soc_write(codec, ES8388_CONTROL2, 0x50);
	snd_soc_write(codec, ES8388_CONTROL1, 0x32);
	snd_soc_write(codec, ES8388_CHIPPOWER, 0x00);
	snd_soc_write(codec, ES8388_DACPOWER, 0x0c);
	snd_soc_write(codec, ES8388_ADCPOWER, 0x59);
	snd_soc_write(codec, ES8388_DACCONTROL27, ES8388_DEF_VOL);
	snd_soc_write(codec, ES8388_DACCONTROL26, ES8388_DEF_VOL);
	snd_soc_write(codec, ES8388_DACCONTROL3, 0x02);
	return 0;
}

static int es8388_reset(struct snd_soc_codec *codec)
{
	snd_soc_write(codec, ES8388_CONTROL1, 0x80);
	return snd_soc_write(codec, ES8388_CONTROL1, 0x00);
}

static int es8388_probe(struct snd_soc_codec *codec)
{
	int ret = 0;
	int val;
	struct es8388_priv *es8388 = snd_soc_codec_get_drvdata(codec);

	if (codec == NULL) {
		dev_err(codec->dev, "Codec device not registered\n");
		return -ENODEV;
	}

	es8388_codec = codec;
	ret = es8388_reset(codec);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to issue reset\n");
		return ret;
	}

	snd_soc_write(codec, ES8388_CONTROL2,0x60);
	snd_soc_write(codec, ES8388_CHIPPOWER, 0xF3);
	snd_soc_write(codec, ES8388_CHIPPOWER, 0xF0);
	snd_soc_write(codec, ES8388_DACCONTROL21,0x80);
	snd_soc_write(codec, ES8388_CONTROL1, 0x36);
	snd_soc_write(codec, ES8388_MASTERMODE, 0x00);
	snd_soc_write(codec, ES8388_DACPOWER, 0x00);
	snd_soc_write(codec, ES8388_CHIPLOPOW2, 0xC3);
	snd_soc_write(codec, ES8388_DACCONTROL3, 0x02);

	if (es8388->hw_con.workmode == STEREO_L1_R1_L2_R2) {
		snd_soc_write(codec, ES8388_ADCCONTROL1, 0x88);
		snd_soc_write(codec, ES8388_ADCCONTROL2, 0xf6);
	} else if (es8388->hw_con.workmode == STEREO_L1_R1) {
		snd_soc_write(codec, ES8388_ADCCONTROL1, 0x00);
		snd_soc_write(codec, ES8388_ADCCONTROL2, 0x00);
	};

	snd_soc_write(codec, ES8388_ADCCONTROL3, 0x00);
	snd_soc_write(codec, ES8388_ADCCONTROL4, 0x0C);
	snd_soc_write(codec, ES8388_ADCCONTROL5, 0x02);
	snd_soc_write(codec, ES8388_ADCCONTROL8, 0x00);
	snd_soc_write(codec, ES8388_ADCCONTROL9, 0x00);
	snd_soc_write(codec, ES8388_DACCONTROL1, 0x18);
	snd_soc_write(codec, ES8388_DACCONTROL2, 0x02);
	snd_soc_write(codec, ES8388_DACCONTROL4, 0x00);
	snd_soc_write(codec, ES8388_DACCONTROL5, 0x00);
	snd_soc_write(codec, ES8388_DACCONTROL17, 0xB8);
	snd_soc_write(codec, ES8388_DACCONTROL20, 0xB8);
	usleep_range(18000, 20000);
	snd_soc_write(codec, ES8388_DACCONTROL24, 0x1E);
	snd_soc_write(codec, ES8388_DACCONTROL25, 0x1E);
	snd_soc_write(codec, ES8388_DACCONTROL26, 0x1E);
	snd_soc_write(codec, ES8388_DACCONTROL27, 0x1E);
	snd_soc_write(codec, ES8388_ADCPOWER, 0x39);
	snd_soc_write(codec, ES8388_CHIPPOWER, 0x00);
	usleep_range(18000, 20000);
	snd_soc_write(codec, ES8388_DACPOWER, 0x00);

	val = snd_soc_read(codec, ES8388_ADCCONTROL4);
	if (es8388->hw_con.ladc && !es8388->hw_con.radc) {
		val &= ~(0x3 << 6);
		val |= 0x1 << 6;
	}
	snd_soc_write(codec, ES8388_ADCCONTROL4, val);

	val = snd_soc_read(codec, ES8388_DACCONTROL7);
	if (es8388->hw_con.ladc && !es8388->hw_con.radc) {
		val &= ~((0x1 << 6) | (0x1 << 5));
		val |= (0x1 << 6) | (0x1 << 5);
	}
	snd_soc_write(codec, ES8388_DACCONTROL7, val);

	es8388_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	return 0;
}

static int es8388_remove(struct snd_soc_codec *codec)
{
	es8388_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_es8388 = {
	.probe = es8388_probe,
	.remove = es8388_remove,
	.suspend = es8388_suspend,
	.resume = es8388_resume,
	//.set_bias_level = es8388_set_bias_level,
	.component_driver = {
		.controls			= es8388_snd_controls,
		.num_controls		= ARRAY_SIZE(es8388_snd_controls),
	},
};

static bool es8388_volatile(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case ES8388_CONTROL1:
		return true;
	default:
		return false;
	}
}

static const struct regmap_config es8388_regmap = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = 52,
	.reg_defaults = es8388_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(es8388_reg_defaults),
	.cache_type = REGCACHE_RBTREE,
	.volatile_reg = es8388_volatile,
};

static int es8388_parse_dt(struct device *dev,
				struct es8388_priv *es8388)
{
	es8388->ce_gpio = devm_gpiod_get(dev, "ce", GPIOD_OUT_HIGH);
	if (!es8388->ce_gpio) {
		dev_err(dev, "devm_gpiod_get failed.\n");
		return -1;
	}

	gpiod_set_value(es8388->ce_gpio, 1);

	es8388->hw_con.workmode = STEREO_L1_R1_L2_R2;

	switch (es8388->hw_con.workmode) {
	case STEREO_L1_R1_L2_R2:
		es8388->hw_con.ladc = true;
		es8388->hw_con.radc = true;
		es8388->hw_con.lout1 = true;
		es8388->hw_con.lout2 = true;
		es8388->hw_con.rout1 = true;
		es8388->hw_con.rout2 = true;
		break;
	case STEREO_L1_R1:
		es8388->hw_con.ladc = true;
		es8388->hw_con.radc = true;
		es8388->hw_con.lout1 = true;
		es8388->hw_con.rout1 = true;
		break;
	default:
		break;
	};

	return 0;
}

enum reg_sysfs_cmd {
	SYSFS_CMD_READ_REG    = 0,
	SYSFS_CMD_WRITE_REG   = 1,
	SYSFS_CMD_MAX
};
struct es8388_sysfs_cmd {
	enum reg_sysfs_cmd cmd;
	const char *key_str;
};

static ssize_t es8388_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	int off = 0;
	int i;
	u32 val;

	if (buf == NULL) {
		dev_err(dev, "buf is NULL\n");
		return 0;
	}

	off += sprintf(buf + off, "------dump regs begin------\n");
	for (i = 0; i < 0x035; i++) {
		val = snd_soc_read(es8388_codec, i);
		off += sprintf(buf + off, "[%02d][0x%02x] 0x%x\n", i, i, val);
	}
	off += sprintf(buf + off, "------dump regs end	------\n");

	return off;

}

static ssize_t es8388_set(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	char *p = NULL;
	char *endp = NULL;
	bool found = false;
	int i;
	u32 op_reg, rd_val = 0, wr_val = 0;

	struct es8388_sysfs_cmd cmd_arr[] = {
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
	if(op_reg < 0 || op_reg > 52) {
		pr_err("op_reg is error\n");
		return count;
	}

	switch (cmd_arr[i].cmd) {
	case SYSFS_CMD_READ_REG:
		rd_val = snd_soc_read(es8388_codec, op_reg);
		pr_err("\nrd reg[%02d][0x%02x] 0x%x\n", op_reg, op_reg, rd_val);
		break;

	case SYSFS_CMD_WRITE_REG:
		if (endp == NULL) {
			pr_err("not enough para, wr_val needed\n");
			return count;
		}

		wr_val = (u32)simple_strtoul(endp + 1, &endp, 0);
		snd_soc_write(es8388_codec, op_reg, wr_val);
		pr_err("\nwr reg [%02d][0x%02x], val = 0x%x\n", op_reg, op_reg, wr_val);
		break;
	default:
		break;

	}

	return count;
}

static DEVICE_ATTR(es8388_test, S_IWUSR | S_IRUSR,
			es8388_show, es8388_set);

static struct attribute *es8388_attributes[] = {
	&dev_attr_es8388_test.attr,
	NULL
};

static const struct attribute_group es8388_attr_group = {
	.attrs = es8388_attributes,
};


static int es8388_i2c_probe(struct i2c_client *i2c,
							const struct i2c_device_id *id)
{
	struct es8388_priv *es8388;
	int ret = -1;
	struct i2c_adapter *adapter = to_i2c_adapter(i2c->dev.parent);

	if (!i2c_check_functionality(adapter, I2C_FUNC_I2C)) {
		dev_warn(&adapter->dev,
			 "I2C-Adapter doesn't support I2C_FUNC_I2C\n");
		return -EIO;
	}

	es8388 = devm_kzalloc(&i2c->dev, sizeof(struct es8388_priv), GFP_KERNEL);
	if (es8388 == NULL)
		return -ENOMEM;

	es8388->regmap = devm_regmap_init_i2c(i2c, &es8388_regmap);
	if (IS_ERR(es8388->regmap))
		return PTR_ERR(es8388->regmap);

	es8388_parse_dt(&i2c->dev, es8388);
	i2c_set_clientdata(i2c, es8388);

	ret = sysfs_create_group(&i2c->dev.kobj, &es8388_attr_group);
	if (ret < 0)
		dev_warn(&i2c->dev, "attr group create failed\n");
	else
		dev_info(&i2c->dev, "attr group create success!\n");


	ret = snd_soc_register_codec(&i2c->dev,
								&soc_codec_dev_es8388, &es8388_dai, 1);
	return ret;
}

static int es8388_i2c_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client->dev.kobj, &es8388_attr_group);

	snd_soc_unregister_codec(&client->dev);
	return 0;
}

static const struct i2c_device_id es8388_i2c_id[] = {
	{"es8388", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, es8388_i2c_id);

void es8388_i2c_shutdown(struct i2c_client *client)
{
	mdelay(20);
	snd_soc_write(es8388_codec, ES8388_CONTROL2, 0x58);
	snd_soc_write(es8388_codec, ES8388_CONTROL1, 0x32);
	snd_soc_write(es8388_codec, ES8388_CHIPPOWER, 0xf3);
	snd_soc_write(es8388_codec, ES8388_DACPOWER, 0xc0);
	mdelay(50);
	snd_soc_write(es8388_codec, ES8388_DACCONTROL26, 0x00);
	snd_soc_write(es8388_codec, ES8388_DACCONTROL27, 0x00);
	mdelay(50);
	snd_soc_write(es8388_codec, ES8388_CONTROL1, 0x30);
	snd_soc_write(es8388_codec, ES8388_CONTROL1, 0x34);
}

static const struct of_device_id es8388_of_match[] = {
	{ .compatible = "everest,es8388", },
	{ }
};
MODULE_DEVICE_TABLE(of, es8388_of_match);

static struct i2c_driver es8388_i2c_driver = {
	.driver = {
		.name = "ES8388",
		.of_match_table = of_match_ptr(es8388_of_match),
	},
	.shutdown	= es8388_i2c_shutdown,
	.probe		= es8388_i2c_probe,
	.remove		= es8388_i2c_remove,
	.id_table	= es8388_i2c_id,
};
module_i2c_driver(es8388_i2c_driver);

MODULE_DESCRIPTION("ASoC ES8388 audio CODEC I2C driver");
MODULE_AUTHOR("Sean Cross <xobs@kosagi.com>");
MODULE_LICENSE("GPL");
