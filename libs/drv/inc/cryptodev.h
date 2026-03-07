#ifndef __CRYPTODEV_H__
#define __CRYPTODEV_H__

#include <linux/types.h>
#include <linux/version.h>

#ifndef __KERNEL__
#define __user
#endif

#define MAX_KEYLEN		160
#define MAX_IVLEN		32

/* All the supported algorithms
 */
enum ts_crypto_alg_type_e {
	TS_CRYPTO_HASH_SM3 = 0,
	TS_CRYPTO_HASH_SHA256,
	TS_CRYPTO_HASH_MD5,
	TS_CRYPTO_SKE_SM4_ECB,
	TS_CRYPTO_SKE_AES_CBC,
	TS_CRYPTO_SKE_AES_ECB = 5,
	TS_CRYPTO_SKE_3DEA_EDE_CFB,
	TS_CRYPTO_PKE_RSA_NO_CRT,
	TS_CRYPTO_PKE_SM2,
	TS_CRYPTO_SKE_AES_GCM,
	TS_CRYPTO_TRNG,
};

enum ts_crypto_op_type_e {
	TS_OP_DEC = 0,
	TS_OP_ENC,
};


struct ts_cipher_config {
	int handle;
	enum ts_crypto_alg_type_e algo_type;
	enum ts_crypto_op_type_e operation;
	unsigned char key[64];
	unsigned int key_len;
	unsigned char iv[16];
	unsigned int	data_len;	/* length of source data */
	unsigned char	__user *src;	/* source data */
	unsigned char	__user *dst;	/* pointer to output data */

	unsigned char	__user *aad;	/* pointer to aad data */
	unsigned int 	aad_len;
	unsigned char	__user *tag;	/* pointer to tag data */
	unsigned int 	tag_len;

	// enum ts_crypto_version_type_e type;

	unsigned int reserved[10];
};

struct ts_hash_config {
	int handle;
	enum ts_crypto_alg_type_e algo_type;
	unsigned int	data_len;		/* length of source data */
	unsigned char	__user *src;	/* source data */
	unsigned int 	digest_size;
	unsigned char	__user *dst;	/* pointer to output data */
	unsigned int reserved[10];
};


struct ts_trng_config {
	int handle;
	enum ts_crypto_alg_type_e algo_type;
	unsigned int	bytes_len;		/* length of bytes*/
	unsigned char	__user *dst;	/* pointer to output data */
	unsigned int reserved[10];
};


struct ts_rsa_pub_key {
	/* rsa */
	unsigned char	*n;
	unsigned char	*e;
	unsigned short	n_len;
	unsigned short	e_len;

	/* sm2 */
	unsigned char *key_buf;
	unsigned short	key_len;
};

struct ts_rsa_priv_key {
	/* rsa */
	unsigned char	*n;
	unsigned char	*d;
	unsigned short	n_len;
	unsigned short	d_len;

	/* crt rsa */
	unsigned char	*p;
	unsigned char	*q;
	unsigned char	*dp;
	unsigned char	*dq;
	unsigned char	*u;
	unsigned short	p_len;
	unsigned short	q_len;
	unsigned short	dp_len;
	unsigned short	dq_len;
	unsigned short	u_len;

	/* sm2 */
	unsigned char *key_buf;
	unsigned short	key_len;
};

struct ts_pke_config {
	int handle;
	enum ts_crypto_alg_type_e algo_type;
	enum ts_crypto_op_type_e operation;
	struct ts_rsa_priv_key priv_key;
	struct ts_rsa_pub_key pub_key;
	unsigned int data_len;		/* length of source data */
	unsigned char __user *src;	/* source data */
	unsigned char __user *dst;	/* pointer to output data */
	unsigned int dst_len;		/* length of dst data */
	unsigned int reserved[10];
};

/* ske ioctl's.
 */
