/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_UACCESS_H__
#define __TS_WRAP_UACCESS_H__

#include <linux/uaccess.h>


unsigned long ts_copy_from_user(void *to, const void __user *from, unsigned long n);
unsigned long ts_copy_to_user(void __user *to, const void *from, unsigned long n);
int ts_access_ok(unsigned long type, void *addr, unsigned long size);

#ifdef __aarch64__
void ts_uaccess_disable_not_uao(void);
void ts_uaccess_enable_not_uao(void);
#endif


#endif

