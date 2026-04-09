/*
 * es8323.c -- es8323 ALSA SoC audio driver
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
#include "es8323.h"

#define INVALID_GPIO -1

#define ES8323_CODEC_SET_SPK	1
#define ES8323_CODEC_SET_HP	2

#define ES8323_DEF_VOL	0x1b
static struct snd_soc_codec *es8323_codec;

static int es8323_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level);

/*
 * es8323 register cache
 * We can't read the es8323 register space when we
 * are using 2 wire for device control, so we cache them instead.
 */
static u16 es8323_reg[] = {
	0x06, 0x1C, 0xC3, 0xFC,	/*  0 */
	0xC0, 0x00, 0x00, 0x7C,	/*  4 */
	0x80, 0x00, 0x00, 0x06,	/*  8 */
	0x00, 0x06, 0x30, 0x30,	/* 12 */
	0xC0, 0xC0, 0x38, 0xB0,	/* 16 */
	0x32, 0x06, 0x00, 0x00,	/* 20 */
	0x06, 0x30, 0xC0, 0xC0,	/* 24 */
	0x08, 0x06, 0x1F, 0xF7,	/* 28 */
	0xFD, 0xFF, 0x1F, 0xF7,	/* 32 */
	0xFD, 0xFF, 0x00, 0x38,	/* 36 */
	0x38, 0x38, 0x38, 0x38,	/* 40 */
	0x38, 0x00, 0x00, 0x00,	/* 44 */
	0x00, 0x00, 0x00, 0x00,	/* 48 */
	0x00, 0x00, 0x00, 0x00,	/* 52 */
};

/* codec private data */
struct es8323_priv {
	unsigned int sysclk;
	struct clk *mclk;
	struct snd_pcm_hw_constraint_list *sysclk_constraints;

	int spk_ctl_gpio;
	int hp_det_gpio;
	struct gpio_desc *ce_gpio;
	bool muted;
	bool hp_inserted;
	bool spk_gpio_level;
	bool hp_det_level;
};

static struct es8323_priv *es8323_private;
/*
static int es8323_set_gpio(int gpio, bool level)
{
	struct es8323_priv *es8323 = es8323_private;

	if (!es8323) {
		return 0;
	}

	if ((gpio & ES8323_CODEC_SET_SPK) && es8323
	    && es8323->spk_ctl_gpio != INVALID_GPIO) {
		gpio_set_value(es8323->spk_ctl_gpio, level);
	}

	return 0;
}

static irqreturn_t hp_det_irq_handler(int irq, void *dev_id)
{
	struct es8323_priv *es8323 = es8323_private;

	if (gpio_get_value(es8323->hp_det_gpio))
		es8323->hp_inserted = 0;
	else
		es8323->hp_inserted = 1;

	if (es8323->muted == 0) {
		if (es8323->hp_det_level != es8323->hp_inserted)
			es8323_set_gpio(ES8323_CODEC_SET_SPK, !es8323->spk_gpio_level);
		else
			es8323_set_gpio(ES8323_CODEC_SET_SPK, es8323->spk_gpio_level);
	}
	return IRQ_HANDLED;
}
*/
static unsigned int es8323_read_reg_cache(struct snd_soc_codec *codec,
					  unsigned int reg)
{
	if (reg >= ARRAY_SIZE(es8323_reg))
		return -1;
	return es8323_reg[reg];
}

static int es8323_write(struct snd_soc_codec *codec, unsigned int reg,
			unsigned int value)
{
	u8 data[2];
	int ret;

	data[0] = reg;
	data[1] = value & 0x00ff;

	if (reg < ARRAY_SIZE(es8323_reg))
		es8323_reg[reg] = value;
	ret = codec->hw_write(codec->control_data, data, 2);
	if (ret == 2)
		return 0;
	if (ret < 0)
		return ret;
	else
		return -EIO;
}

static int es8323_reset(struct snd_soc_codec *codec)
{
	snd_soc_write(codec, ES8323_CONTROL1, 0x80);
	return snd_soc_write(codec, ES8323_CONTROL1, 0x00);
}

static const char *es8323_line_texts[] = {
	"Line 1", "Line 2", "PGA"
};

static const unsigned int es8323_line_values[] = {
	0, 1, 2, 3
};
static const char *es8323_pga_sell[] = { "Line 1L", "Line 2L", "NC", "DifferentialL" };
static const char *es8323_pga_selr[] = { "Line 1R", "Line 2R", "NC", "DifferentialR" };
static const char *es8323_lin_sell[] = {"Line 1L", "Line 2L","NC","MicL"};
static const char *es8323_lin_selr[] = {"Line 1R", "Line 2R","NC","MicR"};

static const char *stereo_3d_txt[] =
    { "No 3D  ", "Level 1", "Level 2", "Level 3", "Level 4", "Level 5",
"Level 6", "Level 7" };
static const char *alc_func_txt[] = { "Off", "Right", "Left", "Stereo" };
static const char *ng_type_txt[] = { "Constant PGA Gain", "Mute ADC Output" };
static const char *deemph_txt[] = { "None", "32Khz", "44.1Khz", "48Khz" };
static const char *adcpol_txt[] =
    { "Normal", "L Invert", "R Invert", "L + R Invert" };
static const char *es8323_mono_mux[] =
    { "Stereo", "Mono (Left)", "Mono (Right)" };
static const char *es8323_diff_sel[] = { "Line 1", "Line 2" };
static const char *es8323_ds_sel[] = { "reg11", "reg10 reg11" };

