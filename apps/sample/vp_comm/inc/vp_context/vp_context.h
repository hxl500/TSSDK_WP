#ifndef VP_CONTEXT_H
#define VP_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "vp_context_type.h"

/**
 * 初始化应用上下文
 *
 * @param app_argv  [in]    启动参数个数
 * @param app_args  [in]    启动参数
 *
 * @return 错误码
 * @retval = 0 成功
 * @retval \<0 失败
 */
int vp_context_init(int app_argv, const char **app_args);

/**
 * 获取App启动参命令数
 *
 * @param argv  [out]   参数个数
 * @param args  [out]   参数数组
 *
 * @return 错误码
 * @retval =0 成功
 * @retval -1 失败
 */
int vp_context_get_args(int *argv, const char ***args);

/**
 * 获取系统启动相关参数
 *
 * @param token [in]    参数Token
 * @param param [in]    参数名
 * @param value [out]   参数值
 *
 * @return 错误码
 * @retval >=0 成功,返回value的长度
 * @retval  <0 失败
 */
int vp_context_get_system_param(const char *token, const char *param, char *value);

/**
 * 设置系统启动相关参数
 *
 * @note 频繁修改系统启动参数可能导致系统无法启动,因此该方法只进行缓存的修改.
 *       在合适的地方进行系统启动参数同步即调用`vp_context_sync_system_param`将参数写入
 *
 * @param token [in]    参数Token
 * @param param [in]    参数名
 * @param value [out]   参数值
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_context_set_system_param(const char *token, const char *param, char *value);

/**
 * 将缓存系统参数同步写入系统文件.
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_context_sync_system_param(void);

/**
 * 获取SDK编译平台
 *
 * @return vp_build_platform_t
 */
vp_platform_t vp_context_get_platform(void);

/**
 * 获取项目名称
 */
const char *vp_project_name();

/**
 * 获取项目打包版本信息
 *
 * @param major [out]   输出主版本号
 * @param minor [out]   输出次版本号
 * @param patch [out]   输出发布版本号
 * @param tweak [out]   输出编译版本号
 *
 * @return 项目打包版本字符串
 */
const char *vp_project_version(uint32_t *major, uint32_t *minor, uint32_t *patch, uint32_t *tweak);

/**
 * 获取项目定义品牌
 */
const char *vp_project_brand();

/**
 * 获取项目定义型号
 */
const char *vp_project_mode();

/**
 * 获取SOC信息
 *
 * @return vp_soc_info_t*
 */
vp_soc_info_t *vp_context_soc_info(void);

/**
 * 获取NIC信息
 *
 * @return vp_nic_info_t*
 */
vp_nic_info_t *vp_context_nic_info(void);

/**
 * 设置设备唯一ID标识
 *
 * @param id    [in]    id名称,字符长度必须小于128字节
 * @param key   [in]    校验码,字符长度必须小于64字节
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_context_set_device_id(char *id, char *key);

/**
 * 重置设备唯一ID标识
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_context_reset_device_id(void);

/**
 * 设置MIPI控制器
 *
 * @param ctrl [in] 控制器
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_context_set_mipi_ctrl(int ctrl);

/**
 * 获取设备唯一ID标识
 *
 * @return 设备唯一ID标识
 */
const char *vp_context_device_id(void);

/**
 * 获取设备ID校验码
 *
 * @return 设备ID校验码
 */
const char *vp_context_device_key(void);

/**
 * 获取是否为生产模式
 *
 * @return 生产模式
 * @retval 0 否
 * @retval 1 是
 */
int vp_context_factory_mode();

/**
 * 关闭context
 */
void vp_context_close();

#ifdef __cplusplus
}
#endif


#endif
    