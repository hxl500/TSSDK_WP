//
// Created by 杨柳 on 2022/11/26.
//

#ifndef VP_FRAMEWORK_VP_SYSTEM_H
#define VP_FRAMEWORK_VP_SYSTEM_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * 获取内存信息
 *
 * @param total [in,out] 存储内存总大小
 * @param free  [in,out] 存储空闲内存大小
 * @param used  [in,out] 存储已使用的内存大小
 *
 * @return 错误码
 * @retval 0 成功
 * @retval 非0 失败
 */
int vp_system_mem(uint64_t *total, uint64_t *free, uint64_t *used);

/**
 * 写入文件
 *
 * @param file  [in]    文件路径
 * @param str   [in]    待写入的字符串
 * @param len   [in]    待写入的字节数
 * @return
 */
int vp_echo(const char *file, const char *str, size_t len);

/**
 * 获取CPU使用率
 *
 * @return CPU使用率
 */
float vp_system_get_cpu();

typedef struct {
    uint64_t total; /* 总CPU使用计数 */
    uint64_t idle;  /* 空闲CPU使用计数 */
}vp_system_cpu_info_t;

/**
 * 获取CPU信息
 *
 * @return 错误码
 * @retval 0 成功
 * @retval 非0 失败
 */
float vp_system_cpu_usage(vp_system_cpu_info_t *info);

/**
 * 释放系统缓存内存
 *
 * @param type [in] 缓存类型 0: 不做任何处理,
 *                          1: pagecache,
 *                          2: dentry+inode cache,
 *                          3: all
 *
 * @return 错误码
 * @retval 0 成功
 * @retval 非0 失败
 */
int vp_system_drop_caches(uint8_t type);

/**
 * 设置系统保留内存
 *
 * @param size [in] 保留内存大小,单位KB
 *
 * @return 错误码
 * @retval 0 成功
 * @retval 非0 失败
 */
int vp_system_keep_mem(uint32_t size);

/**
 * 配置内存脏页大小
 *
 * @param size [in] 脏页大小,单位byte
 *
 * @return 错误码
 * @retval 0 成功
 * @retval 非0 失败
 */
int vp_system_dirty_size(uint32_t size);

/**
 * 配置应用保留内存
 *
 * @param size [in] 保留内存大小,单位KB
 *
 * @return 错误码
 * @retval 0 成功
 * @retval 非0 失败
 */
int vp_system_reserve_size(uint32_t size);

/**
 * 执行指令并获取返回文件流
 *
 * @param cmd [in] 指令
 *
 * @return 返回文件流
 */
FILE *vp_fpopen(const char *cmd);

/**
 * 执行指令
 *
 * @note 函数内部使用1024字节长度的char[],拼接格式化字符串.
 * 当指令长度可能超过1024字节时考虑使用`vp_system_cmd`函数.
 * 该函数内部调用系统函数`system`方法实现
 *
 * @param fmt   [in]    指令格式化字符串
 * @param ...   [in]    格式化参数
 *
 * @return 指令执行结果
 */
int vp_system(const char *fmt, ...);

/**
 * 执行指令并获取返回信息
 *
 * @param cmd       [in]     系统指令
 * @param result    [in,out] 输入返回信息存储地址,输出指令响应信息
 * @param size      [in,out] 输入存储地址长度,输出响应信息长度
 *
 * @return 指令执行结果
 */
int vp_system_result(const char *cmd, char *result, size_t *size);

/**
 * 执行指令并打印输出信息
 *
 * @note 函数内部使用1024字节长度的char[],拼接格式化字符串.
 * 该函数内部调用系统函数`system`方法实现
 *
 * @param fmt   [in]    指令格式化字符串
 * @param ...   [in]    格式化参数
 *
 * @return 指令执行结果
 */
int vp_system_print(const char *fmt, ...);

/**
 * 使用用户自行开辟的内存空间进行指令拼接
 *
 * @details 详细描述可参考函数`vp_system`
 *
 * @param cmd   [in]  用户创建的字符串缓存区
 * @param len   [in]  字符串缓存区长度
 * @param fmt   [in]  指令格式化字符串
 * @param ...   [in]  格式化参数
 *
 * @return 指令执行结果
 */
int vp_system_cmd(char *cmd, size_t len, const char *fmt, ...);

/**
 * 执行指令,并获取一行指令输出.
 *
 * @note 函数内部使用1024字节长度的char[],拼接格式化字符串.
 * 当指令长度可能超过1024字节时考虑使用`vp_system_cmd_line`函数.
 *
 * @param line  [in,out]存储返回指令输出信息的字符串缓冲区
 * @param len   [in]    缓冲区长度
 * @param fmt   [in]    指令格式化字符串
 * @param ...   [in]    格式化参数
 *
 * @return 指令执行结果
 */
int vp_system_line(char *line, int len, const char *fmt, ...);

/**
 * 使用用户自行开辟的内存空间进行指令拼接,并获取一行指令输出.
 *
 * @details 详细描述可参考函数`vp_system_line`
 *
 * @param line      [in,out]存储返回指令输出信息的字符串缓冲区
 * @param line_len  [in]    缓冲区长度
 * @param cmd       [in]    用户创建的字符串缓存区,用于拼接指令
 * @param cmd_len   [in]    指令缓存区长度
 * @param fmt       [in]    指令格式化字符串
 * @param ...       [in]    格式化参数
 *
 * @return 指令执行结果
 */
int vp_system_cmd_line(char *line, int line_len, char *cmd, size_t cmd_len, const char *fmt, ...);

/**
 * 执行指令,并获取多行指令输出.
 *
 * @note 函数内部使用1024字节长度的char[],拼接格式化字符串.
 * 当指令长度可能超过1024字节时考虑使用`vp_system_cmd_lines`函数.
 *
 * @param lines [in,out]   存储返回指令输出信息的字符串缓冲区
 * @param item  [in]    单行长度
 * @param rows  [in,out]输入lines行数,返回输出行数
 * @param fmt   [in]    指令格式化字符串
 * @param ...   [in]    格式化参数
 *
 * @return 指令执行结果
 */
int vp_system_lines(char **lines, int item, int *rows, const char *fmt, ...);

/**
 * 使用用户自行开辟的内存空间进行指令拼接,并获取一行指令输出.
 *
 * @details 详细描述可参考函数`vp_system_lines`
 *
 * @param line      [in,out]存储返回指令输出信息的字符串缓冲区
 * @param item      [in]    单行长度
 * @param rows      [in,out]输入lines行数,返回输出行数
 * @param line_len  [in]    缓冲区长度
 * @param cmd       [in]    用户创建的字符串缓存区,用于拼接指令
 * @param cmd_len   [in]    指令缓存区长度
 * @param fmt       [in]    指令格式化字符串
 * @param ...       [in]    格式化参数
 *
 * @return 指令执行结果
 */
int vp_system_cmd_lines(char **lines, int item, int *rows, char *cmd, size_t cmd_len, const char *fmt, ...);


/**
 * 执行指令 [异步模式 发送到小进程执行] [仅常电使用 需要小进程支持]
 *
 * cmd 需要执行的指令
 *
 * @return 指令执行结果
 */
int vp_system_async(char *cmd);

#ifdef __cplusplus
}
#endif
#endif //VP_FRAMEWORK_VP_SYSTEM_H
