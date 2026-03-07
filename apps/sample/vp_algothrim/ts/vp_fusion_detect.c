//
// Created by 杨柳 on 2024/11/22.
//

#include <string.h>
#include "vp_fusion_detect.h"
#include "vp_algorithm.h"
#include "vp_printf.h"

#define HUMAN_DEFAULT_SCORE  (0.58f) //默认待机检测使用的得分
#define HUMAN_DET_SCORE  (0.40f) //默认检测使用的得分

#if VP_SDK_PLATFORM == VP_SDK_PLATFORM_T41

#include "jzai/ivs_inf_personvehicleDet.h"

typedef personvehicledet_param_input_t vp_human_input_t;
typedef personvehicledet_param_output_t vp_human_output_t;
#endif
#if VP_SDK_PLATFORM == VP_SDK_PLATFORM_T23 || VP_SDK_PLATFORM == VP_SDK_PLATFORM_T31

#include "jzai/ivs_inf_personDet.h"
#include "vp_sensor_config.h"

typedef persondet_param_input_t vp_human_input_t;
typedef persondet_param_output_t vp_human_output_t;

#endif


struct vp_fusion_detect_handle {
    vp_human_detect_param_t human;
    vp_human_detect_param_t motion;
    uint32_t width;
    uint32_t height;
    uint32_t area_width;
    uint32_t area_height;
    uint64_t timestamp;
    vp_human_detect_result_t human_result;
    vp_motion_detect_result_t motion_result;
    /* 无检测结果次数 */
    uint32_t no_result_count;
    uint32_t update_flag;
} vp_fusion_detect_handle_t;

static vp_fusion_detect_handle_p g_handle = NULL;
static uint32_t g_ref = 0;

/**
 * 创建fusion检测句柄
 *
 * @param width   [in]   图像宽度
 * @param height  [in]   图像高度
 *
 * @return fusion检测句柄
 */
vp_fusion_detect_handle_p vp_fusion_detect_create(uint32_t width, uint32_t height) {
    return NULL;
}

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
int vp_fusion_detect_get_human_param(vp_fusion_detect_handle_p handle, vp_human_detect_param_t *param) {
    if (handle == NULL || param == NULL) return -1;
    memcpy(param, &handle->human, sizeof(vp_human_detect_param_t));
    return 0;
}

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
int vp_fusion_detect_get_motion_param(vp_fusion_detect_handle_p handle, vp_motion_detect_param_t *param) {
    if (handle == NULL || param == NULL) return -1;
    memcpy(param, &handle->motion, sizeof(vp_human_detect_param_t));
    return 0;
}

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
int vp_fusion_detect_set_human_param(vp_fusion_detect_handle_p handle, vp_human_detect_param_t *param) {
    memcpy(&handle->human, param, sizeof(vp_human_detect_param_t));
    return 0;
}

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
int vp_fusion_detect_set_motion_param(vp_fusion_detect_handle_p handle, vp_motion_detect_param_t *param) {
    if (handle == NULL || param == NULL) return -1;
    memcpy(&handle->motion, param, sizeof(vp_motion_detect_param_t));
    return 0;
}

static int vp_detect_area_check(vp_fusion_detect_handle_p handle, const uint32_t *detect_area,
                                uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    uint32_t start_x = x / handle->area_width;
    uint32_t end_x = (x + width) / handle->area_width;

    uint32_t start_y = y / handle->area_height;
    uint32_t end_y = (y + height) / handle->area_height;

    uint32_t src_area = 0;
    for (uint32_t i = start_x; i <= end_x; ++i) {
        src_area |= (0x200000 >> i);
    }

    for (uint32_t i = start_y; i < end_y; ++i) {
        uint32_t area = detect_area[i] & 0x3FFFFF;
        if (area == 0x3FFFFF || (src_area & area) != 0) return 1;
    }
    return 0;
}

static int person_info_cmp(const void *a, const void *b) {
    return 0;
}

static void vp_human_detect_set_skip_num(vp_fusion_detect_handle_p handle, int value) {
}

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
int vp_fusion_detect_process(vp_fusion_detect_handle_p handle, vp_video_source_t *frame) {
    return 0;
}

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
int vp_fusion_detect_human_result(vp_fusion_detect_handle_p handle, vp_human_detect_result_t *result) {
    if (handle == NULL) return -1;
    memcpy(result, &handle->human_result, sizeof(vp_human_detect_result_t));
    return 0;
}

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
int vp_fusion_detect_motion_result(vp_fusion_detect_handle_p handle, vp_motion_detect_result_t *result) {
    if (handle == NULL) return -1;
    memcpy(result, &handle->motion_result, sizeof(vp_motion_detect_result_t));
    return 0;
}

/**
 * 恢复human检测
 *
 * @param handle    [in]   human检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_fusion_detect_resume(vp_fusion_detect_handle_p handle, vp_algorithm_type_t type) {
    (void) handle;
    (void) type;
    return 0;
}

/**
 * 暂停human检测
 *
 * @param handle    [in]   human检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_fusion_detect_pause(vp_fusion_detect_handle_p handle, vp_algorithm_type_t type) {
    (void) handle;
    (void) type;
    return 0;
}

/**
 * 销毁human检测句柄
 *
 * @param handle  [in]   human检测句柄
 */
void vp_fusion_detect_destroy(vp_fusion_detect_handle_p handle) {
    if (handle == NULL) return;
    if (handle == g_handle) {
        free(handle);
        g_handle = NULL;
    }
}