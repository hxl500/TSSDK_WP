/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_IDR_H__
#define __TS_WRAP_IDR_H__

#include <linux/idr.h>


int ts_idr_alloc(struct idr *idr, void *ptr,
			    int start, int end, gfp_t gfp);

void ts_idr_init(struct idr *idr);
void ts_idr_preload_end(void);


#endif

