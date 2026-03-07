// SPDX-License-Identifier: GPL-2.0+
/*
 * Tsingmicro PKE Driver
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

#include "ts_utility.h"
#include "ts_pke.h"
#include "ts_trng.h"
#include "ts_eccp_curve.h"

typedef struct {
	void __iomem *regs;
	u32	irq;
} ts_pke_priv_t;

static ts_pke_priv_t s_ts_pke_instance;
ts_pke_priv_t *g_ts_pke;

static unsigned int step;

/* function: load input operand to baseaddr
 * parameters:
 *     baseaddr ------------------- output, destination data
 *     data ----------------------- input, source data
 *     wordLen -------------------- input, word length of data
 * return: none
 * caution:
 */
void pke_load_operand(u32 *baseaddr, u32 *data, u32 wordLen)
{
	if(baseaddr != data)
	{
		uint32_copy(baseaddr, data, wordLen);
	}
}


/* function: get result
 * parameters:
 *     baseaddr ------------------- input, source data
 *     data ----------------------- output, destination data
 *     wordLen -------------------- input, word length of data
 * return: none
 * caution:
 */
static void pke_read_operand(u32 *baseaddr, u32 *data, u32 wordLen)
{
	u32 i;

	if(baseaddr != data)
	{
		for (i = 0; i < wordLen; i++)
		{
			data[i] = *((volatile u32 *)baseaddr);
			baseaddr++;
		}
	}
}

volatile u32 *PKE_A(int a, int step)
{
	return ((volatile u32 *)(g_ts_pke->regs + OSR_PKE_A + (a)*(step)));
}

volatile u32 *PKE_B(int a, int step)
{
	return ((volatile u32 *)(g_ts_pke->regs + OSR_PKE_B + (a)*(step)));

}


/* function: clear and disable interrupt tag
 * parameters: none
 * return: none
 * caution:
 */
void pke_clear_interrupt(void)
{
	u32 value = readl(g_ts_pke->regs + OSR_PKE_STAT);
	if(value & 1)
	{
		value |= 0x01;
		writel(value, g_ts_pke->regs + OSR_PKE_STAT);
	}
}

/* function: enable pke interrupt
 * parameters: none
 * return: none
 * caution:
 */
void pke_enable_interrupt(void)
{
	u32 value = readl(g_ts_pke->regs + OSR_PKE_CFG);
	value |= (1 << PKE_INT_ENABLE_OFFSET);
	writel(value, g_ts_pke->regs + OSR_PKE_CFG);
}


/* function: disable pke interrupt
 * parameters: none
 * return: none
 * caution:
 */
void pke_disable_interrupt(void)
{
	u32 value = readl(g_ts_pke->regs + OSR_PKE_CFG);
	value &= ~(1 << PKE_INT_ENABLE_OFFSET);
	writel(value, g_ts_pke->regs + OSR_PKE_CFG);

}


/* function: set operand width
 * caution: please make sure 0 < bitLen <= 4096
 */
void pke_set_operand_width(u32 bitLen)
{
	u32 cfg = 0, len = 0;

	len = (bitLen+255)/256;

	if(1 == len)
	{
		cfg = 2;
		step = 0x24;
	}
	else if(2 == len)
	{
		cfg = 3;
		step = 0x44;
	}
	else if(len <= 4)
	{
		cfg = 4;
		step = 0x84;
	}
	else if(len <= 8)
	{
		cfg = 5;
		step = 0x104;
	}
	else if(len <= 16)
	{
		cfg = 6;
		step = 0x204;
	}

	cfg = (cfg<<24)|(len<<19);
	u32 value = readl(g_ts_pke->regs + OSR_PKE_CFG);
	value &= ~(0x07FF<<16);
	writel(value, g_ts_pke->regs + OSR_PKE_CFG);

	value = readl(g_ts_pke->regs + OSR_PKE_CFG);
	value |= cfg;
	writel(value, g_ts_pke->regs + OSR_PKE_CFG);

}


/* function: get current operand byte length
 */
u32 pke_get_operand_bytes(void)
{
	return step;
}


/* function: set operation micro code
 * parameters:
 *     addr ----------------------- input, specific micro code
 * return: none
 * caution:
 */
void pke_set_microcode(u32 addr)
{
	writel(addr, g_ts_pke->regs + OSR_PKE_MC_PTR);
}

/* function: set exe config
 * parameters:
 *     cfg ------------------------ input, specific config value
 * return: none
 * caution:
 */
void pke_set_exe_cfg(u32 cfg)
{
	writel(cfg, g_ts_pke->regs + OSR_PKE_EXE_CONF);
}

