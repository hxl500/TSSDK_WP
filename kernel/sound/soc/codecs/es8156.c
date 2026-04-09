/*
 * es8156.c -- es8156 ALSA SoC audio driver
 * Copyright Everest Semiconductor Co.,Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/clk.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
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
#include <linux/regmap.h>
#include "es8156.h"

#define es8156_DEF_VOL			0xBF

static struct snd_soc_codec *es8156_codec;

static const struct reg_default es8156_reg_defaults[] = {
	{0x00, 0x1c}, {0x01, 0x20}, {0x02, 0x00}, {0x03, 0x01},
	{0x04, 0x00}, {0x05, 0x04}, {0x06, 0x11}, {0x07, 0x00},
	{0x08, 0x06}, {0x09, 0x00}, {0x0a, 0x50}, {0x0b, 0x50},
	{0x0c, 0x00}, {0x0d, 0x10}, {0x10, 0x40}, {0x10, 0x40},
	{0x11, 0x00}, {0x12, 0x04}, {0x13, 0x11}, {0x14, 0xbf},
	{0x15, 0x00}, {0x16, 0x00}, {0x17, 0xf7}, {0x18, 0x00},
	{0x19, 0x20}, {0x1a, 0x00}, {0x20, 0x16}, {0x21, 0x7f},
	{0x22, 0x00}, {0x23, 0x86}, {0x24, 0x00}, {0x25, 0x07},
	{0xfc, 0x00}, {0xfd, 0x81}, {0xfe, 0x55}, {0xff, 0x10},
};

/* codec private data */
struct es8156_priv {
	struct regmap *regmap;
	unsigned int dmic_amic;
	unsigned int sysclk;
	struct snd_pcm_hw_constraint_list *sysclk_constraints;
	struct clk *mclk;
	int debounce_time;
	int hp_det_invert;
	struct delayed_work work;

	int spk_ctl_gpio;
	int hp_det_gpio;
	bool muted;
	bool hp_inserted;
	bool spk_active_level;

	int pwr_count;
};

/*
 * es8156_reset
 */
static int es8156_reset(struct snd_soc_codec *codec)
{
	snd_soc_write(codec, ES8156_RESET_REG00, 0x1c);
	usleep_range(5000, 5500);

	return snd_soc_write(codec, ES8156_RESET_REG00, 0x03);
}

static const DECLARE_TLV_DB_SCALE(dac_vol_tlv, -9600, 50, 1);

static const struct snd_kcontrol_new es8156_snd_controls[] = {
	/* DAC Digital controls */
	SOC_SINGLE_TLV("DAC Playback Volume", ES8156_VOLUME_CONTROL_REG14,
			  0, 0xff, 0, dac_vol_tlv),
};

