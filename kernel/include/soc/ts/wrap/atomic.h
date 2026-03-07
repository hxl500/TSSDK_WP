/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_ATOMIC_H__
#define __TS_WRAP_ATOMIC_H__

#include <linux/atomic.h>

void ts_atomic_set(atomic_t *v, int i);

void ts_atomic_add(int i, atomic_t *v);
void ts_atomic_sub(int i, atomic_t *v);
void ts_atomic_inc(atomic_t *v);
void ts_atomic_dec(atomic_t *v);
int ts_atomic_read(atomic_t *v);

int ts_atomic_inc_return(atomic_t *v);
int ts_atomic_dec_return(atomic_t *v);

int ts_atomic_dec_and_test(atomic_t *v);
int ts_atomic_xchg(atomic_t *v, int new);
int ts_atomic_cmpxchg(atomic_t *v, int old, int new);

void ts_atomic64_set(atomic64_t *v, long long i);
int ts_atomic64_inc_return(atomic64_t *v);

#endif

