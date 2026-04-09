#include <linux/module.h>
#include <soc/ts/wrap/wait.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/slab.h>

wait_queue_head_t *ts_alloc_wait_queue_head(void)
{
	return kzalloc(sizeof(struct wait_queue_head), GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(ts_alloc_wait_queue_head);

void ts_free_wait_queue_head(wait_queue_head_t *wq_head)
{
	return kfree(wq_head);
}
EXPORT_SYMBOL_GPL(ts_free_wait_queue_head);

void ts_init_waitqueue_head(wait_queue_head_t *wq_head)
{
	init_waitqueue_head(wq_head);
}
EXPORT_SYMBOL_GPL(ts_init_waitqueue_head);


/* alloc and initial */
wait_queue_head_t *ts_DECLARE_WAIT_QUEUE_HEAD(void)
{
	struct wait_queue_head *wq_head;
	int err = 0;

	wq_head = ts_alloc_wait_queue_head();
	if (!wq_head) {
		err = -1; goto err1;
	}

	spin_lock_init(&(wq_head->lock));
	INIT_LIST_HEAD(&(wq_head->head));

	return wq_head;

err1:
	pr_err("%s err %d\n", __func__, err);
	return NULL;
}
EXPORT_SYMBOL_GPL(ts_DECLARE_WAIT_QUEUE_HEAD);
