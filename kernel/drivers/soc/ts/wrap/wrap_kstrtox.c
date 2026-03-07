#include <linux/module.h>
#include <soc/ts/wrap/kstrtox.h>

int ts_kstrtoul(const char *s, unsigned int base, unsigned long *res)
{
	return kstrtoul(s, base, res);
}
EXPORT_SYMBOL(ts_kstrtoul);

int ts_kstrtoint(const char *s, unsigned int base, int *res)
{
        return kstrtoint(s, base, res);
}
EXPORT_SYMBOL(ts_kstrtoint);


