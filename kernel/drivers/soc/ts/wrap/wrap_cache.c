#include <linux/module.h>
#include <soc/ts/wrap/cache.h>


int ts_cache_line_size(void)
{
	return cache_line_size();
}
EXPORT_SYMBOL(ts_cache_line_size);


