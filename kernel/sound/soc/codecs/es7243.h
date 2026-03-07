/*
 * ALSA SoC ES7243l adc driver
 *
 * Author:      David Yang, <yangxiaohua@everest-semi.com>
 *              or
 *              <info@everest-semi.com>
 * Copyright:   (C) 2019 Everest Semiconductor Co Ltd.,
 *
 * Based on sound/soc/codecs/es7243.c by DavidYang
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Notes:
 *  this is an important file, you need to check it before you use ES7243l.
 *  es7243l.h is a user interface which is convenient for digital
 *  format, clock ratio, etc.
 *
 */

#ifndef __ES7243_H__
#define __ES7243_H__


#include <linux/i2c.h>

#define ENABLE     1
#define DISABLE    0
/*
* Here is the definition of ES7243l ADC Digital Format
* Users must select correct digital format for their systerm.
*
* ES7243l_WORK_MODE is used to select digital format, and user must update it for their system
* In ES7243l codec driver (es7243l.c), ES7243l_WORK_MODE will be used for digital format setting.
*
* In normal mode, ES7243l supports four digital formats including I2S, LJ, DSP-A and DSP-B, with
* resolution from 16bits to 32bits.
* In TDM mode, ES7243l only supports DSP-A TDM, doesn't support DSP-B TDM.
* In NFS mode, ES7243l only supports NFS I2S mode, doesn't support DSP or LJ NFS mode.
*/
#define ES7243l_NORMAL_I2S  0
#define ES7243l_NORMAL_LJ  1
#define ES7243l_NORMAL_DSPA  2
#define ES7243l_NORMAL_DSPB  3
#define ES7243l_TDM_A  4
#define ES7243l_NFS_I2S  5
#define ES7243l_NFS_DSPA  6
#define ES7243l_WORK_MODE ES7243l_NORMAL_I2S
/*
* Here is the definition of the common MCLK/LRCK rato.
* ES7243l will have different register configuration for each MCLK/LRCK ratio.
* Please check the MCLK/LRCK ratio in your system before you update ES7243l_MCLK_LRCK_RATIO.
* ES7243l codec driver will configure the clock registers according to the value of ES7243l_MCLK_LRCK_RATO.
*/
#define RATIO_3072 3072
#define RATIO_2048 2048
#define RATIO_1536 1536
#define RATIO_1024 1024
#define RATIO_768  768
#define RATIO_512  512
#define RATIO_384  384
#define RATIO_256  256
#define RATIO_192  192
#define RATIO_128  128
#define RATIO_64  64
#define ES7243l_MCLK_LRCK_RATIO   RATIO_64
/*
* To select the total analog input channel for microphone array
*/
#define AIN_2_CH   2
#define AIN_4_CH   4
#define AIN_6_CH   6
#define AIN_8_CH   8
#define AIN_10_CH  10
#define AIN_12_CH  12
#define AIN_14_CH  14
#define AIN_16_CH  16
#define ES7243l_CHANNELS_MAX    AIN_2_CH
/*
* to select the clock soure for internal MCLK clock
*/
#define FROM_MCLK_PIN   0
#define FROM_INTERNAL_BCLK  1
#define ES7243l_MCLK_SOURCE  FROM_MCLK_PIN
/*
* to select the data length or resolution
*/
#define DATA_16BITS    0
#define DATA_24BITS    1
#define DATA_32BITS    2
#define ES7243l_DATA_LENGTH   DATA_32BITS
/*
* to select the pdm digital microphone interface
*/
#define DMIC_INTERFACE_ON   true
#define DMIC_INTERFACE_OFF  false
#define DMIC_INTERFACE      DMIC_INTERFACE_OFF
/*
* to select bclk inverted or not
*/
#define BCLK_NORMAL       false
#define BCLK_INVERTED     true
#define BCLK_INVERTED_OR_NOT    BCLK_NORMAL
/*
* to select mclk inverted or not
*/
#define MCLK_NORMAL       false
#define MCLK_INVERTED     true
#define MCLK_INVERTED_OR_NOT    MCLK_NORMAL
/*
* to select PGA gain for different analog input channel
* user must allocate the PGA gain for each analog input channel
* ES7243l_MIC_ARRAY_AIN1_PGA to ES7243l_MIC_ARRAY_AIN16_PGA is used for PGA gain
*/
#define PGA_0DB           0
#define PGA_3DB           1
#define PGA_6DB           2
#define PGA_9DB           3
#define PGA_12DB          4
#define PGA_15DB          5
#define PGA_18DB          6
#define PGA_21DB          7
#define PGA_24DB          8
#define PGA_27DB          9
#define PGA_30DB          10
#define PGA_33DB          11
#define PGA_34DB          12
#define PGA_36DB          13
#define PGA_37DB          14

#if ES7243l_CHANNELS_MAX > 0
#define ES7243l_MIC_ARRAY_AIN1_PGA     PGA_27DB
#define ES7243l_MIC_ARRAY_AIN2_PGA     PGA_27DB
#endif

/*
* here is the definition of digital volume.
* the digital volume is 0dB by default. User can update it
* ES7243l_DIGITAL_VOLUME_1 to ES7243l_DIGITAL_VOLUME_16 is used for digital volume
* digital volume is 0db default.
*/

#if ES7243l_CHANNELS_MAX > 0
#define DIG_VOL_1     0		// DB
#define ES7243l_DIGITAL_VOLUME_1		0xbf + (DIG_VOL_1 * 2)
#define DIG_VOL_2     0		// DB
#define ES7243l_DIGITAL_VOLUME_2		0xbf + (DIG_VOL_2 * 2)
#endif

/*
* set the I2C chip address for each es7243l device in TDM linkloop
* user can update the chip address according their system circuit
*/
#define I2C_CHIP_ADDR_10H	0x10	// AD0 and AD1 pulled down, ASDOUT pulled down or float
#define I2C_CHIP_ADDR_11H       0x11	// AD0 pulled up, AD1 pulled down, ASDOUT pulled down or float
#define I2C_CHIP_ADDR_12H       0x12	// AD0 pulled down, AD1 pulled up, ASDOUT pulled down or float
#define I2C_CHIP_ADDR_13H       0x13	// AD0 and AD1 pulled up, ASDOUT pulled down or float
#define I2C_CHIP_ADDR_14H       0x14	// AD0 and AD1 pulled down, ASDOUT pulled up
#define I2C_CHIP_ADDR_15H       0x15	// AD0 pulled up, AD1 pulled down, ASDOUT pulled up
#define I2C_CHIP_ADDR_16H       0x16	// AD0 pulled down, AD1 pulled up, ASDOUT pulled up
#define I2C_CHIP_ADDR_17H       0x17	// AD0 and AD1 pulled up, ASDOUT pulled up
#if ES7243l_CHANNELS_MAX > 0
#define ES7243l_I2C_CHIP_ADDRESS_0       I2C_CHIP_ADDR_10H
#endif

#define ES7243l_CODEC_RW_TEST_EN        0
#define ES7243l_IDLE_RESET_EN           1	//reset ES7243 when in idle time
#define ES7243l_MATCH_DTS_EN            1	//ES7243 match method select: 0: i2c_detect, 1:of_device_id

#define VDDA_1V8	0
#define VDDA_VOLTAGE	VDDA_1V8

int
es7243l_probe_internel(struct i2c_client *i2c,
					const struct i2c_device_id *i2c_id);

void es7243l_i2c_remove_internel(struct device *dev);
#endif
