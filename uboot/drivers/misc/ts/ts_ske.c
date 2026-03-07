// SPDX-License-Identifier: GPL-2.0+
/*
 * Tsingmicro HASH Driver
 *
 * Copyright (C) 2023
 * author : yangguoyu@tsingmicro.com
 * data: 2023-10-19
 * version: 1.0
 */

#include <common.h>
#include <dm.h>
#include <log.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <dm/of_access.h>
#include <dm/ofnode.h>
#include <errno.h>
#include <clk.h>
#include <asm/io.h>
#include <asm/utils.h>
#include <linux/bitops.h>

#include "ts_ske.h"

typedef struct {
	void __iomem *regs;
	u32	irq;
} ts_ske_priv_t;

static ts_ske_priv_t s_ts_ske_instance;
ts_ske_priv_t *g_ts_ske;

static int ts_ske_get_block_byte_len(enum ske_alg_type alg_type)
{
	u8 block_byte = 0;

	switch (alg_type) {
	case SKE_TYPE_AES:
		case SKE_TYPE_SM4:
		block_byte = 16;
		break;
	case SKE_TYPE_3DES_EDE:
	case SKE_TYPE_DES:
	case SKE_TYPE_3DES_EEE:
		block_byte = 8;
		break;
	default:
		break;
	}

	return block_byte;

}

static u8 ts_ske_get_key_byte_len(enum ske_key_size key_size_bit)
{
	u8 key_len_byte = 0;

	switch (key_size_bit) {
	case SKE_KEY_SIZE_256_BIT:
		key_len_byte = 32;
		break;
	case SKE_KEY_SIZE_192_BIT:
		key_len_byte = 24;
		break;
	case SKE_KEY_SIZE_128_BIT:
		key_len_byte = 16;
		break;
	default:
		break;
	}
	return key_len_byte;
}

static void ts_ske_set_work_mode(enum ske_work_mode mode)
{
	u32 val = readl(g_ts_ske->regs + SKE_CFG);

	val &= ~SKE_CFG_DMAEN_MASK;
	val |= (mode << SKE_CFG_DMAEN_POS);
	writel(val, g_ts_ske->regs + SKE_CFG);
}

static void ts_ske_set_endian_uint32(enum ske_data_type type)
{
	u32 val = readl(g_ts_ske->regs + SKE_CFG);

	val &= ~SKE_CFG_DATA_TYPE_MASK;
	val |= (type << SKE_CFG_DATA_TYPE_POS);
	writel(val, g_ts_ske->regs + SKE_CFG);
}

static void ts_ske_set_secure_port(enum ske_sp_en enable)
{
	u32 val = readl(g_ts_ske->regs + SKE_SP);

	val &= ~SKE_SP_EN_MASK;
	val |= (enable << SKE_SP_EN_POS);
	writel(val, g_ts_ske->regs + SKE_SP);
}

static void ts_ske_set_crypto_dir(enum 	  ske_dec_type dec_type)
{
	u32 val = readl(g_ts_ske->regs + SKE_CFG);

	val &= ~SKE_CFG_DEC_TYPE_MASK;
	val |= (dec_type << SKE_CFG_DEC_TYPE_POS);
	writel(val, g_ts_ske->regs + SKE_CFG);
}

static void ts_ske_set_pop_dout(enum ske_ctrl_pop_dout_en enable)
{
	u32 val = readl(g_ts_ske->regs + SKE_CTRL);

	val &= ~SKE_CTRL_POP_DOUT_MASK;
	val |= (enable << SKE_CTRL_POP_DOUT_POS);
	writel(val, g_ts_ske->regs + SKE_CTRL);
}

static void ts_ske_set_start(enum ske_ctrl_calc_en enable)
{
	u32 val = readl(g_ts_ske->regs + SKE_CTRL);

	val &= ~SKE_CTRL_ENABLE_MASK;
	val |= (enable << SKE_CTRL_ENABLE_POS);
	writel(val, g_ts_ske->regs + SKE_CTRL);
}

