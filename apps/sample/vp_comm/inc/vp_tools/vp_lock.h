//
// Created by 杨柳 on 2023/9/7.
//

#ifndef VP_LOCK_H
#define VP_LOCK_H

#include <pthread.h>
#include <stdint.h>

typedef struct {
    volatile uint8_t flag;
    pthread_mutex_t lock;
} vp_lock_t;

/**
 * 初始化互斥锁
 *
 * @param lock [in] 锁地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_lock_init(vp_lock_t *lock);

/**
 * 加锁
 *
 * @param lock [in] 锁结构地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_lock(vp_lock_t *lock);

/**
 * 解锁
 *
 * @param lock [in] 锁结构地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_unlock(vp_lock_t *lock);

/**
 * 反初始化锁
 *
 * @param lock [in] 锁地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_lock_deinit(vp_lock_t *lock);

typedef struct {
    volatile uint8_t flag;
    pthread_mutex_t lock;
} vp_recursive_t;

/**
 * 初始化递归锁
 *
 * @param lock [in] 锁地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_recursive_init(vp_recursive_t *lock);

/**
 * 加锁
 *
 * @param lock [in] 锁结构地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_recursive_lock(vp_recursive_t *lock);

/**
 * 解锁
 *
 * @param lock [in] 锁结构地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_recursive_unlock(vp_recursive_t *lock);

/**
 * 反初始化锁
 *
 * @param lock [in] 锁地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_recursive_deinit(vp_recursive_t *lock);

typedef struct {
    volatile uint8_t flag;
#if defined(__linux__) && !defined(__ANDROID__)
    pthread_spinlock_t lock;
#else
    pthread_mutex_t lock;
#endif
} vp_spin_t;

/**
 * 初始化自旋锁
 *
 * @param lock [in] 锁地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_spin_init(vp_spin_t *lock);

/**
 * 加锁
 *
 * @param lock [in] 锁结构地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_spin_lock(vp_spin_t *lock);

/**
 * 解锁
 *
 * @param lock [in] 锁结构地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_spin_unlock(vp_spin_t *lock);

/**
 * 反初始化锁
 *
 * @param lock [in] 锁地址
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_spin_deinit(vp_spin_t *lock);


#endif //VP_LOCK_H