/* function: start pke calc
 * parameters: none
 * return: none
 * caution:
 */
void pke_start(void)
{
	u32 value  = readl(g_ts_pke->regs + OSR_PKE_CTRL);
	value |= PKE_START_CALC;
	writel(value, g_ts_pke->regs + OSR_PKE_CTRL);
}


/* function: return calc return code
 * parameters: none
 * return 0(success), other(error)
 * caution:
 */
u8 pke_check_rt_code(void)
{
	u32 value  = readl(g_ts_pke->regs + OSR_PKE_RT_CODE);
	return value & 0x07;
}

/* function: wait till done
 * parameters: none
 * return: none
 * caution:
 */
void pke_wait_till_done(void)
{
	u32 value  = readl(g_ts_pke->regs + OSR_PKE_STAT);
	while(!(value & 1))
	{
		value  = readl(g_ts_pke->regs + OSR_PKE_STAT);
	}
}


/* function: ainv = a^(-1) mod modulus
 * parameters:
 *     modulus -------------------- input, modulus
 *     a -------------------------- input, integer a
 *     ainv ----------------------- output, ainv = a^(-1) mod modulus
 *     modWordLen ----------------- input, word length of modulus and ainv
 *     aWordLen ------------------- input, word length of a
 * return: PKE_SUCCESS(success), other(inverse not exists or error)
 * caution:
 *     1. please make sure aWordLen <= modWordLen <= OPERAND_MAX_WORD_LEN and a < modulos
 */
u8 pke_modinv(const u32 *modulus, const u32 *a, u32 *ainv, u32 modWordLen,//----------------------
				   u32 aWordLen)
{
	u8 ret;

	pke_set_operand_width(modWordLen<<5);

	pke_load_operand((u32 *)(PKE_B(3,step)), (u32 *)modulus, modWordLen); //B3 modulus
	if((step/4) > modWordLen)
	{
		uint32_clear((u32 *)(PKE_B(3,step))+modWordLen, (step/4)-modWordLen);
	}

	pke_load_operand((u32 *)(PKE_B(0,step)), (u32 *)a, aWordLen);         //B0 a
	if((step/4) > aWordLen)
	{
		uint32_clear((u32 *)(PKE_B(0,step))+aWordLen, (step/4)-aWordLen);
	}

	pke_set_microcode(MICROCODE_MODINV);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	ret = pke_check_rt_code();
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pke_read_operand((u32 *)(PKE_A(0,step)), ainv, modWordLen);                //A0 ainv

	return PKE_SUCCESS;
}


/* function: out = (a+b) mod modulus
 * parameters:
 *     modulus -------------------- input, modulus
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a+b mod modulus
 *     wordLen -------------------- input, word length of modulus, a, b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. a,b must be less than modulus
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u8 pke_modadd(const u32 *modulus, const u32 *a, const u32 *b,//----------------------
				   u32 *out, u32 wordLen)
{
	u8 ret;

	pke_set_operand_width(wordLen<<5);

	pke_load_operand((u32 *)(PKE_B(3,step)), (u32 *)modulus, wordLen);    //B3 modulus
	pke_load_operand((u32 *)(PKE_A(0,step)), (u32 *)a, wordLen);          //A0 a
	pke_load_operand((u32 *)(PKE_B(0,step)), (u32 *)b, wordLen);          //B0 b

	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_B(3,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_A(0,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_B(0,step))+wordLen, (step/4)-wordLen);
	}

	pke_set_microcode(MICROCODE_MODADD);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	ret = pke_check_rt_code();
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pke_read_operand((u32 *)(PKE_A(0, step)), out, wordLen);                   //A0 result

	return PKE_SUCCESS;
}


/* function: out = (a-b) mod modulus
 * parameters:
 *     modulus -------------------- input, modulus
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a-b mod modulus
 *     wordLen -------------------- input, word length of modulus, a, b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. a,b must be less than modulus
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u8 pke_modsub(const u32 *modulus, const u32 *a, const u32 *b,//----------------------
				   u32 *out, u32 wordLen)
{
	u8 ret;

	pke_set_operand_width(wordLen<<5);

	pke_load_operand((u32 *)(PKE_B(3,step)), (u32 *)modulus, wordLen);    //B3 modulus
	pke_load_operand((u32 *)(PKE_A(0,step)), (u32 *)a, wordLen);          //A0 a
	pke_load_operand((u32 *)(PKE_B(0,step)), (u32 *)b, wordLen);          //B0 b

	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_B(3,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_A(0,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_B(0,step))+wordLen, (step/4)-wordLen);
	}

	pke_set_microcode(MICROCODE_MODSUB);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	ret = pke_check_rt_code();
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pke_read_operand((u32 *)(PKE_A(0,step)), out, wordLen);                    //A0 result

	return PKE_SUCCESS;
}


/* function: out = a+b
 * parameters:
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a+b
 *     wordLen -------------------- input, word length of a, b, out
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. a+b may overflow
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u8 pke_add(const u32 *a, const u32 *b, u32 *out, u32 wordLen)//---------------------------
{
	u32 i, carry, temp, temp2;

	carry = 0;
	for(i=0; i<wordLen; i++)
	{
		temp2 = a[i];
		temp = a[i]+b[i];
		out[i] = temp+carry;
		if(temp < temp2 || out[i] < carry)
		{
			carry = 1;
		}
		else
		{
			carry = 0;
		}
	}

	return PKE_SUCCESS;
}

/* function: out = a-b
 * parameters:
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a-b
 *     wordLen -------------------- input, word length of a, b, out
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure a > b
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */

