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

#ifdef PKE_PRINT_BUF
void print_buf_U8(u8 buf[], u32 byteLen, char name[])
{
	u32 i;

	printf("\r\n %s: %08x\r\n",name, *buf);
	for(i=0; i<byteLen; i++)
	{
		//if(i%16 ==0 && i>0)
		//	printf("\r\n");
		//printf("%02x", buf[byteLen-1-i]);
		printf("%02x", buf[i]);
	}

	printf("\r\n");
}

void print_buf_U32(u32 buf[], u32 wordLen, char name[])
{
	u32 i;

	printf("\r\n %s: %08x\r\n",name, *buf);
	for(i=0; i<wordLen; i++)
	{
		//if(i%16 ==0 && i>0)
		//	printf("\r\n");
		//printf("%08x", buf[wordLen-1-i]);
		printf("%08x", buf[i]);
	}

	printf("\r\n");
}

void print_BN_buf_U32(u32 buf[], u32 wordLen, char name[])
{
	u32 i;

	printf("\r\n %08x %s: ", *buf, name);
	for(i=0; i<wordLen; i++)
	{
		//if(i%16 ==0 && i>0)
		//	printf("\r\n");
		printf("%08x", buf[wordLen-1-i]);
	}
	printf("\r\n");
}
#endif

void memcpy_(void *dst, void *src, u32 size)
{
	u8 *a = (u8 *)dst;
	u8 *b = (u8 *)src;

	u32 *aa = (u32 *)dst;
	u32 *bb = (u32 *)src;
	u32 i, count, tmp;

	if((((u32)(uintptr_t)dst) & 3) || (((u32)(uintptr_t)src) & 3))
	{
		while(size--)
		{
			*a++ = *b++;
		}
	}
	else
	{
		count = size/4;
		for(i=0; i<count; i++)
		{
			*aa++ = *bb++;
		}

		tmp = size&3;
		if(tmp)
		{
			a+=(size&(~0x03));
			b+=(size&(~0x03));
			while(tmp--)
			{
				*a++ = *b++;
			}
		}
	}
}

void memset_(void *dst, u8 value, u32 size)
{
	u8 *a = (u8 *)dst;

	u32 i, count, tmp;

	tmp = ((u32)(uintptr_t)dst) & 3;
	if(tmp)
	{
		if(size > 4-tmp)
		{
			for(i=0; i<4-tmp; i++)
			{
				*a++ = value;
			}
			size -= (4-tmp);
		}
		else
		{
			for(i=0; i<size; i++)
			{
				*a++ = value;
			}
			return;
		}
	}

	count = size/4;
	if(count)
	{
		tmp = value;
		tmp = (tmp<<8)|value;
		tmp = (tmp<<8)|value;
		tmp = (tmp<<8)|value;
		for(i=0; i<count; i++)
		{
			*((u32 *)a) = tmp;
			a+=4;
		}
	}

	tmp = size&3;
	if(tmp)
	{
		for(i=0; i<tmp; i++)
		{
			*a++ = value;
		}
	}

}

int8_t memcmp_(void *m1, void *m2, u32 size)
{
	int8_t *a = (int8_t *)m1;
	int8_t *b = (int8_t *)m2;
	int8_t c;

	while(size--)
	{
		c = (*a++ - *b++);
		if(c)
		{
			return c;
		}
	}

	return 0;
}

/* function: set uint32 buffer
 * parameters:
 *     a -------------------------- output, output word buffer
 *     value ---------------------- input, input word value
 *     wordLen -------------------- input, word length of buffer a
 * return: none
 * caution:
 */
void uint32_set(u32 *a, u32 value, u32 wordLen)
{
	while(wordLen)
	{
		a[--wordLen] = value;
	}
}

/* function: copy uint32 buffer
 * parameters:
 *     dst ------------------------ output, output word buffer
 *     src ------------------------ input, input word buffer
 *     wordLen -------------------- input, word length of buffer dst or src
 * return: none
 * caution:
 */
void uint32_copy(u32 *dst, u32 *src, u32 wordLen)
{
	u32 i;

	if(dst != src)
	{
		for(i=0; i<wordLen; i++)
		{
			dst[i] = src[i];
		}
	}
}

void uint32_copy_for_littlendian(u32 *dst, u32 *src, u32 wordLen)
{
	u32 i, j;

	if(dst != src)
	{
		for(i = wordLen - 1, j = 0; i >= 0 && j < wordLen; i--, j++)
		{
			dst[j] = src[i];
		}
	}
}


/* function: clear uint32 buffer
 * parameters:
 *     a -------------------------- input&output, word buffer a
 *     aWordLen ------------------- input, word length of buffer a
 * return: none
 * caution:
 */
void uint32_clear(u32 *a, u32 wordLen)
{
	while(wordLen)
	{
		a[--wordLen] = 0;
	}
}


/* function: sleep for a while
 * parameters:
 *     count ---------------------- input, count
 * return: none
 * caution:
 */
