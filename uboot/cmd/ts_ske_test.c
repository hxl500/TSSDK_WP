// SPDX-License-Identifier: GPL-2.0+
/*
 * Tsingmicro Crypto SKE Driver
 *
 * Copyright (C) 2023
 * author : yangguoyu@tsingmicro.com
 * data: 2023-10-19
 * version: 1.0
 */
#include <common.h>
#include <command.h>
#include <console.h>
#include <linux/errno.h>
#include <dm/uclass.h>
#include <dm/device.h>

#include "ts_ske.h"
#include "ts_ske_test.h"

s32 ts_ske_test_entry(enum ske_alg_type alg_type,
				enum ske_mode_type alg_mode_type,
				enum ske_key_size key_size,
				u8 *src,
				u32 src_len,
				u8 *compare_result,
				u8 *key,
				u8 *iv,
				enum ske_sp_en sp_en)
{
	s32 iRet = -1;
	struct ts_ske_aes_state mctx;
	u8 encrypto_buffer[128]; // after encrypto;
	u8 decrypto_buffer[128]; // after decrypto;

	memset(&mctx, 0, sizeof(mctx));
	mctx.alg_type		= alg_type;
	mctx.alg_mode_type  = alg_mode_type;
	mctx.key_size		= key_size;
	mctx.dec_type		= AES_DIR_ENCRYPT;
	mctx.src = src;
	mctx.dst = encrypto_buffer;
	mctx.len = src_len;
	mctx.key = key;
	mctx.iv  = iv;
	mctx.sp_en = sp_en;

	iRet = ts_ske_crypto(&mctx);
	if(iRet) {
		printf("osr_ske_crypto encrypto failed\n");
		return -1;
	}

	if(memcmp(encrypto_buffer, compare_result, mctx.len))
	{
		printf("osr_ske_crypto encrypto do not macth\n");
		return -1;
	}

	memset(&mctx, 0, sizeof(mctx));
	mctx.alg_type		= alg_type;
	mctx.alg_mode_type  = alg_mode_type;
	mctx.key_size		= key_size;
	mctx.dec_type		= AES_DIR_DECRYPT;
	mctx.src = encrypto_buffer;
	mctx.dst = decrypto_buffer;
	mctx.len = src_len;
	mctx.key = key;
	mctx.iv  = iv;
	mctx.sp_en = sp_en;

	iRet = ts_ske_crypto(&mctx);
	if(iRet) {
		printf("osr_ske_crypto encrypto failed\n");
		return -1;
	}

	if(memcmp(decrypto_buffer, src, mctx.len))
	{
		printf("osr_ske_crypto decrypto do not macth\n");
		return -1;
	}

	return 0;

}

