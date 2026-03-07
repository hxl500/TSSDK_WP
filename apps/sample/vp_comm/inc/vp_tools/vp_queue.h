/**
 * @file vp_queue.h
 * @brief 无锁队列封装,内部开辟内存进行数据拷贝
 * @author 杨柳
 * @date 2020.09.01
 */

#ifndef VP_TOOLS_VP_QUEUE_H
#define VP_TOOLS_VP_QUEUE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct vp_queue *vp_queue_p;

/**
 * 创建队列
 *
 * @param type_size [in] 类型占用字节数
 *
 * @return 队列指针
 * @retval NULL      创建失败
 * @retval 非NULL    创建成功
 */
vp_queue_p vp_queue_create(uint32_t type_size);

/**
 * 队列入列
 *
 * @param q     [in]    队列指针
 * @param item  [in]    插入元素指针
 *
 * @return 错误码
 * @retval 0      成功
 * @retval -1     失败
 */
int vp_queue_push(vp_queue_p q, void *item);

/**
 * 队列出列
 *
 * @param q     [in]    队列指针
 * @param item  [in]    弹出元素指针
 *
 * @return 弹出元素指针
 * @retval NULL      出列失败
 * @retval 非NULL    出列成功
 */
void *vp_queue_pull(vp_queue_p q, void *item);

/**
 * 队列是否为空
 *
 * @param q     [in]    队列指针
 *
 * @return 是否为空
 * @retval true      为空
 * @retval false     不为空
 */
bool vp_queue_empty(vp_queue_p q);

/**
 * 获取队列入列长度
 *
 * @param q     [in]    队列指针
 *
 * @return 队列长度
 */
uint32_t vp_queue_size(vp_queue_p q);

/**
 * 重置为空队列
 *
 * @param q     [in]    队列指针
 */
void vp_queue_reset(vp_queue_p q);

/**
 * 销毁队列
 *
 * @param q     [in]    队列指针
 */
void vp_queue_destroy(vp_queue_p q);


static inline int vp_array_push_int(vp_queue_p q, int value) {
    return vp_queue_push(q, &value);
}

static inline int vp_array_push_long(vp_queue_p q, long value) {
    return vp_queue_push(q, &value);
}

static inline int vp_array_push_uint64(vp_queue_p q, uint64_t value) {
    return vp_queue_push(q, &value);
}

static inline int vp_array_push_double(vp_queue_p q, double value) {
    return vp_queue_push(q, &value);
}

static inline int vp_array_push_char(vp_queue_p q, char value) {
    return vp_queue_push(q, &value);
}

static inline int vp_array_push_pointer(vp_queue_p q, void *value) {
    return vp_queue_push(q, &value);
}

static inline int vp_array_pull_int(vp_queue_p q) {
    int value = 0;
    vp_queue_pull(q, &value);
    return value;
}

static inline long vp_array_pull_long(vp_queue_p q) {
    long value = 0;
    vp_queue_pull(q, &value);
    return value;
}

static inline uint64_t vp_array_pull_uint64(vp_queue_p q) {
    uint64_t value = 0;
    vp_queue_pull(q, &value);
    return value;
}

static inline double vp_array_pull_double(vp_queue_p q) {
    double value = 0;
    vp_queue_pull(q, &value);
    return value;
}

static inline char vp_array_pull_char(vp_queue_p q) {
    char value = 0;
    vp_queue_pull(q, &value);
    return value;
}

static inline void *vp_array_pull_pointer(vp_queue_p q) {
    void *value = NULL;
    vp_queue_pull(q, &value);
    return value;
}

#endif //VP_TOOLS_VP_QUEUE_H
