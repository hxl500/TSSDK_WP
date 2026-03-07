#include "vp_fw_env.h"
#include "fw_env_private.h"
#include "fw_env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static struct env_opts global_opts;
static int fw_env_initialized = 0;

/**
 * @brief 默认配置
 */
static struct vp_fw_env_config default_config = {
#ifdef CONFIG_FILE
    .config_file = CONFIG_FILE,
#else
    .config_file = NULL,
#endif
    .lockname = NULL
};

int vp_fw_env_init(const struct vp_fw_env_config *config)
{
    if (fw_env_initialized) {
        return 0; // 已经初始化
    }

    // 设置全局选项
    if (config) {
        if (config->config_file) {
            global_opts.config_file = strdup(config->config_file);
            if (!global_opts.config_file) {
                return -1;
            }
        }
        if (config->lockname) {
            global_opts.lockname = strdup(config->lockname);
            if (!global_opts.lockname) {
                if (global_opts.config_file) {
                    free(global_opts.config_file);
                }
                return -1;
            }
        }
    } else {
        // 使用默认配置
        if (default_config.config_file) {
            global_opts.config_file = strdup(default_config.config_file);
        }
        if (default_config.lockname) {
            global_opts.lockname = strdup(default_config.lockname);
        }
    }

    fw_env_initialized = 1;
    return 0;
}

void vp_fw_env_cleanup(void)
{
    if (global_opts.config_file) {
        free(global_opts.config_file);
        global_opts.config_file = NULL;
    }
    if (global_opts.lockname) {
        free(global_opts.lockname);
        global_opts.lockname = NULL;
    }
    
    fw_env_initialized = 0;
}

int vp_fw_env_get(const char *name, char *value, size_t value_len)
{
    struct env_opts opts;
    char *env_value = NULL;
    int ret = -1;

    // 参数检查
    if (!name || !value || value_len == 0) {
        fprintf(stderr, "vp_fw_env_get: invalid parameters\n");
        errno = EINVAL;
        return -1;
    }

    // 初始化选项
    memset(&opts, 0, sizeof(opts));
#ifdef CONFIG_FILE
    if (global_opts.config_file) {
        opts.config_file = global_opts.config_file;
    }
#endif
    if (global_opts.lockname) {
        opts.lockname = global_opts.lockname;
    }

    // 打开环境
    if (fw_env_open(&opts) != 0) {
        fprintf(stderr, "vp_fw_env_get: fw_env_open failed\n");
        return -1;
    }

    // 获取环境变量值
    env_value = fw_getenv((char *)name);
    if (!env_value) {
        fprintf(stderr, "vp_fw_env_get: variable '%s' not found\n", name);
        errno = ENOENT;
        goto cleanup;
    }

    // 安全地复制值到输出缓冲区
    size_t env_len = strlen(env_value);
    if (env_len >= value_len) {
        fprintf(stderr, "WARNING: vp_fw_env_get: value truncated (needed %zu, have %zu)\n", 
                env_len + 1, value_len);
    }

    size_t copy_len = (env_len < value_len - 1) ? env_len : value_len - 1;
    memcpy(value, env_value, copy_len);
    value[copy_len] = '\0';

    ret = 0;

cleanup:
    // 关闭环境
    if (fw_env_close(&opts) != 0) {
        // 即使关闭失败，如果我们已经成功获取值，仍然返回成功
        if (ret == 0) {
            fprintf(stderr, "WARNING: fw_env_close failed but value was retrieved\n");
        }
    }

    return ret;
}

int vp_fw_env_set(const char *name, const char *value)
{
    struct env_opts opts;
    int ret = -1;

    if (!name) {
        fprintf(stderr, "vp_fw_env_set: name is NULL\n");
        errno = EINVAL;
        return -1;
    }

    // 初始化选项
    memset(&opts, 0, sizeof(opts));
#ifdef CONFIG_FILE
    if (global_opts.config_file) {
        opts.config_file = global_opts.config_file;
    }
#endif
    if (global_opts.lockname) {
        opts.lockname = global_opts.lockname;
    }


    // 打开环境
    if (fw_env_open(&opts) != 0) {
        fprintf(stderr, "vp_fw_env_set: fw_env_open failed\n");
        return -1;
    }

    // 准备参数数组
    char *argv[3];
    int argc = 1;
    
    argv[0] = (char *)name;
    
    if (value && strlen(value) > 0) {
        argv[1] = (char *)value;
        argc = 2;
    }


    ret = fw_env_set(argc, argv, &opts);
    if (ret != 0) {
        fprintf(stderr, "vp_fw_env_set: fw_env_set failed\n");
    }

    // 注意：fw_env_set 内部会调用 fw_env_close，所以我们不需要再次调用
    // 但如果 fw_env_set 失败，我们可能仍然需要关闭环境
    if (ret != 0) {
        fw_env_close(&opts);
    }

    return ret;
}

