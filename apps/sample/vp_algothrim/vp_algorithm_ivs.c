//
// Created by 杨柳 on 2024/11/26.
//
#include <stdlib.h>
#include "vp_algorithm_ivs.h"
#include "vp_printf.h"
#include "vp_video_encoder.h"
#include "vp_time.h"
// #include "vp_sensor_config.h"
#include "vp_pthread.h"

typedef struct {
    volatile uint8_t start;
    volatile uint8_t bind;
    uint16_t width;
    uint16_t height;
    uint64_t timestamp;
} vp_bind_info_t;

typedef struct {
    uint8_t init_flag;
    volatile uint8_t reload;
    vp_algorithm_ivs_args_t args;
    vp_video_source_t frame;
    vp_bind_info_t binds[VP_SENSOR_NUM][vp_video_chn_max];
} vp_algorithm_ivs_ctx_t;

static vp_algorithm_ivs_ctx_t g_ctx = {0};

static void *vp_algorithm_ivs_thread(void *args) {
    (void) args;
    vp_pthread_name("algorithm_ivs");
    pthread_detach(pthread_self());
    vp_bind_info_t *bind;
    uint64_t timestamp;
    int ret;
    if (g_ctx.args.init) g_ctx.args.init(&g_ctx.args);
    vp_debug("start");
    while (g_ctx.init_flag) {
        for (int i = 0; i < VP_SENSOR_NUM; ++i) {
            for (int j = 0; j < vp_video_chn_max; ++j) {
                bind = &g_ctx.binds[i][j];

                if (bind->bind && bind->start == 0) {
                    bind->start = 1;
                    if (g_ctx.args.start) {
                        ret = g_ctx.args.start(i, j, &g_ctx.args);
                        if (ret < 0) {
                            bind->bind = 0;
                            continue;
                        }
                    }
                }

                if (bind->bind) {
                    if (bind->timestamp) {
                        timestamp = vp_system_time_us();
                        // if (timestamp - bind->timestamp < 50000) {
                        //     //usleep(timestamp - bind->timestamp);
                        //     usleep(30*1000);
                        // }
                    }
                    ret = vp_video_encoder_request_source(i, j, &g_ctx.frame);
                    if (ret) continue;
                    bind->timestamp = vp_system_time_us();

                    if (g_ctx.args.before) g_ctx.args.before(i, j, &g_ctx.args, &g_ctx.frame);

                    if (g_ctx.args.process) g_ctx.args.process(i, j, &g_ctx.args, &g_ctx.frame);

                    if (g_ctx.args.after) g_ctx.args.after(i, j, &g_ctx.args, &g_ctx.frame);

                    vp_video_encoder_release_source(i, j, &g_ctx.frame);
                } else if (bind->start) {
                    vp_debug("stop %d %d", i, j);
                    bind->start = 0;
                    bind->timestamp = 0;
                    if (g_ctx.args.stop) g_ctx.args.stop(i, j, &g_ctx.args);
                }
            }
            
        }

        if (g_ctx.reload) {
            g_ctx.reload = 0;
            for (int i = 0; i < VP_SENSOR_NUM; ++i) {
                for (int j = 0; j < vp_video_chn_max; ++j) {
                    bind = &g_ctx.binds[i][j];
                    if (bind->bind && bind->start) {
                        vp_debug("stop %d %d", i, j);
                        bind->start = 0;
                        bind->timestamp = 0;
                        if (g_ctx.args.stop) g_ctx.args.stop(i, j, &g_ctx.args);
                    }
                }
            }
            if (g_ctx.args.deinit) g_ctx.args.deinit(&g_ctx.args);
            //sleep(1);
            if (g_ctx.args.init) g_ctx.args.init(&g_ctx.args);
        }
    }

    if (g_ctx.args.deinit) g_ctx.args.deinit(&g_ctx.args);
    if (g_ctx.frame.frame_data) free(g_ctx.frame.frame_data);
    g_ctx.frame.frame_data = NULL;
    vp_debug("stop");
    return NULL;
}

/**
 * 初始化IVS算法通道
 * @param args  [in]    IVS参数
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_ivs_init(vp_algorithm_ivs_args_t *args) {
    if (args == NULL) return -1;
    if (g_ctx.init_flag) return 0;
    memset(&g_ctx, 0, sizeof(g_ctx));
    g_ctx.init_flag = 1;
    memcpy(&g_ctx.args, args, sizeof(vp_algorithm_ivs_args_t));
    pthread_t pthread;
    return vp_pthread_create(&pthread, 64 * 1024, vp_algorithm_ivs_thread, NULL);
}

/**
 * 绑定IVS算法通道
 *
 * @param idx   [in]    视频传感器索引
 * @param chn   [in]    视频流通道索引,0 主码流通道,1 子码流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_ivs_bind(uint8_t idx, uint8_t chn) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || g_ctx.init_flag == 0) return -1;
    vp_bind_info_t *info = &g_ctx.binds[idx][chn];
    if (info->bind) return -2;

    vp_video_encoder_config_t config;
    int ret = vp_video_encoder_get_config(idx, chn, &config);
    if (ret) return -2;
    info->width = (int16_t) config.width;
    info->height = (int16_t) config.height;
    info->bind = 1;
    return 0;
}

/**
 * 解绑IVS算法通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_ivs_unbind(uint8_t idx, uint8_t chn) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_bind_info_t *info = &g_ctx.binds[idx][chn];
    if (info->bind == 0) return -2;
    info->bind = 0;
    return 0;
}

/**
 * 重新加载IVS算法通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_ivs_reload() {
    if (g_ctx.init_flag == 0) return -1;
    g_ctx.reload = 1;
    return 0;
}

/**
 * 释放IVS算法通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_algorithm_ivs_deinit() {
    g_ctx.init_flag = 0;
    for (int i = 0; i < VP_SENSOR_NUM; ++i) {
        for (int j = 0; j < vp_video_chn_max; ++j) {
            vp_algorithm_ivs_unbind(i, j);
        }
    }
    return 0;
}