u8 pke_sub(const u32 *a, const u32 *b, u32 *out, u32 wordLen)//-------------------------------
{
	u32 i, carry, tmp, tmp2;

	carry = 0;
	for(i=0; i<wordLen; i++)
	{
		tmp = a[i]-b[i];
		tmp2 = tmp-carry;
		if(tmp > a[i] || tmp2 > tmp)
		{
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		out[i] = tmp2;
	}

	return PKE_SUCCESS;
}

/* function: out = a*b
 * parameters:
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a*b
 *     wordLen -------------------- input, word length of a, b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure out buffer word length is bigger than (2*max_bit_len(a,b)+0x1F)>>5
 *     2. please make sure ab_wordLen is not bigger than OPERAND_MAX_WORD_LEN/2
 */

u8 pke_mul(const u32 *a, const u32 *b, u32 *out, u32 ab_wordLen)//-----------------------------
{
	u32 bitLen, tempLen;
	u8 ret;

	bitLen = get_valid_bits(a, ab_wordLen);
	tempLen = get_valid_bits(b, ab_wordLen);

	bitLen = GET_MAX_LEN(bitLen,tempLen);
	tempLen = GET_WORD_LEN(bitLen<<1);
	if(tempLen < (ab_wordLen<<1))
	{
		tempLen = (ab_wordLen<<1)-1;
	}
	else
	{
		tempLen = (ab_wordLen<<1);
	}

	//pke_set_operand_width(tempLen<<5);
	pke_set_operand_width(GET_MAX_LEN(tempLen<<5,512));

	pke_load_operand((u32 *)(PKE_A(0,step)), (u32 *)a, ab_wordLen);       //A0 a
	pke_load_operand((u32 *)(PKE_B(0,step)), (u32 *)b, ab_wordLen);       //B0 b

	uint32_clear((u32 *)(PKE_A(0,step))+ab_wordLen, (step/4)-ab_wordLen);
	uint32_clear((u32 *)(PKE_B(0,step))+ab_wordLen, (step/4)-ab_wordLen);

	pke_set_microcode(MICROCODE_INTMUL);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	ret = pke_check_rt_code();
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pke_read_operand((u32 *)(PKE_A(1,step)), out, tempLen);    //A1 result

	return ret;
}


/* function: calc n0(- modulus ^(-1) mod 2^w) for modMul, and pointMul. etc.
 * parameters: none
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. before calling, please make sure the modulus is set in PKE_A(a, 0)
 *     2. please make sure the modulus is odd, and word length of the modulus
 *        is not bigger than OPERAND_MAX_WORD_LEN
 *     3. the result is set in the internal register, no need to output.//---------------------------

u8 pke_pre_calc_mont_N0()
{
	pke_set_microcode(MICROCODE_MGMR_PRE_N0);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	return pke_check_rt_code();
} */


/* function: calc H(R^2 mod modulus) and n1( - modulus ^(-1) mod 2^w ) for modMul,modExp, and pointMul. etc.
 *           here w is bit width of word, i,e. 32.
 * parameters:
 *     modulus -------------------- input, modulus
 *     wordLen -------------------- input, word length of modulus or H
 *     H -------------------------- output, R^2 mod modulus
 *     n1 ------------------------- output,  - modulus ^(-1) mod 2^w, here w is 32 acutally
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. modulus must be odd
 *     2. please make sure word length of buffer H is equal to wordLen(word length of modulus),
 *        and n1 only need one word.
 *     3. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u8 pke_pre_calc_mont(const u32 *modulus, u32 wordLen, u32 *H, u32 *n1)
{
	u8 ret;

	pke_set_operand_width(wordLen<<5);

	pke_load_operand((u32 *)(PKE_B(3,step)), (u32 *)modulus, wordLen);    //B3 modulus

	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_B(3,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_A(3,step))+wordLen, (step/4)-wordLen);
	}

	pke_set_microcode(MICROCODE_MGMR_PRE);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	ret = pke_check_rt_code();
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	if(NULL != H)
	{
		pke_read_operand((u32 *)(PKE_A(3,step)), H, wordLen);                  //A3 H
	}

	if(NULL != n1)
	{
		pke_read_operand((u32 *)(PKE_B(4,step)), n1, 1);                       //B4 n1
	}

	return PKE_SUCCESS;
}


/* function: like function pke_pre_calc_mont(), but this one is without output here
 * parameters:
 *     modulus -------------------- input, modulus
 *     wordLen -------------------- input, word length of modulus
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. modulus must be odd
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u8 pke_pre_calc_mont_no_output(const u32 *modulus, u32 wordLen)
{
	return pke_pre_calc_mont(modulus, wordLen, NULL, NULL);
}


/* function: load the pre-calculated mont parameters H(R^2 mod modulus)
 * parameters:
 *     H -------------------------- input, R^2 mod modulus
 *     n1 ------------------------- input,  - modulus ^(-1) mod 2^w, here w is 32 acutally
 *     wordLen -------------------- input, word length of modulus or H
 * return: none
 * caution:
 *     1. please make sure the 2 input parameters are both valid
 *     2. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
void pke_load_pre_calc_mont(u32 *H, u32 *n1, u32 wordLen)
{
	pke_set_operand_width(wordLen<<5);

	pke_load_operand((u32 *)(PKE_A(3,step)), H, wordLen);
	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_A(3,step))+wordLen, (step/4)-wordLen);
	}
	pke_load_operand((u32 *)(PKE_B(4,step)), n1, 1);
}


/* function: out = a*b mod modulus
 * parameters:
 *     modulus -------------------- input, modulus
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a*b mod modulus
 *     wordLen -------------------- input, word length of modulus, a, b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. modulus must be odd
 *     2. a, b must less than modulus
 *     3. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 *     4. before calling this function, please make sure the pre-calculated mont arguments
 *        of modulus is located in the right address.
 */
u8 pke_modmul_internal(const u32 *modulus, const u32 *a, const u32 *b, u32 *out,
							u32 wordLen)
{
	u8 ret;

	pke_set_operand_width(wordLen<<5);

	pke_load_operand((u32 *)(PKE_B(3,step)), (u32 *)modulus, wordLen);    //B3 modulus
	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_B(3,step))+wordLen, (step/4)-wordLen);
	}

	pke_load_operand((u32 *)(PKE_A(0,step)), (u32 *)a, wordLen);          //A0 a
	pke_load_operand((u32 *)(PKE_B(0,step)), (u32 *)b, wordLen);          //B0 b
	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_A(0,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_B(0,step))+wordLen, (step/4)-wordLen);
	}

	pke_set_microcode(MICROCODE_MODMUL);

	//pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	ret = pke_check_rt_code();
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pke_read_operand((u32 *)(PKE_A(0,step)), out, wordLen);                    //A0 out

	return PKE_SUCCESS;
}


