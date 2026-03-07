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


typedef struct vp_qrcode_detect_handle *vp_qrcode_detect_handle_p;

/**
 * 创建qrcode检测句柄
 *
 * @param width   [in]   图像宽度
 * @param height  [in]   图像高度
 * @param param   [in]   qrcode检测参数
 *
 * @return qrcode检测句柄
 */
vp_qrcode_detect_handle_p vp_qrcode_detect_create(uint32_t width, uint32_t height, vp_qrcode_detect_param_t *param);

/**
 * 获取qrcode检测参数
 *
 * @param handle  [in]   qrcode检测句柄
 * @param param   [out]  qrcode检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_qrcode_detect_get_param(vp_qrcode_detect_handle_p handle, vp_qrcode_detect_param_t *param);

/**
 * 获取qrcode检测参数
 *
 * @param handle  [in]   qrcode检测句柄
 * @param param   [in]   qrcode检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_qrcode_detect_set_param(vp_qrcode_detect_handle_p handle, vp_qrcode_detect_param_t *param);

/**
 * 执行qrcode检测
 *
 * @param handle    [in]   qrcode检测句柄
 * @param frame     [in]   图像帧数据
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_qrcode_detect_process(vp_qrcode_detect_handle_p handle, vp_video_source_t *frame);

/**
 * 恢复qrcode检测
 *
 * @param handle    [in]   qrcode检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_qrcode_detect_resume(vp_qrcode_detect_handle_p handle);

/**
 * 暂停qrcode检测
 *
 * @param handle    [in]   qrcode检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_qrcode_detect_pause(vp_qrcode_detect_handle_p handle);

/**
 * 获取qrcode检测结果
 *
 * @param result    [out]  检测结果
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_qrcode_detect_result(vp_qrcode_detect_handle_p handle, vp_qrcode_detect_result_t *result);

/**
 * 销毁qrcode检测句柄
 *
 * @param handle  [in]   qrcode检测句柄
 */
void vp_qrcode_detect_destroy(vp_qrcode_detect_handle_p handle);


#ifdef __cplusplus
}
#endif