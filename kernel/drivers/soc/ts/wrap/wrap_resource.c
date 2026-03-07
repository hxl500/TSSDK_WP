#include <soc/ts/wrap/resource.h>
#include <linux/module.h>


struct resource *ts_request_mem_region(resource_size_t start, resource_size_t n,
				const char *name)
{
	return request_mem_region(start, n, name);
}
EXPORT_SYMBOL(ts_request_mem_region);


void ts_release_mem_region(resource_size_t start, resource_size_t n)
{
	release_mem_region(start, n);
}
EXPORT_SYMBOL(ts_release_mem_region);

