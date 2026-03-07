//
// Created by 杨柳 on 2024/11/22.
//

#include <string.h>
#include "vp_motion_detect.h"
#include "vp_time.h"
#include "vp_printf.h"
#include "ts_alg_motiondetect.h"

struct vp_motion_detect_handle {
    vp_motion_detect_param_t param;
    void* motion_detect_handle;
    // IMPFrameInfo frame;
    uint32_t width;
    uint32_t height;
    uint32_t sub_width;
    uint32_t sub_height;
    uint32_t area_width;
    uint32_t area_height;
    uint64_t timestamp;
    uint8_t *data;
} vp_motion_detect_handle_t;


/**
 * 创建motion检测句柄
 *
 * @param width   [in]   图像宽度
 * @param height  [in]   图像高度
 * @param param   [in]   motion检测参数
 *
 * @return motion检测句柄
 */
vp_motion_detect_handle_p vp_motion_detect_create(uint32_t width, uint32_t height, vp_motion_detect_param_t *param) {
    vp_motion_detect_handle_p handle = calloc(1, sizeof(vp_motion_detect_handle_t));
    if (handle == NULL) return NULL;
    if (param->sensitivity > 3) param->sensitivity = 3;
    memcpy(&handle->param, param, sizeof(vp_motion_detect_param_t));
    handle->width = width;
    handle->height = height;
    handle->area_width = width / 22;
    handle->area_height = height / 18;

    ALG_MOTION_DETECT_INIT_S motion_detect_param;
    motion_detect_param.u32ImgW = width;
    motion_detect_param.u32ImgH = height + 24;
    motion_detect_param.u32ImgC = 4;
    int ret = TS_ALG_MotionDetect_Init(&handle->motion_detect_handle, &motion_detect_param);
    if (ret) {
        vp_error("Failed to call TS_ALG_MotionDetect_Init with %d.\n", ret);
        free(handle);
        return NULL;
    }

    TS_ALG_SetLogLevel(ALG_LOG_ERROR);

    return handle;
}

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
int vp_motion_detect_get_param(vp_motion_detect_handle_p handle, vp_motion_detect_param_t *param) {
    if (handle == NULL || param == NULL) return -1;
    memcpy(param, &handle->param, sizeof(vp_motion_detect_param_t));
    return 0;
}

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
int vp_motion_detect_set_param(vp_motion_detect_handle_p handle, vp_motion_detect_param_t *param) {
    if (handle == NULL || param == NULL || handle->motion_detect_handle) return -1;
    if (param->sensitivity > 3) param->sensitivity = 3;
    memcpy(&handle->param, param, sizeof(vp_motion_detect_param_t));
    ALG_MOTION_DETECT_PARAM_S motion_detect_params = { 0 };
    int ret = TS_ALG_MotionDetect_GetParam(handle->motion_detect_handle, &motion_detect_params);
    if (ret) {
        vp_error("Failed to call TS_ALG_MotionDetect_GetParam with %d.\n", ret);
        return -1;
    }

    //MOTION_DETECT.u32DiffThres越大越灵敏,MOTION_DETECT.u32AreaThres越小越灵敏
    switch (param->sensitivity) {
        case 0:
        motion_detect_params.u32DiffThres = 100;
        motion_detect_params.u32AreaThres = 200;
        break;
        case 1:
        motion_detect_params.u32DiffThres = 80;
        motion_detect_params.u32AreaThres = 300;
        break;
        case 2:
        motion_detect_params.u32DiffThres = 50;
        motion_detect_params.u32AreaThres = 400;
        break;
        case 3:
        motion_detect_params.u32DiffThres = 40;
		motion_detect_params.u32AreaThres = 600;
        break;
        default:
        motion_detect_params.u32DiffThres = 80;
        motion_detect_params.u32AreaThres = 300;
        break;
    }

    ret = TS_ALG_MotionDetect_SetParam(handle->motion_detect_handle, &motion_detect_params);
    if (ret) {
        vp_error("Failed to call TS_ALG_MotionDetect_SetParam with %d.\n", ret);
        return -1;
    }
    return 0;
}

