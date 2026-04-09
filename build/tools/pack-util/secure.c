#include <bootrom_wrapper.h>

#define THIS_MODULE MODULE_SECURE

#define SM4_BLOCK_SIZE (16)
#define SM4_KEY_SIZE (16)
#define SM2_KEY_SIZE (64)
#define SM2_SIG_SIZE SM2_KEY_SIZE
#define SM2_ID_LEN_LIMIT (0xffffUL / 8)
#define SM3_DGST_SIZE (256 / 8)

#define SECURE_ASSERT(exp) \
    do {                   \
        if (!(exp)) {      \
            LOG_E();       \
            while (1)      \
                ;          \
        }                  \
    } while (0)

#define ROUND_UP(x, align) (((x) + ((align)-1)) / (align) * (align))
#define ROUND_DOWN(x, align) ((x) / (align) * (align))

extern const bootup_mode bootup_list[];
extern uint8_t g_page_data[MTDT_MAX_PAGE_SIZE];

/*
 * secure header is encrypted with user root key or soc root key
 * encrypto algrithm SM4 ECB.
 */
struct secure_header {
    uint8_t iv[SM4_BLOCK_SIZE]; /* iv for payload(user id and image) cbc encryption */
    uint8_t pubkey[SM2_KEY_SIZE]; /* sm2 public key */
    uint8_t sig[SM2_SIG_SIZE]; /* sm2 signature */
    uint32_t id_off; /* sm2 user id offset */
    uint32_t id_len; /* sm2 user id length */
    uint32_t img_off; /* boot image offset */
    uint32_t img_len; /* boot image length */
    uint32_t magic; /* magic number */
    uint32_t padding[3]; /* padding to 16 bytes aligned */
} __attribute__((packed));

#ifdef __BOOTROM_WRAPPER_H__
#define STORE_SECURE_HEADER(header)                                      \
    do {                                                                 \
        assert(sizeof(struct secure_header) <= sizeof(g_secure_header)); \
        memcpy(g_secure_header, header, sizeof(struct secure_header));   \
    } while (0)

#else
#warn "not on simulation environment"
#endif

void static inline load_root_key(void)
{
    uint32_t tmp;
    unsigned int key;

    SECURE_ASSERT(otp_read_word(14, &tmp) == 0);

    /* bit8 and bit9 */
    /* if bit8 or bit9 has been changed before */
    key = (tmp & (3 << 8)) ? OTP_KEY_CHIP_ROOT_KEY : OTP_KEY_USER_ROOT_KEY;
    SECURE_ASSERT(otp_key_select(key) == 0);
}

/* decrypt secure header with ECB mode */
static int decrypt_secure_header(struct secure_header* header, void* cipher)
{
    /* set key to NULL means use key from secure port */
    /* we have just one key, so sp_key_idx should be ignored */
    if (SKE_SUCCESS != ske_hp_ecb_crypto(SKE_ALG_SM4, SKE_CRYPTO_DECRYPT, NULL, 0, cipher, (uint8_t*)header, sizeof(*header))) {
        LOG_E_S("decrypt secure header failed");
        return -1;
    }
    return 0;
}

extern uint8_t otp_key[SM4_KEY_SIZE];
/* decrypt payload with CBC mode */
/* payload include boot image and user id */
static int decrypt_payload(void* plaintext, void* cipher,
    unsigned long len, struct secure_header* header)
{
    /*
     * key select is done by load_rook_key
     * passing key to NULL means, use secure port key instead of a given key from flash
     * sp_key_index will be ignored
     */
    if (SKE_SUCCESS != ske_hp_cbc_crypto(SKE_ALG_SM4, SKE_CRYPTO_DECRYPT, otp_key, 0, header->iv, cipher, plaintext, len)) {
        LOG_E_S("decrypt payload failed");
        return -1;
    }
    return 0;
}

static inline void get_expected_pubkey_hash(void* hash)
{
    /* get expected publie key hash from eFuse */
    uint32_t otp_hash[SM3_DGST_SIZE / 4];
    int i;

    for (i = 0; i < SM3_DGST_SIZE / 4; ++i)
        otp_read_word(6 + i, otp_hash + i);

    /* little endian */
    memcpy(hash, otp_hash, SM3_DGST_SIZE);
}

