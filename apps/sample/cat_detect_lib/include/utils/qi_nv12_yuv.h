#ifndef __QI_NV12_YUV_H__
#define __QI_NV12_YUV_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief NV12图像缩放函数（支持等比例缩放）
 * 
 * @param src          [IN] 源NV12数据指针
 * @param src_width    [IN] 源图像宽度
 * @param src_height   [IN] 源图像高度
 * @param dst          [OUT] 目标缓冲区指针（需预分配）
 * @param dst_width    [IN] 目标图像宽度
 * @param dst_height   [IN] 目标图像高度
 * @param keep_aspect  [IN] 是否保持宽高比（1=保持，0=拉伸）
 * 
 * @return 0成功，-1失败（参数无效）
 */
int nv12_scale_ex(unsigned char* src, int src_width, int src_height,
                  unsigned char* dst, int dst_width, int dst_height,
                  int keep_aspect);

/**
 * @brief NV12图像缩放函数（最近邻插值，速度优先）
 * 
 * @param src          [IN] 源NV12数据指针
 * @param src_width    [IN] 源图像宽度
 * @param src_height   [IN] 源图像高度
 * @param dst          [OUT] 目标缓冲区指针（需预分配）
 * @param dst_width    [IN] 目标图像宽度
 * @param dst_height   [IN] 目标图像高度
 * @param keep_aspect  [IN] 是否保持宽高比（1=保持，0=拉伸）
 * 
 * @return 0成功，-1失败（参数无效）
 */
int nv12_scale_nearest(unsigned char* src, int src_width, int src_height,
                       unsigned char* dst, int dst_width, int dst_height,
                       int keep_aspect);

/**
 * @brief NV12图像缩放函数（固定点运算，平衡方案）
 * 
 * @param src          [IN] 源NV12数据指针
 * @param src_width    [IN] 源图像宽度
 * @param src_height   [IN] 源图像高度
 * @param dst          [OUT] 目标缓冲区指针（需预分配）
 * @param dst_width    [IN] 目标图像宽度
 * @param dst_height   [IN] 目标图像高度
 * @param keep_aspect  [IN] 是否保持宽高比（1=保持，0=拉伸）
 * 
 * @return 0成功，-1失败（参数无效）
 */
//int nv12_scale_fixed(unsigned char* src, int src_width, int src_height,
                  //   unsigned char* dst, int dst_width, int dst_height,
                   //  int keep_aspect);

/**
 * @brief NV12上下拼接函数（正确的拼接实现）
 * 
 * @param src_nv12_top    [IN] 上帧NV12数据
 * @param src_nv12_bottom [IN] 下帧NV12数据
 * @param dst_nv12        [OUT] 拼接结果缓冲区（需预分配）
 * @param width           [IN] 图像宽度
 * @param height          [IN] 单帧图像高度
 * @param strideY         [IN] Y平面的内存跨距（可能 >= width）
 * @param strideUV        [IN] UV平面的内存跨距（可能 >= width）
 * 
 * @return 0成功，-1失败（参数无效）
 */
int nv12_vertical_concat_correct(const unsigned char *src_nv12_top,
                                 const unsigned char *src_nv12_bottom,
                                 unsigned char *dst_nv12,
                                 int width, int height,
                                 int strideY, int strideUV);

/**
 * @brief 拼接缩放参数结构体
 * 
 * 用于管理NV12图像拼接和缩放过程中的所有参数和缓冲区
 */
typedef struct {
    int src_width;        /* [IN] 拼接后图像宽度（等于single_width） */
    int src_height;       /* [IN] 拼接后图像高度（等于single_height * 2） */
    int dst_width;        /* [IN] 目标缩放后图像宽度 */
    int dst_height;       /* [IN] 目标缩放后图像高度 */
    int single_width;     /* [IN] 单帧图像宽度 */
    int single_height;    /* [IN] 单帧图像高度 */
    uint8_t *frame_top;     /* [INTERNAL] 上帧缓冲区 */
    uint8_t *frame_bottom;  /* [INTERNAL] 下帧缓冲区 */
    uint8_t *frame_concat;  /* [INTERNAL] 拼接后缓冲区 */
    uint8_t *frame_scaled;  /* [INTERNAL] 缩放后缓冲区 */
} STITCH_SCALE_PARAM_S;