/* function: out = a*b mod modulus
 * parameters:
 *     modulus -------------------- input, modulus
 *     a -------------------------- input, integer a
 *     b -------------------------- input, integer b
 *     out ------------------------ output, out = a*b mod modulus
 *     wordLen -------------------- input, word length of modulus, a, b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. modulus must be odd
 *     2. a, b must less than modulus
 *     3. wordLen must not be bigger than OPERAND_MAX_WORD_LEN
 */
u8 pke_modmul(const u32 *modulus, const u32 *a, const u32 *b, u32 *out, u32 wordLen)
{
	u8 ret;

	ret = pke_pre_calc_mont(modulus, wordLen, NULL, NULL);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);

	return pke_modmul_internal(modulus, a, b, out, wordLen);
}


/* function: mod exponent, this could be used for rsa encrypting,decrypting,signing,verifing.
 * parameters:
 *     modulus -------------------- input, modulus
 *     exponent ------------------- input, exponent
 *     base ----------------------- input, base number
 *     out ------------------------ output, out = a^(b) mod modulus
 *     mod_wordLen ---------------- input, word length of modulus and base number
 *     exp_wordLen ---------------- input, word length of exponent
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. before calling this function, please make sure R^2 mod modulos, the pre-calculated
 *        mont arguments of modulus is located in the right address
 *     2. modulus must be odd
 *     3. please make sure exp_wordLen <= mod_wordLen <= OPERAND_MAX_WORD_LEN
 */