static SOC_ENUM_SINGLE_DECL(es8323_left_dac_enum, ES8323_ADCCONTROL2, 6, es8323_pga_sell);
static SOC_ENUM_SINGLE_DECL(es8323_right_dac_enum, ES8323_ADCCONTROL2, 4, es8323_pga_selr);
static SOC_ENUM_SINGLE_DECL(es8323_diffL_enum, ES8323_ADCCONTROL3, 7, es8323_diff_sel);
static SOC_ENUM_SINGLE_DECL(es8323_diffR_enum, ES8323_ADCCONTROL2, 2, es8323_diff_sel);
static SOC_ENUM_SINGLE_DECL(es8323_llin_enum, ES8323_DACCONTROL16, 3, es8323_lin_sell);
static SOC_ENUM_SINGLE_DECL(es8323_rlin_enum, ES8323_DACCONTROL16, 0, es8323_lin_selr);
static SOC_ENUM_SINGLE_DECL(es8323_mono_enum, ES8323_ADCCONTROL3, 3, es8323_mono_mux);



static const struct soc_enum es8323_enum[] = {
	SOC_VALUE_ENUM_SINGLE(ES8323_DACCONTROL16, 3, 7, ARRAY_SIZE(es8323_line_texts), es8323_line_texts, es8323_line_values),	/* LLINE */
	SOC_VALUE_ENUM_SINGLE(ES8323_DACCONTROL16, 0, 7, ARRAY_SIZE(es8323_line_texts), es8323_line_texts, es8323_line_values),	/* RLINE */
	SOC_VALUE_ENUM_SINGLE(ES8323_ADCCONTROL2, 6, 3, ARRAY_SIZE(es8323_pga_sell), es8323_line_texts, es8323_line_values),	/* Left PGA Mux */
	SOC_VALUE_ENUM_SINGLE(ES8323_ADCCONTROL2, 4, 3, ARRAY_SIZE(es8323_pga_sell), es8323_line_texts, es8323_line_values),	/* Right PGA Mux */
	SOC_ENUM_SINGLE(ES8323_DACCONTROL7, 2, 8, stereo_3d_txt),	/* stereo-3d */
	SOC_ENUM_SINGLE(ES8323_ADCCONTROL10, 6, 4, alc_func_txt),	/* alc func */
	SOC_ENUM_SINGLE(ES8323_ADCCONTROL14, 1, 2, ng_type_txt),	/* noise gate type */
	SOC_ENUM_SINGLE(ES8323_DACCONTROL6, 6, 4, deemph_txt),	/* Playback De-emphasis */
	SOC_ENUM_SINGLE(ES8323_ADCCONTROL6, 6, 4, adcpol_txt),
	SOC_ENUM_SINGLE(ES8323_ADCCONTROL3, 3, 3, es8323_mono_mux),
	SOC_ENUM_SINGLE(ES8323_ADCCONTROL3, 7, 2, es8323_diff_sel),
	SOC_ENUM_SINGLE(ES8323_ADCCONTROL2, 3, 2, es8323_ds_sel),
};

static const DECLARE_TLV_DB_SCALE(pga_tlv, 0, 300, 0);
static const DECLARE_TLV_DB_SCALE(adc_tlv, -9600, 50, 1);
static const DECLARE_TLV_DB_SCALE(dac_tlv, -9600, 50, 1);
static const DECLARE_TLV_DB_SCALE(out_tlv, -4500, 150, 0);
static const DECLARE_TLV_DB_SCALE(bypass_tlv, 0, 300, 0);
static const DECLARE_TLV_DB_SCALE(bypass_tlv2, -15, 300, 0);

static const struct snd_kcontrol_new es8323_left_dac_mux_controls =SOC_DAPM_ENUM("Route", es8323_left_dac_enum);
static const struct snd_kcontrol_new es8323_right_dac_mux_controls =SOC_DAPM_ENUM("Route", es8323_right_dac_enum);
static const struct snd_kcontrol_new es8323_diffmuxL_controls =SOC_DAPM_ENUM("Route2", es8323_diffL_enum);
static const struct snd_kcontrol_new es8323_diffmuxR_controls =SOC_DAPM_ENUM("Route2", es8323_diffR_enum);


