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


typedef struct vp_fusion_detect_handle *vp_fusion_detect_handle_p;

/**
 * 创建fusion检测句柄
 *
 * @param width   [in]   图像宽度
 * @param height  [in]   图像高度
 *
 * @return fusion检测句柄
 */
vp_fusion_detect_handle_p vp_fusion_detect_create(uint32_t width, uint32_t height);

/**
 * 获取人形检测参数
 *
 * @param handle  [in]   fusion检测句柄
 * @param param   [out]  检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_fusion_detect_get_human_param(vp_fusion_detect_handle_p handle, vp_human_detect_param_t *param);

/**
 * 获取移动检测参数
 *
 * @param handle  [in]   fusion检测句柄
 * @param param   [out]  检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_fusion_detect_get_motion_param(vp_fusion_detect_handle_p handle, vp_motion_detect_param_t *param);

/**
 * 设置人形检测参数
 *
 * @param handle  [in]   fusion检测句柄
 * @param param   [in]   检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_fusion_detect_set_human_param(vp_fusion_detect_handle_p handle, vp_human_detect_param_t *param);

/**
 * 设置移动检测参数
 *
 * @param handle  [in]   fusion检测句柄
 * @param param   [in]   检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_fusion_detect_set_motion_param(vp_fusion_detect_handle_p handle, vp_motion_detect_param_t *param);

/**
 * 执行fusion检测
 *
 * @param handle    [in]   fusion检测句柄
 * @param frame     [in]   图像帧数据
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_fusion_detect_process(vp_fusion_detect_handle_p handle, vp_video_source_t *frame);

/**
 * 获取人形检测结果
 *
 * @param result    [out]  检测结果
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_fusion_detect_human_result(vp_fusion_detect_handle_p handle, vp_human_detect_result_t *result);

/**
 * 获取移动检测结果
 *
 * @param result    [out]  检测结果
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_fusion_detect_motion_result(vp_fusion_detect_handle_p handle, vp_motion_detect_result_t *result);

/**
 * 恢复fusion检测
 *
 * @param handle    [in]   fusion检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_fusion_detect_resume(vp_fusion_detect_handle_p handle, vp_algorithm_type_t type);

/**
 * 暂停fusion检测
 *
 * @param handle    [in]   fusion检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_fusion_detect_pause(vp_fusion_detect_handle_p handle, vp_algorithm_type_t type);

/**
 * 销毁fusion检测句柄
 *
 * @param handle  [in]   fusion检测句柄
 */
void vp_fusion_detect_destroy(vp_fusion_detect_handle_p handle);


#ifdef __cplusplus
}
#endif