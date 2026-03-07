/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     io.h
 * @brief    Header File for register bits operation
 * @version  V1.0
 * @date     26. February 2019
 ******************************************************************************/


#ifndef _IO_H_
#define _IO_H_

#define BIT(n)                  (1UL << (n))
#define BIT_MASK(n)             (BIT(n) - 1)
#ifdef __cplusplus
extern "C" {
#endif


/* bit field operate*/
#define REG64(addr)        (*(volatile uint64_t *)(addr))
#define REG32(addr)        (*(volatile uint32_t *)(addr))
#define REG16(addr)        (*(volatile uint16_t *)(addr))
#define REG8(addr)         (*(volatile uint8_t *)(addr))

/** insert value to some field in reg,other field is set to 0(the Field MaKe macro) */
#define HAL_FMK(PER_REG_FIELD, val)                                         \
    (((val) << PER_REG_FIELD##_SHIFT) & PER_REG_FIELD##_MASK)

/* get value of some field in reg(the Field EXTract macro) */
#define HAL_FEXT(reg, PER_REG_FIELD)                                        \
    (((reg) & PER_REG_FIELD##_MASK) >> PER_REG_FIELD##_SHIFT)

/* insert value to some field in reg,other field don't change(the Field INSert macro) */
#define HAL_FINS(reg, PER_REG_FIELD, val)                                   \
    ((reg) = ((reg) & ~PER_REG_FIELD##_MASK)                                \
             | HAL_FMK(PER_REG_FIELD, val))


/* bit operate */
/* set one value to 1,other bit don't change*/
#define HAL_BIT_SET(reg, bit) ((reg) = ((reg) | (1u << (bit))))

/* set one value to 0,other bit don't change*/
#define HAL_BIT_CLR(reg, bit) ((reg) = ((reg) & (~(1u << (bit)))))

/* get value of one bit(0/1) */
#define HAL_GET_BIT_VAL(reg, bit) (((reg)>> (bit)) & 1u)

/*judge one bit is 1 or not */
#define HAL_IS_BIT_SET(reg, pos) (((reg) & (1u << (pos))) != 0x0u)

/* judge one bit is 0 or not */
#define HAL_IS_BIT_CLR(reg, pos) (((reg) & (1u << (pos))) == 0x0u)

/* set one value to bit,other bit don't change*/
#define HAL_BIT_INSR(reg, bit, val)                                       \
    ((reg) = (((reg) & (~(1u << (bit)))) | (((val) & 1u) << (bit))))


static inline uint8_t getreg8(volatile uint32_t *addr)
{
    return *(volatile uint8_t *)addr;
}

static inline void putreg8(uint8_t val, volatile uint32_t *addr)
{
    *(volatile uint8_t *)addr = val;
}

static inline uint32_t getreg32(volatile uint32_t *addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void putreg32(uint32_t val, volatile uint32_t *addr)
{
    *(volatile uint32_t *)addr = val;
}

static inline uint32_t inl(uint32_t addr)
{
    return *(volatile uint32_t *)(uintptr_t)addr;
}

static inline void outl(uint32_t val, uint32_t addr)
{
    *(volatile uint32_t *)(uintptr_t)addr = val;
}

static inline void set_bit(int nr, volatile uint32_t *addr)
{
    *addr |= BIT(nr);
}

static inline void clear_bit(int nr, volatile uint32_t *addr)
{
    *addr &= ~BIT(nr);
}

static inline void set_bit_mask(int mask, volatile uint32_t *addr)
{
    *addr |= mask;
}

static inline void clear_bit_mask(int mask, volatile uint32_t *addr)
{
    *addr &= ~mask;
}

static inline void clear_set_bit_mask(int mask0, int mask1, volatile uint32_t *addr)
{
    *addr &= ~mask0;
    *addr |= mask1;
}

static inline void write_bit(int nr, volatile uint32_t *addr, int val)
{
    uint32_t temp = *addr;

    temp = val ? (temp | BIT(nr)) : (temp & ~BIT(nr));

    *addr = temp;
}

static inline int test_bit(int nr, const volatile uint64_t *addr)
{
    return (*addr >> nr) & 0x1;
}

static inline int read_bits(int width, int nr, const volatile uint32_t *addr)
{
    return (*addr >> nr) & BIT_MASK(width);
}

static inline void write_bits(int width, int nr, volatile uint32_t *addr, int val)
{
    uint32_t temp = *addr;

    temp &= ~(BIT_MASK(width) << nr);
    temp |= (val << nr);

    *addr = temp;
}
#if 0
static inline uint32_t readl(uintptr_t addr)
{
	return *((volatile uint32_t *)(addr));
}

static inline void writel(uint32_t val, uintptr_t addr)
{
	*((volatile uint32_t *)(addr)) = val;
}
#endif
#define writeb(b, addr) (void)((*(volatile unsigned char *)(addr)) = (b))
#define writew(b, addr) (void)((*(volatile unsigned short *)(addr)) = (b))
#define writel(b, addr) (void)((*(volatile unsigned int *)(addr)) = (b))

#define readb(addr) (*(volatile unsigned char *)(uintptr_t)(addr))
#define readw(addr) (*(volatile unsigned short *)(uintptr_t)(addr))
#define readl(addr) (*(volatile unsigned int *)(uintptr_t)(addr))

/**
 * upper_32_bits - return bits 32-63 of a number
 * @n: the number we're accessing
 *
 * A basic shift-right of a 64- or 32-bit quantity.  Use this to suppress
 * the "right shift count >= width of type" warning when that quantity is
 * 32-bits.
 */
#define upper_32_bits(n) ((unsigned int)(((n) >> 16) >> 16))

/**
 * lower_32_bits - return bits 0-31 of a number
 * @n: the number we're accessing
 */
#define lower_32_bits(n) ((unsigned int)(n))

/*
 * abs() handles unsigned and signed longs, ints, shorts and chars.  For all
 * input types abs() returns a signed long.
 * abs() should not be used for 64-bit types (s64, u64, long long) - use abs64()
 * for those.
 */

#ifdef __cplusplus
}
#endif

#endif /* _IO_H_ */