u8 pke_modexp(const u32 *modulus, const u32 *exponent, const u32 *base,//------------------------------------------
				   u32 *out, u32 mod_wordLen, u32 exp_wordLen)
{
	u8 ret;

	pke_set_operand_width(mod_wordLen<<5);

	pke_load_operand((u32 *)(PKE_A(1,step)), (u32 *)exponent, exp_wordLen);   //A1 exponent
	if((step/4) > exp_wordLen)
	{
		uint32_clear((u32 *)(PKE_A(1,step))+exp_wordLen, (step/4)-exp_wordLen);
	}

	pke_load_operand((u32 *)(PKE_B(3,step)), (u32 *)modulus, mod_wordLen);    //B3 modulus
	pke_load_operand((u32 *)(PKE_B(0,step)), (u32 *)base, mod_wordLen);       //B0 base

	if((step/4) > mod_wordLen)
	{
		uint32_clear((u32 *)(PKE_B(3,step))+mod_wordLen, (step/4)-mod_wordLen);
		uint32_clear((u32 *)(PKE_B(0,step))+mod_wordLen, (step/4)-mod_wordLen);
	}

	pke_set_microcode(MICROCODE_MODEXP);

	pke_set_exe_cfg(PKE_EXE_CFG_MOD_EXP);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	ret = pke_check_rt_code();
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pke_read_operand((u32 *)(PKE_A(0,step)), out, mod_wordLen);    //A0 result

	return ret;
}


/* function: c = a mod b
 * parameters:
 *     a -------------------------- input, integer a
 *     aWordLen ------------------- input, word length of integer
 *     b -------------------------- input, integer b, modulus
 *     bWordLen ------------------- input, word length of integer b
 *     c -------------------------- output, c = a mod b
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. b must be odd, and please make sure bWordLen is real word length of b
 *     2. pleae make sure aWordLen <= 256, bWordLen <= OPERAND_MAX_WORD_LEN,
 *     3. real bit length of a can not be bigger than 2*(real bit length of b)
 */
