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
#include "ts_pke_prime.h"

extern void pke_load_operand(u32 *baseaddr, u32 *data, u32 wordLen);
extern void pke_set_operand_width(u32 bitLen);
extern u32 pke_get_operand_bytes(void);
extern void pke_clear_interrupt(void);
extern void pke_set_microcode(u32 addr);
extern void pke_start(void);
extern u8 pke_check_rt_code(void);
extern void pke_wait_till_done(void);


//small prime number table
const u16 PrimeTable[PTL_MAX] =
{
	3,      5,      7,      11,     13,     17,     19,     23,     29,     31,
	37,     41,     43,     47,     53,     59,     61,     67,     71,     73,
	79,     83,     89,     97,     101,    103,    107,    109,    113,    127,
	131,    137,    139,    149,    151,    157,    163,    167,    173,    179,
	181,    191,    193,    197,    199,    211,    223,    227,    229,    233,    //50
	239,    241,    251,    257,    263,    269,    271,    277,    281,    283,
	293,    307,    311,    313,    317,    331,    337,    347,    349,    353,
	359,    367,    373,    379,    383,    389,    397,    401,    409,    419,
	421,    431,    433,    439,    443,    449,    457,    461,    463,    467,
	479,    487,    491,    499,    503,    509,    521,    523,    541,    547,    //100
	557,    563,    569,    571,    577,    587,    593,    599,    601,    607,
	613,    617,    619,    631,    641,    643,    647,    653,    659,    661,
	673,    677,    683,    691,    701,    709,    719,    727,    733,    739,
	743,    751,    757,    761,    769,    773,    787,    797,    809,    811,
	821,    823,    827,    829,    839,    853,    857,    859,    863,    877,    //150
	881,    883,    887,    907,    911,    919,    929,    937,    941,    947,
	953,    967,    971,    977,    983,    991,    997,    1009,   1013,   1019,
	1021,   1031,   1033,   1039,   1049,   1051,   1061,   1063,   1069,   1087,
	1091,   1093,   1097,   1103,   1109,   1117,   1123,   1129,   1151,   1153,
	1163,   1171,   1181,   1187,   1193,   1201,   1213,   1217,   1223,   1229,   //200
	1231,   1237,   1249,   1259,   1277,   1279,   1283,   1289,   1291,   1297,
	1301,   1303,   1307,   1319,   1321,   1327,   1361,   1367,   1373,   1381,
	1399,   1409,   1423,   1427,   1429,   1433,   1439,   1447,   1451,   1453,
	1459,   1471,   1481,   1483,   1487,   1489,   1493,   1499,   1511,   1523,
	1531,   1543,   1549,   1553,   1559,   1567,   1571,   1579,   1583,   1597,   //250
	1601,   1607,   1609,   1613,   1619,   1621,   1627,   1637,   1657,   1663,
	1667,   1669,   1693,   1697,   1699,   1709,   1721,   1723,   1733,   1741,
	1747,   1753,   1759,   1777,   1783,   1787,   1789,   1801,   1811,   1823,
	1831,   1847,   1861,   1867,   1871,   1873,   1877,   1879,   1889,   1901,
	1907,   1913,   1931,   1933,   1949,   1951,   1973,   1979,   1987,   1993,   //300
	1997,   1999,   2003,   2011,   2017,   2027,   2029,   2039,   2053,   2063,
	2069,   2081,   2083,   2087,   2089,   2099,   2111,   2113,   2129,   2131,
	2137,   2141,   2143,   2153,   2161,   2179,   2203,   2207,   2213,   2221,
	2237,   2239,   2243,   2251,   2267,   2269,   2273,   2281,   2287,   2293,
	2297,   2309,   2311,   2333,   2339,   2341,   2347,   2351,   2357,   2371,   //350
	2377,   2381,   2383,   2389,   2393,   2399,   2411,   2417,   2423,   2437,
	2441,   2447,   2459,   2467,   2473,   2477,   2503,   2521,   2531,   2539,
	2543,   2549,   2551,   2557,   2579,   2591,   2593,   2609,   2617,   2621,
	2633,   2647,   2657,   2659,   2663,   2671,   2677,   2683,   2687,   2689,
};

