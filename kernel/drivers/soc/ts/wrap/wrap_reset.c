#include <linux/module.h>
#include <soc/ts/wrap/reset.h>

struct reset_control *ts_devm_reset_control_get(
				struct device *dev, const char *id)
{
	return devm_reset_control_get(dev, id);
}
EXPORT_SYMBOL_GPL(ts_devm_reset_control_get);

struct reset_control *ts_devm_reset_control_get_optional(
				 struct device *dev, const char *id)
{
	return devm_reset_control_get_optional(dev, id);
}
EXPORT_SYMBOL(ts_devm_reset_control_get_optional);

struct reset_control *ts_devm_reset_control_get_optional_exclusive(
				 struct device *dev, const char *id)
{
	return devm_reset_control_get_optional_exclusive(dev, id);
}
EXPORT_SYMBOL(ts_devm_reset_control_get_optional_exclusive);


