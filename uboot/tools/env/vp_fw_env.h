#ifndef VP_FW_ENV_H
#define VP_FW_ENV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
/**
 * @brief 环境变量API配置结构体
 */
struct vp_fw_env_config {
    char *config_file;    /**< 配置文件路径 */
    char *lockname;       /**< 锁文件路径 */
};

/**
 * @brief 初始化环境变量API
 * @param config 配置参数，如果为NULL则使用默认配置
 * @return 0:成功, -1:失败
 */
int vp_fw_env_init(const struct vp_fw_env_config *config);

/**
 * @brief 清理环境变量API资源
 */
void vp_fw_env_cleanup(void);

/**
 * @brief 读取环境变量值
 * @param name 环境变量名
 * @param value 输出缓冲区
 * @param value_len 缓冲区长度
 * @return 0:成功, -1:失败
 */
int vp_fw_env_get(const char *name, char *value, size_t value_len);

/**
 * @brief 设置环境变量
 * @param name 环境变量名
 * @param value 环境变量值
 * @return 0:成功, -1:失败
 */
int vp_fw_env_set(const char *name, const char *value);

/**
 * @brief 删除环境变量
 * @param name 环境变量名
 * @return 0:成功, -1:失败
 */
int vp_fw_env_delete(const char *name);

/**
 * @brief 打印所有环境变量
 * @param noheader 是否显示变量名(0:显示, 1:不显示)
 * @return 0:成功, -1:失败
 */
int vp_fw_env_print_all(int noheader);

/**
 * @brief 批量设置环境变量(通过脚本文件)
 * @param script_file 脚本文件路径
 * @return 0:成功, -1:失败
 */
int vp_fw_env_script(const char *script_file);

/**
 * @brief 保存环境变量到存储设备
 * @return 0:成功, -1:失败
 */
int vp_fw_env_save(void);


#ifdef __cplusplus
}
#endif

#endif /* VP_FW_ENV_H */