//0xFFFFFFFF/prime
const u32 PrimeTable_s[PTL_MAX] =
{
	0x55555555,0x33333333,0x24924924,0x1745d174,0x13b13b13,0x0f0f0f0f,0x0d79435e,0x0b21642c,0x08d3dcb0,0x08421084,
	0x06eb3e45,0x063e7063,0x05f417d0,0x0572620a,0x04d4873e,0x0456c797,0x04325c53,0x03d22635,0x039b0ad1,0x0381c0e0,
	0x033d91d2,0x03159721,0x02e05c0b,0x02a3a0fd,0x0288df0c,0x027c4597,0x02647c69,0x02593f69,0x0243f6f0,0x02040810,
	0x01f44659,0x01de5d6e,0x01d77b65,0x01b7d6c3,0x01b20364,0x01a16d3f,0x01920fb4,0x01886e5f,0x017ad220,0x016e1f76,
	0x016a13cd,0x01571ed3,0x01539094,0x014cab88,0x0149539e,0x013698df,0x0125e227,0x0120b470,0x011e2ef3,0x01194538,//50
	0x0112358e,0x010fef01,0x0105197f,0x00ff00ff,0x00f92fb2,0x00f3a0d5,0x00f1d48b,0x00ec9791,0x00e93965,0x00e79372,
	0x00dfac1f,0x00d578e9,0x00d2ba08,0x00d16154,0x00cebcf8,0x00c5fe74,0x00c27806,0x00bcdd53,0x00bbc840,0x00b9a786,
	0x00b68d31,0x00b2927c,0x00afb321,0x00aceb0f,0x00ab1cbd,0x00a87917,0x00a513fd,0x00a36e71,0x00a03c16,0x009c6916,
	0x009baade,0x00980e41,0x00975a75,0x009548e4,0x0093efd1,0x0091f5bc,0x008f67a1,0x008e2917,0x008d8be3,0x008c5584,
	0x0088d180,0x00869222,0x0085797b,0x008355ac,0x00824a4e,0x0080c121,0x007dc9f3,0x007d4ece,0x0079237d,0x0077cf53,//100
	0x0075a8ac,0x007467ac,0x00732d70,0x0072c62a,0x007194a1,0x006fa549,0x006e8419,0x006d68b5,0x006d0b80,0x006bf790,
	0x006ae907,0x006a3799,0x0069dfbd,0x0067dc4c,0x00663d80,0x0065ec17,0x00654ac8,0x00645c85,0x00637299,0x00632591,
	0x006160ff,0x0060cdb5,0x005ff401,0x005ed79e,0x005d7d42,0x005c6f35,0x005b2618,0x005a2553,0x0059686c,0x0058ae97,
	0x0058345f,0x005743d5,0x005692c4,0x00561e46,0x005538ed,0x0054c807,0x005345ef,0x00523a75,0x00510237,0x0050cf12,
	0x004fd319,0x004fa170,0x004f3ed6,0x004f0de5,0x004e1cae,0x004cd47b,0x004c78ae,0x004c4b19,0x004bf093,0x004aba3c,//150
	0x004a6360,0x004a383e,0x0049e28f,0x0048417b,0x0047f043,0x00474ff2,0x00468b6f,0x0045f13f,0x0045a522,0x0045342c,
	0x0044c4a2,0x0043c5c2,0x00437e49,0x0043142d,0x0042ab5c,0x00422195,0x0041bbb2,0x0040f391,0x0040b1e9,0x00405064,
	0x00403024,0x003f90c2,0x003f7141,0x003f1377,0x003e7988,0x003e5b19,0x003dc4a5,0x003da6e4,0x003d4e4f,0x003c4a6b,
	0x003c11d5,0x003bf5b1,0x003bbdb9,0x003b6a88,0x003b183c,0x003aabe3,0x003a5ba3,0x003a0c3e,0x0038f035,0x0038d6ec,
	0x003859cf,0x0037f741,0x00377df0,0x00373622,0x0036ef0c,0x0036915f,0x0036072c,0x0035d9b7,0x00359615,0x0035531c,//200
	0x00353cee,0x0034fad3,0x00347884,0x00340dd3,0x003351fd,0x00333d72,0x0033148d,0x0032d7ae,0x0032c385,0x00328766,
	0x00325fa1,0x00324bd6,0x0032246e,0x0031afa5,0x00319c63,0x003162f7,0x0030271f,0x002ff104,0x002fbb62,0x002f7499,
	0x002ed84a,0x002e832d,0x002e0e08,0x002decfb,0x002ddc87,0x002dbbc1,0x002d8af0,0x002d4a7b,0x002d2a85,0x002d1a9a,
	0x002ceb1e,0x002c8d50,0x002c404d,0x002c3106,0x002c1297,0x002c0370,0x002be540,0x002bb845,0x002b5f62,0x002b07e6,
	0x002ace56,0x002a791d,0x002a4eff,0x002a3319,0x002a0986,0x0029d295,0x0029b752,0x00298137,0x0029665e,0x00290975,//250
	0x0028ef35,0x0028c815,0x0028bb1b,0x0028a13f,0x00287ab3,0x00286dea,0x002847bf,0x002808c1,0x00278d0e,0x00276886,
	0x00275051,0x00274441,0x0026b5c1,0x00269e65,0x002692c2,0x002658fa,0x00261487,0x00260936,0x0025d106,0x0025a48a,
	0x00258371,0x00256292,0x002541ed,0x0024e150,0x0024c18b,0x0024ac7b,0x0024a1fc,0x00246380,0x0024300f,0x0023f314,
	0x0023cade,0x00237b7e,0x00233729,0x00231a30,0x002306fa,0x0022fd67,0x0022ea50,0x0022e0cc,0x0022b188,0x00227977,
	0x00225db3,0x0022421b,0x0021f05b,0x0021e75d,0x0021a01d,0x0021974a,0x00213767,0x00211d9f,0x0020fb7d,0x0020e212,//300
	0x0020d135,0x0020c8cd,0x0020b80b,0x002096b9,0x00207de7,0x002054de,0x00204cb6,0x00202428,0x001fec0c,0x001fc46f,
	0x001facda,0x001f7e17,0x001f765a,0x001f66ea,0x001f5f38,0x001f38f4,0x001f0b85,0x001f03ff,0x001ec853,0x001ec0ee,
	0x001eaad3,0x001e9c28,0x001e94d8,0x001e707b,0x001e53a2,0x001e1380,0x001dbf9f,0x001db1d1,0x001d9d35,0x001d81e6,
	0x001d4bdf,0x001d452c,0x001d37cf,0x001d1d3a,0x001ce89f,0x001ce219,0x001cd516,0x001cbb33,0x001ca7e7,0x001c94b5,
	0x001c87f7,0x001c6202,0x001c5bb8,0x001c1743,0x001c04d0,0x001bfeb0,0x001bec5d,0x001be034,0x001bce09,0x001ba402,//350
	0x001b9225,0x001b864a,0x001b8060,0x001b6eb1,0x001b62f4,0x001b516b,0x001b2e9c,0x001b1d56,0x001b0c26,0x001ae45f,
	0x001ad917,0x001ac83d,0x001aa6c7,0x001a90a7,0x001a8027,0x001a7533,0x001a2ed7,0x0019fefc,0x0019e4b0,0x0019cfcd,
	0x0019c569,0x0019b5e1,0x0019b0b8,0x0019a149,0x00196951,0x00194b30,0x00194631,0x00191e84,0x00190adb,0x00190113,
	0x0018e3e6,0x0018c233,0x0018aa58,0x0018a598,0x00189c1e,0x0018893f,0x00187b2b,0x00186d27,0x001863d8,0x00185f33,
	0x001855ef,0x00184816,0x001835b7,0x00182c92,0x00182802,0x00181a5c,0x001803c0,0x0017ff40,0x0017e8d6,0x0017d706,//400
};

