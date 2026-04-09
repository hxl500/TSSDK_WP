#include <linux/module.h>
#include <soc/ts/wrap/sched.h>


void ts__set_current_state(volatile long state_value)
{
	__set_current_state(state_value);
}
EXPORT_SYMBOL(ts__set_current_state);

char *ts_task_struct_get_comm(struct task_struct *task)
{
	return task->comm;
}
EXPORT_SYMBOL(ts_task_struct_get_comm);

int ts_task_struct_get_comm_sz(struct task_struct *task)
{
	return sizeof(task->comm);
}
EXPORT_SYMBOL(ts_task_struct_get_comm_sz);

