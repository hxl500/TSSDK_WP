#include <linux/module.h>
#include <soc/ts/wrap/current.h>

pid_t ts_current_get_pid(void)
{
	return current->pid;
}
EXPORT_SYMBOL(ts_current_get_pid);

struct mm_struct *ts_current_get_mm(void)
{
	return current->mm;
}
EXPORT_SYMBOL(ts_current_get_mm);

struct rw_semaphore *ts_current_get_mm_mmap_sem(void)
{
	return &current->mm->mmap_sem;
}
EXPORT_SYMBOL(ts_current_get_mm_mmap_sem);

pid_t ts_current_get_tgid(void)
{
	return current->tgid;
}
EXPORT_SYMBOL(ts_current_get_tgid);