//(0xFFFFFFFF%prime)+1
const u16 PrimeTable_r[PTL_MAX] =
{
	1,     1,     4,     4,     9,     1,     6,     12,    16,    4,
	7,     37,    16,    42,    42,    51,    57,    33,    9,     32,
	50,    77,    45,    35,    68,    63,    29,    75,    16,    16,
	117,   34,    41,    129,   4,     93,    100,   7,     96,    126,
	15,    147,   108,   88,    46,    51,    7,     176,   161,   8,     //50
	110,   15,    123,   1,     34,    47,    219,   27,    35,    250,
	133,   149,   72,    76,    232,   4,     26,    127,   192,   58,
	73,    60,    235,   203,   317,   13,    167,   255,   218,   254,
	234,   145,   27,    260,   341,   324,   407,   405,   115,   52,
	384,   338,   279,   444,   190,   355,   117,   294,   215,   423,   //100
	452,   188,   528,   82,    287,   413,   535,   125,   128,   400,
	573,   63,    513,   172,   640,   571,   136,   191,   37,    155,
	417,   87,    341,   134,   582,   567,   664,   331,   708,   539,
	71,    549,   620,   490,   19,    733,   579,   447,   49,    506,
	211,   240,   686,   367,   446,   553,   386,   797,   115,   116,   //150
	672,   550,   647,   311,   403,   578,   561,   105,   518,   316,
	238,   50,    285,   67,    444,   53,    966,   383,   259,   500,
	108,   690,   183,   7,     440,   93,    39,    836,   29,    939,
	321,   843,   575,   8,     1044,  649,   1015,  658,   437,   788,
	155,   429,   976,   90,    276,   337,   1156,  265,   429,   660,   //200
	910,   625,   1020,  847,   1271,  882,   345,   1250,  73,    1082,
	715,   454,   614,   1245,  1317,  423,   1073,  932,   870,   675,
	922,   1363,  392,   1247,  621,   1191,  1264,  707,   41,    1006,
	1030,  336,   651,   574,   1255,  400,   448,   1017,  1170,  686,
	942,   565,   781,   1367,  246,   501,   970,   451,   190,   287,   //250
	1419,  1069,  845,   1549,  1527,  1358,  1307,  1499,  98,    390,
	141,   1083,  675,   1147,  634,   782,   113,   398,   610,   382,
	989,   1598,  1165,  944,   227,   359,   500,   128,   1507,  1172,
	1582,  1518,  755,   1008,  730,   361,   880,   1708,  888,   1877,
	919,   1085,  407,   1735,  823,   778,   813,   987,   1225,  478,   //300
	1423,  1853,  495,   189,   1785,  1590,  386,   1384,  964,   1407,
	542,   1801,  434,   602,   8,     1892,  581,   1089,  1469,  726,
	1189,  1400,  984,   1421,  1406,  1408,  699,   1841,  1239,  1938,
	1117,  1068,  1363,  770,   2043,  2155,  1962,  1685,  1879,  1735,
	2241,  1014,  1528,  617,   1936,  144,   1889,  628,   1827,  378,   //350
	1651,  446,   608,   1595,  1324,  1611,  2252,  1802,  1110,  933,
	945,   237,   1667,  1707,  1857,  393,   2015,  100,   2032,  2513,
	505,   1707,  1656,  2523,  1277,  1328,  2479,  188,   1853,  121,
	1898,  683,   1192,  1592,  1006,  1967,  1881,  2115,  2008,  2381,
	1237,  14,    491,   482,   718,   2268,  1600,  64,    1202,  1170,  //400
};

