#ifndef VP_ISP_H
#define VP_ISP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * 初始化ISP
 */
void vp_isp_init();

/**
 * 设置图像输出帧率
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    图像帧率
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_isp_set_fps(uint8_t idx, uint8_t value);

/**
 * 获取图像输出帧率
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   图像帧率
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_isp_get_fps(uint8_t idx, uint8_t *value);

typedef enum {
    /* 未知模式 */
    VP_ISP_RUNNING_UNKNOW,
    /* 白天模式 */
    VP_ISP_RUNNING_DAY,
    /* 夜晚模式 */
    VP_ISP_RUNNING_NIGHT,
    /* 自定义模式 */
    VP_ISP_RUNNING_CUSTOM,
    VP_ISP_RUNNING_MAX,
} vp_isp_running_mode_t;

/**
 * 设置ISP工作模式
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param mode  [in]    工作模式
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_isp_set_run_mode(uint8_t idx, vp_isp_running_mode_t mode);

/**
 * 获取ISP当前工作模式
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param mode  [out]   工作模式
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_isp_get_run_mode(uint8_t idx, vp_isp_running_mode_t *mode);

/**
 * 软光敏检测
 *
 * @param index [in]    传感器序号 [0,3]
 *
 * @return 根据检测光敏值返回适合的工作模式
 *
 * @retval VP_ISP_RUNNING_UNKNOW 未知模式,检测出现错误
 * @retval VP_ISP_RUNNING_DAY    白天模式
 * @retval VP_ISP_RUNNING_NIGHT  夜晚模式
 */
vp_isp_running_mode_t vp_isp_photosensitive_check(uint8_t index);

/**
 * 设置图像对比度
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    对比度,默认使用128 取值范围[0-255]
 *                      大于128增加对比度,小于128降低对比度
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_contrast(uint8_t idx, uint8_t value);

/**
 * 获取图像对比度
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   对比度
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_contrast(uint8_t idx, uint8_t *value);

/**
 * 设置图像锐度值
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    锐度值,默认使用128 取值范围[0-255]
 *                      大于128增加锐度值,小于128降低锐度值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_sharpness(uint8_t idx, uint8_t value);

/**
 * 获取图像锐度值
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   锐度值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_sharpness(uint8_t idx, uint8_t *value);

/**
 * 设置图像饱和度
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    饱和度,默认使用128 取值范围[0-255]
 *                      大于128增加饱和度,小于128降低饱和度
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_saturation(uint8_t idx, uint8_t value);

/**
 * 获取图像饱和度
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   饱和度
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_saturation(uint8_t idx, uint8_t *value);

/**
 * 设置图像亮度值
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    亮度值,默认使用128 取值范围[0-255]
 *                      大于128增加亮度值,小于128降低亮度值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_brightness(uint8_t idx, uint8_t value);

/**
 * 获取图像亮度值
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   亮度值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_brightness(uint8_t idx, uint8_t *value);

/**
 * 设置图像色调
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param hue   [in]    色调值,默认使用128 取值范围[0-255]
 *                      大于128增加色调,小于128降低色调
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_hue(uint8_t idx, uint8_t value);

/**
 * 获取图像色调
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   色调值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_hue(uint8_t idx, uint8_t *value);

/**
 * 设置图像镜像翻转
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param mirr  [in]    镜像使能,1 启用 0 禁用
 * @param flip  [in]    翻转使能,1 启用 0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_mirr_flip(uint8_t idx, uint8_t mirr, uint8_t flip);

/**
 * 获取图像镜像翻转使能
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param mirr  [out]   镜像使能,1 启用 0 禁用
 * @param flip  [out]   翻转使能,1 启用 0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_mirr_flip(uint8_t idx, uint8_t *mirr, uint8_t *flip);

typedef enum {
    VP_ISP_ANTI_FLICKER_OFF,
    VP_ISP_ANTI_FLICKER_50HZ,
    VP_ISP_ANTI_FLICKER_60HZ,
    VP_ISP_ANTI_FLICKER_MAX,
} vp_isp_anti_flicker_freq_t;

/**
 * 设置图像防闪烁
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param freq  [in]    防闪烁频率
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_anti_flicker(uint8_t idx, vp_isp_anti_flicker_freq_t freq);

/**
 * 获取图像防闪烁
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param freq  [out]   防闪烁频率
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_anti_flicker(uint8_t idx, vp_isp_anti_flicker_freq_t *freq);

typedef enum {
    VP_ISP_CORE_WB_MODE_AUTO = 0,          /**< 自动模式 */
    VP_ISP_CORE_WB_MODE_MANUAL,            /**< 手动模式 */
    VP_ISP_CORE_WB_MODE_DAY_LIGHT,         /**< 晴天 */
    VP_ISP_CORE_WB_MODE_CLOUDY,            /**< 阴天 */
    VP_ISP_CORE_WB_MODE_INCANDESCENT,      /**< 白炽灯 */
    VP_ISP_CORE_WB_MODE_FLOURESCENT,       /**< 荧光灯 */
    VP_ISP_CORE_WB_MODE_TWILIGHT,          /**< 黄昏 */
    VP_ISP_CORE_WB_MODE_SHADE,             /**< 阴影 */
    VP_ISP_CORE_WB_MODE_WARM_FLOURESCENT,  /**< 暖色荧光灯 */
    VP_ISP_CORE_WB_MODE_CUSTOM,            /**< 自定义模式 */
} vp_isp_core_wb_mode_t;
typedef struct {
    vp_isp_core_wb_mode_t mode;        /**< 白平衡模式，分为自动与手动模式 */
    uint16_t rgain;                       /**< 红色增益，手动模式时有效 */
    uint16_t bgain;                       /**< 蓝色增益，手动模式时有效 */

    uint32_t wgt_rgain;
    uint32_t wgt_bgain;
    uint32_t global_rgain;
    uint32_t global_bgain;
    uint32_t wgt_grgain;
    uint32_t wgt_gbgain;

} vp_isp_wb_info_t;

