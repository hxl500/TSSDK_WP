#include <linux/module.h>
#include <soc/ts/wrap/jiffies.h>


unsigned long ts_msecs_to_jiffies(const unsigned int m)
{
	return msecs_to_jiffies(m);
}
EXPORT_SYMBOL(ts_msecs_to_jiffies);


