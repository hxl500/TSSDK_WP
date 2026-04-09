#include <linux/module.h>
#include <soc/ts/wrap/of_reserved_mem.h>

int ts_of_reserved_mem_device_init(struct device *dev)
{
	return of_reserved_mem_device_init(dev);
}
EXPORT_SYMBOL(ts_of_reserved_mem_device_init);

