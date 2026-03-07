/**
 * Created by 杨柳 on 2024/6/11.
 * @details Veepai 平台的原子操作封装
 * @note Windows 平台下实现的原子操作暂未经过测试, 请谨慎使用.
 */
#ifndef VP_TOOLS_VP_ATOMIC_H
#define VP_TOOLS_VP_ATOMIC_H

#include <stdbool.h>


#ifdef __GNUC__

#include <sched.h>

// 释放CPU占用,主动让出CPU的使用权,让其他线程有机会进行运行.
#define vp_sched_yield() sched_yield()

#else

#include <Windows.h>

// 释放CPU占用,主动让出CPU的使用权,让其他线程有机会进行运行.
#define vp_sched_yield() SwitchToThread()

#endif


#ifndef __STDC_NO_ATOMICS__

#include <stdatomic.h>

#define vp_atomic_bool atomic_bool

#define vp_atomic_char atomic_char
#define vp_atomic_uchar atomic_uchar

#define vp_atomic_short atomic_short
#define vp_atomic_ushort atomic_ushort

#define vp_atomic_int atomic_int
#define vp_atomic_uint atomic_uint

#define vp_atomic_long atomic_long
#define vp_atomic_ulong atomic_ulong

#define vp_atomic_llong atomic_llong
#define vp_atomic_ullong atomic_ullong

// 初始化PTR的值为value
#define vp_atomic_init(ptr, value) atomic_init(ptr,value)

// 将PTR的值进行加法操作,并返回加操作之前的值
#define vp_atomic_add(ptr, value) atomic_fetch_add(ptr, value)
// 将PTR的值进行加法操作,并返回加操作之后的值
#define vp_atomic_add_fetch(ptr, value) atomic_fetch_add(ptr, value) + (value)

// 将PTR的值进行减法操作,并返回减操作之前的值
#define vp_atomic_sub(ptr, value) atomic_fetch_sub(ptr, value)
// 将PTR的值进行减法操作,并返回减操作之后的值
#define vp_atomic_sub_fetch(ptr, value) atomic_fetch_sub(ptr, value) - (value)

// 将PTR的值进行比较操作,如果ptr的值等于expected,则将ptr的值设置为desired,并返回true,否则返回false,并将当前ptr的值赋值给expected
#define vp_atomic_compare(ptr, expected, desired) atomic_compare_exchange_weak(ptr, expected, desired)

// 获取PTR的值,不需要确保原子性
#define vp_atomic_value(ptr) *(ptr)
// 获取PTR的值
#define vp_atomic_get(ptr) atomic_load_explicit(ptr, memory_order_relaxed)
// 设置PTR的值
#define vp_atomic_set(ptr, value) atomic_store_explicit(ptr,value, memory_order_relaxed)

// 获取PTR的值,确保原子性
#define vp_atomic_load(ptr) atomic_load_explicit(ptr, memory_order_acquire)
// 将PTR的值进行赋值操作,并返回赋值之前的值
#define vp_atomic_store(ptr, value) atomic_store_explicit(ptr, value, memory_order_release)

#else

#ifdef __cplusplus
typedef volatile bool atomic_bool;
#else
typedef volatile _Bool atomic_bool;
#endif
typedef volatile char atomic_char;
typedef volatile unsigned char atomic_uchar;
typedef volatile short atomic_short;
typedef volatile unsigned short atomic_ushort;
typedef volatile int atomic_int;
typedef volatile unsigned int atomic_uint;
typedef volatile long atomic_long;
typedef volatile unsigned long atomic_ulong;
typedef volatile long long atomic_llong;
typedef volatile unsigned long long atomic_ullong;

#define vp_atomic_bool atomic_bool

#define vp_atomic_char atomic_char
#define vp_atomic_uchar atomic_uchar

#define vp_atomic_short atomic_short
#define vp_atomic_ushort atomic_ushort

#define vp_atomic_int atomic_int
#define vp_atomic_uint atomic_uint

#define vp_atomic_long atomic_long
#define vp_atomic_ulong atomic_ulong

#define vp_atomic_llong atomic_llong
#define vp_atomic_ullong atomic_ullong

#if defined(__GNUC__)

// 初始化PTR的值为value
#define vp_atomic_init(ptr, value) *(ptr) = (value)

// 将PTR的值进行加法操作,并返回加操作之前的值
#define vp_atomic_add(ptr, value) __sync_fetch_and_add(ptr, value)
// 将PTR的值进行加法操作,并返回加操作之后的值
#define vp_atomic_add_fetch(ptr, value) __sync_add_and_fetch(ptr, value)

// 将PTR的值进行减法操作,并返回减操作之前的值
#define vp_atomic_sub(ptr, value) __sync_fetch_and_sub(ptr, value)
// 将PTR的值进行减法操作,并返回减操作之后的值
#define vp_atomic_sub_fetch(ptr, value) __sync_sub_and_fetch(ptr, value)

// 将PTR的值进行比较操作,如果ptr的值等于expected,则将ptr的值设置为desired,并返回true,否则返回false,并将当前ptr的值赋值给expected
#define vp_atomic_compare(ptr, expected, desired)                                                           \
__extension__                                                                                               \
  ({                                                                                                        \
    bool ret = false;                                                                                       \
    vp_atomic_ullong __atomic_value;                                                                        \
    if((__atomic_value = __sync_val_compare_and_swap(ptr, *expected, desired)) == *expected) {              \
        ret = true;                                                                                         \
    }                                                                                                       \
    else {                                                                                                  \
        *expected = __atomic_value;                                                                         \
    }                                                                                                       \
    ret;                                                                                                    \
  })