s32 ts_ske_aes_256_test(void)
{
	s32 iRet = -1;

	iRet = ts_ske_test_entry(SKE_TYPE_AES, CBC, SKE_KEY_SIZE_256_BIT,
							std_aes256_in, 128, std_aes256_cbc_out,
							std_aes256_key, std_aes256_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry aes cbc failed!\r\n");
	else
		printf("ts_ske_test_entry aes cbc ok!\r\n");

	iRet = ts_ske_test_entry(SKE_TYPE_AES, ECB, SKE_KEY_SIZE_256_BIT,
							std_aes256_in, 128, std_aes256_ecb_out,
							std_aes256_key, std_aes256_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry aes ecb failed!\r\n");
	else
		printf("ts_ske_test_entry aes ecb ok!\r\n");

	iRet = ts_ske_test_entry(SKE_TYPE_AES, CFB, SKE_KEY_SIZE_256_BIT,
							std_aes256_in, 128, std_aes256_cfb_out,
							std_aes256_key, std_aes256_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry aes cfb failed!\r\n");
	else
		printf("ts_ske_test_entry aes cfb ok!\r\n");

	iRet = ts_ske_test_entry(SKE_TYPE_AES, OFB, SKE_KEY_SIZE_256_BIT,
							std_aes256_in, 128, std_aes256_ofb_out,
							std_aes256_key, std_aes256_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry aes ofb failed!\r\n");
	else
		printf("ts_ske_test_entry aes ofb ok!\r\n");

	iRet = ts_ske_test_entry(SKE_TYPE_AES, CTR, SKE_KEY_SIZE_256_BIT,
							std_aes256_in, 128, std_aes256_ctr_out,
							std_aes256_key, std_aes256_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry aes ctr failed!\r\n");
	else
		printf("ts_ske_test_entry aes ctr ok!\r\n");

	iRet = ts_ske_test_entry(SKE_TYPE_AES, XTS_NORMAL, SKE_KEY_SIZE_256_BIT,
							std_aes256_in, 128, std_aes256_xts_out,
							std_aes256_key, std_aes256_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry aes xts failed!\r\n");
	else
		printf("ts_ske_test_entry aes xts ok!\r\n");

	return iRet;
}

s32 ts_ske_sm4_test(void)
{
	s32 iRet = -1;

	iRet = ts_ske_test_entry(SKE_TYPE_SM4, CBC, SKE_KEY_SIZE_128_BIT,
							std_sm4_in, 128, std_sm4_cbc_out,
							std_sm4_key, std_sm4_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry sm4 cbc failed!\r\n");
	else
		printf("ts_ske_test_entry sm4 cbc ok!\r\n");

	iRet = ts_ske_test_entry(SKE_TYPE_SM4, ECB, SKE_KEY_SIZE_128_BIT,
							std_sm4_in, 128, std_sm4_ecb_out,
							std_sm4_key, std_sm4_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry sm4 ecb failed!\r\n");
	else
		printf("ts_ske_test_entry sm4 ecb ok!\r\n");

	iRet = ts_ske_test_entry(SKE_TYPE_SM4, CFB, SKE_KEY_SIZE_128_BIT,
							std_sm4_in, 128, std_sm4_cfb_out,
							std_sm4_key, std_sm4_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry sm4 cfb failed!\r\n");
	else
		printf("ts_ske_test_entry sm4 cfb ok!\r\n");

	iRet = ts_ske_test_entry(SKE_TYPE_SM4, OFB, SKE_KEY_SIZE_128_BIT,
							std_sm4_in, 128, std_sm4_ofb_out,
							std_sm4_key, std_sm4_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry sm4 ofb failed!\r\n");
	else
		printf("ts_ske_test_entry sm4 ofb ok!\r\n");

	iRet = ts_ske_test_entry(SKE_TYPE_SM4, CTR, SKE_KEY_SIZE_128_BIT,
							std_sm4_in, 128, std_sm4_ctr_out,
							std_sm4_key, std_sm4_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry sm4 ctr failed!\r\n");
	else
		printf("ts_ske_test_entry sm4 ctr ok!\r\n");

	iRet = ts_ske_test_entry(SKE_TYPE_SM4, XTS_NORMAL, SKE_KEY_SIZE_128_BIT,
							std_sm4_in, 128, std_sm4_xts_out,
							std_sm4_key, std_sm4_iv, SKE_SP_DISABLE);
	if(iRet)
		printf("ts_ske_test_entry sm4 xts failed!\r\n");
	else
		printf("ts_ske_test_entry sm4 xts ok!\r\n");

	return iRet;

}

static int do_ts_ske_test(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{

	s32 iRet = -1;

	iRet = ts_ske_aes_256_test();
	if(iRet)
		printf("ts_ske_aes_256_test failed!\r\n");

	iRet = ts_ske_sm4_test();
	if(iRet)
		printf("ts_ske_sm4_test failed!\r\n");
	else
		printf("ts_ske_sm4_test ok!\r\n");

	return 0;
}

U_BOOT_CMD(
	ts_ske_test, CONFIG_SYS_MAXARGS, 0, do_ts_ske_test,
	"ts_ske test system",
	     "ts_ske_test\n"
);