/**
 * 获取WB（白平衡）属性。
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   WB参数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_wb_info(uint8_t idx, vp_isp_wb_info_t *value);

typedef struct {
    uint32_t ev;            /**< 曝光值 */
    uint32_t expr_us;       /**< 曝光时间 */
    uint32_t ev_log2;       /**<log格式曝光时间 */
    uint32_t again;         /**< 模拟增益 */
    uint32_t dgain;         /**< 数字增益 */
    uint32_t gain_log2;     /**< log格式增益 */
} vp_isp_ev_info_t;

/**
 * 获取EV属性。
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   EV参数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_ev_info(uint8_t idx, vp_isp_ev_info_t *value);

/**
 * AE曝光时间单位
 */
typedef enum {
    VP_ISP_AE_TIMEUNIT_LINE,            /**< 单位为曝光行 */
    VP_ISP_AE_TIMEUNIT_US,                /**< 单位为微秒 */
} vp_isp_ae_timeunit_t;
/**
 * ISP功能模式
 */
typedef enum {
    VP_ISP_TUNING_OPS_TYPE_AUTO,            /**< 该模块的操作为自动模式 */
    VP_ISP_TUNING_OPS_TYPE_MANUAL,            /**< 该模块的操作为手动模式 */
    VP_ISP_TUNING_OPS_TYPE_BUTT,            /**< 用于判断参数的有效性，参数大小必须小于这个值 */
} vp_isp_tuning_ops_type_t;

