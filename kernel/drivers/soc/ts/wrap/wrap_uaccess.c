#include <linux/module.h>
#include <linux/uaccess.h>


unsigned long ts_copy_from_user(void *to, const void __user *from, unsigned long n)
{
	return copy_from_user(to, from, n);
}
EXPORT_SYMBOL(ts_copy_from_user);

unsigned long ts_copy_to_user(void __user *to, const void *from, unsigned long n)
{
	return copy_to_user(to, from, n);
}
EXPORT_SYMBOL(ts_copy_to_user);

int ts_access_ok(unsigned long type, void *addr, unsigned long size)
{
	return access_ok(type, addr, size);
}
EXPORT_SYMBOL(ts_access_ok);


#ifdef __aarch64__
void ts_uaccess_disable_not_uao(void)
{
	uaccess_disable_not_uao();
}
EXPORT_SYMBOL(ts_uaccess_disable_not_uao);

void ts_uaccess_enable_not_uao(void)
{
	uaccess_enable_not_uao();
}
EXPORT_SYMBOL(ts_uaccess_enable_not_uao);
#endif

