//
// Created by 杨柳 on 2022/12/8.
//

#ifndef VP_FRAMEWORK_VP_CMDLINE_H
#define VP_FRAMEWORK_VP_CMDLINE_H

#include <stdint.h>
#include <pthread.h>

typedef struct {
    char system_args[2048];
    char write_args[2048];
    volatile uint8_t close;
    uint8_t system_changed;
    pthread_mutex_t mutex;
} vp_cmdline_t;

/**
 * 初始化cmdline参数
 *
 * @param cmdline   [in,out]    cmdline结构体
 */
void vp_cmdline_init(vp_cmdline_t *cmdline);

/**
 * 读取cmdline中选项值
 *
 * @param cmdline   [in]    cmdline结构体
 * @param token     [in]    参数Token
 *                          比如:cmdline有字符串 `senv;[HW];init_vw=1920;init_vh=1080;eenv`
 *                          Token为`HW`
 *
 * @param parameter [in]    参数名,比如`init_vw`和`init_vh`
 * @param value     [out]   输出参数值
 *
 * @return 返回value的字符长度或者错误
 * @retval >=0 成功,value字符长度
 * @retval  <0 失败
 */
int vp_cmdline_read_env(vp_cmdline_t *cmdline, const char *token, const char *parameter, char *value);

/**
 * 读取cmdline中选项值
 *
 * @param cmdline   [in]    cmdline结构体
 * @param token     [in]    参数Token
 *                          比如:cmdline有字符串 `senv;[HW];init_vw=1920;init_vh=1080;eenv`
 *                          Token为`HW`
 *
 * @param parameter [in]    参数名,比如`init_vw`和`init_vh`
 * @param value     [in]    参数值
 *
 * @return 返回value的字符长度或者错误
 * @retval >=0 成功,value字符长度
 * @retval  <0 失败
 */
int vp_cmdline_write_env(vp_cmdline_t *cmdline, const char *token, const char *parameter, char *value);

/**
 * 删除cmdline中节点
 *
 * @param cmdline   [in]    cmdline结构体
 * @param token     [in]    参数Token
 *                          比如:cmdline有字符串 `senv;[HW];init_vw=1920;init_vh=1080;eenv`
 *                          Token为`HW`
 *
 * @param item      [in]    参数名,比如`init_vw`和`init_vh`
 *
 * @return返回value的字符长度或者错误
 * @retval >=0 成功,value字符长度
 * @retval  <0 失败
 */
int vp_cmdline_del_item(vp_cmdline_t *cmdline, const char *token, const char *item);

/**
 * 将缓存系统参数同步写入系统文件.
 *
 * @param cmdline   [in]    cmdline结构体
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_cmdline_sync(vp_cmdline_t *cmdline);

/**
 * 关闭并同步CmdLine
 * @param cmdline   [in]    cmdline结构体
 */
void vp_cmdline_close(vp_cmdline_t *cmdline);

#endif //VP_FRAMEWORK_VP_CMDLINE_H
