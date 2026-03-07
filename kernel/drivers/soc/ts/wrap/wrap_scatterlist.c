#include <linux/module.h>
#include <soc/ts/wrap/scatterlist.h>

unsigned int ts_sgl_get_offset(struct scatterlist *sgl)
{
	return sgl->offset;
}
EXPORT_SYMBOL(ts_sgl_get_offset);

dma_addr_t ts_sg_dma_address(struct scatterlist *sg)
{
	return sg_dma_address(sg);
}
EXPORT_SYMBOL(ts_sg_dma_address);

unsigned int ts_sg_dma_len(struct scatterlist *sg)
{
	return sg_dma_len(sg);
}
EXPORT_SYMBOL(ts_sg_dma_len);

size_t ts_scatterlist_get_size(void)
{
	return sizeof(struct scatterlist);
}
EXPORT_SYMBOL(ts_scatterlist_get_size);

void ts_sg_set_page(struct scatterlist *sg, struct page *page,
			       unsigned int len, unsigned int offset)
{
	sg_set_page(sg, page, len, offset);
}
EXPORT_SYMBOL(ts_sg_set_page);


