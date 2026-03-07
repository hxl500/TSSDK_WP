/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_SCATTERLIST_H__
#define __TS_WRAP_SCATTERLIST_H__

#include <linux/scatterlist.h>

unsigned int ts_sgl_get_offset(struct scatterlist *sgl);
dma_addr_t ts_sg_dma_address(struct scatterlist *sg);
unsigned int ts_sg_dma_len(struct scatterlist *sg);
size_t ts_scatterlist_get_size(void);
void ts_sg_set_page(struct scatterlist *sg, struct page *page,
			       unsigned int len, unsigned int offset);


#endif

