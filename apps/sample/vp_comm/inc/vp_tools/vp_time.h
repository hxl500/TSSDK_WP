//
// Created by 杨柳 on 2023/1/7.
//

#ifndef VP_FRAMEWORK_VP_TIME_H
#define VP_FRAMEWORK_VP_TIME_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

typedef struct {
    uint16_t year;
    uint16_t month;
    uint16_t day;
} vp_date_t;

/**
 * 将时间戳转换为日期格式
 *
 * @param timestamp     [in]    需要转换的时间戳,单位(秒)
 * @param date          [out]   输出转换后的日期结构体
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_make_date(uint64_t timestamp, vp_date_t *date);

typedef struct {
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
} vp_time_t;

/**
 * 将时间戳转换为时间格式
 *
 * @param timestamp     [in]    需要转换的时间戳,单位(秒)
 * @param time          [out]   输出转换后的时间结构体
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_make_time(uint64_t timestamp, vp_time_t *time);

typedef struct {
    uint16_t year;
    uint16_t month;
    uint16_t day;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
} vp_datetime_t;

/**
 * 将时间戳转换为日期和时间格式
 *
 * @param timestamp     [in]    需要转换的时间戳,单位(秒)
 * @param datetime      [out]   输出转换后的结构体
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_make_datetime(uint64_t timestamp, vp_datetime_t *datetime);

/**
 * 系统启动时间校准开始
 */
void vp_system_calibration_start();

/**
 * 系统启动时间校准结束
 */
void vp_system_calibration_stop();

/**
 * 同步UTC时间
 *
 * @param timestamp     [in]        需要同步的时间戳.单位(秒)
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_time_sync(uint64_t timestamp);

/**
 * 同步UTC时间
 *
 * @param timestamp     [in]
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_timespec_sync(struct timespec *time);

/**
 * 设置时区
 *
 * @param timezone [in] 偏移值,偏移秒数
 */
void vp_timezone_sync(int time_zone);

/**
 * 获取时区
 *
 * @return 时区
 */
int vp_timezone_get();

/**
 * 时间转换,将系统启动时间转换为UTC时间
 *
 * @param time     [in,out]    输入系统启动时间,输出对应UTC时间.单位秒(s)
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_time_system_convert(uint64_t *time);

/**
 * 时间转换,将系统启动时间转换为本地时间
 *
 * @param time     [in,out]    输入系统启动时间,输出对应本地时间.单位秒(s)
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_time_local_convert(uint64_t *time);

/**
 * 获取当前UTC时间
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_time(struct timespec *time);

/**
 * 获取本地时间
 *
 * @param time  [in]    时间结构体地址
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_local_time(struct timespec *time);

/**
 * 获取系统启动时间
 *
 * @param time  [in]    时间结构体地址
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_system_time(struct timespec *time);

/**
 * 获取当前UTC时间,单位秒(s)
 *
 * @return 当前时间
 */
uint64_t vp_time_s();

/**
 * 获取本地时间,单位秒(s)
 *
 * @return 当前时间
 */
uint64_t vp_local_time_s();

/**
 * 获取系统启动时间,单位秒(s)
 *
 * @return 系统启动时间
 */
uint64_t vp_system_time_s();

/**
 * 获取UTC时间,单位毫秒(ms)
 *
 * @return 当前时间
 */
uint64_t vp_time_ms();

/**
 * 获取本地时间,单位毫秒(ms)
 *
 * @return 当前时间
 */
uint64_t vp_local_time_ms();

/**
 * 获取系统启动时间,单位毫秒(ms)
 *
 * @return 系统启动时间
 */
uint64_t vp_system_time_ms();

/**
 * 获取UTC时间,单位微秒(us)
 *
 * @return 当前时间
 */
uint64_t vp_time_us();

/**
 * 获取本地时间,单位微秒(us)
 *
 * @return 当前时间
 */
uint64_t vp_local_time_us();

/**
 * 获取系统启动时间,单位微秒(us)
 *
 * @return 系统启动时间
 */
uint64_t vp_system_time_us();

#ifdef __cplusplus
}
#endif

#endif //VP_FRAMEWORK_VP_TIME_H
