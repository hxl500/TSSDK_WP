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
#include <asm/utils.h>
#include <linux/bitops.h>
#include <rand.h>

#include "ts_trng.h"
#include "ts_utility.h"

typedef u32 GET_RAND_WORDS(u32 *a, u32 words);

/* function: TRNG global interruption enable
 * parameters: none
 * return: none
 * caution:
 */
void trng_global_int_enable()
{
	TRNG_CR |= (1<<TRNG_GLOBAL_INT_OFFSET);
}


/* function: TRNG global interruption disable
 * parameters: none
 * return: none
 * caution:
 */
void trng_global_int_disable()
{
	TRNG_CR &= ~(1<<TRNG_GLOBAL_INT_OFFSET);
}


/* function: TRNG empty-read interruption enable
 * parameters: none
 * return: none
 * caution:
 *     1. works when global interruption is enabled
 */
void trng_empty_read_int_enable()
{
	TRNG_CR |= (1<<TRNG_READ_EMPTY_INT_OFFSET);
}


/* function: TRNG empty-read interruption disable
 * parameters: none
 * return: none
 * caution:
 */
void trng_empty_read_int_disable()
{
	TRNG_CR &= ~(1<<TRNG_READ_EMPTY_INT_OFFSET);
}


/* function: TRNG data interruption enable
 * parameters: none
 * return: none
 * caution:
 *     1. works when global interruption is enabled
 */
void trng_data_int_enable()
{
	TRNG_CR |= (1<<TRNG_DATA_INT_OFFSET);
}


/* function: TRNG data interruption disable
 * parameters: none
 * return: none
 * caution:
 */
void trng_data_int_disable()
{
	TRNG_CR &= ~(1<<TRNG_DATA_INT_OFFSET);
}


/* function: TRNG enable
 * parameters: none
 * return: none
 * caution:
 */
void trng_enable()
{
	volatile u32 i;

    TRNG_CR |= 1;

	//sleep for a while
	i=0xFFF;
	while(i--)
	{;}
}


/* function: TRNG disable
 * parameters: none
 * return: none
 * caution:
 */
void trng_disable()
{
    TRNG_CR &= (~1);
}


/* function: set RO entropy config
 * parameters:
 *     cfg ------------------------ RO entropy config, only the low 4 bits are valid, every bit
 *                                  indicates one RO entropy, the MSB is RO 0, and LSB is RO 3
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 *     1. only the low 4 bits of cfg are valid
 *     2. if the low 4 bits of cig is 0, that means to disable all RO entropy
 */
u32 trng_ro_entropy_config(u8 cfg)
{
	if(cfg > 15)
	{
		return TRNG_INVALID_INPUT;
	}
	else
	{;}

	TRNG_CR &= ~(0x0F<<TRNG_RO_ENTROPY_OFFSET);
	TRNG_CR	|= 	(((u32)(cfg&0x0F))<<TRNG_RO_ENTROPY_OFFSET);

	return TRNG_SUCCESS;
}


/* function: set sub RO entropy config
 * parameters:
 *     sn ------------------------- input, RO entropy source series number, must be in [0,3]
 *     value ---------------------- input, the config value of RO sn
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 trng_ro_sub_entropy_config(u8 sn, uint16_t cfg)
{
	switch(sn)
	{
		case 0:
		RO_SRC_EN1 &= ~0xFFFF0000;
		RO_SRC_EN1 |= ((u32)cfg)<<16;
		break;

		case 1:
		RO_SRC_EN1 &= ~0x0000FFFF;
		RO_SRC_EN1 |= (u32)cfg;
		break;

		case 2:
		RO_SRC_EN2 &= ~0xFFFF0000;
		RO_SRC_EN2 |= ((u32)cfg)<<16;
		break;

		case 3:
		RO_SRC_EN2 &= ~0x0000FFFF;
		RO_SRC_EN2 |= (u32)cfg;
		break;

		default:
			return TRNG_INVALID_INPUT;
	}

	return TRNG_SUCCESS;
}


/* function: set TRNG mode
 * parameters:
 *     with_post_processing ------- 0:no,  other:yes
 * return: none
 * caution:
 */
