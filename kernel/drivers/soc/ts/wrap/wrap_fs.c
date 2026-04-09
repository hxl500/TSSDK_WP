#include <linux/module.h>
#include <soc/ts/wrap/fs.h>


void *ts_inode_get_priv(struct inode *inode)
{
	return inode->i_private;
}
EXPORT_SYMBOL(ts_inode_get_priv);


void *ts_file_get_private_data(struct file *file)
{
	return file->private_data;
}
EXPORT_SYMBOL(ts_file_get_private_data);


void ts_file_set_private_data(struct file *file, void *priv_data)
{
	file->private_data = priv_data;
}
EXPORT_SYMBOL(ts_file_set_private_data);

