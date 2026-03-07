#ifndef VP_EVENTS_H
#define VP_EVENTS_H

#include <stdint.h>
#include <time.h>

typedef struct vp_events *vp_events_p;

#define VP_EVENT_BIT(x) (1LL << (x))
#define VP_EVENT_BIT0  VP_EVENT_BIT(0)
#define VP_EVENT_BIT1  VP_EVENT_BIT(1)
#define VP_EVENT_BIT4  VP_EVENT_BIT(4)
#define VP_EVENT_BIT2  VP_EVENT_BIT(2)
#define VP_EVENT_BIT3  VP_EVENT_BIT(3)
#define VP_EVENT_BIT5  VP_EVENT_BIT(5)
#define VP_EVENT_BIT6  VP_EVENT_BIT(6)
#define VP_EVENT_BIT7  VP_EVENT_BIT(7)

#define VP_EVENTS_ALL UINT64_MAX

typedef enum {
    VP_EVENTS_FLAG_AND = (1 << 0),                      /* 等待所有事件标识位都完成,不能和VP_EVENTS_FLAG_OR同时使用 */
    VP_EVENTS_FLAG_OR = (1 << 1),                       /* 等待任意一个事件标识位完成,不能和VP_EVENTS_FLAG_AND同时使用 */
    VP_EVENTS_FLAG_CLEAR = (1 << 2),                    /* 标识位触发后进行清理 */
    VP_EVENTS_FLAG_AND_CLEAR =
    VP_EVENTS_FLAG_AND | VP_EVENTS_FLAG_CLEAR,          /* VP_EVENTS_FLAG_AND | VP_EVENTS_FLAG_CLEAR */
    VP_EVENTS_FLAG_OR_CLEAR =
    VP_EVENTS_FLAG_OR | VP_EVENTS_FLAG_CLEAR,           /* VP_EVENTS_FLAG_OR | VP_EVENTS_FLAG_CLEAR */
} vp_events_flags_t;


/**
 * 创建事件管理器
 *
 * @return 事件指针,为空则视为失败
 */
vp_events_p vp_events_create(void);

/**
 * 获取用于事件定时的时间
 *
 * @param spec  [in]    时间结构体地址
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_events_time(struct timespec *spec);

/**
 * 等待相应事件位完成
 *
 * @param events    [in]     事件指针
 * @param bits      [in,out] 需要等待的事件标识位
 * @param flags     [in]     事件等待选项
 * @param timeout   [in]     等待超时时间(ms) 0为一直等待
 *
 * @return 等待响应码
 *
 * @retval 0    成功
 * @retval -1   失败
 * @retval -2   超时
 */
int vp_events_wait(vp_events_p events, uint64_t *bits, vp_events_flags_t flags, uint32_t timeout);

/**
 * 等待相应事件位完成
 *
 * @param events    [in]     事件指针
 * @param bits      [in,out] 需要等待的事件标识位
 * @param flags     [in]     事件等待选项
 * @param time      [in]     等待到指定时间
 *                           使用`vp_events_time`获取用于事件定时的初始时间
 *
 * @return 等待响应码
 *
 * @retval 0    成功
 * @retval -1   失败
 * @retval -2   超时
 */
int vp_events_wait_time(vp_events_p events, uint64_t *bits, vp_events_flags_t flags, struct timespec *time);

/**
 * 清除事件标识位
 *
 * @param events    [in]    事件指针
 * @param bits      [in]    需要清除的事件标识位 使用 VP_EVENTS_ALL 清除全部
 *
 * @return int
 *
 * @retval 0    成功
 * @retval -1   失败
 */
int vp_events_clear(vp_events_p events, uint64_t bits);

/**
 * 发送事件标识位
 *
 * @param events    [in]    事件指针
 * @param bits      [in]    需要发送的事件标识位 不能使用 VP_EVENTS_ALL 发送全部
 *
 * @return 发送响应码
 *
 * @retval 0    成功
 * @retval -1   失败
 */
int vp_events_send(vp_events_p events, uint64_t bits);

/**
 * 等待相应事件值完成
 *
 * @param events    [in]     事件指针
 * @param value     [in]     需要等待的事件值
 * @param flags     [in]     事件等待选项
 * @param timeout   [in]     等待超时时间(ms) 0为一直等待
 * @param args      [out]    输出参数由`vp_events_send_value`传入
 * @return 等待响应码
 *
 * @retval 0    成功
 * @retval -1   失败
 * @retval -2   超时
 */
int vp_events_wait_value(vp_events_p events, uint64_t value, uint32_t timeout, void **args);

/**
 * 发送事件值
 *
 * @param events    [in]   事件指针
 * @param value     [in]   需要发送的事件值,值不能使用`VP_EVENTS_ALL`
 * @param args      [in]   事件参数
 *
 * @return 发送响应码
 *
 * @retval 0    成功
 * @retval -1   失败
 */
int vp_events_send_value(vp_events_p events, uint64_t value, void *args);

/**
 * 销毁事件
 *
 * @param events    [in]    事件指针
 */
void vp_events_destroy(vp_events_p events);

#endif
    