//
// Created by 杨柳 on 2024/11/22.
//

#include <string.h>
#include "vp_human_detect.h"
#include "vp_printf.h"
#include "ts_alg_face_detect.h"
#include <stdio.h>
#include "ts_alg_log.h"
#include "vp_time.h"

#define HUMAN_DEFAULT_SCORE  (0.0f) //默认待机检测使用的得分
#define HUMAN_ACTIVITY_SCORE (0.0f) //检测到人后追踪使用的得分

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

static void* g_human_detect_handle = NULL;

struct vp_human_detect_handle {
    vp_human_detect_param_t param;
    void* human_detect_handle;
    float score;
    uint32_t width;
    uint32_t height;
    uint32_t area_width;
    uint32_t area_height;
    uint64_t timestamp;
    vp_human_detect_result_t result;
    /* 无检测结果次数 */
    uint32_t no_result_count;
    uint32_t update_flag;
    int skip_num;
    int record_frame_count;
    int vp_human_no_checked_skip_num;
    int vp_human_checked_skip_num;
} vp_human_detect_handle_t;

static TS_U32 vp_human_get_file_size(FILE *pf)
{
	TS_U32 fileSize = 0;
	if (NULL != pf) {
		if (0 == fseek(pf, 0, SEEK_END)) {
			fileSize = ftell(pf);
		}
		rewind(pf);
	}
	return fileSize;
}

/**
 * 创建human检测句柄
 *
 * @param width   [in]   图像宽度
 * @param height  [in]   图像高度
 * @param param   [in]   human检测参数
 *
 * @return human检测句柄
 */
vp_human_detect_handle_p vp_human_detect_create(uint32_t width, uint32_t height, vp_human_detect_param_t *param) {

    if (g_human_detect_handle == NULL) {
        const TS_U8 weightpath[] = "facedetect_weight.weight";
        const TS_U8 cfgpath[] = "facedetect_cfg.cfg";

        FILE *pf_cfg = NULL;
        FILE *pf_weight = NULL;

        int ret = 0;

        pf_cfg = fopen((const char *)cfgpath, "rb");
        pf_weight = fopen((const char *)weightpath, "rb");

        if ((NULL == pf_cfg) || (NULL == pf_weight)) {
            vp_error("pcpp model file fopen failed !!!\n");
            vp_error("model_cfg: %s, model_weight:%s\n", cfgpath,  weightpath);
            return NULL;
        }

        TS_U32 u32WeightLen = vp_human_get_file_size(pf_weight);
        TS_U32 u32CfgLen = vp_human_get_file_size(pf_cfg);

        TS_U8 *pWeightBuf = (TS_U8 *)malloc(u32WeightLen);
        TS_U8 *pCfgBuf = (TS_U8 *)malloc(u32CfgLen);
        if (NULL == pWeightBuf || NULL == pCfgBuf) {
            vp_error("failed to malloc memory. weight-%p, cfg-%p.\n", pWeightBuf,  pCfgBuf);
            free(pWeightBuf);
            free(pCfgBuf);
            fclose(pf_weight);
            fclose(pf_cfg);
            return NULL;
        }

        fread(pWeightBuf, u32WeightLen, 1, pf_weight);
        fread(pCfgBuf, u32CfgLen, 1, pf_cfg);
        fclose(pf_weight);
        fclose(pf_cfg);

        ALG_MODEL_INIT_S model_param;
        memset(&model_param, 0, sizeof(ALG_MODEL_INIT_S));
        model_param.pGraph = pCfgBuf;
        model_param.pWeight = pWeightBuf;
        model_param.u32GraphSize = u32CfgLen;
        model_param.u32WeightSize = u32WeightLen;
        model_param.eImageType = ALG_IMAGE_TYPE_INT_HWC_RGB0;
        model_param.bRneOff = 0;

        ret = TS_ALG_FaceDetect_Init(&g_human_detect_handle, &model_param);
        free(pCfgBuf);
        free(pWeightBuf);

        if (ret) {
            vp_error("Failed to init pcpp detect with %d.\n", ret);
            return NULL;
        }

        TS_ALG_SetLogLevel(ALG_LOG_ERROR);
    }

    vp_human_detect_handle_p handle = calloc(1, sizeof(vp_human_detect_handle_t));
    if (handle == NULL) return NULL;
    handle->vp_human_no_checked_skip_num = 0;

    if (param->enable_track == 0) {
        handle->vp_human_no_checked_skip_num = 0;
        handle->vp_human_checked_skip_num = 0;
    }

    handle->human_detect_handle = g_human_detect_handle;
    handle->width = width;
    handle->height = height;
    handle->area_width = width / 22;
    handle->area_height = height / 18;
    handle->skip_num = handle->vp_human_no_checked_skip_num;
	
    return handle;
}

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
int vp_human_detect_get_param(vp_human_detect_handle_p handle, vp_human_detect_param_t *param) {
    if (handle == NULL || param == NULL) return -1;
    memcpy(param, &handle->param, sizeof(vp_human_detect_param_t));
    return 0;
}

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
int vp_human_detect_set_param(vp_human_detect_handle_p handle, vp_human_detect_param_t *param) {
    if (handle == NULL || param == NULL) return -1;
    memcpy(&handle->param, param, sizeof(vp_human_detect_param_t));
    return 0;
}

