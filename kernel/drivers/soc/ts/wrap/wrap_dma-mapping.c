#include <linux/module.h>
#include <soc/ts/wrap/dma-mapping.h>

int ts_dma_set_mask_and_coherent(struct device *dev, u64 mask)
{
	return dma_set_mask_and_coherent(dev, mask);
}
EXPORT_SYMBOL(ts_dma_set_mask_and_coherent);

void *ts_dma_alloc_coherent(struct device *dev, size_t size,
		dma_addr_t *dma_handle, gfp_t flag)
{
	return dma_alloc_coherent(dev, size, dma_handle, flag);
}
EXPORT_SYMBOL(ts_dma_alloc_coherent);


int ts_dma_mmap_coherent(struct device *dev, struct vm_area_struct *vma, void *cpu_addr,
	       dma_addr_t dma_addr, size_t size)
{
	return dma_mmap_attrs(dev, vma, cpu_addr, dma_addr, size, 0);
}
EXPORT_SYMBOL(ts_dma_mmap_coherent);


void ts_dma_cap_zero(dma_cap_mask_t *dstp)
{
	return __dma_cap_zero(dstp);
}
EXPORT_SYMBOL(ts_dma_cap_zero);

void ts_dma_cap_set(enum dma_transaction_type tx_type, dma_cap_mask_t *dstp)
{
	return __dma_cap_set(tx_type, dstp);
}
EXPORT_SYMBOL(ts_dma_cap_set);

void ts_dma_async_issue_pending(struct dma_chan *chan)
{
	return dma_async_issue_pending(chan);
}
EXPORT_SYMBOL(ts_dma_async_issue_pending);

enum dma_status ts_dma_async_is_tx_complete(struct dma_chan *chan,
	dma_cookie_t cookie, dma_cookie_t *last, dma_cookie_t *used)
{
	return dma_async_is_tx_complete(chan, cookie, last, used);
}
EXPORT_SYMBOL(ts_dma_async_is_tx_complete);

int ts_dmaengine_terminate_sync(struct dma_chan *chan)
{
	return dmaengine_terminate_sync(chan);
}
EXPORT_SYMBOL(ts_dmaengine_terminate_sync);

void ts_dma_free_coherent(struct device *dev, size_t size,
		void *cpu_addr, dma_addr_t dma_handle)
{
	return dma_free_coherent(dev, size, cpu_addr, dma_handle);
}
EXPORT_SYMBOL(ts_dma_free_coherent);

int ts_dmaengine_slave_config(struct dma_chan *chan,
					  struct dma_slave_config *config)
{
	return dmaengine_slave_config(chan, config);
}
EXPORT_SYMBOL(ts_dmaengine_slave_config);

struct dma_async_tx_descriptor *ts_dmaengine_prep_dma_interval_memcpy(
	struct dma_chan *chan, struct dma_interval *itrl,
	unsigned long flags)
{
	return dmaengine_prep_dma_interval_memcpy(chan, itrl, flags);
}
EXPORT_SYMBOL(ts_dmaengine_prep_dma_interval_memcpy);

struct dma_async_tx_descriptor *ts_dmaengine_prep_dma_memcpy(
		struct dma_chan *chan, dma_addr_t dest, dma_addr_t src,
		size_t len, unsigned long flags)
{
	return dmaengine_prep_dma_memcpy(chan, dest, src,
						    len, flags);
}
EXPORT_SYMBOL(ts_dmaengine_prep_dma_memcpy);

struct dma_async_tx_descriptor *ts_dmaengine_prep_dma_memcpy_gather(
		struct dma_chan *chan, dma_addr_t dest, dma_addr_t src,
		size_t len, size_t src_cnt, size_t incr_num, unsigned long flags)
{
	return dmaengine_prep_dma_memcpy_gather(chan, dest, src,
						    len, src_cnt, incr_num, flags);
}
EXPORT_SYMBOL(ts_dmaengine_prep_dma_memcpy_gather);

struct dma_async_tx_descriptor *ts_dmaengine_prep_dma_memcpy_scatter(
		struct dma_chan *chan, dma_addr_t dest, dma_addr_t src,
		size_t len, size_t dsc_cnt, size_t incr_num, unsigned long flags)
{
	return dmaengine_prep_dma_memcpy_scatter(chan, dest, src,
						    len, dsc_cnt, incr_num, flags);
}
EXPORT_SYMBOL(ts_dmaengine_prep_dma_memcpy_scatter);

dma_cookie_t ts_dmaengine_submit(struct dma_async_tx_descriptor *desc)
{
	return dmaengine_submit(desc);
}
EXPORT_SYMBOL(ts_dmaengine_submit);

