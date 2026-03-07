#include <linux/module.h>
#include <soc/ts/wrap/seqfile.h>

void ts_seqfile_set_priv(struct seq_file *seqf, void *priv)
{
	seqf->private = priv;
}
EXPORT_SYMBOL(ts_seqfile_set_priv);

void *ts_seqfile_get_priv(struct seq_file *seqf)
{
	return seqf->private;
}
EXPORT_SYMBOL(ts_seqfile_get_priv);

