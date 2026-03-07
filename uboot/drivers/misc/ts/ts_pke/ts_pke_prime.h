#ifndef _TS_PKE_PRIME_H_
#define _TS_PKE_PRIME_H_

#ifdef __cplusplus
extern "C" {
#endif



//1:use hardware;  2:use software
#define BIGINT_DIV_CHOICE     (2)

typedef u32 double_u32;


//1:use Fermat primality test;  2:use Miller�CRabin primality test
#define PRIMALITY_TEST_CHOICE (1)

#define FERMAT_ROUND          (3)

//prime table level(total number of small prime numbers)
#define PTL_MAX               (400)   //the max PTL value
#define PTL_512               (400)   //the best PTL value for prime bit length 512 (RSA1024)
#define PTL_1024              (400)   //the best PTL value for prime bit length 1024 (RSA2048)

#define NOT_PRIME             (0xFF)
#define MAYBE_PRIME           (0)

u8 get_prime(u32* p, u32 pBitLen);


#ifdef __cplusplus
}
#endif

#endif