void trng_set_mode(u8 with_post_processing)
{
    if(with_post_processing)
    {
        TRNG_MSEL |= 1;
    }
    else
    {
        TRNG_MSEL &= (~1);
    }

    TRNG_SR |= 0x07; //write 1 to clear
}


/* function: reseed TRNG(works when DRBG is enabled)
 * parameters: none
 * return: none
 * caution:
 *     1. used for DRBG
 */
void trng_reseed()
{
    TRNG_RESEED |= 0x01;

    TRNG_SR |= 0x07; //write 1 to clear
}


/* function: TRNG set frequency
 * parameters:
 *     freq ----------------------- input, frequency config, must be in [0,3], and
 *                                  0: 1/4 of input frequency,
 *                                  1: 1/8 ...,
 *                                  2: 1/16 ...,
 *                                  3: 1/32 ...,
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 trng_set_freq(u8 freq)
{
	if(freq > 3)
	{
		return TRNG_INVALID_INPUT;
	}
	else
	{;}

	SCLK_FREQ = freq;

	return TRNG_SUCCESS;
}


/* function: get some rand words
 * parameters:
 *     a -------------------------- output, random words
 *     words ---------------------- input, word number of output, must be in [1, 8]
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the two parameters are valid
 */
u32 get_rand_uint32(u32 *a, u32 words)
{
	u32 i;

	while(0 == (TRNG_SR & 2))
	{
		if(TRNG_SR & 1)
		{
			return TRNG_HT_ERROR;
		}
		else
		{;}
	}

	for(i=0; i<words; i++)
	{
		*(a++) = TRNG_DR;  //printf("\r\n %08x", *(a-1));
	}

	TRNG_SR |= 2;  //clear

	return TRNG_SUCCESS;
}


/* function: get some rand words(with reseed)
 * parameters:
 *     a -------------------------- output, random words
 *     words ---------------------- input, word number of output, must be in [1, 8]
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure the two parameters are valid
 */
u32 get_rand_uint32_with_reseed(u32 *a, u32 words)
{
	u32 i;

	trng_reseed();

	while(0 == (TRNG_SR & 2))
	{
		if(TRNG_SR & 1)
		{
			return TRNG_HT_ERROR;
		}
		else
		{;}
	}

	for(i=0; i<words; i++)
	{
		*(a++) = TRNG_DR;  //printf("\r\n %08x", *(a-1));
	}

	TRNG_SR |= 2;  //clear

	return TRNG_SUCCESS;
}


