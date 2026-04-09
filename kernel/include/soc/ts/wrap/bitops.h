/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_BITOPS_H__
#define __TS_WRAP_BITOPS_H__

#include <linux/bitops.h>

void ts_set_bit(int nr, volatile unsigned long *p);
void ts_clear_bit(int nr, volatile unsigned long *p);
void ts_change_bit(int nr, volatile unsigned long *p);
int ts_test_and_set_bit(int nr, volatile unsigned long *p);
int ts_test_and_clear_bit(int nr, volatile unsigned long *p);
int ts_test_and_change_bit(int nr, volatile unsigned long *p);

#endif

