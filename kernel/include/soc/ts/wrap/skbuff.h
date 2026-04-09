/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_SKBUFF_H__
#define __TS_WRAP_SKBUFF_H__

#include <linux/skbuff.h>

int ts_skbuff_get_len(struct sk_buff *skb);
struct nlmsghdr *ts_nlmsg_hdr(const struct sk_buff *skb);


#endif

