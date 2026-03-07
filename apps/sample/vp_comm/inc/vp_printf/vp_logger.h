//
// Created by 杨柳 on 2024/11/12.
//

#ifndef VP_PRINTF_VP_LOGGER_H
#define VP_PRINTF_VP_LOGGER_H

#include <stdint.h>
#include <time.h>

typedef enum {
    VP_LOGGER_TYPE_APP,     // 应用日志
    VP_LOGGER_TYPE_SYS,     // 系统日志
    VP_LOGGER_TYPE_KERNEL,  // 内核日志
    VP_LOGGER_TYPE_ALL,     // 全部日志
} vp_logger_type_t;

typedef struct vp_logger *vp_logger_p;

/**
 * 日志回调函数
 */
typedef void (*vp_logger_callback_f)(vp_logger_p logger, vp_logger_type_t type, struct timespec *ts,
                                     const char *data, uint32_t len, void *user_dta);

/**
 * 打开日志
 *
 * @param type      [in]    日志类型
 * @param callback  [in] 日志回调函数
 * @param user_data [in]  用户数据
 *
 * @return  日志句柄
 * @retval  =NULL      打开失败
 * @retval  !NULL      日志句柄
 */
vp_logger_p vp_logger_open(vp_logger_type_t type, vp_logger_callback_f callback, void *user_data);

/**
 * 关闭日志
 *
 * @param logger [in] 日志句柄
 */
void vp_logger_close(vp_logger_p logger);

#endif //VP_PRINTF_VP_LOGGER_H