u8 pke_mod(u32 *a, u32 aWordLen, u32 *b, u32 bWordLen, u32 *c)  //----------------------
{
	s32 flag;
	u32 bitLen, tmpLen;
	u32 *A1, *B2;
	u8 ret;

	flag = uint32_BigNumCmp(a, aWordLen, b, bWordLen);
	if(flag < 0)
	{
		aWordLen = get_valid_words(a, aWordLen);
		uint32_copy(c, a, aWordLen);
		uint32_clear(c+aWordLen, bWordLen-aWordLen);

		return PKE_SUCCESS;
	}
	else if(0 == flag)
	{
		uint32_clear(c, bWordLen);

		return PKE_SUCCESS;
	}

	pke_set_operand_width(bWordLen<<5);
	A1 = (u32 *)(PKE_A(1, step));
	B2 = (u32 *)(PKE_B(2, step));

	bitLen = get_valid_bits(b, bWordLen) & 0x1F;

	//get A1 = 1000...000 mod b
	uint32_clear(B2, bWordLen);
	if(bitLen)
	{
		B2[bWordLen-1] = 1<<(bitLen);
	}

	ret = pke_sub(B2, b, A1, bWordLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//get B2 = a high part mod b
	if(bitLen)
	{
		tmpLen = aWordLen-bWordLen+1;
		uint32_copy(B2, a+bWordLen-1, tmpLen);
		Big_Div2n(B2, tmpLen, bitLen);
		if(tmpLen < bWordLen)
		{
			uint32_clear(B2+tmpLen, bWordLen-tmpLen);
		}

		if(uint32_BigNumCmp(B2, bWordLen, b, bWordLen) >= 0)
		{
			ret = pke_sub(B2, b, B2, bWordLen);
			if(PKE_SUCCESS != ret)
			{
				return ret;
			}
		}
	}
	else
	{
		tmpLen = aWordLen - bWordLen;
		if(uint32_BigNumCmp(a+bWordLen, tmpLen, b, bWordLen) >= 0)
		{
			ret = pke_sub(a+bWordLen, b, B2, bWordLen);
			if(PKE_SUCCESS != ret)
			{
				return ret;
			}
		}
		else
		{
			uint32_copy(B2, a+bWordLen, tmpLen);
			uint32_clear(B2+tmpLen, bWordLen-tmpLen);
		}
	}

	//set the pre-calculated mont parameter
	ret = pke_pre_calc_mont(b, bWordLen, NULL, NULL);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//get B2 = a_high * 1000..000 mod b
	pke_set_exe_cfg(PKE_EXE_CFG_ALL_NON_MONT);
	ret = pke_modmul_internal(b, A1, B2, B2, bWordLen);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	//get A1 = a low part mod b
	if(bitLen)
	{
		uint32_copy(A1, a, bWordLen);
		A1[bWordLen-1] &= ((1<<(bitLen))-1);
		if(uint32_BigNumCmp(A1, bWordLen, b, bWordLen) >= 0)
		{
			ret = pke_sub(A1, b, A1, bWordLen);
			if(PKE_SUCCESS != ret)
			{
				return ret;
			}
		}
	}
	else
	{
		if(uint32_BigNumCmp(a, bWordLen, b, bWordLen) >= 0)
		{
			ret = pke_sub(a, b, A1, bWordLen);
			if(PKE_SUCCESS != ret)
			{
				return ret;
			}
		}
		else
		{
			A1 = a;
		}
	}

	return pke_modadd(b, A1, B2, c, bWordLen);
}


/********************************** ECCp functions *************************************/

/* function: ECCP curve point mul(random point), Q=[k]P
 * parameters:
 *     curve ---------------------- input, ECCp curve struct pointer
 *     k -------------------------- input, scalar
 *     Px ------------------------- input, x coordinate of point P
 *     Py ------------------------- input, y coordinate of point P
 *     Qx ------------------------- output, x coordinate of point Q
 *     Qy ------------------------- output, y coordinate of point Q
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure k in [1,n-1], n is order of ECCP curve
 *     2. please make sure input point P is on the curve
 *     3. please make sure bit length of the curve is not bigger than 521(ECC521)
 */
u8 eccp_pointMul(eccp_curve_t *curve, u32 *k, u32 *Px, u32 *Py,
					  u32 *Qx, u32 *Qy)
{
	u32 wordLen = GET_WORD_LEN(curve->eccp_p_bitLen);
	u8 ret;

	pke_set_operand_width(curve->eccp_p_bitLen);

	pke_load_operand((u32 *)(PKE_B(3,step)), curve->eccp_p, wordLen);          //B3 p
	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_B(3,step))+wordLen, (step/4)-wordLen);
	}

	//set ecc_p_h & ecc_p_n1
	if((NULL == curve->eccp_p_h) || (NULL == curve->eccp_p_n1))
	{
		pke_pre_calc_mont((u32 *)(PKE_B(3,step)), wordLen, NULL, NULL);
	}
	else
	{
		pke_load_pre_calc_mont(curve->eccp_p_h, curve->eccp_p_n1, wordLen);
	}

	pke_load_operand((u32 *)(PKE_B(0,step)), Px, wordLen);                     //B0 Px
	pke_load_operand((u32 *)(PKE_B(1,step)), Py, wordLen);                     //B1 Py
	pke_load_operand((u32 *)(PKE_A(5,step)), curve->eccp_a, wordLen);          //A5 a
	pke_load_operand((u32 *)(PKE_A(4,step)), k, wordLen);                      //A4 k
	pke_load_operand((u32 *)(PKE_B(5,step)), curve->eccp_n, wordLen);          //B5 n

	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_B(0,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_B(1,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_A(5,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_A(4,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_B(5,step))+wordLen, (step/4)-wordLen);
	}

	pke_set_microcode(MICROCODE_PMUL);

	pke_set_exe_cfg(PKE_EXE_ECCP_EXE_CFG);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	ret = pke_check_rt_code();
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pke_read_operand((u32 *)(PKE_A(0,step)), Qx, wordLen);                     //A0 Qx
	if(NULL != Qy)
	{
		pke_read_operand((u32 *)(PKE_A(1,step)), Qy, wordLen);                 //A1 Qy
	}

	return PKE_SUCCESS;
}


/* function: ECCP curve point add, Q=P1+P2
 * parameters:
 *     curve ---------------------- input, ECCp curve struct pointer
 *     P1x ------------------------ input, x coordinate of point P1
 *     P1y ------------------------ input, y coordinate of point P1
 *     P2x ------------------------ input, x coordinate of point P2
 *     P2y ------------------------ input, y coordinate of point P2
 *     Qx ------------------------- output, x coordinate of point Q=P1+P2
 *     Qy ------------------------- output, y coordinate of point Q=P1+P2
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure input point P1 and P2 are both on the curve
 *     2. please make sure bit length of the curve is not bigger than 521(ECC521)
 */
