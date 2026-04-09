//
// Created by 杨柳 on 2024/11/22.
//

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "vp_context.h"
#include "vp_observers.h"
#include "vp_video_encoder.h"
#include "vp_printf.h"
#include "vp_video_encoder.h"
#include "vp_algorithm.h"
#include "vp_video_osd.h"
#include "sample_comm.h"
// #if VP_ALGORITHM_TYPE == VP_ALGORITHM_TYPE_YTLD
// #include "vp_flash.h"
// #include "vp_nvs.h"
// #endif

volatile int running = 1;

static void vp_obs_alarm_callback_func(uint32_t type, uint32_t event, void *args, void *user_info) {
    vp_debug("type:%d event:%d", type, event);
    vp_algorithm_notify_t *notify = (vp_algorithm_notify_t *) args;
    vp_debug("idx:%d chn:%d type:%d jpeg:%p size:%d", notify->idx, notify->chn, notify->type, notify->jpeg_data,
             notify->jpeg_size);
    switch (notify->type) {
        case VP_ALGORITHM_TYPE_MOTION_DETECT: {
            char path[1024];
            sprintf(path, "/tmp/nfs/motion_idx%d_ch%d.jpeg", notify->idx, notify->chn);
            FILE *file = fopen(path, "wb");
            if (file) {
                fwrite(notify->jpeg_data, 1, notify->jpeg_size, file);
                fclose(file);
            }
            
            break;
        }
        case VP_ALGORITHM_TYPE_HUMAN_DETECT: {
            char path[1024];
            sprintf(path, "/tmp/nfs/human_idx%d_ch%d.jpeg", notify->idx, notify->chn);
            FILE *file = fopen(path, "wb");
            if (file) {
                fwrite(notify->jpeg_data, 1, notify->jpeg_size, file);
                fclose(file);
            }
            break;
        }
        default:
            break;
    }
    if (notify->jpeg_data) {
        free(notify->jpeg_data);
    }
    free(notify);
}

void *video_thread_func(void *args) {
    pthread_detach(pthread_self());
    int chn = (int) args;
    vp_debug("idx:0 chn:%d start", chn);
    vp_video_coder_frame_t frame;
    char path[1024];
    sprintf(path, "/mnt/sda0/stream_ch%d.h265", chn);
    uint8_t *data;
    int data_size;
    FILE *file = fopen(path, "wb");
    while (running) {
        int ret = vp_video_encoder_request_frame(0, chn, &frame);
        if (ret < 0) break;
        if (ret == 1) continue;
        while ((data_size = vp_video_encoder_frame_data(0, chn, &frame, &data)) > 0) {
            if (file) fwrite(data, data_size, 1, file);
        }
        vp_video_encoder_release_frame(0, chn, &frame);
    }
    fclose(file);
    vp_debug("idx:0 chn:%d exit", chn);
    return NULL;
}

