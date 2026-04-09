#include <linux/module.h>
#include <soc/ts/wrap/dma-buf.h>

void ts_get_dma_buf(struct dma_buf *dmabuf)
{
	get_dma_buf(dmabuf);
}
EXPORT_SYMBOL(ts_get_dma_buf);

const char *ts_get_dma_buf_exp_name(struct dma_buf *dmabuf)
{
	return dmabuf->exp_name;
}
EXPORT_SYMBOL(ts_get_dma_buf_exp_name);


void *ts_get_dmabuf_priv(struct dma_buf *dmabuf)
{
	return dmabuf->priv;
}
EXPORT_SYMBOL(ts_get_dmabuf_priv);

void ts_set_dmabuf_priv(struct dma_buf *dmabuf, void *priv)
{
	dmabuf->priv = priv;
}
EXPORT_SYMBOL(ts_set_dmabuf_priv);