/**
 * @brief 初始化拼接缩放模块
 * 
 * @param pParam        [OUT] 拼接缩放参数结构体指针
 * @param single_width  [IN] 单帧图像宽度
 * @param single_height [IN] 单帧图像高度
 * @param dst_width     [IN] 目标缩放后图像宽度
 * @param dst_height    [IN] 目标缩放后图像高度
 * 
 * @return 0成功，-1失败（参数无效或内存分配失败）
 */
int STITCH_SCALE_Init(STITCH_SCALE_PARAM_S *pParam, int single_width, int single_height, 
                     int dst_width, int dst_height);

/**
 * @brief 清理拼接缩放模块
 * 
 * 释放所有分配的内存缓冲区
 * 
 * @param pParam [IN] 拼接缩放参数结构体指针
 * 
 * @return 0成功，-1失败（参数无效）
 */
int STITCH_SCALE_Deinit(STITCH_SCALE_PARAM_S *pParam);

/**
 * @brief 执行拼接和缩放处理（手动实现版本）
 * 
 * 将两帧NV12图像上下拼接后缩放到目标尺寸
 * 
 * @param pParam       [IN] 拼接缩放参数结构体指针
 * @param pTopFrame    [IN] 上帧NV12数据
 * @param pBottomFrame [IN] 下帧NV12数据
 * 
 * @return 0成功，-1失败（参数无效或处理失败）
 */
int STITCH_SCALE_Process(STITCH_SCALE_PARAM_S *pParam, 
                       uint8_t *pTopFrame, uint8_t *pBottomFrame);

/**
 * @brief 执行拼接和缩放处理（快速版本，使用固定点运算）
 * 
 * 将两帧NV12图像上下拼接后缩放到目标尺寸，使用固定点运算优化
 * 
 * @param pParam       [IN] 拼接缩放参数结构体指针
 * @param pTopFrame    [IN] 上帧NV12数据
 * @param pBottomFrame [IN] 下帧NV12数据
 * 
 * @return 0成功，-1失败（参数无效或处理失败）
 */
int STITCH_SCALE_Process_Fast(STITCH_SCALE_PARAM_S *pParam, 
                              uint8_t *pTopFrame, uint8_t *pBottomFrame);

/**
 * @brief 执行拼接和缩放处理（OpenCV实现版本）
 * 
 * 将两帧NV12图像上下拼接后缩放到目标尺寸，使用OpenCV实现
 * 
 * @param pParam       [IN] 拼接缩放参数结构体指针
 * @param pTopFrame    [IN] 上帧NV12数据
 * @param pBottomFrame [IN] 下帧NV12数据
 * 
 * @return 0成功，-1失败（参数无效或处理失败）
 */
// int STITCH_SCALE_Process_Opencv(STITCH_SCALE_PARAM_S *pParam, 
//                                 uint8_t *pTopFrame, uint8_t *pBottomFrame);

// /**
//  * @brief 获取缩放后的帧数据
//  * 
//  * @param pParam [IN] 拼接缩放参数结构体指针
//  * 
//  * @return 缩放后的NV12数据指针，失败返回NULL
//  */
// uint8_t* STITCH_SCALE_GetScaledFrame(STITCH_SCALE_PARAM_S *pParam);
int nv12_scale_fit_fast(const uint8_t *src_nv12,
                         int src_w, int src_h, int src_stride,
                         uint8_t *dst_nv12,
                         int dst_w, int dst_h);

/**
 * @brief RGBA 等比例缩放并居中填充到目标尺寸（同你NV12风格）
 * @param src_rgba   输入RGBA数据
 * @param src_w      原图宽度
 * @param src_h      原图高度
 * @param src_stride 原图行跨度（字节），通常 = src_w * 4
 * @param dst_rgba   输出RGBA数据（外部分配）
 * @param dst_w      目标宽度
 * @param dst_h      目标高度
 * @return 0 成功，非0 失败
 */
int rgba_scale_fit_fast(const uint8_t *src_rgba,
                        int src_w, int src_h, int src_stride,
                        uint8_t *dst_rgba,
                        int dst_w, int dst_h);
#ifdef __cplusplus
}
#endif

#endif
