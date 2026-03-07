// SPDX-License-Identifier: GPL-2.0+
/*
 * Tsingmicro Crypto HASH Driver
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
#include <linux/io.h>
#include <asm/arch/sys_proto.h>
#include <dm/of_access.h>
#include <dm/ofnode.h>
#include <errno.h>
#include <clk.h>

#include "ts_hash.h"

typedef struct {
	void __iomem *regs;
	u32	irq;
} ts_hash_priv_t;

static ts_hash_priv_t s_ts_hash_instance;

ts_hash_priv_t *g_ts_hash;

u32 const HASH_MD5_IV[4]    = { 0x67452301, 0xefcdab89,
						   0x98badcfe, 0x10325476,};
u32 const HASH_SHA256_IV[8] = { 0x67E6096A, 0x85AE67BB,
						   0x72F36E3C, 0x3AF54FA5,
						   0x7F520E51, 0x8C68059B,
						   0xABD9831F, 0x19CDE05B,};
u32 const HASH_SM3_IV[8]    = { 0x6F168073, 0xB9B21449,
						   0xD7422417, 0x00068ADA,
						   0xBC306FA9, 0xAA383116,
						   0x4DEE8DE3, 0x4E0EFBB0,};

static s32 ts_hash_get_block_word_len(enum hash_alg_type hash_type)
{
	u8 block_words = 0;

	switch (hash_type) {
	case HASH_TYPE_SM3:
	case HASH_TYPE_MD5:
	case HASH_TYPE_SHA1:
	case HASH_TYPE_SHA256:
	case HASH_TYPE_SHA224:
		block_words = 16;
		break;
	case HASH_TYPE_SHA384:
	case HASH_TYPE_SHA512:
	case HASH_TYPE_SHA512_224:
	case HASH_TYPE_SHA512_256:
		block_words = 32;
		break;
	default:
		break;
	}

	return block_words;

}

static s32 ts_hash_get_iterator_word_len(enum hash_alg_type hash_type)
{
	u8 iterator_words = 0;

	switch (hash_type) {
	case HASH_TYPE_MD5:
		iterator_words = 4;
		break;
	case HASH_TYPE_SHA1:
		iterator_words = 5;
		break;
	case HASH_TYPE_SM3:
	case HASH_TYPE_SHA256:
	case HASH_TYPE_SHA224:
		iterator_words = 8;
		break;
	case HASH_TYPE_SHA384:
	case HASH_TYPE_SHA512:
	case HASH_TYPE_SHA512_224:
	case HASH_TYPE_SHA512_256:
		iterator_words = 16;
		break;
	default:
		break;
	}

	return iterator_words;
}

static s32 ts_hash_get_digest_word_len(enum hash_alg_type hash_type)
{
	u8 digest_words = 0;

	switch (hash_type) {
	case HASH_TYPE_MD5:
		digest_words = 4;
		break;
	case HASH_TYPE_SHA1:
		digest_words = 5;
		break;
	case HASH_TYPE_SHA224:
	case HASH_TYPE_SHA512_224:
		digest_words = 7;
		break;
	case HASH_TYPE_SM3:
	case HASH_TYPE_SHA256:
	case HASH_TYPE_SHA512_256:
		digest_words = 8;
		break;
	case HASH_TYPE_SHA384:
	case HASH_TYPE_SHA512:
		digest_words = 16;
		break;
	default:
		break;
	}

	return digest_words;
}

void ts_hash_u32_clear(u32 *a, u32 wordLen)
{
	while (wordLen)
		a[--wordLen] = 0;
}

static void ts_hash_set_alg(enum hash_alg_type alg_type)
{
	u32 val = readl(g_ts_hash->regs + HASH_CFG);

	val &= ~HASH_CFG_MSEL_MASK;
	val |= (alg_type << HASH_CFG_MSEL_POS);
	writel(val, g_ts_hash->regs + HASH_CFG);
}

static void ts_hash_set_last_block(enum hash_last_block isLastBlock)
{
	u32 val = readl(g_ts_hash->regs + HASH_CFG);

	val &= ~HASH_CFG_LAST_MASK;
	val |= (isLastBlock << HASH_CFG_LAST_POS);
	writel(val, g_ts_hash->regs + HASH_CFG);
}

static void ts_hash_set_work_mode(enum hash_work_mode mode)
{
	u32 val = readl(g_ts_hash->regs + HASH_CFG);

	val &= ~HASH_CFG_DMAEN_MASK;
	val |= (mode << HASH_CFG_DMAEN_POS);
	writel(val, g_ts_hash->regs + HASH_CFG);

}

void ts_hash_set_iterator(u32 *iterator, u8 hash_iterator_words)
{
	u8 i, offset;

	if(iterator == NULL)
		return;

	for (i = 0; i < hash_iterator_words; i++) {
		offset = HASH_IN + i * HASH_IN_OFFSET;
		if (offset > HASH_IN_END)
			break;

		writel(iterator[i], g_ts_hash->regs + offset);
	}
}

void ts_hash_get_iterator(u8 *iterator, u8 hash_iterator_words)
{
	u32 temp;
	u8 i, offset;

	/* for the case that iterator is not aligned by word */
	if (((u32)(uintptr_t)iterator) & 3) {
		for (i = 0; i < hash_iterator_words; i++) {
			offset = HASH_OUT + i * HASH_OUT_OFFSET;
			if (offset > HASH_OUT_END)
				break;

			temp = readl(g_ts_hash->regs + offset);
			memcpy(iterator + (i << 2), &temp, 4);
		}
	} else {
		for (i = 0; i < hash_iterator_words; i++) {
			offset = HASH_OUT + i * HASH_OUT_OFFSET;
			if (offset > HASH_OUT_END)
				break;
			((u32 *)iterator)[i] = readl(g_ts_hash->regs + offset);
		}
	}

}

