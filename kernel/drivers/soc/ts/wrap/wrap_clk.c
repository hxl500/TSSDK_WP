#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/clk.h>


int ts_clk_prepare_enable(struct clk *clk)
{
	return clk_prepare_enable(clk);
}
EXPORT_SYMBOL_GPL(ts_clk_prepare_enable);

void ts_clk_disable_unprepare(struct clk *clk)
{
	clk_disable_unprepare(clk);
}
EXPORT_SYMBOL_GPL(ts_clk_disable_unprepare);


