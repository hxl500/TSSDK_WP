/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_IO_H__
#define __TS_WRAP_IO_H__

#include <linux/io.h>

void ts_writel(u32 value, volatile void __iomem *addr);
unsigned int ts_readl(const volatile void __iomem *addr);

void __iomem *ts_ioremap_wc(phys_addr_t phys_addr, size_t size);
void __iomem *ts_ioremap_nocache(phys_addr_t phys_addr, size_t size);

void ts_iounmap(volatile void __iomem *iomem_cookie);


void ts_crypto_writel(u32 val, void __iomem *addr);
u32 ts_crypto_readl(const void __iomem *addr);
void ts_cve_writel(uint32_t val,void __iomem *addr);
uint32_t ts_cve_readl(const void __iomem *addr);

#endif

