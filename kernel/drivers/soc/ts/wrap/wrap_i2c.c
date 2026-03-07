#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/i2c.h>


void *ts_i2c_get_clientdata(const struct i2c_client *dev)
{
	return i2c_get_clientdata(dev);
}
EXPORT_SYMBOL_GPL(ts_i2c_get_clientdata);


struct device_node *ts_of_get_dev_node_i2c_c(struct i2c_client *client)
{
	int errcode = 0;

	if (!client) {
		errcode = -1; goto err;
	}

	return client->dev.of_node;
err:
	pr_err("%s %d err %d\n", __func__, __LINE__, errcode);
	return NULL;
}
EXPORT_SYMBOL_GPL(ts_of_get_dev_node_i2c_c);