int vp_fw_env_delete(const char *name)
{
    struct env_opts opts;
    int ret = -1;

    if (!name) {
        fprintf(stderr, "vp_fw_env_delete: name is NULL\n");
        errno = EINVAL;
        return -1;
    }

    // 初始化选项
    memset(&opts, 0, sizeof(opts));
#ifdef CONFIG_FILE
    if (global_opts.config_file) {
        opts.config_file = global_opts.config_file;
    }
#endif
    if (global_opts.lockname) {
        opts.lockname = global_opts.lockname;
    }


    // 打开环境
    if (fw_env_open(&opts) != 0) {
        fprintf(stderr, "vp_fw_env_delete: fw_env_open failed\n");
        return -1;
    }

    // 准备参数数组 - 只传递名字，不传递值，表示删除
    char *argv[1];
    int argc = 1;
    
    argv[0] = (char *)name;

    ret = fw_env_set(argc, argv, &opts);
    if (ret != 0) {
        fprintf(stderr, "vp_fw_env_delete: fw_env_set failed\n");
    }

    // 注意：fw_env_set 内部会调用 fw_env_close
    if (ret != 0) {
        fw_env_close(&opts);
    }

    return ret;
}

int vp_fw_env_print_all(int noheader)
{
    struct env_opts opts;

    // 初始化选项
    memset(&opts, 0, sizeof(opts));
#ifdef CONFIG_FILE
    if (global_opts.config_file) {
        opts.config_file = global_opts.config_file;
    }
#endif
    if (global_opts.lockname) {
        opts.lockname = global_opts.lockname;
    }

    // 打开环境
    if (fw_env_open(&opts) != 0) {
        fprintf(stderr, "vp_fw_env_print_all: fw_env_open failed\n");
        return -1;
    }

    // 调用fw_printenv打印所有变量
    int ret = fw_printenv(0, NULL, noheader, &opts);
    if (ret != 0) {
        fprintf(stderr, "vp_fw_env_print_all: fw_printenv failed\n");
        fw_env_close(&opts);
        return -1;
    }

    // fw_printenv 内部会调用 fw_env_close
    return 0;
}

int vp_fw_env_script(const char *script_file)
{
    struct env_opts opts;

    if (!script_file) {
        fprintf(stderr, "vp_fw_env_script: script_file is NULL\n");
        errno = EINVAL;
        return -1;
    }

    // 初始化选项
    memset(&opts, 0, sizeof(opts));
#ifdef CONFIG_FILE
    if (global_opts.config_file) {
        opts.config_file = global_opts.config_file;
    }
#endif
    if (global_opts.lockname) {
        opts.lockname = global_opts.lockname;
    }

    // 打开环境
    if (fw_env_open(&opts) != 0) {
        fprintf(stderr, "vp_fw_env_script: fw_env_open failed\n");
        return -1;
    }

    int ret = fw_parse_script((char *)script_file, &opts);
    if (ret != 0) {
        fprintf(stderr, "vp_fw_env_script: fw_parse_script failed\n");
        fw_env_close(&opts);
        return -1;
    }

    // fw_parse_script 内部可能会调用 fw_env_close，但为了安全我们还是调用一次
    fw_env_close(&opts);
    return 0;
}

int vp_fw_env_save(void)
{
    struct env_opts opts;

    // 初始化选项
    memset(&opts, 0, sizeof(opts));
#ifdef CONFIG_FILE
    if (global_opts.config_file) {
        opts.config_file = global_opts.config_file;
    }
#endif
    if (global_opts.lockname) {
        opts.lockname = global_opts.lockname;
    }


    // 打开环境
    if (fw_env_open(&opts) != 0) {
        fprintf(stderr, "vp_fw_env_save: fw_env_open failed\n");
        return -1;
    }

    int ret = fw_env_flush(&opts);
    if (ret != 0) {
        fprintf(stderr, "vp_fw_env_save: fw_env_flush failed\n");
    }

    fw_env_close(&opts);
    return ret;
}