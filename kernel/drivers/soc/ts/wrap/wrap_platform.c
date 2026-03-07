#include <linux/module.h>
#include <soc/ts/wrap/platform.h>

void *ts_platform_get_dev_pm_domain(const struct platform_device *pdev)
{
	return pdev->dev.pm_domain;
}
EXPORT_SYMBOL(ts_platform_get_dev_pm_domain);


void *ts_platform_get_drvdata(const struct platform_device *pdev)
{
	return platform_get_drvdata(pdev);
}
EXPORT_SYMBOL(ts_platform_get_drvdata);

void ts_platform_set_drvdata(struct platform_device *pdev, void *data)
{
	platform_set_drvdata(pdev, data);
}
EXPORT_SYMBOL(ts_platform_set_drvdata);

struct device_node *ts_of_get_dev_node_plat(struct platform_device *pdev)
{
	int errcode = 0;

	if (!pdev) {
		errcode = -1; goto err;
	}

	return pdev->dev.of_node;
err:
	pr_err("%s %d err %d\n", __func__, __LINE__, errcode);
	return NULL;
}
EXPORT_SYMBOL_GPL(ts_of_get_dev_node_plat);
