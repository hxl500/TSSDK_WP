#include <linux/module.h>
#include <soc/ts/wrap/list.h>

void ts_INIT_LIST_HEAD(struct list_head *list)
{
	INIT_LIST_HEAD(list);
}
EXPORT_SYMBOL(ts_INIT_LIST_HEAD);

void ts_list_add(struct list_head *new, struct list_head *head)
{
	list_add(new, head);
}
EXPORT_SYMBOL(ts_list_add);

void ts_list_add_tail(struct list_head *new, struct list_head *head)
{
	list_add_tail(new, head);
}
EXPORT_SYMBOL(ts_list_add_tail);


void ts_list_del(struct list_head *entry)
{
	list_del(entry);
}
EXPORT_SYMBOL(ts_list_del);


void ts_list_del_init(struct list_head *entry)
{
	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}
EXPORT_SYMBOL(ts_list_del_init);


int ts_list_empty(const struct list_head *head)
{
	return list_empty(head);
}
EXPORT_SYMBOL(ts_list_empty);



int ts_hlist_empty(const struct hlist_head *head)
{
	return hlist_empty(head);
}
EXPORT_SYMBOL(ts_hlist_empty);


// list_for_each_entry_safe????
// list_last_entry
// list_first_entry
