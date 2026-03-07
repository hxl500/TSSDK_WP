/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_PAGE_H__
#define __TS_WRAP_PAGE_H__

#include  <linux/page-flags.h>

#define ts_alloc_page(gfp_mask) ts_alloc_pages(gfp_mask, 0)

int ts_PageHighMem(struct page *page);
int ts_PageReserved(struct page *page);

void ts_SetPageReserved(struct page *page);
void ts_ClearPageReserved(struct page *page);
void ts_SetPageDirty(struct page *page);

size_t ts_page_get_size(void);
struct page *ts_alloc_pages(gfp_t gfp_mask, unsigned int order);

void ts_put_page(struct page *page);

int ts_get_page_unless_zero(struct page *page);

#endif

