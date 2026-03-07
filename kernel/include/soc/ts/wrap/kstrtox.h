/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_KSTRTOX_H__
#define __TS_WRAP_KSTRTOX_H__

#include <linux/kernel.h>

int ts_kstrtoul(const char *s, unsigned int base, unsigned long *res);

int ts_kstrtoint(const char *s, unsigned int base, int *res);

#endif

