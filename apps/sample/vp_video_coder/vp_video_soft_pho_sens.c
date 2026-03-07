//
// Created by xupengyu on 2024/10/23.
//
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vp_pthread.h"
#include "vp_printf.h"
#include "vp_video_soft_pho_sens.h"
#include "vp_context_config.h"
#include "sample_comm.h"

#if 0
#define DEBUG_PRINTF
#endif

#define VP_ISP_PARAM_DAY_2_NIGHT            "day2night"
#define VP_ISP_PARAM_NIGHT_2_DAY            "night2day"
#define VP_ISP_PARAM_GB_LIMIT               "gblimit"
#define VP_ISP_PARAM_GB_MAX_VALID           "gbmaxvalid"
#define VP_ISP_PARAM_DAY_EV_COMP            "dayevcomp"
#define VP_ISP_PARAM_NIGHT_EV_COMP          "nightevcomp"
#define VP_ISP_PARAM_GB_COMP                "gbcomp"

#define VP_SOFT_PHO_SENSOR                  (0)

typedef struct {
    int running;
    vp_video_soft_pho_mode_e soft_model;
    vp_video_mo_pho_mode_e mo_soft_model;
    vp_isp_sensor_param_t isp_sensor_param;
    int gain_buf[GAIN_STATIST_NUM];
    int gain_index;
} vp_video_soft_pho_sens_t;

static vp_video_soft_pho_sens_t g_vp_video_soft = {
        .running = 0,
        .mo_soft_model = VIDEO_SOFT_AUTO,
#if VP_POWER_TYPE == VP_POWER_TYPE_LOW
        .soft_model = -1,
#endif
};

static void vp_soft_sensor_read_param(int *day2night, int *night2day, int *gb_limit, int *gb_max_valid,
                                      int *day_ev_comp, int *night_ev_comp, int *gb_comp) {
    if (access(DEBUG_FILE_DIR, F_OK) != 0) return;
    FILE *fp = fopen(DEBUG_FILE_DIR, "rb");
    if (fp == NULL) return;
    char read_buf[128];
    memset(read_buf, 0, sizeof(read_buf));
    fread(read_buf, 1, sizeof(read_buf), fp);
    fclose(fp);

    vp_debug("read param: %s", read_buf);

    char *pread = NULL;
    long temp;
    char *end_ptr = NULL;

    pread = strstr(read_buf, VP_ISP_PARAM_DAY_2_NIGHT);
    if (pread != NULL) {
        pread += strlen(VP_ISP_PARAM_DAY_2_NIGHT);
        temp = strtol(pread, &end_ptr, 10);
        if (*end_ptr == '\0') {
            *day2night = (int) temp;
        } else {
            vp_error("Invalid value for %s: %s", VP_ISP_PARAM_DAY_2_NIGHT, pread);
        }
    }

    pread = strstr(read_buf, VP_ISP_PARAM_NIGHT_2_DAY);
    if (pread != NULL) {
        pread += strlen(VP_ISP_PARAM_NIGHT_2_DAY);
        temp = strtol(pread, &end_ptr, 10);
        if (*end_ptr == '\0') {
            *night2day = (int) temp;
        } else {
            vp_error("Invalid value for %s: %s", VP_ISP_PARAM_NIGHT_2_DAY, pread);
        }
    }

    pread = strstr(read_buf, VP_ISP_PARAM_GB_LIMIT);
    if (pread != NULL) {
        pread += strlen(VP_ISP_PARAM_GB_LIMIT);
        temp = strtol(pread, &end_ptr, 10);
        if (*end_ptr == '\0') {
            *gb_limit = (int) temp;
        } else {
            vp_error("Invalid value for %s: %s", VP_ISP_PARAM_GB_LIMIT, pread);
        }
    }

    pread = strstr(read_buf, VP_ISP_PARAM_GB_MAX_VALID);
    if (pread != NULL) {
        pread += strlen(VP_ISP_PARAM_GB_MAX_VALID);
        temp = strtol(pread, &end_ptr, 10);
        if (*end_ptr == '\0') {
            *gb_max_valid = (int) temp;
        } else {
            vp_error("Invalid value for %s: %s", VP_ISP_PARAM_GB_MAX_VALID, pread);
        }
    }

    pread = strstr(read_buf, VP_ISP_PARAM_DAY_EV_COMP);
    if (pread != NULL) {
        pread += strlen(VP_ISP_PARAM_DAY_EV_COMP);
        temp = strtol(pread, &end_ptr, 10);
        if (*end_ptr == '\0') {
            *day_ev_comp = (int) temp;
        } else {
            vp_error("Invalid value for %s: %s", VP_ISP_PARAM_DAY_EV_COMP, pread);
        }
    }

    pread = strstr(read_buf, VP_ISP_PARAM_NIGHT_EV_COMP);
    if (pread != NULL) {
        pread += strlen(VP_ISP_PARAM_NIGHT_EV_COMP);
        temp = strtol(pread, &end_ptr, 10);
        if (*end_ptr == '\0') {
            *night_ev_comp = (int) temp;
        } else {
            vp_error("Invalid value for %s: %s", VP_ISP_PARAM_NIGHT_EV_COMP, pread);
        }
    }

    pread = strstr(read_buf, VP_ISP_PARAM_GB_COMP);
    if (pread != NULL) {
        pread += strlen(VP_ISP_PARAM_GB_COMP);
        temp = strtol(pread, &end_ptr, 10);
        if (*end_ptr == '\0') {
            *gb_comp = (int) temp;
        } else {
            vp_error("Invalid value for %s: %s", VP_ISP_PARAM_GB_COMP, pread);
        }
    }
}