static const struct snd_kcontrol_new es8323_snd_controls[] = {
	SOC_ENUM("3D Mode", es8323_enum[4]),
	SOC_SINGLE("ALC Capture Target Volume", ES8323_ADCCONTROL11, 4, 15, 0),
	SOC_SINGLE("ALC Capture Max PGA", ES8323_ADCCONTROL10, 3, 7, 0),
	SOC_SINGLE("ALC Capture Min PGA", ES8323_ADCCONTROL10, 0, 7, 0),
	SOC_ENUM("ALC Capture Function", es8323_enum[5]),
	SOC_SINGLE("ALC Capture ZC Switch", ES8323_ADCCONTROL13, 6, 1, 0),
	SOC_SINGLE("ALC Capture Hold Time", ES8323_ADCCONTROL11, 0, 15, 0),
	SOC_SINGLE("ALC Capture Decay Time", ES8323_ADCCONTROL12, 4, 15, 0),
	SOC_SINGLE("ALC Capture Attack Time", ES8323_ADCCONTROL12, 0, 15, 0),
	SOC_SINGLE("ALC Capture NG Threshold", ES8323_ADCCONTROL14, 3, 31, 0),
	SOC_ENUM("ALC Capture NG Type", es8323_enum[6]),
	SOC_SINGLE("ALC Capture NG Switch", ES8323_ADCCONTROL14, 0, 1, 0),
	SOC_SINGLE("ZC Timeout Switch", ES8323_ADCCONTROL13, 6, 1, 0),
	SOC_DOUBLE_R_TLV("Capture Digital Volume", ES8323_ADCCONTROL8,
			 ES8323_ADCCONTROL9, 0, 192, 1, adc_tlv),
	SOC_SINGLE("Capture Mute", ES8323_ADCCONTROL7, 2, 1, 0),
	SOC_SINGLE_TLV("Left Channel Capture Volume", ES8323_ADCCONTROL1, 4,8,
		       0, bypass_tlv),
	SOC_SINGLE_TLV("Right Channel Capture Volume", ES8323_ADCCONTROL1, 0,
		       8, 0, bypass_tlv),
	SOC_ENUM("Playback De-emphasis", es8323_enum[7]),
	SOC_ENUM("Capture Polarity", es8323_enum[8]),
	SOC_ENUM("Capture DSSEL", es8323_enum[11]),
	SOC_DOUBLE_R_TLV("PCM Volume", ES8323_DACCONTROL4, ES8323_DACCONTROL5,
			 0, 192, 1, dac_tlv),
	SOC_SINGLE_TLV("Left Mixer Left Bypass Volume", ES8323_DACCONTROL17, 3,
		       7, 1, bypass_tlv2),
	SOC_SINGLE_TLV("Right Mixer Right Bypass Volume", ES8323_DACCONTROL20,
		       3, 7, 1, bypass_tlv2),
	SOC_DOUBLE_R_TLV("Output 1 Playback Volume", ES8323_DACCONTROL24,
			 ES8323_DACCONTROL25, 0, 33, 0, out_tlv),
	SOC_DOUBLE_R_TLV("Output 2 Playback Volume", ES8323_DACCONTROL26,
			 ES8323_DACCONTROL27, 0, 33, 0, out_tlv),
};

static const struct snd_kcontrol_new es8323_left_line_controls =
SOC_DAPM_ENUM("LLIN Mux", es8323_llin_enum);

static const struct snd_kcontrol_new es8323_right_line_controls =
SOC_DAPM_ENUM("RLIN Mux", es8323_rlin_enum);
/* Mono ADC Mux */
static const struct snd_kcontrol_new es8323_monomux_controls =
SOC_DAPM_ENUM("Mono Mux", es8323_mono_enum);
/* Left PGA Mux */
static const struct snd_kcontrol_new es8323_left_pga_controls =
SOC_DAPM_ENUM("Route", es8323_enum[2]);

/* Right PGA Mux */
//static const struct snd_kcontrol_new es8323_right_pga_controls =
//SOC_DAPM_ENUM("Route", es8323_enum[3]);

/* Left Mixer */
static const struct snd_kcontrol_new es8323_left_mixer_controls[] = {
	SOC_DAPM_SINGLE("Left Playback Switch", ES8323_DACCONTROL17, 7, 1, 0),
	SOC_DAPM_SINGLE("Left Bypass Switch", ES8323_DACCONTROL17, 6, 1, 0),
};

/* Right Mixer */
static const struct snd_kcontrol_new es8323_right_mixer_controls[] = {
	SOC_DAPM_SINGLE("Right Playback Switch", ES8323_DACCONTROL20, 7, 1, 0),
	SOC_DAPM_SINGLE("Right Bypass Switch", ES8323_DACCONTROL20, 6, 1, 0),
};

/* Differential Mux */
//static const struct snd_kcontrol_new es8323_diffmux_controls =
//SOC_DAPM_ENUM("Route", es8323_enum[10]);

