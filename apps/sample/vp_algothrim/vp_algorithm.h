#ifndef VP_ALGORITHM_H
#define VP_ALGORITHM_H

#include <stdint.h>
// #include "vp_algorithm_config.h"
#include "vp_algorithm_type.h"

typedef enum {
    /* 空闲状态 */
    VP_ALGORITHM_STATE_IDLE = 0,
    /* 启动中状态 */
    VP_ALGORITHM_STATE_STARTING,
    /* 运行中 */
    VP_ALGORITHM_STATE_RUNNING,
    /* 暂停状态 */
    VP_ALGORITHM_STATE_PAUSED,
    /* 停止状态 */
    VP_ALGORITHM_STATE_STOPPED,
    /* 错误 */
    VP_ALGORITHM_STATE_ERROR,
    /* 无授权信息 */
    VP_ALGORITHM_STATE_NO_AUTH,
} vp_algorithm_state_t;

/**
 * 初始化AI算法模块
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_init();

/**
 * 获取AI算法模块鉴权文件路径
 * @return 算法鉴权文件路径
 */
char *vp_algorithm_auth_file();

/**
 * 获取AI算法模块模型文件路径
 * @return 算法模型文件路径
 */
char *vp_algorithm_model_file();

/**
 * 获取AI算法模块配置文件路径
 * @return 算法配置文件路径
 */
char *vp_algorithm_config_file();

/**
 * 启动AI算法模块
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @param param [in]    算法参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_start(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, vp_algorithm_param_t *param);

typedef struct {
    uint8_t enable;
    uint8_t paused;
    /**
     * 最后通知时间,系统启动时间,(us)
     */
    uint64_t notify_timestamp;
    /**
     * 最后检测时间,系统启动时间,(us)
     */
    uint64_t detect_timestamp;
} vp_algorithm_status_t;

/**
 * 获取AI算法模块状态
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @param status [out] 算法状态
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_state(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, vp_algorithm_status_t *status);

/**
 * 获取AI算法模块是否启用
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @return 0 禁用,1 启用
 */
int vp_algorithm_enable_state(uint8_t idx, uint8_t chn, vp_algorithm_type_t type);

/**
 * 获取AI算法模块是否暂停
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @return 0 暂停,1 运行
 */
int vp_algorithm_pause_state(uint8_t idx, uint8_t chn, vp_algorithm_type_t type);

/**
 * 启用禁用AI算法模块通知
 *
 * @param idx       [in]    视频传感器索引
 * @param chn       [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type      [in]    算法类型
 * @param enable    [in]    是否启用通知
 * @param jpeg      [in]    是否推送图片
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_enable_notify(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, uint8_t enable, uint8_t jpeg);

/**
 * 获取AI算法模块参数
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @param param [out]   算法参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_get_param(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, vp_algorithm_param_t *param);

/**
 * 更新AI算法模块参数
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @param param [in]    算法参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_set_param(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, vp_algorithm_param_t *param);

/**
 * 暂停AI算法模块
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_algorithm_pause(uint8_t idx, uint8_t chn, vp_algorithm_type_t type);

/**
 * 恢复AI算法模块
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_algorithm_resume(uint8_t idx, uint8_t chn, vp_algorithm_type_t type);

/**
 * 等待AI算法模块结果
 *
 * @param idx       [in]    视频传感器索引
 * @param chn       [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type      [in]    算法类型
 * @param timeout   [in]    等待超时时间,单位毫秒(ms)
 * @param result    [out]  算法结果
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_algorithm_wait_result(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, uint32_t timeout,
                             vp_algorithm_result_t *result);

/**
 * 获取AI算法模块结果
 *
 * @param idx       [in]    视频传感器索引
 * @param chn       [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type      [in]    算法类型
 * @param result    [out]  算法结果
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_algorithm_get_result(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, vp_algorithm_result_t *result);

/**
 * 停止AI算法模块
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_stop(uint8_t idx, uint8_t chn, vp_algorithm_type_t type);

/**
 * 重新加载AI算法模块
 */
void vp_algorithm_reload();

/**
 * 释放AI算法模块资源
 */
void vp_algorithm_deinit();

#endif