// 获取PTR的值,不需要确保原子性
#define vp_atomic_value(ptr) *(ptr)
// 获取PTR的值
#define vp_atomic_get(ptr) __sync_fetch_and_add(ptr, 0)
// 设置PTR的值
#define vp_atomic_set(ptr, value) __sync_lock_test_and_set(ptr, value)

// 获取PTR的值,确保原子性
#define vp_atomic_load(ptr) __sync_fetch_and_add(ptr, 0)
// 将PTR的值进行赋值操作,并返回赋值之前的值
#define vp_atomic_store(ptr, value) __sync_lock_test_and_set(ptr, value)

#else

#include <Windows.h>

// 初始化PTR的值为value
#define vp_atomic_init(ptr, value) *(ptr) = (value)

// 获取PTR的值,不需要确保原子性
#define vp_atomic_value(ptr) *(ptr)

#ifdef _WIN64

// 将PTR的值进行加法操作,并返回加操作之前的值
#define vp_atomic_add(ptr, value) InterlockedAdd64((LONG64 volatile *)(ptr), (LONG64)(value))
// 将PTR的值进行加法操作,并返回加操作之后的值
#define vp_atomic_add_fetch(ptr, value) InterlockedAdd64((LONG64 volatile *)(ptr), (LONG64)(value)) + ((LONG64)(value))

// 将PTR的值进行减法操作,并返回减操作之前的值
#define vp_atomic_sub(ptr, value) InterlockedAdd64((LONG64 volatile *)(ptr), -((LONG64)(value)))
// 将PTR的值进行减法操作,并返回减操作之后的值
#define vp_atomic_sub_fetch(ptr, value) InterlockedAdd64((LONG64 volatile *)(ptr), -((LONG64)(value))) - ((LONG64)(value))

// 将PTR的值进行比较操作,如果ptr的值等于expected,则将ptr的值设置为desired,并返回true,否则返回false,并将当前ptr的值赋值给expected
static inline vp_atomic_compare(LONG64 volatile* ptr, LONG64 *expected, LONG64 desired) {
    bool ret = false;
    LONG64 __atomic_value = InterlockedCompareExchange64(ptr, desired, *expected);
    if (__atomic_value == *expected) {
        ret = true;
    }
    else {
        *expected = __atomic_value;
    }
    return ret;
}

// 获取PTR的值
#define vp_atomic_get(ptr) InterlockedAdd64((LONG64 volatile *)(ptr), 0)
// 设置PTR的值
#define vp_atomic_set(ptr, value) InterlockedExchange64((LONG64 volatile *)(ptr), (LONG64)(value))

// 获取PTR的值,确保原子性
#define vp_atomic_load(ptr) InterlockedAddAcquire64((LONG64 volatile *)(ptr), 0)
// 将PTR的值进行赋值操作,并返回赋值之前的值
#define vp_atomic_store(ptr, value) InterlockedExchangeAddRelease64((LONG64 volatile *)(ptr), (LONG64)(value))

#else

// 将PTR的值进行加法操作,并返回加操作之前的值
#define vp_atomic_add(ptr, value) InterlockedAdd((LONG volatile*)(ptr), value)
// 将PTR的值进行加法操作,并返回加操作之后的值
#define vp_atomic_add_fetch(ptr, value) InterlockedAdd((LONG volatile*)(ptr), value) + (value)

// 将PTR的值进行减法操作,并返回减操作之前的值
#define vp_atomic_sub(ptr, value) InterlockedAdd((LONG volatile*)(ptr), -(value))
// 将PTR的值进行减法操作,并返回减操作之后的值
#define vp_atomic_sub_fetch(ptr, value) InterlockedAdd((LONG volatile*)(ptr), -(value)) - (value)

// 将PTR的值进行比较操作,如果ptr的值等于expected,则将ptr的值设置为desired,并返回true,否则返回false,并将当前ptr的值赋值给expected
static inline vp_atomic_compare(LONG volatile* ptr, LONG* expected, LONG desired) {
    bool ret = false;
    LONG64 __atomic_value = InterlockedCompareExchange(ptr, desired, *expected);
    if (__atomic_value == *expected) {
        ret = true;
    }
    else {
        *expected = __atomic_value;
    }
    return ret;
}

// 获取PTR的值
#define vp_atomic_get(ptr) InterlockedAdd((LONG volatile *)(ptr), 0)
// 设置PTR的值
#define vp_atomic_set(ptr, value) InterlockedExchange((LONG volatile *)(ptr), (LONG)(value))

// 获取PTR的值,确保原子性
#define vp_atomic_load(ptr) InterlockedAddAcquire((LONG volatile*)(ptr), 0)
// 将PTR的值进行赋值操作,并返回赋值之前的值
#define vp_atomic_store(ptr, value) InterlockedExchangeAddRelease((LONG volatile*)(ptr), value)

#endif

#endif

#endif

#endif //VP_TOOLS_VP_ATOMIC_H
