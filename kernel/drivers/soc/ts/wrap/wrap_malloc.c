#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>


void *ts_kmalloc(size_t size, gfp_t flags)
{
	return kmalloc(size, flags);
}
EXPORT_SYMBOL(ts_kmalloc);


void *ts_kzalloc(size_t size, gfp_t flags)
{
	return kzalloc(size, flags);
}
EXPORT_SYMBOL(ts_kzalloc);

void *ts_kmalloc_array(size_t n, size_t size, gfp_t flags)
{
	return kmalloc_array(n, size, flags);
}
EXPORT_SYMBOL(ts_kmalloc_array);



