#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "vp_algorithm.h"
#include "sample_comm.h"
// #include "vp_sensor_config.h"
#include "vp_video_coder_type.h"
#include "vp_video_encoder.h"
#include "vp_events.h"
#include "vp_printf.h"
#include "vp_video_osd.h"
#include "vp_time.h"
#include "vp_lock.h"
#include "vp_observers.h"
#include "vp_pthread.h"
// #include "vp_status_param.h"
#include "vp_motion_detect.h"
#include "ts/vp_cat_detect.h"
#include "vp_convergence_detect.h"
#include "vp_context_config.h"
#include "vp_fusion_detect.h"
#include "vp_multiobject_detect.h"
#include "ts_alg_type.h"
#include "ts_alg_imgproc.h"
// #include "ts_alg_singletargettrack.h"

#define VP_ALGORITHM_TYPE           1
#define VP_ALGORITHM_TYPE_TS        1

#if VP_ALGORITHM_TYPE == VP_ALGORITHM_TYPE_YTLD

#include "ytld/vp_algorithm_ytld.h"

#endif
#if VP_PLATFORM_IS_JZ(VP_SDK_PLATFORM)

#include "vp_track.h"

#if VP_SDK_PLATFORM == VP_SDK_PLATFORM_T23

#include "T23/vp_video_encoder.h"

#endif

#endif

// static void * g_singletrack_handle = NULL;

#define SAVE_RGBA_PATH  "/mnt/sda0/rgba/"
#define SAVE_RGBA_ENABLE 0  // 1=启用保存，0=禁用保存
#define SAVE_RGBA_INTERVAL_BASE 30  // 初始保存间隔（帧数）
#define SAVE_RGBA_INTERVAL_STEP 30  // 每保存200张后增加的间隔
#define SAVE_RGBA_BATCH_SIZE 200  // 累计保存多少张后增加间隔
#define SAVE_RGBA_CROP_PADDING 0  // 1=裁剪填充区域保存640x360，0=保存完整640x384
#define SAVE_RGB_FORMAT 1  // 1=保存为RGB三通道，0=保存为RGBA四通道

#define LOAD_RGBA_FROM_SD 0  // 1=从SD卡读取固定RGBA文件，0=使用实时图像
#define LOAD_RGBA_FILE_PATH "/mnt/sda0/rgba/cam1_rgba_120.rgba"  // 要读取的RGBA文件路径

typedef struct {
    volatile uint8_t enable: 1;
    volatile uint8_t pause: 1;
    volatile uint8_t update: 1;
    volatile uint8_t notify: 1;
    volatile uint8_t paused: 1;
    volatile uint8_t has_notify: 1;
    uint64_t last_notify;
    uint64_t detect_timestamp;
    vp_algorithm_param_t param;
    vp_algorithm_result_t result;
    void *handle;
} vp_algorithm_info_t;

typedef struct {
    uint16_t frame_width;
    uint16_t frame_height;

    uint32_t data_size;
    uint32_t jpeg_size;
    uint8_t *jpeg_data;

    vp_events_p events;
    uint64_t human_timestamp;
    vp_lock_t lock;
    vp_algorithm_info_t infos[VP_ALGORITHM_TYPE_MAX];
} vp_algorithm_channel_t;

typedef struct {
    vp_algorithm_state_t state;
    vp_events_p events;
    // vp_track_opt_t track;
    // vp_track_obj_t track_objs[15];
    vp_algorithm_channel_t channels[VP_SENSOR_NUM][vp_video_chn_max];
} vp_algorithm_context_t;

static vp_algorithm_context_t g_context = {0};
static volatile int g_shutting_down = 0;
static pthread_t g_notify_thread = 0;

typedef struct {
    uint8_t visible;
} vp_mosaic_state_t;

typedef struct {
    uint8_t visible;
} vp_box_state_t;

static vp_mosaic_state_t g_human_mosaic_states[VP_VIDEO_OSD_MOSAIC_MAX];
static vp_mosaic_state_t g_cat_mosaic_states[VP_VIDEO_OSD_MOSAIC_MAX];
static vp_box_state_t g_human_box_states[VP_VIDEO_OSD_RECT_MAX];
static vp_box_state_t g_cat_box_states[VP_VIDEO_OSD_RECT_MAX];

static void vp_algorithm_init_states() {
    for (int i = 0; i < VP_VIDEO_OSD_MOSAIC_MAX; ++i) {
        g_human_mosaic_states[i].visible = 0;
        g_cat_mosaic_states[i].visible = 0;
    }
    for (int i = 0; i < VP_VIDEO_OSD_RECT_MAX; ++i) {
        g_human_box_states[i].visible = 0;
        g_cat_box_states[i].visible = 0;
    }
}

// 获取当前时间戳（毫秒）
static uint64_t get_timestamp_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static int vp_algorithm_human_draw_boxes(uint8_t idx, vp_video_chn_t chn, uint32_t width, uint32_t height,
                                         vp_human_detect_result_t *result) {

    vp_video_encoder_config_t config;
    if (vp_video_encoder_get_config(idx, chn, &config) != 0) return -1;
    vp_area_rect_t *rect;
    
    for (int i = 0; i < VP_VIDEO_OSD_RECT_MAX; ++i) {
        if (i < result->count) {
            rect = &result->objs[i].rect;
            uint32_t dest_x = rect->x, dest_y = rect->y;
            uint32_t dest_w = rect->w, dest_h = rect->h;
            if (config.width != width) {
                dest_x = rect->x * config.width / width;
                dest_w = rect->w * config.width / width;
            }
            if (config.height != height) {
                dest_y = rect->y * config.height / height;
                dest_h = rect->h * config.height / height;
            }
            
            if (dest_x >= config.width) dest_x = config.width - 1;
            if (dest_y >= config.height) dest_y = config.height - 1;
            if (dest_x + dest_w > config.width) dest_w = config.width - dest_x;
            if (dest_y + dest_h > config.height) dest_h = config.height - dest_y;
            if (dest_w < 10) dest_w = 10;
            if (dest_h < 10) dest_h = 10;
            
            vp_video_osd_show_rect(idx, chn, i);
            g_human_box_states[i].visible = 1;
            
            vp_video_osd_update_rect(idx, chn, i, dest_x, dest_y, dest_w, dest_h);
        } else {
            if (g_human_box_states[i].visible) {
                vp_video_osd_hide_rect(idx, chn, i);
                g_human_box_states[i].visible = 0;
            }
        }
    }
    
    return 0;
}

static int vp_algorithm_human_draw_mosaic(uint8_t idx, vp_video_chn_t chn, uint32_t width, uint32_t height,
                                         vp_human_detect_result_t *result) {

    vp_video_encoder_config_t config;
    if (vp_video_encoder_get_config(idx, chn, &config) != 0) return -1;
    vp_area_rect_t *rect;
    
    for (int i = 0; i < VP_VIDEO_OSD_MOSAIC_MAX; ++i) {
        if (i < result->count) {
            rect = &result->objs[i].rect;
            uint32_t dest_x = rect->x, dest_y = rect->y;
            uint32_t dest_w = rect->w, dest_h = rect->h;
            if (config.width != width) {
                dest_x = rect->x * config.width / width;
                dest_w = rect->w * config.width / width;
            }
            if (config.height != height) {
                dest_y = rect->y * config.height / height;
                dest_h = rect->h * config.height / height;
            }
            
            if (dest_x >= config.width) dest_x = config.width - 1;
            if (dest_y >= config.height) dest_y = config.height - 1;
            if (dest_x + dest_w > config.width) dest_w = config.width - dest_x;
            if (dest_y + dest_h > config.height) dest_h = config.height - dest_y;
            if (dest_w < 10) dest_w = 10;
            if (dest_h < 10) dest_h = 10;
            
            vp_video_osd_show_mosaic(idx, chn, i);
            g_human_mosaic_states[i].visible = 1;
            
            vp_video_osd_update_mosaic(idx, chn, i, dest_x, dest_y, dest_w, dest_h);
        } else {
            if (g_human_mosaic_states[i].visible) {
                vp_video_osd_hide_mosaic(idx, chn, i);
                g_human_mosaic_states[i].visible = 0;
            }
        }
    }
    
    return 0;
}

static void vp_algorithm_human_clear_boxes(uint8_t idx, vp_video_chn_t chn) {
    for (int i = 0; i < VP_VIDEO_OSD_RECT_MAX; ++i) {
        vp_video_osd_hide_rect(idx, chn, i);
    }
}

static void vp_algorithm_cat_clear_boxes(uint8_t idx, vp_video_chn_t chn) {
    for (int i = 0; i < VP_VIDEO_OSD_RECT_MAX; ++i) {
        vp_video_osd_hide_rect(idx, chn, i);
    }
}

static void vp_algorithm_human_clear_mosaic(uint8_t idx, vp_video_chn_t chn) {
    for (int i = 0; i < VP_VIDEO_OSD_MOSAIC_MAX; ++i) {
        vp_video_osd_hide_mosaic(idx, chn, i);
    }
}

