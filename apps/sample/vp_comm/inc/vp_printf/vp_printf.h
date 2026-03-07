#ifndef VP_PRINTF_H
#define VP_PRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vp_printf_config.h"
#include <string.h>
#include "stddef.h"
#include <unistd.h>
#include "vp_context.h"

typedef enum {
    vp_printf_level_none = 0,   /* 不输出日志到控制台 */
    vp_printf_level_error = 1,  /* 仅允许Error的信息输出到控制台 */
    vp_printf_level_warn = 2,   /* 允许Warn和小于Warn的信息输出到控制台 */
    vp_printf_level_info = 3,   /* 允许Info和小于Info的信息输出到控制台 */
    vp_printf_level_debug = 4,  /* 允许Debug和小于Debug的信息输出到控制台 */
    vp_printf_level_all,        /* 允许所有的信息输出到控制台*/
} vp_printf_level_t;

#if VP_PRINTF_ENABLE

#ifndef __FILE_NAME__
#define filename(x) strrchr(x,'/')?strrchr(x,'/')+1:x
#define __FILE_NAME__ filename(__FILE__)
#endif

/**
 * 初始化打印模块
 *
 * @note 一般情况不需要用户进行调用,当启用打印时SDK会自行进行初始化
 */
void vp_printf_init();

/**
 * 设置允许日志输出级别
 *
 * @param level [in] 允许输出的日志级别,小于等于该级别的日志允许输出
 */
void vp_printf_set_level(vp_printf_level_t level);

/**
 * 获取日志输出级别
 *
 * @return 当前配置的日志输出级别
 */
vp_printf_level_t vp_printf_get_level(void);

/**
 * 日志打印输出
 * @param tag   [in] 标记
 * @param level [in] 日志级别
 * @param color [in] 颜色代码
 * @param file  [in] 文件名
 * @param line  [in] 行号
 * @param func  [in] 函数名
 * @param fmt   [in] 输出格式化字符串
 * @param ...   [in] 参数
 */
__attribute__((__format__ (__printf__, 7, 8))) void
vp_printf(const char *tag, vp_printf_level_t level, const char *color, const char *file,
          size_t line, const char *func, const char *fmt, ...);

/**
 * 日志打印输出
 * @param level 日志级别
 * @param msg   输出格式化字符串
 */
void vp_print(vp_printf_level_t level, const char *msg);

/**
 * 打印二进制数据
 *
 * @param title [in] 标签
 * @param level [in] 日志级别
 * @param data  [in] 数据源
 * @param len   [in] 数据长度
 * @param file  [in] 文件名
 * @param line  [in] 行号
 * @param func  [in] 函数名
 */
void vp_print_hex(const char *title, vp_printf_level_t level, uint8_t *data, size_t len, const char *file,
                  size_t line, const char *func);

/**
 * 打印应用内存使用情况
 *
 * @param title [in] 标签
 * @param level [in] 日志级别
 * @param file  [in] 文件名
 * @param line  [in] 行号
 * @param func  [in] 函数名
 */
void vp_print_ram(const char *title, vp_printf_level_t level, const char *file, size_t line, const char *func);

/**
 * 反初始化打印模块
 */
void vp_printf_deinit();

#define vp_hex(title, data, len) vp_print_hex(title,VP_PRINTF_LEVEL_INFO,data,len,__FILE_NAME__,__LINE__,__FUNCTION__)

#define vp_ram(title) vp_print_ram(title,VP_PRINTF_LEVEL_INFO,__FILE_NAME__,__LINE__,__FUNCTION__)

#if VP_PRINTF_LEVEL >= VP_PRINTF_LEVEL_STACK
#define vp_stack(fmt, ...) vp_printf("STACK",VP_PRINTF_LEVEL_DEBUG,"\e[1;32m",__FILE_NAME__, __LINE__, __FUNCTION__,fmt,##__VA_ARGS__)
#else
#define vp_stack(fmt, ...)
#endif

#if VP_PRINTF_LEVEL >= VP_PRINTF_LEVEL_DEBUG
#define vp_debug(fmt, ...) vp_printf("DEBUG",VP_PRINTF_LEVEL_DEBUG,"\e[1;33m",__FILE_NAME__, __LINE__, __FUNCTION__,fmt,##__VA_ARGS__)
#else
#define vp_debug(fmt, ...)
#endif

#if VP_PRINTF_LEVEL >= VP_PRINTF_LEVEL_ERROR
#define vp_error(fmt, ...) vp_printf("ERROR",VP_PRINTF_LEVEL_ERROR,"\e[1;31m",__FILE_NAME__, __LINE__, __FUNCTION__,fmt,##__VA_ARGS__)
#else
#define vp_error(fmt, ...)
#endif

#if VP_PRINTF_LEVEL >= VP_PRINTF_LEVEL_WARN
#define vp_warn(fmt, ...) vp_printf("WARN",VP_PRINTF_LEVEL_WARN,"\e[1;35m",__FILE_NAME__, __LINE__, __FUNCTION__,fmt,##__VA_ARGS__)
#else
#define vp_warn(fmt, ...)
#endif

#if VP_PRINTF_LEVEL >= VP_PRINTF_LEVEL_INFO
#define vp_info(fmt, ...) vp_printf("INFO",VP_PRINTF_LEVEL_INFO,"\e[1;37m",__FILE_NAME__, __LINE__, __FUNCTION__,fmt,##__VA_ARGS__)
#else
#define vp_info(fmt, ...)
#endif

#else

#define vp_printf_init(context)
#define vp_printf_set_level(level)
#define vp_printf_get_level() vp_printf_level_none
#define vp_print(tag, color, file, line, func, fmt, ...)
#define vp_print_hex(title, data, len, file, line, func)
#define vp_print_ram(title, file, line, func)
#define vp_printf_deinit()
#define vp_hex(title, data, len)
#define vp_ram(title)
#define vp_print_get_mem_info(total, free)
#define vp_stack(fmt, ...)
#define vp_debug(fmt, ...)
#define vp_error(fmt, ...)
#define vp_warn(fmt, ...)
#define vp_info(fmt, ...)

#endif

#ifdef __cplusplus
}
#endif

#endif
    