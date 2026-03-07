#include <assert.h>
#include <bootrom_wrapper.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/pem.h>

#define SM4_BLOCK_SIZE (16)
#define SM4_KEY_SIZE (16)
#define SM2_KEY_SIZE (64)
#define SM2_SIG_SIZE SM2_KEY_SIZE
#define SM2_ID_LEN_LIMIT (0xffffUL / 8)
#define SM3_DGST_SIZE (256 / 8)

const bootup_mode bootup_list[1];
uint8_t g_page_data[MTDT_MAX_PAGE_SIZE];
uint8_t __internal_sram[CONFIG_BOOTUP_COPY_FLASH_SIZE];
void* const SOC_INTERNAL_SRAM_BASEADDR = __internal_sram;
uint8_t g_secure_header[256];

extern void* load_file(const char* file, unsigned long* size);
extern void str2bin(void* bin, char* str);

static mtdt_info_t default_mtd;

int mtdt_find_device(mtdt_info_t** mtd, int type, int id)
{
    *mtd = &default_mtd;
    return SUCCESS;
}

static uint32_t dummy_otp[16] = {
    [14] = 0x0,
};

uint8_t otp_key[SM4_KEY_SIZE];

static int32_t flash_read(struct mtdt_info* mtd, uint32_t offset,
    uint32_t len, uint32_t* retlen, uint8_t* buf)
{
    assert(mtd == &default_mtd);
    assert(offset % mtd->writesize == 0);
    assert(len % mtd->writesize == 0);
    memcpy(buf, ((char*)mtd->flash_image) + offset, len);
    *retlen = len;
    return SUCCESS;
}

void bootrom_wrapper_init(opt_t* opt)
{
    assert(opt->pub_key_hash[0]);

    uint8_t* expected_pubkey_hash = (void*)(dummy_otp + 6);

    unsigned long pubkey_hash_file_len;
    void* pubkey_hash_file_data;

    /* set publie key hash */
    pubkey_hash_file_data = load_file(opt->pub_key_hash, &pubkey_hash_file_len);
    assert(pubkey_hash_file_data);
    assert(pubkey_hash_file_len >= SM3_DGST_SIZE * 2);
    ((char*)pubkey_hash_file_data)[SM3_DGST_SIZE * 2] = 0;
    str2bin(expected_pubkey_hash, pubkey_hash_file_data);

    /* set otp key */
    assert(strlen(opt->otp_key) == SM4_KEY_SIZE * 2);
    str2bin(otp_key, opt->otp_key);

    /* set secure_boot_flag
      0x00 for none secure boot
      0x01 for secure boot,encrypt and signature
      0x02 for secure boot,signature only */
    uint8_t secure_boot_flag = 0x01;
    *(uint32_t*)(dummy_otp + 14) &= (~(0x03 << 10));
    *(uint32_t*)(dummy_otp + 14) |= (secure_boot_flag << 10);

    /* load flash image */
    unsigned long flash_image_len;
    void* flash_image = load_file(opt->input, &flash_image_len);
    assert(flash_image);

    default_mtd.flash_image = flash_image;
    default_mtd.writesize = 512;
    default_mtd.read = flash_read;
}

int otp_read_word(unsigned long offset, uint32_t* data)
{
    assert(offset < sizeof(dummy_otp) / 4);
    *data = dummy_otp[offset];
    return 0;
}

int otp_key_select(unsigned int key)
{
    return 0;
}

uint32_t ske_hp_ecb_crypto(SKE_ALG alg, SKE_CRYPTO crypto, uint8_t* key,
    uint8_t sp_key_idx, uint8_t* in, uint8_t* out, uint32_t bytes)
{
    assert(alg == SKE_ALG_SM4);
    assert(crypto == SKE_CRYPTO_DECRYPT);

    EVP_CIPHER_CTX* ctx;

    ctx = EVP_CIPHER_CTX_new();
    assert(ctx);
    if (key)
        assert(EVP_DecryptInit_ex(ctx, EVP_sm4_ecb(), NULL, key, NULL));
    else
        assert(EVP_DecryptInit_ex(ctx, EVP_sm4_ecb(), NULL, otp_key, NULL));

    /* set no padding */
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    int inlen = bytes;
    int outlen = bytes;
    assert(EVP_DecryptUpdate(ctx, out, &outlen, in, inlen));
    assert(inlen == outlen);
    assert(EVP_DecryptFinal_ex(ctx, out + outlen, &outlen));
    assert(outlen == 0);
    EVP_CIPHER_CTX_free(ctx);
    return SKE_SUCCESS;
}

extern void hexdump_string(void* data, unsigned long len);

