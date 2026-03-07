#include <linux/module.h>
#include <soc/ts/wrap/delay.h>

void ts_udelay(unsigned long us)
{
	udelay(us);
}
EXPORT_SYMBOL(ts_udelay);

void ts_mdelay(unsigned long ms)
{
	mdelay(ms);
}
EXPORT_SYMBOL(ts_mdelay);

