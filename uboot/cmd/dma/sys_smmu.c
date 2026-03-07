#include <stdint.h>
#include "sys_smu.h"
#include "reg.h"

#include <linux/bitops.h>
#include <linux/bug.h>
#include <linux/io.h>
//#include "io.h"
#define SMMU_DMA0_BYPASS_ENABLE 0x8000000
#define SMMU_DMA0_LP_ENABLE 0x20000
#define SMMU_DMA0_IDLE_BIT (17)

static inline void set_bit_mask(int mask, volatile uint32_t *addr)
{
    *addr |= mask;
}

static inline void clear_bit_mask(int mask, volatile uint32_t *addr)
{
    *addr &= ~mask;
}

void smmu_dma0_bypass(void)
{
	writel(SMMU_DMA0_BYPASS_ENABLE, (uint32_t *)(MCU_SGRF_BASE + 0x54));	 //bypass
}

/*
 * dma0 mmu enter lowpower mode
 */
void smmu_dma0_lp_en(void)
{
	set_bit_mask(SMMU_DMA0_LP_ENABLE, (uint32_t *)(MCU_SGRF_BASE + 0x54));
}

/*dma0 mmu is idle*/
void smmu_dma0_is_idle(void)
{
	test_bit(SMMU_DMA0_IDLE_BIT, (uint64_t *)MCU_GRF_BASE + 0xF0);
}

void smmu_dma0_lp_quit(void)
{
	clear_bit_mask(SMMU_DMA0_LP_ENABLE, (uint32_t *)(MCU_SGRF_BASE + 0x54));
}

void smmu_set_sgrf_act(uint32_t exp_data, uint8_t lshift)
{
	uint32_t wdata;
	uint32_t leftdat;

	wdata = readl(MCU_SGRF_BASE + 0x54);
	leftdat = exp_data << lshift;
	wdata = wdata & (~leftdat);
	writel(wdata, MCU_SGRF_BASE + 0x54);
}
