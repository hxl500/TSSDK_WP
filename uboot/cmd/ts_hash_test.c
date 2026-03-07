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

#include "ts_hash.h"
#include "ts_hash_test.h"

#define TS_HASH_MD5_DIGEST_LEN (16)
#define TS_HASH_SM3_DIGEST_LEN (32)
#define TS_HASH_SHA256_DIGEST_LEN (32)

static void ts_hash_print_buf_U8(u8 buf[], u32 byteLen, char name[])
{
	u32 i;

	printf("%s\r\n",name);
	for (i = 0; i < byteLen; i++) {
		printf("%02x", buf[i]);
	}
	printf("\r\n");
}

s32 ts_hash_md5_test(void)
{
	s32 iRet = -1;
	s32 i;
	struct ts_hash_state mctx;
	u8 message1[3] = {0x61,0x62,0x63};
	u8 message2[500] = {0};
	u8 digest[64] = {0};

	for(i=0;i<500;i++)
		message2[i] = i;

	/* ************ md5 case1 ************ */
	mctx.hash_type = HASH_TYPE_MD5;
	iRet = ts_hash_init(&mctx);
	iRet |= ts_hash_update(&mctx, message1, 3);
	iRet |= ts_hash_final(&mctx, digest);
	if (0 != iRet || memcmp(digest, std_md5_digest1, TS_HASH_MD5_DIGEST_LEN)) {
		printf("\r\n %s test 1 failure, ret=%d\r\n", "md5", iRet);
		ts_hash_print_buf_U8(message1, 3, "message1");
		ts_hash_print_buf_U8(digest, TS_HASH_MD5_DIGEST_LEN, "digest");
		ts_hash_print_buf_U8(std_md5_digest1, TS_HASH_MD5_DIGEST_LEN, "std_md5_digest1");

		return -1;
	}

	/* ************ md5 case2 ************ */
	memset(digest, 0, sizeof(digest));
	mctx.hash_type = HASH_TYPE_MD5;
	iRet = ts_hash_init(&mctx);
	iRet |= ts_hash_update(&mctx, message2, 500);
	iRet |= ts_hash_final(&mctx, digest);
	if (0 != iRet || memcmp(digest, std_md5_digest2, TS_HASH_MD5_DIGEST_LEN)) {
		printf("\r\n %s test 2 failure, ret=%d\r\n", "md5", iRet);
		ts_hash_print_buf_U8(message2, 500, "message2");
		ts_hash_print_buf_U8(digest, TS_HASH_MD5_DIGEST_LEN, "digest");
		ts_hash_print_buf_U8(std_md5_digest2, TS_HASH_MD5_DIGEST_LEN, "std_md5_digest2");
		return -2;
	}

	return 0;

}

s32 ts_hash_sm3_test(void)
{
	s32 iRet = -1;
	s32 i;
	struct ts_hash_state mctx;
	u8 message1[3] = {0x61,0x62,0x63};
	u8 message2[500] = {0};
	u8 digest[64] = {0};

	for(i=0;i<500;i++)
		message2[i] = i;

	/* ************ sm3 case1 ************ */
	mctx.hash_type = HASH_TYPE_SM3;
	iRet = ts_hash_init(&mctx);
	iRet |= ts_hash_update(&mctx, message1, 3);
	iRet |= ts_hash_final(&mctx, digest);
	if (0 != iRet || memcmp(digest, std_sm3_digest1, TS_HASH_SM3_DIGEST_LEN)) {
		printf("\r\n %s test 1 failure, ret=%d\r\n", "sm3", iRet);
		ts_hash_print_buf_U8(message1, 3, "message1");
		ts_hash_print_buf_U8(digest, TS_HASH_MD5_DIGEST_LEN, "digest");
		ts_hash_print_buf_U8(std_sm3_digest1, TS_HASH_MD5_DIGEST_LEN, "std_sm3_digest1");

		return -1;
	}

	/* ************ sm3 case2 ************ */
	memset(digest, 0, sizeof(digest));
	mctx.hash_type = HASH_TYPE_SM3;
	iRet = ts_hash_init(&mctx);
	iRet |= ts_hash_update(&mctx, message2, 500);
	iRet |= ts_hash_final(&mctx, digest);
	if (0 != iRet || memcmp(digest, std_sm3_digest2, TS_HASH_SM3_DIGEST_LEN)) {
		printf("\r\n %s test 2 failure, ret=%d\r\n", "sm3", iRet);
		ts_hash_print_buf_U8(message2, 500, "message2");
		ts_hash_print_buf_U8(digest, TS_HASH_SM3_DIGEST_LEN, "digest");
		ts_hash_print_buf_U8(std_sm3_digest2, TS_HASH_SM3_DIGEST_LEN, "std_sm3_digest2");
		return -2;
	}

	return 0;

}

