/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_RESOURCE_H__
#define __TS_WRAP_RESOURCE_H__

#include <linux/types.h>
#include <linux/ioport.h>

struct resource *ts_request_mem_region(resource_size_t start, resource_size_t n,
				const char *name);
void ts_release_mem_region(resource_size_t start, resource_size_t n);


#endif

