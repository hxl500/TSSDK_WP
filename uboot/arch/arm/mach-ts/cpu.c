// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2014 - 2015 Xilinx, Inc.
 * Michal Simek <michal.simek@xilinx.com>
 */

#include <common.h>
#include <init.h>
#include <time.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#if defined(CONFIG_ARCH_TS_TX536) || defined(CONFIG_ARCH_TS_TX5336)
#include <asm/armv8/mmu.h>
#else
#include <asm/mmu.h>
#endif
#include <asm/cache.h>
#include <asm/io.h>
#include <zynqmp_firmware.h>
#include <asm/cache.h>

#define ZYNQ_SILICON_VER_MASK	0xF000
#define ZYNQ_SILICON_VER_SHIFT	12

DECLARE_GLOBAL_DATA_PTR;

/*
 * Number of filled static entries and also the first empty
 * slot in zynqmp_mem_map.
 */
#define ZYNQMP_MEM_MAP_USED	4

#if !defined(CONFIG_ZYNQMP_NO_DDR)
#define DRAM_BANKS CONFIG_NR_DRAM_BANKS
#else
#define DRAM_BANKS 0
#endif

#if defined(CONFIG_DEFINE_TCM_OCM_MMAP)
#define TCM_MAP 1
#else
#define TCM_MAP 0
#endif

/* +1 is end of list which needs to be empty */
#define ZYNQMP_MEM_MAP_MAX (ZYNQMP_MEM_MAP_USED + DRAM_BANKS + TCM_MAP + 1)

#if defined(CONFIG_ARCH_TS_TX5215) || defined(CONFIG_ARCH_TS_TX5112)
#define PTE_BLOCK_MEMTYPE(x)	((x) << 2)
#define PTE_BLOCK_NS            (1 << 5)
#define PTE_BLOCK_NON_SHARE	(0 << 8)
#define PTE_BLOCK_OUTER_SHARE	(2 << 8)
#define PTE_BLOCK_INNER_SHARE	(3 << 8)
#define PTE_BLOCK_AF		(1 << 10)
#define PTE_BLOCK_NG		(1 << 11)
#define PTE_BLOCK_PXN		(UL(1) << 23) //(UL(1) << 53)  //to modify
#define PTE_BLOCK_UXN		(UL(1) << 24) //(UL(1) << 54)  //to modify

/*
 * Memory types
 */
#define MT_DEVICE_NGNRNE	0
#define MT_DEVICE_NGNRE		1
#define MT_DEVICE_GRE		2
#define MT_NORMAL_NC		3
#define MT_NORMAL		4

struct mm_region {
	u64 virt;
	u64 phys;
	u64 size;
	u64 attrs;
};
#endif

static struct mm_region zynqmp_mem_map[ZYNQMP_MEM_MAP_MAX] = {
	{  /*ospi flash*/
		.virt = 0xE0000000UL,
		.phys = 0xE0000000UL,
		.size = 0x10000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	},
	{
		.virt = 0xF0000000UL,
		.phys = 0xF0000000UL,
		.size = 0x00d00000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	},
	{
		.virt = 0xF0D00000UL,
		.phys = 0xF0D00000UL,
		.size = 0x001c0000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	},
	{
		.virt = 0xF1000000UL,
		.phys = 0xF1000000UL,
		.size = 0x002a6000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	},
};

void mem_map_fill(void)
{
	int banks = ZYNQMP_MEM_MAP_USED;

#if defined(CONFIG_DEFINE_TCM_OCM_MMAP)
	zynqmp_mem_map[banks].virt = 0xffe00000UL;
	zynqmp_mem_map[banks].phys = 0xffe00000UL;
	zynqmp_mem_map[banks].size = 0x00200000UL;
	zynqmp_mem_map[banks].attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
				      PTE_BLOCK_INNER_SHARE;
	banks = banks + 1;
#endif

#if !defined(CONFIG_ZYNQMP_NO_DDR)
	for (int i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		/* Zero size means no more DDR that's this is end */
		if (!gd->bd->bi_dram[i].size)
			break;

		zynqmp_mem_map[banks].virt = gd->bd->bi_dram[i].start;
		zynqmp_mem_map[banks].phys = gd->bd->bi_dram[i].start;
		zynqmp_mem_map[banks].size = gd->bd->bi_dram[i].size;
		zynqmp_mem_map[banks].attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
					      PTE_BLOCK_INNER_SHARE;
		banks = banks + 1;
	}
#endif

	memset(&zynqmp_mem_map[banks], 0, sizeof(zynqmp_mem_map[banks]));
}

