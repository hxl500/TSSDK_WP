/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_DMA_BUF_H__
#define __TS_WRAP_DMA_BUF_H__

#include <linux/dma-buf.h>

void ts_get_dma_buf(struct dma_buf *dmabuf);
const char *ts_get_dma_buf_exp_name(struct dma_buf *dmabuf);
void *ts_get_dmabuf_priv(struct dma_buf *dmabuf);
void ts_set_dmabuf_priv(struct dma_buf *dmabuf, void *priv);

#endif