/*
 * @brief judge if we are in secure boot mode
 * @return true if secure boot, false none secure boot
 */
int is_secure_boot(void)
{
    uint32_t tmp;

    SECURE_ASSERT(otp_read_word(14, &tmp) == 0);
    /* bit10 and bit11 */
    // return (tmp & (3 << 10)) ? 1 : 0;
    return (tmp >> 10) & 0x03;
}

int check_version(uint8_t version)
{
    uint32_t tmp;
    uint8_t expected_version;

    otp_read_word(14, &tmp);
    /* bit0 to bit7 is version */
    expected_version = tmp & 0xff;
    /* unti-rollback */
    if (version < expected_version)
        return 0;
    return 1;
}

static inline int calculate_pubkey_hash(void* hash, void* key)
{
    if (sm3(key, SM2_KEY_SIZE, hash) != HASH_SUCCESS) {
        LOG_E_S("sm3 hash for public key failed");
        return -1;
    }

    return 0;
}

static int load(mtdt_info_t* mtd, void* mem, unsigned long off, unsigned long len)
{
    int32_t err;
    uint32_t read_len;
    if (off % mtd->writesize == 0) {
        /* page aligned */
        unsigned long align_len = ROUND_UP(len, mtd->writesize);
        err = mtd->read(mtd, off, align_len, &read_len, mem);
        /* we get enough data */
        return (err == SUCCESS && read_len == align_len) ? 0 : -1;
    }

    /* handle none aligned offset */

    unsigned long off_unalign_down = off % mtd->writesize;
    unsigned long off_unalign_up = mtd->writesize - off_unalign_down;
    unsigned long off_align = off - off_unalign_down;

    err = mtd->read(mtd, off_align, mtd->writesize, &read_len, g_page_data);
    if (err != SUCCESS || read_len != mtd->writesize)
        return FAILURE;

    unsigned long copy_len = 0;
    unsigned long single_copy_len;

    single_copy_len = len > off_unalign_up ? off_unalign_up : len;

    memcpy(mem, g_page_data + off_unalign_down, single_copy_len);
    copy_len += single_copy_len;

    single_copy_len = ROUND_DOWN(len - copy_len, mtd->writesize);
    if (single_copy_len) {
        err = mtd->read(mtd, off_align + mtd->writesize, single_copy_len, &read_len, mem + copy_len);
        if (err != SUCCESS || read_len != single_copy_len)
            return FAILURE;
        copy_len += single_copy_len;
    }

    single_copy_len = len - copy_len;
    off_align = ROUND_DOWN((off + len), mtd->writesize);
    if (single_copy_len) {
        err = mtd->read(mtd, off_align, mtd->writesize, &read_len, g_page_data);
        if (err != SUCCESS || read_len != mtd->writesize)
            return FAILURE;
    }
    memcpy(mem + copy_len, g_page_data, single_copy_len);

    return SUCCESS;
}