static const struct snd_soc_dapm_widget es8323_dapm_widgets[] = {
	SND_SOC_DAPM_INPUT("LINPUT1"),
	SND_SOC_DAPM_INPUT("LINPUT-RINPUT"),
	SND_SOC_DAPM_INPUT("LINPUT2"),
	SND_SOC_DAPM_INPUT("RINPUT1"),
	SND_SOC_DAPM_INPUT("RINPUT2"),
	SND_SOC_DAPM_MUX("Left PGA Mux", SND_SOC_NOPM, 0, 0,
			 &es8323_left_dac_mux_controls),
	SND_SOC_DAPM_MUX("Right PGA Mux",SND_SOC_NOPM , 0, 0,
			 &es8323_right_dac_mux_controls),
	SND_SOC_DAPM_MICBIAS("Mic Bias", ES8323_ADCPOWER, 3, 1),

	SND_SOC_DAPM_MUX("Differential MuxL", SND_SOC_NOPM, 0, 0,
			 &es8323_diffmuxL_controls),
	SND_SOC_DAPM_MUX("Differential MuxR", SND_SOC_NOPM, 0, 0,
			 &es8323_diffmuxR_controls),

	SND_SOC_DAPM_MUX("Left ADC Mux", SND_SOC_NOPM, 0, 0,
			 &es8323_monomux_controls),
	SND_SOC_DAPM_MUX("Right ADC Mux", SND_SOC_NOPM, 0, 0,
			 &es8323_monomux_controls),

	//SND_SOC_DAPM_MUX("Left PGA Mux", ES8323_ADCPOWER, 7, 1,
	//		 &es8323_left_pga_controls),


	SND_SOC_DAPM_MUX("Left Line Mux", SND_SOC_NOPM, 0, 0,
			 &es8323_left_line_controls),
	SND_SOC_DAPM_MUX("Right Line Mux", SND_SOC_NOPM, 0, 0,
			 &es8323_right_line_controls),

	SND_SOC_DAPM_ADC("Right ADC", "Right Capture", ES8323_ADCPOWER, 4, 1),
	SND_SOC_DAPM_ADC("Left ADC", "Left Capture", ES8323_ADCPOWER, 5, 1),

	/* gModify.Cmmt Implement when suspend/startup */
	SND_SOC_DAPM_DAC("Right DAC", "Right Playback", ES8323_DACPOWER, 6, 1),
	SND_SOC_DAPM_DAC("Left DAC", "Left Playback", ES8323_DACPOWER, 7, 1),

	SND_SOC_DAPM_MIXER("Left Mixer", SND_SOC_NOPM, 0, 0,
			   &es8323_left_mixer_controls[0],
			   ARRAY_SIZE(es8323_left_mixer_controls)),
	SND_SOC_DAPM_MIXER("Right Mixer", SND_SOC_NOPM, 0, 0,
			   &es8323_right_mixer_controls[0],
			   ARRAY_SIZE(es8323_right_mixer_controls)),
	SND_SOC_DAPM_PGA("Right ADC Power", ES8323_ADCPOWER, 6, 1, NULL, 0),
	SND_SOC_DAPM_PGA("Left ADC Power", ES8323_ADCPOWER, 7, 1, NULL, 0),
	SND_SOC_DAPM_PGA("Right Out 2", ES8323_DACPOWER, 2, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Left Out 2", ES8323_DACPOWER, 3, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Right Out 1", ES8323_DACPOWER, 4, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Left Out 1", ES8323_DACPOWER, 5, 0, NULL, 0),
	SND_SOC_DAPM_PGA("LAMP", ES8323_ADCCONTROL1, 4, 0, NULL, 0),
	SND_SOC_DAPM_PGA("RAMP", ES8323_ADCCONTROL1, 0, 0, NULL, 0),

	SND_SOC_DAPM_OUTPUT("LOUT1"),
	SND_SOC_DAPM_OUTPUT("ROUT1"),
	SND_SOC_DAPM_OUTPUT("LOUT2"),
	SND_SOC_DAPM_OUTPUT("ROUT2"),
	SND_SOC_DAPM_OUTPUT("VREF"),
};

static const struct snd_soc_dapm_route audio_map[] = {
	{"Left PGA Mux", "Line 1L", "LINPUT1"},
	{"Left PGA Mux", "Line 1L", "LINPUT-RINPUT"},
	{"Left PGA Mux", "Line 2L", "LINPUT2"},
	{"Left PGA Mux", "Line 2L", "LINPUT-RINPUT"},
	{"Left PGA Mux", "DifferentialL", "Differential MuxL"},

	{"Right PGA Mux", "Line 1R", "RINPUT1"},
	{"Right PGA Mux", "Line 1R", "LINPUT-RINPUT"},
	{"Right PGA Mux", "Line 2R", "RINPUT2"},
	{"Right PGA Mux", "Line 2R", "LINPUT-RINPUT"},
	{"Right PGA Mux", "DifferentialR", "Differential MuxR"},

	{"Differential MuxL", "Line 1", "LINPUT1"},
	{"Differential MuxL", "Line 1", "RINPUT1"},
	{"Differential MuxL", "Line 1", "LINPUT-RINPUT"},
	{"Differential MuxL", "Line 2", "LINPUT2"},
	{"Differential MuxL", "Line 2", "RINPUT2"},
	{"Differential MuxL", "Line 2", "LINPUT-RINPUT"},

	{"Differential MuxR", "Line 1", "LINPUT1"},
	{"Differential MuxR", "Line 1", "RINPUT1"},
	{"Differential MuxR", "Line 1", "LINPUT-RINPUT"},
	{"Differential MuxR", "Line 2", "LINPUT2"},
	{"Differential MuxR", "Line 2", "RINPUT2"},
	{"Differential MuxR", "Line 2", "LINPUT-RINPUT"},

	{"Left ADC Mux", "Stereo", "Right PGA Mux"},
	{"Left ADC Mux", "Stereo", "Left PGA Mux"},
	{"Left ADC Mux", "Mono (Left)", "Left PGA Mux"},

	{"Right ADC Mux", "Stereo", "Left PGA Mux"},
	{"Right ADC Mux", "Stereo", "Right PGA Mux"},
	{"Right ADC Mux", "Mono (Right)", "Right PGA Mux"},

	{"Left ADC Power",NULL,"Left ADC Mux"},
	{"Right ADC Power", NULL,"Right ADC Mux"},
	{"Left ADC", NULL, "Left ADC Power"},
	{"Right ADC", NULL, "Right ADC Power"},

	{"Left Line Mux", "Line 1L", "LINPUT1"},
	{"Left Line Mux", "Line 2L", "LINPUT2"},
	{"Left Line Mux", "MicL", "Left PGA Mux"},

	{"Right Line Mux", "Line 1R", "RINPUT1"},
	{"Right Line Mux", "Line 2R", "RINPUT2"},
	{"Right Line Mux", "MicR", "Right PGA Mux"},

	{"Left Mixer", "Left Playback Switch", "Left DAC"},
	{"Left Mixer", "Left Bypass Switch", "Left Line Mux"},

	{"Right Mixer", "Right Playback Switch", "Right DAC"},
	{"Right Mixer", "Right Bypass Switch", "Right Line Mux"},

	{"Left Out 1", NULL, "Left Mixer"},
	{"LOUT1", NULL, "Left Out 1"},
	{"Right Out 1", NULL, "Right Mixer"},
	{"ROUT1", NULL, "Right Out 1"},

	{"Left Out 2", NULL, "Left Mixer"},
	{"LOUT2", NULL, "Left Out 2"},
	{"Right Out 2", NULL, "Right Mixer"},
	{"ROUT2", NULL, "Right Out 2"},

};
struct _coeff_div {
	u32 mclk;
	u32 rate;
	u16 fs;
	u8 sr:4;
	u8 usb:1;
};

/* codec hifi mclk clock divider coefficients */
#if 0 /* codec is master*/
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
static int es8323_set_dai_sysclk(struct snd_soc_dai *codec_dai,
				 int clk_id, unsigned int freq, int dir)
{

	struct snd_soc_codec *codec = codec_dai->codec;
	struct es8323_priv *es8323 = snd_soc_codec_get_drvdata(codec);

	switch (freq) {
	case 11289600:
	case 18432000:
	case 22579200:
	case 36864000:
		es8323->sysclk_constraints = &constraints_112896;
		es8323->sysclk = freq;
		return 0;

	case 12288000:
	case 16934400:
	case 24576000:
	case 33868800:
		es8323->sysclk_constraints = &constraints_12288;
		es8323->sysclk = freq;
		return 0;

	case 12000000:
	case 24000000:
		es8323->sysclk_constraints = &constraints_12;
		es8323->sysclk = freq;
		return 0;
	}

	return -EINVAL;

}

static int es8323_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	u8 iface = 0;
	u8 adciface = 0;
	u8 daciface = 0;

	/* set master/slave audio interface */
	iface = snd_soc_read(codec, ES8323_IFACE);
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:	/* MASTER MODE */
		iface |= ES8323_MASTERMODE_MSC;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:	/* SLAVE MODE */
		iface &= ~ES8323_MASTERMODE_MSC;
		break;
	default:
		return -EINVAL;
	}

	/* interface format */
	adciface = snd_soc_read(codec, ES8323_ADC_IFACE);
	daciface = snd_soc_read(codec, ES8323_DAC_IFACE);
	adciface &= ~ES8323_ADCCONTROL4_ADCFORMAT_MASK;
	daciface &= ~ES8323_DACCONTROL1_DACFORMAT_MASK;
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		adciface |= ES8323_ADCCONTROL4_ADCFORMAT_I2S;
		daciface |= ES8323_DACCONTROL1_DACFORMAT_I2S;
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
		iface		|= ES8323_MASTERMODE_BCLK_NORMAL;
		adciface	|= ES8323_ADCCONTROL4_ADCLRP_I2S_POL_NORMAL;
		daciface	|= ES8323_DACCONTROL1_DACLRP_I2S_POL_NORMAL;
		break;
	case SND_SOC_DAIFMT_IB_IF:
		iface		|= ES8323_MASTERMODE_BCLK_INV;
		adciface	|= ES8323_ADCCONTROL4_ADCLRP_I2S_POL_INV;
		daciface	|= ES8323_DACCONTROL1_DACLRP_I2S_POL_INV;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		iface		|= ES8323_MASTERMODE_BCLK_INV;
		adciface	|= ES8323_ADCCONTROL4_ADCLRP_I2S_POL_NORMAL;
		daciface	|= ES8323_DACCONTROL1_DACLRP_I2S_POL_NORMAL;
		break;
	case SND_SOC_DAIFMT_NB_IF:
		iface		|= ES8323_MASTERMODE_BCLK_NORMAL;
		adciface	|= ES8323_ADCCONTROL4_ADCLRP_I2S_POL_INV;
		daciface	|= ES8323_DACCONTROL1_DACLRP_I2S_POL_INV;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_write(codec, ES8323_IFACE, iface);
	snd_soc_write(codec, ES8323_ADC_IFACE, adciface);
	snd_soc_write(codec, ES8323_DAC_IFACE, daciface);

	return 0;
}

