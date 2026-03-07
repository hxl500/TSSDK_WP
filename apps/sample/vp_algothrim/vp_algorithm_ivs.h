//
// Created by 杨柳 on 2024/11/26.
//

#ifndef VP_ALGORITHM_VP_ALGORITHM_IVS_H
#define VP_ALGORITHM_VP_ALGORITHM_IVS_H

#include <stdint.h>
#include <vp_video_coder_type.h>

typedef struct vp_algorithm_ivs_args vp_algorithm_ivs_args_t;


/**
 * ivs 初始化函数
 *
 * @return 错误码
 * @retval >=0 初始化成功
 * @retval <0 初始化失败
 */
typedef int (*vp_algorithm_ivs_init_f)(vp_algorithm_ivs_args_t *args);

/**
 * ivs 开始通道函数
 *
 * @return 错误码
 * @retval >=0 成功
 * @retval <0 失败
 */
typedef int (*vp_algorithm_ivs_start_f)(uint8_t idx, uint8_t chn, vp_algorithm_ivs_args_t *args);

/**
 * ivs 前置处理函数
 *
 * @return 错误码
 * @retval =1 跳帧
 * @retval =0 成功
 * @retval <0 失败
 */
typedef int (*vp_algorithm_ivs_before_f)(uint8_t idx, uint8_t chn, vp_algorithm_ivs_args_t *args,
                                         vp_video_source_t *frame);

/**
 * ivs 处理函数
 *
 * @return 错误码
 * @retval =1 跳帧
 * @retval =0 成功
 * @retval <0 失败
 */
typedef int (*vp_algorithm_ivs_process_f)(uint8_t idx, uint8_t chn, vp_algorithm_ivs_args_t *args,
                                          vp_video_source_t *frame);

/**
 * ivs 后置处理函数
 *
 * @return 错误码
 * @retval =1 跳帧
 * @retval =0 成功
 * @retval <0 失败
 */
typedef int (*vp_algorithm_ivs_after_f)(uint8_t idx, uint8_t chn, vp_algorithm_ivs_args_t *args,
                                        vp_video_source_t *frame);

/**
 * ivs 结束通道函数
 *
 * @return 错误码
 * @retval >=0 成功
 * @retval <0 失败
 */
typedef int (*vp_algorithm_ivs_stop_f)(uint8_t idx, uint8_t chn, vp_algorithm_ivs_args_t *args);

/**
 * ivs 释放函数
 */
typedef int (*vp_algorithm_ivs_deinit_f)(vp_algorithm_ivs_args_t *args);

struct vp_algorithm_ivs_args {
    void *user_data;
    vp_algorithm_ivs_init_f init;
    vp_algorithm_ivs_start_f start;
    vp_algorithm_ivs_before_f before;
    vp_algorithm_ivs_process_f process;
    vp_algorithm_ivs_after_f after;
    vp_algorithm_ivs_stop_f stop;
    vp_algorithm_ivs_deinit_f deinit;
};

/**
 * 初始化IVS算法通道
 * @param args  [in]    IVS参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_ivs_init(vp_algorithm_ivs_args_t *args);

/**
 * 绑定IVS算法通道
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_ivs_bind(uint8_t idx, uint8_t chn);

/**
 * 解绑IVS算法通道
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_ivs_unbind(uint8_t idx, uint8_t chn);

/**
 * 重新加载IVS算法通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_ivs_reload();

/**
 * 释放IVS算法通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_ivs_deinit();

#endif //VP_ALGORITHM_VP_ALGORITHM_IVS_H