//根据sensor获取切换值
static int vp_soft_sensor_update_switch_value(int *day2night, int *night2day, int *gb_limit, int *gb_max_valid,
                                              int *day_ev_comp, int *night_ev_comp, int *gb_comp) {
    // char sensor_name[32] = {0};
    // vp_sensor_info_t sensorInfo;
    // vp_sensor_info(&sensorInfo);
    // strcpy(sensor_name, sensorInfo.configs[0].name);

    // if (strcmp(sensor_name, "gc2083") == 0) {
    //     *day2night = GC2083_DAY2NIGHT_EV;
    //     *night2day = GC2083_NIGHT2DAY_EV;
    //     *gb_limit = GC2083_GB_VALUE;
    //     *gb_max_valid = GC2083_GB_MAX_LIMIT;
    //     *day_ev_comp = GC2083_DAY_EV_COMPENSATE;
    //     *night_ev_comp = GC2083_NIGHT_EV_COMPENSATE;
    //     *gb_comp = GC2083_NIGHT_GB_COMPENSATE;
    // } else if (strcmp(sensor_name, "os02n10") == 0) {
    //     *day2night = OS02N10_DAY2NIGHT_EV;
    //     *night2day = OS02N10_NIGHT2DAY_EV;
    //     *gb_limit = OS02N10_GB_VALUE;
    //     *gb_max_valid = OS02N10_GB_MAX_LIMIT;
    //     *day_ev_comp = OS02N10_DAY_EV_COMPENSATE;
    //     *night_ev_comp = OS02N10_NIGHT_EV_COMPENSATE;
    //     *gb_comp = OS02N10_NIGHT_GB_COMPENSATE;
    // } else {
    //     *day2night = GC1084_DAY2NIGHT_EV;
    //     *night2day = GC1084_NIGHT2DAY_EV;
    //     *gb_limit = GC1084_GB_VALUE;
    //     *gb_max_valid = GC1084_GB_MAX_LIMIT;
    //     *day_ev_comp = GC1084_DAY_EV_COMPENSATE;
    //     *night_ev_comp = GC1084_NIGHT_EV_COMPENSATE;
    //     *gb_comp = GC1084_NIGHT_GB_COMPENSATE;
    // }

    vp_soft_sensor_read_param(day2night, night2day, gb_limit, gb_max_valid, day_ev_comp, night_ev_comp, gb_comp);

    if (g_vp_video_soft.isp_sensor_param.isp_sensor_day2night != -1) {
        *day2night = g_vp_video_soft.isp_sensor_param.isp_sensor_day2night;
    }

    if (g_vp_video_soft.isp_sensor_param.isp_sensor_night2day != -1) {
        *night2day = g_vp_video_soft.isp_sensor_param.isp_sensor_night2day;
    }

    if (g_vp_video_soft.isp_sensor_param.isp_sensor_gb_limit != -1) {
        *gb_limit = g_vp_video_soft.isp_sensor_param.isp_sensor_gb_limit;
    }

    if (g_vp_video_soft.isp_sensor_param.isp_sensor_gb_max_valid != -1) {
        *gb_max_valid = g_vp_video_soft.isp_sensor_param.isp_sensor_gb_max_valid;
    }

    if (g_vp_video_soft.isp_sensor_param.isp_sensor_day_ev_comp != -1) {
        *day_ev_comp = g_vp_video_soft.isp_sensor_param.isp_sensor_day_ev_comp;
    }

    if (g_vp_video_soft.isp_sensor_param.isp_sensor_night_ev_comp != -1) {
        *night_ev_comp = g_vp_video_soft.isp_sensor_param.isp_sensor_night_ev_comp;
    }

    if (g_vp_video_soft.isp_sensor_param.isp_sensor_gb_comp != -1) {
        *gb_comp = g_vp_video_soft.isp_sensor_param.isp_sensor_gb_comp;
    }

    vp_debug("%s=%d %s=%d %s=%d %s=%d %s=%d %s=%d %s=%d", VP_ISP_PARAM_DAY_2_NIGHT, *day2night,
             VP_ISP_PARAM_NIGHT_2_DAY, *night2day, VP_ISP_PARAM_GB_LIMIT, *gb_limit,
             VP_ISP_PARAM_DAY_EV_COMP, *day_ev_comp, VP_ISP_PARAM_NIGHT_EV_COMP, *night_ev_comp,
             VP_ISP_PARAM_GB_MAX_VALID, *gb_max_valid, VP_ISP_PARAM_GB_COMP, *gb_comp);

    return 0;
}

