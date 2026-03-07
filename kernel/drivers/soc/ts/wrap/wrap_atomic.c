#include <linux/module.h>
#include <soc/ts/wrap/atomic.h>


void ts_atomic_set(atomic_t *v, int i)
{
	atomic_set(v, i);
}
EXPORT_SYMBOL(ts_atomic_set);

void ts_atomic_add(int i, atomic_t *v)
{
	atomic_add(i, v);
}
EXPORT_SYMBOL(ts_atomic_add);

void ts_atomic_sub(int i, atomic_t *v)
{
	atomic_sub(i, v);
}
EXPORT_SYMBOL(ts_atomic_sub);


void ts_atomic_inc(atomic_t *v)
{
	atomic_inc(v);
}
EXPORT_SYMBOL(ts_atomic_inc);

void ts_atomic_dec(atomic_t *v)
{
	atomic_dec(v);
}
EXPORT_SYMBOL(ts_atomic_dec);


int ts_atomic_read(atomic_t *v)
{
	return atomic_read(v);
}
EXPORT_SYMBOL(ts_atomic_read);

int ts_atomic_inc_return(atomic_t *v)
{
	return atomic_inc_return(v);
}
EXPORT_SYMBOL(ts_atomic_inc_return);

int ts_atomic_dec_return(atomic_t *v)
{
	return atomic_dec_return(v);
}
EXPORT_SYMBOL(ts_atomic_dec_return);


int ts_atomic_dec_and_test(atomic_t *v)
{
	return atomic_dec_and_test(v);
}
EXPORT_SYMBOL(ts_atomic_dec_and_test);

int ts_atomic_xchg(atomic_t *v, int new)
{
	return atomic_xchg(v, new);
}
EXPORT_SYMBOL(ts_atomic_xchg);

int ts_atomic_cmpxchg(atomic_t *v, int old, int new)
{
	return atomic_cmpxchg(v, old, new);
}
EXPORT_SYMBOL(ts_atomic_cmpxchg);



void ts_atomic64_set(atomic64_t *v, long long i)
{
	atomic64_set(v, i);
}
EXPORT_SYMBOL(ts_atomic64_set);

int ts_atomic64_inc_return(atomic64_t *v)
{
	return atomic64_inc_return(v);
}
EXPORT_SYMBOL(ts_atomic64_inc_return);

