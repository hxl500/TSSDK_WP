#include <assert.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include <bootrom_wrapper.h>
#include <log.h>
#include <opt.h>

#if _WIN32 || _WIN64
#define memalign(boundary, size) _aligned_malloc(size, boundary)
#endif

#if !(_WIN32 || _WIN64)
#define O_BINARY 0x0000
#endif

#define MAGIC (0x4e5a5751)

#ifndef SM4_BLOCK_SIZE
#define SM4_BLOCK_SIZE (16)
#endif

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

/*
 * secure header is encrypted with user root key or soc root key
 * encrypto algrithm SM4 ECB.
 */
struct secure_header {
    uint8_t iv[SM4_BLOCK_SIZE]; /* iv for payload(user id and image) ecb encryption */
    uint8_t pubkey[SM2_KEY_SIZE]; /* sm2 public key */
    uint8_t sig[SM2_SIG_SIZE]; /* sm2 signature */
    uint32_t id_off; /* sm2 user id offset */
    uint32_t id_len; /* sm2 user id length */
    uint32_t img_off; /* boot image offset */
    uint32_t img_len; /* boot image length */
    uint32_t magic; /* magic number */
    uint32_t padding[3]; /* padding to 16 bytes aligned */
} __attribute__((packed));

void* load_file(const char* file, unsigned long* size)
{
    int fd;
    int err = -1;
    struct stat stat;
    void* buf;

    fd = open(file, O_RDONLY | O_BINARY);
    if (fd < 0) {
        err("cannot load file %s\n", file);
        return NULL;
    }

    err = fstat(fd, &stat);
    if (err < 0) {
        err("cannot stat file %s\n", file);
        goto close_file;
    }
    *size = stat.st_size;

    /* we should append boot image version at the end of boot image, one byte long, so, just add one byte for malloc enough space */
    unsigned long malloc_size = ROUND_UP(stat.st_size + 1, SM4_BLOCK_SIZE);

    buf = malloc(malloc_size);
    /* padding with zero */
    memset(buf, 0x00, malloc_size);

    if (buf == NULL) {
        err("cannot malloc buffer for file %s\n", file);
        err = -1;
        goto close_file;
    }

    if (read(fd, buf, stat.st_size) != stat.st_size) {
        err("cannot load whole file %s\n", file);
        err = -1;
        goto close_file;
    }

    err = 0;
close_file:
    if (err)
        buf = NULL;
    close(fd);
    return buf;
}

int store_file(void* buf, unsigned long size, const char* file)
{
    int fd;
    int err = -1;

    fd = open(file, O_RDWR | O_CREAT | O_BINARY,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd < 0) {
        err("cannot open file %s\n", file);
        perror("");
        return -1;
    }

    if (write(fd, buf, size) != size) {
        err("cannot store hole file %s\n", file);
        err = -1;
        goto close_file;
    }
    err = 0;
close_file:
    close(fd);
    return err;
}

EVP_PKEY* load_key(const char* file)
{
    BIO* io = NULL;
    EVP_PKEY* key = NULL;

    io = BIO_new_file(file, "rb");
    assert(io);

    key = PEM_read_bio_PrivateKey(io, NULL, NULL, NULL);
    assert(key);
    BIO_free(io);
    return key;
}

void hexdump_string(void* data, unsigned long len)
{
    int i;
    for (i = 0; i < len; ++i)
        info("%02x", ((unsigned char*)data)[i]);
}

#define DUMP_HEX(name, value)                   \
    do {                                        \
        info(name ": ");                        \
        hexdump_string((value), sizeof(value)); \
        info("\n");                             \
    } while (0)

void dump_secure_header(struct secure_header* header)
{
    DUMP_HEX("sm4 cbc iv", header->iv);
    DUMP_HEX("sm2 ec public key", header->pubkey);
    DUMP_HEX("sm2 signature", header->sig);
    info("user id offset: 0x%08x\n", header->id_off);
    info("user id length: 0x%08x\n", header->id_len);
    info("boot image offset: 0x%08x\n", header->img_off);
    info("boot image length: 0x%08x\n", header->img_len);
    info("magic: 0x%08x\n", header->magic);
}