static void ts_ske_set_mode(enum	   ske_mode_type mode)
{
	u32 val = readl(g_ts_ske->regs + SKE_CFG);

	val &= ~SKE_CFG_MODE_MASK;
	val |= (mode << SKE_CFG_MODE_POS);
	writel(val, g_ts_ske->regs + SKE_CFG);
}

static void ts_ske_set_alg(enum	  ske_alg_type alg_type)
{
	u32 val = readl(g_ts_ske->regs + SKE_CFG);

	val &= ~SKE_CFG_ALG_TYPE_MASK;
	val |= (alg_type << SKE_CFG_ALG_TYPE_POS);
	writel(val, g_ts_ske->regs + SKE_CFG);
}

static void ts_ske_set_key1_size(enum		ske_key_size size)
{
	u32 val = readl(g_ts_ske->regs + SKE_CFG);

	val &= ~SKE_CFG_KEY1_SIZE_MASK;
	val |= (size << SKE_CFG_KEY1_SIZE_POS);
	writel(val, g_ts_ske->regs + SKE_CFG);
}

static void ts_ske_set_key2_size(enum		ske_key_size size)
{
	u32 val = readl(g_ts_ske->regs + SKE_CFG);

	val &= ~SKE_CFG_KEY2_SIZE_MASK;
	val |= (size << SKE_CFG_KEY2_SIZE_POS);
	writel(val, g_ts_ske->regs + SKE_CFG);
}

static void ts_ske_set_key2(const u32 *key, u32 key_words)
{
	u32 i, offset;

	for (i = key_words; i > 0; i--) {
		offset = SKE_KEY2 + SKE_KEY2_OFFSET * (i - 1);
		if (offset < SKE_KEY2)
			break;

		writel(key[key_words - i], g_ts_ske->regs + offset);
	}
}

static void ts_ske_set_key1(const u32 *key, u32 key_words)
{
	u32 i, offset;

	for (i = key_words; i > 0; i--) {
		offset = SKE_KEY1 + SKE_KEY1_OFFSET * (i - 1);
		if (offset < SKE_KEY1)
			break;

		writel(key[key_words - i], g_ts_ske->regs + offset);
	}
}

static void ts_ske_set_key(struct ts_ske_aes_state *mctx, u8 key_index)
{
	u32 tmp[8];
	u32 key_bytes;

	key_bytes = ts_ske_get_key_byte_len(mctx->key_size);

	if(key_index == 1)
		memcpy(tmp, mctx->key, key_bytes);
	else  // XTS_MODE
		memcpy(tmp, mctx->key + key_bytes, key_bytes);


	//for 3DES-2key, set key3=key1
	if(mctx->key_size == SKE_KEY_SIZE_128_BIT) {
		switch(mctx->alg_type)
		{
			case SKE_TYPE_3DES_EDE:
			case SKE_TYPE_3DES_EEE:
				memcpy(tmp+4, mctx->key, 8);
				key_bytes += 8;
				break;
			default:
				break;
		}
	}

	if(key_index == 1)
		ts_ske_set_key1(tmp, key_bytes / 4);
	else // XTS_MODE
		ts_ske_set_key2(tmp, key_bytes / 4);
}

static void ts_ske_set_last_block(enum ske_last_block is_last_block)
{
	u32 val = readl(g_ts_ske->regs + SKE_DIN_CR);

	val &= ~SKE_DIN_CR_LAST_MASK;
	val |= (is_last_block << SKE_DIN_CR_LAST_POS);
	writel(val, g_ts_ske->regs + SKE_DIN_CR);

}

static void ts_ske_set_update_flag(enum ske_update_flag up_flag)
{
	u32 val = readl(g_ts_ske->regs + SKE_CFG);

	val &= ~SKE_CFG_UPDATE_CFG_MASK;
	val |= (up_flag << SKE_CFG_UPDATE_CFG_POS);
	writel(val, g_ts_ske->regs + SKE_CFG);

}

