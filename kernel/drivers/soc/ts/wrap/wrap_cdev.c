#include <linux/module.h>
#include <soc/ts/wrap/cdev.h>
#include <linux/slab.h>


struct cdev *ts_alloc_st_cdev(void)
{
	return kzalloc(sizeof(struct cdev), GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(ts_alloc_st_cdev);


void ts_cdev_set_owner(struct cdev *cdev, struct module *owner)
{
	if (cdev && owner) {
		cdev->owner = owner;
		return;
	}
	pr_err("%s input null\n", __func__);
}
EXPORT_SYMBOL(ts_cdev_set_owner);



