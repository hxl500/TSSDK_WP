#include <linux/module.h>
#include <soc/ts/wrap/preempt.h>


int ts_in_irq(void)
{
	return in_irq();
}
EXPORT_SYMBOL(ts_in_irq);

