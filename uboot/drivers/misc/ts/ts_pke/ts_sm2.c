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

#include "ts_utility.h"
#include "ts_sm2.h"
#include "ts_pke.h"
#include "ts_hash.h"
#include "ts_trng.h"
#include "ts_eccp_curve.h"

#define sm2_default_id_byte_len    (16)
static const char sm2_default_id[] = "1234567812345678";

/* function: a=a+1 (for 1+dA in SM2 signing)
 * parameters:
 *     a -------------------------- input, destination data
 *     wordLen -------------------- input, word length of data
 * return: none
 * caution:
 *     1. if a of wordLen words can not hold the carry, then the carry will be discarded,
 *        actually this is used in sm2 signing(1+dA)
 */
void uint32_BigNum_Add_One(u32 *a, u32 wordLen)
{
	u32 i, carry;

	carry = 1;
	for(i=0; i<wordLen; i++)
	{
		a[i] += carry;
		if(a[i] < carry)
		{
			carry = 1;
		}
		else
		{
			break;
		}
	}
}


/* function: a=a+1 (for SM2 KDF counter addition)
 * parameters:
 *     a[4] ----------------------- input, count of 4 bytes, big-endian
 * return: none
 * caution: if a of 4 bytes can not hold the carry, then the carry will be discarded
 */
void sm2_kdf_counter_add_one(u8 a[4])
{
	s32 i;
	u8 carry;

	carry = 1;
	for(i=3; i>=0; i--)
	{
		a[i] += carry;
		if(a[i] < carry)
		{
			carry = 1;
		}
		else
		{
			break;
		}
	}
}


/* function: SM2 kdf (for SM2 encrypting, decrypting and key exchange)
 * parameters:
 *     in ------------------------- input, sm2 kdf input
 *     inByteLen ------------------ input, byte length of in
 *     k -------------------------- output, output key
 *     kByteLen ------------------- input, byte length of output key
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1.
 */
u8 sm2_kdf(u8 *in , u32 inByteLen, u8 *k, u32 kByteLen)
{
	u8 digest[SM3_DIGEST_BYTE_LEN];
	u32 i, t;
	u8 counter[4] = {0x00,0x00,0x00,0x01};      // count = 1;
	struct ts_hash_state mctx;
	u8 ret;
	mctx.hash_type = HASH_TYPE_SM3;
	t = kByteLen>>5;                                 // t = kByteLen/32;
	for(i=0; i<t; i++)
	{
		ret = ts_hash_init(&mctx);
		if(TS_HASH_SUCCESS != ret)
		{
			return ret;
		}

		ret = ts_hash_update(&mctx, in, inByteLen);
		if(TS_HASH_SUCCESS != ret)
		{
			return ret;
		}

		ret = ts_hash_update(&mctx, counter, 4);
		if(TS_HASH_SUCCESS != ret)
		{
			return ret;
		}

		ret = ts_hash_final(&mctx, k+(i<<5));
		if(TS_HASH_SUCCESS != ret)
		{
			return ret;
		}

		sm2_kdf_counter_add_one(counter);
	}

	kByteLen = kByteLen & 0x1F;
	if(kByteLen)
	{
		ret = ts_hash_init(&mctx);
		if(TS_HASH_SUCCESS != ret)
		{
			return ret;
		}

		ret = ts_hash_update(&mctx, in, inByteLen);
		if(TS_HASH_SUCCESS != ret)
		{
			return ret;
		}

		ret = ts_hash_update(&mctx, counter, 4);
		if(TS_HASH_SUCCESS != ret)
		{
			return ret;
		}

		ret = ts_hash_final(&mctx, digest);
		if(TS_HASH_SUCCESS != ret)
		{
			return ret;
		}

		memcpy_(k + (t<<5), digest, kByteLen);
	}

	return SM2_SUCCESS;
}


/* function: get SM2 Z value = SM3(bitLenofID||ID||a||b||Gx||Gy||Px||Py)
 * parameters:
 *     ID ------------------------- input, User ID
 *     byteLenofID ---------------- input, byte length of ID, must be less than 2^13
 *     pubKey --------------------- input, public key(0x04 + x + y), 65 bytes, big-endian
 *     Z -------------------------- output, Z value, SM3 digest, 32 bytes
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. bit length of ID must be less than 2^16, thus byte length must be less than 2^13
 *     2. if ID is NULL, then replace it with sm2 default ID
 *     3. please make sure the pubKey is valid
 */
