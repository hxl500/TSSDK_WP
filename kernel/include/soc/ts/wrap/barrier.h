/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_BARRIER_H__
#define __TS_WRAP_BARRIER_H__

#include <linux/device.h>

void ts_mb(void);
void ts_wmb(void);
void ts_smp_mb(void);
void ts_smp_rmb(void);
void ts_smp_wmb(void);
void ts_smp_read_barrier_depends(void);
void ts_barrier(void);


#endif

