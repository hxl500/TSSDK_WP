#include <linux/module.h>
#include <linux/device.h>
#include <media/v4l2-dev.h>
#include <media/videobuf2-v4l2.h>
#include <media/v4l2-device.h>
#include <linux/slab.h>
#include <linux/slab.h>
#include <soc/ts/wrap/v4l2.h>
#include <soc/ts/wrap/v4l2_fh.h>


int ts_v4l2_fh_is_singular_file(struct file *filp)
{
	return v4l2_fh_is_singular_file(filp);
}

EXPORT_SYMBOL_GPL(ts_v4l2_fh_is_singular_file);


struct v4l2_fh *ts_alloc_st_v4l2_fh(void)
{
	return kzalloc(sizeof(struct v4l2_fh), GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(ts_alloc_st_v4l2_fh);

