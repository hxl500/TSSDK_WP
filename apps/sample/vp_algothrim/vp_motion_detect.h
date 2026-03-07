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


typedef struct vp_motion_detect_handle *vp_motion_detect_handle_p;

/**
 * 创建motion检测句柄
 *
 * @param width   [in]   图像宽度
 * @param height  [in]   图像高度
 * @param param   [in]   motion检测参数
 *
 * @return motion检测句柄
 */
vp_motion_detect_handle_p vp_motion_detect_create(uint32_t width, uint32_t height, vp_motion_detect_param_t *param);

/**
 * 获取motion检测参数
 *
 * @param handle  [in]   motion检测句柄
 * @param param   [out]  motion检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_motion_detect_get_param(vp_motion_detect_handle_p handle, vp_motion_detect_param_t *param);

/**
 * 获取motion检测参数
 *
 * @param handle  [in]   motion检测句柄
 * @param param   [in]   motion检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_motion_detect_set_param(vp_motion_detect_handle_p handle, vp_motion_detect_param_t *param);

/**
 * 执行motion检测
 *
 * @param handle    [in]   motion检测句柄
 * @param frame     [in]   图像帧数据
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_motion_detect_process(vp_motion_detect_handle_p handle, vp_video_source_t *frame);

/**
 * 恢复motion检测
 *
 * @param handle    [in]   motion检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_motion_detect_resume(vp_motion_detect_handle_p handle);

/**
 * 暂停motion检测
 *
 * @param handle    [in]   motion检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_motion_detect_pause(vp_motion_detect_handle_p handle);

/**
 * 获取motion检测结果
 *
 * @param result    [out]  检测结果
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_motion_detect_result(vp_motion_detect_handle_p handle, vp_motion_detect_result_t *result);

/**
 * 销毁motion检测句柄
 *
 * @param handle  [in]   motion检测句柄
 */
void vp_motion_detect_destroy(vp_motion_detect_handle_p handle);


#ifdef __cplusplus
}
#endif