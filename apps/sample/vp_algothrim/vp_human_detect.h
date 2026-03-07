//
// Created by 杨柳 on 2024/11/22.
//

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "vp_algorithm_type.h"
#include "vp_algorithm_ivs.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct vp_human_detect_handle *vp_human_detect_handle_p;

/**
 * 创建human检测句柄
 *
 * @param width   [in]   图像宽度
 * @param height  [in]   图像高度
 * @param param   [in]   human检测参数
 *
 * @return human检测句柄
 */
vp_human_detect_handle_p vp_human_detect_create(uint32_t width, uint32_t height, vp_human_detect_param_t *param);

/**
 * 获取human检测参数
 *
 * @param handle  [in]   human检测句柄
 * @param param   [out]  human检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_human_detect_get_param(vp_human_detect_handle_p handle, vp_human_detect_param_t *param);

/**
 * 获取human检测参数
 *
 * @param handle  [in]   human检测句柄
 * @param param   [in]   human检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_human_detect_set_param(vp_human_detect_handle_p handle, vp_human_detect_param_t *param);

/**
 * 执行human检测
 *
 * @param handle    [in]   human检测句柄
 * @param frame     [in]   图像帧数据
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_human_detect_process(vp_human_detect_handle_p handle, vp_video_source_t *frame);

/**
 * 获取human检测结果
 *
 * @param result    [out]  检测结果
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_human_detect_result(vp_human_detect_handle_p handle, vp_human_detect_result_t *result);

/**
 * 恢复human检测
 *
 * @param handle    [in]   human检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_human_detect_resume(vp_human_detect_handle_p handle);

/**
 * 暂停human检测
 *
 * @param handle    [in]   human检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_human_detect_pause(vp_human_detect_handle_p handle);

/**
 * 销毁human检测句柄
 *
 * @param handle  [in]   human检测句柄
 */
void vp_human_detect_destroy(vp_human_detect_handle_p handle);


#ifdef __cplusplus
}
#endif