void ts_hash_set_tx_bit_len(u32 bits)
{
	writel(bits, g_ts_hash->regs + HASH_TX_LEN);
}

static void ts_hash_set_endian_uint32(enum data_type type)
{
	u32 val = readl(g_ts_hash->regs + HASH_CFG);

	val &= ~HASH_CFG_DATA_TYPE_MASK;
	val |= (type << HASH_CFG_DATA_TYPE_POS);
	writel(val, g_ts_hash->regs + HASH_CFG);
}

static void ts_hash_disable_cpu_intr(void)
{
	u32 val = readl(g_ts_hash->regs + HASH_IMCR);

	val &= ~HASH_IMCR_CPU_IRQ_MASK;
	val |= (HASH_DISABLE << HASH_IMCR_CPU_IRQ_POS);
	writel(val, g_ts_hash->regs + HASH_IMCR);
}

static void ts_hash_disable_dma_intr(void)
{
	u32 val = readl(g_ts_hash->regs + HASH_IMCR);

	val &= ~HASH_IMCR_CPU_IRQ_MASK;
	val |= (HASH_DISABLE << HASH_IMCR_DMA_IRQ_POS);
	writel(val, g_ts_hash->regs + HASH_IMCR);
}

static void __attribute__((unused)) ts_hash_enable_cpu_intr(void)
{
	u32 val = readl(g_ts_hash->regs + HASH_IMCR);

	val &= ~HASH_IMCR_CPU_IRQ_MASK;
	val |= (HASH_ENABLE << HASH_IMCR_CPU_IRQ_POS);
	writel(val, g_ts_hash->regs + HASH_IMCR);
}

static void __attribute__((unused)) ts_hash_enable_dma_intr(void)
{
	u32 val = readl(g_ts_hash->regs + HASH_IMCR);

	val &= ~HASH_IMCR_DMA_IRQ_MASK;
	val |= (HASH_ENABLE << HASH_IMCR_DMA_IRQ_POS);
	writel(val, g_ts_hash->regs + HASH_IMCR);
}

static void ts_hash_enable(void)
{
	u32 val = readl(g_ts_hash->regs + HASH_CTRL);

	val &= ~HASH_CTRL_ENABLE_MASK;
	val |= (HASH_ENABLE << HASH_CTRL_ENABLE_POS);
	writel(val, g_ts_hash->regs + HASH_CTRL);

}

static void __attribute__((unused)) ts_hash_disable(void)
{
	u32 val = readl(g_ts_hash->regs + HASH_CTRL);

	val &= ~HASH_CTRL_ENABLE_MASK;
	val |= (HASH_DISABLE << HASH_CTRL_ENABLE_POS);
	writel(val, g_ts_hash->regs + HASH_CTRL);

}

static void ts_hash_wait_till_done(void)
{
	u32 val = readl(g_ts_hash->regs + HASH_CTRL);

	while (val & HASH_CTRL_ENABLE_MASK)
		val = readl(g_ts_hash->regs + HASH_CTRL);

}