static int vp_detect_area_check(vp_human_detect_handle_p handle,
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

static int person_info_cmp(const void *a, const void *b) {
    vp_human_obj_t *ia = (vp_human_obj_t *) a;
    vp_human_obj_t *ib = (vp_human_obj_t *) b;
    if (ia->score > ib->score) return -1;
    if (ia->score < ib->score) return 1;
    return 0;
}

static void vp_human_detect_set_skip_num(vp_human_detect_handle_p handle, int value) {
    if (handle) handle->skip_num = value;
}

static void vp_human_detect_set_score(vp_human_detect_handle_p handle, float value) {
    if (handle) handle->score = value;
}


/**
 * 执行human检测
 *
 * @param handle    [in]   human检测句柄
 * @param data      [in]   图像数据
 * @param result    [out]  检测结果
 * @param count     [in]   最大检测结果数量
 *
 * @return 错误码
 * @retval >0 检测结果数量
 * @retval =0 无检测结果
 * @retval !0 失败
 */
int vp_human_detect_process(vp_human_detect_handle_p handle, vp_video_source_t *frame) {
    if (handle == NULL || frame == NULL || handle->human_detect_handle == NULL) return -1;
    ALG_IMAGE_S* pimage = (ALG_IMAGE_S*)frame->frame_data;
    ALG_FACE_DETECT_RESULT_S result = { 0 };
    handle->record_frame_count++;

    if (handle->record_frame_count < handle->skip_num) {
        usleep(30 * 1000);
        return handle->result.count;
    } else {
        int ret = TS_ALG_FaceDetect_Process(handle->human_detect_handle, pimage, &result);
        if (ret) {
            vp_error("Failed to call TS_ALG_FaceDetect_Process.\n");
            return 0;
        }
        handle->record_frame_count = 0;
    }

    handle->result.count = 0;
    vp_human_obj_t *human = NULL;
    int x, y, w, h;
    for (int i = 0; i < result.u32FaceNum; ++i) {
        if (result.stBox[i].f32Score >= handle->score) {
            x = result.stBox[i].f32Xmin * handle->width;
            y = result.stBox[i].f32Ymin * handle->height;
            w = (result.stBox[i].f32Xmax - result.stBox[i].f32Xmin) * handle->width;
            h = (result.stBox[i].f32Ymax - result.stBox[i].f32Ymin) * handle->height;
            human = &handle->result.objs[handle->result.count];
            human->score = result.stBox[i].f32Score;
            human->track_id = i;
            human->rect.x = x;
            human->rect.y = y;
            human->rect.w = w;
            human->rect.h = h;
            handle->result.count += 1;
            if (handle->result.count >= 15) break;
        }
    }

    qsort(handle->result.objs, handle->result.count, sizeof(vp_human_obj_t), person_info_cmp);
    if (handle->result.count == 0) {
        handle->result.count = 0;
        handle->no_result_count++;
        if (handle->no_result_count > 10000) handle->no_result_count = 21;
        if (handle->no_result_count > 20 && handle->update_flag == 1) {
            vp_human_detect_set_score(handle, HUMAN_DEFAULT_SCORE);
            vp_human_detect_set_skip_num(handle, handle->vp_human_no_checked_skip_num);
            handle->update_flag = 0;
        }

        return 0;
    } else {
        handle->no_result_count = 0;
        if (handle->update_flag == 0) {
            vp_human_detect_set_score(handle, HUMAN_ACTIVITY_SCORE);
            vp_human_detect_set_skip_num(handle, handle->vp_human_checked_skip_num);
            handle->update_flag = 1;
            handle->no_result_count = 0;
        }

        return 1;
    }
}

/**
 * 获取human检测结果
 *
 * @param handle    [in]   human检测句柄
 * @param result    [out]  检测结果
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_human_detect_result(vp_human_detect_handle_p handle, vp_human_detect_result_t *result) {
    if (handle == NULL) return -1;
    memcpy(result, &handle->result, sizeof(vp_human_detect_result_t));
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
int vp_human_detect_resume(vp_human_detect_handle_p handle) {
    (void) handle;
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
int vp_human_detect_pause(vp_human_detect_handle_p handle) {
    (void) handle;
    return 0;
}

/**
 * 销毁human检测句柄
 *
 * @param handle  [in]   human检测句柄
 */
void vp_human_detect_destroy(vp_human_detect_handle_p handle) {
    if (handle) {
        free(handle);
    }
}