static void vp_algorithm_cat_clear_mosaic(uint8_t idx, vp_video_chn_t chn) {
    for (int i = 0; i < VP_VIDEO_OSD_MOSAIC_MAX; ++i) {
        vp_video_osd_hide_mosaic(idx, chn, i);
    }
}

static int vp_algorithm_cat_draw_boxes(uint8_t idx, vp_video_chn_t chn, uint32_t width, uint32_t height,
                                      vp_cat_detect_result_t *result) 
{
    if (g_shutting_down) return -1;
    if (width == 0 || height == 0) return -1;
    vp_video_encoder_config_t config;
    if (vp_video_encoder_get_config(idx, chn, &config) != 0) return -1;
    
    for (int i = 0; i < VP_VIDEO_OSD_RECT_MAX; ++i) {
        if (i < result->count) {
            vp_cat_obj_t *obj = &result->objs[i];
            uint32_t dest_x = obj->Xmin * config.width;
            uint32_t dest_y = obj->Ymin * config.height;
            uint32_t dest_w = (obj->Xmax - obj->Xmin) * config.width;
            uint32_t dest_h = (obj->Ymax - obj->Ymin) * config.height;
            
            if (dest_x >= config.width) dest_x = config.width - 1;
            if (dest_y >= config.height) dest_y = config.height - 1;
            if (dest_x + dest_w > config.width) dest_w = config.width - dest_x;
            if (dest_y + dest_h > config.height) dest_h = config.height - dest_y;
            if (dest_w < 10) dest_w = 10;
            if (dest_h < 10) dest_h = 10;
            
            vp_video_osd_show_rect(idx, chn, i);
            g_cat_box_states[i].visible = 1;
            
            vp_video_osd_update_rect(idx, chn, i, dest_x, dest_y, dest_w, dest_h);
        } else {
            if (g_cat_box_states[i].visible) {
                vp_video_osd_hide_rect(idx, chn, i);
                g_cat_box_states[i].visible = 0;
            }
        }
    }
    
    return 0;
}

static int vp_algorithm_cat_draw_mosaic(uint8_t idx, vp_video_chn_t chn, uint32_t width, uint32_t height,
                                      vp_cat_detect_result_t *result) {
    return 0;
    if (g_shutting_down) return -1;
    if (width == 0 || height == 0) return -1;
    vp_video_encoder_config_t config;
    if (vp_video_encoder_get_config(idx, chn, &config) != 0) return -1;
    
    for (int i = 0; i < VP_VIDEO_OSD_MOSAIC_MAX; ++i) {
        if (i < result->count) {
            vp_cat_obj_t *obj = &result->objs[i];
            uint32_t dest_x = obj->Xmin * config.width;
            uint32_t dest_y = obj->Ymin * config.height;
            uint32_t dest_w = (obj->Xmax - obj->Xmin) * config.width;
            uint32_t dest_h = (obj->Ymax - obj->Ymin) * config.height;
            
            if (dest_x >= config.width) dest_x = config.width - 1;
            if (dest_y >= config.height) dest_y = config.height - 1;
            if (dest_x + dest_w > config.width) dest_w = config.width - dest_x;
            if (dest_y + dest_h > config.height) dest_h = config.height - dest_y;
            if (dest_w < 10) dest_w = 10;
            if (dest_h < 10) dest_h = 10;
            
            vp_video_osd_show_mosaic(idx, chn, i);
            g_cat_mosaic_states[i].visible = 1;
            
            vp_video_osd_update_mosaic(idx, chn, i, dest_x, dest_y, dest_w, dest_h);
        } else {
            if (g_cat_mosaic_states[i].visible) {
                vp_video_osd_hide_mosaic(idx, chn, i);
                g_cat_mosaic_states[i].visible = 0;
            }
        }
    }
    
    return 0;
}

static int vp_algorithm_detect_init(vp_algorithm_ivs_args_t *args) {
    (void) args;
    vp_debug("start");
    
    // 初始化状态数组
    vp_algorithm_init_states();
    
#if VP_ALGORITHM_TYPE == VP_ALGORITHM_TYPE_YTLD
    while (vp_algorithm_ytld_load()) {
        sleep(1);
    }
#endif
    vp_debug("end");

    ALG_IMAGE_S* pimage = calloc(1, sizeof(ALG_IMAGE_S));
    if (pimage == NULL) return -1;

    if (pimage != NULL && pimage->pData == NULL) {
        pimage->s32W = 640;
        pimage->s32H = 384;//384;
        pimage->s32C = 4;
        int alg_rgba_size = pimage->s32W * pimage->s32H * pimage->s32C;
        if (TS_MPI_SYS_MmzAlloc_Cached(&(pimage->pDataPhy), &(pimage->pData), NULL, NULL, alg_rgba_size)) {
            free(pimage);
            vp_error("Failed to call TS_MPI_SYS_MmzAlloc_Cached.\n");
            return -1;
        }

        memset(pimage->pData, 114, alg_rgba_size);
    }

    args->user_data = pimage;

    // if (TS_ALG_SingleTgtTrack_Init(&g_singletrack_handle)) {
    //     vp_error("Failed to Call TS_ALG_SingleTgtTrack_Init.\n");
    //     g_singletrack_handle = NULL;
    // }


    return 0;
}

static void vp_algorithm_detect_pause(vp_algorithm_type_t type, vp_algorithm_info_t *info) {
    int ret = 0;
    switch (type) {
#if VP_ALGORITHM_FUSION && VP_ALGORITHM_HUMAN
        case VP_ALGORITHM_TYPE_HUMAN_DETECT:
        case VP_ALGORITHM_TYPE_MOTION_DETECT: {
            vp_fusion_detect_pause(info->handle, type);
        }
#elif VP_ALGORITHM_MULTIOBJECT && VP_ALGORITHM_HUMAN
            case VP_ALGORITHM_TYPE_HUMAN_DETECT:
            case VP_ALGORITHM_TYPE_MOTION_DETECT: {
                vp_multiobject_detect_pause(info->handle, type);
            }
#else
            case VP_ALGORITHM_TYPE_MOTION_DETECT: {
                ret = vp_motion_detect_pause(info->handle);
                break;
            }
            case VP_ALGORITHM_TYPE_HUMAN_DETECT:{
                ret = vp_cat_detect_pause(info->handle);
                break;
            }
#endif
        case VP_ALGORITHM_TYPE_CONVERGENCE_DETECT: {
            // ret = vp_convergence_detect_pause(info->handle);
            break;
        }
        case VP_ALGORITHM_TYPE_MAX:
        default:
            break;
    }
    vp_stack("ret:%d type:%d", ret, type);
    info->paused = 1;
}

static void vp_algorithm_detect_resume(vp_algorithm_type_t type, vp_algorithm_info_t *info) {
    int ret = 0;
    switch (type) {
#if VP_ALGORITHM_FUSION && VP_ALGORITHM_HUMAN
        case VP_ALGORITHM_TYPE_HUMAN_DETECT:
        case VP_ALGORITHM_TYPE_MOTION_DETECT: {
            vp_fusion_detect_resume(info->handle, type);
            break;
        }
#elif VP_ALGORITHM_MULTIOBJECT && VP_ALGORITHM_HUMAN
            case VP_ALGORITHM_TYPE_HUMAN_DETECT:
            case VP_ALGORITHM_TYPE_MOTION_DETECT: {
                vp_multiobject_detect_resume(info->handle, type);
                break;
            }
#else
            case VP_ALGORITHM_TYPE_MOTION_DETECT:
                ret = vp_motion_detect_resume(info->handle);
                break;
            case VP_ALGORITHM_TYPE_HUMAN_DETECT:
                ret = vp_cat_detect_resume(info->handle);
                break;
#endif
        case VP_ALGORITHM_TYPE_CONVERGENCE_DETECT:
            // ret = vp_convergence_detect_resume(info->handle);
            break;
        case VP_ALGORITHM_TYPE_MAX:
        default:
            break;
    }
    vp_stack("ret:%d type:%d", ret, type);
    info->paused = 0;
}

