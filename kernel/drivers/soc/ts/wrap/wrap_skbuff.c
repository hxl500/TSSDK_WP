#include <linux/module.h>
#include <soc/ts/wrap/skbuff.h>


int ts_skbuff_get_len(struct sk_buff *skb)
{
	return skb->len;
}
EXPORT_SYMBOL(ts_skbuff_get_len);