static int es8156_set_dai_fmt(struct snd_soc_dai *codec_dai,
			      unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	/* set master/slave audio interface */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM://es8156 master
		snd_soc_update_bits(codec, ES8156_SCLK_MODE_REG02, 0x01,0x01);
		break;
	case SND_SOC_DAIFMT_CBS_CFS://es8156 slave
		snd_soc_update_bits(codec, ES8156_SCLK_MODE_REG02, 0x01,0x00);
		break;
	default:
		return -EINVAL;
	}
	/* interface format */

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		snd_soc_update_bits(codec, ES8156_DAC_SDP_REG11, 0x07,0x00);
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		return -EINVAL;
	case SND_SOC_DAIFMT_LEFT_J:
		snd_soc_update_bits(codec, ES8156_DAC_SDP_REG11, 0x07,0x01);
		break;
	case SND_SOC_DAIFMT_DSP_A:
		snd_soc_update_bits(codec, ES8156_DAC_SDP_REG11, 0x07,0x03);
		break;
	case SND_SOC_DAIFMT_DSP_B:
		snd_soc_update_bits(codec, ES8156_DAC_SDP_REG11, 0x07,0x07);
		break;
	default:
		return -EINVAL;
	}

	/* clock inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		snd_soc_update_bits(codec, ES8156_SCLK_MODE_REG02, (1<<4),0x00);
		break;
	case SND_SOC_DAIFMT_IB_IF:
		snd_soc_update_bits(codec, ES8156_SCLK_MODE_REG02, (1<<4),0x01);
		break;
	case SND_SOC_DAIFMT_IB_NF:
		snd_soc_update_bits(codec, ES8156_SCLK_MODE_REG02, (1<<4),0x01);
		break;
	case SND_SOC_DAIFMT_NB_IF:
		snd_soc_update_bits(codec, ES8156_SCLK_MODE_REG02, (1<<4),0x00);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int es8156_pcm_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		snd_soc_update_bits(codec, ES8156_DAC_SDP_REG11, 0x70,0x30);
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		snd_soc_update_bits(codec, ES8156_DAC_SDP_REG11, 0x70,0x10);
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		snd_soc_update_bits(codec, ES8156_DAC_SDP_REG11, 0x70,0x00);
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		snd_soc_update_bits(codec, ES8156_DAC_SDP_REG11, 0x70,0x40);
		break;
	}
	return 0;
}

static int es8156_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	struct es8156_priv *es8156 = snd_soc_codec_get_drvdata(codec);
	es8156->muted = mute;
	if (mute) {
		msleep(100);
		snd_soc_update_bits(codec, ES8156_DAC_MUTE_REG13, 0xff,0xff);
	} else if (dai->playback_active) {
		snd_soc_update_bits(codec, ES8156_DAC_MUTE_REG13, 0xff,0x00);
	}
	return 0;
}


static int es8156_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
	pr_err("%s *** %d ****level = %d\n", __func__, __LINE__, level);

	switch (level) {
	case SND_SOC_BIAS_ON:
		break;
	case SND_SOC_BIAS_PREPARE:
		break;
	case SND_SOC_BIAS_STANDBY:
		snd_soc_write(codec, ES8156_SCLK_MODE_REG02,0x04);
		snd_soc_write(codec, ES8156_ANALOG_SYS1_REG20,0x2A);
		snd_soc_write(codec, ES8156_ANALOG_SYS2_REG21,0x3C);
		snd_soc_write(codec, ES8156_ANALOG_SYS3_REG22,0x08);
		snd_soc_write(codec, ES8156_ANALOG_LP_REG24,0x07);
		snd_soc_write(codec, ES8156_ANALOG_SYS4_REG23,0x00);
		snd_soc_write(codec, ES8156_TIME_CONTROL1_REG0A,0x01);
		snd_soc_write(codec, ES8156_TIME_CONTROL2_REG0B,0x01);
		snd_soc_write(codec, ES8156_VOLUME_CONTROL_REG14,0xBF);
		snd_soc_write(codec, ES8156_MAINCLOCK_CTL_REG01,0x21);
		snd_soc_write(codec, ES8156_P2S_CONTROL_REG0D,0x14);
		snd_soc_write(codec, ES8156_MISC_CONTROL3_REG18,0x00);
		snd_soc_write(codec, ES8156_CLOCK_ON_OFF_REG08,0x3F);
		snd_soc_write(codec, ES8156_RESET_REG00,0x02);
		snd_soc_write(codec, ES8156_RESET_REG00,0x03);
		snd_soc_write(codec, ES8156_ANALOG_SYS5_REG25,0x20);
		break;

	case SND_SOC_BIAS_OFF:
		snd_soc_write(codec,  ES8156_VOLUME_CONTROL_REG14, 0x00);
		snd_soc_write(codec,  ES8156_EQ_CONTROL1_REG19, 0x02);
		snd_soc_write(codec,  ES8156_ANALOG_SYS2_REG21, 0x1F);
		snd_soc_write(codec,  ES8156_ANALOG_SYS3_REG22, 0x02);
		snd_soc_write(codec,  ES8156_ANALOG_SYS5_REG25, 0x21);
		snd_soc_write(codec,  ES8156_ANALOG_SYS5_REG25, 0x01);
		snd_soc_write(codec,  ES8156_ANALOG_SYS5_REG25, 0x87);
		snd_soc_write(codec,  ES8156_MISC_CONTROL3_REG18, 0x01);
		snd_soc_write(codec,  ES8156_MISC_CONTROL2_REG09, 0x02);
		snd_soc_write(codec,  ES8156_MISC_CONTROL2_REG09, 0x01);
		snd_soc_write(codec,  ES8156_CLOCK_ON_OFF_REG08, 0x00);
	}
	return 0;
}

#define es8156_RATES SNDRV_PCM_RATE_8000_96000

#define es8156_FORMATS (SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FMTBIT_S16_LE | \
						SNDRV_PCM_FMTBIT_S24_3LE | SNDRV_PCM_FMTBIT_S24_LE | \
						SNDRV_PCM_FORMAT_S32_LE)

static struct snd_soc_dai_ops es8156_ops = {
    .startup = NULL,
	.hw_params = es8156_pcm_hw_params,
	.set_fmt = es8156_set_dai_fmt,
	.set_sysclk = NULL,
	.digital_mute = es8156_mute,
	.shutdown = NULL,
};

static struct snd_soc_dai_driver es8156_dai = {
	.name = "ES8156 HiFi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = es8156_RATES,
		.formats = es8156_FORMATS,
	},
	.ops = &es8156_ops,
	.symmetric_rates = 1,
};

static int es8156_init_regs(struct snd_soc_codec *codec)
{
	/*
	*set clock and analog power
	*/
	snd_soc_write(codec, ES8156_SCLK_MODE_REG02,0x04);
	snd_soc_write(codec, ES8156_ANALOG_SYS1_REG20,0x2A);
	snd_soc_write(codec, ES8156_ANALOG_SYS2_REG21,0x3C);
	snd_soc_write(codec, ES8156_ANALOG_SYS3_REG22,0x08);
	snd_soc_write(codec, ES8156_ANALOG_LP_REG24,0x07);
	snd_soc_write(codec, ES8156_ANALOG_SYS4_REG23,0x00);

	/*
	*set powerup time
	*/
	snd_soc_write(codec, ES8156_TIME_CONTROL1_REG0A,0x01);
	snd_soc_write(codec, ES8156_TIME_CONTROL2_REG0B,0x01);

	/*
	*set digtal volume
	*/
	snd_soc_write(codec, ES8156_VOLUME_CONTROL_REG14,0xBF);

	/*
	*set MCLK
	*/
	snd_soc_write(codec, ES8156_MAINCLOCK_CTL_REG01,0x21);
	snd_soc_write(codec, ES8156_P2S_CONTROL_REG0D,0x14);
	snd_soc_write(codec, ES8156_MISC_CONTROL3_REG18,0x00);
	snd_soc_write(codec, ES8156_CLOCK_ON_OFF_REG08,0x3F);
	snd_soc_write(codec, ES8156_RESET_REG00,0x02);
	snd_soc_write(codec, ES8156_RESET_REG00,0x03);
	snd_soc_write(codec, ES8156_ANALOG_SYS5_REG25,0x20);

	return 0;
}

