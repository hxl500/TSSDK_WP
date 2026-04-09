#include <linux/module.h>
#include <soc/ts/wrap/proc_fs.h>

struct proc_dir_entry *ts_proc_mkdir(const char *dir, struct proc_dir_entry *entry)
{
	return proc_mkdir(dir, entry);
}
EXPORT_SYMBOL(ts_proc_mkdir);

struct proc_dir_entry *ts_proc_create(const char *name, umode_t mode, struct proc_dir_entry *parent, const struct file_operations *proc_fops)
{
	return proc_create(name, mode, parent, proc_fops);
}
EXPORT_SYMBOL(ts_proc_create);

void ts_proc_remove(struct proc_dir_entry *entry)
{
	proc_remove(entry);
}
EXPORT_SYMBOL(ts_proc_remove);

void ts_remove_proc_entry(const char *dir, struct proc_dir_entry *entry)
{
	remove_proc_entry(dir, entry);
}
EXPORT_SYMBOL(ts_remove_proc_entry);