u8 eccp_pointAdd(eccp_curve_t *curve, u32 *P1x, u32 *P1y, u32 *P2x, u32 *P2y,
					  u32 *Qx, u32 *Qy)
{
	u32 wordLen = GET_WORD_LEN(curve->eccp_p_bitLen);
	u8 ret;

	pke_set_operand_width(curve->eccp_p_bitLen);

	pke_load_operand((u32 *)(PKE_B(3,step)), curve->eccp_p, wordLen);          //B3 p
	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_B(3,step))+wordLen, (step/4)-wordLen);
	}

	//set ecc_p_h & ecc_p_n1
	if((NULL == curve->eccp_p_h) || (NULL == curve->eccp_p_n1))
	{
		pke_pre_calc_mont((u32 *)(PKE_B(3,step)), wordLen, NULL, NULL);
	}
	else
	{
		pke_load_pre_calc_mont(curve->eccp_p_h, curve->eccp_p_n1, wordLen);
	}

	//pke_pre_calc_mont() may cover A1, so load A1(P1x) here
	pke_load_operand((u32 *)(PKE_A(0,step)), P1x, wordLen);                    //A0 P1x
	pke_load_operand((u32 *)(PKE_A(1,step)), P1y, wordLen);                    //A1 P1y
	pke_load_operand((u32 *)(PKE_B(0,step)), P2x, wordLen);                    //B0 P2x
	pke_load_operand((u32 *)(PKE_B(1,step)), P2y, wordLen);                    //B1 P2y
	pke_load_operand((u32 *)(PKE_A(5,step)), curve->eccp_a, wordLen);          //A5 a

	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_A(0,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_A(1,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_B(0,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_B(1,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_A(5,step))+wordLen, (step/4)-wordLen);
	}

	pke_set_microcode(MICROCODE_PADD);

	pke_set_exe_cfg(PKE_EXE_ECCP_EXE_CFG);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	ret = pke_check_rt_code();
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	pke_read_operand((u32 *)(PKE_A(0,step)), Qx, wordLen);                     //A0 Qx
	if(Qy != NULL)
	{
		pke_read_operand((u32 *)(PKE_A(1,step)), Qy, wordLen);                 //A1 Qy
	}

	return PKE_SUCCESS;
}

/* function: check whether the input point P is on ECCP curve or not
 * parameters:
 *     curve ---------------------- input, ECCp curve struct pointer
 *     Px ------------------------- input, x coordinate of point P
 *     Py ------------------------- input, y coordinate of point P
 * return: PKE_SUCCESS(success, on the curve), other(error or not on the curve)
 * caution:
 *     1. please make sure bit length of the curve is not bigger than 521(ECC521)
 *     2. after calculation, A1 and A2 will be changed!
 */