struct mm_region *mem_map = zynqmp_mem_map;

u64 get_page_table_size(void)
{
	return 0x14000;
}

#if defined(CONFIG_SYS_MEM_RSVD_FOR_MMU) || defined(CONFIG_DEFINE_TCM_OCM_MMAP)
void tcm_init(u8 mode)
{
	puts("WARNING: Initializing TCM overwrites TCM content\n");
	initialize_tcm(mode);
	memset((void *)ZYNQMP_TCM_BASE_ADDR, 0, ZYNQMP_TCM_SIZE);
}
#endif

#ifdef CONFIG_SYS_MEM_RSVD_FOR_MMU
int arm_reserve_mmu(void)
{
	tcm_init(TCM_LOCK);
	gd->arch.tlb_size = PGTABLE_SIZE;
	gd->arch.tlb_addr = ZYNQMP_TCM_BASE_ADDR;

	return 0;
}
#endif

unsigned int zynqmp_get_silicon_version_secure(void)
{
	u32 ver;

	ver = readl(&csu_base->version);
	ver &= ZYNQMP_SILICON_VER_MASK;
	ver >>= ZYNQMP_SILICON_VER_SHIFT;

	return ver;
}

unsigned int zynqmp_get_silicon_version(void)
{
#if defined(CONFIG_ARCH_TS_TX536) || defined(CONFIG_ARCH_TS_TX5336)
	if (current_el() == 3)
		return zynqmp_get_silicon_version_secure();
#endif
	gd->cpu_clk = get_tbclk();

	switch (gd->cpu_clk) {
	case 50000000:
		return ZYNQMP_CSU_VERSION_QEMU;
	}

	return ZYNQMP_CSU_VERSION_SILICON;
}

static int zynqmp_mmio_rawwrite(const u32 address,
		      const u32 mask,
		      const u32 value)
{
	u32 data;
	u32 value_local = value;
	int ret;

	ret = zynqmp_mmio_read(address, &data);
	if (ret)
		return ret;

	data &= ~mask;
	value_local &= mask;
	value_local |= data;
	writel(value_local, (ulong)address);
	return 0;
}

static int zynqmp_mmio_rawread(const u32 address, u32 *value)
{
	*value = readl((ulong)address);
	return 0;
}

int zynqmp_is_rawrw(void)
{
#if defined(CONFIG_ARCH_TS_TX536) || defined(CONFIG_ARCH_TS_TX5336)
	if (current_el() == 3)
		return 1;
#endif
	if (IS_ENABLED(CONFIG_SPL_BUILD))
		return 1;
	else
		return 0;
}

int zynqmp_mmio_write(const u32 address,
		      const u32 mask,
		      const u32 value)
{
	if (zynqmp_is_rawrw())
		return zynqmp_mmio_rawwrite(address, mask, value);
#if defined(CONFIG_ZYNQMP_FIRMWARE)
	else
		return xilinx_pm_request(PM_MMIO_WRITE, address, mask,
					 value, 0, NULL);
#endif

	return -EINVAL;
}

int zynqmp_mmio_read(const u32 address, u32 *value)
{
	u32 ret = -EINVAL;

	if (!value)
		return ret;

	if (zynqmp_is_rawrw()) {
		ret = zynqmp_mmio_rawread(address, value);
	}
#if defined(CONFIG_ZYNQMP_FIRMWARE)
	else {
		u32 ret_payload[PAYLOAD_ARG_CNT];

		ret = xilinx_pm_request(PM_MMIO_READ, address, 0, 0,
					0, ret_payload);
		*value = ret_payload[1];
	}
#endif

	return ret;
}


