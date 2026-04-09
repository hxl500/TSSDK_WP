#include <linux/module.h>
#include <soc/ts/wrap/barrier.h>


void ts_mb(void)
{
	mb();
}
EXPORT_SYMBOL(ts_mb);
void ts_wmb(void)
{
	wmb();
}
EXPORT_SYMBOL(ts_wmb);

void ts_smp_mb(void)
{
	smp_mb();
}
EXPORT_SYMBOL(ts_smp_mb);

void ts_smp_rmb(void)
{
	smp_rmb();
}
EXPORT_SYMBOL(ts_smp_rmb);

void ts_smp_wmb(void)
{
	smp_wmb();
}
EXPORT_SYMBOL(ts_smp_wmb);

void ts_smp_read_barrier_depends(void)
{
	smp_read_barrier_depends();
}
EXPORT_SYMBOL(ts_smp_read_barrier_depends);


void ts_barrier(void)
{
	barrier();
}
EXPORT_SYMBOL(ts_barrier);

