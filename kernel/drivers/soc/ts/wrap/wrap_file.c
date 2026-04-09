#include <linux/module.h>
#include <soc/ts/wrap/file.h>

void ts_file_set_priv(struct file *filp, void *priv)
{
	filp->private_data = priv;
}
EXPORT_SYMBOL(ts_file_set_priv);

void *ts_file_get_priv(struct file *filp)
{
	return filp->private_data;
}
EXPORT_SYMBOL(ts_file_get_priv);

fmode_t ts_file_get_fmode(struct file *filp)
{
	return filp->f_mode;
}
EXPORT_SYMBOL(ts_file_get_fmode);


loff_t *ts_file_get_f_pos_ptr(struct file *filp)
{
	return &filp->f_pos;
}
EXPORT_SYMBOL(ts_file_get_f_pos_ptr);

