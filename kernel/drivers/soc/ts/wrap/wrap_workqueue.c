#include <linux/module.h>
#include <soc/ts/wrap/workqueue.h>
#include <linux/slab.h>

wait_queue_head_t *ts_alloc_st_wq_head(void)
{
	return kzalloc(sizeof(wait_queue_head_t), GFP_KERNEL);
}
EXPORT_SYMBOL(ts_alloc_st_wq_head);


void ts_INIT_WORK(struct work_struct *work, work_func_t func)
{
	INIT_WORK(work, func);
}
EXPORT_SYMBOL(ts_INIT_WORK);

void ts_INIT_DELAYED_WORK(struct delayed_work *work, work_func_t func)
{
	INIT_DELAYED_WORK(work, func);
}
EXPORT_SYMBOL(ts_INIT_DELAYED_WORK);

bool ts_queue_work(struct workqueue_struct *wq,
			      struct work_struct *work)
{
	return queue_work(wq, work);
}
EXPORT_SYMBOL(ts_queue_work);


bool ts_queue_delayed_work(struct workqueue_struct *wq,
				      struct delayed_work *dwork,
				      unsigned long delay)
{
	return queue_delayed_work(wq, dwork, delay);
}
EXPORT_SYMBOL(ts_queue_delayed_work);


bool ts_mod_delayed_work(struct workqueue_struct *wq,
				    struct delayed_work *dwork,
				    unsigned long delay)
{
	return mod_delayed_work(wq, dwork, delay);
}
EXPORT_SYMBOL(ts_mod_delayed_work);


bool ts_schedule_work(struct work_struct *work)
{
	return schedule_work(work);
}
EXPORT_SYMBOL(ts_schedule_work);

bool ts_schedule_delayed_work(struct delayed_work *dwork,
					 unsigned long delay)
{
	return schedule_delayed_work(dwork, delay);
}
EXPORT_SYMBOL(ts_schedule_delayed_work);


struct workqueue_struct *ts_create_singlethread_workqueue(char *name)
{
	return create_singlethread_workqueue(name);
}
EXPORT_SYMBOL(ts_create_singlethread_workqueue);

