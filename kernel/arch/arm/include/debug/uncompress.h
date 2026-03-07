/* SPDX-License-Identifier: GPL-2.0 */
#ifdef CONFIG_DEBUG_UNCOMPRESS
extern void putc(int c);
#else
#define UART0_BASE			0xF2449000UL

#define UART_DR   \
	(*(volatile unsigned char *)(UART0_BASE + 0x0))
#define UART_FR   \
	(*(volatile unsigned char *)(UART0_BASE + 0x14))

/*
 * This does not append a newline
 */
static inline void putc(int c)
{
	while ((UART_FR & 0x20) != 0x20)
		barrier();

	UART_DR = c;
}

static inline void flush(void)
{
	while ((UART_FR & 0x20) != 0x20)
		barrier();
}

/*
 * nothing to do
 */
#define arch_decomp_setup()
#define arch_decomp_wdog()
#endif

