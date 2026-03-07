#ifndef _TS_TRNG_H__
#define _TS_TRNG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TRNG_BASE 0x000

//TRNG register address
#define TRNG_CR             (*(volatile u32 *)(TRNG_BASE + 0x0000))
#define TRNG_MSEL           (*(volatile u32 *)(TRNG_BASE + 0x0004))
#define TRNG_SR             (*(volatile u32 *)(TRNG_BASE + 0x0008))
#define TRNG_DR             (*(volatile u32 *)(TRNG_BASE + 0x000C))
#define TRNG_VERSION        (*(volatile u32 *)(TRNG_BASE + 0x0010))
#define TRNG_RESEED         (*(volatile u32 *)(TRNG_BASE + 0x0040))
#define TRNG_HT_CR          (*(volatile u32 *)(TRNG_BASE + 0x0060))
#define TRNG_HT_SR          (*(volatile u32 *)(TRNG_BASE + 0x0070))
#define RO_SRC_EN1          (*(volatile u32 *)(TRNG_BASE + 0x0080))
#define RO_SRC_EN2          (*(volatile u32 *)(TRNG_BASE + 0x0084))
#define SCLK_FREQ           (*(volatile u32 *)(TRNG_BASE + 0x0088))

#define TERO_CR             (*(volatile u32 *)(TRNG_BASE + 0x00B0))
#define TERO_THOLD          (*(volatile u32 *)(TRNG_BASE + 0x00B4))
#define TERO_CNT(i)         (*(volatile u32 *)(TRNG_BASE + 0x00C0 + 4*i))
#define TERO_SR             (*(volatile u32 *)(TRNG_BASE + 0x00D0))
#define TERO_DR            	(*(volatile u32 *)(TRNG_BASE + 0x00D4))
#define TERO_RCR(i)			(*(volatile u32 *)(TRNG_BASE + 0x00E0 + 4*i))


//TRNG freq config
#define TRNG_RO_FREQ_4      (0)
#define TRNG_RO_FREQ_8      (1)
#define TRNG_RO_FREQ_16     (2)
#define TRNG_RO_FREQ_32     (3)     //default


//TRNG action offset
#define TRNG_GLOBAL_INT_OFFSET          (24)
#define TRNG_READ_EMPTY_INT_OFFSET      (17)
#define TRNG_DATA_INT_OFFSET            (16)
#define TRNG_RO_ENTROPY_OFFSET          (4)
#define TRNG_TERO_THRESHOLD_OFFSET      (24)
#define TRNG_TERO_ENTROPY_OFFSET        (8)


//TRNG return code
enum TRNG_RET_CODE
{
	TRNG_SUCCESS = 0,
	TRNG_BUFFER_NULL,
	TRNG_INVALID_INPUT,
	TRNG_INVALID_CONFIG,
	TRNG_HT_ERROR,
	TRNG_ERROR
};


//API

void trng_global_int_enable(void);

void trng_global_int_disable(void);

void trng_empty_read_int_enable(void);

void trng_empty_read_int_disable(void);

void trng_data_int_enable(void);

void trng_data_int_disable(void);

void trng_enable(void);

void trng_disable(void);

u32 trng_ro_entropy_config(u8 cfg);

u32 trng_ro_sub_entropy_config(u8 sn, uint16_t cfg);

void trng_set_mode(u8 with_post_processing);

void trng_reseed(void);

u32 trng_set_freq(u8 freq);

u32 get_rand_internal(u8 *a, u32 bytes);

u32 get_rand(u8 *prand, u32 bytes);



void tero_enable(void);

void tero_disable(void);

u32 tero_set_stop_threshold(u8 threshold_value);

u32 tero_entropy_config(u8 cfg);

void tero_set_output_rng(void);

void tero_set_output_osc_times(void);

void tero_set_osc_sel(u8 cfg);

void tero_set_osc_times_lower_limit(uint16_t value);

void tero_set_osc_times_upper_limit(uint16_t value);

u32 get_tero_rand(u8 *a, u32 bytes);

u32 get_rand(u8 *prand, u32 bytes);

#ifdef __cplusplus
}
#endif


#endif

