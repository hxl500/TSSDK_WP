#include <linux/module.h>
#include <soc/ts/wrap/device.h>

void ts_dev_set_drvdata(struct device *dev, void *data)
{
	dev_set_drvdata(dev, data);
}
EXPORT_SYMBOL(ts_dev_set_drvdata);

void *ts_dev_get_drvdata(const struct device *dev)
{
	return dev_get_drvdata(dev);
}
EXPORT_SYMBOL(ts_dev_get_drvdata);

void *ts_dev_get_platdata(const struct device *dev)
{
	return dev_get_platdata(dev);
}
EXPORT_SYMBOL(ts_dev_get_platdata);

struct device_node *ts_of_dev_get_node_dev(struct device *dev)
{
	int errcode = 0;

	if (!dev) {
		errcode = -1; goto err;
	}

	return dev->of_node;
err:
	pr_err("%s %d err %d\n", __func__, __LINE__, errcode);
	return NULL;
}
EXPORT_SYMBOL(ts_of_dev_get_node_dev);

const char *ts_dev_name(const struct device *dev)
{
	return dev_name(dev);
}
EXPORT_SYMBOL(ts_dev_name);


void ts_dev_set_dma_mask_ptr(struct device *dev, void *dma_mask)
{
	dev->dma_mask = dma_mask;
}
EXPORT_SYMBOL(ts_dev_set_dma_mask_ptr);

