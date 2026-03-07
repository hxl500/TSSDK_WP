#include <linux/module.h>
#include <soc/ts/wrap/bitops.h>


void ts_set_bit(int nr, volatile unsigned long *p)
{
	set_bit(nr, p);
}
EXPORT_SYMBOL(ts_set_bit);

void ts_clear_bit(int nr, volatile unsigned long *p)
{
	clear_bit(nr, p);
}
EXPORT_SYMBOL(ts_clear_bit);

void ts_change_bit(int nr, volatile unsigned long *p)
{
	change_bit(nr, p);
}
EXPORT_SYMBOL(ts_change_bit);

int ts_test_and_set_bit(int nr, volatile unsigned long *p)
{
	return test_and_set_bit(nr, p);
}
EXPORT_SYMBOL(ts_test_and_set_bit);

int ts_test_and_clear_bit(int nr, volatile unsigned long *p)
{
	return test_and_clear_bit(nr, p);
}
EXPORT_SYMBOL(ts_test_and_clear_bit);

int ts_test_and_change_bit(int nr, volatile unsigned long *p)
{
	return test_and_change_bit(nr, p);
}
EXPORT_SYMBOL(ts_test_and_change_bit);