int encrypt(void* plaintext, unsigned long plaintext_len, void* key, void* iv, void* ciphertext)
{
    EVP_CIPHER_CTX* ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    ctx = EVP_CIPHER_CTX_new();
    assert(ctx);

    /*
	 * Initialise the encryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits
	 */
    if (iv)
        assert(EVP_EncryptInit_ex(ctx, EVP_sm4_cbc(), NULL, key, iv));
    else
        assert(EVP_EncryptInit_ex(ctx, EVP_sm4_ecb(), NULL, key, iv));

    /* set no padding */
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    /*
	 * Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
    assert(EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len));
    ciphertext_len = len;

    /*
	 * Finalise the encryption. Further ciphertext bytes may be written at
	 * this stage.
	 */
    assert(EVP_EncryptFinal_ex(ctx, ciphertext + len, &len));
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

void str2bin(void* bin, char* str)
{
    char tmp[3];
    unsigned int str_len = strlen(str);

    assert(str_len % 2 == 0);

    char* p;
    unsigned char* q;

    tmp[2] = 0;
    for (p = str, q = bin; *p; p += 2, ++q) {
        tmp[0] = *p;
        tmp[1] = *(p + 1);
        *q = (unsigned char)strtol(tmp, NULL, 16);
    }
}

/*big endian: msb-----lsb*/
void bin2str(char* str, void* bin, unsigned long len, int big_endian)
{
	char* p;
	int i;

	if(big_endian)
	{
		for (p = str, i = 0; i < len; ++i)
			p += snprintf(p, 3, "%02x", ((unsigned char*)bin)[i]);
	}
	else
	{
		for (p = str, i = len-1; i >= 0; --i)
			p += snprintf(p, 3, "%02x", ((unsigned char*)bin)[i]);
	}
}

void store_otp_info(opt_t* opt, void* pubkey_digest, void* otp_key)
{
    char otp_info[1024];
    char pubkey_digest_ascii[EVP_MAX_MD_SIZE * 2 + 1];
    char otp_key_ascii[SM4_KEY_SIZE * 2 + 1];
    char pubkey_digest_ascii_otp[EVP_MAX_MD_SIZE / 4 * 12 + 1];
    char otp_key_ascii_otp[SM4_KEY_SIZE / 4 * 12 + 1];

    bin2str(pubkey_digest_ascii, pubkey_digest, SM3_DGST_SIZE, 0);
    bin2str(otp_key_ascii, otp_key, SM4_KEY_SIZE, 1);

    int i;
    uint8_t* p;
    char* q;

    p = pubkey_digest;
    q = pubkey_digest_ascii_otp;
    /* little endian */
    for (i = 0; i < SM3_DGST_SIZE; i += 4) {
        uint32_t tmp;
        tmp = p[i] | (p[i + 1] << 8) | (p[i + 2] << 16) | (p[i + 3] << 24);
        q += sprintf(q, "0x%08x,\n", tmp);
    }

    p = otp_key;
    q = otp_key_ascii_otp;
#if 0
    /* little endian */
    for (i = 0; i < SM4_KEY_SIZE; i+=4) {
        uint32_t tmp;
        tmp = p[i] | (p[i + 1] << 8) | (p[i + 2] << 16) | (p[i + 3] << 24);
        q += sprintf(q, "%08x\n", tmp);
    }
#else
    for (i = SM4_KEY_SIZE - 4; i >= 0; i -= 4) {
        uint32_t tmp;
        tmp = p[i + 3] | (p[i + 2] << 8) | (p[i + 1] << 16) | (p[i] << 24);
        q += sprintf(q, "0x%08x,\n", tmp);
    }
#endif

    sprintf(otp_info, "\n//%s\n//%s\n\n//public key hash otp image\n%s\n//customer root key otp image\n%s",
		pubkey_digest_ascii,
		otp_key_ascii,
        pubkey_digest_ascii_otp,
        otp_key_ascii_otp);

    assert(store_file(otp_info, strlen(otp_info), opt->pub_key_hash) == 0);
}

int pack_secure(opt_t* opt)
{
    /* check options */
    if (!(opt->priv_key[0]
            // && opt->otp_key[0]
            && opt->pub_key_hash[0]
            && opt->output[0])) {
        err("-------------------------------------\n");
        err("invalid argument for secure mode\n");
        err("-------------------------------------\n");
        opt_help();
        return 2;
    }

    assert(opt->boot_image_version <= 0xff);
    /* end check options */

    /* get --private-key */
    char* priv_key_file = opt->priv_key;
    EVP_PKEY* priv_key;
    priv_key = load_key(priv_key_file);
    assert(priv_key);

    /* get --user-id user id */
    char* user_id_file = NULL;
    void* user_id;
    unsigned long user_id_len;

    if (opt->user_id[0]) {
        user_id_file = opt->user_id;
        user_id = load_file(user_id_file, &user_id_len);
        assert(user_id);
        if (user_id_len > 8192) {
            err("user id len > 8192\r\n");
            while(1);
        }
    } else {
        info("-------------------------------------\n");
        info("no user id was given, use SM2 default\n");
        info("-------------------------------------\n");
        user_id = "1234567812345678"; /* default uid,defined in the standard */
        user_id_len = strlen(user_id);
    }

    /* get --user-root-key */
    uint8_t user_root_key[SM4_KEY_SIZE];
    str2bin(user_root_key, opt->otp_key);

    /* get --iv */
    uint8_t iv[SM4_BLOCK_SIZE];
    /* get --iv witch will be stored in image using for SM4 CBC */
    str2bin(iv, opt->iv);

    /* get bootloader binary */
    char* boot_img_file = opt->input;
    void* boot_img;
    unsigned long boot_img_len;
    boot_img = load_file(boot_img_file, &boot_img_len);
    assert(boot_img);

    /* set secure header */
    struct secure_header header;
    memset(&header, 0x00, sizeof(header));

    /* set header magic num */
    header.magic = MAGIC;
    /* done */

    /* set PKE_PUB_KEY */
    EC_KEY* ec_key;
    ec_key = EVP_PKEY_get1_EC_KEY(priv_key);
    assert(ec_key);

    const EC_GROUP* ec_group;
    ec_group = EC_KEY_get0_group(ec_key);

    const EC_POINT* pub = EC_KEY_get0_public_key(ec_key);
    assert(pub);

    BIGNUM* bn_x = BN_new();
    BIGNUM* bn_y = BN_new();
    assert(EC_POINT_get_affine_coordinates_GFp(ec_group, pub, bn_x, bn_y, NULL));

    if (opt->verbose) {
        printf("ecc x coordinate: ");
        BN_print_fp(stdout, bn_x);
        putc('\n', stdout);
        printf("ecc y coordinate: ");
        BN_print_fp(stdout, bn_y);
        putc('\n', stdout);
    }

    unsigned char x[32], y[32];
    assert(sizeof(x) == BN_bn2binpad(bn_x, x, sizeof(x)));
    assert(sizeof(y) == BN_bn2binpad(bn_y, y, sizeof(y)));

#if 0
    printf("x coordinate: ");
    hexdump_string(x, sizeof(x));
    printf("\n");
    printf("y coordinate: ");
    hexdump_string(y, sizeof(y));
    printf("\n");
#endif

    memcpy(header.pubkey, x, sizeof(x));
    memcpy(header.pubkey + sizeof(x), y, sizeof(y));
    /* done */


    /* set USER_ID_OFF */
    header.id_off = sizeof(struct secure_header);
    /* done */

    /* set USER_ID_LEN */
    header.id_len = user_id_len;
    /* done */

    /* set IMG_OFF */
    header.img_off = header.id_off + ROUND_UP(header.id_len, SM4_BLOCK_SIZE);
    /* done */ 

    /* set IMG_LEN */
    /* append boot image version (one byte) at the end of boot image */
    *((uint8_t*)boot_img + boot_img_len) = opt->boot_image_version;
    boot_img_len += 1;
    /* set header user id offset,NOTE:boot_img_len contain 1Byte boot version */
    header.img_len = boot_img_len;
    /* done */

    /* set SIG */
    unsigned int pubkey_digest_len = EVP_MAX_MD_SIZE;
    unsigned char pubkey_digest[EVP_MAX_MD_SIZE] = { 0 };
    assert(EVP_Digest(header.pubkey, sizeof(header.pubkey),
        pubkey_digest, &pubkey_digest_len,
        EVP_sm3(), NULL));

    /* start sign */
    assert(EVP_PKEY_set_alias_type(priv_key, EVP_PKEY_SM2));

    EVP_MD_CTX* mctx = EVP_MD_CTX_create();
    assert(mctx);

    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new(priv_key, NULL);
    assert(EVP_PKEY_CTX_set1_id(pctx, user_id, user_id_len) > 0);
    EVP_MD_CTX_set_pkey_ctx(mctx, pctx);

    assert(EVP_DigestSignInit(mctx, NULL, EVP_sm3(), NULL, priv_key));
    assert(EVP_DigestSignUpdate(mctx, boot_img, boot_img_len));

    uint8_t sig_der[128];
    size_t sig_der_len = sizeof(sig_der);
    const unsigned char* sig_der_ptr = sig_der;

    assert(EVP_DigestSignFinal(mctx, sig_der, &sig_der_len));
    ECDSA_SIG* sig = d2i_ECDSA_SIG(NULL, &sig_der_ptr, sig_der_len);
    assert(sig);

    const BIGNUM* bn_r;
    const BIGNUM* bn_s;
    ECDSA_SIG_get0(sig, &bn_r, &bn_s);

    if (opt->verbose) {
        printf("dsa r: ");
        BN_print_fp(stdout, bn_r);
        putc('\n', stdout);
        printf("dsa s: ");
        BN_print_fp(stdout, bn_s);
        putc('\n', stdout);
    }

    unsigned char r[32], s[32];
    assert(sizeof(r) == BN_bn2binpad(bn_r, r, sizeof(r)));
    assert(sizeof(s) == BN_bn2binpad(bn_s, s, sizeof(s)));

#if 0
    printf("r coordinate: ");
    hexdump_string(r, sizeof(r));
    printf("\n");
    printf("s coordinate: ");
    hexdump_string(s, sizeof(s));
    printf("\n");
#endif

    /* set header sig */
    memcpy(header.sig, r, sizeof(r));
    memcpy(header.sig + sizeof(r), s, sizeof(s));
    /* done */

    /* encrypt */
    /* encrypt user id */
    unsigned long user_id_len_aligned = ROUND_UP(user_id_len, SM4_BLOCK_SIZE);
    void* user_id_cipher = NULL;
    unsigned long boot_img_len_aligned = ROUND_UP(boot_img_len, SM4_BLOCK_SIZE);
    void* boot_img_cipher = NULL;

    user_id_cipher = malloc(user_id_len_aligned);
    boot_img_cipher = malloc(boot_img_len_aligned);

    if (opt->otp_key[0]) {
        assert(strlen(opt->iv) == SM4_BLOCK_SIZE * 2);

        /* set SKE IV */
        uint8_t iv[SM4_BLOCK_SIZE];
        /* get --iv witch will be stored in image using for SM4 CBC */
        str2bin(iv, opt->iv);
        /* set header SKE IV */
        memcpy(header.iv, iv, sizeof(header.iv));
        /* done */

        /* encrypt user id */
        if (header.id_len) {
            assert(user_id_cipher);
            encrypt(user_id, user_id_len_aligned, user_root_key, iv, user_id_cipher);
        }

        /* encrypt user boot img */
        assert(boot_img_cipher);
        encrypt(boot_img, boot_img_len_aligned, user_root_key, iv, boot_img_cipher);
    }
    else{
        memcpy(user_id_cipher, user_id, user_id_len_aligned);
        memcpy(boot_img_cipher, boot_img, boot_img_len_aligned);
        dbg("----------------------------------------------------\r\n");
        dbg("no user root key,do not encrypt user id and boot img\r\n");
        dbg("----------------------------------------------------\r\n");
    }

    /* encrypt header */
    struct secure_header header_cipher;
    if (opt->otp_key[0]) {
        assert(sizeof(header) % SM4_BLOCK_SIZE == 0);
        encrypt(&header, sizeof(header), user_root_key, NULL, &header_cipher);
    } else {
        memcpy(&header_cipher, &header, sizeof(header));
        memset(user_root_key, 0, SM4_KEY_SIZE);
        dbg("--------------------------------------\r\n");
        dbg("no user root key,do not encrypt header\r\n");
        dbg("--------------------------------------\r\n");
    }

    unsigned long total_secure_img_len = header.img_off
        + ROUND_UP(header.img_len, SM4_BLOCK_SIZE);

    /* write to image */
    void* img = malloc(total_secure_img_len);
    assert(img);
    memset(img, 0x00, total_secure_img_len);
    /* write secure header */
    memcpy(img, &header_cipher, sizeof(struct secure_header));
    /* write user id */
    if (header.id_len) {
        memcpy(((char*)img) + header.id_off, user_id_cipher, user_id_len_aligned);
    } else {
        assert(header.img_off == sizeof(struct secure_header));
    }
    /* write boot image */
    memcpy(((char*)img) + header.img_off, boot_img_cipher, boot_img_len_aligned);

    assert(store_file(img, total_secure_img_len, opt->output) == 0);
    /* done */


    /* store the pubkey_digest and user root key in txt file for verify test */
    store_otp_info(opt, pubkey_digest, user_root_key);

    if (opt->verbose) {
        info("--------------------begin secure header----------------------\n");
        dump_secure_header(&header);
        info("---------------------end secure header-----------------------\n");
        DUMP_HEX("user root key", user_root_key);

        info("secure header length 0x%08lx\n",
            (long unsigned int)sizeof(struct secure_header));
        info("image length 0x%08lx\n", total_secure_img_len);
    }

    return 0;
}

int pack_none_secure(opt_t* opt)
{
    typedef uint32_t checksum_t;

    /* check options */
    if (opt->priv_key[0]
        || opt->otp_key[0]
        || opt->iv[0]
        || opt->user_id[0]
        || opt->pub_key_hash[0]) {

        err("-------------------------------------\n");
        err("invalid argument for none secure mode\n");
        err("-------------------------------------\n");
        opt_help();
        return 2;
    }

    if (opt->output[0] == 0) {
        err("missing output file\n");
        return 3;
    }

    unsigned long boot_img_len;
    void* boot_img;

    boot_img = load_file(opt->input, &boot_img_len);
    assert(boot_img);
    boot_img_len = ROUND_UP(boot_img_len, 4);

    /*
    * header:
    * | magic 4 bytes | size 4 bytes | 4 bytes chechsum| payload n bytes |
    */
    /*
     * size = magic + size + payload + checksum
    */

    unsigned long img_len = boot_img_len + 4 + 4 + sizeof(checksum_t);
    unsigned char* img = memalign(8, img_len);
    assert(img);

    uint32_t* magic = (uint32_t*)img;
    uint32_t* size = (uint32_t*)(img + 4);
    checksum_t* checksum = (checksum_t*)(img + 8);
    uint32_t* payload = (uint32_t*)(img + 12);

    *magic = MAGIC;

    /* intel pc is always little endian, and this filed is also little endian */
    *size = (uint32_t)img_len;
    memcpy(payload, boot_img, boot_img_len);

    *checksum = 0;

    uint32_t sum = 0;
    int i;
    for (i = 0; i < boot_img_len; ++i)
        sum += ((uint8_t*)boot_img)[i];

    *checksum = (checksum_t)sum;

    assert(store_file(img, img_len, opt->output) == 0);

    return 0;
}

int main(int argc, char* argv[])
{
    opt_t* opt;
    opt = opt_parse(argc, argv);
    if (opt == NULL) {
        err("invalid argument\n");
        return 1;
    }

    /* opt_info(opt); */

    if (opt->help || argc == 1) {
        opt_help();
        return 0;
    }

    if (opt->version) {
        info("\npack-util " VERSION "\n\n");
        return 0;
    }

    int err;

    if (opt->input[0] == 0) {
        err("boot image file is needed\n");
        return 3;
    }

    if (opt->verify) {
        bootrom_wrapper_init(opt);
        err = secure_copy_flashdata_to_sram(0);
        if (opt->verbose) {
            info("-------------------begin secure header-------------------\n");
            dump_secure_header((struct secure_header*)g_secure_header);
            info("--------------------end secure header--------------------\n");
        }
        info("flash image verify %s\n", err == 0 ? "success" : "failed");
    } else {
        /* if hash priv key then its secure boot */
        /* priv key used for SM2 in creating signature */
        if (opt->priv_key[0])
            err = pack_secure(opt);
        else
            err = pack_none_secure(opt);
    }

    return err;
}