int main(int argv, const char **args) {
    vp_osd_config_text_t text = {
            .color = {.value = 0xFFFFFFFF},
            .font = VP_VIDEO_OSD_FONT_MEDIUM,
            .x = 8,
            .y = 8,
    };

    vp_osd_config_rect_t config = {
            .color = VP_OSD_RECT_GREEN,
            .line = 1,
    };

//     vp_context_init(argv, args);

// #if VP_ALGORITHM_TYPE == VP_ALGORITHM_TYPE_YTLD
//     vp_flash_init();
//     vp_nvs_init();
// #endif
    vp_observers_init();
    vp_observers_add(VP_OBS_TYPE_ALARM, VP_OBS_EVENT_ALARM_ALL, vp_obs_alarm_callback_func, NULL);
    // vp_isp_init();

    int ret = vp_video_encoder_init(0);
    vp_debug("vp_video_encoder_init:%d", ret);

    for (int j = 0; j < VP_SENSOR_NUM; ++j) {
        vp_video_osd_init(j, 0);
        vp_video_osd_init(j, 1);

        for (int i = 0; i < VP_VIDEO_OSD_RECT_MAX; ++i) {
            vp_video_osd_config_rect(j, 0, i, &config);
            vp_video_osd_config_rect(j, 1, i, &config);
        }

        vp_video_osd_config_text(j, 0, 0, &text, "%Y-%m-%d %H:%M:%S");

        ret = vp_video_osd_start(j, 0);
        if (ret) {
            vp_debug("vp_video_osd_start:%d error:%d", 0, ret);
        }
        vp_video_osd_show_text(j, 0, 0);

        text.y = 8;
        vp_video_osd_config_text(j, 1, 0, &text, "%Y-%m-%d %H:%M:%S");

        ret = vp_video_osd_start(j, 1);
        if (ret) {
            vp_debug("vp_video_osd_start:%d error:%d", 1, ret);
        }
        vp_video_osd_show_text(j, 1, 0);

        vp_video_osd_config_mosaic(j, 0, 0, &config);
        vp_video_osd_config_mosaic(j, 0, 1, &config);
        vp_video_osd_config_mosaic(j, 1, 0, &config);
        vp_video_osd_config_mosaic(j, 1, 1, &config);
    }

    #if VP_SENSOR_NUM > 0
    ret = vp_video_encoder_start(0, 0);
    if (ret) {
        vp_debug("vp_video_encoder_start(0,0)error:%d", ret);
        return ret;
    }

    ret = vp_video_encoder_start(0, 1);
    if (ret) {
        vp_debug("vp_video_encoder_start(0,1)error:%d", ret);
        return ret;
    }
    #endif

    #if VP_SENSOR_NUM > 1
    ret = vp_video_encoder_start(1, 0);
    if (ret) {
        vp_debug("vp_video_encoder_start(1,0)error:%d", ret);
        return ret;
    }
    ret = vp_video_encoder_start(1, 1);
    if (ret) {
        vp_debug("vp_video_encoder_start(1,1)error:%d", ret);
        return ret;
    }
    #endif
    #if VP_SENSOR_NUM > 2
    ret = vp_video_encoder_start(2, 0);
    if (ret) {
        vp_debug("vp_video_encoder_start(2,0)error:%d", ret);
        return ret;
    }
    ret = vp_video_encoder_start(2, 1);
    if (ret) {
        vp_debug("vp_video_encoder_start(2,1)error:%d", ret);
        return ret;
    }
    #endif

#if 0
    pthread_t ph;
    pthread_create(&ph, NULL, video_thread_func, (void *) 0);
    pthread_create(&ph, NULL, video_thread_func, (void *) 1);
    pthread_create(&ph, NULL, video_thread_func, (void *) 2);
    pthread_create(&ph, NULL, video_thread_func, (void *) 3);
    pthread_create(&ph, NULL, video_thread_func, (void *) 4);
    pthread_create(&ph, NULL, video_thread_func, (void *) 5);
#endif

#if 1
    VENC_CHN        VencChn[6] = {0,1,2,3,4,5};
    SAMPLE_COMM_VENC_StartGetStreamV2(VencChn, VP_SENSOR_NUM * 2, SAMPLE_VENC_RTSP);
#endif

    // pthread_t ph;
    // pthread_create(&ph, NULL, video_thread_func, (void *) 0);
    // pthread_create(&ph, NULL, video_thread_func, (void *) 1);

    ret = vp_algorithm_init();
    vp_debug("vp_algorithm_init:%d", ret);

    vp_algorithm_param_t motion_param = {.notify = 1, .jpeg = 1, .interval = 30};
    vp_algorithm_param_t human_param = {.notify = 1, .jpeg = 1, .interval = 10};
    for (int j = 0; j < VP_SENSOR_NUM; ++j) {
        // vp_motion_detect_param_default(&motion_param.motion);
        // ret = vp_algorithm_start(j, 1, VP_ALGORITHM_TYPE_MOTION_DETECT, &motion_param);
        // if (ret) {
        //     vp_debug("vp_algorithm_start(0,1)error:%d", ret);
        //     return ret;
        // }
        //vp_isp_set_mirr_flip(j,3);
        vp_cat_detect_param_default(&human_param.cat);
        ret = vp_algorithm_start(j, 1, VP_ALGORITHM_TYPE_HUMAN_DETECT, &human_param);
        if (ret) {
            vp_debug("vp_algorithm_start(0,1)error:%d", ret);
            return ret;
        }
    }

    // vp_algorithm_param_t convergence_param = {.notify = 0, .jpeg = 0, .interval = 0};
    // vp_convergence_detect_param_default(&convergence_param.convergence);
    // ret = vp_algorithm_start(0, 1, VP_ALGORITHM_TYPE_CONVERGENCE_DETECT, &convergence_param);
    // if (ret) {
    //     vp_debug("vp_algorithm_start(0,1)error:%d", ret);
    //     return ret;
    // }

    // 自动启动视频编码器
    // running = 1;
    // ret = vp_video_encoder_start(0, 0);
    // if (ret) {
    //     vp_debug("vp_video_encoder_start(0,0)error:%d", ret);
    // }
    // ret = vp_video_encoder_start(0, 1);
    // if (ret) {
    //     vp_debug("vp_video_encoder_start(0,1)error:%d", ret);
    // }

    char line[1024];
    vp_algorithm_result_t result;
    while (1) {
        char *string = fgets(line, 1024, stdin);
        if (string == NULL) { 
            sleep(1);
            continue;
        }
        if (strncmp(line, "exit", 4) == 0) break;

        if (strncmp(line, "video_start", strlen("video_start")) == 0) {
            running = 1;
            ret = vp_video_encoder_start(0, 0);
            if (ret) {
                vp_debug("vp_video_encoder_start(0,0)error:%d", ret);
                return ret;
            }

            ret = vp_video_encoder_start(0, 1);
            if (ret) {
                vp_debug("vp_video_encoder_start(0,1)error:%d", ret);
                return ret;
            }

            // pthread_create(&ph, NULL, video_thread_func, (void *) 0);
            // pthread_create(&ph, NULL, video_thread_func, (void *) 1);

        } else if (strncmp(line, "video_stop", strlen("video_stop")) == 0) {
            running = 0;

            vp_video_encoder_stop(0, 1);
            vp_video_encoder_stop(0, 0);

        } else if (strncmp(line, "start", strlen("start")) == 0) {

            ret = vp_algorithm_start(0, 1, VP_ALGORITHM_TYPE_MOTION_DETECT, &motion_param);
            vp_debug("vp_algorithm_start: %d", ret);
            ret = vp_algorithm_start(0, 1, VP_ALGORITHM_TYPE_HUMAN_DETECT, &human_param);
            vp_debug("vp_algorithm_start: %d", ret);

        } else if (strncmp(line, "stop", strlen("stop")) == 0) {

            ret = vp_algorithm_stop(0, 1, VP_ALGORITHM_TYPE_MOTION_DETECT);
            vp_debug("vp_algorithm_stop:%d", ret);
            ret = vp_algorithm_stop(0, 1, VP_ALGORITHM_TYPE_HUMAN_DETECT);
            vp_debug("vp_algorithm_stop:%d", ret);

        } else if (strncmp(line, "pause", strlen("pause")) == 0) {


            ret = vp_algorithm_pause(0, 1, VP_ALGORITHM_TYPE_MOTION_DETECT);
            vp_debug("vp_algorithm_pause:%d", ret);
            ret = vp_algorithm_pause(0, 1, VP_ALGORITHM_TYPE_HUMAN_DETECT);
            vp_debug("vp_algorithm_pause:%d", ret);

            vp_video_encoder_pause(0, 1);
            vp_video_encoder_pause(0, 0);

        } else if (strncmp(line, "resume", strlen("resume")) == 0) {

            ret = vp_algorithm_resume(0, 1, VP_ALGORITHM_TYPE_MOTION_DETECT);
            vp_debug("vp_algorithm_resume:%d", ret);
            ret = vp_algorithm_resume(0, 1, VP_ALGORITHM_TYPE_HUMAN_DETECT);
            vp_debug("vp_algorithm_resume:%d", ret);

            vp_video_encoder_resume(0, 0);
            vp_video_encoder_resume(0, 1);

        } else if (strncmp(line, "set_param", strlen("set_param")) == 0) {

            static uint8_t flag = 0;
            motion_param.motion.sensitivity += 1;
            if (flag == 0) {
                flag = 1;
                memset(motion_param.motion.detect_area, 0, sizeof(motion_param.motion.detect_area));
                memset(human_param.human.detect_area, 0, sizeof(human_param.human.detect_area));
            } else {
                flag = 0;
                memset(motion_param.motion.detect_area, 0xFF, sizeof(motion_param.motion.detect_area));
                memset(human_param.human.detect_area, 0xFF, sizeof(human_param.human.detect_area));
            }
            ret = vp_algorithm_set_param(0, 1, VP_ALGORITHM_TYPE_MOTION_DETECT, &motion_param);
            vp_debug("set_ivs_param:%d", ret);
            ret = vp_algorithm_set_param(0, 1, VP_ALGORITHM_TYPE_HUMAN_DETECT, &human_param);
            vp_debug("set_ivs_param:%d", ret);

        } else if (strncmp(line, "wait_result", strlen("wait_result")) == 0) {

            ret = vp_algorithm_wait_result(0, 1, VP_ALGORITHM_TYPE_HUMAN_DETECT, 5000, &result);
            vp_debug("wait_result:%d", ret);
            ret = vp_algorithm_wait_result(0, 1, VP_ALGORITHM_TYPE_MOTION_DETECT, 5000, &result);
            vp_debug("wait_result:%d", ret);

        } else if (strncmp(line, "get_result", strlen("get_result")) == 0) {

            ret = vp_algorithm_get_result(0, 1, VP_ALGORITHM_TYPE_HUMAN_DETECT, &result);
            vp_debug("get_result:%d", ret);
            ret = vp_algorithm_get_result(0, 1, VP_ALGORITHM_TYPE_MOTION_DETECT, &result);
            vp_debug("get_result:%d", ret);

        }
    }
    running = 0;
    vp_algorithm_deinit();
    vp_debug("vp_algorithm_deinit");
    vp_video_encoder_deinit();
    vp_debug("vp_video_encoder_deinit");
    return 0;
}