void ts_hash_set_IV(enum hash_alg_type hash_type, u8 hash_iterator_words)
{
	u32 *iv = NULL;

	switch (hash_type) {
	case HASH_TYPE_SM3:
		iv = (u32 *)HASH_SM3_IV;
		break;
	case HASH_TYPE_MD5:
		iv = (u32 *)HASH_MD5_IV;
		break;
	case HASH_TYPE_SHA1:
	case HASH_TYPE_SHA256:
		iv = (u32 *)HASH_SHA256_IV;
		break;
	case HASH_TYPE_SHA224:
		break;
	case HASH_TYPE_SHA384:
	case HASH_TYPE_SHA512:
	case HASH_TYPE_SHA512_224:
	case HASH_TYPE_SHA512_256:
		break;
	default:
		break;
	}

	ts_hash_set_iterator(iv, hash_iterator_words);
}

static void ts_hash_start(struct ts_hash_state *ctx)
{
	if (ctx->first_update_flag) {
		ts_hash_set_IV(ctx->hash_type, ctx->iterator_word_len);
		ctx->first_update_flag = 0;   //clear the flag
	}

	ts_hash_disable_cpu_intr();
	ts_hash_disable_dma_intr();
	ts_hash_enable();
}

static inline u8 ts_hash_total_len_add_uint32(u32 *a, u32 a_words, u32 b)
{
	u32 i;

	for (i = 0; i < a_words; i++) {
		a[i] += b;
		if (a[i] < b)
			b = 1;
		else
			break;
	}

	if (i == a_words)
		return 1;
	else if (a[a_words-1] & 0xE0000000)  //bit length overflow
		return 1;
	else
		return 0;
}

static inline void ts_hash_total_bytelen_2_bitlen(u32 *a, u32 a_words)
{
	int32_t i;

	for (i = a_words-1; i > 0; i--) {
		a[i] <<= 3;
		a[i] |= a[i-1]>>(32-3);
	}
	a[i] <<= 3;
}

void ts_hash_set_msg_total_bit_len(u32 *msg_total_bits, u8 words)
{
	s32 i, offset;

	for (i = 0; i < words; i++) {
		offset = HASH_MSG_LEN + i * HASH_MSG_LEN_OFFSET;
		if (offset > HASH_MSG_LEN_END)
			break;
		writel(msg_total_bits[i], g_ts_hash->regs + offset);
	}
}

void ts_hash_input_msg(u8 *msg, u32 msg_words)
{
	u32 tmp;

	if (((u32)(uintptr_t)msg) & 3) {
		while (msg_words--) {
			memcpy(&tmp, msg, 4);
			writel(tmp, g_ts_hash->regs + HASH_M_DIN);
			msg += 4;
		}
	} else {
		while (msg_words--) {
			writel(*((u32 *)msg), g_ts_hash->regs + HASH_M_DIN);
			msg += 4;
		}
	}
}

static void ts_hash_calc_rand_len_msg(struct ts_hash_state *ctx, const u8 *msg, u32 msg_bytes)
{
	/* set the input iterator data */
	if (1 != ctx->first_update_flag)
		ts_hash_set_iterator(ctx->iterator, ctx->iterator_word_len);

	ts_hash_set_last_block(HASH_WORK_HAS_LAST);
	ts_hash_set_tx_bit_len(msg_bytes*8);
	ts_hash_start(ctx);
	ts_hash_input_msg((u8 *)msg, (msg_bytes+3)/4);
	ts_hash_wait_till_done();
}


static void ts_hash_calc_blocks(struct ts_hash_state *ctx, const u8 *msg, u32 block_count)
{
	uint8_t block_word_len = (ctx->block_byte_len)>>2;

	/* set the input iterator data */
	if (1 != ctx->first_update_flag)
		ts_hash_set_iterator(ctx->iterator, ctx->iterator_word_len);

	/* set the bit length of the input blocks */
	ts_hash_set_tx_bit_len(((ctx->block_byte_len) << 3) * block_count);
	ts_hash_start(ctx);

	while (block_count--) {
		/* input the block message */
		ts_hash_input_msg((u8 *)msg, block_word_len);
		msg += ctx->block_byte_len;
	}

	ts_hash_wait_till_done();

	/* if message update not done, get the new iterator hash value */
	if (1 != ctx->finish_flag)
		ts_hash_get_iterator((u8 *)(ctx->iterator), ctx->iterator_word_len);

}