static int es8323_pcm_startup(struct snd_pcm_substream *substream,
			      struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct es8323_priv *es8323 = snd_soc_codec_get_drvdata(codec);

	if(es8323->sysclk_constraints && es8323->sysclk) {
		snd_pcm_hw_constraint_list(substream->runtime, 0,
					SNDRV_PCM_HW_PARAM_RATE, es8323->sysclk_constraints);
	}

	return 0;
}

static int es8323_pcm_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	struct es8323_priv *es8323 = snd_soc_codec_get_drvdata(codec);
	u16 adciface = 0;
	u16 daciface = 0;
	int coeff;
	u16 srate = snd_soc_read(codec, ES8323_IFACE);

	coeff = get_coeff(es8323->sysclk, params_rate(params));
	if (coeff < 0) {
		dev_err(codec->dev,
			"Unable to configure sample rate %dHz with %dHz MCLK\n",
			params_rate(params), es8323->sysclk);
		return coeff;
	}

	/* bit size */
	adciface = snd_soc_read(codec, ES8323_ADC_IFACE);
	daciface = snd_soc_read(codec, ES8323_DAC_IFACE);
	adciface &= ~ES8323_ADCCONTROL4_ADCWL_MASK;
	daciface &= ~ES8323_DACCONTROL1_DACWL_MASK;
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		adciface |= (ADCWL_16BIT << ES8323_ADCCONTROL4_ADCWL_SHIFT);
		daciface |= (DACWL_16BIT << ES8323_DACCONTROL1_DACWL_SHIFT);
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		adciface |= (ADCWL_20BIT << ES8323_ADCCONTROL4_ADCWL_SHIFT);
		daciface |= (DACWL_20BIT << ES8323_DACCONTROL1_DACWL_SHIFT);
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		adciface |= (ADCWL_24BIT << ES8323_ADCCONTROL4_ADCWL_SHIFT);
		daciface |= (DACWL_24BIT << ES8323_DACCONTROL1_DACWL_SHIFT);
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		adciface |= (ADCWL_32BIT << ES8323_ADCCONTROL4_ADCWL_SHIFT);
		daciface |= (DACWL_32BIT << ES8323_DACCONTROL1_DACWL_SHIFT);
		break;
	}

	/* set iface & srate */
	snd_soc_write(codec, ES8323_DAC_IFACE, daciface);
	snd_soc_write(codec, ES8323_ADC_IFACE, adciface);

	if (coeff >= 0) {
		snd_soc_write(codec, ES8323_IFACE, srate);
		snd_soc_write(codec, ES8323_ADCCONTROL5,
					coeff_div[coeff].sr | (coeff_div[coeff].usb) << 5);
		snd_soc_write(codec, ES8323_DACCONTROL2,
					coeff_div[coeff].sr | (coeff_div[coeff].usb) << 5);
	}

	return 0;
}