u8 bigint_div_table_high(u32 *a, u32 aWordLen, u16 *r, double_u32 *s,
							  u16 *high_result, u32 PTL)
{
	s32 j;
	u32 i;
	uint64_t carry, a1, a2, high1;

	for(i=0; i<PTL; i++)
	{
		carry = 0;
		for (j = aWordLen - 1; j >= 0; j--)
		{
			a1=carry*r[i]+a[j];
			while(a1>0xFFFFFFFF)
			{
				a2=a1&0xFFFFFFFF;
				high1=a1>>32;
				a1=high1*r[i]+a2;
			}
			a2=(a1*s[i])>>32;
			carry=a1-a2*PrimeTable[i];
			if(carry>=PrimeTable[i])
			{
				carry -= PrimeTable[i];
			}
		}
		high_result[i]=(u16)carry;
	}

	return PKE_SUCCESS;
}

u8 bigint_div_table_low(u32 *a, u16 *r, double_u32 *s, u16 *high_result, u32 PTL)
{
	u32 i;
	uint64_t carry, a1, a2, high1;

	for(i=0; i<PTL; i++)
	{
		carry = high_result[i];
		{
			a1=carry*r[i]+a[0];
			while(a1>0xFFFFFFFF)
			{
				a2=a1&0xFFFFFFFF;
				high1=a1>>32;
				a1=high1*r[i]+a2;
			}
			a2=(a1*s[i])>>32;
			carry=a1-a2*PrimeTable[i];
			if(carry>=PrimeTable[i])
			{
				carry -= PrimeTable[i];
			}
		}
		if(0 == (u32)carry)
		{
			return NOT_PRIME;
		}
	}

	return MAYBE_PRIME;
}

/* function: for given odd number, according to Fermat Method, test whether it is prime or not
 * parameters:
 *     p ---------------- pointer to u32 big odd number p
 *     pWordLen --------- word length of p
 *     round ------------ round of test
 * return:
 *     0 ---------------- p is prime number with high probability
 *     0xFF ------------- p is composite number
 *     other ------------ error
 * caution:
 *     1. make sure p is odd, and pWordLen>1
 *     2. make sure round>0
 */
