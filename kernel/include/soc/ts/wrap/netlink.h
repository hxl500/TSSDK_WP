/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_NETLINK_H__
#define __TS_WRAP_NETLINK_H__

#include <net/netlink.h>

int ts_nlmsg_total_size(int payload);
struct nlmsghdr *ts_nlmsg_hdr(const struct sk_buff *skb);
struct sk_buff *ts_nlmsg_new(size_t payload, gfp_t flags);
struct nlmsghdr *ts_nlmsg_put(struct sk_buff *skb, u32 portid, u32 seq,
			int type, int payload, int flags);

#endif