static int es8323_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	struct es8323_priv *es8323 = snd_soc_codec_get_drvdata(codec);

	pr_err("%s ****** %d, mute = %d\n", __func__, __LINE__, mute);

	es8323->muted = mute;
	if (mute) {
		//es8323_set_gpio(ES8323_CODEC_SET_SPK, !es8323->spk_gpio_level);
		usleep_range(18000, 20000);
		snd_soc_write(codec, ES8323_DACCONTROL3, ES8323_DACCONTROL3_DACMUTE);
	} else {
		snd_soc_write(codec, ES8323_DACCONTROL3, ES8323_DACCONTROL3_DACUNMUTE);
		snd_soc_write(codec, ES8323_DACCONTROL26, ES8323_DEF_VOL);
		snd_soc_write(codec, ES8323_DACCONTROL27, ES8323_DEF_VOL);
		msleep(50);
		//if (!es8323->hp_inserted)
		//	es8323_set_gpio(ES8323_CODEC_SET_SPK, es8323->spk_gpio_level);
		usleep_range(18000, 20000);
	}
	return 0;
}

static int es8323_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
	struct es8323_priv *es8323 = snd_soc_codec_get_drvdata(codec);
	int ret;

	switch (level) {
	case SND_SOC_BIAS_ON:
		dev_dbg(codec->dev, "%s on\n", __func__);
		break;
	case SND_SOC_BIAS_PREPARE:
		dev_dbg(codec->dev, "%s prepare\n", __func__);
		if (IS_ERR(es8323->mclk))
			break;
		if (snd_soc_codec_get_bias_level(codec) == SND_SOC_BIAS_ON) {
			clk_disable_unprepare(es8323->mclk);
		} else {
			ret = clk_prepare_enable(es8323->mclk);
			if (ret)
				return ret;
		}
		snd_soc_write(codec, ES8323_ANAVOLMANAG, 0x7C);
		snd_soc_write(codec, ES8323_CHIPLOPOW1, 0x00);
		snd_soc_write(codec, ES8323_CHIPLOPOW2, 0x00);
		snd_soc_write(codec, ES8323_CHIPPOWER, 0x00);
		snd_soc_write(codec, ES8323_ADCPOWER, 0x19);
		break;
	case SND_SOC_BIAS_STANDBY:
		dev_dbg(codec->dev, "%s standby\n", __func__);
		snd_soc_write(codec, ES8323_ANAVOLMANAG, 0x7C);
		snd_soc_write(codec, ES8323_CHIPLOPOW1, 0x00);
		snd_soc_write(codec, ES8323_CHIPLOPOW2, 0x00);
		snd_soc_write(codec, ES8323_CHIPPOWER, 0x00);
		snd_soc_write(codec, ES8323_ADCPOWER, 0x19);
		break;
	case SND_SOC_BIAS_OFF:
		if (es8323->mclk)
			clk_disable_unprepare(es8323->mclk);
		dev_dbg(codec->dev, "%s off\n", __func__);
		snd_soc_write(codec, ES8323_ADCPOWER, 0xFF);
		snd_soc_write(codec, ES8323_DACPOWER, 0xC0);
		snd_soc_write(codec, ES8323_CHIPLOPOW1, 0xFF);
		snd_soc_write(codec, ES8323_CHIPLOPOW2, 0xFF);
		snd_soc_write(codec, ES8323_CHIPPOWER, 0xFF);
		snd_soc_write(codec, ES8323_ANAVOLMANAG, 0x7B);
		break;
	}
		return 0;
}

static struct snd_soc_dai_ops es8323_ops = {
	.startup		= es8323_pcm_startup,
	.hw_params		= es8323_pcm_hw_params,
	.set_fmt		= es8323_set_dai_fmt,
	.set_sysclk		= es8323_set_dai_sysclk,
	.digital_mute	= es8323_mute,
};

static struct snd_soc_dai_driver es8323_dai = {
	.name = "ES8323 HiFi",
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
	.ops = &es8323_ops,
	.symmetric_rates = 1,
};

static int es8323_suspend(struct snd_soc_codec *codec)
{
	pr_err("%s ****** %d\n", __func__, __LINE__);

	snd_soc_write(codec, ES8323_DACCONTROL3, 0x06);
	snd_soc_write(codec, ES8323_DACCONTROL26, 0x00);
	snd_soc_write(codec, ES8323_DACCONTROL27, 0x00);
	snd_soc_write(codec, ES8323_ADCPOWER, 0xFF);
	snd_soc_write(codec, ES8323_DACPOWER, 0xc0);
	snd_soc_write(codec, ES8323_CHIPPOWER, 0xF3);
	snd_soc_write(codec, ES8323_CONTROL1, 0x00);
	snd_soc_write(codec, ES8323_CONTROL2, 0x58);
	snd_soc_write(codec, ES8323_DACCONTROL21, 0x9c);
	usleep_range(18000, 20000);
	return 0;
}

