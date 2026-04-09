#include <linux/module.h>
#include <soc/ts/wrap/idr.h>


int ts_idr_alloc(struct idr *idr, void *ptr,
			    int start, int end, gfp_t gfp)
{
	return idr_alloc(idr, ptr, start, end, gfp);
}
EXPORT_SYMBOL(ts_idr_alloc);


void ts_idr_init(struct idr *idr)
{
	idr_init(idr);
}
EXPORT_SYMBOL(ts_idr_init);


void ts_idr_preload_end(void)
{
	idr_preload_end();
}
EXPORT_SYMBOL(ts_idr_preload_end);