//统计白天的平衡值做为比较
static int vp_soft_sensor_gain_statistics(int gb_gain) {
    if (g_vp_video_soft.gain_index >= GAIN_STATIST_NUM) {
        g_vp_video_soft.gain_index = 0;
    }

    g_vp_video_soft.gain_buf[g_vp_video_soft.gain_index] = gb_gain;
    g_vp_video_soft.gain_index++;

    return 0;
}

static int vp_soft_sensor_get_day_gb_gain(int gb_limit_max) {
    int i;
    int total = 0;
    int sum = 0;
    int first_time = 0;
    int min, max;
    int ret_value = 140;
    for (i = 0; i < GAIN_STATIST_NUM; i++) {
        //200为实测异常门限值
        if ((g_vp_video_soft.gain_buf[i] != 0) && (g_vp_video_soft.gain_buf[i] < gb_limit_max)) {
            if (first_time == 0) {
                min = g_vp_video_soft.gain_buf[i];
                max = g_vp_video_soft.gain_buf[i];
                first_time = 1;
            }

            if (min < g_vp_video_soft.gain_buf[i]) {
                min = g_vp_video_soft.gain_buf[i];
            }

            if (max > g_vp_video_soft.gain_buf[i]) {
                max = g_vp_video_soft.gain_buf[i];
            }

            total++;
            sum += g_vp_video_soft.gain_buf[i];

            vp_debug("g_gain_buf[%d] = %d", i, g_vp_video_soft.gain_buf[i]);
        }
    }

    if (total > 2) {
        ret_value = (sum - max - min) / (total - 2);
    }

#ifdef DEBUG_PRINTF
    vp_debug("max[%d], min[%d] sum[%d] total[%d]", max, min, sum, total);
#endif
    return ret_value;
}

#if 0 //动态帧率 实验性功能
#define SENSOR_DYNAMIC_FPS 
#endif

