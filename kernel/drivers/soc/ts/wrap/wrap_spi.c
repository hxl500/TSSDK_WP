#include <linux/module.h>
#include <linux/spi/spi.h>


void *ts_spi_get_drvdata(struct spi_device *spi)
{
	return spi_get_drvdata(spi);
}
EXPORT_SYMBOL_GPL(ts_spi_get_drvdata);

void ts_spi_set_drvdata(struct spi_device *spi, void *data)
{
	spi_set_drvdata(spi, data);
}
EXPORT_SYMBOL_GPL(ts_spi_set_drvdata);

unsigned int ts_spi_get_max_speed(struct spi_device *spi)
{
	if (spi)
		return spi->max_speed_hz;

	pr_err("%s input null\n", __func__);
	return 0;
}
EXPORT_SYMBOL_GPL(ts_spi_get_max_speed);

struct device_node *ts_of_get_dev_node_spi(struct spi_device *pspi_dev)
{
	int errcode = 0;

	if (!pspi_dev) {
		errcode = -1; goto err;
	}

	return pspi_dev->dev.of_node;
err:
	pr_err("%s %d err %d\n", __func__, __LINE__, errcode);
	return NULL;
}
EXPORT_SYMBOL_GPL(ts_of_get_dev_node_spi);

void ts_spi_message_init(struct spi_message *m)
{
	spi_message_init(m);
}
EXPORT_SYMBOL_GPL(ts_spi_message_init);

void ts_spi_message_add_tail(struct spi_transfer *t, struct spi_message *m)
{
	spi_message_add_tail(t, m);
}
EXPORT_SYMBOL_GPL(ts_spi_message_add_tail);