typedef struct {
    vp_isp_ae_timeunit_t integration_timeunit;              /**< AE曝光时间单位 */
    vp_isp_tuning_ops_type_t mode;                          /**< AE Freezen使能 */
    vp_isp_tuning_ops_type_t integration_time_mode;         /**< AE曝光手动模式使能 */
    vp_isp_tuning_ops_type_t again_manual_mode;             /**< AE Sensor 模拟增益手动模式使能 */
    vp_isp_tuning_ops_type_t dgain_manual_mode;             /**< AE Sensor数字增益手动模式使能 */
    vp_isp_tuning_ops_type_t isp_dgain_manual_mode;         /**< AE ISP 数字增益手动模式使能 */
    uint32_t integration_time;                              /**< AE手动模式下的曝光值 */
    uint32_t again;                                         /**< AE Sensor 模拟增益值，单位是倍数 x 1024 */
    uint32_t dgain;                                         /**< AE Sensor数字增益值，单位是倍数 x 1024 */
    uint32_t isp_dgain;                                     /**< AE ISP 数字增益值，单位倍数 x 1024 */

    vp_isp_tuning_ops_type_t min_integration_time_mode;     /**< AE最小曝光使能位(预留) */
    vp_isp_tuning_ops_type_t min_again_mode;                /**< AE最小模拟增益使能位 */
    vp_isp_tuning_ops_type_t min_dgain_mode;                /**< AE最小数字增益使能位(预留) */
    vp_isp_tuning_ops_type_t min_isp_dgain_mode;            /**< AE最小ISP数字增益使能位(预留) */
    vp_isp_tuning_ops_type_t max_integration_time_mode;     /**< AE最大曝光使能位 */
    vp_isp_tuning_ops_type_t max_again_mode;                /**< AE最大sensor模拟增益使能位 */
    vp_isp_tuning_ops_type_t max_dgain_mode;                /**< AE最大sensor数字增益使能位 */
    vp_isp_tuning_ops_type_t max_isp_dgain_mode;            /**< AE最大ISP数字增益使能位 */
    uint32_t min_integration_time;                          /**< AE最小曝光时间 */
    uint32_t min_again;                                     /**< AE最小sensor模拟增益，单位是倍数 x 1024 */
    uint32_t min_dgain;                                     /**< AE最小sensor数字增益，单位是倍数 x 1024 */
    uint32_t min_isp_dgain;                                 /**< AE最小ISP数字增益，单位是倍数 x 1024 */
    uint32_t max_integration_time;                          /**< AE最大曝光时间 */
    uint32_t max_again;                                     /**< AE最大sensor模拟增益，单位是倍数 x 1024 */
    uint32_t max_dgain;                                     /**< AE最大sensor数字增益，单位是倍数 x 1024 */
    uint32_t max_isp_dgain;                                 /**< AE最大ISP数字增益，单位是倍数 x 1024 */

    /* WDR模式下短帧的AE 手动模式属性*/
    vp_isp_tuning_ops_type_t short_mode;                    /**< AE Freezen使能 */
    vp_isp_tuning_ops_type_t short_integration_time_mode;   /**< AE曝光手动模式使能 */
    vp_isp_tuning_ops_type_t short_again_manual_mode;       /**< AE Sensor 模拟增益手动模式使能 */
    vp_isp_tuning_ops_type_t short_dgain_manual_mode;       /**< AE Sensor数字增益手动模式使能 */
    vp_isp_tuning_ops_type_t short_isp_dgain_manual_mode;   /**< AE ISP 数字增益手动模式使能 */
    uint32_t short_integration_time;                        /**< AE手动模式下的曝光值 */
    uint32_t short_again;                                   /**< AE Sensor 模拟增益值，单位是倍数 x 1024 */
    uint32_t short_dgain;                                   /**< AE Sensor数字增益值，单位是倍数 x 1024 */
    uint32_t short_isp_dgain;                               /**< AE ISP 数字增益值，单位倍数 x 1024 */

    vp_isp_tuning_ops_type_t short_min_integration_time_mode; /**< AE最小曝光使能位(预留) */
    vp_isp_tuning_ops_type_t short_min_again_mode;           /**< AE最小模拟增益使能位 */
    vp_isp_tuning_ops_type_t short_min_dgain_mode;           /**< AE最小数字增益使能位(预留) */
    vp_isp_tuning_ops_type_t short_min_isp_dgain_mode;       /**< AE最小ISP数字增益使能位(预留) */
    vp_isp_tuning_ops_type_t short_max_integration_time_mode;/**< AE最大曝光使能位 */
    vp_isp_tuning_ops_type_t short_max_again_mode;           /**< AE最大sensor模拟增益使能位 */
    vp_isp_tuning_ops_type_t short_max_dgain_mode;           /**< AE最大sensor数字增益使能位 */
    vp_isp_tuning_ops_type_t short_max_isp_dgain_mode;       /**< AE最大ISP数字增益使能位 */
    uint32_t short_min_integration_time;                     /**< AE最小曝光时间 */
    uint32_t short_min_again;                                /**< AE最小sensor模拟增益 */
    uint32_t short_min_dgain;                                /**< AE最小sensor数字增益 */
    uint32_t short_min_isp_dgain;                            /**< AE最小ISP数字增益 */
    uint32_t short_max_integration_time;                     /**< AE最大曝光时间 */
    uint32_t short_max_again;                                /**< AE最大sensor模拟增益 */
    uint32_t short_max_dgain;                                /**< AE最大sensor数字增益 */
    uint32_t short_max_isp_dgain;                            /**< AE最大ISP数字增益 */

    uint32_t sensor_expt_long;                               /**< AE 曝光值，单位为s */
    uint32_t sensor_again_long;
    uint32_t isp_dgain_long;

    uint32_t total_gain_db;                                  /**< AE total gain，单位为db(只读) */
    uint32_t total_gain_db_short;                            /**< AE 短帧 total gain, 单位为db */
    uint64_t exposure_value;                                 /**< AE 曝光值，为integration time x again x dgain */
    uint32_t ev_log2;                                        /**< AE 曝光值，此值经过log运算 */
} vp_isp_ae_expr_info_t;

