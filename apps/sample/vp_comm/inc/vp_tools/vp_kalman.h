//
// Created by 杨柳 on 2025/1/14.
//

#ifndef VP_TOOLS_VP_KALMAN_H
#define VP_TOOLS_VP_KALMAN_H

// 卡尔曼滤波结构体
typedef struct {
    float x; // 状态估计值
    float Q; // 过程噪声协方差
    float R; // 测量噪声协方差
    float P; // 状态协方差估计值
    float K; // 卡尔曼增益
} vp_kalman_t;

/**
 * 初始化卡尔曼滤波器
 *
 * @param kalman    [in]   卡尔曼滤波器结构体
 * @param x         [in]   状态估计值
 * @param p         [in]   状态协方差估计值
 * @param q         [in]   过程噪声协方差
 * @param r         [in]   测量噪声协方差
 * @param k         [in]   卡尔曼增益
 *
 * @return          无
 */
void vp_kalman_init(vp_kalman_t *kalman, float x, float p, float q, float r,float k);

/**
 * 卡尔曼滤波
 *
 * @param kalman    [in]   卡尔曼滤波器结构体
 * @param value     [in]   测量值
 *
 * @return          状态估计值
 */
float vp_kalman_filter(vp_kalman_t *kalman, float value);

#endif //VP_TOOLS_VP_KALMAN_H