s32 ts_hash_init(struct ts_hash_state *mctx)
{
	ts_hash_set_work_mode(HASH_WORK_TYPE_CPU);
	ts_hash_set_endian_uint32(DATA_TYPE_BYTE_EXCHANGE);
	ts_hash_disable_cpu_intr();
	ts_hash_set_last_block(HASH_WORK_NO_LAST);
	ts_hash_set_alg(mctx->hash_type);

	mctx->block_byte_len = ts_hash_get_block_word_len(mctx->hash_type) << 2;
	mctx->iterator_word_len = ts_hash_get_iterator_word_len(mctx->hash_type);
	mctx->digest_byte_len = ts_hash_get_digest_word_len(mctx->hash_type) << 2;
	mctx->first_update_flag = 1;
	mctx->finish_flag = 0;
	ts_hash_u32_clear(mctx->total, mctx->block_byte_len / 32);

	return 0;
}

s32 ts_hash_update(struct ts_hash_state *mctx,
						const u8 *data, u32 len)
{
	u32 count;
	u8 left, fill;
	const u8 *msg = data;

	ts_hash_set_work_mode(HASH_WORK_TYPE_CPU);
	ts_hash_set_endian_uint32(DATA_TYPE_BYTE_EXCHANGE);
	ts_hash_disable_cpu_intr();
	ts_hash_set_last_block(HASH_WORK_NO_LAST); //set not the last block
	ts_hash_set_alg(mctx->hash_type);

	/* byte length of valid message left in block buffer */
	left = mctx->total[0] % (mctx->block_byte_len);
	/* byte length that block buffer need to fill a block */
	fill = (mctx->block_byte_len) - left;

	/* update total byte length */
	if (ts_hash_total_len_add_uint32(mctx->total,
				mctx->block_byte_len/32, len))
		return -1;

	if (left) {
		if (len >= fill) {
			memcpy(mctx->hash_buffer + left, (uint8_t *)msg, fill);
			ts_hash_calc_blocks(mctx, mctx->hash_buffer, 1);
			len -= fill;
			msg += fill;
		} else {
			memcpy(mctx->hash_buffer + left, (uint8_t *)msg, len);
			goto end;
		}
	}

	/* process some blocks */
	count = len / (mctx->block_byte_len);
	if (count)
		ts_hash_calc_blocks(mctx, msg, count);

	/* process the remainder */
	msg += (mctx->block_byte_len)*count;
	len = len % (mctx->block_byte_len);
	if (len)
		memcpy(mctx->hash_buffer, (uint8_t *)msg, len);
end:
	return 0;
}

s32 ts_hash_final(struct ts_hash_state *mctx, u8 *out)
{
	u8 tmp;

	ts_hash_set_work_mode(HASH_WORK_TYPE_CPU);
	ts_hash_set_endian_uint32(DATA_TYPE_BYTE_EXCHANGE);
	ts_hash_disable_cpu_intr();
	ts_hash_set_last_block(HASH_WORK_NO_LAST);//set not the last block
	ts_hash_set_alg(mctx->hash_type);

	mctx->finish_flag = 1;	  //the last block calc

	/* get the byte length of the remainder msg(less than one block) */
	tmp = mctx->total[0] % (mctx->block_byte_len);

	/* set total msg bit length */
	ts_hash_total_bytelen_2_bitlen(mctx->total,
					(mctx->block_byte_len) / 32);
	ts_hash_set_msg_total_bit_len(mctx->total,
					(mctx->block_byte_len) / 32);

	/* input the remainder msg(less than one block) */
	ts_hash_calc_rand_len_msg(mctx, mctx->hash_buffer, tmp);

	/* get the hash result */
	ts_hash_get_iterator(out, (mctx->digest_byte_len)>>2);

	/* clear the context */
    memset(mctx, 0, sizeof(*mctx));

	return 0;
}

static int ts_hash_clk_enable(struct udevice *dev)
{
	int ret;
	struct clk clk;

#if CONFIG_IS_ENABLED(CLK)
	ret = clk_get_by_name(dev, "hash_clk", &clk);
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

static int ts_hash_probe(struct udevice *dev)
{
	int ret;
	void __iomem *base;

	base = dev_remap_addr_index(dev, 0);
	if (!base)
		return -ENODEV;

	ts_hash_priv_t *ts_hash_priv = &s_ts_hash_instance;
	ts_hash_priv->regs = base;
	g_ts_hash = ts_hash_priv;

	ret = ts_hash_clk_enable(dev);
	if (ret) {
		dev_err(dev, "failed to enable clock %d\n", ret);
		return ret;
	}

	return 0;
}


static const struct udevice_id ts_hash_match[] = {
	{ .compatible = "tsingmicro,ts-hash" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(ts_hash) = {
	.name = "ts_hash",
	.id = UCLASS_MISC,
	.of_match = ts_hash_match,
	.probe	= ts_hash_probe,
};

