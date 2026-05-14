//
// Created by 杨柳 on 2024/11/26.
//

#ifndef VP_ALGORITHM_VP_ALGORITHM_TYPE_H
#define VP_ALGORITHM_VP_ALGORITHM_TYPE_H

#include <string.h>
#include <stdint.h>

typedef enum {
    /* 人形检测算法 */
    VP_ALGORITHM_TYPE_HUMAN_DETECT = 0,
    /* 移动检测算法 */
    VP_ALGORITHM_TYPE_MOTION_DETECT,
    /* 高亮收敛算法 */
    VP_ALGORITHM_TYPE_CONVERGENCE_DETECT,
//    /* 物体检测算法 */
//    VP_ALGORITHM_TYPE_OBJECT_DETECT,
//    /* 人脸检测算法 */
//    VP_ALGORITHM_TYPE_FACE_DETECT,
//    /* 车辆检测算法 */
//    VP_ALGORITHM_TYPE_VEHICLE_DETECT,
//    /* 宠物检测算法 */
//    VP_ALGORITHM_TYPE_PET_DETECT,
//    /* 烟火检测算法 */
//    VP_ALGORITHM_TYPE_SMOKE_DETECT,
    VP_ALGORITHM_TYPE_MAX,
} vp_algorithm_type_t;


typedef struct {
    /**
     * 检测间隔，单位为帧，默认为0 ，表示每帧检测一次
     */
    uint8_t interval;
    /**
     * 灵敏度，取值范围[0, 3]，值越低灵敏度越高
     */
    uint8_t sensitivity;
    /**
     * 检测区域
     * 将整个画面分为宽22,高18的区域
     * 每个bit位表示一个区域，1表示检测该区域，0表示不检测该区域
     * 一个uint32_t 代表一行区域，共18行，共22列，共22*18=396个区域
     */
    uint32_t detect_area[18];
} vp_motion_detect_param_t;

/**
 * 设置默认参数
 * @param param [in] param 参数
 */
static inline void vp_motion_detect_param_default(vp_motion_detect_param_t *param) {
    param->interval = 1;
    param->sensitivity = 1;
    memset(param->detect_area, 0xFF, sizeof(param->detect_area));
}

typedef struct {
    /**
     * 是否启用画框
     */
    uint8_t draw_box;
    /**
     * 是否启用马赛克
     */
    uint8_t draw_mosaic;
    /**
     * 检测区域
     * 将整个画面分为宽22,高18的区域
     * 每个bit位表示一个区域，1表示检测该区域，0表示不检测该区域
     * 一个uint32_t 代表一行区域，共18行，共22列，共22*18=396个区域
     */
    uint32_t detect_area[18];
} vp_cat_detect_param_t;

static inline void vp_cat_detect_param_default(vp_cat_detect_param_t *param) {
    param->draw_box = 1;
    param->draw_mosaic = 1;//0;
    memset(param->detect_area, 0xFF, sizeof(param->detect_area));
}

typedef struct {
    /**
     * 是否启用追踪
     */
    uint8_t enable_track;
    /**
     * 是否启用画框
     */
    uint8_t draw_box;

    /**
     * 检测区域
     * 将整个画面分为宽22,高18的区域
     * 每个bit位表示一个区域，1表示检测该区域，0表示不检测该区域
     * 一个uint32_t 代表一行区域，共18行，共22列，共22*18=396个区域
     */
    uint32_t detect_area[18];
} vp_human_detect_param_t;

/**
 * 设置默认参数
 * @param param [in] param 参数
 */
static inline void vp_human_detect_param_default(vp_human_detect_param_t *param) {
    param->enable_track = 0;
    param->draw_box = 1;
    memset(param->detect_area, 0xFF, sizeof(param->detect_area));
}

typedef struct {
    /**
     * 获取LED状态
     * @return 0: 关闭, 1: 开启
     */
    int (*led_state)();

    /**
     * 高档过亮阈值
     */
    uint8_t threshold_h;
    /**
     * 中档过亮阈值
     */
    uint8_t threshold_m;
    /**
     * 低档过亮阈值
     */
    uint8_t threshold_l;
    /**
     * 低亮阈值
     */
    uint8_t threshold_b;
} vp_convergence_detect_param_t;

/**
 * 设置默认参数
 * @param param [in] param 参数
 */
static inline void vp_convergence_detect_param_default(vp_convergence_detect_param_t *param) {
    param->threshold_l = 186;
    param->threshold_m = 200;
    param->threshold_h = 220;
    param->threshold_b = 40;
}

