
#include <common.h>
#include <config.h>

#define TIMER_COMP_VAL 0xffffffffffffffffull
#define ARCH_TIMER_CTRL_ENABLE (1 << 0)

unsigned int tbu;
unsigned int tbl;

int timer_init(void)
{
	unsigned long ctrl, freq;
	unsigned long long val;

	freq = CONFIG_SYS_HZ_CLOCK;//COUNTER_FREQUENCY;
	asm("mcr p15, 0, %0, c14, c0, 0" : : "r" (freq));

	/* Set PL1 Physical Timer Ctrl */
	ctrl = ARCH_TIMER_CTRL_ENABLE;
	asm("mcr p15, 0, %0, c14, c2, 1" : : "r" (ctrl));

	/* Set PL1 Physical Comp Value */
	val = TIMER_COMP_VAL;
	asm("mcrr p15, 2, %Q0, %R0, c14" : : "r" (val));

	tbl = 0;
	tbu = 0;
	//printf("timer init done\r\n");
	pr_debug("timer init done\r\n");

	return 0;
}

/* get CNTPCT
 MRRC p15, 0, <Rt>, <Rt2>, c14 ;
 Read 64-bit CNTPCT into Rt (low word) and Rt2 (high word)*/
unsigned long long get_ticks(void)
{
	unsigned long long now;

	asm("mrrc p15, 0, %Q0, %R0, c14" : "=r" (now));

	tbl = (unsigned long)(now & 0xffffffff);
	tbu = (unsigned long)(now >> 32);
	//printf("tbl:%x\r\n", tbl);
	//printf("tbu:%x\r\n", tbu);

	return now;
}

ulong get_tbclk(void)
{
	unsigned int frq;

	asm volatile("mrc p15, 0, %0, c14, c0, 0" : "=r" (frq));
	//printf("frq:%x\r\n", frq);
	return frq;
}
