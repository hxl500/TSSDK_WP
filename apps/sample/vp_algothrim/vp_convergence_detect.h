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


typedef struct vp_convergence_detect_handle *vp_convergence_detect_handle_p;

/**
 * 创建convergence检测句柄
 *
 * @param width   [in]   图像宽度
 * @param height  [in]   图像高度
 * @param idx     [in]   检测通道
 * @param param   [in]   convergence检测参数
 *
 * @return convergence检测句柄
 */
vp_convergence_detect_handle_p vp_convergence_detect_create(uint32_t width, uint32_t height, uint8_t idx,
                                                            vp_convergence_detect_param_t *param);

/**
 * 获取convergence检测参数
 *
 * @param handle  [in]   convergence检测句柄
 * @param param   [out]  convergence检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_convergence_detect_get_param(vp_convergence_detect_handle_p handle, vp_convergence_detect_param_t *param);

/**
 * 获取convergence检测参数
 *
 * @param handle  [in]   convergence检测句柄
 * @param param   [in]   convergence检测参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_convergence_detect_set_param(vp_convergence_detect_handle_p handle, vp_convergence_detect_param_t *param);

/**
 * 执行convergence检测
 *
 * @param handle            [in]   convergence检测句柄
 * @param human_timestamp   [in]   人形检测时间戳
 * @param result            [in]   人形检测结果
 * @param frame             [in]   图像数据
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_convergence_detect_process(vp_convergence_detect_handle_p handle, uint32_t human_timestamp,
                                  vp_human_detect_result_t *result, vp_video_source_t *frame);

/**
 * 恢复convergence检测
 *
 * @param handle    [in]   convergence检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_convergence_detect_resume(vp_convergence_detect_handle_p handle);

/**
 * 暂停convergence检测
 *
 * @param handle    [in]   convergence检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_convergence_detect_pause(vp_convergence_detect_handle_p handle);

/**
 * 获取convergence检测结果
 *
 * @param result    [out]  检测结果
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_convergence_detect_result(vp_convergence_detect_handle_p handle, vp_convergence_detect_result_t *result);

/**
 * 销毁convergence检测句柄
 *
 * @param handle  [in]   convergence检测句柄
 */
void vp_convergence_detect_destroy(vp_convergence_detect_handle_p handle);


#ifdef __cplusplus
}
#endif