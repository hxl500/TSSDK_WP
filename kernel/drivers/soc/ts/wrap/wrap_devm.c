#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/reset.h>



void *ts_devm_kzalloc(struct device *dev, size_t size, gfp_t gfp)
{
	return devm_kzalloc(dev, size, gfp);
}
EXPORT_SYMBOL_GPL(ts_devm_kzalloc);
void ts_devm_kfree(struct device *dev, void *name)
{
	devm_kfree(dev, name);
}
EXPORT_SYMBOL_GPL(ts_devm_kfree);

