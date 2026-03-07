#include <linux/module.h>
#include <soc/ts/wrap/kfifo.h>
void ts_kfifo_in_spinlocked(kfifo_wrap* kfifo_t, unsigned int *buf, unsigned int size,void *lock)
{
	kfifo_in_spinlocked(kfifo_t,buf,size,lock);
}
EXPORT_SYMBOL(ts_kfifo_in_spinlocked);

int ts_kfifo_out_spinlocked(kfifo_wrap* kfifo_t, unsigned int *buf, unsigned int size,void *lock)
{
    int res = 0;
	res = kfifo_out_spinlocked(kfifo_t,buf,size,lock);
	return res;
}
EXPORT_SYMBOL(ts_kfifo_out_spinlocked);



int  ts_kfifo_alloc(kfifo_wrap* kfifo_t,unsigned int size,gfp_t gfp_mask)
{
	int res = 0;
	res = kfifo_alloc(kfifo_t,size,gfp_mask);
	return res;

}
EXPORT_SYMBOL(ts_kfifo_alloc);

void  ts_kfifo_free(kfifo_wrap* kfifo_t)
{
	 kfifo_free(kfifo_t);

}
EXPORT_SYMBOL(ts_kfifo_free);


int  ts_kfifo_len(kfifo_wrap* kfifo_t)
{
	return kfifo_len(kfifo_t);

}
EXPORT_SYMBOL(ts_kfifo_len);



void  ts_kfifo_reset(kfifo_wrap* kfifo_t)
{
	kfifo_reset(kfifo_t);

}
EXPORT_SYMBOL(ts_kfifo_reset);

int ts_kfifo_is_full(kfifo_wrap* kfifo_t)
{
	return kfifo_is_full(kfifo_t);

}
EXPORT_SYMBOL(ts_kfifo_is_full);