#define TS_CRYPTO_IO_SKE_INIT			_IOWR('c', 10, struct ts_cipher_config)
#define TS_CRYPTO_IO_SKE_SET_KEY		_IOWR('c', 11, struct ts_cipher_config)
#define TS_CRYPTO_IO_SKE_SET_CONFIG		_IOWR('c', 12, struct ts_cipher_config)
#define TS_CRYPTO_IO_SKE_CRYPTO			_IOWR('c', 13, struct ts_cipher_config)
#define TS_CRYPTO_IO_SKE_CRYPTO2		_IOWR('c', 14, struct ts_cipher_config)

int ts_crypto_ske_init(void);
int ts_crypto_ske_set_algo_type(int handle,
					struct ts_cipher_config *cipher_cfg);
int ts_crypto_ske_key(int handle, struct ts_cipher_config *cipher_cfg);
int ts_crypto_ske_set_cfg(int handle,
				struct ts_cipher_config *cipher_cfg);
int ts_crypto_ske_encrypto(int handle,
				struct ts_cipher_config *cipher_cfg);
int ts_crypto_ske_encrypto2(int handle,
				struct ts_cipher_config *cipher_cfg);
int ts_crypto_ske_decrypto(int handle,
				struct ts_cipher_config *cipher_cfg);
int ts_crypto_ske_decrypto2(int handle,
				struct ts_cipher_config *cipher_cfg);
void ts_crypto_ske_deinit(int handle);

/* hash ioctl's.
 */
#define TS_CRYPTO_IO_HASH_INIT			_IOWR('c', 20, struct ts_hash_config)
#define TS_CRYPTO_IO_HASH_DIGEST		_IOWR('c', 21, struct ts_hash_config)

int ts_crypto_hash_init(void);
int ts_crypto_hash_set_algo_type(int handle,
					struct ts_hash_config *hash_cfg);
int ts_crypto_hash_digest(int handle,
					struct ts_hash_config *hash_cfg);
void ts_crypto_hash_deinit(int handle);


/* pke ioctl's.
 */
#define TS_CRYPTO_IO_PKE_INIT			_IOWR('c', 30, struct ts_pke_config)
#define TS_CRYPTO_IO_PKE_SET_PUB_KEY	_IOWR('c', 31, struct ts_pke_config)
#define TS_CRYPTO_IO_PKE_SET_PRI_KEY	_IOWR('c', 32, struct ts_pke_config)
#define TS_CRYPTO_IO_PKE_CRYPTO			_IOWR('c', 33, struct ts_pke_config)
#define TS_CRYPTO_IO_PKE_SET_CONFIG		_IOWR('c', 34, struct ts_pke_config)
#define TS_CRYPTO_IO_PKE_SIGN			_IOWR('c', 35, struct ts_pke_config)
#define TS_CRYPTO_IO_PKE_VERIFY			_IOWR('c', 36, struct ts_pke_config)
int ts_crypto_pke_init(void);
int ts_crypto_pke_set_algo_type(int handle,
					struct ts_pke_config *pke_cfg);
int ts_crypto_pke_set_priv_key(int handle,
					struct ts_pke_config *pke_cfg);
int ts_crypto_pke_set_pub_key(int handle,
					struct ts_pke_config *pke_cfg);
int ts_crypto_pke_set_cfg(int handle,
				struct ts_pke_config *pke_cfg);
int ts_crypto_pke_encrypto(int handle,
				struct ts_pke_config *pke_cfg);
int ts_crypto_pke_decrypto(int handle,
				struct ts_pke_config *pke_cfg);
int ts_crypto_pke_sign(int handle, struct ts_pke_config *pke_cfg);
int ts_crypto_pke_verity(int handle, struct ts_pke_config *pke_cfg);
void ts_crypto_pke_deinit(int handle);


/* trng ioctl's.
 */
#define TS_CRYPTO_IO_TRNG_INIT			_IOWR('c', 37, struct ts_trng_config)
#define TS_CRYPTO_IO_GET_BYTES			_IOWR('c', 38, struct ts_trng_config)
int ts_crypto_trng_init(void);
int ts_crypto_trng_set_algo_type(int handle,
					struct ts_trng_config *trng_cfg);
int ts_crypto_trng_get_bytes(int handle,
				struct ts_trng_config *trng_cfg);
void ts_crypto_trng_deinit(int handle);

#endif
