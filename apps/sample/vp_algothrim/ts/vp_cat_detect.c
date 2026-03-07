#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "vp_cat_detect.h"
#include "vp_printf.h"
#include "ts_alg_log.h"
#include "video_alg_catdetect-api.h"


#define CAT_DEFAULT_SCORE  (0.65f)
#define CAT_ACTIVITY_SCORE (0.65f)

static void* g_cat_detect_handle = NULL;
static pthread_mutex_t g_model_mutex = PTHREAD_MUTEX_INITIALIZER;
static int g_ref_cnt = 0;

struct vp_cat_detect_handle {
    vp_cat_detect_param_t param;
    void* cat_detect_handle;
    float score;
    uint32_t width;
    uint32_t height;
    uint32_t area_width;
    uint32_t area_height;
    uint64_t timestamp;
    vp_cat_detect_result_t result;
    uint32_t no_result_count;
    uint32_t update_flag;
    int skip_num;
    int record_frame_count;
    int vp_cat_no_checked_skip_num;
    int vp_cat_checked_skip_num;
    ALG_CatDetect_DET_RESULT_S cat_result;
    uint8_t cam_id;
};

typedef struct vp_cat_detect_handle vp_cat_detect_handle_t;

vp_cat_detect_handle_p vp_cat_detect_create(uint32_t width, uint32_t height, vp_cat_detect_param_t *param)
{
    if (width == 0 || height == 0) {
        vp_error("width height invalid\n");
        return NULL;
    }

    pthread_mutex_lock(&g_model_mutex);

    if (g_cat_detect_handle == NULL) {
        int ret = 0;

        ALG_CAT_MODEL_INIT_S model_param;
        memset(&model_param, 0, sizeof(model_param));

        model_param.yolo_model_cfg        = (TS_U8 *)"/model/yolov5_7_quantize_r.cfg";
        model_param.yolo_model_weight     = (TS_U8 *)"/model/yolov5_7_quantize_r.weight";
        model_param.embedding_model_cfg   = (TS_U8 *)"";
        model_param.embedding_model_weight= (TS_U8 *)"";
        model_param.food_model_cfg        = (TS_U8 *)"";
        model_param.food_model_weight     = (TS_U8 *)"";
        model_param.model_version_file    = (TS_U8 *)"";
        model_param.cat_callback          = NULL;
        model_param.userdata              = NULL;

        vp_error("===================== 开始加载模型 =====================");
        vp_error("cfg : %s", model_param.yolo_model_cfg);
        vp_error("weight: %s", model_param.yolo_model_weight);

        g_cat_detect_handle = NULL;
        ret = TS_ALG_CatDetect_Init((void**)&g_cat_detect_handle, &model_param);

        if (ret != 0) {
            vp_error("模型加载失败 ret=%d\n", ret);
            g_cat_detect_handle = NULL;
            pthread_mutex_unlock(&g_model_mutex);
            return NULL;
        }

        vp_error("===================== 模型加载成功 =====================");

        ALG_CatDetect_DET_PARAM_S config = {0};
        config.DetectionConfThres    = 0.65f;
        config.SimilarityThres_Day   = 0.6f;
        config.SimilarityThres_Night = 0.65f;
        config.EAT_Thres             = 0.25f;
        config.OUT_times             = 3;
        config.EAT_OUT_times         = 10;
        TS_ALG_CatDetect_SetParam(&config);

        CatSetPicDir("/tmp");
        CatConfigRenew("http://veepai-dev.eye4.cn:31110/api/v1/pets/getBySn", "OKB0505202RYRN");
        TS_ALG_SetLogLevel(ALG_LOG_ERROR);
    }

    g_ref_cnt++;

    vp_cat_detect_handle_p handle = calloc(1, sizeof(vp_cat_detect_handle_t));
    if (!handle) {
        g_ref_cnt--;
        pthread_mutex_unlock(&g_model_mutex);
        return NULL;
    }

    vp_cat_detect_handle_t* h = (vp_cat_detect_handle_t*)handle;
    h->cat_detect_handle = g_cat_detect_handle;
    h->width  = width;
    h->height = height;
    h->area_width  = width / 22;
    h->area_height = height / 18;
    h->vp_cat_no_checked_skip_num = 0;
    h->vp_cat_checked_skip_num    = 0;
    h->skip_num  = h->vp_cat_no_checked_skip_num;
    h->score     = CAT_DEFAULT_SCORE;
    h->update_flag       = 0;
    h->no_result_count   = 0;
    h->record_frame_count = 0;

    if (param) {
        memcpy(&h->param, param, sizeof(vp_cat_detect_param_t));
    }

    pthread_mutex_unlock(&g_model_mutex);

    return handle;
}

int vp_cat_detect_get_param(vp_cat_detect_handle_p handle, vp_cat_detect_param_t *param)
{
    if (!handle || !param) return -1;
    vp_cat_detect_handle_t* h = (vp_cat_detect_handle_t*)handle;
    memcpy(param, &h->param, sizeof(vp_cat_detect_param_t));
    return 0;
}

int vp_cat_detect_set_param(vp_cat_detect_handle_p handle, vp_cat_detect_param_t *param)
{
    if (!handle || !param) return -1;
    vp_cat_detect_handle_t* h = (vp_cat_detect_handle_t*)handle;
    memcpy(&h->param, param, sizeof(vp_cat_detect_param_t));
    return 0;
}

static int cat_info_cmp(const void *a, const void *b)
{
    vp_cat_obj_t *ia = (vp_cat_obj_t*)a;
    vp_cat_obj_t *ib = (vp_cat_obj_t*)b;
    if (ia->DetectionConf > ib->DetectionConf) return -1;
    if (ia->DetectionConf < ib->DetectionConf) return 1;
    return 0;
}

