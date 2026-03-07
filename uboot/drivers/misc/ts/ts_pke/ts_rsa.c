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
#include "ts_trng.h"
#include "ts_pke.h"
#include "ts_rsa.h"
#include "ts_pke_prime.h"

/* function: out = a^e mod n
 * parameters:
 *     a -------------------------- input, u32 big integer a, base number, make sure a < n
 *     e -------------------------- input, u32 big integer e, exeponent, make sure e < n
 *     n -------------------------- input, u32 big integer n, modulus, make sure n is odd
 *     out ------------------------ output, out = a^e mod n
 *     eBitLen  ------------------- input, real bit length of u32 big integer e
 *     nBitLen  ------------------- input, real bit length of u32 big integer n
 * return: RSA_SUCCESS(success), other(error)
 * caution:
 *     1. a, n, and out have the same word length:((nBitLen+31)>>5); e word length is (eBitLen+31)>>5
 */
u8 RSA_ModExp(u32 *a, u32 *e, u32 *n, u32 *out, u32 eBitLen, u32 nBitLen)
{
	u32 nWordLen = GET_WORD_LEN(nBitLen);
	u8 ret;

	if(NULL == a || NULL == e || NULL == n || NULL == out)
	{
		return RSA_BUFFER_NULL;
	}

	if(nBitLen > MAX_RSA_BIT_LEN || eBitLen > nBitLen)
	{
		return RSA_INPUT_TOO_LONG;
	}

	if(!(n[0] & 1))
	{
		return RSA_INPUT_INVALID;
	}

	ret = pke_pre_calc_mont(n, nWordLen, NULL, NULL);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	return pke_modexp(n, e, a, out, nWordLen, GET_WORD_LEN(eBitLen));
}


/* function: out = a^d mod n, here d represents RSA CRT private key (p,q,dp,dq,u)
 * parameters:
 *     a -------------------------- input, u32 big integer a, base number, make sure a < n=pq
 *     p -------------------------- input, u32 big integer p, prime number, one part of private key (p,q,dp,dq,u)
 *     q -------------------------- input, u32 big integer q, prime number, one part of private key (p,q,dp,dq,u)
 *     dp ------------------------- input, u32 big integer dp = e^(-1) mod (p-1), one part of private key (p,q,dp,dq,u)
 *     dq ------------------------- input, u32 big integer dq = e^(-1) mod (q-1), one part of private key (p,q,dp,dq,u)
 *     u -------------------------- input, u32 big integer u = q^(-1) mod p, one part of private key (p,q,dp,dq,u)
 *     out ------------------------ output, out = a^d mod n
 *     nBitLen  ------------------- input, real bit length of u32 big integer n=pq
 * return: RSA_SUCCESS(success), other(error)
 * caution:
 *     1. a and out have the same word length:((nBitLen+31)>>5); and p,p_h,q,q_h,dp,dq,u
 *        have the same word length:((nBitLen/2+31)>>5)
 */