static void ts_ske_set_iv_uint32(u32 *iv, u32 block_words)
{
	u8 i, offset;

	for (i = block_words; i > 0; i--) {
		offset = SKE_IV + SKE_IV_OFFSET * (i - 1);
		if ((offset > SKE_IV_END) || (offset < SKE_IV))
			break;

		writel(iv[block_words-i], g_ts_ske->regs + offset);
	}
}

static void ts_ske_set_iv(u8 *iv, u32 block_bytes)
{
	u32 tmp[4];

	if (((u32)(uintptr_t)iv) & 3) {
		memcpy(tmp, iv, block_bytes);
		ts_ske_set_iv_uint32(tmp, block_bytes / 4);
	} else
		ts_ske_set_iv_uint32((u32 *)iv, block_bytes / 4);

}

static s32 ts_ske_set_iv_param(struct ts_ske_aes_state *mctx)
{
	if(mctx->alg_mode_type == ECB)
		mctx->iv = NULL;
	else if(mctx->iv == NULL)
		return -1;

	if(mctx->iv)
		ts_ske_set_iv(mctx->iv, mctx->block_bytes);

	return 0;
}

static void ts_ske_expand_key_wait_till_done(void)
{
	u32 val = readl(g_ts_ske->regs + SKE_SR);

	while ((val & SKE_SR_RFC_MASK) == 0)
		val = readl(g_ts_ske->regs + SKE_SR);
}

static void ts_ske_wait_till_output(void)
{
	u32 val = readl(g_ts_ske->regs + SKE_SR);

	while ((val & SKE_SR_RFDO_MASK) == 0)
		val = readl(g_ts_ske->regs + SKE_SR);
}

static void ts_ske_start(void)
{
	writel(0x0, g_ts_ske->regs + SKE_RISR);
	ts_ske_set_start(CALC_YES);
}

static void ts_ske_expand_key(void)
{
	//update cfg
	ts_ske_set_update_flag(UPDATE_CFG_YES);

	//expand key
	ts_ske_start();
	ts_ske_expand_key_wait_till_done();

	//not update cfg
	ts_ske_set_update_flag(UPDATE_CFG_NO);
}

void ts_ske_set_one_block(u32 *in, u32 block_words)
{
	u8 i, offset;

	for (i = block_words; i > 0; i--) {
		offset = SKE_DIN + SKE_DIN_OFFSET * (i - 1);
		if ((offset > SKE_DIN_END) || (offset < SKE_DIN))
			break;

		writel(in[block_words-i], g_ts_ske->regs + offset);
	}
}

void ts_ske_get_one_block(u32 *out, u32 block_words)
{
	u8 i, offset;

	ts_ske_set_pop_dout(POP_DOUT_YES);

	for (i = block_words; i > 0; i--) {
		offset = SKE_DOUT + SKE_DOUT_OFFSET * (i - 1);
		if ((offset > SKE_DOUT_END) || (offset < SKE_DOUT))
			break;
		out[block_words - i] = readl(g_ts_ske->regs + offset);
	}

}

static void ts_ske_set_alg_param(struct ts_ske_aes_state *mctx)
{
	ts_ske_set_alg(mctx->alg_type);
	ts_ske_set_key1_size(mctx->key_size);
	ts_ske_set_key2_size(mctx->key_size);

	ts_ske_set_mode(mctx->alg_mode_type);
	ts_ske_set_crypto_dir(mctx->dec_type);
}

static void ts_ske_set_key_param(struct ts_ske_aes_state *mctx)
{
	if(mctx->key != NULL) {
		ts_ske_set_secure_port(SKE_SP_DISABLE);
		ts_ske_set_key(mctx, 1);

		if(mctx->alg_mode_type == XTS_NORMAL)
			ts_ske_set_key(mctx, 2);

		ts_ske_expand_key();

	} else { // key is from secure port
		ts_ske_set_secure_port(SKE_SP_ENABLE);
	}
}