void uint32_sleep(u32 count)
{
	volatile u32 i=0;

	while(i<count)
	{
		i++;
	}
}



/* function: reverse byte array
 * parameters:
 *     in ------------------------- input, input buffer
 *     out ------------------------ output, output buffer
 *     byteLen -------------------- input, byte length of in or out
 * return: none
 * caution:
 *    1. in and out could point the same buffer
 */
void reverse_byte_array(u8 *in, u8 *out, u32 byteLen)
{
    u32 idx, round = byteLen >> 1;
	u8 tmp;

    for (idx = 0; idx < round; idx++)
	{
        tmp = in[idx];
        out[idx] = in[byteLen - 1 - idx];
        out[byteLen - 1 - idx] = tmp;
    }

    if ((byteLen & 0x1) && (in != out))
	{
        out[round] = in[round];
    }
}

/* function: reverse byte order in every u32 word
 * parameters:
 *     in ------------------------- input, input byte buffer
 *     out ------------------------ output, output word buffer
 *     bytelen -------------------- input, byte length of buffer in or out
 * return: none
 * caution:  1. byteLen must be multiple of 4

void reverse_word(u8 *in, u8 *out, u32 bytelen)
{
	u32 i, len;
	u8 tmp;
	u8 *p = in;

	if(in == out)
	{
		while(bytelen>0)
		{
			tmp=*p;
			*p=*(p+3);
			*(p+3)=tmp;
			p+=1;
			tmp=*p;
			*p=*(p+1);
			*(p+1)=tmp;
			bytelen-=4;
			p+=3;
		}
	}
	else
	{
	    for (i = 0; i < bytelen; i++)
		{
			len = i >> 2;
			len = len << 3;
			out[i] = p[len + 3 - i];
		}
    }
}*/


/* function: reverse word order
 * parameters:
 *     in ------------------------- input, input word buffer
 *     out ------------------------ output, output word buffer
 *     wordLen -------------------- input, word length of buffer in or out
 *     reverse_word --------------- input, whether to reverse byte order in every word, 0:no, other:yes
 * return: none
 * caution:
 *     1. in DAM mode, the memory may be accessed by words, not by bytes, this function is designed
 *        for the case

void dma_reverse_word_array(u32 *in, u32 *out, u32 wordLen, u32 reverse_word)
{
	u32 i, j;
	u32 tmp;
	u32 *p=out;

	if(in == out)
	{
		for(i=0; i<wordLen; i+=4)
		{
			for (j = 0; j < 2; j++)
			{
				tmp = p[j];
				p[j] = p[4 - 1 - j];
				p[4 - 1 - j] = tmp;
			}
			p+=4;
		}
	}
	else
	{
		for(i=0; i<wordLen; i+=4)
		{
			p[0] = in[3];
			p[1] = in[2];
			p[2] = in[1];
			p[3] = in[0];
			p+=4;
			in+=4;
		}
	}

	if(reverse_word)
	{
		for (i = 0; i < wordLen; i++)
		{
			tmp = *out;
			*out = tmp&0xFF;
			*out <<= 8;
			*out |= (tmp>>8)&0xFF;
			*out <<= 8;
			*out |= (tmp>>16)&0xFF;
			*out <<= 8;
			*out |= (tmp>>24)&0xFF;

			out++;
		}
	}
	else
	{;}
} */


/* function: C = A XOR B
 * parameters:
 *     A -------------------------- input, byte buffer a
 *     B -------------------------- input, byte buffer b
 *     C -------------------------- output, C = A XOR B
 *     byteLen -------------------- input, byte length of A,B,C
 * return: none
 * caution:
 */
void uint8_XOR(u8 *A, u8 *B, u8 *C, u32 byteLen)
{
	u32 i;

	for(i=0; i<byteLen; i++)
	{
		C[i] = A[i] ^ B[i];
	}
}


/* function: get real bit length of big number a of wordLen words
 */
u32 get_valid_bits(const u32 *a, u32 wordLen)
{
    u32 i = 0;
    u32 j = 0;

	if(0 == wordLen)
	{
		return 0;
	}

    for (i = wordLen; i > 0; i--)
    {
        if (a[i - 1])
        {
            break;
        }
    }

	if(0 == i)
	{
		return 0;
	}

    for (j = 32; j > 0; j--)
    {
        if (a[i - 1] & (((u32)0x1) << (j - 1)))
        {
            break;
        }
    }

    return ((i - 1) << 5) + j;
}


/* function: get real word lenth of big number a of max_words words
 * parameters:
 *     a -------------------------- input, big integer a
 *     max_words ------------------ input, max word length of a
 * return: real word lenth of big number a
 * caution:
 */
u32 get_valid_words(u32 *a, u32 max_words)
{
    u32 i;

    for (i = max_words; i > 0; i--)
    {
        if (a[i - 1])
        {
            return i;
        }
    }

    return 0;
}


/* function: check whether big number or u8 buffer a is all zero or not
 * parameters:
 *     a -------------------------- input, byte buffer a
 *     aByteLen ------------------- input, byte length of a
 * return: 0(a is not zero),1(a is all zero)
 * caution:
 */
