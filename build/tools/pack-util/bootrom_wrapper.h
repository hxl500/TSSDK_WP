#ifndef __BOOTROM_WRAPPER_H__
#define __BOOTROM_WRAPPER_H__

#include <assert.h>
#include <opt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define CONFIG_MAGIC (0x4e5a5751)

#define MODULE_SECURE "secure"
#define LOG_E() printf("[%s, %d]\n", THIS_MODULE, __LINE__)
#define LOG_E_S(str) printf("[%s, %d] %s\n", THIS_MODULE, __LINE__, str)

#define CONFIG_BOOTUP_COPY_FLASH_SIZE (128 * 1024)
#define MTDT_MAX_PAGE_SIZE (4 * 1024)

extern uint8_t __internal_sram[CONFIG_BOOTUP_COPY_FLASH_SIZE];
extern void* const SOC_INTERNAL_SRAM_BASEADDR;
extern uint8_t g_secure_header[256];

enum {
    SUCCESS = 0,
    FAILURE = -1,
};

typedef struct mtdt_info {
    uint32_t writesize;
    int32_t (*read)(
        struct mtdt_info* mtdt,
        uint32_t offset,
        uint32_t len,
        uint32_t* retlen,
        uint8_t* buf);
    void* flash_image;
} mtdt_info_t;

typedef struct {
    int m_type;
    int m_id;
} bootup_mode;

enum {
    OTP_KEY_USER_ROOT_KEY = 0,
    OTP_KEY_CHIP_ROOT_KEY,
};

typedef struct {
    int alg;
} HASH_CTX;

#define SKE_SUCCESS 0
#define HASH_SUCCESS 0
#define SM2_SUCCESS 0

typedef enum {
    SKE_ALG_SM4,
} SKE_ALG;

typedef enum {
    SKE_CRYPTO_ENCRYPT,
    SKE_CRYPTO_DECRYPT,
} SKE_CRYPTO;

void bootrom_wrapper_init(opt_t* opt);
int mtdt_find_device(mtdt_info_t** mtd, int type, int id);
int otp_read_word(unsigned long offset, uint32_t* data);
int otp_key_select(unsigned int key);
uint32_t ske_hp_ecb_crypto(SKE_ALG alg, SKE_CRYPTO crypto, uint8_t* key,
    uint8_t sp_key_idx, uint8_t* in, uint8_t* out, uint32_t bytes);
uint32_t ske_hp_cbc_crypto(SKE_ALG alg, SKE_CRYPTO crypto, uint8_t* key,
    uint8_t sp_key_idx, uint8_t* iv, uint8_t* in, uint8_t* out, uint32_t bytes);
uint32_t sm3(uint8_t* msg, uint32_t msg_bytes, uint8_t* digest);
uint8_t sm2_getZ(uint8_t* ID, uint32_t byteLenofID,
    uint8_t pubKey[65], uint8_t Z[32]);
uint8_t sm2_getE(uint8_t* M, uint32_t byteLen, uint8_t Z[32], uint8_t E[32]);
uint8_t sm2_verify(uint8_t E[32], uint8_t pubKey[65], uint8_t signature[64]);

int32_t secure_copy_flashdata_to_sram(int32_t bootup_id);

#endif
