#ifndef _TS_RSA_H_
#define _TS_RSA_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RSA_BIT_LEN  (2048)
#define MIN_RSA_BIT_LEN  (512)
#define MAX_RSA_WORD_LEN (MAX_RSA_BIT_LEN>>5)

//RSA return code
enum RSA_RET_CODE
{
	RSA_SUCCESS,
	RSA_BUFFER_NULL = 0x30,
	RSA_INPUT_TOO_LONG,
	RSA_INPUT_INVALID,
};


//APIs

u8 RSA_ModExp(u32 *a, u32 *e, u32 *n, u32 *out, u32 eBitLen, u32 nBitLen);

u8 RSA_CRTModExp(u32 *a, u32 *p, u32 *q, u32 *dp, u32*dq,
					  u32 *u, u32 *out,  u32 nBitLen);

u8 RSA_GetKey(u32 *e, u32 *d, u32 *n, u32 eBitLen, u32 nBitLen);

u8 RSA_GetCRTKey(u32 *e, u32 *p, u32 *q, u32 *dp, u32 *dq, u32 *u,
					  u32 *n, u32 eBitLen, u32 nBitLen);



#ifdef __cplusplus
}
#endif

#endif
