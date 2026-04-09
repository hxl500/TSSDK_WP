#include <linux/module.h>
#include <soc/ts/wrap/of_device.h>

int ts_of_dma_configure(struct device *dev, struct device_node *np)
{
	return of_dma_configure(dev, np);
}
EXPORT_SYMBOL(ts_of_dma_configure);
