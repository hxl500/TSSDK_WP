//
// Created by 杨柳 on 2023/11/15.
//

#ifndef VP_TOOLS_VP_MEMORY_H
#define VP_TOOLS_VP_MEMORY_H

#include <stddef.h>

typedef struct vp_memory *vp_memory_p;

/* 用于执行外部销毁 */
typedef void (*vp_memory_free_cb_f)(void *data);

/**
 * 创建内存管理对象
 *
 * @param size      [in]    内存大小
 * @param free_cb   [in]    释放回调函数
 *
 * @return 错误码
 * @retval 为0     成功
 * @retval 非0     失败
 */
vp_memory_p vp_memory_malloc(size_t size, vp_memory_free_cb_f free_cb);

/**
 * 申请内存
 *
 * @param memory    [in]    内存存储对象
 * @param data      [in]    内存数据
 * @param size      [in]    内存大小
 *
 * @return 错误码
 * @retval 0       成功
 * @retval 非0     失败
 */
vp_memory_p vp_memory_data(void *data, size_t size);

/**
 * 获取内存
 *
 * @param memory    [in]    内存存储对象
 *
 * @return 内存数据
 * @retval NULL     失败
 * @retval 非NULL   成功
 */
void *vp_memory_get(vp_memory_p *memory);

/**
 * 引用内存, 增加引用计数
 *
 * @param memory    [in,out]    内存管理对象
 *
 * @return 实际内存数据访问地址
 *
 * @retval 非NULL   成功
 * @retval 为NULL   失败
 */
void *vp_memory_ref(vp_memory_p *memory);

/**
 * 释放内存引用, 减少引用计数
 *
 * @param memory    [in,out]    内存存储对象
 */
void vp_memory_unref(vp_memory_p *memory);

/**
 * 释放内存
 *
 * @param memory    [in]    内存存储对象
 */
void vp_memory_free(vp_memory_p memory);


#endif //VP_TOOLS_VP_MEMORY_H