int32_t secure_copy_flashdata_to_sram(int32_t bootup_id)
{
    /* make sure, secure header size is multiple of cipher block size */
    SECURE_ASSERT(sizeof(struct secure_header) % SM4_BLOCK_SIZE == 0);

    uint32_t read_len;
    int32_t err;
    mtdt_info_t* mtd;

    uint8_t* sram = (uint8_t*)(SOC_INTERNAL_SRAM_BASEADDR);

    err = mtdt_find_device(
        &mtd,
        bootup_list[bootup_id].m_type,
        bootup_list[bootup_id].m_id);

    if (err != SUCCESS) {
        LOG_E_S("error find flash"); /* can't find mtd device */
        return FAILURE;
    }

    unsigned long required_read_len;
    if (mtd->writesize >= sizeof(struct secure_header))
        required_read_len = mtd->writesize;
    else
        required_read_len = ROUND_UP(sizeof(struct secure_header), mtd->writesize);

    mtd->read(mtd, 0, required_read_len, &read_len, g_page_data);
    if (err != SUCCESS || read_len != required_read_len) {
        LOG_E_S("try to read secure header failed");
        return FAILURE;
    }

    /* select a root key, ether user root key or soc root key */
    load_root_key();

    struct secure_header secure_header;
    if (1 == is_secure_boot()) {
        if (decrypt_secure_header(&secure_header, g_page_data)) {
            LOG_E_S("decrypt secure header failed");
            return FAILURE;
        }
    } else {
        memcpy(&secure_header, g_page_data, sizeof(secure_header));
    }

    STORE_SECURE_HEADER(&secure_header);

    if (secure_header.magic != CONFIG_MAGIC) {
        LOG_E_S("magic number error");
        return FAILURE;
    }

    if (secure_header.img_len > CONFIG_BOOTUP_COPY_FLASH_SIZE) {
        LOG_E_S("img_len is over limit size");
        return FAILURE;
    }

    if (secure_header.id_len > SM2_ID_LEN_LIMIT) {
        LOG_E_S("id_len is over sm2 limitation");
        return FAILURE;
    }
    /* authentificate sm2 public key using sm3 */
    uint8_t pubkey_hash_expected[SM3_DGST_SIZE];
    uint8_t pubkey_hash_calculated[SM3_DGST_SIZE];
    /* get expected public key hash from eFuse */
    get_expected_pubkey_hash(pubkey_hash_expected);
    /* calculate current public key hash */
    if (calculate_pubkey_hash(pubkey_hash_calculated, secure_header.pubkey)) {
        LOG_E_S("calculated public key hash failed");
        return FAILURE;
    }

    if (memcmp(pubkey_hash_expected, pubkey_hash_calculated, SM3_DGST_SIZE)) {
        LOG_E_S("unexpected public key");
        return FAILURE;
    }

    /* authentificate image using sm2*/
    /********** sm2 get Z standard data test **********/
    uint8_t Z[SM3_DGST_SIZE], E[SM3_DGST_SIZE];

    /*
     * if id len is 0, use default one.
     */
    unsigned long aligned_len;
    void* user_id = NULL;
    if (secure_header.id_len) {
        aligned_len = ROUND_UP(secure_header.id_len, SM4_BLOCK_SIZE);
        /* load user id */
        if (load(mtd, sram, secure_header.id_off, aligned_len)) {
            LOG_E_S("load user id failed");
            return FAILURE;
        }

        if (1 == is_secure_boot()) /* decrypt user id */{
            if (decrypt_payload(sram, sram, aligned_len, &secure_header)) {
                LOG_E_S("decrypt user id failed");
                return FAILURE;
            }
        }
        user_id = sram;
    }

    uint8_t pubkey[SM2_KEY_SIZE + 1];
    pubkey[0] = 4;
    memcpy(pubkey + 1, secure_header.pubkey, SM2_KEY_SIZE);

    /*
     * if user_id is NULL not sram base, use default user id
     * which is defined in sm2 spec
     */
    err = sm2_getZ(user_id, secure_header.id_len, pubkey, Z);
    if (SM2_SUCCESS != err) {
        LOG_E_S("sm2 get Z parameter failed");
        return FAILURE;
    }

    /********** sm2 get E standard data test **********/

    aligned_len = ROUND_UP(secure_header.img_len, SM4_BLOCK_SIZE);
    /* load image */
    if (load(mtd, sram, secure_header.img_off, aligned_len)) {
        LOG_E_S("load boot image failed");
        return FAILURE;
    }

    if (1 == is_secure_boot()) /* decrypt img */ {
        if (decrypt_payload(sram, sram, aligned_len, &secure_header)) {
            LOG_E_S("decrypt boot image failed");
            return FAILURE;
        }
    }

    err = sm2_getE(sram, secure_header.img_len, Z, E);
    if (SM2_SUCCESS != err) {
        LOG_E_S("sm2 get E parameter failed");
        return FAILURE;
    }

    /* verify signature */
    err = sm2_verify(E, pubkey, secure_header.sig);
    if (SM2_SUCCESS != err) {
        LOG_E_S("sm2 verify signature failed");
        return FAILURE;
    }

    /* last byte of boot image is boot image version */
    uint8_t version = *(sram + (secure_header.img_len - 1));

    /* check boot image version */
    if (check_version(version) == 0) {
        LOG_E_S("check first loader version failed");
        return FAILURE;
    }

    return SUCCESS;
}

/*
 * @brief clean all sram data
 */
void secure_final(void)
{
    /* TODO: lock bootrom */
}
