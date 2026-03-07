/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_DEBUG_H__
#define __TS_WRAP_DEBUG_H__

#include <linux/printk.h>

/*
 * e.g
 *	#define pr_fmt(fmt) "xxxx"fmt	// before include this file
 *
 *	#include <xxx.h>
 *	...
 *	#include <soc/ts/wrap/debug.h>  // last include file
 *
 */

#ifdef pr_debug
#undef pr_debug
#endif

#if defined(DEBUG)
#define pr_debug(fmt, ...) \
	printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...) \
	no_printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#endif



#ifdef dev_dbg
#undef dev_dbg
#endif

#if defined(DEBUG)
#define dev_dbg(dev, format, arg...)		\
	dev_printk(KERN_DEBUG, dev, format, ##arg)
#else
#define dev_dbg(dev, format, arg...)				\
({								\
	if (0)							\
		dev_printk(KERN_DEBUG, dev, format, ##arg);	\
})
#endif

#endif

