#include <linux/module.h>
#include <soc/ts/wrap/netlink.h>


int ts_nlmsg_total_size(int payload)
{
	return nlmsg_total_size(payload);
}
EXPORT_SYMBOL(ts_nlmsg_total_size);

struct nlmsghdr *ts_nlmsg_hdr(const struct sk_buff *skb)
{
	return nlmsg_hdr(skb);
}
EXPORT_SYMBOL(ts_nlmsg_hdr);

struct sk_buff *ts_nlmsg_new(size_t payload, gfp_t flags)
{
	return nlmsg_new(payload, flags);
}
EXPORT_SYMBOL(ts_nlmsg_new);

struct nlmsghdr *ts_nlmsg_put(struct sk_buff *skb, u32 portid, u32 seq,
			int type, int payload, int flags)
{
	return nlmsg_put(skb, portid, seq, type, payload, flags);
}
EXPORT_SYMBOL(ts_nlmsg_put);