typedef struct {
    /**
     * 是否启用亮度调整
     */
    uint8_t brightness_adjustment;
} vp_qrcode_detect_param_t;

/**
 * 设置默认参数
 * @param param [in] param 参数
 */
static inline void vp_qrcode_detect_param_default(vp_qrcode_detect_param_t *param) {
    param->brightness_adjustment = 0x01;
}

typedef struct {
    /**
     * 是否启用通知
     */
    uint8_t notify;
    /**
     * 是否启用jpeg抓图
     */
    uint8_t jpeg;
    /**
     * 通知触发间隔,单位秒
     */
    uint16_t interval;

    union {
        vp_motion_detect_param_t motion;
        vp_human_detect_param_t human;
        vp_cat_detect_param_t cat;
        vp_convergence_detect_param_t convergence;
        vp_qrcode_detect_param_t qrcode;
    };
} vp_algorithm_param_t;

typedef struct {
    uint16_t x, y;
    uint16_t w, h;
} vp_area_rect_t;

typedef struct {
    uint8_t state;
} vp_motion_detect_result_t;

typedef struct {
    int track_id;
    float score; //!< 目标分数
    vp_area_rect_t rect; //!< 目标检测框
} vp_human_obj_t;

typedef struct {
    uint8_t count;
    vp_human_obj_t objs[15];
} vp_human_detect_result_t;

typedef enum {
    VP_CAT_ACT_OUT = 0,
    VP_CAT_ACT_INT,
    VP_CAT_ACT_EAT,
    VP_CAT_ACT_EAT_OUT,
} vp_cat_act_t;

typedef enum {
    VP_CAT_CLASS_ID_FACE = 0,
    VP_CAT_CLASS_ID_FOOD,
    VP_CAT_CLASS_ID_HEAD,
    VP_CAT_CLASS_ID_SIDE,
} vp_cat_class_id_t;

#define VP_MAX_CAT_DET_NUM     25

typedef struct {
    float Xmin;
    float Ymin;
    float Xmax;
    float Ymax;
    float Conf;
    float Sim;
    char nameid[64];
    vp_cat_act_t act;
    int cls_id;
    char f_in;
    char f_eat;
    int cam_id;
    vp_cat_act_t act_cat;
    int act_sta;
    char cat_f_in;
    char cat_f_eat;
    int sta;
    int evt;
    int cnt_in;
    int cnt_eat;
} vp_cat_obj_t;

typedef struct {
    uint32_t count;
    vp_cat_obj_t objs[VP_MAX_CAT_DET_NUM];
} vp_cat_detect_result_t;

typedef struct {
     /* 高亮区域个数
     */
    uint8_t high_count;
    /**
     * 低亮区域个数
     */
    uint8_t low_count;
} vp_convergence_detect_result_t;

typedef struct {
    /**
     * 二维码识别内容
     */
    const char *text;
} vp_qrcode_detect_result_t;

typedef struct {
    /**
     * 检测状态
     * 0 无结果, 1 检测到结果
     */
    uint8_t state;
    /**
     * 算法类型
     */
    vp_algorithm_type_t type;
    /**
     * 检测图像宽
     */
    uint32_t width;
    /**
     * 检测图像高
     */
    uint32_t height;
    /**
     * 触发时间,系统启动时间,单位为微秒
     */
    uint64_t timestamp;
    union {
        vp_motion_detect_result_t motion;
        vp_human_detect_result_t human;
        vp_cat_detect_result_t cat;
        vp_convergence_detect_result_t convergence;
        vp_qrcode_detect_result_t qrcode;
    };
} vp_algorithm_result_t;

typedef struct {
    /**
     * Sensor ID
     */
    uint8_t idx;
    /**
     * Channel ID
     */
    uint8_t chn;
    /**
     * 算法类型
     */
    vp_algorithm_type_t type;

    /**
     * 检测图像宽
     */
    uint32_t width;
    /**
     * 检测图像高
     */
    uint32_t height;
    /**
     * jpeg图片数据
     */
    uint8_t *jpeg_data;
    /**
     * jpeg图片大小
     */
    uint32_t jpeg_size;

    /**
     * 算法检测结果
     */
    vp_algorithm_result_t result;
} vp_algorithm_notify_t;
#endif //VP_ALGORITHM_VP_ALGORITHM_TYPE_H