static int es8323_resume(struct snd_soc_codec *codec)
{
	pr_err("%s ****** %d\n", __func__, __LINE__);

	snd_soc_write(codec, ES8323_DACCONTROL21, 0x80);
	snd_soc_write(codec, ES8323_CONTROL2, 0x50);
	snd_soc_write(codec, ES8323_CONTROL1, 0x32);
	snd_soc_write(codec, ES8323_CHIPPOWER, 0x00);
	snd_soc_write(codec, ES8323_DACPOWER, 0x0c);
	snd_soc_write(codec, ES8323_ADCPOWER, 0x59);
	snd_soc_write(codec, ES8323_DACCONTROL27, ES8323_DEF_VOL);
	snd_soc_write(codec, ES8323_DACCONTROL26, ES8323_DEF_VOL);
	snd_soc_write(codec, ES8323_DACCONTROL3, 0x02);
	return 0;
}

static int es8323_probe(struct snd_soc_codec *codec)
{
	int ret = 0;

	if (codec == NULL) {
		dev_err(codec->dev, "Codec device not registered\n");
		return -ENODEV;
	}

	codec->hw_write = (hw_write_t) i2c_master_send;
	codec->control_data = container_of(codec->dev, struct i2c_client, dev);

	es8323_codec = codec;
	ret = es8323_reset(codec);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to issue reset\n");
		return ret;
	}

	snd_soc_write(codec, ES8323_CONTROL2,0x60);
	snd_soc_write(codec, ES8323_CHIPPOWER, 0xF3);
	snd_soc_write(codec, ES8323_CHIPPOWER, 0xF0);
	snd_soc_write(codec, ES8323_DACCONTROL21,0x80);
	snd_soc_write(codec, ES8323_CONTROL1, 0x36);
	snd_soc_write(codec, ES8323_MASTERMODE, 0x00);
	snd_soc_write(codec, ES8323_DACPOWER, 0x00);
	snd_soc_write(codec, ES8323_CHIPLOPOW2, 0xC3);
	snd_soc_write(codec, ES8323_DACCONTROL3, 0x02);
	snd_soc_write(codec, ES8323_ADCCONTROL1, 0x88);
	snd_soc_write(codec, ES8323_ADCCONTROL2, 0xfc);
	snd_soc_write(codec, ES8323_ADCCONTROL3, 0x02);
	snd_soc_write(codec, ES8323_ADCCONTROL4, 0x0C);
	snd_soc_write(codec, ES8323_ADCCONTROL5, 0x02);
	snd_soc_write(codec, ES8323_ADCCONTROL8, 0x00);
	snd_soc_write(codec, ES8323_ADCCONTROL9, 0x00);
	snd_soc_write(codec, ES8323_DACCONTROL1, 0x18);
	snd_soc_write(codec, ES8323_DACCONTROL2, 0x02);
	snd_soc_write(codec, ES8323_DACCONTROL4, 0x00);
	snd_soc_write(codec, ES8323_DACCONTROL5, 0x00);
	snd_soc_write(codec, ES8323_DACCONTROL17, 0xB8);
	snd_soc_write(codec, ES8323_DACCONTROL20, 0xB8);
	snd_soc_write(codec, 0x35, 0xA0);
	usleep_range(18000, 20000);
	snd_soc_write(codec, ES8323_DACCONTROL24, 0x1E);
	snd_soc_write(codec, ES8323_DACCONTROL25, 0x1E);
	snd_soc_write(codec, ES8323_DACCONTROL26, 0x1E);
	snd_soc_write(codec, ES8323_DACCONTROL27, 0x1E);
	snd_soc_write(codec, ES8323_ADCPOWER, 0x09);
	snd_soc_write(codec, ES8323_CHIPPOWER, 0x00);
	usleep_range(18000, 20000);
	snd_soc_write(codec, ES8323_DACPOWER, 0x3C);

	es8323_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	return 0;
}

static int es8323_remove(struct snd_soc_codec *codec)
{
	es8323_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_es8323 = {
	.probe = es8323_probe,
	.remove = es8323_remove,
	.suspend = es8323_suspend,
	.resume = es8323_resume,
	.set_bias_level = es8323_set_bias_level,
	.reg_cache_size = ARRAY_SIZE(es8323_reg),
	.reg_word_size = sizeof(u16),
	.reg_cache_default = es8323_reg,
	.reg_cache_step = 1,
	.read = es8323_read_reg_cache,
	.write = es8323_write,

	.component_driver = {
		.dapm_widgets		= es8323_dapm_widgets,
		.num_dapm_widgets	= ARRAY_SIZE(es8323_dapm_widgets),
		.dapm_routes		= audio_map,
		.num_dapm_routes	= ARRAY_SIZE(audio_map),
		.controls			= es8323_snd_controls,
		.num_controls		= ARRAY_SIZE(es8323_snd_controls),
	},
};

static int es8323_parse_dt(struct device *dev,
				struct es8323_priv *es8323)
{
	es8323->ce_gpio = devm_gpiod_get(dev, "ce", GPIOD_OUT_HIGH);
	if (!es8323->ce_gpio) {
		dev_err(dev, "devm_gpiod_get failed.\n");
		return -1;
	}

	gpiod_set_value(es8323->ce_gpio, 1);

	return 0;
}

enum reg_sysfs_cmd {
	SYSFS_CMD_READ_REG    = 0,
	SYSFS_CMD_WRITE_REG   = 1,
	SYSFS_CMD_MAX
};
struct es8323_sysfs_cmd {
	enum reg_sysfs_cmd cmd;
	const char *key_str;
};

static ssize_t es8323_show(struct device *dev,
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
		val = snd_soc_read(es8323_codec, i);
		off += sprintf(buf + off, "[%02d][0x%02x] 0x%x\n", i, i, val);
	}
	off += sprintf(buf + off, "------dump regs end	------\n");

	return off;

}