u8 primality_test_Fermat(u32 *p, u32 pWordLen, u32 round)
{
	u32 tmp_step, i, tag;
	u8 ret;

	pke_set_operand_width(pWordLen<<5);
	tmp_step = pke_get_operand_bytes();

	for(i=0; i<round; i++)
	{
		//A1, exponent, make it to be (p-1)
		uint32_copy((u32 *)(PKE_A(1,tmp_step)), p, pWordLen);
		*((u32 *)(PKE_A(1,tmp_step))) -= 1;

		//B0, base, make it to be in [2, p-2]
		uint32_clear((u32 *)(PKE_B(0,tmp_step)), pWordLen);

GET_RAND_BASE:

		ret = get_rand((u8 *)&tag, 4);
		if(TRNG_SUCCESS != ret)
		{
			return ret;
		}

		if(tag < 2)
		{
			goto GET_RAND_BASE;
		}

		*((u32 *)(PKE_B(0,tmp_step))) = tag;
		uint32_clear((u32 *)(PKE_B(0,tmp_step))+pWordLen-1, (tmp_step/4)-pWordLen+1);

		//get pre-calculated mont paras
		ret = pke_pre_calc_mont(p, pWordLen, NULL, NULL);
		if(PKE_SUCCESS != ret)
		{
			return ret;
		}

		//A0, base^d mod p
		ret = pke_modexp((u32 *)(PKE_B(3,tmp_step)), (u32 *)(PKE_A(1,tmp_step)), (u32 *)(PKE_B(0,tmp_step)),
				(u32 *)(PKE_A(0,tmp_step)), pWordLen, pWordLen);
		if(PKE_SUCCESS != ret)
		{
			return ret;
		}

		//if the result is 1 mod p, then p is probablly prime
		if(Bigint_Check_1((u32 *)(PKE_A(0,tmp_step)), pWordLen))
		{
			continue;
		}
		else
		{
			return NOT_PRIME;
		}
	}

	return MAYBE_PRIME;
}

/* function: get prime number of pBitLen
 * parameters:
 *     p -------------------------- pointer to u32 big prime number
 *     pBitLen -------------------- bit length of p
 * return:
 *     0 -------------------------- success, p is prime number with high probability
 *     other ---------------------- error
 * caution:
 *     1. pBitLen must be bigger than 32, but less than 2048
 */
u8 get_prime(u32* p, u32 pBitLen)
{
	u32 ret;
	u32 pWordLen = (pBitLen+0x1F)>>5;
	u16 high_result[PTL_MAX];
	u32 PTL;

	if(pBitLen <= 512)
	{
		PTL = PTL_512;
	}
	else if(pBitLen >= 1024)
	{
		PTL = PTL_1024;
	}
	else
	{
		PTL = ((PTL_1024-PTL_512)/(1024-512))*(pBitLen-512)+PTL_512;
	}

	if(PTL > PTL_MAX)
	{
		PTL = PTL_MAX;
	}

	ret = get_rand((u8 *)p, pWordLen<<2);
	if(TRNG_SUCCESS != ret)
	{
		return ret;
	}

	//make high two bit all 1
	pBitLen &= 0x1F;
	switch(pBitLen)
	{
	case 0:
		p[pWordLen - 1] |= 0xC0000000;
		break;

	case 1:
		p[pWordLen - 1]  = 1;
		p[pWordLen - 2] |= 0x80000000;
		break;

	default:
		p[pWordLen-1] &= ((1<<pBitLen)-1);
		p[pWordLen-1] |= (0x3<<(pBitLen-2));
		break;
	}

	//make p odd
	p[0] |= 0x01;

	ret = bigint_div_table_high(p+1, pWordLen-1, (u16 *)PrimeTable_r, (double_u32 *)PrimeTable_s, high_result, PTL);
	if(PKE_SUCCESS != ret)
	{
		return ret;
	}

ADD_2:
	p[0] += 2;
	ret = bigint_div_table_low(p, (u16 *)PrimeTable_r, (double_u32 *)PrimeTable_s, high_result, PTL);
	if(NOT_PRIME == ret)
	{
		goto ADD_2;
	}
	else if(MAYBE_PRIME != ret)
	{
		return ret;
	}

	ret = primality_test_Fermat(p, pWordLen, FERMAT_ROUND);


	if(NOT_PRIME == ret)
	{
		goto ADD_2;
	}

	return ret;
}

