#ifndef _TS_PKE_H_
#define _TS_PKE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ts_eccp_curve.h"

#define OSR_PKE_CTRL        0x00
#define OSR_PKE_CFG         0x04
#define OSR_PKE_MC_PTR      0x10
#define OSR_PKE_STAT        0x20
#define OSR_PKE_RT_CODE     0x24
#define OSR_PKE_RAN_SEED    0x40
#define OSR_PKE_EXE_CONF    0x50
#define OSR_PKE_VERSION     0x80
#define OSR_PKE_A			0x400
#define OSR_PKE_B			0x1000


//PKE offset
#define PKE_INT_ENABLE_OFFSET         (8)
#define PKE_START_CALC                (1)
#define PKE_EXE_ECCP_EXE_CFG          (0x0015)
#define PKE_EXE_CFG_ALL_NON_MONT      (0x0000)
#define PKE_EXE_CFG_MOD_EXP           (0x0316)

//PKE microcode
#define MICROCODE_PDBL             0x04
#define MICROCODE_PADD             0x08
#define MICROCODE_PVER             0x0C
#define MICROCODE_PMUL             0x10
#define MICROCODE_MODEXP           0x14
#define MICROCODE_MODMUL           0x18
#define MICROCODE_MODINV           0x1C
#define MICROCODE_MODADD           0x20
#define MICROCODE_MODSUB           0x24
#define MICROCODE_MGMR_PRE         0x28
#define MICROCODE_INTMUL           0x34

#define ECCP_MAX_BIT_LEN       (521) //ECC521
#define ECCP_MAX_BYTE_LEN      (GET_BYTE_LEN(ECCP_MAX_BIT_LEN))
#define ECCP_MAX_WORD_LEN      (GET_WORD_LEN(ECCP_MAX_BIT_LEN))

#define OPERAND_MAX_WORD_LEN   (64)

//PKE return code
enum PKE_RET_CODE
{
	PKE_SUCCESS = 0,
	PKE_STOP,
	PKE_NO_MODINV,
	PKE_NOT_ON_CURVE,
	PKE_INVALID_MC,
	PKE_ZERO_ALL,                   //for ECCP input check
	PKE_INTEGER_TOO_BIG,            //for ECCP input check
};


//define KDF
typedef void *(*KDF_FUNC)(const void *input, u32 byteLen, u8 *key, u32 keyByteLen);

//APIs

u8 pke_modinv(const u32 *modulus, const u32 *a, u32 *ainv, u32 modWordLen,
				   u32 aWordLen);

u8 pke_modadd(const u32 *modulus, const u32 *a, const u32 *b,
				   u32 *out, u32 wordLen);

u8 pke_modsub(const u32 *modulus, const u32 *a, const u32 *b,
				   u32 *out, u32 wordLen);

u8 pke_add(const u32 *a, const u32 *b, u32 *out, u32 wordLen);

u8 pke_sub(const u32 *a, const u32 *b, u32 *out, u32 wordLen);

u8 pke_mul(const u32 *a, const u32 *b, u32 *out, u32 ab_wordLen);

u8 pke_pre_calc_mont(const u32 *modulus, u32 wordLen, u32 *H, u32 *n1);

u8 pke_pre_calc_mont_no_output(const u32 *modulus, u32 wordLen);

void pke_load_pre_calc_mont(u32 *H, u32 *n1, u32 wordLen);

u8 pke_modmul_internal(const u32 *modulus, const u32 *a, const u32 *b, u32 *out,
							u32 wordLen);

u8 pke_modmul(const u32 *modulus, const u32 *a, const u32 *b, u32 *out, u32 wordLen);

u8 pke_modexp(const u32 *modulus, const u32 *exponent, const u32 *base,
				   u32 *out, u32 mod_wordLen, u32 exp_wordLen);

u8 pke_mod(u32 *a, u32 aWordLen, u32 *b, u32 bWordLen, u32 *c);

u8 eccp_pointMul(eccp_curve_t *curve, u32 *k, u32 *Px, u32 *Py,
					  u32 *Qx, u32 *Qy);

u8 eccp_pointAdd(eccp_curve_t *curve, u32 *P1x, u32 *P1y, u32 *P2x, u32 *P2y,
					  u32 *Qx, u32 *Qy);

u8 eccp_pointVerify(eccp_curve_t *curve, u32 *Px, u32 *Py);

u8 eccp_get_pubkey_from_prikey(eccp_curve_t *curve, u8 *priKey, u8 *pubKey);

u8 eccp_getkey(eccp_curve_t *curve, u8 *priKey, u8 *pubKey);

void pke_disable_interrupt(void);
void pke_set_exe_cfg(u32 cfg);
void pke_enable_interrupt(void);

volatile u32 *PKE_A(int a, int step);
volatile u32 *PKE_B(int a, int step);
u32 pke_get_operand_bytes(void);
void pke_set_operand_width(u32 bitLen);


#ifdef __cplusplus
}
#endif

#endif
