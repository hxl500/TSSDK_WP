/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_DELAY_H__
#define __TS_WRAP_DELAY_H__

#include <linux/delay.h>

void ts_udelay(unsigned long us);
void ts_mdelay(unsigned long us);


#endif

