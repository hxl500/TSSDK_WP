#include <linux/module.h>
#include <linux/rcupdate.h>
#include <linux/rculist.h>
#include <soc/ts/wrap/rcu.h>


void ts_rcu_read_lock(void)
{
	rcu_read_lock();

}
EXPORT_SYMBOL(ts_rcu_read_lock);


void ts_rcu_read_unlock(void)
{
	rcu_read_unlock();
}
EXPORT_SYMBOL(ts_rcu_read_unlock);

void ts_list_del_rcu(struct list_head *entry)
{
	list_del_rcu(entry);
}
EXPORT_SYMBOL(ts_list_del_rcu);

void ts_list_add_tail_rcu(struct list_head *new,
					struct list_head *head)
{
	list_add_tail_rcu(new, head);
}
EXPORT_SYMBOL(ts_list_add_tail_rcu);