/**
 * 获取AE统计信息
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   统计信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_ae_expr(uint8_t idx, vp_isp_ae_expr_info_t *value);

typedef struct {
    void *revert;
    int8_t stable;                       /**< AE 收敛状态 -1 为不支持*/
    uint32_t hi_light_depress;           /**< AE 强光抑制强度（0 ~ 10）*/
    uint32_t bl_light_strength;          /**< AE 背光补偿强度（0 ~ 10） */
    uint32_t target_comp;                /**< AE 目标亮度调节强度（0 ~ 255，小于128变暗，大于128变亮） */
} vp_isp_ae_info_t;

/**
 * 获取AE参数
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   AE参数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_ae_info(uint8_t idx, vp_isp_ae_info_t *value);

/**
 * 设置AE参数
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    AE参数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_ae_info(uint8_t idx, vp_isp_ae_info_t *value);

/**
 * 还原AE参数
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    AE参数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_revert_ae_info(uint8_t idx, vp_isp_ae_info_t *value);

/**
 * 释放AE参数
 *
 * @param value [in]    AE参数
 */
void vp_isp_clear_ae_info(vp_isp_ae_info_t *value);

typedef struct {
    unsigned int weight[15][15];    /**< 各区域统计信息 [0 ~ 255]*/
} vp_isp_ae_statistics_t;

/**
 * 获取AE统计信息
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   AE统计信息
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_ae_statistics(uint8_t idx, vp_isp_ae_statistics_t *value);

/**
 * AE 权重信息
 */
typedef struct {
    void *revert;
    unsigned char weight[15][15];    /**< 各区域权重信息 [0 ~ 8]*/
} vp_isp_ae_weight_t;

/**
 * 获取AE权重
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   AE权重
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_ae_weight(uint8_t idx, vp_isp_ae_weight_t *value);

/**
 * 设置AE权重
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    AE权重
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_ae_weight(uint8_t idx, vp_isp_ae_weight_t *value);

/**
 * 还原AE权重
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    AE权重
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_revert_ae_weight(uint8_t idx, vp_isp_ae_weight_t *value);

/**
 * 释放资源
 *
 * @param value [in]    AE权重
 */
void vp_isp_clear_ae_weight(vp_isp_ae_weight_t *value);

typedef enum {
    /* 白天模式 */
    VP_ISP_IR_MODE_DAY = 0,
    /* 自动模式 */
    VP_ISP_IR_MODE_AUTO,
    /* 夜晚模式 */
    VP_ISP_IR_MODE_NIGHT,
    VP_ISP_IR_MODE_MAX
} vp_isp_ir_mode_t;

/**
 * 设置ir夜视模式
 *
 * @param mode [in]    夜视模式
 */
void vp_isp_set_ir_night_mode(vp_isp_ir_mode_t mode);

/**
 * 获取芯片ID
 *
 * @param chip_id [out] 芯片ID
 *
 * @return 0 成功 非0失败
 */
int vp_isp_get_chip_id(char *chip_id);

/**
 * 开启监听MIPI切换
 *
 * @param enable 0:关闭监听 1:开启监听
 *
 */
void vp_isp_enable_mipi_listen(int enable);

/**
 * 反初始化ISP,销毁所有初始化创建的资源
 */
void vp_isp_deinit();

#ifdef __cplusplus
}
#endif

#endif
    