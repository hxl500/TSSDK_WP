/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_DMA_MAPPING_H__
#define __TS_WRAP_DMA_MAPPING_H__

#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>

int ts_dma_set_mask_and_coherent(struct device *dev, u64 mask);
void *ts_dma_alloc_coherent(struct device *dev, size_t size,
		dma_addr_t *dma_handle, gfp_t flag);
int ts_dma_mmap_coherent(struct device *dev, struct vm_area_struct *vma, void *cpu_addr,
	       dma_addr_t dma_addr, size_t size);
void ts_dma_cap_zero(dma_cap_mask_t *dstp);
void ts_dma_cap_set(enum dma_transaction_type tx_type, dma_cap_mask_t *dstp);
void ts_dma_async_issue_pending(struct dma_chan *chan);
enum dma_status ts_dma_async_is_tx_complete(struct dma_chan *chan,
	dma_cookie_t cookie, dma_cookie_t *last, dma_cookie_t *used);
int ts_dmaengine_terminate_sync(struct dma_chan *chan);
void ts_dma_free_coherent(struct device *dev, size_t size,
		void *cpu_addr, dma_addr_t dma_handle);
int ts_dmaengine_slave_config(struct dma_chan *chan,
					  struct dma_slave_config *config);
struct dma_async_tx_descriptor *ts_dmaengine_prep_dma_interval_memcpy(
			struct dma_chan *chan, struct dma_interval *itrl, unsigned long flags);

struct dma_async_tx_descriptor *ts_dmaengine_prep_dma_memcpy(
		struct dma_chan *chan, dma_addr_t dest, dma_addr_t src,
		size_t len, unsigned long flags);
struct dma_async_tx_descriptor *ts_dmaengine_prep_dma_memcpy_gather(
		struct dma_chan *chan, dma_addr_t dest, dma_addr_t src,
		size_t len, size_t src_cnt, size_t incr_num, unsigned long flags);
struct dma_async_tx_descriptor *ts_dmaengine_prep_dma_memcpy_scatter(
		struct dma_chan *chan, dma_addr_t dest, dma_addr_t src,
		size_t len, size_t dsc_cnt, size_t incr_num, unsigned long flags);
dma_cookie_t ts_dmaengine_submit(struct dma_async_tx_descriptor *desc);
int ts_dma_submit_error(dma_cookie_t cookie);
int ts_dma_mmap_wc(struct device *dev,
			      struct vm_area_struct *vma,
			      void *cpu_addr, dma_addr_t dma_addr,
			      size_t size);
phys_addr_t ts_dma_to_phys(struct device *dev, dma_addr_t dev_addr);

void ts_dma_sync_sg_for_device(struct device *dev, struct scatterlist *sg,
		       int nelems, enum dma_data_direction dir);
void ts_dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sg,
		    int nelems, enum dma_data_direction dir);
void ts_dma_sync_single_for_cpu(struct device *dev, dma_addr_t addr,
					   size_t size,
					   enum dma_data_direction dir);
void ts_dma_sync_single_for_device(struct device *dev,
					      dma_addr_t addr, size_t size,
					      enum dma_data_direction dir);
dma_addr_t ts_dma_map_page(struct device *dev,
					    struct page *page,
					    size_t offset, size_t size,
					    enum dma_data_direction dir);
int ts_dma_map_sg(struct device *dev, struct scatterlist *sg,
				   int nents, enum dma_data_direction dir);
dma_addr_t ts_dma_map_resource(struct device *dev,
					  phys_addr_t phys_addr,
					  size_t size,
					  enum dma_data_direction dir,
					  unsigned long attrs);
int ts_dma_map_sg_attrs(struct device *dev, struct scatterlist *sg,
				   int nents, enum dma_data_direction dir,
				   unsigned long attrs);

void ts_dma_unmap_page(struct device *dev,
					dma_addr_t addr, size_t size,
					enum dma_data_direction dir);
void ts_dma_unmap_sg(struct device *dev, struct scatterlist *sg,
				      int nents, enum dma_data_direction dir);
void ts_dma_unmap_resource(struct device *dev, dma_addr_t addr,
				      size_t size, enum dma_data_direction dir,
				      unsigned long attrs);
void ts_dma_unmap_sg_attrs(struct device *dev, struct scatterlist *sg,
				      int nents, enum dma_data_direction dir,
				      unsigned long attrs);

int ts_dma_mapping_error(struct device *dev, dma_addr_t dma_addr);
int ts_dma_coerce_mask_and_coherent(struct device *dev, u64 mask);

#endif

