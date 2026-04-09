/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_MALLOC_H__
#define __TS_WRAP_MALLOC_H__

void *ts_kmalloc(size_t size, gfp_t flags);
void *ts_kzalloc(size_t size, gfp_t flags);
void *ts_kmalloc_array(size_t n, size_t size, gfp_t flags);

#endif