static ssize_t es8323_set(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	char *p = NULL;
	char *endp = NULL;
	bool found = false;
	int i;
	u32 op_reg, rd_val = 0, wr_val = 0;

	struct es8323_sysfs_cmd cmd_arr[] = {
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
		rd_val = snd_soc_read(es8323_codec, op_reg);
		pr_err("\nrd reg[%02d][0x%02x] 0x%x\n", op_reg, op_reg, rd_val);
		break;

	case SYSFS_CMD_WRITE_REG:
		if (endp == NULL) {
			pr_err("not enough para, wr_val needed\n");
			return count;
		}

		wr_val = (u32)simple_strtoul(endp + 1, &endp, 0);
		snd_soc_write(es8323_codec, op_reg, wr_val);
		pr_err("\nwr reg [%02d][0x%02x], val = 0x%x\n", op_reg, op_reg, wr_val);
		break;
	default:
		break;

	}

	return count;
}

static DEVICE_ATTR(es8323_test, S_IWUSR | S_IRUSR,
			es8323_show, es8323_set);

static struct attribute *es8323_attributes[] = {
	&dev_attr_es8323_test.attr,
	NULL
};

static const struct attribute_group es8323_attr_group = {
	.attrs = es8323_attributes,
};


static int es8323_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct es8323_priv *es8323;
	int ret = -1;
	struct i2c_adapter *adapter = to_i2c_adapter(i2c->dev.parent);
	//enum of_gpio_flags flags;
	//char reg;

	if (!i2c_check_functionality(adapter, I2C_FUNC_I2C)) {
		dev_warn(&adapter->dev,
			 "I2C-Adapter doesn't support I2C_FUNC_I2C\n");
		return -EIO;
	}

	es8323 = devm_kzalloc(&i2c->dev, sizeof(struct es8323_priv), GFP_KERNEL);
	if (es8323 == NULL)
		return -ENOMEM;

	es8323_parse_dt(&i2c->dev, es8323);
	i2c_set_clientdata(i2c, es8323);
	/*
	reg = ES8323_DACCONTROL18;
	ret = i2c_master_recv(i2c, &reg, 1);
	if (ret < 0) {
		dev_err(&i2c->dev, "i2c recv Failed\n");
		return ret;
	}
	*/
	//es8323->spk_ctl_gpio = of_get_named_gpio_flags(i2c->dev.of_node, "spk-con-gpio", 0, &flags);
	//es8323->hp_det_gpio = of_get_named_gpio_flags(i2c->dev.of_node, "hp-det-gpio", 0, &flags);
	es8323_private = es8323;

	ret = sysfs_create_group(&i2c->dev.kobj, &es8323_attr_group);
	if (ret < 0)
		dev_warn(&i2c->dev, "attr group create failed\n");
	else
		dev_info(&i2c->dev, "attr group create success!\n");


	ret = snd_soc_register_codec(&i2c->dev,
								&soc_codec_dev_es8323, &es8323_dai, 1);
	return ret;
}

static int es8323_i2c_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client->dev.kobj, &es8323_attr_group);

	snd_soc_unregister_codec(&client->dev);
	return 0;
}

static const struct i2c_device_id es8323_i2c_id[] = {
	{"es8323", 0},
	{"es8388", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, es8323_i2c_id);

void es8323_i2c_shutdown(struct i2c_client *client)
{
//	struct es8323_priv *es8323 = es8323_private;

	//es8323_set_gpio(ES8323_CODEC_SET_SPK, !es8323->spk_gpio_level);
	mdelay(20);
	snd_soc_write(es8323_codec, ES8323_CONTROL2, 0x58);
	snd_soc_write(es8323_codec, ES8323_CONTROL1, 0x32);
	snd_soc_write(es8323_codec, ES8323_CHIPPOWER, 0xf3);
	snd_soc_write(es8323_codec, ES8323_DACPOWER, 0xc0);
	mdelay(50);
	snd_soc_write(es8323_codec, ES8323_DACCONTROL26, 0x00);
	snd_soc_write(es8323_codec, ES8323_DACCONTROL27, 0x00);
	mdelay(50);
	snd_soc_write(es8323_codec, ES8323_CONTROL1, 0x30);
	snd_soc_write(es8323_codec, ES8323_CONTROL1, 0x34);
}

static const struct of_device_id es8323_of_match[] = {
	{ .compatible = "everest,es8323", },
	{ .compatible = "everest,es8388", },
	{ }
};
MODULE_DEVICE_TABLE(of, es8323_of_match);

static struct i2c_driver es8323_i2c_driver = {
	.driver = {
		.name = "ES8323",
		.of_match_table = of_match_ptr(es8323_of_match),
	},
	.shutdown	= es8323_i2c_shutdown,
	.probe		= es8323_i2c_probe,
	.remove		= es8323_i2c_remove,
	.id_table	= es8323_i2c_id,
};
module_i2c_driver(es8323_i2c_driver);

MODULE_DESCRIPTION("ASoC ES8328 audio CODEC I2C driver");
MODULE_AUTHOR("Sean Cross <xobs@kosagi.com>");
MODULE_LICENSE("GPL");
