//
// Created by 杨柳 on 2024/7/29.
//

#ifndef VP_TOOLS_VP_ARRAY_H
#define VP_TOOLS_VP_ARRAY_H

#include <stdint.h>

typedef struct vp_array *vp_array_p;

/**
 * 创建数组
 *
 * @param type_size     [in]    元素大小
 * @param capacity      [in]    初始容量
 *
 * @return 数组指针
 * @retval NULL     创建失败
 * @retval !NULL    创建成功
 */
vp_array_p vp_array_create(uint32_t type_size, uint32_t capacity);

/**
 * 创建默认大小数组
 *
 * @param type_size     [in]    元素大小
 *
 * @return 数组指针
 * @retval NULL     创建失败
 * @retval !NULL    创建成功
 */
vp_array_p vp_array_create_default(uint32_t type_size);

/**
 * 获取数组大小
 *
 * @param array         [in]    数组指针
 * @return 数组大小
 */
uint32_t vp_array_size(vp_array_p array);

/**
 * 添加元素
 *
 * @param array         [in]    数组指针
 * @param item          [in]    元素指针
 *
 * @return 添加结果
 * @retval 0     添加成功
 * @retval -1    添加失败
 */
int vp_array_add(vp_array_p array, void *item);

/**
 * 插入元素到指定位置
 *
 * @param array          [in]    数组指针
 * @param index          [in]    插入位置
 * @param item           [in]    元素指针
 *
 * @return 插入结果
 * @retval 0     插入成功
 * @retval -1    插入失败
 */
int vp_array_insert(vp_array_p array, uint32_t index, void *item);

/**
 * 设置指定元素值
 *
 * @param array          [in]    数组指针
 * @param index          [in]    元素位置
 * @param item           [in]    元素指针
 *
 * @return 设置结果
 * @retval 0     设置成功
 * @retval -1    设置失败
 */
int vp_array_set(vp_array_p array, uint32_t index, void *item);

/**
 * 获取指定元素位于数组中的位置
 *
 * @param array          [in]    数组指针
 * @param item           [in]    元素指针
 *
 * @return 元素位置
 * @retval -1    元素不存在
 * @retval >=0   元素位置
 */
int64_t vp_array_item_index(vp_array_p array, void *item);

/**
 * 在数组中删除元素
 *
 * @param array          [in]    数组指针
 * @param item           [in]    元素指针
 *
 * @return 删除结果
 * @retval 0     删除成功
 * @retval -1    删除失败
 */
int vp_array_item_remove(vp_array_p array, void *item);

/**
 * 删除指定位置元素
 *
 * @param array          [in]    数组指针
 * @param index          [in]    元素位置
 *
 * @return 删除结果
 * @retval 0     删除成功
 * @retval -1    删除失败
 */
int vp_array_remove(vp_array_p array, uint32_t index);

/**
 * 获取指定位置的元素
 *
 * @param array          [in]    数组指针
 * @param index          [in]    元素位置
 * @param dst            [in]    元素存储指针
 *
 * @return 返回元素指针
 * @retval NULL     获取失败
 * @retval !NULL    元素指针
 */
void *vp_array_get(vp_array_p array, uint32_t index, void *dst);

/**
 * 重置数组
 *
 * @param array          [in]    数组指针
 */
void vp_array_reset(vp_array_p array);

/**
 * 销毁数组
 *
 * @param array          [in]    数组指针
 */
void vp_array_destroy(vp_array_p array);

static inline int vp_array_add_int(vp_array_p array, int value) {
    return vp_array_add(array, &value);
}

static inline int vp_array_add_long(vp_array_p array, long value) {
    return vp_array_add(array, &value);
}

static inline int vp_array_add_uint64(vp_array_p array, uint64_t value) {
    return vp_array_add(array, &value);
}

static inline int vp_array_add_double(vp_array_p array, double value) {
    return vp_array_add(array, &value);
}

static inline int vp_array_add_char(vp_array_p array, char value) {
    return vp_array_add(array, &value);
}

static inline int vp_array_add_pointer(vp_array_p array, void *value) {
    return vp_array_add(array, &value);
}

static inline int vp_array_insert_int(vp_array_p array, uint32_t index, int value) {
    return vp_array_insert(array, index, &value);
}

static inline int vp_array_insert_long(vp_array_p array, uint32_t index, long value) {
    return vp_array_insert(array, index, &value);
}

static inline int vp_array_insert_uint64(vp_array_p array, uint32_t index, uint64_t value) {
    return vp_array_insert(array, index, &value);
}

static inline int vp_array_insert_double(vp_array_p array, uint32_t index, double value) {
    return vp_array_insert(array, index, &value);
}

static inline int vp_array_insert_char(vp_array_p array, uint32_t index, char value) {
    return vp_array_insert(array, index, &value);
}

static inline int vp_array_insert_pointer(vp_array_p array, uint32_t index, void *value) {
    return vp_array_insert(array, index, &value);
}

static inline int vp_array_set_int(vp_array_p array, uint32_t index, int value) {
    return vp_array_set(array, index, &value);
}

static inline int vp_array_set_long(vp_array_p array, uint32_t index, long value) {
    return vp_array_set(array, index, &value);
}

static inline int vp_array_set_uint64(vp_array_p array, uint32_t index, uint64_t value) {
    return vp_array_set(array, index, &value);
}

static inline int vp_array_set_double(vp_array_p array, uint32_t index, double value) {
    return vp_array_set(array, index, &value);
}

static inline int vp_array_set_char(vp_array_p array, uint32_t index, char value) {
    return vp_array_set(array, index, &value);
}

static inline int vp_array_set_pointer(vp_array_p array, uint32_t index, void *value) {
    return vp_array_set(array, index, &value);
}

static inline int vp_array_get_int(vp_array_p array, uint32_t index) {
    int value = 0;
    vp_array_get(array, index, &value);
    return value;
}

static inline long vp_array_get_long(vp_array_p array, uint32_t index) {
    long value = 0;
    vp_array_get(array, index, &value);
    return value;
}

static inline uint64_t vp_array_get_uint64(vp_array_p array, uint32_t index) {
    uint64_t value = 0;
    vp_array_get(array, index, &value);
    return value;
}

static inline double vp_array_get_double(vp_array_p array, uint32_t index) {
    double value = 0;
    vp_array_get(array, index, &value);
    return value;
}

static inline char vp_array_get_char(vp_array_p array, uint32_t index) {
    char value = 0;
    vp_array_get(array, index, &value);
    return value;
}

static inline void *vp_array_get_pointer(vp_array_p array, uint32_t index) {
    void *value = NULL;
    vp_array_get(array, index, &value);
    return value;
}


#endif //VP_TOOLS_VP_ARRAY_H
