/*
 * Tsingmicro io lock
 *
 */

#include <linux/io.h>
#include <linux/spinlock.h>
#include <asm/io_lock.h>

DEFINE_SPINLOCK(io_lock);

/*
 * Generic IO read/write with spin_lock
 */
void writeb_lock(u8 val, volatile void __iomem *addr)
{
	spin_lock(&io_lock);
	writeb(val, addr);
	spin_unlock(&io_lock);
}
EXPORT_SYMBOL(writeb_lock);

void writew_lock(u16 val, volatile void __iomem *addr)
{
	spin_lock(&io_lock);
	writew(val, addr);
	spin_unlock(&io_lock);
}
EXPORT_SYMBOL(writew_lock);

void writel_lock(u32 val, volatile void __iomem *addr)
{
	spin_lock(&io_lock);
	writel(val, addr);
	spin_unlock(&io_lock);
}
EXPORT_SYMBOL(writel_lock);

u8 readb_lock(const volatile void __iomem *addr)
{
	u8 val;

	spin_lock(&io_lock);
	val = readb(addr);
	spin_unlock(&io_lock);
	return val;
}
EXPORT_SYMBOL(readb_lock);

u16 readw_lock(const volatile void __iomem *addr)
{
	u16 val;

	spin_lock(&io_lock);
	val = readw(addr);
	spin_unlock(&io_lock);
	return val;
}
EXPORT_SYMBOL(readw_lock);

u32 readl_lock(const volatile void __iomem *addr)
{
	u32 val;

	spin_lock(&io_lock);
	val = readl(addr);
	spin_unlock(&io_lock);
	return val;
}
EXPORT_SYMBOL(readl_lock);


/*
 * Generic IO read/write with spin_lock_irqsave
 */
void writeb_lock_irqsave(u8 val, volatile void __iomem *addr)
{
	unsigned long flag;

	spin_lock_irqsave(&io_lock, flag);
	writeb(val, addr);
	spin_unlock_irqrestore(&io_lock, flag);
}
EXPORT_SYMBOL(writeb_lock_irqsave);

void writew_lock_irqsave(u16 val, volatile void __iomem *addr)
{
	unsigned long flag;

	spin_lock_irqsave(&io_lock, flag);
	writew(val, addr);
	spin_unlock_irqrestore(&io_lock, flag);
}
EXPORT_SYMBOL(writew_lock_irqsave);

void writel_lock_irqsave(u32 val, volatile void __iomem *addr)
{
	unsigned long flag;

	spin_lock_irqsave(&io_lock, flag);
	writel(val, addr);
	spin_unlock_irqrestore(&io_lock, flag);
}
EXPORT_SYMBOL(writel_lock_irqsave);

u8 readb_lock_irqsave(const volatile void __iomem *addr)
{
	u8 val;
	unsigned long flag;

	spin_lock_irqsave(&io_lock, flag);
	val = readb(addr);
	spin_unlock_irqrestore(&io_lock, flag);
	return val;
}
EXPORT_SYMBOL(readb_lock_irqsave);

u16 readw_lock_irqsave(const volatile void __iomem *addr)
{
	u16 val;
	unsigned long flag;

	spin_lock_irqsave(&io_lock, flag);
	val = readw(addr);
	spin_unlock_irqrestore(&io_lock, flag);
	return val;
}
EXPORT_SYMBOL(readw_lock_irqsave);

u32 readl_lock_irqsave(const volatile void __iomem *addr)
{
	u32 val;
	unsigned long flag;

	spin_lock_irqsave(&io_lock, flag);
	val = readl(addr);
	spin_unlock_irqrestore(&io_lock, flag);
	return val;
}
EXPORT_SYMBOL(readl_lock_irqsave);