uint32_t ske_hp_cbc_crypto(SKE_ALG alg, SKE_CRYPTO crypto, uint8_t* key, uint8_t sp_key_idx,
    uint8_t* iv, uint8_t* in, uint8_t* out, uint32_t bytes)
{
    assert(alg == SKE_ALG_SM4);
    assert(crypto == SKE_CRYPTO_DECRYPT);

    EVP_CIPHER_CTX* ctx;

    ctx = EVP_CIPHER_CTX_new();
    assert(ctx);
    if (key)
        assert(EVP_DecryptInit_ex(ctx, EVP_sm4_cbc(), NULL, key, iv));
    else
        assert(EVP_DecryptInit_ex(ctx, EVP_sm4_cbc(), NULL, otp_key, iv));

    /* set no padding */
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    uint8_t* real_out = malloc(bytes);
    assert(real_out);

    int inlen = bytes;
    int outlen = bytes;
    assert(EVP_DecryptUpdate(ctx, real_out, &outlen, in, inlen));
    assert(inlen == outlen);
    assert(EVP_DecryptFinal_ex(ctx, real_out + outlen, &outlen));
    assert(outlen == 0);
    EVP_CIPHER_CTX_free(ctx);
    memcpy(out, real_out, bytes);
    return SKE_SUCCESS;
}

uint32_t sm3(uint8_t* msg, uint32_t msg_bytes, uint8_t* digest)
{
    unsigned int digest_len;
    digest_len = SM3_DGST_SIZE;
    assert(EVP_Digest(msg, msg_bytes,
        digest, &digest_len,
        EVP_sm3(), NULL));
    return HASH_SUCCESS;
}

static uint8_t user_id[8192];
static unsigned long user_id_len;
static uint8_t* boot_img;
static unsigned long boot_img_len;

uint8_t sm2_getZ(uint8_t* ID, uint32_t byteLenofID, uint8_t pubKey[65], uint8_t Z[32])
{
    assert(byteLenofID <= sizeof(user_id));
    if (byteLenofID)
        memcpy(user_id, ID, byteLenofID);
    user_id_len = byteLenofID;
    return SM2_SUCCESS;
}

uint8_t sm2_getE(uint8_t* M, uint32_t byteLen, uint8_t Z[32], uint8_t E[32])
{
    boot_img = M;
    boot_img_len = byteLen;
    return SM2_SUCCESS;
}

uint8_t sm2_verify(uint8_t E[32], uint8_t pubKey[65], uint8_t signature[64])
{
    /* load ec key */
    EC_KEY* ec_key;
    ec_key = EC_KEY_new_by_curve_name(NID_sm2);
    assert(ec_key);

    assert(pubKey[0] == 0x04);
    assert(EC_KEY_oct2key(ec_key, pubKey, SM2_KEY_SIZE + 1, NULL));

    EVP_PKEY* pkey;
    pkey = EVP_PKEY_new();
    assert(pkey);
    assert(EVP_PKEY_set1_EC_KEY(pkey, ec_key));
    assert(EVP_PKEY_set_alias_type(pkey, EVP_PKEY_SM2));

    EVP_MD_CTX* mctx = EVP_MD_CTX_create();
    assert(mctx);

    char* default_user_id = "1234567812345678";
    unsigned long default_user_id_len = strlen(default_user_id);

    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new(pkey, NULL);

    if (user_id_len)
        assert(EVP_PKEY_CTX_set1_id(pctx, user_id, user_id_len) > 0);
    else
        assert(EVP_PKEY_CTX_set1_id(pctx, default_user_id, default_user_id_len) > 0);

    EVP_MD_CTX_set_pkey_ctx(mctx, pctx);

    assert(EVP_DigestVerifyInit(mctx, NULL, EVP_sm3(), NULL, pkey));
    assert(EVP_DigestVerifyUpdate(mctx, boot_img, boot_img_len));

    ECDSA_SIG* ec_sig = ECDSA_SIG_new();
    assert(ec_sig);
    BIGNUM *r, *s;
    r = BN_bin2bn(signature, 32, NULL);
    assert(r);
    s = BN_bin2bn(signature + 32, 32, NULL);
    assert(s);
    assert(ECDSA_SIG_set0(ec_sig, r, s));

    unsigned char* sig_der = NULL;
    unsigned long sig_der_len;
    sig_der_len = i2d_ECDSA_SIG(ec_sig, &sig_der);
    assert(sig_der_len > 0);

    int err;
    err = EVP_DigestVerifyFinal(mctx, sig_der, sig_der_len);

    return err == 1 ? SM2_SUCCESS : -1;
}
