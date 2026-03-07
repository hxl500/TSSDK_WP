#include <linux/module.h>
#include <soc/ts/wrap/getorder.h>

int ts_get_order(unsigned long size)
{
	return get_order(size);
}
EXPORT_SYMBOL(ts_get_order);