u8 uint8_BigNum_Check_Zero(u8 a[], u32 aByteLen)
{
	u32 i;

	for(i=0; i<aByteLen; i++)
	{
		if(a[i])
		{
			return 0;
		}
	}

	return 1;
}


/* function: check whether big number or u32 buffer a is all zero or not
 * parameters:
 *     a -------------------------- input, big integer or word buffer a
 *     aWordLen ------------------- input, word length of a
 * return: 0(a is not zero), 1(a is all zero)
 * caution:
 */
u8 uint32_BigNum_Check_Zero(u32 a[], u32 aWordLen)
{
	u32 i;

	for(i=0; i<aWordLen; i++)
	{
		if(a[i])
		{
			return 0;
		}
	}

	return 1;
}


/* function: compare big integer a and b
 * parameters:
 *     a -------------------------- input, big integer a
 *     aWordLen ------------------- input, word length of a
 *     b -------------------------- input, big integer b
 *     bWordLen ------------------- input, word length of b
 * return:
 *     0:a=b,   1:a>b,   -1: a<b
 * caution:
 */
s32 uint32_BigNumCmp(u32 *a, u32 aWordLen, u32 *b, u32 bWordLen)
{
	s32 i;

	aWordLen = get_valid_words(a, aWordLen);
	bWordLen = get_valid_words(b, bWordLen);

	if(aWordLen > bWordLen)
	{
		return 1;
	}

	if(aWordLen < bWordLen)
	{
		return -1;
	}

	for(i=(aWordLen-1);i>=0;i--)
	{
		if(a[i] > b[i])
		{
			return 1;
		}

		if(a[i] < b[i])
		{
			return -1;
		}
	}

	return 0;
}


/* function: for a = b*2^t, b is odd, get t
 * parameters:
 *     a -------------------------- big integer a
 * return:
 *     number of multiple by 2, for a
 * caution:
 *     1. make sure a != 0
 */
u32 Get_Multiple2_Number(u32 a[])
{
	u32 t, i=0, j=0;

	while(0 == (a[i]))
	{
		i++;
	}

	t = a[i];
	while(!(t&1))
	{
		j++;
		t>>=1;
	}

	return (i<<5)+j;
}


/* function: a = a/(2^n)
 * parameters:
 *     a -------------------------- big integer a
 *     aWordLen ------------------- word length of a
 *     n -------------------------- exponent of 2^n
 * return:
 *     word length of a = a/(2^n)
 * caution:
 *     1. make sure aWordLen is real word length of a
 *     2. a may be 0, then aWordLen is 0, to make sure aWordLen-1 is available, so data
 *        type of aWordLen is s32, not u32
 */
u32 Big_Div2n(u32 a[], s32 aWordLen, u32 n)
{
	s32 i;
	u32 j;

	aWordLen = get_valid_words(a, aWordLen);
	if(0 == n)
	{
		return aWordLen;
	}

	if(!aWordLen)
		return 0;

	if(n<=32)
	{
		for(i=0; i<aWordLen-1; i++)
		{
			a[i] >>= n;
			a[i] |= (a[i+1]<<(32-n));
		}
		a[i] >>= n;

		if(!a[i])
			return i;
		return aWordLen;
	}
	else        //general method
	{
		j=n>>5; //j=n/32;
		n&=31;  //n=n%32;
		for(i=0; i<aWordLen-(s32)j-1; i++)
		{
			a[i] = a[i+j]>>n;
			a[i] |= (a[i+j+1]<<(32-n));
		}
		a[i] = a[i+j]>>n;
		uint32_clear(a+aWordLen-j, j);

		if(!a[i])
			return i;
		return aWordLen-j;
	}
}


/* Function: check whether a is equal to 1 or not
 * Parameters:
 *     a ---------------- pointer to u32 big integer a
 *     aWordLen --------- word length of big integer a
 * Return: 1(a is 1), 0(a is not 1)
 * Caution:
 */
u8 Bigint_Check_1(u32 a[], u32 aWordLen)
{
	u32 i;

	if(!aWordLen)
	{
		return 0;
	}

	if(a[0] != 1)
	{
		return 0;
	}

	for(i=1; i<aWordLen; i++)
	{
		if(a[i])
			return 0;
	}

	return 1;
}


/* function: check whether a is equal to p-1 or not
 * parameters:
 *     a ---------------- pointer to u32 big integer a
 *     p ---------------- pointer to u32 big integer p, p must be odd
 *     wordLen ---------- word length of a and p
 * return: 1(a is 1), 0(a is not 1)
 * caution:
 *     1. make sure p is odd
 */
u8 Bigint_Check_p_1(u32 a[], u32 p[], u32 wordLen)
{
	u32 i;

	if(!wordLen)
	{
		return 0;
	}

	if(a[0] != p[0] - 1)
	{
		return 0;
	}

	for(i=1; i<wordLen; i++)
	{
		if(a[i] != p[i])
			return 0;
	}

	return 1;
}

