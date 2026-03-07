/*
 * Tsingmicro io lock
 *
 */


#ifndef __ASM_IO_LOCK_H
#define __ASM_IO_LOCK_H

#ifdef __KERNEL__

#include <asm/io.h>
#include <linux/spinlock.h>

void writeb_lock(u8 val, volatile void __iomem *addr);
void writew_lock(u16 val, volatile void __iomem *addr);
void writel_lock(u32 val, volatile void __iomem *addr);
u8 readb_lock(const volatile void __iomem *addr);
u16 readw_lock(const volatile void __iomem *addr);
u32 readl_lock(const volatile void __iomem *addr);

/*
 * Generic IO read/write with spin_lock_irqsave
 */
void writeb_lock_irqsave(u8 val, volatile void __iomem *addr);
void writew_lock_irqsave(u16 val, volatile void __iomem *addr);
void writel_lock_irqsave(u32 val, volatile void __iomem *addr);
u8 readb_lock_irqsave(const volatile void __iomem *addr);
u16 readw_lock_irqsave(const volatile void __iomem *addr);
u32 readl_lock_irqsave(const volatile void __iomem *addr);

#endif	/* __KERNEL__ */
#endif	/* __ASM_IO_H */

