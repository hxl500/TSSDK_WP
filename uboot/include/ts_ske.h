#ifndef _TS_SKE__
#define _TS_SKE__

#ifdef __cplusplus
extern "C" {
#endif

#define AES_KEYSIZE_MAX_BYTE_LEN (32)

#define SKE_CTRL		0x000	/*(W1S) SKE Control Register */
#define SKE_CFG 		0x004	/*(R/W) SKE Config Register */
#define SKE_SR			0x008	/*(R)   SKE Status Register */
#define SKE_RISR		0x00C	/*(W0C) SKE Interrupt Source Register */
#define SKE_IMCR		0x010	/*(R/W) SKE Interrupt Enable Register */
#define SKE_MISR		0x014	/*(R)   SKE Interrupt Output Register */
#define SKE_SP			0x01C	/*(R)   SKE Secure Port Register */
#define SKE_KEY1		0x020	/*(W)   Key1 */
#define SKE_KEY1_OFFSET 0x004
#define SKE_KEY2		0x040	/*(W)   Key2 */
#define SKE_KEY2_OFFSET 0x004
#define SKE_A_LEN_L		0x060	/*(R/W) GCM mode AAD length low Register */
#define SKE_A_LEN_H		0x064	/*(R/W) GCM mode AAD length high Register */
#define SKE_C_LEN_L		0x068	/*(R/W) GCM mode plaintext/ciphertext length low Register */
#define SKE_C_LEN_H		0x06C	/*(R/W) GCM mode plaintext/ciphertext length high Register */
#define SKE_IV			0x070	/*(R/W) Initial Vector */
#define SKE_IV_OFFSET	0x4
#define SKE_IV_END		0x07C
#define SKE_DIN_CR		0x080	/*(R/W) SKE Input Register */
#define SKE_DIN 		0x090	/*(W)	SKE Input Register */
#define SKE_DIN_OFFSET	0x4
#define SKE_DIN_END 	0x09C
#define SKE_DOUT		0x0B0	/*(R)	SKE Output Register */
#define SKE_DOUT_OFFSET	0x4
#define SKE_DOUT_END	0x0BC
#define SKE_VERSION 	0x0FC	/*(R)	SKE version Register */
#define SKE_DMA_CR		0x300	/*(R/W)	DMA Config register */
#define SKE_DMA_SR		0x304	/*(W0C)	DMA Status register */
#define SKE_DMA_TO		0x308	/*(R/W)	DMA Timeout Threshold register */
#define SKE_DMA_SA_L	0x310	/*(R/W)	DMA Source Address Low part register */
#define SKE_DMA_SA_H	0x314	/*(R/W)	DMA Source Address High part register */
#define SKE_DMA_DA_L	0x320	/*(R/W)	DMA Destination Address Low part register */
#define SKE_DMA_DA_H	0x324	/*(R/W)	DMA Destination Address High part register */
#define SKE_DMA_RLEN	0x330	/*(R/W)	DMA read Length register */
#define SKE_DMA_WLEN	0x334	/*(R/W)	DMA write Length register */
#define SKE_DMA_AWCC	0x340	/*(R/W)	DMA AWCC register */
#define SKE_DMA_ARCC	0x344	/*(R/W)	DMA ARCC register */
#define SKE_DMA_LLP_L	0x348
#define SKE_DMA_LLP_H	0x34C

#define SKE_ENABLE	1
#define SKE_DISABLE	0

/* SKE_CTRL (0x000) */
#define SKE_CTRL_ENABLE_POS		(0)
#define SKE_CTRL_ENABLE_MASK	(0x1 << SKE_CTRL_ENABLE_POS)
enum ske_ctrl_calc_en {
	CALC_NO,
	CALC_YES,
};

#define SKE_CTRL_POP_DOUT_POS	(1)
#define SKE_CTRL_POP_DOUT_MASK	(0x1 << SKE_CTRL_POP_DOUT_POS)

enum ske_ctrl_pop_dout_en {
	POP_DOUT_NO,
	POP_DOUT_YES,
};

/* SKE_CFG  (0x004) */
#define SKE_CFG_MODE_POS		(28)
#define SKE_CFG_MODE_MASK		(0xF << SKE_CFG_MODE_POS)
enum ske_mode_type {
	BYPASS = 0,
	ECB,
	XTS_NORMAL,
	CBC,
	CFB,
	OFB = 5,
	CTR,
	CMAC,
	CBC_MAC,
	GCM,
};

#define SKE_CFG_DATA_TYPE_POS	(24)
#define SKE_CFG_DATA_TYPE_MASK	(0x3 << SKE_CFG_DATA_TYPE_POS)
enum ske_data_type {
	SKE_DATA_TYPE_NO_EXCHANGE,
	SKE_DATA_TYPE_HALF_WORD_EXCHANGE,
	SKE_DATA_TYPE_BYTE_EXCHANGE,
	SKE_DATA_TYPE_BIT_EXCHANGE,
};

#define SKE_CFG_LLEN_POS		(17)
#define SKE_CFG_LLEN_MASK		(0x1 << SKE_CFG_LLEN_POS)
enum ske_dma_linkedList_en {
	SKE_DMA_LL_DISABLE,
	SKE_DMA_LL_ENABLE,
};

#define SKE_CFG_DMAEN_POS		(16)
#define SKE_CFG_DMAEN_MASK		(0x1 << SKE_CFG_DMAEN_POS)
enum ske_work_mode {
	SKE_WORK_TYPE_CPU,
	SKE_WORK_TYPE_DMA,
};

#define SKE_CFG_UPDATE_CFG_POS	(12)
#define SKE_CFG_UPDATE_CFG_MASK	(0x1 << SKE_CFG_UPDATE_CFG_POS)
enum ske_update_flag {
	UPDATE_CFG_NO,
	UPDATE_CFG_YES,
};

#define SKE_CFG_DEC_TYPE_POS	(11)
#define SKE_CFG_DEC_TYPE_MASK	(0x1 << SKE_CFG_DEC_TYPE_POS)
enum ske_dec_type {
	AES_DIR_ENCRYPT,	/* encrypto */
	AES_DIR_DECRYPT, 	/* decrypto */
};

#define SKE_CFG_KEY2_SIZE_POS	(6)
#define SKE_CFG_KEY2_SIZE_MASK	(0x3 << SKE_CFG_KEY2_SIZE_POS)
#define SKE_CFG_KEY1_SIZE_POS	(4)
#define SKE_CFG_KEY1_SIZE_MASK	(0x3 << SKE_CFG_KEY1_SIZE_POS)

enum ske_key_size {
	SKE_KEY_SIZE_64_BIT,   /* 8 byte */
	SKE_KEY_SIZE_128_BIT,  /* 16 byte */
	SKE_KEY_SIZE_192_BIT,  /* 24 byte */
	SKE_KEY_SIZE_256_BIT,  /* 32 byte */
};

#define SKE_CFG_ALG_TYPE_POS	(0)
#define SKE_CFG_ALG_TYPE_MASK	(0xf << SKE_CFG_ALG_TYPE_POS)
enum ske_alg_type {
	SKE_TYPE_AES = 1,   /* key len has 128/192/256 bit */
	SKE_TYPE_SM4,		/* key len has 128 bit */
	SKE_TYPE_DES,
	SKE_TYPE_3DES_EDE,  /* key len has 128/192 bit */
	SKE_TYPE_3DES_EEE,  /* key len has 128/192/256 bit */
};

/* SKE_SR (0x008) */
#define SKE_SR_RFDO_POS		(17)
#define SKE_SR_RFDO_MASK	(0x1 << SKE_SR_RFDO_POS)
#define SKE_SR_RFDI_POS		(16)
#define SKE_SR_RFDI_MASK	(0x1 << SKE_SR_RFDI_POS)
#define SKE_SR_RFC_POS		(0)
#define SKE_SR_RFC_MASK		(0x1 << SKE_SR_RFC_POS)

/* SKE_RISR (0x00C) */
#define SKE_RISR_LL_POS		(2)
#define SKE_RISR_LL_MASK	(0x1 << SKE_RISR_LL_POS)
#define SKE_RISR_DMA_POS	(1)
#define SKE_RISR_DMA_MASK	(0x1 << SKE_RISR_DMA_POS)
#define SKE_RISR_CORE_POS	(0)
#define SKE_RISR_CORE_MASK	(0x1 << SKE_RISR_CORE_POS)

/* SKE_IMCR (0x0010) */
#define SKE_IMCR_LL_POS		(2)
#define SKE_IMCR_LL_MASK	(0x1 << SKE_IMCR_LL_POS)
#define SKE_IMCR_DMA_POS	(1)
#define SKE_IMCR_DMA_MASK	(0x1 << SKE_IMCR_DMA_POS)
#define SKE_IMCR_CORE_POS	(0)
#define SKE_IMCR_CORE_MASK	(0x1 << SKE_IMCR_CORE_POS)

/* SKE_MISR (0x0014) */
#define SKE_MISR_LL_POS		(2)
#define SKE_MISR_LL_MASK	(0x1 << SKE_MISR_LL_POS)
#define SKE_MISR_DMA_POS	(1)
#define SKE_MISR_DMA_MASK	(0x1 << SKE_MISR_DMA_POS)
#define SKE_MISR_CORE_POS	(0)
#define SKE_MISR_CORE_MASK	(0x1 << SKE_MISR_CORE_POS)

/* SKE_SP (0x001C) */
#define SKE_SP_EN_POS		(0)
#define SKE_SP_EN_MASK		(0x1 << SKE_SP_EN_POS)
enum ske_sp_en {
	SKE_SP_DISABLE,
	SKE_SP_ENABLE,
};

/* SKE_DIN_CR (0x080)*/
#define SKE_DIN_CR_LAST_POS		(16)
#define SKE_DIN_CR_LAST_MASK	(0x1 << SKE_DIN_CR_LAST_POS)
enum ske_last_block {
	SKE_WORK_NO_LAST,
	SKE_WORK_HAS_LAST,
};

#define SKE_DIN_CR_LEN_POS		(0)
#define SKE_DIN_CR_LEN_MASK		(0x1 << SKE_DIN_CR_LEN_POS)

/* SKE_DIN_CR (0x300)*/
#define SKE_DMA_CR_MOTDL_POS	(8)
#define SKE_DMA_CR_MOTDL_MASK	(0x3 << SKE_DMA_CR_MOTDL_POS)
enum ske_dma_read_max_len {
	SKE_DMA_READ_MAX_ONE,
	SKE_DMA_READ_MAX_TWO,
};

#define SKE_DMA_CR_MBL_POS		(0)
#define SKE_DMA_CR_MBL_MASK		(0xf << SKE_DMA_CR_MBL_POS)

struct ts_ske_aes_state {
	enum ske_alg_type alg_type;
	enum ske_mode_type alg_mode_type;
	enum ske_key_size key_size;
	enum ske_dec_type dec_type;

	u8 block_bytes;
	u8 block_words;

	u8 *src;
	u8 *dst;
	u32 len;

	enum ske_sp_en sp_en; /* secure_port_key enable*/
	u8 *key;
	u8 *iv;
};

u32 ts_ske_crypto(struct ts_ske_aes_state *mctx);

#ifdef __cplusplus
}
#endif
#endif

