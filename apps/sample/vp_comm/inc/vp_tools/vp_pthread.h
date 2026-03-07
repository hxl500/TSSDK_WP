//
// Created by 杨柳 on 2022/12/30.
//

#ifndef VP_FRAMEWORK_VP_PTHREAD_H
#define VP_FRAMEWORK_VP_PTHREAD_H

#include <pthread.h>
#include <stdint.h>

/**
 * 创建指定栈大小的线程
 *
 * @param pthread       [out]   输出线程句柄
 * @param stack_size    [in]    栈大小,值为0将使用默认大小8192.单位字节,
 * @param func          [in]    线程执行方法
 * @param args          [in]    线程执行参数
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_pthread_create(pthread_t *pthread, uint32_t stack_size, void *(func)(void *), void *args);

/**
 * 设置当前线程名称
 *
 * @param name [in] 线程名称
 */
void vp_pthread_name(const char *name);

typedef struct vp_thpool *vp_thpool_p;

/**
 * 创建线程池
 *
 * @param stack_size    [in]    线程栈大小
 * @param thread_num    [in]    线程数量
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
vp_thpool_p vp_thpool_create(uint32_t stack_size, uint32_t thread_num);

/**
 * 获取线程池中创建线程数量
 *
 * @param pool [in] 线程池句柄
 *
 * @return 错误码或线程数量
 * @retval  >=0 成功
 * @retval !0 失败
 */
int vp_thpool_count(vp_thpool_p pool);

/**
 * 向线程池中添加线程
 *
 * @param pool [in] 线程池句柄
 * @param func [in] 线程执行方法
 * @param args [in] 线程执行参数
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_thpool_add_work(vp_thpool_p pool, void (func)(void *), void *args);

/**
 * 获取线程池中工作线程数量
 *
 * @param pool [in] 线程池句柄
 *
 * @return 错误码或工作线程数量
 * @retval  >=0 成功
 * @retval !0 失败
 */
int vp_thpool_work_count(vp_thpool_p pool);

/**
 * 等待线程池中线程执行完成
 *
 * @param pool [in] 线程池句柄
 */
void vp_thpool_wait(vp_thpool_p pool);

/**
 * 销毁线程池
 *
 * @param pool [in] 线程池句柄
 */
void vp_thpool_destroy(vp_thpool_p pool);


#endif //VP_FRAMEWORK_VP_PTHREAD_H
