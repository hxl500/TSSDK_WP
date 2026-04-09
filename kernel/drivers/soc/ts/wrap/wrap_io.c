#include <linux/module.h>
#include <soc/ts/wrap/io.h>

#ifdef CONFIG_ARCH_TS_TX536
#include <asm/io_lock.h>
#endif

void ts_writel(u32 value, volatile void __iomem *addr)
{
	writel(value, addr);
}
EXPORT_SYMBOL(ts_writel);

unsigned int ts_readl(const volatile void __iomem *addr)
{
	return readl(addr);
}
EXPORT_SYMBOL(ts_readl);


void __iomem *ts_ioremap_wc(phys_addr_t phys_addr, size_t size)
{
	return ioremap_wc(phys_addr, size);
}
EXPORT_SYMBOL(ts_ioremap_wc);

void __iomem *ts_ioremap_nocache(phys_addr_t phys_addr, size_t size)
{
	return ioremap_nocache(phys_addr, size);
}
EXPORT_SYMBOL(ts_ioremap_nocache);

void ts_iounmap(volatile void __iomem *iomem_cookie)
{
	iounmap(iomem_cookie);
}
EXPORT_SYMBOL(ts_iounmap);


#ifdef CONFIG_ARCH_TS_TX536
void ts_crypto_writel(u32 val, void __iomem *addr)
{
	if (in_irq())
		writel_lock(val, addr);
	else
		writel_lock_irqsave(val, addr);
}

u32 ts_crypto_readl(const void __iomem *addr)
{
	if (in_irq())
		return readl_lock(addr);
	else
		return readl_lock_irqsave(addr);
}
#else
void ts_crypto_writel(u32 val, void __iomem *addr)
{
	writel(val, addr);
}

u32 ts_crypto_readl(const void __iomem *addr)
{
	return readl(addr);
}

#endif

EXPORT_SYMBOL(ts_crypto_writel);
EXPORT_SYMBOL(ts_crypto_readl);


#ifdef CONFIG_ARCH_TS_TX536
void ts_cve_writel(uint32_t val,void __iomem *addr)
{
	if (in_irq())
		writel_lock(val,addr);
	else
		writel_lock_irqsave(val,addr);
}

uint32_t ts_cve_readl(const void __iomem *addr)
{
	if (in_irq())
		return readl_lock(addr);
	else
		return readl_lock_irqsave(addr);
}
#else
void ts_cve_writel(uint32_t val,void __iomem *addr)
{
	writel(val, addr);
}

uint32_t ts_cve_readl(const void __iomem *addr)
{
	return readl(addr);
}
#endif
EXPORT_SYMBOL(ts_cve_writel);
EXPORT_SYMBOL(ts_cve_readl);