u8 RSA_CRTModExp(u32 *a, u32 *p, u32 *q, u32 *dp, u32*dq, u32 *u,
					  u32 *out,  u32 nBitLen)
{
	u32 buf[MAX_RSA_WORD_LEN];
	u32 *m1 = buf;
	u32 *m2 = buf+(MAX_RSA_WORD_LEN/2);

	u32 tmp_step;
	u32 nWordLen = GET_WORD_LEN(nBitLen);
	u32 pWordLen = GET_WORD_LEN(nBitLen/2);
	u8 ret;

	if(NULL == a || NULL == p || NULL == q || NULL == dp || NULL == dq
	   || NULL == u || NULL == out)
	{
		return RSA_BUFFER_NULL;
	}

	if(nBitLen > MAX_RSA_BIT_LEN)
	{
		return RSA_INPUT_TOO_LONG;
	}

	//store the nBitLen step
	pke_set_operand_width(nBitLen);
	tmp_step = pke_get_operand_bytes();

	//m2 = (a) mod q
	ret = pke_mod(a, nWordLen, q, pWordLen, m2);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}
	//m2 = (a)^dq mod q
	ret = pke_modexp(q, dq, m2, m2, pWordLen, pWordLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//m1 = (a) mod p
	ret = pke_mod(a, nWordLen, p, pWordLen, m1);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}
	//m1 = (a)^dp mod p
	ret = pke_modexp(p, dp, m1, m1, pWordLen, pWordLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//m2 = m2 mod p
	if(uint32_BigNumCmp(m2, pWordLen, p, pWordLen) > 0)
	{
		ret = pke_sub(m2, p, m2, pWordLen);
		if(PKE_SUCCESS != ret)
		{
			return ret;
		}
	}

	//m1 = (m1-m2) mod p
	ret = pke_modsub(p, m1, m2, m1, pWordLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//m1 = h = u*(m1-m2) mod p
	ret = pke_modmul(p, m1, u, m1, pWordLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//A1 = hq
	ret = pke_mul(m1, q, (u32 *)(PKE_A(1,tmp_step)), pWordLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//out = m2+hq
	uint32_copy((u32 *)(PKE_B(1,tmp_step)), m2, pWordLen);
	uint32_clear((u32 *)(PKE_B(1,tmp_step))+pWordLen, nWordLen-pWordLen);
	return pke_add((u32 *)(PKE_A(1,tmp_step)), (u32 *)(PKE_B(1,tmp_step)), out, nWordLen);
}


/* function: get big odd integer e of eBitLen
 * parameters:
 *     e -------------------------- input, u32 big odd integer e
 *     eBitLen  ------------------- input, bit length of u32 big odd integer e
 * return: 0(success), 1(error: eBitLen<2)
 * caution:
 *     1. eBitLen must be big than 1
 */
u8 RSA_Get_E1(u32 e[], u32 eBitLen)
{
	u32 eWordLen = (eBitLen+0x1F)>>5;
	u8 ret;

	if(eBitLen<2)
	{
		return RSA_INPUT_INVALID;
	}

	ret = get_rand((u8 *)e, eWordLen<<2);
	if(0 != ret)
	{
		return ret;
	}

	eBitLen &= 31;

	if(eBitLen)
	{
		e[eWordLen - 1] &= (1<<(eBitLen))-1;
		e[eWordLen - 1] |= 1<<(eBitLen - 1);
	}
	else
	{
		e[eWordLen - 1] |= 0x80000000;
	}

	e[0] |= 0x01;          //make e odd

	return 0;
}


/* function: get big odd integer e of eBitLen, satisfies e < fai_n of bitLen
 * parameters:
 *     e -------------------------- input, u32 big odd integer e
 *     fai_n ---------------------- input, u32 big even integer fai_n
 *     bitLen   ------------------- input, bit length of u32 big odd integer e and n
 * return: 0(success), 1(error: bitLen<66), 2(error, n is 1000000000...000000)
 * caution:
 *     1. eBitLen must be big than 65
 *     2. n can not be 1000000000...000000
 */
u8 RSA_Get_E2(u32 e[], u32 fai_n[], u32 bitLen)
{
	u32 wordLen;
	s32 i;
	u8 j;

	if(bitLen < 66)
		return 1;

	RSA_Get_E1(e, bitLen);
	wordLen = (bitLen+0x1F)>>5;
	j = bitLen&31;
	if(j==0)
	{
		j = 32;
	}
	j--;
	i = wordLen - 1;

	if(j==0)
	{
		i--;
		j=32;
	}

	while(i>=0)
	{

		e[i] &= (~(1<<(j-1)));
		if(uint32_BigNumCmp(e, i+1, fai_n, i+1) < 0)	   //if e < n
		{
			return 0;
		}

		j--;
		if(0 == j)
		{
			i--;
			j=32;
		}
	}

	return 2;		  //fail, because n is 1000000000...000000
}


/* function: judge whether big integer a is equal to 0x5a5a5a5a5a...5a or not
 * parameters:
 *     a -------------------------- input, u32 big integer a
 *     aBitLen -------------------- input, real bit length of a
 * return: 0(a==0x5a5a5a5a5a...5a), 1(a!=0x5a5a5a5a5a...5a)
 * caution:
 *     1. aBitLen can not be 0
 *     2. if aBitLen%32 != 0, then the highest word of a should be 0
 */
u8 CheckValue_0x5a5a5a5a(u32 a[], u32 aBitLen)
{
	u32 i, wordLen = aBitLen>>5;

	if(aBitLen & 0x1F)
	{
		if(a[wordLen] != 0)
			return 1;
	}

	for(i=0; i<wordLen; i++)
	{
		if(a[i] != 0x5a5a5a5a)
			return 1;
	}
	return 0;
}


/* function: generate RSA key (e,d,n)
 * parameters:
 *     e -------------------------- output, u32 big integer, RSA public key e
 *     d -------------------------- output, u32 big integer, RSA private key d
 *     n -------------------------- output, u32 big integer, RSA public module n
 *     eBitLen  ------------------- input, real bit length of e
 *     nBitLen  ------------------- input, real bit length of n
 * return: RSA_SUCCESS(success), other(error)
 * caution:
 *     1. nBitLen can not be even
 *     2. eBitLen must be larger than 1, and less than or equal to nBitLen
 */
u8 RSA_GetKey(u32 *e, u32 *d, u32 *n, u32 eBitLen, u32 nBitLen)
{
	u32 buf[MAX_RSA_WORD_LEN];
	u32 *p, *q, *in, *out;
	u32 pBitLen, pWordLen, eWordLen, nWordLen, tmpLen;
	u8 count,flag;

	if(NULL == e || NULL == d || NULL == n)
	{
		return RSA_BUFFER_NULL;
	}

	if(nBitLen&1 || nBitLen < MIN_RSA_BIT_LEN || nBitLen > MAX_RSA_BIT_LEN)  //nBitLen can not be odd
	{
		return RSA_INPUT_INVALID;
	}
	else if(eBitLen<2 || eBitLen>nBitLen)
	{
		return RSA_INPUT_INVALID;
	}

	p = buf;
	q = buf+MAX_RSA_WORD_LEN/2;

	pke_set_operand_width(nBitLen);
	tmpLen = pke_get_operand_bytes();
	in = (u32 *)(PKE_B(1,tmpLen));
	out = (u32 *)(PKE_A(2,tmpLen));

	eWordLen = GET_WORD_LEN(eBitLen);
	nWordLen = GET_WORD_LEN(nBitLen);
	pBitLen = nBitLen>>1;
	pWordLen = GET_WORD_LEN(pBitLen);

GET_PQ:

	flag = get_prime(p, pBitLen);
	if(flag)
	{
		return flag;
	}

	flag = get_prime(q, pBitLen);
	if(flag)
	{
		return flag;
	}

	p[0]--;                                            // p=p-1
	q[0]--;                                            // q=q-1
	pke_mul(p, q, n, pWordLen);                        // get fai(n)=(p-1)(q-1)

	count = 0;
GET_E:
	count++;
	if(count == 7)
		goto GET_PQ;

	switch(eBitLen)
	{
		case 2 :  {e[0] = 3; break;}
		case 5 :  {e[0] = 17; break;}
		case 17:  {e[0] = 65537; break;}
		default:
		{
			if(eBitLen == nBitLen)
			{
				flag = RSA_Get_E2(e, n, eBitLen);
				if(flag)
				{
					return flag;
				}
			}
			else
			{
				flag = RSA_Get_E1(e, eBitLen);
				if(flag)
				{
					return flag;
				}
			}
			break;
		}
	}

	//get d = e^(-1) mod n
	flag = pke_modinv(n, e, d, nWordLen, eWordLen);
	if(PKE_NO_MODINV == flag)                          //if d doesn't exist
	{
		if(eBitLen==2 || eBitLen==5 || eBitLen==17)    //if e is prime, and e divide fai(n)
			goto GET_PQ;
		else                                           //1. e is prime, and e divide fai(n) 2.e is not prime, and
			goto GET_E;                                //e, fai(n) have common divisor.
	}
	else if(PKE_SUCCESS != flag)
	{
		return flag;
	}

	//get n = pq
	p[0]++;
	q[0]++;
	flag = pke_mul(p, q, n, pWordLen);
	if(PKE_SUCCESS != flag)
	{
		return flag;
	}

	//Encryption test
	if(nBitLen & 0x1F)
	{
		in[nWordLen-1]=0;
	}
	tmpLen = nBitLen>>5;
	uint32_set(in, 0x5a5a5a5a, tmpLen);

	flag = pke_pre_calc_mont(n, nWordLen, NULL, NULL);
	if(PKE_SUCCESS != flag)
	{
		return flag;
	}

	flag = pke_modexp(n, e, in, out, nWordLen, eWordLen);
	if(PKE_SUCCESS != flag)
	{
		return flag;
	}

	flag = pke_modexp(n, d, out, out, nWordLen, nWordLen);
	if(PKE_SUCCESS != flag)
	{
		return flag;
	}

	if(CheckValue_0x5a5a5a5a(out, nBitLen))
	{
		goto GET_PQ;
	}

	return RSA_SUCCESS;
}


/* Function: generate RSA-CRT key (e,p,q,dp,dq,u,n)
 * Parameters:
 *     e -------------------------- output, u32 big integer, RSA public key e
 *     p -------------------------- output, u32 big integer, RSA private key p
 *     q -------------------------- output, u32 big integer, RSA private key q
 *     dp-------------------------- output, u32 big integer, RSA private key dp
 *     dq-------------------------- output, u32 big integer, RSA private key dq
 *     u -------------------------- output, u32 big integer, RSA private key u = q^(-1) mod p
 *     n -------------------------- output, u32 big integer, RSA public module n
 *     eBitLen  ------------------- input, real bit length of e
 *     nBitLen  ------------------- input, real bit length of n
 * Return: RSA_SUCCESS(success), other(error)
 * Caution:
 *     1. nBitLen can not be even
 *     2. eBitLen must be larger than 1, and less than or equal to nBitLen
 */
u8 RSA_GetCRTKey(u32 *e, u32 *p, u32 *q, u32 *dp, u32 *dq, u32 *u,
					  u32 *n, u32 eBitLen, u32 nBitLen)
{
	u32 buf[MAX_RSA_WORD_LEN];
	u32 pBitLen, pWordLen, eWordLen, nWordLen, i, wordLen;
	s32 count;
	u8 ret;

	if(NULL == e || NULL == p || NULL == q || NULL == dp || NULL == dq || NULL == u || NULL == n)
	{
		return RSA_BUFFER_NULL;
	}

	if(nBitLen&1 || nBitLen<MIN_RSA_BIT_LEN || nBitLen > MAX_RSA_BIT_LEN)  //nBitLen can not be odd
	{
		return RSA_INPUT_INVALID;
	}
	else if(eBitLen<2 || eBitLen>nBitLen)
	{
		return RSA_INPUT_INVALID;
	}

	eWordLen = GET_WORD_LEN(eBitLen);
	nWordLen = GET_WORD_LEN(nBitLen);
	pBitLen = nBitLen>>1;
	pWordLen = GET_WORD_LEN(pBitLen);

GET_PQ:

	ret = get_prime(p, pBitLen);
	if(ret)
	{
		return ret;
	}

	ret = get_prime(q, pBitLen);
	if(ret)
	{
		return ret;
	}

	count = uint32_BigNumCmp(p, pWordLen, q, pWordLen);         // make p > q, for get u = q^(-1) mod p convenient
	if(count == -1)
	{
		for(i=0; i<pWordLen; i++)
		{
			wordLen = p[i];
			p[i] = q[i];
			q[i] = wordLen;
		}
	}
	else if(count == 0)
	{
		goto GET_PQ;
	}

	p[0]--;                                                // p=p-1
	q[0]--;                                                // q=q-1
	if(eBitLen == nBitLen)
	{
		ret = pke_mul(p, q, n, pWordLen); // get fai(n)=(p-1)(q-1)
		if(PKE_SUCCESS != ret)
		{
			return ret;
		}
	}

	count = 0;
GET_E:
	count++;
	if(count == 7)
		goto GET_PQ;

	switch(eBitLen)
	{
		case 2 :  {e[0] = 3; break;}
		case 5 :  {e[0] = 17; break;}
		case 17:  {e[0] = 65537; break;}
		default:
		{
			if(eBitLen == nBitLen)
			{
				ret = RSA_Get_E2(e, n, eBitLen);
				if(ret)
				{
					return ret;
				}
			}
			else
			{
				ret = RSA_Get_E1(e, eBitLen);
				if(ret)
				{
					return ret;
				}
			}
			break;
		}
	}

	// dp = e^(-1) mod (p-1)
	if(uint32_BigNumCmp(e, eWordLen, p, pWordLen) > 0)
	{
		ret = pke_mod(e, eWordLen, p, pWordLen, u);
		if(PKE_SUCCESS != ret)
		{
			return ret;
		}
		wordLen = pWordLen;
	}
	else
	{
		uint32_copy(u, e, eWordLen);
		wordLen = eWordLen;
	}

	ret = pke_modinv(p, u, dp, pWordLen, wordLen);
	if(PKE_NO_MODINV == ret)
	{
		if(eBitLen==2 || eBitLen==5 || eBitLen==17)    //if e is prime, and e divide fai(n)
			goto GET_PQ;
		else                                           //1. e is prime, and e divide fai(n) 2.e is not prime, and
			goto GET_E;                                //e, fai(n) have common divisor.
	}
	else if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	// dq = e^(-1) mod (q-1)
	if(uint32_BigNumCmp(e, eWordLen, q, pWordLen) > 0)
	{
		ret = pke_mod(e, eWordLen, q, pWordLen, u);
		if(PKE_SUCCESS != ret)
		{
			return ret;
		}
		wordLen = pWordLen;
	}
	else
	{
		uint32_copy(u, e, eWordLen);
		wordLen = eWordLen;
	}

	ret = pke_modinv(q, u, dq, pWordLen, wordLen);
	if(PKE_NO_MODINV == ret)
	{
		if(eBitLen==2 || eBitLen==5 || eBitLen==17)    //if e is prime, and e divide fai(n)
			goto GET_PQ;
		else                                           //1. e is prime, and e divide fai(n) 2.e is not prime, and
			goto GET_E;                                //e, fai(n) have common divisor.
	}
	else if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	p[0]++;
	q[0]++;

	// u = q^(-1) mod p
	ret = pke_modinv(p, q, u, pWordLen, pWordLen);
	if(PKE_NO_MODINV == ret)
	{
		goto GET_PQ;
	}
	else if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	// get n
	ret = pke_mul(p, q, n, pWordLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//Encryption test
	if(nBitLen & 0x1F)
	{
		buf[nWordLen-1]=0;
	}
	wordLen = nBitLen>>5;
	uint32_set(buf, 0x5a5a5a5a, wordLen);

	ret = pke_pre_calc_mont(n, nWordLen, NULL, NULL);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	ret = pke_modexp(n, e, buf, buf, nWordLen, eWordLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	if(!CheckValue_0x5a5a5a5a(buf, nBitLen))
	{
		goto GET_PQ;
	}

	ret = RSA_CRTModExp(buf, p, q, dp, dq, u, buf, nBitLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	if(CheckValue_0x5a5a5a5a(buf, nBitLen))
	{
		goto GET_PQ;
	}

	return RSA_SUCCESS;
}