static void *vp_soft_sensor_thread(void *arg) {
    int ret, gb_gain;
    uint64_t ev_value;
    int gb_gain_limit;
    int day2night, night2day, gb_limit, gb_max_limit, day_ev_comp, night_ev_comp, gb_comp;
    int day2night_cnt = 0, night2day_cnt = 0;
    int day2night_flag = 0; //用于第一次切换时获取当前状态位
    int last_day_gain = 140;
    int i, sum, count = 0;
    int first_night_ev_buf[NIGHT_EV_STA_NUM] = {0};
    int first_night_flag = 0;
    int night_ev_average = 0; //初始进入夜视时的ev平均值
    // IMPISPRunningMode isp_mode;
    // IMPISPEVAttr ExpAttr;
    // IMPISPWB wb;
    TS_U32 isp_mode;
    int day_toggle_flag   = 0;
    int night_toggle_flag = 0;

#ifdef SENSOR_DYNAMIC_FPS
    static char set_fps_flag = 0;
#endif

    vp_pthread_name("vp_video_soft_pho_proc");

    vp_soft_sensor_update_switch_value(&day2night, &night2day, &gb_limit, &gb_max_limit, &day_ev_comp, &night_ev_comp,
                                       &gb_comp);

    ISP_IR_AUTO_ATTR_S ir_auto_attr = {
        .Day2NightThresh = 1000000,
        .IrNight2DayThresh = 100000,
        .WlNight2DayThresh = 100000,
        .RgMax = 280,
        .RgMin = 240,
        .BgMax = 280,
        .BgMin = 240,
    };

    while (g_vp_video_soft.running) {
        TS_MPT_ISP_IrAuto(0, &ir_auto_attr);
        if (ir_auto_attr.IR_switch != ISP_IR_SWITCH_TO_NONE) {
            if (ir_auto_attr.IR_switch == ISP_IR_SWITCH_TO_DAY) {
                g_vp_video_soft.soft_model = VIDEO_SOFT_DAY;
            } else if (ir_auto_attr.IR_switch == ISP_IR_SWITCH_TO_NIGHT) {
                g_vp_video_soft.soft_model = VIDEO_SOFT_INFRARED_NIGHT;
            }
        }

        if (g_vp_video_soft.soft_model == VIDEO_SOFT_DAY && day_toggle_flag == 0) {
            day_toggle_flag   = 1;
            night_toggle_flag = 0;
			SAMPLE_COMM_ISP_SetParamIndex(0, 0);
        } else if (g_vp_video_soft.soft_model == VIDEO_SOFT_INFRARED_NIGHT && night_toggle_flag == 0) {
            night_toggle_flag = 1;
            day_toggle_flag   = 0;
			SAMPLE_COMM_ISP_SetParamIndex(0, 1);
        }

        usleep(500000);
    }

    g_vp_video_soft.running = 0;
    pthread_detach(pthread_self());
    return NULL;
}

/**
 * 软光敏初始化
 *
 * @return 0 成功 非 0失败
 */
int vp_video_soft_pho_sens_init() {
    g_vp_video_soft.running = 1;
    memset(g_vp_video_soft.gain_buf, 0, sizeof(g_vp_video_soft.gain_buf));
    memset(&g_vp_video_soft.isp_sensor_param, -1, sizeof(g_vp_video_soft.isp_sensor_param));
    pthread_t pth;
    vp_pthread_create(&pth, 0, vp_soft_sensor_thread, NULL);
    vp_debug("vp_video_soft_pho_sens init success");
    return 0;
}

/**
 * 获取当前软光敏模式
 *
 * @return 模式
 */
vp_video_soft_pho_mode_e vp_video_soft_pho_get_model() {
    vp_video_soft_pho_mode_e ret;
    if (g_vp_video_soft.mo_soft_model == VIDEO_SOFT_FORCE_DAY) {
        ret = VIDEO_SOFT_DAY;
    } else if (g_vp_video_soft.mo_soft_model == VIDEO_SOFT_FORCE_INFRARED_NIGHT) {
        ret = VIDEO_SOFT_INFRARED_NIGHT;
    } else {
        ret = g_vp_video_soft.soft_model;
    }
    return ret;
}

/**
 * 手动强制切换软光敏模式
 *
 * @param model 模式
 */
void vp_video_soft_pho_set_model(vp_video_mo_pho_mode_e model) {
    g_vp_video_soft.mo_soft_model = model;
}

/**
 * 更新软光敏的初始状态
 *
 * @param ir_states 快起boot的白天夜视状态
 */
void vp_video_soft_pho_set_init_sta(vp_video_mo_pho_mode_e ir_states)
{
    if(ir_states == VIDEO_SOFT_DAY)
    {
        g_vp_video_soft.soft_model = VIDEO_SOFT_DAY;
    }
    else
    {
        g_vp_video_soft.soft_model = VIDEO_SOFT_INFRARED_NIGHT;
    }
}

/**
 * 获取EV
 *
 */
int vp_video_get_exposure_value(uint64_t *value) {
    // int ret = 0;
    // ISP_EXP_INFO_S stExpInfo = { 0 };
    // ret = TS_MPI_ISP_QueryExposureInfo(VP_SOFT_PHO_SENSOR, &stExpInfo);
    
    // if (ret == 0) {
    //     *value = (uint64_t)stExpInfo.u32TotalGain * stExpInfo.u32ExpTime;
    // }

    // return ret;
    return 0;
}
/**
 * 获取awb bgain
 *
 */
int vp_video_get_awb_bgain(uint32_t *value) {
    // int ret = 0;
    // ISP_WB_INFO_S stWBInfo;
    // TS_MPI_ISP_QueryWBInfo(VP_SOFT_PHO_SENSOR, &stWBInfo);

    // if(ret == 0) {
    //     *value = stWBInfo.awbBGain;
    // }

    // return ret;
    return 0;
}