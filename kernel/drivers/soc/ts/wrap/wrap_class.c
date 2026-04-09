#include <linux/module.h>
#include <soc/ts/wrap/class.h>

struct class *ts_class_create(struct module *owner, const char *name)
{
	return class_create(owner, name);
}
EXPORT_SYMBOL(ts_class_create);