static void ts_ske_init_param_config(struct ts_ske_aes_state *mctx)
{
	mctx->block_bytes = ts_ske_get_block_byte_len(mctx->alg_type);
	mctx->block_words = mctx->block_bytes >> 2;

	ts_ske_set_endian_uint32(SKE_DATA_TYPE_BYTE_EXCHANGE);
	ts_ske_set_alg_param(mctx);
	ts_ske_set_last_block(SKE_WORK_NO_LAST);
	ts_ske_set_iv_param(mctx);
	ts_ske_set_key_param(mctx);
}

static void ts_ske_update_blocks(struct ts_ske_aes_state *mctx)
{
	u32 in_word_align, out_word_align;
	u32 tmp_in[4];
	u32 tmp_out[4];
	u32 i;
	u8 *in = mctx->src;
	u8 *out = mctx->dst;
	int bytes = mctx->len;

	if(((u32)(uintptr_t)in) & 0x3)
		in_word_align = 0;
	else
		in_word_align = 1;

	if(((u32)(uintptr_t)out) & 0x3)
		out_word_align = 0;
	else
		out_word_align = 1;

	//input one block ---> calculating ---> output one block
	for (i = 0; i < bytes; i += mctx->block_bytes) {
		if(in_word_align)
			ts_ske_set_one_block((u32 *)in, mctx->block_words);
		else {
			memcpy(tmp_in, in, mctx->block_bytes);
			ts_ske_set_one_block((u32 *)tmp_in, mctx->block_words);
		}

		ts_ske_start();
		//ske_hp_calc_wait_till_done();
		ts_ske_wait_till_output();

		if(NULL != out) {
			if(out_word_align)
				ts_ske_get_one_block((u32 *)out, mctx->block_words);
			else {
				ts_ske_get_one_block((u32 *)tmp_in, mctx->block_words);
				memcpy(out, tmp_in, mctx->block_bytes);
			}

			out += mctx->block_bytes;
		} else {
			ts_ske_get_one_block((u32 *)tmp_out, mctx->block_words);
		}

		in += mctx->block_bytes;
	}
}

u32 ts_ske_crypto(struct ts_ske_aes_state *mctx)
{
	ts_ske_set_work_mode(SKE_WORK_TYPE_CPU);
	ts_ske_init_param_config(mctx);
	ts_ske_update_blocks(mctx);

	return 0;
}

static int ts_ske_clk_enable(struct udevice *dev)
{
	int ret;
	struct clk clk;

#if CONFIG_IS_ENABLED(CLK)
	ret = clk_get_by_name(dev, "ske_clk", &clk);
	if (ret) {
		dev_err(dev, "failed to get clock\n");
		return ret;
	}

	ret = clk_enable(&clk);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP) {
		clk_free(&clk);
		dev_err(dev, "failed to enable clock\n");
		return ret;
	}
#endif
	return 0;
}

static int ts_ske_probe(struct udevice *dev)
{
	void __iomem *base;
	int ret;

	base = dev_remap_addr_index(dev, 0);
	if (!base)
		return -ENODEV;

	ts_ske_priv_t *ts_ske_priv = &s_ts_ske_instance;
	ts_ske_priv->regs = base;
	g_ts_ske = ts_ske_priv;

	ret = ts_ske_clk_enable(dev);
	if (ret) {
		dev_err(dev, "failed to enable clock %d\n", ret);
		return ret;
	}

	return 0;
}

static const struct udevice_id ts_ske_match[] = {
	{ .compatible = "tsingmicro,ts-ske" },
	{ /* sentinel */ }
};


U_BOOT_DRIVER(ts_ske) = {
	.name = "ts_ske",
	.id = UCLASS_MISC,
	.of_match = ts_ske_match,
	.probe = ts_ske_probe,
};