int ts_dma_submit_error(dma_cookie_t cookie)
{
	return dma_submit_error(cookie);
}
EXPORT_SYMBOL(ts_dma_submit_error);


int ts_dma_mmap_wc(struct device *dev,
			      struct vm_area_struct *vma,
			      void *cpu_addr, dma_addr_t dma_addr,
			      size_t size)
{
	return dma_mmap_wc(dev, vma, cpu_addr, dma_addr, size);
}
EXPORT_SYMBOL(ts_dma_mmap_wc);

phys_addr_t ts_dma_to_phys(struct device *dev, dma_addr_t dev_addr)
{
	return dma_to_phys(dev, dev_addr);
}
EXPORT_SYMBOL(ts_dma_to_phys);


void ts_dma_sync_sg_for_device(struct device *dev, struct scatterlist *sg,
		       int nelems, enum dma_data_direction dir)
{
	dma_sync_sg_for_device(dev, sg, nelems, dir);
}
EXPORT_SYMBOL(ts_dma_sync_sg_for_device);

void ts_dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sg,
		    int nelems, enum dma_data_direction dir)
{
	dma_sync_sg_for_cpu(dev, sg, nelems, dir);
}
EXPORT_SYMBOL(ts_dma_sync_sg_for_cpu);

void ts_dma_sync_single_for_cpu(struct device *dev, dma_addr_t addr,
					   size_t size,
					   enum dma_data_direction dir)
{
	dma_sync_single_for_cpu(dev, addr, size, dir);
}
EXPORT_SYMBOL(ts_dma_sync_single_for_cpu);

void ts_dma_sync_single_for_device(struct device *dev,
					      dma_addr_t addr, size_t size,
					      enum dma_data_direction dir)
{
	dma_sync_single_for_device(dev, addr, size, dir);
}
EXPORT_SYMBOL(ts_dma_sync_single_for_device);

dma_addr_t ts_dma_map_page(struct device *dev,
					    struct page *page,
					    size_t offset, size_t size,
					    enum dma_data_direction dir)
{
	return dma_map_page(dev, page, offset, size, dir);

}
EXPORT_SYMBOL(ts_dma_map_page);

void ts_dma_unmap_page(struct device *dev,
					dma_addr_t addr, size_t size,
					enum dma_data_direction dir)
{
	dma_unmap_page(dev, addr, size, dir);
}
EXPORT_SYMBOL(ts_dma_unmap_page);


int ts_dma_map_sg(struct device *dev, struct scatterlist *sg,
				   int nents, enum dma_data_direction dir)
{
	return dma_map_sg(dev, sg, nents, dir);

}
EXPORT_SYMBOL(ts_dma_map_sg);

int ts_dma_map_sg_attrs(struct device *dev, struct scatterlist *sg,
				   int nents, enum dma_data_direction dir,
				   unsigned long attrs)
{
	return dma_map_sg_attrs(dev, sg, nents, dir, attrs);
}
EXPORT_SYMBOL(ts_dma_map_sg_attrs);

dma_addr_t ts_dma_map_resource(struct device *dev,
					  phys_addr_t phys_addr,
					  size_t size,
					  enum dma_data_direction dir,
					  unsigned long attrs)
{
	return dma_map_resource(dev, phys_addr, size, dir, attrs);
}
EXPORT_SYMBOL(ts_dma_map_resource);

void ts_dma_unmap_sg(struct device *dev, struct scatterlist *sg,
				      int nents, enum dma_data_direction dir)
{
	dma_unmap_sg(dev, sg, nents, dir);

}
EXPORT_SYMBOL(ts_dma_unmap_sg);

void ts_dma_unmap_resource(struct device *dev, dma_addr_t addr,
				      size_t size, enum dma_data_direction dir,
				      unsigned long attrs)
{
	dma_unmap_resource(dev, addr, size, dir, attrs);
}
EXPORT_SYMBOL(ts_dma_unmap_resource);

void ts_dma_unmap_sg_attrs(struct device *dev, struct scatterlist *sg,
				      int nents, enum dma_data_direction dir,
				      unsigned long attrs)
{
	dma_unmap_sg_attrs(dev, sg, nents, dir, attrs);
}
EXPORT_SYMBOL(ts_dma_unmap_sg_attrs);

int ts_dma_mapping_error(struct device *dev, dma_addr_t dma_addr)
{
	return dma_mapping_error(dev, dma_addr);
}
EXPORT_SYMBOL(ts_dma_mapping_error);

int ts_dma_coerce_mask_and_coherent(struct device *dev, u64 mask)
{
	return dma_coerce_mask_and_coherent(dev, mask);
}
EXPORT_SYMBOL(ts_dma_coerce_mask_and_coherent);