/* function: get rand buffer(internal basis interface)
 * parameters:
 *     rand ----------------------- input, byte buffer rand
 *     bytes ---------------------- input, byte length of rand
 *     get_rand_words ------------- input, function pointer to get some random words(at most 8 words)
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 get_rand_buffer(u8 *rand, u32 bytes, GET_RAND_WORDS get_rand_words)
{
	u32 i;
	u32 tmp, tmp_len, rng_data;
	u32 count, ret;
	u8 *a = rand;

	//check input parameters
	if(NULL == rand || NULL == get_rand_words)
	{
		return TRNG_BUFFER_NULL;
	}
	else if(0 == bytes)
	{
		return TRNG_SUCCESS;
	}
	else
	{;}

	//make sure trng and ro are enabled
	if(0 == (TRNG_CR & 1))
	{
		return TRNG_INVALID_CONFIG;
	}
	else if(0 == (TRNG_CR & (0x0F<<TRNG_RO_ENTROPY_OFFSET)))
	{
		return TRNG_INVALID_CONFIG;
	}
	else
	{;}

	tmp_len = bytes;

	tmp = ((u32)(uintptr_t)a) & 3;
	if(tmp)
	{
		i = 4-tmp;

		ret = get_rand_words(&rng_data, 1);
		if(TRNG_SUCCESS != ret)
		{
			goto END;
		}
		else
		{
			if(tmp_len > i)
			{
				memcpy_(a, &rng_data, i);
				a += i;
				tmp_len -= i;
			}
			else
			{
				memcpy_(a, &rng_data, tmp_len);
				goto END;
			}
		}
	}
	else
	{;}

	tmp = tmp_len/4;
	while(tmp)
	{
		if(tmp>8)
		{
			count = 8;
		}
		else
		{
			count = tmp;
		}

		ret = get_rand_words((u32 *)a, count);
		if(TRNG_SUCCESS != ret)
		{
			goto END;
		}
		else
		{
			a += count<<2;
			tmp -= count;
		}
	}

	tmp_len = tmp_len & 3;
	if(tmp_len)
	{
		ret = get_rand_words(&rng_data, 1);
		if(TRNG_SUCCESS != ret)
		{
			goto END;
		}
		else
		{
			memcpy_(a, &rng_data, tmp_len);
		}
	}

	ret = TRNG_SUCCESS;

END:

	if(TRNG_SUCCESS != ret)
	{
		memset_(rand, 0, bytes);
	}
	else
	{;}

#ifdef TRNG_POKER_TEST
	if(TRNG_SUCCESS == ret)
	{
		poker_test(rand, bytes);
	}
#endif

	return ret;
}


/* function: get rand(for internal test)
 * parameters:
 *     rand ----------------------- input, byte buffer rand
 *     bytes ---------------------- input, byte length of rand
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 get_rand_internal(u8 *rand, u32 bytes)
{
	return get_rand_buffer(rand, bytes, get_rand_uint32);
}


/* function: get rand(without entropy reducing)
 * parameters:
 *     rand ----------------------- input, byte buffer rand
 *     bytes ---------------------- input, byte length of rand
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 get_rand(u8 *prand, u32 bytes)
{
#if 0
	//with post-processing
	if(0 == TRNG_MSEL)
	{
		trng_disable();
		trng_set_mode(1);
		trng_enable();
	}
	else
	{;}

	return get_rand_buffer(rand, bytes, get_rand_uint32_with_reseed);
#else
    u8 *a = prand;
    u32 value = 0;

    if((prand == NULL) || (bytes == 0)) {
        return 1;
    }
    //srand((unsigned)time(NULL));

    for(int i =0;i<bytes; i++) {
        value = rand();
        *(a++) = (u8)(value) & 0xff;
    }

    return 0;
#endif
}


/*********************************** TREO ************************************/

/* function: TERO RNG enable
 * parameters: none
 * return: none
 * caution:
 */
void tero_enable()
{
	volatile u32 i;

	TERO_CR |= 1;

	//sleep for a while
	i=0xFFF;
	while(i--)
	{;}
}


/* function: TERO RNG disable
 * parameters: none
 * return: none
 * caution:
 */
void tero_disable()
{
	TERO_CR &= ~1;
}


/* function: TERO RNG set the system cycle threshold of the TERO counter kept,
 * parameters:
 *     threshold_value ------------ input, threshold value
 * return: none
 * caution:
 */
u32 tero_set_stop_threshold(u8 threshold_value)
{
	if(0 == threshold_value)
	{
		return TRNG_INVALID_INPUT;
	}
	else
	{;}

	TERO_CR &= ~(0xFF<<TRNG_TERO_THRESHOLD_OFFSET);
	TERO_CR |= (((u32)threshold_value)<<TRNG_TERO_THRESHOLD_OFFSET);

	return TRNG_SUCCESS;
}


/* function: set TERO entropy config
 * parameters:
 *     cfg ------------------------ RO entropy config, only the low 4 bits are valid, every bit
 *                                  indicates one RO entropy, the LSB is TERO 0, and MSB is RO 3
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 *     1. only the low 4 bits of cfg are valid
 *     2. if the low 4 bits of cig is 0, that means to disable all TERO entropy
 */