static int vp_algorithm_detect_before(uint8_t idx, uint8_t chn, vp_algorithm_ivs_args_t *args,
                                      vp_video_source_t *frame) {
    (void) args;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    vp_events_clear(channel->events, VP_EVENT_BIT(VP_ALGORITHM_TYPE_MAX));
    vp_algorithm_info_t *info;
    vp_algorithm_param_t *param;
    uint8_t has_jpeg = 0;
    for (int i = 0; i < VP_ALGORITHM_TYPE_MAX; ++i) {
        info = &channel->infos[i];
        if (info->enable == 0 || info->pause) continue;
        param = &info->param;
        switch (i) {
#if VP_ALGORITHM_FUSION && VP_ALGORITHM_HUMAN
            case VP_ALGORITHM_TYPE_MOTION_DETECT: {
                if (info->handle == NULL) {
                    info->result.type = i;
                    info->handle = vp_fusion_detect_create(frame->width, frame->height);
                    vp_stack("create idx:%d chn:%d motion handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                             info->handle, param->notify, param->interval, param->jpeg);
                    vp_fusion_detect_set_motion_param(info->handle, &info->param.motion);
                } else {
                    if (info->update) {
                        vp_fusion_detect_set_motion_param(info->handle, &info->param.motion);
                        vp_stack("update idx:%d chn:%d motion handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                                 info->handle, param->notify, param->interval, param->jpeg);
                        info->update = 0;
                    }
                }
                break;
            }
            case VP_ALGORITHM_TYPE_HUMAN_DETECT: {
                if (info->handle == NULL) {
                    info->result.type = i;
                    info->handle = vp_fusion_detect_create(frame->width, frame->height);
                    vp_stack("create idx:%d chn:%d human handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                             info->handle, param->notify, param->interval, param->jpeg);
                    vp_fusion_detect_set_human_param(info->handle, &info->param.human);
                    if (info->param.human.draw_box) {
                        vp_osd_config_rect_t rect = {
                                .color = VP_OSD_RECT_GREEN,
                        };
                        for (int j = 0; j < VP_VIDEO_OSD_RECT_MAX; ++j) {
                            rect.line = 12;
                            vp_video_osd_config_rect(idx, vp_video_chn_main, j, &rect);
                            rect.line = 1;
                            vp_video_osd_config_rect(idx, vp_video_chn_sec, j, &rect);
                            vp_video_osd_config_rect(idx, vp_video_chn_thr, j, &rect);
                        }
                    }
                } else {
                    if (info->update) {
                        vp_fusion_detect_set_human_param(info->handle, &info->param.human);
                        vp_stack("update idx:%d chn:%d human handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                                 info->handle, param->notify, param->interval, param->jpeg);
                        info->update = 0;
                        if (info->param.human.draw_box) {
                            vp_osd_config_rect_t rect = {
                                    .color = VP_OSD_RECT_GREEN,
                            };
                            for (int j = 0; j < VP_VIDEO_OSD_RECT_MAX; ++j) {
                                rect.line = 12;
                                vp_video_osd_config_rect(idx, vp_video_chn_main, j, &rect);
                                rect.line = 1;
                                vp_video_osd_config_rect(idx, vp_video_chn_sec, j, &rect);
                                vp_video_osd_config_rect(idx, vp_video_chn_thr, j, &rect);
                            }
                        }
                    }
                }
                break;
            }

#elif VP_ALGORITHM_MULTIOBJECT && VP_ALGORITHM_HUMAN
            case VP_ALGORITHM_TYPE_MOTION_DETECT: {
                if (info->handle == NULL) {
                    info->result.type = i;
                    info->handle = vp_multiobject_detect_create(frame->width, frame->height);
                    if (info->handle != NULL) {
                        vp_stack("T32 Multiobject create success, idx:%d chn:%d motion handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                                 info->handle, param->notify, param->interval, param->jpeg);
                        vp_multiobject_detect_set_motion_param(info->handle, &info->param.motion);
                    }else{
                        sleep(1);
                    }
                } else {
                    if (info->update) {
                        vp_multiobject_detect_set_motion_param(info->handle, &info->param.motion);
                        vp_stack("update idx:%d chn:%d motion handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                                 info->handle, param->notify, param->interval, param->jpeg);
                        info->update = 0;
                    }
                }
                break;
            }
            case VP_ALGORITHM_TYPE_HUMAN_DETECT: {
                if (info->handle == NULL) {
                    info->result.type = i;
                    info->handle = vp_multiobject_detect_create(frame->width, frame->height);
                    vp_multiobject_detect_set_human_param(info->handle, &info->param.human);
                    if (info->param.human.draw_box) {
                        vp_osd_config_rect_t rect = {
                                .color = VP_OSD_RECT_GREEN,
                        };
                        for (int j = 0; j < VP_VIDEO_OSD_RECT_MAX; ++j) {
                            rect.line = 12;
                            vp_video_osd_config_rect(idx, vp_video_chn_main, j, &rect);
                            rect.line = 1;
                            vp_video_osd_config_rect(idx, vp_video_chn_sec, j, &rect);
                            vp_video_osd_config_rect(idx, vp_video_chn_thr, j, &rect);
                        }
                    }
                } else {
                    if (info->update) {
                        vp_multiobject_detect_set_human_param(info->handle, &info->param.human);
                        // vp_stack("update idx:%d chn:%d human handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                        //           info->handle, param->notify, param->interval, param->jpeg);
                        info->update = 0;
                        if (info->param.human.draw_box) {
                            vp_osd_config_rect_t rect = {
                                    .color = VP_OSD_RECT_GREEN,
                            };
                            for (int j = 0; j < VP_VIDEO_OSD_RECT_MAX; ++j) {
                                rect.line = 12;
                                vp_video_osd_config_rect(idx, vp_video_chn_main, j, &rect);
                                rect.line = 1;
                                vp_video_osd_config_rect(idx, vp_video_chn_sec, j, &rect);
                                vp_video_osd_config_rect(idx, vp_video_chn_thr, j, &rect);
                            }
                        }
                    }
                }
                break;
            }
#else
                case VP_ALGORITHM_TYPE_MOTION_DETECT: {
                    if (info->handle == NULL) {
                        info->result.type = i;
                        info->handle = vp_motion_detect_create(frame->width, frame->height, &info->param.motion);
                        vp_stack("create idx:%d chn:%d motion handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                                 info->handle, param->notify, param->interval, param->jpeg);

                    } else {
                        if (info->update) {
                            vp_motion_detect_set_param(info->handle, &info->param.motion);
                            vp_stack("update idx:%d chn:%d motion handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                                     info->handle, param->notify, param->interval, param->jpeg);
                            info->update = 0;
                        }
                    }
                    break;
                }
                case VP_ALGORITHM_TYPE_HUMAN_DETECT: {
                    if (info->handle == NULL) {
                        info->result.type = i;
                        info->handle = vp_cat_detect_create(frame->width, frame->height, &info->param.cat);
                        vp_stack("create idx:%d chn:%d cat handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                                 info->handle, param->notify, param->interval, param->jpeg);
                        // 强制配置 OSD，确保划线和马赛克能够正常显示
                        vp_osd_config_rect_t rect = {
                                .color = VP_OSD_RECT_GREEN,
                        };
                        for (int j = 0; j < VP_VIDEO_OSD_RECT_MAX; ++j) {
                            rect.line = 3;
                            vp_video_osd_config_rect(idx, vp_video_chn_main, j, &rect);
                            rect.line = 1;
                            vp_video_osd_config_rect(idx, vp_video_chn_sec, j, &rect);
                            vp_video_osd_config_rect(idx, vp_video_chn_thr, j, &rect);
                        }
                        vp_osd_config_rect_t config = {0};
                        for (int j = 0; j < VP_VIDEO_OSD_MOSAIC_MAX; ++j) {
                            vp_video_osd_config_mosaic(idx, vp_video_chn_main, j, &config);
                            vp_video_osd_config_mosaic(idx, vp_video_chn_sec, j, &config);
                        }
                    } else {
                        if (info->update) {
                            vp_cat_detect_set_param(info->handle, &info->param.cat);
                            vp_stack("update idx:%d chn:%d cat handle:%p notify:%d interval:%d jpeg:%d", idx, chn,
                                     info->handle, param->notify, param->interval, param->jpeg);
                            info->update = 0;
                            // 强制配置 OSD，确保划线和马赛克能够正常显示
                            vp_osd_config_rect_t rect = {
                                    .color = VP_OSD_RECT_GREEN,
                            };
                            for (int j = 0; j < VP_VIDEO_OSD_RECT_MAX; ++j) {
                                rect.line = 2;
                                vp_video_osd_config_rect(idx, vp_video_chn_main, j, &rect);
                                rect.line = 1;
                                vp_video_osd_config_rect(idx, vp_video_chn_sec, j, &rect);
                                vp_video_osd_config_rect(idx, vp_video_chn_thr, j, &rect);
                            }
                            vp_osd_config_rect_t config = {0};
                            for (int j = 0; j < VP_VIDEO_OSD_MOSAIC_MAX; ++j) {
                                vp_video_osd_config_mosaic(idx, vp_video_chn_main, j, &config);
                                vp_video_osd_config_mosaic(idx, vp_video_chn_sec, j, &config);
                            }
                        }
                    }
                    break;
                }
#endif
            // case VP_ALGORITHM_TYPE_CONVERGENCE_DETECT: {
            //     if (info->handle == NULL) {
            //         info->result.type = i;
            //         info->handle = vp_convergence_detect_create(frame->width, frame->height, idx,
            //                                                     &param->convergence);
            //         vp_debug("create convergence handle:%p", info->handle);
            //     } else {
            //         if (info->update) {
            //             vp_convergence_detect_set_param(info->handle, &param->convergence);
            //             vp_debug("update convergence param:%p", info->handle);
            //             info->update = 0;
            //         }
            //     }
            //     break;
            // }
            case VP_ALGORITHM_TYPE_MAX:
            default:
                break;
        }

        if (info->paused) vp_algorithm_detect_resume(i, info);
        if (info->handle && info->param.jpeg) has_jpeg = 1;
    }
    if (channel->frame_width != frame->width || channel->frame_height != frame->height) {
        channel->frame_width = frame->width;
        channel->frame_height = frame->height;
    }

    if (channel->jpeg_data == NULL && has_jpeg) {
        channel->data_size = 128 * 1024;
        channel->jpeg_data = (uint8_t *) malloc(channel->data_size);
        channel->jpeg_size = 0;
    }

    return 0;
}


static TS_S32 vp_algorithm_yuv2rgb(TS_U8 *y_image, TS_U8 *uv_image, TS_U8 *rgb_image,TS_U32 src_width, TS_U32 src_height, TS_U32 des_width, TS_U32 des_height, ALG_RGB_TYPE_E rgb_type)
{
	TS_ALG_YUV2RGB(y_image, uv_image, rgb_image, src_width, src_height, des_width, des_height, rgb_type);

    return TS_SUCCESS;
}

static int vp_algorithm_detect_process(uint8_t idx, uint8_t chn, vp_algorithm_ivs_args_t *args,
                                       vp_video_source_t *frame) 
{
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    time_t start_sec = start_time.tv_sec;
    struct tm *start_tm = localtime(&start_sec);
    printf("%04d-%02d-%02d %02d:%02d:%02d-%03ld [DEBUG] vp_algorithm.c:%03d Start: %04d-%02d-%02d %02d:%02d:%02d-%03ld\n", 
           start_tm->tm_year + 1900, start_tm->tm_mon + 1, start_tm->tm_mday, 
           start_tm->tm_hour, start_tm->tm_min, start_tm->tm_sec, start_time.tv_usec / 1000, 
           __LINE__, 
           start_tm->tm_year + 1900, start_tm->tm_mon + 1, start_tm->tm_mday, 
           start_tm->tm_hour, start_tm->tm_min, start_tm->tm_sec, start_time.tv_usec / 1000);

    //printf("###########vp_algorithm_detect_process###############idx:%d ,chn:%d\n",idx,chn);
    (void) args;
    // vp_track_opt_t *track = &g_context.track;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    vp_algorithm_info_t *info;
    int ret, need_jpeg = 0;
    static int box_flag[VP_SENSOR_NUM] = {0};
    static int box_count[VP_SENSOR_NUM] = {0};
    vp_human_detect_result_t *human_result = NULL;
    uint8_t* frame_data;

    if (frame == NULL || channel == NULL) return -1;
    ALG_IMAGE_S* pimage = (ALG_IMAGE_S*)args->user_data;
    if (frame->width != 640 || frame->height != 360) {
        vp_error("human detect and motion detect only support 640 * 360.\n");
        return -1;
    } 
    else {
        
        if (pimage == NULL || pimage->pData == NULL || frame->frame_data == NULL) {
            vp_error("Invalid params: pimage-%p, frame_data:%p.\n", pimage, frame->frame_data);
            return -1;
        }

        vp_algorithm_yuv2rgb(frame->frame_data, frame->frame_data + frame->width * frame->height, 
            ((TS_U8*)(uintptr_t)pimage->pData) + 4 * 12 * pimage->s32W, frame->width, frame->height, 
            frame->width, frame->height, ALG_RGB_TYPE_RGBA32);

        TS_MPI_SYS_MmzFlushCache(pimage->pDataPhy, pimage->pData, pimage->s32W * pimage->s32H * pimage->s32C);

#if LOAD_RGBA_FROM_SD
        {
            static int load_once = 0;
            static uint8_t *loaded_rgba_data = NULL;
            if (!load_once) {
                FILE *fp = fopen(LOAD_RGBA_FILE_PATH, "rb");
                if (fp) {
                    uint32_t rgba_size = 640 * 384 * 4;
                    loaded_rgba_data = (uint8_t *)malloc(rgba_size);
                    if (loaded_rgba_data) {
                        size_t read_size = fread(loaded_rgba_data, 1, rgba_size, fp);
                        if (read_size == rgba_size) {
                            vp_debug("Load RGBA from SD: %s (%dx%d)\n", LOAD_RGBA_FILE_PATH, 640, 384);
                        } else {
                            vp_error("Read RGBA file incomplete: %s, read=%zu, expected=%u\n", 
                                     LOAD_RGBA_FILE_PATH, read_size, rgba_size);
                            free(loaded_rgba_data);
                            loaded_rgba_data = NULL;
                        }
                    }
                    fclose(fp);
                } else {
                    vp_error("Open RGBA file failed: %s\n", LOAD_RGBA_FILE_PATH);
                }
                load_once = 1;
            }
            if (loaded_rgba_data) {
                memcpy(pimage->pData, loaded_rgba_data, 640 * 384 * 4);
                TS_MPI_SYS_MmzFlushCache(pimage->pDataPhy, pimage->pData, pimage->s32W * pimage->s32H * pimage->s32C);
            }
        }
#endif

        static uint32_t save_frame_count = 0;
        static uint32_t saved_image_count = 0;
        static uint32_t current_save_interval = SAVE_RGBA_INTERVAL_BASE;
        
#if SAVE_RGBA_ENABLE
        save_frame_count++;
        if (save_frame_count % current_save_interval == 0) {
            struct stat st = {0};
            if (stat(SAVE_RGBA_PATH, &st) == -1) {
                mkdir(SAVE_RGBA_PATH, 0777);
                vp_debug("Create directory: %s\n", SAVE_RGBA_PATH);
            }
            
            // 1. 保存RGBA格式
            char rgba_filename[256];
            snprintf(rgba_filename, sizeof(rgba_filename), "%scam%d_rgba_%u.rgba", SAVE_RGBA_PATH, idx, save_frame_count);
            
            FILE *rgba_fp = fopen(rgba_filename, "wb");
            if (rgba_fp) {
#if SAVE_RGBA_CROP_PADDING
                uint32_t save_width = pimage->s32W;
                uint32_t save_height = 360;
                uint32_t padding_top = 12;
                uint8_t* src_ptr = (uint8_t*)pimage->pData + padding_top * save_width * 4;
#else
                uint32_t save_width = pimage->s32W;
                uint32_t save_height = pimage->s32H;
                uint8_t* src_ptr = (uint8_t*)pimage->pData;
#endif
                uint32_t data_size = save_width * save_height * 4;
                size_t written = fwrite(src_ptr, 1, data_size, rgba_fp);
                fclose(rgba_fp);
                if (written == data_size) {
                    saved_image_count++;
                    vp_debug("Save clean RGBA image: %s (%dx%d), saved_count=%u, interval=%u\n", 
                             rgba_filename, save_width, save_height, saved_image_count, current_save_interval);
                } else {
                    vp_error("Save RGBA image failed: %s\n", rgba_filename);
                }
            } else {
                vp_error("Open RGBA file failed: %s\n", rgba_filename);
            }
            
            // 2. 保存RGB格式
            char rgb_filename[256];
            snprintf(rgb_filename, sizeof(rgb_filename), "%scam%d_rgb_%u.rgb", SAVE_RGBA_PATH, idx, save_frame_count);
            
            FILE *rgb_fp = fopen(rgb_filename, "wb");
            if (rgb_fp) {
#if SAVE_RGBA_CROP_PADDING
                uint32_t save_width = pimage->s32W;
                uint32_t save_height = 360;
                uint32_t padding_top = 12;
                uint8_t* src_ptr = (uint8_t*)pimage->pData + padding_top * save_width * 4;
#else
                uint32_t save_width = pimage->s32W;
                uint32_t save_height = pimage->s32H;
                uint8_t* src_ptr = (uint8_t*)pimage->pData;
#endif
                uint32_t src_stride = save_width * 4;
                uint32_t dst_stride = save_width * 3;
                uint32_t data_size = save_width * save_height * 3;
                uint8_t* rgb_buffer = (uint8_t*)malloc(data_size);
                if (rgb_buffer) {
                    for (uint32_t y = 0; y < save_height; y++) {
                        uint8_t* src_line = src_ptr + y * src_stride;
                        uint8_t* dst_line = rgb_buffer + y * dst_stride;
                        for (uint32_t x = 0; x < save_width; x++) {
                            dst_line[x * 3 + 0] = src_line[x * 4 + 0];
                            dst_line[x * 3 + 1] = src_line[x * 4 + 1];
                            dst_line[x * 3 + 2] = src_line[x * 4 + 2];
                        }
                    }
                    size_t written = fwrite(rgb_buffer, 1, data_size, rgb_fp);
                    free(rgb_buffer);
                    fclose(rgb_fp);
                    if (written == data_size) {
                        saved_image_count++;
                        vp_debug("Save clean RGB image: %s (%dx%d), saved_count=%u, interval=%u\n", 
                                 rgb_filename, save_width, save_height, saved_image_count, current_save_interval);
                    } else {
                        vp_error("Save RGB image failed: %s\n", rgb_filename);
                    }
                } else {
                    fclose(rgb_fp);
                    vp_error("Malloc RGB buffer failed\n");
                }
            } else {
                vp_error("Open RGB file failed: %s\n", rgb_filename);
            }
            
            // 调整保存间隔
            if (saved_image_count % SAVE_RGBA_BATCH_SIZE == 0) {
                current_save_interval += SAVE_RGBA_INTERVAL_STEP;
                vp_debug("Increase save interval to %u after %u images saved\n", 
                         current_save_interval, saved_image_count);
            }
        }
#endif
        //frame_data = frame->frame_data;
        //frame->frame_data = pimage;
    }

    for (int i = 0; i < VP_ALGORITHM_TYPE_MAX; ++i) {
        info = &channel->infos[i];

        if ((info->enable == 0 || info->handle == NULL || info->pause)) {
            if (i == VP_ALGORITHM_TYPE_HUMAN_DETECT) {
                if (box_flag[idx] == 1) {
                    box_flag[idx] = 0;
                    box_count[idx] = 0;
                    vp_algorithm_human_clear_boxes(idx, 0);
                    vp_algorithm_human_clear_boxes(idx, 1);
                }
            }
            continue;
        }

        info->result.state = 0;
        info->result.width = frame->width;
        info->result.height = frame->height;
        switch (i) {
#if VP_ALGORITHM_FUSION && VP_ALGORITHM_HUMAN
            case VP_ALGORITHM_TYPE_MOTION_DETECT: {
                if (frame->timestamp - channel->human_timestamp < 5000000) {
                    // 人形触发,忽略移动检测
                    continue;
                }
                if (track->motor_state && track->motor_state() == 1) continue;
#if (VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23DL || VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23ZN_3)
                status_param_t *status_param = vp_status_param_get_handle();
                if (status_param->factory_mode.mode != 0 || status_param->speak.status != 0 ||
                    status_param->record_play_status.status > 0 || status_param->record_play_status.download_status > 0) {
                    continue;
                }
#endif
                ret = vp_fusion_detect_process(info->handle, frame);
                if (ret > 0) {
                    vp_lock(&channel->lock);
                    vp_fusion_detect_motion_result(info->handle, &info->result.motion);
                    if (info->result.motion.state) {
                        info->result.state = 1;
                        info->result.timestamp = frame->timestamp;
                    } else {
                        ret = 0;
                    }
                    vp_unlock(&channel->lock);
                }
                break;
            }
            case VP_ALGORITHM_TYPE_HUMAN_DETECT: {
                if (track->motor_state && track->motor_state() == 1) {
                    if (box_flag[idx] == 1) {
                        vp_algorithm_human_clear_boxes(idx, 0);
                        vp_algorithm_human_clear_boxes(idx, 1);
                        box_flag[idx] = 0;
                        box_count[idx] = 0;
                    }
                    continue;
                }
#if (VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23DL || VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23ZN_3)
                status_param_t *status_param = vp_status_param_get_handle();
                if (status_param->factory_mode.mode != 0 || status_param->speak.status != 0 ||
                    status_param->record_play_status.status > 0) {
                    if (box_flag[idx] == 1) {
                        vp_algorithm_human_clear_boxes(idx, 0);
                        vp_algorithm_human_clear_boxes(idx, 1);
                        box_flag[idx] = 0;
                        box_count[idx] = 0;
                    }
                    continue;
                }
#endif
                ret = vp_fusion_detect_process(info->handle, frame);
                if (ret > 0) {
                    vp_lock(&channel->lock);
                    vp_fusion_detect_human_result(info->handle, &info->result.human);
                    if (info->result.human.count > 0) {
                        info->result.state = 1;
                        info->result.timestamp = frame->timestamp;
                        channel->human_timestamp = frame->timestamp;
                        human_result = &info->result.human;
                    } else {
                        ret = 0;
                    }
                    vp_unlock(&channel->lock);

                    if (info->result.state == 1) {
                        if (info->param.human.draw_box) {
                            if (box_flag[idx] == 1) {
                                vp_algorithm_human_clear_boxes(idx, 0);
                                vp_algorithm_human_clear_boxes(idx, 1);
                            }
                            box_flag[idx] = 1;
                            box_count[idx] = 0;
                            vp_algorithm_human_draw_boxes(idx, 0, frame->width, frame->height, &info->result.human);
                            vp_algorithm_human_draw_boxes(idx, 1, frame->width, frame->height, &info->result.human);
                        }
                        if (info->param.human.enable_track && track->track_target) {
                            for (int j = 0; j < info->result.human.count; ++j) {
                                g_context.track_objs[j].idx = idx;
                                g_context.track_objs[j].track_id = info->result.human.objs[j].track_id;
                                g_context.track_objs[j].score = info->result.human.objs[j].score;
                                g_context.track_objs[j].x = info->result.human.objs[j].rect.x;
                                g_context.track_objs[j].y = info->result.human.objs[j].rect.y;
                                g_context.track_objs[j].w = info->result.human.objs[j].rect.w;
                                g_context.track_objs[j].h = info->result.human.objs[j].rect.h;
                                g_context.track_objs[j].width = frame->width;
                                g_context.track_objs[j].height = frame->height;
                            }
                            track->track_target(g_context.track_objs, info->result.human.count);
                        }
                    } else {
                        if (box_flag[idx] == 1 && box_count[idx]++ >= 3) {
                            vp_algorithm_human_clear_boxes(idx, 0);
                            vp_algorithm_human_clear_boxes(idx, 1);
                            box_flag[idx] = 0;
                        }

                        if (info->param.human.enable_track && track->track_target) {
                            track->track_target(g_context.track_objs, 0);
                        }
                    }
                } else {
                    if (box_flag[idx] == 1 && box_count[idx]++ >= 3) {
                        vp_algorithm_human_clear_boxes(idx, 0);
                        vp_algorithm_human_clear_boxes(idx, 1);
                        box_flag[idx] = 0;
                    }

                    if (info->param.human.enable_track && track->track_target) {
                        track->track_target(g_context.track_objs, 0);
                    }
                }
                break;
            }
#elif VP_ALGORITHM_MULTIOBJECT && VP_ALGORITHM_HUMAN
            case VP_ALGORITHM_TYPE_MOTION_DETECT: {
                if (frame->timestamp - channel->human_timestamp < 5000000) {
                    // 人形触发,忽略移动检测
                    continue;
                }
                if (track->motor_state && track->motor_state() == 1) continue;
                status_param_t *status_param = vp_status_param_get_handle();
                if (status_param->factory_mode.mode != 0){
                    continue;
                }
                ret = vp_multiobject_detect_process(info->handle, frame);
                if (ret > 0) {
                    vp_lock(&channel->lock);
                    vp_multiobject_detect_motion_result(info->handle, &info->result.motion);
                    if (info->result.motion.state) {
                        info->result.state = 1;
                        info->result.timestamp = frame->timestamp;
                    } else {
                        ret = 0;
                    }
                    vp_unlock(&channel->lock);
                }
                break;
            }
            case VP_ALGORITHM_TYPE_HUMAN_DETECT: {
                if (track->motor_state && track->motor_state() == 1) {
                    if (box_flag[idx] == 1) {
                        vp_algorithm_human_clear_boxes(idx, 0);
                        vp_algorithm_human_clear_boxes(idx, 1);
                        box_flag[idx] = 0;
                        box_count[idx] = 0;
                    }
                    continue;
                }
                status_param_t *status_param = vp_status_param_get_handle();
                if (status_param->factory_mode.mode != 0) {
                    if (box_flag[idx] == 1) {
                        vp_algorithm_human_clear_boxes(idx, 0);
                        vp_algorithm_human_clear_boxes(idx, 1);
                        box_flag[idx] = 0;
                        box_count[idx] = 0;
                    }
                    continue;
                }
                ret = vp_multiobject_detect_process(info->handle, frame);
                
                if (ret > 0) {
                    vp_lock(&channel->lock);
                    vp_multiobject_detect_human_result(info->handle, &info->result.human);
                    if (info->result.human.count > 0) {
                        info->result.state = 1;
                        info->result.timestamp = frame->timestamp;
                        channel->human_timestamp = frame->timestamp;
                        human_result = &info->result.human;
                    } else {
                        ret = 0;
                    }
                    vp_unlock(&channel->lock);

                    if (info->result.state == 1) {
                        if (info->param.human.draw_box) {
                            if (box_flag[idx] == 1) {
                                vp_algorithm_human_clear_boxes(idx, 0);
                                vp_algorithm_human_clear_boxes(idx, 1);
                            }
                            box_flag[idx] = 1;
                            box_count[idx] = 0;

                            vp_algorithm_human_draw_boxes(idx, 0, frame->width, frame->height, &info->result.human);
                            vp_algorithm_human_draw_boxes(idx, 1, frame->width, frame->height, &info->result.human);
                        }
                        if (info->param.human.enable_track && track->track_target) {
                            for (int j = 0; j < info->result.human.count; ++j) {
                                g_context.track_objs[j].idx = idx;
                                g_context.track_objs[j].track_id = info->result.human.objs[j].track_id;
                                g_context.track_objs[j].score = info->result.human.objs[j].score;
                                g_context.track_objs[j].x = info->result.human.objs[j].rect.x;
                                g_context.track_objs[j].y = info->result.human.objs[j].rect.y;
                                g_context.track_objs[j].w = info->result.human.objs[j].rect.w;
                                g_context.track_objs[j].h = info->result.human.objs[j].rect.h;
                                g_context.track_objs[j].width = frame->width;
                                g_context.track_objs[j].height = frame->height;
                            }
                            track->track_target(g_context.track_objs, info->result.human.count);
                        }
                    } else {
                        if (box_flag[idx] == 1 && box_count[idx]++ >= 3) {
                            vp_algorithm_human_clear_boxes(idx, 0);
                            vp_algorithm_human_clear_boxes(idx, 1);
                            box_flag[idx] = 0;
                        }

                        if (info->param.human.enable_track && track->track_target) {
                            track->track_target(g_context.track_objs, 0);
                        }
                    }
                } else {
                    if (box_flag[idx] == 1 && box_count[idx]++ >= 3) {
                        vp_algorithm_human_clear_boxes(idx, 0);
                        vp_algorithm_human_clear_boxes(idx, 1);
                        box_flag[idx] = 0;
                    }

                    if (info->param.human.enable_track && track->track_target) {
                        track->track_target(g_context.track_objs, 0);
                    }
                }
                break;
            }
#else
                case VP_ALGORITHM_TYPE_MOTION_DETECT: {
                    if (frame->timestamp - channel->human_timestamp < 5000000) {
                        // 人形触发,忽略移动检测
                        continue;
                    }
                    // if (track->motor_state && track->motor_state() == 1) continue;
#if (VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23DL || VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23ZN_3)
                    status_param_t *status_param = vp_status_param_get_handle();
                    if (status_param->factory_mode.mode != 0 || status_param->speak.status != 0 ||
                        status_param->record_play_status.status > 0) {
                        continue;
                    }
#endif
                    ret = vp_motion_detect_process(info->handle, frame);
                    if (ret > 0) {
                        vp_lock(&channel->lock);
                        info->result.state = 1;
                        vp_motion_detect_result(info->handle, &info->result.motion);
                        info->result.timestamp = frame->timestamp;
                        vp_unlock(&channel->lock);
                    }
                    break;
                }
                case VP_ALGORITHM_TYPE_HUMAN_DETECT: {
                    // if (track->motor_state && track->motor_state() == 1) {
                    //     if (box_flag[idx] == 1) {
                    //         vp_algorithm_human_clear_boxes(idx, 0);
                    //         vp_algorithm_human_clear_boxes(idx, 1);
                    //         box_flag[idx] = 0;
                    //         box_count[idx] = 0;
                    //     }
                    //     continue;
                    // }
#if (VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23DL || VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23ZN_3)
                    status_param_t *status_param = vp_status_param_get_handle();
                    if (status_param->factory_mode.mode != 0 || status_param->speak.status != 0 ||
                        status_param->record_play_status.status > 0) {
                        if (box_flag[idx] == 1) {
                            vp_algorithm_human_clear_boxes(idx, 0);
                            vp_algorithm_human_clear_boxes(idx, 1);
                            box_flag[idx] = 0;
                            box_count[idx] = 0;
                        }
                        continue;
                    }
#endif
                    ret = vp_cat_detect_process(info->handle,pimage, frame, idx);
                    if (ret > 0) {
                        channel->human_timestamp = frame->timestamp;
                        vp_lock(&channel->lock);
                        info->result.state = 1;
                        vp_cat_detect_result(info->handle, (vp_cat_detect_result_t*)&info->result.cat);
                        info->result.timestamp = frame->timestamp;
                        vp_unlock(&channel->lock);
                        
                        vp_algorithm_cat_draw_boxes(idx, 0, frame->width, frame->height, (vp_cat_detect_result_t*)&info->result.cat);
                        vp_algorithm_cat_draw_boxes(idx, 1, frame->width, frame->height, (vp_cat_detect_result_t*)&info->result.cat);
                        
                        vp_algorithm_cat_draw_mosaic(idx, 0, frame->width, frame->height, (vp_cat_detect_result_t*)&info->result.cat);
                        vp_algorithm_cat_draw_mosaic(idx, 1, frame->width, frame->height, (vp_cat_detect_result_t*)&info->result.cat);
                    } else {
                        vp_algorithm_cat_clear_boxes(idx, 0);
                        vp_algorithm_cat_clear_boxes(idx, 1);
                        vp_algorithm_cat_clear_mosaic(idx, 0);
                        vp_algorithm_cat_clear_mosaic(idx, 1);
                    }
                    break;
                }
#endif
                case VP_ALGORITHM_TYPE_CONVERGENCE_DETECT: {
                    // ret = vp_convergence_detect_process(info->handle, channel->human_timestamp, human_result, frame);
                    // if (ret > 0) {
                    //     vp_lock(&channel->lock);
                    //     info->result.state = 1;
                    //     vp_convergence_detect_result(info->handle, &info->result.convergence);
                    //     info->result.timestamp = frame->timestamp;
                    //     vp_unlock(&channel->lock);
                    // }
                }
                case VP_ALGORITHM_TYPE_MAX:
                default:
                    break;
        }
        info->detect_timestamp = frame->timestamp;
        vp_events_send(channel->events, VP_EVENT_BIT(i));

        // if (ret > 0 && info->param.notify && info->notify == 0) {
        if (info->param.notify && info->notify == 0) {
            if (info->last_notify == 0 || (frame->timestamp - info->last_notify) > (1 * 1000000)) {
                if (info->param.jpeg) need_jpeg = 1;
                info->has_notify = 1;
                info->last_notify = frame->timestamp;
            }
        }
    }
    if (need_jpeg && channel->jpeg_data) {
        for (int j = 0; j < VP_VIDEO_OSD_MAX; ++j) {
            // vp_video_osd_overlay(idx, chn, j, frame_data,
            //                      frame_data + frame->width * frame->height,
            //                      frame->width, frame->height);
        }

        vp_lock(&channel->lock);
        if (channel->jpeg_data) {
#if VP_SDK_PLATFORM == VP_SDK_PLATFORM_T41
            uint32_t height = (frame->height + 15) & (~15);
            uint32_t width = (frame->width + 15) & (~15);

            memmove(frame->frame_data + height * width, frame->frame_data + frame->width * frame->height,
                    frame->width * frame->height / 2);

#endif
            channel->jpeg_size = channel->data_size;
#if (VP_SDK_PLATFORM == VP_SDK_PLATFORM_T31 || VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23DL || \
     VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23ZN_V9 || VP_SOC_MODEL == VP_SOC_MODEL_JZ_T23ZN_3)
            ret = vp_video_encoder_jpeg(idx, 1, NULL, channel->jpeg_data, &channel->jpeg_size);
#else
            ret = vp_video_encoder_jpeg(idx, 1, frame, channel->jpeg_data, &channel->jpeg_size);
#endif
            if (ret) {
                channel->jpeg_size = 0;
            }
        }
        vp_unlock(&channel->lock);
    }

    uint8_t need_notify = 0;
    for (int j = 0; j < VP_ALGORITHM_TYPE_MAX; ++j) {
        info = &channel->infos[j];
        if (info->has_notify) {
            need_notify = 1;
            info->has_notify = 0;
            info->notify = 1;
        }
    }
    if (need_notify) vp_events_send(g_context.events, VP_EVENT_BIT0);
    
    gettimeofday(&end_time, NULL);
    time_t end_sec = end_time.tv_sec;
    struct tm *end_tm = localtime(&end_sec);
    long execution_time_ms = (end_time.tv_sec - start_time.tv_sec) * 1000 + 
                          (end_time.tv_usec - start_time.tv_usec) / 1000;
    printf("%04d-%02d-%02d %02d:%02d:%02d-%03ld [DEBUG] vp_algorithm.c:%03d End: %04d-%02d-%02d %02d:%02d:%02d-%03ld, Execution time: %ld ms\n", 
           end_tm->tm_year + 1900, end_tm->tm_mon + 1, end_tm->tm_mday, 
           end_tm->tm_hour, end_tm->tm_min, end_tm->tm_sec, end_time.tv_usec / 1000, 
           __LINE__, 
           end_tm->tm_year + 1900, end_tm->tm_mon + 1, end_tm->tm_mday, 
           end_tm->tm_hour, end_tm->tm_min, end_tm->tm_sec, end_time.tv_usec / 1000, 
           execution_time_ms);
    
    return 0;
}

static void vp_algorithm_detect_destroy(vp_algorithm_type_t type, vp_algorithm_info_t *info) {
    switch (type) {
#if VP_ALGORITHM_FUSION && VP_ALGORITHM_HUMAN
        case VP_ALGORITHM_TYPE_MOTION_DETECT:
        case VP_ALGORITHM_TYPE_HUMAN_DETECT: {
            vp_fusion_detect_destroy(info->handle);
            break;
        }
#elif VP_ALGORITHM_MULTIOBJECT && VP_ALGORITHM_HUMAN
        case VP_ALGORITHM_TYPE_MOTION_DETECT:
        case VP_ALGORITHM_TYPE_HUMAN_DETECT: {
            vp_multiobject_detect_destroy(info->handle);
            break;
        }
#else
        case VP_ALGORITHM_TYPE_MOTION_DETECT: {
            vp_motion_detect_destroy(info->handle);
            break;
        }
        case VP_ALGORITHM_TYPE_HUMAN_DETECT:
                vp_cat_detect_destroy(info->handle);
                break;
#endif
        case VP_ALGORITHM_TYPE_CONVERGENCE_DETECT: {
            // vp_convergence_detect_destroy(info->handle);
            break;
        }
        case VP_ALGORITHM_TYPE_MAX:
        default:
            break;
    }
    vp_stack("destroy algorithm:%d handle:%p", type, info->handle);
    info->handle = NULL;
}


static int vp_algorithm_detect_after(uint8_t idx, uint8_t chn, vp_algorithm_ivs_args_t *args,
                                     vp_video_source_t *frame) {
    (void) args;
    (void) frame;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    vp_events_send(channel->events, VP_EVENT_BIT(VP_ALGORITHM_TYPE_MAX));
    vp_algorithm_info_t *info;
    uint8_t has_jpeg = 0;
    for (int i = 0; i < VP_ALGORITHM_TYPE_MAX; ++i) {
        info = &channel->infos[i];
        if (info->enable) {
            if (info->param.jpeg) has_jpeg = 1;
            if (info->pause && info->paused == 0) {
                vp_algorithm_detect_pause(i, info);
            }
            continue;
        }
        if (info->handle == NULL) continue;
        vp_algorithm_detect_destroy(i, info);
    }
    if (has_jpeg == 0 && channel->jpeg_data) {
        vp_debug("free jpeg buffer:%p", channel->jpeg_data);
        vp_lock(&channel->lock);
        if (channel->jpeg_data) {
            free(channel->jpeg_data);
            channel->jpeg_data = NULL;
        }
        vp_unlock(&channel->lock);
    }
    return 0;
}

static int vp_algorithm_detect_deinit(vp_algorithm_ivs_args_t *args) {
    (void) args;
    vp_algorithm_info_t *info;
    for (int i = 0; i < VP_SENSOR_NUM; ++i) {
        for (int j = 0; j < vp_video_chn_max; ++j) {
            vp_algorithm_channel_t *channel = &g_context.channels[i][j];
            for (int k = 0; k < VP_ALGORITHM_TYPE_MAX; ++k) {
                info = &channel->infos[k];
                if (info->handle == NULL) continue;
                vp_algorithm_detect_destroy(k, info);
            }
            vp_lock(&channel->lock);
            if (channel->jpeg_data) {
                free(channel->jpeg_data);
                channel->jpeg_data = NULL;
            }
            vp_unlock(&channel->lock);
        }
    }
#if VP_ALGORITHM_TYPE == VP_ALGORITHM_TYPE_YTLD
    vp_algorithm_ytld_unload();
#endif

    ALG_IMAGE_S* pimage = args->user_data;
    if (pimage != NULL) {
        if (pimage->pData != NULL) {
            TS_MPI_SYS_MmzFree(pimage->pDataPhy, pimage->pData);
        }

        free(pimage);
    }
    args->user_data = NULL;

    // if (g_singletrack_handle) {
    //     TS_ALG_SingleTgtTrack_Exit(g_singletrack_handle);
    //     g_singletrack_handle = NULL;
    // }

    return 0;
}

void *vp_algorithm_notify_thread(void *args) {
    (void) args;
    vp_pthread_name("algorithm_notify");
    vp_debug("start");
    uint64_t bits;
    int ret;
    while (g_context.state != VP_ALGORITHM_STATE_IDLE) {
        bits = VP_EVENTS_ALL;
        ret = vp_events_wait(g_context.events, &bits, VP_EVENTS_FLAG_OR_CLEAR, 1000);
        if (ret) {
            if (ret == -2) continue;
            break;
        }
        vp_algorithm_info_t *info;
        for (int i = 0; i < VP_SENSOR_NUM; ++i) {
            for (int j = 0; j < vp_video_chn_max; ++j) {
                vp_algorithm_channel_t *channel = &g_context.channels[i][j];
                for (int k = 0; k < VP_ALGORITHM_TYPE_MAX; ++k) {
                    info = &channel->infos[k];
                    if (info->enable == 0 || info->notify == 0) continue;
                    
                    vp_algorithm_notify_t *notify = malloc(sizeof(vp_algorithm_notify_t));
                    if (!notify) {
                        vp_error("malloc notify failed");
                        info->notify = 0;
                        continue;
                    }
                    memset(notify, 0, sizeof(vp_algorithm_notify_t));
                    
                    vp_lock(&channel->lock);
                    memcpy(&notify->result, &info->result, sizeof(vp_algorithm_result_t));
                    vp_unlock(&channel->lock);
                    vp_stack("rect notify: idx:%d chn:%d type:%d last timestamp:%lld jpeg:%d width:%d height:%d", i, j,
                             k, info->last_notify, info->param.jpeg, channel->frame_width, channel->frame_height);

                    if (info->param.jpeg) {
                        vp_lock(&channel->lock);
                        if (channel->jpeg_data && channel->jpeg_size < 128 * 1024) {
                            notify->jpeg_data = malloc(channel->jpeg_size);
                            if (notify->jpeg_data) {
                                memcpy(notify->jpeg_data, channel->jpeg_data, channel->jpeg_size);
                                notify->jpeg_size = channel->jpeg_size;
                            } else {
                                vp_error("malloc jpeg_data failed");
                                notify->jpeg_data = 0;
                                notify->jpeg_size = 0;
                            }
                        } else {
                            if (channel->jpeg_size >= 128 * 1024) {
                                vp_error("jpeg buffer too small: %d", channel->jpeg_size);
                            }
                            notify->jpeg_data = 0;
                            notify->jpeg_size = 0;
                        }
                        vp_unlock(&channel->lock);
                    } else {
                        notify->jpeg_data = 0;
                        notify->jpeg_size = 0;
                    }

                    notify->idx = i;
                    notify->chn = j;
                    notify->type = k;
                    notify->width = channel->frame_width;
                    notify->height = channel->frame_height;
                    switch (k) {
                        case VP_ALGORITHM_TYPE_MOTION_DETECT: {
                            vp_observers_post(VP_OBS_TYPE_ALARM, VP_OBS_EVENT_ALARM_MOTION, notify);
                            break;
                        }
                        case VP_ALGORITHM_TYPE_HUMAN_DETECT: {
                            vp_observers_post(VP_OBS_TYPE_ALARM, VP_OBS_EVENT_ALARM_PERSON, notify);
                            break;
                        }
                        case VP_ALGORITHM_TYPE_MAX:
                        default:
                            break;
                    }

                    info->notify = 0;
                }
            }
        }
    }
    vp_debug("exit");
    return NULL;
}

/**
 * 初始化AI算法模块
 *
 * @return 错误码
 * @retval 0  成功
 * @retval <0 失败
 */
int vp_algorithm_init() {
    if (g_context.state != VP_ALGORITHM_STATE_IDLE) return -1;
    g_context.state = VP_ALGORITHM_STATE_STARTING;

    memset(&g_context.channels, 0, sizeof(g_context.channels));
    g_context.events = vp_events_create();
    for (int i = 0; i < VP_SENSOR_NUM; ++i) {
        for (int j = 0; j < vp_video_chn_max; ++j) {
            g_context.channels[i][j].events = vp_events_create();
            vp_lock_init(&g_context.channels[i][j].lock);
        }
    }
    // memset(&g_context.track, 0, sizeof(g_context.track));
    // vp_track_opt(&g_context.track);
    vp_algorithm_ivs_args_t args = {
            .init = vp_algorithm_detect_init,
            .before = vp_algorithm_detect_before,
            .process = vp_algorithm_detect_process,
            .after = vp_algorithm_detect_after,
            .deinit = vp_algorithm_detect_deinit,
            .user_data = NULL,
    };
    vp_algorithm_ivs_init(&args);
    for (int i = 0; i < VP_SENSOR_NUM; ++i) {
        int ret = vp_algorithm_ivs_bind(i, vp_video_chn_sec);
        vp_debug("vp_algorithm_ivs_bind:%d", ret);
    }
    vp_pthread_create(&g_notify_thread, 64 * 1024, vp_algorithm_notify_thread, NULL);
    return 0;
}

/**
 * 获取AI算法模块鉴权文件路径
 * @return 算法鉴权文件路径
 */
char *vp_algorithm_auth_file() {
#if VP_PLATFORM_IS_DEVICE(VP_SDK_PLATFORM)
    return "/tmp/ai_auth.lic";
#else
    return "./tmp/ai_auth.lic";
#endif
}

/**
 * 获取AI算法模块模型文件路径
 * @return 算法模型文件路径
 */
char *vp_algorithm_model_file() {
#if VP_PLATFORM_IS_DEVICE(VP_SDK_PLATFORM)
    return "/model";
#else
    return "./model";
#endif
}

/**
 * 获取AI算法模块配置文件路径
 * @return 算法配置文件路径
 */
char *vp_algorithm_config_file() {
#if VP_PLATFORM_IS_DEVICE(VP_SDK_PLATFORM)
    return "/model/config/config.bin";
#else
    return "./model/config/config.bin";
#endif
}

/**
 * 启动AI算法模块
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @param param [in]    算法参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_start(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, vp_algorithm_param_t *param) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || type >= VP_ALGORITHM_TYPE_MAX) return -1;
    if (g_context.state == VP_ALGORITHM_STATE_IDLE) return -1;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    vp_algorithm_info_t *info = &channel->infos[type];
    if (param) memcpy(&info->param, param, sizeof(vp_algorithm_param_t));
    else param = &info->param;
    vp_stack("start idx:%d chn:%d detect type:%d notify:%d interval:%d jpeg:%d enable:%d notify:%d", idx, chn, type,
             param->notify, param->interval, param->jpeg, info->enable, info->notify);
    if (info->enable) {
        info->update = 1;
        return 0;
    }

    info->pause = 0;
    info->enable = 1;
    info->notify = 0;
    return 0;
}

/**
 * 获取AI算法模块状态
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @param status [out] 算法状态
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_state(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, vp_algorithm_status_t *status) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || type >= VP_ALGORITHM_TYPE_MAX || status == NULL) return -1;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    vp_algorithm_info_t *info = &channel->infos[type];
    status->enable = info->enable;
    status->paused = info->pause;
    return 0;
}

/**
 * 获取AI算法模块是否启用
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @return 0 禁用,1 启用
 */
int vp_algorithm_enable_state(uint8_t idx, uint8_t chn, vp_algorithm_type_t type) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || type >= VP_ALGORITHM_TYPE_MAX) return 0;
    return g_context.channels[idx][chn].infos[type].enable;
}

/**
 * 获取AI算法模块是否暂停
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @return 0 暂停,1 运行
 */
int vp_algorithm_pause_state(uint8_t idx, uint8_t chn, vp_algorithm_type_t type) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || type >= VP_ALGORITHM_TYPE_MAX) return 0;
    return g_context.channels[idx][chn].infos[type].pause;
}

/**
 * 启用禁用AI算法模块通知
 *
 * @param idx       [in]    视频传感器索引
 * @param chn       [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type      [in]    算法类型
 * @param enable    [in]    是否启用通知
 * @param jpeg      [in]    是否推送图片
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_enable_notify(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, uint8_t enable, uint8_t jpeg) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || type >= VP_ALGORITHM_TYPE_MAX) return -1;
    if (g_context.state == VP_ALGORITHM_STATE_IDLE) return -1;
    g_context.channels[idx][chn].infos[type].param.notify = enable;
    g_context.channels[idx][chn].infos[type].param.jpeg = jpeg;
    return 0;
}

/**
 * 更新AI算法模块参数
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @param param [out]   算法参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_get_param(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, vp_algorithm_param_t *param) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || type >= VP_ALGORITHM_TYPE_MAX || param == NULL) return -1;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    vp_algorithm_info_t *info = &channel->infos[type];
    memcpy(param, &info->param, sizeof(vp_algorithm_param_t));
    return 0;
}

/**
 * 更新AI算法模块参数
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 * @param param [in]    算法参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_set_param(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, vp_algorithm_param_t *param) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || type >= VP_ALGORITHM_TYPE_MAX || param == NULL) return -1;
    if (g_context.state == VP_ALGORITHM_STATE_IDLE) return -1;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    vp_algorithm_info_t *info = &channel->infos[type];
    memcpy(&info->param, param, sizeof(vp_algorithm_param_t));
    if (info->enable) info->update = 1;
    info->notify = 0;
    vp_stack("update idx:%d chn:%d detect type:%d notify:%d interval:%d jpeg:%d enable:%d notify:%d", idx, chn, type,
             param->notify, param->interval, param->jpeg, info->enable, info->notify);
    return 0;
}

/**
 * 暂停AI算法模块
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_algorithm_pause(uint8_t idx, uint8_t chn, vp_algorithm_type_t type) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    if (g_context.state == VP_ALGORITHM_STATE_IDLE) return -1;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    if (type == VP_ALGORITHM_TYPE_MAX) {
        for (int i = 0; i < VP_ALGORITHM_TYPE_MAX; ++i) {
            channel->infos[i].pause = 1;
        }
    } else {
        channel->infos[type].pause = 1;
    }
    return 0;
}

/**
 * 恢复AI算法模块
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_algorithm_resume(uint8_t idx, uint8_t chn, vp_algorithm_type_t type) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    if (g_context.state == VP_ALGORITHM_STATE_IDLE) return -1;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    if (type == VP_ALGORITHM_TYPE_MAX) {
        for (int i = 0; i < VP_ALGORITHM_TYPE_MAX; ++i) {
            channel->infos[i].pause = 0;
        }
    } else {
        channel->infos[type].pause = 0;
    }
    return 0;
}

/**
 * 等待AI算法模块结果
 *
 * @param idx       [in]    视频传感器索引
 * @param chn       [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type      [in]    算法类型
 * @param timeout   [in]    等待超时时间,单位毫秒(ms)
 * @param result    [out]  算法结果
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_algorithm_wait_result(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, uint32_t timeout,
                             vp_algorithm_result_t *result) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || type > VP_ALGORITHM_TYPE_MAX) return -1;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    vp_algorithm_info_t *info = &channel->infos[type];
    if (info->enable == 0) return -2;
    uint64_t bits = (1 << type);
    uint64_t timestamp = vp_system_time_us();
    if (type == VP_ALGORITHM_TYPE_MAX) {
        return vp_events_wait(channel->events, &bits, VP_EVENTS_FLAG_OR_CLEAR, timeout);
    } else {
        do {
            int ret = vp_events_wait(channel->events, &bits, VP_EVENTS_FLAG_OR_CLEAR, timeout);
            if (ret < 0) return ret;
        } while (info->detect_timestamp < timestamp);
    }

    if (result == NULL) return 0;

    vp_lock(&channel->lock);
    memcpy(result, &info->result, sizeof(vp_algorithm_result_t));
    vp_unlock(&channel->lock);
    return 0;
}

/**
 * 获取AI算法模块结果
 *
 * @param idx       [in]    视频传感器索引
 * @param chn       [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type      [in]    算法类型
 * @param result    [out]   算法结果
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_algorithm_get_result(uint8_t idx, uint8_t chn, vp_algorithm_type_t type, vp_algorithm_result_t *result) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || type >= VP_ALGORITHM_TYPE_MAX) return -1;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    vp_algorithm_info_t *info = &channel->infos[type];
    if (info->enable == 0) return -2;
    vp_lock(&channel->lock);
    memcpy(result, &info->result, sizeof(vp_algorithm_result_t));
    vp_unlock(&channel->lock);
    return 0;
}

/**
 * 停止AI算法模块
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 * @param type  [in]    算法类型
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_stop(uint8_t idx, uint8_t chn, vp_algorithm_type_t type) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_algorithm_channel_t *channel = &g_context.channels[idx][chn];
    if (type == VP_ALGORITHM_TYPE_MAX) {
        for (int i = 0; i < VP_ALGORITHM_TYPE_MAX; ++i) {
            channel->infos[i].enable = 0;
        }
    } else {
        channel->infos[type].enable = 0;
    }
    vp_stack("stop idx:%d chn:%d detect type:%d", idx, chn, type);
    return 0;
}

/**
 * 重新加载AI算法模块
 */
void vp_algorithm_reload() {
    if (g_context.state == VP_ALGORITHM_STATE_IDLE) return;
    vp_algorithm_ivs_reload();
}

/**
 * 释放AI算法模块资源
 */
void vp_algorithm_deinit() {
    if (g_context.state == VP_ALGORITHM_STATE_IDLE) return;
    g_shutting_down = 1;
    g_context.state = VP_ALGORITHM_STATE_IDLE;
    vp_algorithm_ivs_deinit();
    if (g_notify_thread != 0) {
        pthread_join(g_notify_thread, NULL);
        g_notify_thread = 0;
    }
    for (int i = 0; i < VP_SENSOR_NUM; ++i) {
        for (int j = 0; j < vp_video_chn_max; ++j) {
            vp_algorithm_stop(i, j, VP_ALGORITHM_TYPE_MAX);
        }
    }
}