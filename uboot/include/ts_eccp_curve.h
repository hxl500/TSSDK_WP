#ifndef _TS_ECCP_CURVE_H_
#define _TS_ECCP_CURVE_H_

#ifdef __cplusplus
extern "C" {
#endif

// eccp curve struct
typedef struct
{
	u32 eccp_p_bitLen;		  //bit length of prime p
	u32 eccp_n_bitLen;		  //bit length of order n
	u32 *eccp_p;
	u32 *eccp_p_h;
	u32 *eccp_p_n1;
	u32 *eccp_a;
	u32 *eccp_b;
	u32 *eccp_Gx;
	u32 *eccp_Gy;
	u32 *eccp_n;
	u32 *eccp_n_h;
	u32 *eccp_n_n1;
} eccp_curve_t;

extern eccp_curve_t sm2_curve[1];
extern u32 sm2p256v1_n_1[8];

#ifdef __cplusplus
}
#endif

#endif
