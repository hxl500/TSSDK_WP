//
// Created by 杨柳 on 2024/5/7.
//

#ifndef VP_CONTEXT_VP_UEVENT_H
#define VP_CONTEXT_VP_UEVENT_H


/**
 * 系统事件监听回调函数
 *
 * @param msgs  事件消息行集合
 * @param count 事件消息行数量,最大64行消息,如果行数量为64则表明可能存在数据未读取完
 * @param args  用户数据
 */
typedef void (*vp_uevent_callback_f)(const char **msgs, int count, void *args);

/**
 * 开启系统事件监听
 *
 * @note SDK启动后自行调用,一般不用用户调用
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_uevent_start();

/**
 * 注册系统事件监听
 *
 * @param callback 回调函数
 * @param args 回调函数参数
 *
 * @return 监听句柄
 * @retval 非空 成功
 * @retval 为空 失败
 */
void *vp_uevent_register(vp_uevent_callback_f callback, void *args);

/**
 * 注销系统事件监听
 *
 * @param handle    [in]    监听句柄
 */
void vp_uevent_unregister(void *handle);

/**
 * 停止系统事件监听
 *
 * @note SDK停止时自行调用,一般不用用户调用
 */
void vp_uevent_stop();

#endif //VP_CONTEXT_VP_UEVENT_H