u32 tero_entropy_config(u8 cfg)
{
	if(cfg > 15)
	{
		return TRNG_INVALID_INPUT;
	}
	else
	{;}

	TERO_CR &= ~(0x0F<<TRNG_TERO_ENTROPY_OFFSET);
	TERO_CR	|= 	(((u32)(cfg&0x0F))<<TRNG_TERO_ENTROPY_OFFSET);

	return TRNG_SUCCESS;
}


/* function: TERO RNG set output as rng
 * parameters: none
 * return: none
 * caution:
 */
void tero_set_output_rng()
{
	TERO_CR &= ~(1<<1);
}


/* function: TERO RNG set output as oscillation times
 * parameters: none
 * return: none
 * caution:
 */
void tero_set_output_osc_times()
{
	TERO_CR |= (1<<1);
}


/* function: select TREO 1&2 or TERO 3&4 when output is oscillation times
 * parameters:
 *     cfg ------------------------ input, 0:TREO 1&2, other:TREO 3&4
 * return: none
 * caution:
 */
void tero_set_osc_sel(u8 cfg)
{
	if(0 == cfg)
	{
		TERO_CR &= ~(1<<2);
	}
	else
	{
		TERO_CR |= (1<<2);
	}
}


/* function: set lower limit of oscillation times
 * parameters:
 *     value ---------------------- input, lower limit value
 * return: none
 * caution:
 */
void tero_set_osc_times_lower_limit(uint16_t value)
{
	TERO_THOLD &= ~(0xFF<<16);
	TERO_THOLD |= ((u32)value)<<16;
}


/* function: set upper limit of oscillation times
 * parameters:
 *     value ---------------------- input, upper limit value
 * return: none
 * caution:
 */
void tero_set_osc_times_upper_limit(uint16_t value)
{
	TERO_THOLD &= ~(0xFF);
	TERO_THOLD |= ((u32)value);
}


/* function: get tero rand
 * parameters:
 *     a -------------------------- input, byte buffer a
 *     bytes ---------------------- input, byte length of rand
 * return: TRNG_SUCCESS(success), other(error)
 * caution:
 */
u32 get_tero_rand(u8 *a, u32 bytes)
{
	u32 i;
	u32 tmp, rng_data;
#ifdef TRNG_POKER_TEST
	u32 tmp_len;
#endif

	//check input parameters
	if(NULL == a)
	{
		return TRNG_BUFFER_NULL;
	}
	else if(0 == bytes)
	{
		return TRNG_SUCCESS;
	}
	else
	{;}

	//make sure tero config is valid
	if(0 == (TERO_CR & 1))
	{
		return TRNG_INVALID_CONFIG;
	}
	else if(TERO_CR & (1<<1))
	{
		return TRNG_INVALID_CONFIG;
	}
	else if(0 == (TERO_CR & (0x0F<<TRNG_TERO_ENTROPY_OFFSET)))
	{
		return TRNG_INVALID_CONFIG;
	}
	else
	{;}

#ifdef TRNG_POKER_TEST
	tmp_len = bytes;
#endif

	tmp = ((u32)(uintptr_t)a) & 3;
	if(tmp)
	{
		i = 4-tmp;

		while(0 == (TERO_SR & 1))
		{;}
		rng_data = TERO_DR;//printf("\r\n %08x", rng_data);
		if(bytes > i)
		{
			memcpy_(a, &rng_data, i);
			a += i;
			bytes -= i;
		}
		else
		{
			memcpy_(a, &rng_data, bytes);
			return TRNG_SUCCESS;
		}
	}

	tmp = bytes/4;
	while(tmp)
	{
		while(0 == (TERO_SR & 1))
		{;}
		*((u32 *)a) = TERO_DR;//printf("\r\n %08x", *((u32 *)a));

		a += 4;
		tmp--;
	}

	bytes = bytes & 3;
	if(bytes)
	{
		while(0 == (TERO_SR & 1))
		{;}
		rng_data = TERO_DR;//printf("\r\n %08x", rng_data);
		memcpy_(a, &rng_data, bytes);
	}

#ifdef TRNG_POKER_TEST
	poker_test(a-tmp_len, tmp_len);
#endif

	return TRNG_SUCCESS;
}