u8 sm2_getZ(u8 *ID, u32 byteLenofID, u8 pubKey[65], u8 Z[32])
{
	u32 tmp[SM2_WORD_LEN];
	struct ts_hash_state mctx;
	u8 ret;
	mctx.hash_type = HASH_TYPE_SM3;

	if(NULL == pubKey || NULL == Z)
	{
		return SM2_BUFFER_NULL;
	}

	if(NULL == ID)
	{
		ID = (u8 *)sm2_default_id;
		byteLenofID = sm2_default_id_byte_len;
	}
	else if((0 == byteLenofID) || (byteLenofID >= SM2_MAX_ID_BYTE_LEN))
	{
		return SM2_INPUT_INVALID;
	}

	if(POINT_NOT_COMPRESSED != pubKey[0])
	{
		return SM2_INPUT_INVALID;
	}

	ret = ts_hash_init(&mctx);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	byteLenofID <<= 3;
	ret = (byteLenofID>>8) & 0xFF;
	ret = ts_hash_update(&mctx, (u8 *)&ret, 1);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = byteLenofID & 0xFF;
	ret = ts_hash_update(&mctx, (u8 *)&ret, 1);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	byteLenofID >>= 3;
	ret = ts_hash_update(&mctx, ID, byteLenofID);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	reverse_byte_array((u8 *)sm2_curve->eccp_a, (u8 *)tmp, SM2_BYTE_LEN);

	ret = ts_hash_update(&mctx, (u8 *)tmp, SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	reverse_byte_array((u8 *)sm2_curve->eccp_b, (u8 *)tmp, SM2_BYTE_LEN);
	ret = ts_hash_update(&mctx, (u8 *)tmp, SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	reverse_byte_array((u8 *)sm2_curve->eccp_Gx, (u8 *)tmp, SM2_BYTE_LEN);

	ret = ts_hash_update(&mctx, (u8 *)tmp, SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}


	reverse_byte_array((u8 *)sm2_curve->eccp_Gy, (u8 *)tmp, SM2_BYTE_LEN);

	ret = ts_hash_update(&mctx, (u8 *)tmp, SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = ts_hash_update(&mctx, pubKey+1, SM2_BYTE_LEN<<1);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = ts_hash_final(&mctx, Z);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = SM2_SUCCESS;

end:

	return ret;
}


/* function: get SM2 E value = SM3(Z||M)
 * parameters:
 *     M      --------------------- input, Message
 *     byteLen -------------------- input, byte length of M
 *     Z      --------------------- input, Z value, 32 bytes
 *     E      --------------------- output, E value, 32 bytes
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 */
u8 sm2_getE(u8 *M, u32 byteLen, u8 Z[32], u8 E[32])
{
	struct ts_hash_state mctx;
	u8 ret;
	mctx.hash_type = HASH_TYPE_SM3;

	if(NULL == M || NULL == Z || NULL == E)
	{
		return SM2_BUFFER_NULL;
	}

	if(0 == byteLen)
	{
		return SM2_INPUT_INVALID;
	}

	ret = ts_hash_init(&mctx);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = ts_hash_update(&mctx, Z, 32);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = ts_hash_update(&mctx, M, byteLen);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = ts_hash_final(&mctx, E);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = SM2_SUCCESS;

end:

	return ret;
}


/* function: Generate SM2 public key from private key
 * parameters:
 *     priKey --------------------- input, private key, 32 bytes, big-endian
 *     pubKey --------------------- output, public key(0x04 + x + y), 65 bytes, big-endian
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 */
u8 sm2_get_pubkey_from_prikey(u8 priKey[32], u8 pubKey[65])
{
	u8 ret;

	if(NULL == priKey || NULL == pubKey)
	{
		return SM2_BUFFER_NULL;
	}

	ret = eccp_get_pubkey_from_prikey(sm2_curve, priKey, pubKey+1);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pubKey[0] = POINT_NOT_COMPRESSED;

	return SM2_SUCCESS;
}


/* function: Generate SM2 random Key pair
 * parameters:
 *     priKey --------------------- output, private key, 32 bytes, big-endian
 *     pubKey --------------------- output, public key(0x04 + x + y), 65 bytes, big-endian
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 */
u8 sm2_getkey(u8 priKey[32], u8 pubKey[65])
{
	u8 ret;

#if 1
	if(NULL == priKey || NULL == pubKey)
	{
		return SM2_BUFFER_NULL;
	}

	ret = eccp_getkey(sm2_curve, priKey, pubKey+1);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pubKey[0] = POINT_NOT_COMPRESSED;

	return SM2_SUCCESS;
#else

	u32 k[SM2_WORD_LEN], tmp[SM2_WORD_LEN<<1];

	if(NULL == priKey || NULL == pubKey)
	{
		return SM2_BUFFER_NULL;
	}

SM2_GETKEY_LOOP:

	ret = get_rand((u8 *)k, SM2_BYTE_LEN);
	if(TRNG_SUCCESS != ret)
	{
		return ret;
	}

	//make sure priKey in [1, n-2]
	if(uint32_BigNum_Check_Zero(k, SM2_WORD_LEN))
	{
		goto SM2_GETKEY_LOOP;
	}
	if(uint32_BigNumCmp(k, SM2_WORD_LEN, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN) >= 0)
	{
		goto SM2_GETKEY_LOOP;
	}

#ifdef SM2_HIGH_SPEED
	ret = eccp_pointMul_base(sm2_curve, k, tmp, tmp+SM2_WORD_LEN);
#else
	ret = eccp_pointMul(sm2_curve, k, sm2_curve->eccp_Gx, sm2_curve->eccp_Gy, tmp, tmp+SM2_WORD_LEN);
#endif
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pubKey[0] = POINT_NOT_COMPRESSED;
#ifdef PKE_BIG_ENDIAN
	convert_word_array((u8 *)k, k, SM2_WORD_LEN);
	memcpy_(priKey, k, SM2_BYTE_LEN);
	convert_word_array((u8 *)tmp, k, SM2_WORD_LEN);
	memcpy(pubKey+1, k, SM2_BYTE_LEN);
	convert_word_array((u8 *)(tmp+SM2_WORD_LEN), k, SM2_WORD_LEN);
	memcpy_(pubKey+1+SM2_BYTE_LEN, k, SM2_BYTE_LEN);
#else
	convert_byte_array((u8 *)k, priKey, SM2_BYTE_LEN);
	convert_byte_array((u8 *)tmp, pubKey+1, SM2_BYTE_LEN);
	convert_byte_array((u8 *)(tmp+SM2_WORD_LEN), pubKey+1+SM2_BYTE_LEN, SM2_BYTE_LEN);
#endif
	return SM2_SUCCESS;
#endif
}


/* function: Generate SM2 Signature r and s with rand k
 * parameters:
 *     e[8]   --------------------- input, e value, 8 words, little-endian
 *     k[8]   --------------------- input, random number k, 8 words, little-endian
 *     dA[8]  --------------------- input, private key, 8 words, little-endian
 *     r[8]   --------------------- output, Signature r, 8 words, little-endian
 *     s[8]   --------------------- output, Signature s, 8 words, little-endian
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. e and dA can not be modified
 *     2. e must be less than n(order of the SM2 curve)
 *     3. dA must be in [1, n-2]
 */
u8 sm2_sign_with_k(u32 e[8], u32 k[8], u32 dA[8], u32 r[8], u32 s[8])
{
	u32 tmp1[SM2_WORD_LEN], tmp2[SM2_WORD_LEN];
	u8 ret;

	if(NULL == e || NULL == k || NULL == dA || NULL == r || NULL == s)
	{
		return SM2_BUFFER_NULL;
	}

	//make sure k in [1, n-1]
	if(uint32_BigNum_Check_Zero(k, SM2_WORD_LEN))
	{
		return SM2_ZERO_ALL;
	}
	if(uint32_BigNumCmp(k, SM2_WORD_LEN, (u32 *)sm2_curve->eccp_n, SM2_WORD_LEN) >= 0)
	{
		return SM2_INTEGER_TOO_BIG;
	}

	ret = eccp_pointMul(sm2_curve, k, sm2_curve->eccp_Gx, sm2_curve->eccp_Gy, tmp1, NULL);

	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//tmp1 = x1 mod n
	if(uint32_BigNumCmp(tmp1, SM2_WORD_LEN, (u32 *)sm2_curve->eccp_n, SM2_WORD_LEN) >= 0)
	{
		ret = pke_sub(tmp1, (u32 *)sm2_curve->eccp_n, tmp1, SM2_WORD_LEN);
		if(PKE_SUCCESS != ret)
		{
			return ret;
		}
	}

	//r = e + x1 mod n
	ret = pke_modadd((u32 *)sm2_curve->eccp_n, e, tmp1, r, SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//make sure r is not zero
	if(uint32_BigNum_Check_Zero(r, SM2_WORD_LEN))
	{
		return SM2_ZERO_ALL;
	}

	//tmp1 = r + k mod n
	ret = pke_modadd((u32 *)sm2_curve->eccp_n, r, k, tmp1, SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}
	//make sure r+k is not n
	if(uint32_BigNum_Check_Zero(tmp1, SM2_WORD_LEN))
	{
		return SM2_ZERO_ALL;
	}

	//tmp1 =  r*dA mod n
	pke_load_pre_calc_mont((u32 *)sm2_curve->eccp_n_h, (u32 *)sm2_curve->eccp_n_n1, SM2_WORD_LEN);
	pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
	ret = pke_modmul_internal((u32 *)sm2_curve->eccp_n, r, dA, tmp1, SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//tmp1 =  (k - r*dA) mod n
	ret = pke_modsub((u32 *)sm2_curve->eccp_n, k, tmp1, tmp1, SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//tmp2 = (1+dA)^(-1) mod n
	uint32_copy(tmp2, dA, SM2_WORD_LEN);
	uint32_BigNum_Add_One(tmp2, SM2_WORD_LEN);
	ret = pke_modinv((u32 *)sm2_curve->eccp_n, tmp2, tmp2, SM2_WORD_LEN, SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//s = ((1+dA)^(-1))*(k - r*dA) mod n
	ret = pke_modmul_internal((u32 *)sm2_curve->eccp_n, tmp1, tmp2, s, SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//make sure s is not zero
	if(uint32_BigNum_Check_Zero(s, SM2_WORD_LEN))
	{
		return SM2_ZERO_ALL;
	}

	return SM2_SUCCESS;
}


/* function: Generate SM2 Signature
 * parameters:
 *     E[32] ---------------------- input, E value, 32 bytes, big-endian
 *     rand_k[32] ----------------- input, random big integer k in signing, 32 bytes, big-endian,
 *                                  if you do not have this integer, please set this parameter to be NULL,
 *                                  it will be generated inside.
 *     priKey[32] ----------------- input, private key, 32 bytes, big-endian
 *     signature[64] -------------- output, Signature r and s, 64 bytes, big-endian
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. if you do not have rand_k, please set the parameter to be NULL, it will be generated inside.
 */
u8 sm2_sign(u8 E[32], u8 rand_k[32], u8 priKey[32], u8 signature[64])
{
	u32 e[SM2_WORD_LEN], k[SM2_WORD_LEN], dA[SM2_WORD_LEN], r[SM2_WORD_LEN], s[SM2_WORD_LEN];
	u8 ret;

	if(NULL == E || NULL == priKey || NULL == signature)
	{
		return SM2_BUFFER_NULL;
	}

    //e = e mod n
	reverse_byte_array(E, (u8 *)e, SM2_BYTE_LEN);

	if(uint32_BigNumCmp(e, SM2_WORD_LEN, (u32 *)sm2_curve->eccp_n, SM2_WORD_LEN) >= 0)
	{
		ret = pke_sub(e, (u32 *)sm2_curve->eccp_n, e, SM2_WORD_LEN);
		if(PKE_SUCCESS != ret)
		{
			return ret;
		}
	}

	//make sure priKey in [1, n-2]
	reverse_byte_array(priKey, (u8 *)dA, SM2_BYTE_LEN);
	if(uint32_BigNum_Check_Zero(dA, SM2_WORD_LEN))
	{
		return SM2_ZERO_ALL;
	}
	if(uint32_BigNumCmp(dA, SM2_WORD_LEN, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN) >= 0)
	{
		return SM2_INTEGER_TOO_BIG;
	}

	if(rand_k)
	{
		reverse_byte_array(rand_k, (u8 *)k, SM2_BYTE_LEN);
	}
	else
	{
SM2_SIGN_LOOP:

		ret = get_rand((u8 *)k, SM2_BYTE_LEN);
		if(TRNG_SUCCESS != ret)
		{
			return ret;
		}
	}

	ret = sm2_sign_with_k(e, k, dA, r, s);
	if(SM2_ZERO_ALL == ret || SM2_INTEGER_TOO_BIG == ret)
	{
		if(NULL == rand_k)
		{
			goto SM2_SIGN_LOOP;
		}
	}
	if(SM2_SUCCESS != ret)
	{
		return ret;
	}

	reverse_byte_array((u8 *)r, signature, SM2_BYTE_LEN);
	reverse_byte_array((u8 *)s, signature+SM2_BYTE_LEN, SM2_BYTE_LEN);

	return SM2_SUCCESS;
}


/* function: Verify SM2 Signature
 * parameters:
 *     E[32] ---------------------- input, E value, 32 bytes, big-endian
 *     pubKey[65] ----------------- input, public key(0x04 + x + y), 65 bytes, big-endian
 *     signature[64] -------------- input, Signature r and s, 64 bytes, big-endian
 * return:
 *     SM2_SUCCESS(success, the signature is valid); other(error or the signature is invalid)
 * caution:
 */
u8 sm2_verify(u8 E[32], u8 pubKey[65], u8 signature[64])
{
	u32 e[SM2_WORD_LEN], r[SM2_WORD_LEN], s[SM2_WORD_LEN], tmp[SM2_WORD_LEN*4];
	u32 *t = e;
	u8 ret;

	if(NULL == E || NULL == pubKey || NULL == signature)
	{
		return SM2_BUFFER_NULL;
	}

	//make sure pubKey[0] is POINT_NOT_COMPRESSED
	if(POINT_NOT_COMPRESSED != pubKey[0])
	{
		return SM2_INPUT_INVALID;
	}

	//get PA and check PA
	reverse_byte_array(pubKey+1, (u8 *)(tmp+2*SM2_WORD_LEN), SM2_BYTE_LEN);
	reverse_byte_array(pubKey+1+SM2_BYTE_LEN, (u8 *)(tmp+3*SM2_WORD_LEN), SM2_BYTE_LEN);
	ret = eccp_pointVerify(sm2_curve, (u32 *)(tmp+2*SM2_WORD_LEN), (u32 *)(tmp+3*SM2_WORD_LEN));
	if(PKE_SUCCESS != ret)
	{
		return SM2_NOT_ON_CURVE;
	}

	//make sure r in [1, n-1]
	reverse_byte_array(signature, (u8 *)r, SM2_BYTE_LEN);
	if(uint32_BigNum_Check_Zero(r, SM2_WORD_LEN))
	{
		ret = SM2_ZERO_ALL;
		goto end;
	}
	if(uint32_BigNumCmp(r, SM2_WORD_LEN, (u32 *)sm2_curve->eccp_n, SM2_WORD_LEN) >= 0)
	{
		ret = SM2_INTEGER_TOO_BIG;
		goto end;
	}

	//make sure s in [1, n-1]
	reverse_byte_array(signature+SM2_BYTE_LEN, (u8 *)s, SM2_BYTE_LEN);
	if(uint32_BigNum_Check_Zero(s, SM2_WORD_LEN))
	{
		ret = SM2_ZERO_ALL;
		goto end;
	}
	if(uint32_BigNumCmp(s, SM2_WORD_LEN, (u32 *)sm2_curve->eccp_n, SM2_WORD_LEN) >= 0)
	{
		ret = SM2_INTEGER_TOO_BIG;
		goto end;
	}

	//t = (r+s) mod n
	ret = pke_modadd((u32 *)sm2_curve->eccp_n, r, s, t, SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		goto end;
	}

	//if t is 0, refuse the signature
	if(uint32_BigNum_Check_Zero(t, SM2_WORD_LEN))
	{
		ret = SM2_ZERO_ALL;
		goto end;
	}
	//[s]G
	ret = eccp_pointMul(sm2_curve, s, sm2_curve->eccp_Gx, sm2_curve->eccp_Gy, tmp, tmp+SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		goto end;
	}

	//[t]PA
	ret = eccp_pointMul(sm2_curve, t, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN, tmp+2*SM2_WORD_LEN,
						tmp+3*SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		goto end;
	}

	//[s]G + [t]PA
	ret = eccp_pointAdd(sm2_curve, tmp, tmp+SM2_WORD_LEN, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN,
						tmp, NULL);
	if(PKE_SUCCESS != ret)
	{
		goto end;
	}

	//e = e mod n
	reverse_byte_array(E, (u8 *)e, SM2_BYTE_LEN);
	if(uint32_BigNumCmp(e, SM2_WORD_LEN, (u32 *)sm2_curve->eccp_n, SM2_WORD_LEN) >= 0)
	{
		ret = pke_sub(e, (u32 *)sm2_curve->eccp_n, e, SM2_WORD_LEN);
		if(PKE_SUCCESS != ret)
		{
			goto end;
		}
	}

	//tmp = x1 mod n
	if(uint32_BigNumCmp(tmp, SM2_WORD_LEN, (u32 *)sm2_curve->eccp_n, SM2_WORD_LEN) >= 0)
	{
		ret = pke_sub(tmp, (u32 *)sm2_curve->eccp_n, tmp, SM2_WORD_LEN);
		if(PKE_SUCCESS != ret)
		{
			goto end;
		}
	}

	//tmp = e + x1 mod n
	ret = pke_modadd((u32 *)sm2_curve->eccp_n, e, tmp, tmp, SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		goto end;
	}

	//cmp
	if(uint32_BigNumCmp(tmp, SM2_WORD_LEN, r, SM2_WORD_LEN))
	{
		ret = SM2_VERIFY_FAILED;
		goto end;
	}

	//success
	ret = SM2_SUCCESS;

end:

	return ret;
}


/* function: SM2 Encryption with rand k
 * parameters:
 *     M -------------------------- input, plaintext, MByteLen bytes, big-endian
 *     MByteLen ------------------- input, byte length of M
 *     k[8] ----------------------- input, random number k, 8 words, little-endian
 *     pubkey_x ------------------- input, x coordinate of public key point, 8 words, little-endian
 *     pubkey_y ------------------- input, y coordinate of public key point, 8 words, little-endian
 *     order ---------------------- input, either SM2_C1C3C2 or SM2_C1C2C3
 *     C -------------------------- output, ciphertext, CByteLen bytes, big-endian
 *     CByteLen ------------------- output, byte length of C, should be MByteLen+97 if success
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. M and C can not be the same buffer
 *     2. please make sure pubkey_x and pubkey_y are valid
 */
u8 sm2_encrypt_with_k(u8 *M, u32 MByteLen, u32 *k,
						   u32 *pubkey_x, u32 *pubkey_y,
						   sm2_cipher_order_e order,
						   u8 *C, u32 *CByteLen)
{
	u32 xy[SM2_WORD_LEN<<1];
	u8 *C2, *C3;
	struct ts_hash_state mctx;
	u8 ret;

	if(NULL == M || NULL == k || NULL == pubkey_x || NULL == pubkey_y || NULL == C || NULL == CByteLen)
	{
		return SM2_BUFFER_NULL;
	}

	if(MByteLen == 0)
	{
		return SM2_INPUT_INVALID;
	}

	if(order > SM2_C1C2C3)
	{
		return SM2_INPUT_INVALID;
	}

	if(M == C)
	{
		return SM2_IN_OUT_SAME_BUFFER;
	}

	//make sure k in [1, n-1]
	if(uint32_BigNum_Check_Zero(k, SM2_WORD_LEN))
	{
		return SM2_ZERO_ALL;
	}
	if(uint32_BigNumCmp(k, SM2_WORD_LEN, (u32 *)sm2_curve->eccp_n, SM2_WORD_LEN) >= 0)
	{
		return SM2_INTEGER_TOO_BIG;
	}

	//get [k]G
	ret = eccp_pointMul(sm2_curve, k, sm2_curve->eccp_Gx, sm2_curve->eccp_Gy, xy, xy+SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//output C1
	C[0] = POINT_NOT_COMPRESSED;
	reverse_byte_array((u8 *)xy, C+1, SM2_BYTE_LEN);
	reverse_byte_array((u8 *)(xy+SM2_WORD_LEN), C+1+SM2_BYTE_LEN, SM2_BYTE_LEN);

	//get [k]PB
	ret = eccp_pointMul(sm2_curve, k, pubkey_x, pubkey_y, xy, xy+SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//get C2
	reverse_byte_array((u8 *)xy, (u8 *)xy, SM2_BYTE_LEN);
	reverse_byte_array((u8 *)(xy+SM2_WORD_LEN), (u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN);

	C2 = C+1+2*SM2_BYTE_LEN + ((SM2_C1C2C3 == order)?0:SM2_BYTE_LEN);

	ret = sm2_kdf((u8 *)xy, SM2_BYTE_LEN<<1, C2, MByteLen);
	if(SM2_SUCCESS != ret)
	{
		return ret;
	}

	if(uint8_BigNum_Check_Zero(C2, MByteLen))
	{
		return SM2_ZERO_ALL;
	}

	uint8_XOR(C2, M, C2, MByteLen);

	//get C3
	C3 = C+1+2*SM2_BYTE_LEN +((SM2_C1C2C3 == order)?MByteLen:0);

	mctx.hash_type = HASH_TYPE_SM3;
	ret = ts_hash_init(&mctx);
	if(TS_HASH_SUCCESS != ret)
	{
		return ret;
	}

	ret = ts_hash_update(&mctx, (u8 *)xy, SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		return ret;
	}

	ret = ts_hash_update(&mctx, M, MByteLen);
	if(TS_HASH_SUCCESS != ret)
	{
		return ret;
	}

	ret = ts_hash_update(&mctx, (u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		return ret;
	}

	ret = ts_hash_final(&mctx, C3);
	if(TS_HASH_SUCCESS != ret)
	{
		return ret;
	}

	*CByteLen = MByteLen+1+3*SM2_BYTE_LEN;

	return SM2_SUCCESS;
}


/* function: SM2 Encryption
 * parameters:
 *     M -------------------------- input, plaintext, MByteLen bytes, big-endian
 *     MByteLen ------------------- input, byte length of M
 *     rand_k[32] ----------------- input, random big integer k in encrypting, 32 bytes, big-endian,
 *                                  if you do not have this integer, please set this parameter to be NULL,
 *                                  it will be generated inside.
 *     pubKey[65] ----------------- input, public key, 65 bytes, big-endian
 *     order ---------------------- input, either SM2_C1C3C2 or SM2_C1C2C3
 *     C -------------------------- output, ciphertext, CByteLen bytes, big-endian
 *     CByteLen ------------------- output, byte length of C, should be MByteLen+97 if success
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. M and C can not be the same buffer
 *     2. if you do not have rand_k, please set the parameter to be NULL, it will be generated inside.
 *     3. please make sure pubKey is valid
 */
u8 sm2_encrypt(u8 *M, u32 MByteLen, u8 rand_k[32], u8 pubKey[65],
					sm2_cipher_order_e order, u8 *C, u32 *CByteLen)
{
	u32 k[SM2_WORD_LEN];
	u32 pubkey_x[SM2_WORD_LEN],pubkey_y[SM2_WORD_LEN];
	u8 ret;

	if(NULL == pubKey)
	{
		return SM2_BUFFER_NULL;
	}

	if(POINT_NOT_COMPRESSED != pubKey[0])
	{
		return SM2_INPUT_INVALID;
	}

	reverse_byte_array(pubKey+1, (u8 *)pubkey_x, SM2_BYTE_LEN);
	reverse_byte_array(pubKey+1+SM2_BYTE_LEN, (u8 *)pubkey_y, SM2_BYTE_LEN);

	if(rand_k)
	{
		reverse_byte_array(rand_k, (u8 *)k, SM2_BYTE_LEN);
	}
	else
	{
SM2_ENCRYPT_LOOP:

		ret = get_rand((u8 *)k, SM2_BYTE_LEN);
		if(TRNG_SUCCESS != ret)
		{
			return ret;
		}
	}

	//encrypt
	ret = sm2_encrypt_with_k(M, MByteLen, k, pubkey_x, pubkey_y, order, C, CByteLen);
	if(SM2_ZERO_ALL == ret || SM2_INTEGER_TOO_BIG == ret)
	{
		if(NULL == rand_k)
		{
			goto SM2_ENCRYPT_LOOP;
		}
	}

	return ret;
}


/* function: SM2 Decryption
 * parameters:
 *     C -------------------------- input, ciphertext, CByteLen bytes, big-endian
 *     CByteLen ------------------- input, byte length of C, make sure MByteLen>97
 *     priKey[32] ----------------- input, private key, 32 bytes, big-endian
 *     M -------------------------- output, plaintext, MByteLen bytes, big-endian
 *     MByteLen ------------------- output, byte length of M, should be CByteLen-97 if success
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution:
 *     1. M and C can not be the same buffer
 */
u8 sm2_decrypt(u8 *C, u32 CByteLen, u8 priKey[32],
					sm2_cipher_order_e order, u8 *M, u32 *MByteLen)
{
	u32 temLen;
	u32 dA[SM2_WORD_LEN], xy[SM2_WORD_LEN<<1];
	u8 digest[SM2_BYTE_LEN];
	u8 *C2, *C3;
	struct ts_hash_state mctx;
	u8 ret;

	if(NULL == C || NULL == priKey || NULL == M || NULL == MByteLen)
	{
		return SM2_BUFFER_NULL;
	}

	if(M == C)
	{
		return SM2_IN_OUT_SAME_BUFFER;
	}

	if(CByteLen <= 1+3*SM2_BYTE_LEN)                                        //97 = 1+3*ECCP_BYTELEN
	{
		return SM2_INPUT_INVALID;
	}

	if(order > SM2_C1C2C3)
	{
		return SM2_INPUT_INVALID;
	}

	//make sure C1 is on the SM2 curve
	reverse_byte_array(C+1, (u8 *)xy, SM2_BYTE_LEN);
	reverse_byte_array(C+1+SM2_BYTE_LEN, (u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN);

	ret = eccp_pointVerify(sm2_curve, xy, xy+SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return SM2_NOT_ON_CURVE;
	}

	//make sure priKey in [1, n-2]
	reverse_byte_array(priKey, (u8 *)dA, SM2_BYTE_LEN);
	if(uint32_BigNum_Check_Zero(dA, SM2_WORD_LEN))
	{
		return SM2_ZERO_ALL;
	}
	if(uint32_BigNumCmp(dA, SM2_WORD_LEN, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN) >= 0)
	{
		return SM2_INTEGER_TOO_BIG;
	}

	//[dA]C1
	ret = eccp_pointMul(sm2_curve, dA, xy, xy+SM2_WORD_LEN, xy, xy+SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	reverse_byte_array((u8 *)xy, (u8 *)xy, SM2_BYTE_LEN);
	reverse_byte_array((u8 *)(xy+SM2_WORD_LEN), (u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN);

	C2 = C+1+2*SM2_BYTE_LEN +((SM2_C1C2C3 == order)?0:SM2_BYTE_LEN);

	temLen = CByteLen-1-(3*SM2_BYTE_LEN);
	ret = sm2_kdf((u8 *)xy, SM2_BYTE_LEN<<1, M, temLen);
	if(SM2_SUCCESS != ret)
	{
		return ret;
	}

	if(uint8_BigNum_Check_Zero(M, temLen))
	{
		return SM2_ZERO_ALL;
	}

	uint8_XOR(M, C2, M, temLen);

	C3 = C+1+2*SM2_BYTE_LEN +((SM2_C1C2C3 == order)?temLen:0);

	mctx.hash_type = HASH_TYPE_SM3;
	ret = ts_hash_init(&mctx);
	if(TS_HASH_SUCCESS != ret)
	{
		return ret;
	}

	ret = ts_hash_update(&mctx, (u8 *)xy, SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		return ret;
	}

	ret = ts_hash_update(&mctx, M, temLen);
	if(TS_HASH_SUCCESS != ret)
	{
		return ret;
	}

	ret = ts_hash_update(&mctx, (u8 *)(xy+SM2_WORD_LEN), SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		return ret;
	}

	ret = ts_hash_final(&mctx, digest);
	if(TS_HASH_SUCCESS != ret)
	{
		return ret;
	}

	if(memcmp_(C3, digest, SM2_BYTE_LEN))
	{
		return SM2_DECRYPT_VERIFY_FAILED;
	}

	*MByteLen = temLen;

	return SM2_SUCCESS;
}



/* function: SM2 Key Exchange
 * parameters:
 *     role        ------ input, SM2_Role_Sponsor - sponsor, SM2_Role_Responsor - responsor
 *     dA[32]      ------ input, sponsor's permanent private key
 *     PB[65]      ------ input, responsor's permanent public key
 *     rA[32]      ------ input, sponsor's temporary private key
 *     RA[65]      ------ input, sponsor's temporary public key
 *     RB[65]      ------ input, responsor's temporary public key
 *     ZA[32]      ------ input, sponsor's Z value
 *     ZB[32]      ------ input, responsor's Z value
 *     kByteLen    ------ input, byte length of output key, should be less than (2^32 - 1)bit
 *     KA[kByteLen]------ output, output key
 *     S1[32]      ------ output, sponsor's S1, or responsor's S2
 *     SA[32]      ------ output, sponsor's SA, or responsor's SB
 * return:
 *     SM2_SUCCESS(success); other(error)
 * caution: *
 *     1. please make sure the inputs are valid
 *     2. if S1=SB,S2=SA, then success.
 */
u8 sm2_exchangekey(sm2_exchange_role_e role,
						u8 *dA, u8 *PB,
						u8 *rA, u8 *RA,
						u8 *RB,
						u8 *ZA, u8 *ZB,
						u32 kByteLen,
						u8 *KA, u8 *S1, u8 *SA)
{
	u32 x1[SM2_WORD_LEN], t1[SM2_WORD_LEN], tmp[SM2_WORD_LEN<<2];
	struct ts_hash_state mctx;
	u8 ret;

	if(NULL == dA || NULL == PB || NULL == rA || NULL == RA || NULL == RB)
	{
		return SM2_BUFFER_NULL;
	}

	if(NULL == ZA || NULL == ZB || NULL == KA || NULL == S1 || NULL == SA)
	{
		return SM2_BUFFER_NULL;
	}

	if(role > SM2_Role_Responsor)
	{
		return SM2_EXCHANGE_ROLE_INVALID;
	}

	if(0 == kByteLen)
	{
		return SM2_INPUT_INVALID;
	}

	if((POINT_NOT_COMPRESSED != PB[0]) || (POINT_NOT_COMPRESSED != RA[0]) || (POINT_NOT_COMPRESSED != RB[0]))
	{
		return SM2_INPUT_INVALID;
	}

	uint32_clear(x1+SM2_WORD_LEN/2, SM2_WORD_LEN/2);
	reverse_byte_array(RA+1+(SM2_BYTE_LEN/2), (u8 *)x1, SM2_BYTE_LEN/2);
	x1[(SM2_WORD_LEN/2)-1] |= 0x80000000;

	//make sure rA in [1, n-2]
	reverse_byte_array(rA, (u8 *)t1, SM2_BYTE_LEN);

	if(uint32_BigNum_Check_Zero(t1, SM2_WORD_LEN))
	{
		return SM2_ZERO_ALL;
	}
	if(uint32_BigNumCmp(t1, SM2_WORD_LEN, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN) >= 0)
	{
		return SM2_INTEGER_TOO_BIG;
	}

	//t1 = x1*rA mod n
	pke_load_pre_calc_mont((u32 *)sm2_curve->eccp_n_h, (u32 *)sm2_curve->eccp_n_n1, SM2_WORD_LEN);
	pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
	ret = pke_modmul_internal((u32 *)sm2_curve->eccp_n, x1, t1, t1, SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		goto end;
	}

	//make sure dA in [1, n-2]
	reverse_byte_array(dA, (u8 *)x1, SM2_BYTE_LEN);

	if(uint32_BigNum_Check_Zero(x1, SM2_WORD_LEN))
	{
		return SM2_ZERO_ALL;
	}
	if(uint32_BigNumCmp(x1, SM2_WORD_LEN, (u32 *)sm2p256v1_n_1, SM2_WORD_LEN) >= 0)
	{
		return SM2_INTEGER_TOO_BIG;
	}

    //t1 = (dA + x1*rA) mod n
	ret = pke_modadd((u32 *)sm2_curve->eccp_n, t1, x1, t1, SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		goto end;
	}

	//make sure RB on the SM2 curve

	reverse_byte_array(RB+1, (u8 *)tmp, SM2_BYTE_LEN);
	reverse_byte_array(RB+1+SM2_BYTE_LEN, (u8 *)(tmp+SM2_WORD_LEN), SM2_BYTE_LEN);

	ret = eccp_pointVerify(sm2_curve, tmp, tmp+SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		return SM2_NOT_ON_CURVE;
	}

	uint32_clear(x1+SM2_WORD_LEN/2, SM2_WORD_LEN/2);

	reverse_byte_array(RB+1+(SM2_BYTE_LEN/2), (u8 *)x1, SM2_BYTE_LEN/2);
	x1[(SM2_WORD_LEN/2)-1] |= 0x80000000;

	ret = eccp_pointMul(sm2_curve, x1, tmp, tmp+SM2_WORD_LEN, tmp, tmp+SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		goto end;
	}

	reverse_byte_array(PB+1, (u8 *)(tmp+2*SM2_WORD_LEN), SM2_BYTE_LEN);
	reverse_byte_array(PB+1+SM2_BYTE_LEN, (u8 *)(tmp+3*SM2_WORD_LEN), SM2_BYTE_LEN);
	ret = eccp_pointAdd(sm2_curve, tmp, tmp+SM2_WORD_LEN, tmp+2*SM2_WORD_LEN, tmp+3*SM2_WORD_LEN,
					    tmp, tmp+SM2_WORD_LEN);
	if(PKE_SUCCESS != ret)
	{
		goto end;
	}

	ret = eccp_pointMul(sm2_curve, t1, tmp, tmp+SM2_WORD_LEN, tmp, tmp+SM2_WORD_LEN);

	if(PKE_SUCCESS != ret)
	{
		goto end;
	}

	//xU||yU
	reverse_byte_array((u8 *)tmp, (u8 *)tmp, SM2_BYTE_LEN);
	reverse_byte_array((u8 *)(tmp+SM2_WORD_LEN), (u8 *)(tmp+SM2_WORD_LEN), SM2_BYTE_LEN);


	if(SM2_Role_Sponsor == role)
	{
		memcpy_(tmp+2*SM2_WORD_LEN, ZA, SM2_BYTE_LEN);
		memcpy_(tmp+3*SM2_WORD_LEN, ZB, SM2_BYTE_LEN);
	}
	else
	{
		memcpy_(tmp+2*SM2_WORD_LEN, ZB, SM2_BYTE_LEN);
		memcpy_(tmp+3*SM2_WORD_LEN, ZA, SM2_BYTE_LEN);
	}

	//KA
	ret = sm2_kdf((u8 *)tmp, SM2_BYTE_LEN<<2, KA, kByteLen);
	if(SM2_SUCCESS != ret)
	{
		goto end;
	}

	mctx.hash_type = HASH_TYPE_SM3;
	ret = ts_hash_init(&mctx);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = ts_hash_update(&mctx, (u8 *)tmp, SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	if(SM2_Role_Sponsor == role)
	{
		ret = ts_hash_update(&mctx, ZA, SM2_BYTE_LEN);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}

		ret = ts_hash_update(&mctx, ZB, SM2_BYTE_LEN);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}

		ret = ts_hash_update(&mctx, RA+1, SM2_BYTE_LEN<<1);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}

		ret = ts_hash_update(&mctx, RB+1, SM2_BYTE_LEN<<1);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}
	}
	else
	{
		ret = ts_hash_update(&mctx, ZB, SM2_BYTE_LEN);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}

		ret = ts_hash_update(&mctx, ZA, SM2_BYTE_LEN);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}

		ret = ts_hash_update(&mctx, RB+1, SM2_BYTE_LEN<<1);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}

		ret = ts_hash_update(&mctx, RA+1, SM2_BYTE_LEN<<1);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}
	}
	ret = ts_hash_final(&mctx, (u8 *)t1);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	mctx.hash_type = HASH_TYPE_SM3;
	ret = ts_hash_init(&mctx);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	*(((u8 *)(tmp+SM2_WORD_LEN))-1) = 0x03;
	ret = ts_hash_update(&mctx, ((u8 *)(tmp+SM2_WORD_LEN))-1, SM2_BYTE_LEN+1);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = ts_hash_update(&mctx, (u8 *)t1, SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	if(SM2_Role_Sponsor == role)
	{
		ret = ts_hash_final(&mctx, SA);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}
	}
	else
	{
		ret = ts_hash_final(&mctx, S1);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}
	}

	mctx.hash_type = HASH_TYPE_SM3;
	ret = ts_hash_init(&mctx);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	*(((u8 *)(tmp+SM2_WORD_LEN))-1) = 0x02;
	ret = ts_hash_update(&mctx, ((u8 *)(tmp+SM2_WORD_LEN))-1, SM2_BYTE_LEN+1);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	ret = ts_hash_update(&mctx, (u8 *)t1, SM2_BYTE_LEN);
	if(TS_HASH_SUCCESS != ret)
	{
		goto end;
	}

	if(SM2_Role_Sponsor == role)
	{
		ret = ts_hash_final(&mctx, S1);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}
	}
	else
	{
		ret = ts_hash_final(&mctx, SA);
		if(TS_HASH_SUCCESS != ret)
		{
			goto end;
		}
	}

	ret = SM2_SUCCESS;

end:

	return ret;
}

