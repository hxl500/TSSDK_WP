/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_KFIFO_H__
#define __TS_WRAP_KFIFO_H__

#include <linux/kfifo.h>

typedef struct kfifo kfifo_wrap;




void ts_kfifo_in_spinlocked(kfifo_wrap* kfifo_t, unsigned int *buf, unsigned int size,void *lock);
int ts_kfifo_out_spinlocked(kfifo_wrap* kfifo_t, unsigned int *buf, unsigned int size,void *lock);
int  ts_kfifo_alloc(kfifo_wrap* kfifo_t,unsigned int size,gfp_t gfp_mask);
void  ts_kfifo_free(kfifo_wrap* kfifo_t);

int  ts_kfifo_len(kfifo_wrap* kfifo_t);
void  ts_kfifo_reset(kfifo_wrap* kfifo_t);
int ts_kfifo_is_full(kfifo_wrap* kfifo_t);



#endif

