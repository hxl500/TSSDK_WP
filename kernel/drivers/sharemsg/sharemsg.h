/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_SHAREMSG_H__
#define __TS_SHAREMSG_H__

#ifdef __KERNEL__
#include <linux/time.h>		/* replace to <sys/time.h> in user space */
#else
#include <sys/time.h>
#endif

#define BYTE_PER_LINE 			(4)
#define BARE_PARA_MAGIC			0x4250524D   // BPRM

struct sharemsg_mem_dev {
	struct device *dev_p;
	u8 verson;
	uint32_t mem_len;
	uint32_t phy_addr;
	uint32_t fbs_num;
	uint32_t fbs_maxlen;
	uint32_t det_chnnum;
};

struct sharemsg_info {
	u8 version;
};

#endif
