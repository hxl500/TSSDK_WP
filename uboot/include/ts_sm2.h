#ifndef _TS_SM2_H_
#define _TS_SM2_H_

#ifdef __cplusplus
extern "C" {
#endif

//some sm2 length
#define SM2_BIT_LEN               (256)
#define SM2_BYTE_LEN              (32)
#define SM2_WORD_LEN              (8)
#define SM3_DIGEST_BYTE_LEN       SM2_BYTE_LEN
#define SM2_MAX_ID_BYTE_LEN       (1<<13)

#define POINT_NOT_COMPRESSED      (0x04)

//SM2 error code
enum SM2_RET_CODE
{
	SM2_SUCCESS = 0,
	SM2_BUFFER_NULL = 0x40,
	SM2_NOT_ON_CURVE,
	SM2_EXCHANGE_ROLE_INVALID,
	SM2_INPUT_INVALID,
	SM2_ZERO_ALL,
	SM2_INTEGER_TOO_BIG,
	SM2_VERIFY_FAILED,
	SM2_IN_OUT_SAME_BUFFER,
	SM2_DECRYPT_VERIFY_FAILED
};


//SM2 key exchange role
typedef enum {
	SM2_Role_Sponsor = 0,
	SM2_Role_Responsor
} sm2_exchange_role_e;

// SM2 ciphertext order
typedef enum {
	SM2_C1C3C2   = 0,
    SM2_C1C2C3,
} sm2_cipher_order_e;


//APIs

u8 sm2_getZ(u8 *ID, u32 byteLenofID, u8 pubKey[65], u8 Z[32]);

u8 sm2_getE(u8 *M, u32 byteLen, u8 Z[32], u8 E[32]);

u8 sm2_get_pubkey_from_prikey(u8 priKey[32], u8 pubKey[65]);

u8 sm2_getkey(u8 priKey[32], u8 pubKey[65]);

u8 sm2_sign(u8 E[32], u8 rand_k[32], u8 priKey[32], u8 signature[64]);

u8 sm2_verify(u8 E[32], u8 pubKey[65], u8 signature[64]);

u8 sm2_encrypt(u8 *M, u32 MByteLen, u8 rand_k[32], u8 pubKey[65],
					sm2_cipher_order_e order, u8 *C, u32 *CByteLen);

u8 sm2_decrypt(u8 *C, u32 CByteLen, u8 priKey[32],
					sm2_cipher_order_e order, u8 *M, u32 *MByteLen);

u8 sm2_exchangekey(sm2_exchange_role_e role,
						u8 *dA, u8 *PB,
						u8 *rA, u8 *RA,
						u8 *RB,
						u8 *ZA, u8 *ZB,
						u32 kByteLen,
						u8 *KA, u8 *S1, u8 *SA);


#ifdef __cplusplus
}
#endif

#endif
