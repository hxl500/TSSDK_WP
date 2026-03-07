//
// Created by 杨柳 on 2024/8/21.
//

#ifndef VP_VIDEO_CODER_VP_VIDEO_SOFT_PHO_SENS_H
#define VP_VIDEO_CODER_VP_VIDEO_SOFT_PHO_SENS_H

#include <stdint.h>

#define GAIN_STATIST_NUM            10    // 用于统计白天的gbgain
#define NIGHT_EV_STA_NUM            3    // 用于统计刚进入晚上的的EV值 在gbgain变化交小的情况下比较ev值
#define DEBUG_FILE_DIR              "/tmp/ev.ini"

#define GC2083_DAY2NIGHT_EV        110000    //60000
#define GC2083_NIGHT2DAY_EV        1100    //4500
#define GC2083_GB_VALUE            50    //40  白平衡 蓝色增益补偿值
#define GC2083_GB_MAX_LIMIT        209    //160 全黑时用于判断的临界值 主要用于过滤夜晚切白天的情况
#define GC2083_DAY_EV_COMPENSATE    750    //600 增益合理范围时，增益补偿值 越小晚上切白天切的越早
#define GC2083_NIGHT_EV_COMPENSATE  0        //200 增益未调节时，增益补偿值 越小晚上切白天切的越早
#define GC2083_NIGHT_GB_COMPENSATE  20    //20  增益调节 当前GB小于 MAX_LIMIT -GB 则可直接切白天

#define OS02N10_DAY2NIGHT_EV        90000    //85000
#define OS02N10_NIGHT2DAY_EV        1100    //4500
#define OS02N10_GB_VALUE            47    //白平衡 蓝色增益补偿值
#define OS02N10_GB_MAX_LIMIT        208    //160 //全黑时用于判断的临界值 主要用于过滤夜晚切白天的情况
#define OS02N10_DAY_EV_COMPENSATE    750    //600 //增益合理范围时，增益补偿值 越小晚上切白天切的越早
#define OS02N10_NIGHT_EV_COMPENSATE  0        //200 //增益未调节时，增益补偿值 越小晚上切白天切的越早
#define OS02N10_NIGHT_GB_COMPENSATE  26    //增益调节 当前GB小于 MAX_LIMIT -GB 则可直接切白天

#define GC1084_DAY2NIGHT_EV        80000    //85000
#define GC1084_NIGHT2DAY_EV        3100    //4500
#define GC1084_GB_VALUE            54    //白平衡 蓝色增益补偿值
#define GC1084_GB_MAX_LIMIT        220    //160 //全黑时用于判断的临界值 主要用于过滤夜晚切白天的情况
#define GC1084_DAY_EV_COMPENSATE    3000    //600 //增益合理范围时，增益补偿值 越小晚上切白天切的越早
#define GC1084_NIGHT_EV_COMPENSATE  0        //200 //增益未调节时，增益补偿值 越小晚上切白天切的越早
#define GC1084_NIGHT_GB_COMPENSATE  41    //增益调节 当前GB小于 MAX_LIMIT -GB 则可直接切白天


typedef struct {
    int isp_sensor_day2night;
    int isp_sensor_night2day;
    int isp_sensor_gb_limit;
    int isp_sensor_gb_max_valid;
    int isp_sensor_day_ev_comp;
    int isp_sensor_night_ev_comp;
    int isp_sensor_gb_comp;
    int isp_sensor_night2day_ex;
} vp_isp_sensor_param_t;


typedef enum {
    VIDEO_SOFT_DAY = 0,             /*白天*/
    VIDEO_SOFT_INFRARED_NIGHT,      /*（红外）黑白夜视【T41ZM需要人形补光的夜视】*/
    VIDEO_SOFT_TRAD_NIGHT,          /*传统黑光*/
    VIDEO_SOFT_AI_ISP_NIGHT,        /*AI_ISP黑光*/
} vp_video_soft_pho_mode_e;

typedef enum {
    VIDEO_SOFT_FORCE_DAY = 0,             /*强制白天*/
    VIDEO_SOFT_FORCE_INFRARED_NIGHT,      /*强制（红外）黑白夜视*/
    VIDEO_SOFT_AUTO,                      /*自动模式*/
} vp_video_mo_pho_mode_e;

#if (VP_SDK_PLATFORM == VP_SDK_PLATFORM_T41 || VP_SDK_PLATFORM == VP_SDK_PLATFORM_T23 || \
     VP_SDK_PLATFORM == VP_SDK_PLATFORM_T32 || VP_SDK_PLATFORM == VP_SDK_PLATFORM_T31 || \
     VP_SDK_PLATFORM == VP_SDK_PLATFORM_T32ZL)

/**
 * 软光敏的初始化函数
 *
 */
int vp_video_soft_pho_sens_init();

/**
 * 获取当前日夜模式
 *
 */
vp_video_soft_pho_mode_e vp_video_soft_pho_get_model();

/**
 * 手动强制切换软光敏模式
 *
 * @param model 模式
 */
void vp_video_soft_pho_set_model(vp_video_mo_pho_mode_e model);

/**
 * 更新软光敏的初始状态
 *
 * @param model 模式
 */
void vp_video_soft_pho_set_init_sta(vp_video_mo_pho_mode_e ir_states);

/**
 * 获取EV
 *
 */
int vp_video_get_exposure_value(uint64_t *value)
/**
 * 获取awb bgain
 *
 */
int vp_video_get_awb_bgain(uint32_t *value);

#ifdef VP_AOV_SRC

/**
 * 设置软光敏是否进入AOV模式
 *
 * @param value  0：正常模式，1：AOV模式
 */

void vp_video_soft_pho_set_aov_model(uint8_t model);

/**
 * 设置软光敏是否进入暂停模式
 *
 * @param value  0：正常模式，1：暂停检测模式
 */
void vp_video_soft_pho_set_pause_model(uint8_t model);

#else
#define vp_video_soft_pho_set_aov_model(x) (-1)
#define vp_video_soft_pho_set_pause_model(x) (-1)
#endif

#else
int vp_video_soft_pho_sens_init();
vp_video_soft_pho_mode_e vp_video_soft_pho_get_model();
void vp_video_soft_pho_set_model(vp_video_mo_pho_mode_e model);
#endif


#endif //VP_VIDEO_CODER_VP_VIDEO_SOFT_PHO_SENS_H