s32 ts_hash_sha256_test(void)
{
	s32 iRet = -1;
	s32 i;
	struct ts_hash_state mctx;
	u8 message1[3] = {0x61,0x62,0x63};
	u8 message2[500] = {0};
	u8 digest[64] = {0};

	for(i=0;i<500;i++)
		message2[i] = i;

	/* ************ sm3 case1 ************ */
	mctx.hash_type = HASH_TYPE_SHA256;
	iRet = ts_hash_init(&mctx);
	iRet |= ts_hash_update(&mctx, message1, 3);
	iRet |= ts_hash_final(&mctx, digest);
	if (0 != iRet || memcmp(digest, std_sha256_digest1, TS_HASH_SHA256_DIGEST_LEN)) {
		printf("\r\n %s test 1 failure, ret=%d\r\n", "sha256", iRet);
		ts_hash_print_buf_U8(message1, 3, "message1");
		ts_hash_print_buf_U8(digest, TS_HASH_SHA256_DIGEST_LEN, "digest");
		ts_hash_print_buf_U8(std_sha256_digest1, TS_HASH_SHA256_DIGEST_LEN, "std_sha256_digest1");

		return -1;
	}

	/* ************ sm3 case2 ************ */
	memset(digest, 0, sizeof(digest));
	mctx.hash_type = HASH_TYPE_SHA256;
	iRet = ts_hash_init(&mctx);
	iRet |= ts_hash_update(&mctx, message2, 500);
	iRet |= ts_hash_final(&mctx, digest);
	if (0 != iRet || memcmp(digest, std_sha256_digest2, TS_HASH_SHA256_DIGEST_LEN)) {
		printf("\r\n %s test 2 failure, ret=%d\r\n", "sha256", iRet);
		ts_hash_print_buf_U8(message2, 500, "message2");
		ts_hash_print_buf_U8(digest, TS_HASH_SHA256_DIGEST_LEN, "digest");
		ts_hash_print_buf_U8(std_sha256_digest2, TS_HASH_SHA256_DIGEST_LEN, "std_sha256_digest1");
		return -2;
	}

	return 0;

}

static int do_ts_hash_test(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{

	s32 iRet = -1;

	iRet = ts_hash_md5_test();
	if(iRet == 0)
		printf("ts_hash_md5_test test ok!\r\n");
	else
		printf("ts_hash_md5_test test failed!\r\n");

	iRet = ts_hash_sm3_test();
	if(iRet == 0)
		printf("ts_hash_sm3_test test ok!\r\n");
	else
		printf("ts_hash_sm3_test test failed!\r\n");

	iRet = ts_hash_sha256_test();
	if(iRet == 0)
		printf("ts_hash_sha256_test test ok!\r\n");
	else
		printf("ts_hash_sha256_test test failed!\r\n");

	return 0;
}

U_BOOT_CMD(
	ts_hash_test, CONFIG_SYS_MAXARGS, 0, do_ts_hash_test,
	"ts_hash test system",
	     "ts_hash_test\n"
);
