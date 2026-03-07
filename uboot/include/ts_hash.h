#ifndef _TS_HASH__
#define _TS_HASH__

#ifdef __cplusplus
extern "C" {
#endif

#define HASH_CTRL	0x000	/*(R/W)  Control register */
#define HASH_CFG	0x004	/*((R/W) Config register */
#define HASH_RISR	0x010	/*((W0C) Resource of Interrupt Status register */
#define HASH_IMCR	0x014	/*((R/W) Interrupt Management and Control register */
#define HASH_MISR	0x018	/*((R)   Multiple  Interrupt Status register */
#define HASH_TX_LEN 0x020	/*((R/W) message length register */
#define HASH_MSG_LEN 0x030	/*((R/W) message total length register */
#define HASH_MSG_LEN_OFFSET 0x4
#define HASH_MSG_LEN_END 0x03C
#define HASH_IN		 0x040	 /*((W)    Hash iterator Input register */
#define HASH_IN_OFFSET 0x4
#define HASH_IN_END	 0x07C
#define HASH_OUT	 0x080	 /*((R)    Output register */
#define HASH_OUT_OFFSET  0x4
#define HASH_OUT_END	 0x0BC
#define HASH_M_DIN	 0x0C0	 /*((W)    Hash message Input register */
#define HASH_VERSION 0x0FC	 /*((R)    Version register */
#define DMA_CR		 0x300	 /*((R/W)  DMA Config register */
#define DMA_SR		 0x304	 /*((W0C)  DMA Status register */
#define DMA_TO		 0x308	 /*((R/W)  DMA Timeout Threshold register */
#define DMA_SA_L	 0x310	 /*((R/W)  DMA Source Address Low part register */
#define DMA_SA_H	 0x314	 /*((R/W)  DMA Source Address High part register */
#define DMA_DA_L	 0x320	 /*((R/W)  DMA Destination Address Low part register */
#define DMA_DA_H	 0x324	 /*((R/W)  DMA Destination Address High part register */
#define DMA_LEN		 0x330	 /*((R/W)  DMA Length register */
#define DMA_AWCC	 0x340	 /*((R/W)  DMA AWCC register */
#define DMA_ARCC	 0x344	 /*((R/W)  DMA ARCC register */

#define HASH_ENABLE	 1
#define HASH_DISABLE 0

/* HASH_CTRL (0x000) */
#define HASH_CTRL_ENABLE_POS (0)
#define HASH_CTRL_ENABLE_MASK	(0x1 << HASH_CTRL_ENABLE_POS)

/* HASH_CFG (0x004)*/
#define HASH_CFG_LAST_POS	(24)
#define HASH_CFG_LAST_MASK	(0x1 << HASH_CFG_LAST_POS)
enum hash_last_block {
	HASH_WORK_NO_LAST,
	HASH_WORK_HAS_LAST,
};

#define HASH_CFG_DMAEN_POS	(16)
#define HASH_CFG_DMAEN_MASK	(0x1 << HASH_CFG_DMAEN_POS)
enum hash_work_mode {
	HASH_WORK_TYPE_CPU,
	HASH_WORK_TYPE_DMA,
};

#define HASH_CFG_DATA_TYPE_POS	(8)
#define HASH_CFG_DATA_TYPE_MASK (0x3 << HASH_CFG_DATA_TYPE_POS)
enum data_type {
	DATA_TYPE_NO_EXCHANGE,
	DATA_TYPE_HALF_WORD_EXCHANGE,
	DATA_TYPE_BYTE_EXCHANGE,
	DATA_TYPE_BIT_EXCHANGE,
};

#define HASH_CFG_MSEL_POS	(0)
#define HASH_CFG_MSEL_MASK	(0xf << HASH_CFG_MSEL_POS)
enum hash_alg_type {
	HASH_TYPE_SM3,
	HASH_TYPE_MD5,
	HASH_TYPE_SHA256,
	HASH_TYPE_SHA384,
	HASH_TYPE_SHA512,
	HASH_TYPE_SHA1,
	HASH_TYPE_SHA224,
	HASH_TYPE_SHA512_224,
	HASH_TYPE_SHA512_256,
};

/* HASH_CFG (0x014)*/
#define HASH_IMCR_DMA_IRQ_POS	(1)
#define HASH_IMCR_DMA_IRQ_MASK	(0x1 << HASH_IMCR_DMA_IRQ_POS)
#define HASH_IMCR_CPU_IRQ_POS	(0)
#define HASH_IMCR_CPU_IRQ_MASK	(0x1 << HASH_IMCR_CPU_IRQ_POS)


/* HASH_IN (0x040) */
#define HASH_MD5_ITERATOR_WORD_LEN			4
#define HASH_SHA1_ITERATOR_WORD_LEN			5
#define HASH_SM3_ITERATOR_WORD_LEN			8
#define HASH_SHA256_ITERATOR_WORD_LEN		8
#define HASH_SHA224_ITERATOR_WORD_LEN		8
#define HASH_SHA384_ITERATOR_WORD_LEN		8
#define HASH_SHA512_ITERATOR_WORD_LEN		8
#define HASH_SHA512_224_ITERATOR_WORD_LEN	8
#define HASH_SHA512_256_ITERATOR_WORD_LEN	8

//HASH max length
#define HASH_DIGEST_MAX_WORD_LEN	(16)
#define HASH_BLOCK_MAX_WORD_LEN		(32)

#define HASH_ITERATOR_MAX_WORD_LEN	HASH_DIGEST_MAX_WORD_LEN
#define HASH_BLOCK_MAX_BYTE_LEN		(HASH_BLOCK_MAX_WORD_LEN<<2)

#define CONFIG_HASH_SUPPORT_MUL_THREAD

//HASH return code
enum HASH_RET_CODE
{
	TS_HASH_SUCCESS = 0,
	TS_HASH_BUFFER_NULL,
	TS_HASH_CONFIG_INVALID,
	TS_HASH_INPUT_INVALID,
	TS_HASH_LEN_OVERFLOW,
	TS_HASH_ERROR,
};

struct ts_hash_state {
	enum hash_alg_type hash_type;
	u8 first_update_flag;
	u8 finish_flag;
	u8 block_byte_len;
	u8 iterator_word_len;
	u8 digest_byte_len;
	u8 hash_buffer[HASH_BLOCK_MAX_BYTE_LEN];
	u32 total[HASH_BLOCK_MAX_WORD_LEN/8];
	u32 iterator[HASH_ITERATOR_MAX_WORD_LEN];
};

s32 ts_hash_init(struct ts_hash_state *mctx);
s32 ts_hash_update(struct ts_hash_state *mctx,
						const u8 *data, u32 len);
s32 ts_hash_final(struct ts_hash_state *mctx, u8 *out);


#ifdef __cplusplus
}
#endif
#endif