// static int vp_detect_area_check(vp_motion_detect_handle_p handle, uint32_t x, uint32_t y) {
//     uint32_t index_x = x / handle->area_width;
//     uint32_t index_y = y / handle->area_height;
//     uint32_t area = handle->param.detect_area[index_y] & 0x3FFFFF;
//     if (area == 0x3FFFFF) return 1;
//     if (area == 0) return 0;
//     if (area & (1 << index_x)) return 1;
//     return 0;
// }

static int vp_detect_area_check(vp_motion_detect_handle_p handle,
    uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    uint32_t start_x = x / handle->area_width;
    uint32_t end_x = (x + width) / handle->area_width;

    uint32_t start_y = y / handle->area_height;
    uint32_t end_y = (y + height) / handle->area_height;

    uint32_t src_area = 0;
    for (uint32_t i = start_x; i <= end_x; ++i) {
        src_area |= (1 << i);
    }
    for (uint32_t i = start_y; i < end_y; ++i) {
        uint32_t area = handle->param.detect_area[i] & 0x3FFFFF;
        if (area == 0x3FFFFF || (src_area & area) != 0) return 1;
    }
    return 0;
}

/**
 * 执行motion检测
 *
 * @param handle    [in]   motion检测句柄
 * @param data      [in]   图像数据
 * @param result    [out]  检测结果
 * @param count     [in]   最大检测结果数量
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_motion_detect_process(vp_motion_detect_handle_p handle, vp_video_source_t *frame) {
    if (handle == NULL || frame == NULL || handle->motion_detect_handle == NULL) return -1;
    ALG_MOTION_DETECT_RESULT_S result = { 0 };
    ALG_IMAGE_S* pimage = (ALG_IMAGE_S*)frame->frame_data;
    int ret = TS_ALG_MotionDetect_Process(handle->motion_detect_handle, pimage, &result);
    if (ret) {
        vp_error("Failed to call TS_ALG_MotionDetect_Process with %d.\n", ret);
        return 0;
    }

    usleep(200 * 1000);

    int x, y, w, h, count = 0;
    for (int i = 0; i < result.u32ObjNum; ++i) {
        x = result.stBox[i].f32Xmin * handle->width;
        y = result.stBox[i].f32Ymin * handle->height;
        w = (result.stBox[i].f32Xmax - result.stBox[i].f32Xmin) * handle->width;
        h = (result.stBox[i].f32Ymax - result.stBox[i].f32Ymin) * handle->height;

        count += vp_detect_area_check(handle, x, y, w, h);
    }
    
    return count > 2;
}

/**
 * 获取motion检测结果
 *
 * @param handle    [in]   motion检测句柄
 * @param result    [out]  检测结果
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_motion_detect_result(vp_motion_detect_handle_p handle, vp_motion_detect_result_t *result) {
    (void) handle;
    (void) result;
    return 0;
}

/**
 * 恢复motion检测
 *
 * @param handle    [in]   motion检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_motion_detect_resume(vp_motion_detect_handle_p handle) {
    (void) handle;
    return 0;
}

/**
 * 暂停motion检测
 *
 * @param handle    [in]   motion检测句柄
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_motion_detect_pause(vp_motion_detect_handle_p handle) {
    (void) handle;
    return 0;
}

/**
 * 释放human检测结果
 *
 * @param handle    [in]   human检测句柄
 * @param result    [in]   检测结果
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_human_release_result(vp_human_detect_result_t *result) {
    return 0;
}

/**
 * 销毁motion检测句柄
 *
 * @param handle  [in]   motion检测句柄
 */
void vp_motion_detect_destroy(vp_motion_detect_handle_p handle) {
    if (handle) {
        if (handle->motion_detect_handle)
            TS_ALG_MotionDetect_Exit(handle->motion_detect_handle);
        free(handle);
    }
}