u8 eccp_pointVerify(eccp_curve_t *curve, u32 *Px, u32 *Py)
{
	u32 wordLen = GET_WORD_LEN(curve->eccp_p_bitLen);

	pke_set_operand_width(curve->eccp_p_bitLen);

	pke_load_operand((u32 *)(PKE_B(3,step)), curve->eccp_p, wordLen);          //B3 p
	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_B(3,step))+wordLen, (step/4)-wordLen);
	}

	//set ecc_p_h & ecc_p_n1
	if((NULL == curve->eccp_p_h) || (NULL == curve->eccp_p_n1))
	{
		pke_pre_calc_mont((u32 *)(PKE_B(3,step)), wordLen, NULL, NULL);
	}
	else
	{
		pke_load_pre_calc_mont(curve->eccp_p_h, curve->eccp_p_n1, wordLen);
	}

	//pke_pre_calc_mont() may cover A1, so load A1(Px) and other paras here
	pke_load_operand((u32 *)(PKE_B(0,step)), Px, wordLen);                     //B0 Px
	pke_load_operand((u32 *)(PKE_B(1,step)), Py, wordLen);                     //B1 Py
	pke_load_operand((u32 *)(PKE_A(5,step)), curve->eccp_a, wordLen);          //A5 a
	pke_load_operand((u32 *)(PKE_A(4,step)), curve->eccp_b, wordLen);          //A4 b

	if((step/4) > wordLen)
	{
		uint32_clear((u32 *)(PKE_B(0,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_B(1,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_A(5,step))+wordLen, (step/4)-wordLen);
		uint32_clear((u32 *)(PKE_A(4,step))+wordLen, (step/4)-wordLen);
	}

	pke_set_microcode(MICROCODE_PVER);

	pke_set_exe_cfg(PKE_EXE_ECCP_EXE_CFG);

	pke_clear_interrupt();

	pke_start();

	pke_wait_till_done();

	return pke_check_rt_code();
}


/* function: get ECCP public key from private key(the key pair could be used in SM2/ECDSA/ECDH)
 * parameters:
 *     curve ---------------------- input, eccp_curve_t struct pointer
 *     priKey --------------------- input, private key, big-endian
 *     pubKey --------------------- output, public key, big-endian
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure bit length of the curve is not bigger than 521(ECC521)
 */
u8 eccp_get_pubkey_from_prikey(eccp_curve_t *curve, u8 *priKey, u8 *pubKey)
{
	u32 nByteLen = GET_BYTE_LEN(curve->eccp_n_bitLen);
	u32 nWordLen = GET_WORD_LEN(curve->eccp_n_bitLen);
	u32 pByteLen = GET_BYTE_LEN(curve->eccp_p_bitLen);
	u32 k[ECCP_MAX_WORD_LEN]={0};
	u32 *x;
	u32 *y;
	u8 ret;

	pke_set_operand_width(curve->eccp_p_bitLen);
	x = (u32 *)(PKE_A(0,step));
	y = (u32 *)(PKE_A(1,step));

	reverse_byte_array(priKey, (u8 *)k, nByteLen);

	//make sure k in [1, n-1]
	if(uint32_BigNum_Check_Zero(k, nWordLen))
	{
		return PKE_ZERO_ALL;
	}
	if(uint32_BigNumCmp(k, nWordLen, curve->eccp_n, nWordLen) >= 0)
	{
		return PKE_INTEGER_TOO_BIG;
	}

	if(curve == sm2_curve)
	{
		if((k[0] == sm2_curve->eccp_n[0]-1) && (0 == uint32_BigNumCmp(k+1, nWordLen-1, (curve->eccp_n)+1, nWordLen-1)))
		{
			return PKE_INTEGER_TOO_BIG;
		}
	}

	//get pubKey
	ret = eccp_pointMul(curve, k, curve->eccp_Gx, curve->eccp_Gy, x, y);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

	reverse_byte_array((u8 *)x, pubKey, pByteLen);
	reverse_byte_array((u8 *)y, pubKey+pByteLen, pByteLen);

	return PKE_SUCCESS;
}


/* function: get ECCP key pair(the key pair could be used in SM2/ECDSA/ECDH)
 * parameters:
 *     curve ---------------------- input, eccp_curve_t struct pointer
 *     priKey --------------------- output, private key, big-endian
 *     pubKey --------------------- output, public key, big-endian
 * return: PKE_SUCCESS(success), other(error)
 * caution:
 *     1. please make sure bit length of the curve is not bigger than 521(ECC521)
 */
u8 eccp_getkey(eccp_curve_t *curve, u8 *priKey, u8 *pubKey)
{
	u32 tmpLen;
	u32 nByteLen = GET_BYTE_LEN(curve->eccp_n_bitLen);
	u8 ret;

ECCP_GETKEY_LOOP:

	ret = get_rand(priKey, nByteLen);
	if(TRNG_SUCCESS != ret)
	{
		return ret;
	}

	//make sure k has the same bit length as n
	tmpLen = (curve->eccp_n_bitLen)&7;
	if(tmpLen)
	{
		priKey[0] &= (1<<(tmpLen))-1;
	}

	ret = eccp_get_pubkey_from_prikey(curve, priKey, pubKey);
	if(PKE_ZERO_ALL == ret || PKE_INTEGER_TOO_BIG == ret)
	{
		goto ECCP_GETKEY_LOOP;
	}

	return ret;
}

/****************************** ECCp functions finished ********************************/

static int ts_pke_clk_enable(struct udevice *dev)
{
	int ret;
	struct clk clk;


#if CONFIG_IS_ENABLED(CLK)
	ret = clk_get_by_name(dev, "pke_clk", &clk);
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

static int ts_pke_probe(struct udevice *dev)
{
	int ret;
	void __iomem *base;

	base = dev_remap_addr_index(dev, 0);
	if (!base)
		return -ENODEV;

	ts_pke_priv_t *ts_pke_priv = &s_ts_pke_instance;
	ts_pke_priv->regs = base;
	g_ts_pke = ts_pke_priv;

	ret = ts_pke_clk_enable(dev);
	if (ret) {
		dev_err(dev, "failed to enable clock %d\n", ret);
		return ret;
	}

	return 0;
}

static const struct udevice_id ts_pke_match[] = {
	{ .compatible = "tsingmicro,ts-pke" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(ts_pke) = {
	.name = "ts_pke",
	.id = UCLASS_MISC,
	.of_match = ts_pke_match,
	.probe = ts_pke_probe,
};