static void vp_cat_detect_set_skip_num(vp_cat_detect_handle_p handle, int value)
{
    if (handle) {
        vp_cat_detect_handle_t* h = (vp_cat_detect_handle_t*)handle;
        h->skip_num = value;
    }
}

static void vp_cat_detect_set_score(vp_cat_detect_handle_p handle, float value)
{
    if (handle) {
        vp_cat_detect_handle_t* h = (vp_cat_detect_handle_t*)handle;
        h->score = value;
    }
}

int vp_cat_detect_process(vp_cat_detect_handle_p handle, ALG_IMAGE_S *rgba_image, uint8_t *yuv_data, uint8_t cam_id)
{
    if (!handle || !rgba_image || !yuv_data || !rgba_image->pData) {
        vp_debug("vp_cat_detect_process: image or data null\n");
        return 0;
    }

    vp_cat_detect_handle_t* h = (vp_cat_detect_handle_t*)handle;
    if (!h || !h->cat_detect_handle) {
        vp_error("vp_cat_detect_process: handle is null !!!\n");
        return -1;
    }

    h->cam_id = cam_id;

    h->record_frame_count++;
    if (h->record_frame_count < h->skip_num) {
        return h->result.count > 0 ? 1 : 0;
    }
    h->record_frame_count = 0;

    ALG_CatDetect_DET_RESULT_S* result = &h->cat_result;
    ALG_IMAGE_S yuv_image = {0};
    
    yuv_image.s32H = h->height;
    yuv_image.s32W = h->width;
    yuv_image.s32C = 1;
    yuv_image.pData = yuv_data;
    yuv_image.pDataPhy = 0;

    int ret = VIDEO_ALG_CatDetect_Proc(h->cat_detect_handle, &yuv_image, rgba_image, result, h->cam_id);

    if (ret != 0) {
        vp_error("CatDetect_Proc fail: %d\n", ret);
        return 0;
    }

    h->result.count = 0;
    for (int i = 0; i < result->u32ObjNum && h->result.count < VP_MAX_CAT_DET_NUM; i++) {
        if (result->stBox[i].DetectionConf >= h->score) {
            vp_cat_obj_t *cat = &h->result.objs[h->result.count++];
            cat->f32Xmin         = result->stBox[i].f32Xmin;
            cat->f32Ymin         = result->stBox[i].f32Ymin;
            cat->f32Xmax         = result->stBox[i].f32Xmax;
            cat->f32Ymax         = result->stBox[i].f32Ymax;
            cat->DetectionConf   = result->stBox[i].DetectionConf;
            cat->MaxSimilarity   = result->stBox[i].MaxSimilarity;
            memcpy(cat->nameid, result->stBox[i].nameid, 64);
            cat->act             = (vp_cat_act_t)result->stBox[i].act;
            cat->class_id        = (vp_cat_class_id_t)result->stBox[i].class_id;
            cat->first_in        = result->stBox[i].first_in;
            cat->first_eat       = result->stBox[i].first_eat;
            cat->cam_id          = h->cam_id;
            cat->act_cat         = result->stBox[i].act_cat;
            cat->cat_first_in    = result->stBox[i].cat_first_in;
            cat->cat_first_eat   = result->stBox[i].cat_first_eat;
        }
    }
    
    qsort(h->result.objs, h->result.count, sizeof(vp_cat_obj_t), cat_info_cmp);
    
    if (h->result.count == 0) {
        h->no_result_count++;
        if (h->no_result_count > 10000) h->no_result_count = 21;
        if (h->no_result_count > 20 && h->update_flag) {
            vp_cat_detect_set_score(handle, CAT_DEFAULT_SCORE);
            vp_cat_detect_set_skip_num(handle, h->vp_cat_no_checked_skip_num);
            h->update_flag = 0;
        }
        return 0;
    } else {
        h->no_result_count = 0;
        if (!h->update_flag) {
            vp_cat_detect_set_score(handle, CAT_ACTIVITY_SCORE);
            vp_cat_detect_set_skip_num(handle, h->vp_cat_checked_skip_num);
            h->update_flag = 1;
        }
        return 1;
    }
}

int vp_cat_detect_result(vp_cat_detect_handle_p handle, vp_cat_detect_result_t *result)
{
    if (!handle || !result) return -1;
    vp_cat_detect_handle_t* h = (vp_cat_detect_handle_t*)handle;
    memcpy(result, &h->result, sizeof(vp_cat_detect_result_t));
    return 0;
}

int vp_cat_detect_resume(vp_cat_detect_handle_p handle) { return 0; }
int vp_cat_detect_pause(vp_cat_detect_handle_p handle)  { return 0; }

int vp_cat_detect_should_skip_preprocess(vp_cat_detect_handle_p handle)
{
    if (!handle) return 0;
    vp_cat_detect_handle_t* h = (vp_cat_detect_handle_t*)handle;
    return (h->skip_num > 0 && h->record_frame_count < h->skip_num) ? 1 : 0;
}

void vp_cat_detect_destroy(vp_cat_detect_handle_p handle)
{
    if (!handle) return;

    pthread_mutex_lock(&g_model_mutex);

    vp_cat_detect_handle_t* h = (vp_cat_detect_handle_t*)handle;
    h->cat_detect_handle = NULL;

    g_ref_cnt--;
    if (g_ref_cnt == 0 && g_cat_detect_handle) {
        vp_error("===================== 开始释放模型 =====================");
        VIDEO_ALG_CatDetect_Exit(g_cat_detect_handle);
        g_cat_detect_handle = NULL;
        vp_error("===================== 模型释放成功 =====================");
    }

    pthread_mutex_unlock(&g_model_mutex);

    free(handle);
}