static int es8156_suspend(struct snd_soc_codec *codec)
{
	es8156_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int es8156_resume(struct snd_soc_codec *codec)
{
	return 0;
}

enum reg_sysfs_cmd {
	SYSFS_CMD_READ_REG    = 0,
	SYSFS_CMD_WRITE_REG   = 1,
	SYSFS_CMD_MAX
};
struct es8156_sysfs_cmd {
	enum reg_sysfs_cmd cmd;
	const char *key_str;
};

static ssize_t es8156_show(struct device *dev,
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
	for (i = 0; i < 0x025; i++) {
		val = snd_soc_read(es8156_codec, i);
		off += sprintf(buf + off, "[%02d][0x%02x] 0x%x\n", i, i, val);
	}
	off += sprintf(buf + off, "------dump regs end	------\n");

	return off;

}

static ssize_t es8156_set(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	char *p = NULL;
	char *endp = NULL;
	bool found = false;
	int i;
	u32 op_reg, rd_val = 0, wr_val = 0;

	struct es8156_sysfs_cmd cmd_arr[] = {
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
	if(op_reg < 0 || op_reg > 37) {
		pr_err("op_reg is error\n");
		return count;
	}

	switch (cmd_arr[i].cmd) {
	case SYSFS_CMD_READ_REG:
		rd_val = snd_soc_read(es8156_codec, op_reg);
		pr_err("\nrd reg[%02d][0x%02x] 0x%x\n", op_reg, op_reg, rd_val);
		break;

	case SYSFS_CMD_WRITE_REG:
		if (endp == NULL) {
			pr_err("not enough para, wr_val needed\n");
			return count;
		}

		wr_val = (u32)simple_strtoul(endp + 1, &endp, 0);
		snd_soc_write(es8156_codec, op_reg, wr_val);
		pr_err("\nwr reg [%02d][0x%02x], val = 0x%x\n", op_reg, op_reg, wr_val);
		break;
	default:
		break;

	}

	return count;
}

static DEVICE_ATTR(es8156_test, S_IWUSR | S_IRUSR,
			es8156_show, es8156_set);

static struct attribute *es8156_attributes[] = {
	&dev_attr_es8156_test.attr,
	NULL
};

static const struct attribute_group es8156_attr_group = {
	.attrs = es8156_attributes,
};

static int es8156_probe(struct snd_soc_codec *codec)
{
	int ret = 0;

	es8156_codec = codec;
	es8156_reset(codec);
	es8156_init_regs(codec);
	return ret;
}

static int es8156_remove(struct snd_soc_codec *codec)
{
	return es8156_set_bias_level(codec, SND_SOC_BIAS_OFF);
}

const struct regmap_config es8156_regmap_config = {
	.reg_bits	= 8,
	.val_bits	= 8,
	.max_register	= 0xff,
	.cache_type	= REGCACHE_RBTREE,
	.reg_defaults = es8156_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(es8156_reg_defaults),
};
EXPORT_SYMBOL_GPL(es8156_regmap_config);

static struct snd_soc_codec_driver soc_codec_dev_es8156 = {
	.probe =	es8156_probe,
	.remove =	es8156_remove,
	.suspend =	es8156_suspend,
	.resume =	es8156_resume,
	.set_bias_level = es8156_set_bias_level,
	.component_driver = {
		.controls = es8156_snd_controls,
		.num_controls = ARRAY_SIZE(es8156_snd_controls),
	}
};

static int es8156_i2c_probe(struct i2c_client *i2c,
						const struct i2c_device_id *id)
{
	struct es8156_priv *es8156;
	int ret = -1;

	es8156 = devm_kzalloc(&i2c->dev, sizeof(*es8156), GFP_KERNEL);
	if (!es8156)
		return -ENOMEM;

	es8156->regmap = devm_regmap_init_i2c(i2c, &es8156_regmap_config);
	if (IS_ERR(es8156->regmap))
		return PTR_ERR(es8156->regmap);

	es8156->debounce_time = 200;
	es8156->hp_det_invert = 0;
	es8156->pwr_count = 0;
	es8156->hp_inserted = false;
	es8156->muted = true;

	dev_set_drvdata(&i2c->dev, es8156);

	ret = sysfs_create_group(&i2c->dev.kobj, &es8156_attr_group);
	if (ret < 0)
		dev_warn(&i2c->dev, "attr group create failed\n");
	else
		dev_info(&i2c->dev, "attr group create success!\n");


	ret = snd_soc_register_codec(&i2c->dev, &soc_codec_dev_es8156, &es8156_dai, 1);

	return ret;
}

void es8156_i2c_shutdown(struct i2c_client *client)
{
	if (es8156_codec != NULL) {
		es8156_set_bias_level(es8156_codec, SND_SOC_BIAS_OFF);
	}
}

static int es8156_i2c_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client->dev.kobj, &es8156_attr_group);
	snd_soc_unregister_codec(&client->dev);
	return 0;
}

static const struct i2c_device_id es8156_i2c_id[] = {
	{"es8156", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, es8156_i2c_id);


static const struct of_device_id es8156_of_match[] = {
	{ .compatible = "everest,es8156", },
	{ }
};
MODULE_DEVICE_TABLE(of, es8156_of_match);

static struct i2c_driver es8156_i2c_driver = {
	.driver = {
		.name		= "es8156",
		.of_match_table = of_match_ptr(es8156_of_match),
	},
	.probe    = es8156_i2c_probe,
	.remove   = es8156_i2c_remove,
	.shutdown = es8156_i2c_shutdown,
	.id_table = es8156_i2c_id,
};

module_i2c_driver(es8156_i2c_driver);

MODULE_DESCRIPTION("ASoC es8156 driver");
MODULE_AUTHOR("Will <pengxiaoxin@everset-semi.com>");
MODULE_LICENSE("GPL");
