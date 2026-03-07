#ifndef VP_VIDEO_ENCODER_H
#define VP_VIDEO_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vp_video_coder_type.h"
#include "vp_video_coder_config.h"
#include "vp_video_encoder_platform.h"

#define VP_SENSOR_NUM 2

/**
 * 初始化视频编码模块
 *
 * @param format   [in]        1：视频264编码使能  其他： 使用默认编码
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_init(int format);

/**
 * 获取编码基础配置
 *
 * @param idx   [in]        视频编号,与 Sensor 对应
 * @param chn   [in]        视频流通道
 * @param chn   [in,out]    视频流通道配置结构体
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_get_config(uint8_t idx, vp_video_chn_t chn, vp_video_encoder_config_t *config);

/**
 * 获取编码特征数据
 *
 * @param idx       [in]        视频编号,与 Sensor 对应
 * @param chn       [in]        视频流通道
 * @param specific  [out]       特征数据
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_specific(uint8_t idx, vp_video_chn_t chn, vp_video_encoder_specific_t *specific);

/**
 * 获取编码异常状态
 *
 * @param idx   [in]        视频编号,与 Sensor 对应
 *
 * @return 状态码
 * @retval -1 获取失败
 * @retval =0 无异常
 * @retval >0 异常
 */
int vp_video_encoder_error_flag(uint8_t idx);

/**
 * 获取视频编码帧率
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param fps   [out]   视频编码帧率
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_get_fps(uint8_t idx, vp_video_chn_t chn, float *fps);

/**
 * 设置GOP大小
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param gop   [in]    GOP大小
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_set_gop(uint8_t idx, vp_video_chn_t chn, uint32_t gop);

/**
 * 获取GOP大小
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param gop   [out]    GOP大小
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_get_gop(uint8_t idx, vp_video_chn_t chn, uint32_t *gop);

/**
 * 开启视频编码
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_start(uint8_t idx, vp_video_chn_t chn);

/**
 * 暂停视频编码
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_pause(uint8_t idx, vp_video_chn_t chn);

/**
 * 恢复视频编码
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_resume(uint8_t idx, vp_video_chn_t chn);

/**
 * 插入关键帧编码
 *
 * @details 调用插入关键帧函数后,下一帧可能不是关键帧.需要对请求的帧进行判断
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_install_key(uint8_t idx, vp_video_chn_t chn);

/**
 * 获取视频原始帧(未编码数据帧)
 *
 * @details 该接口获取用于编码的视频数据,图像格式为NV12
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param frame [out]   输出帧信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_request_source(uint8_t idx, vp_video_chn_t chn, vp_video_source_t *frame);

/**
 * 获取视频原始帧(未编码数据帧)
 *
 * @details 该接口获取用于编码的视频数据,图像格式为NV12
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param frame [out]   输出帧信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_request_source_algo(uint8_t idx, vp_video_chn_t chn, vp_video_source_t *frame);

/**
 * 获取JPEG编码图片
 *
 * @param idx       [in]    视频编号,与 Sensor 对应
 * @param chn       [in]    视频流通道
 * @param frame     [in]    视频原始帧数据,可为空
 *                          为空时,将使用指定流中当前数据进行JPEG编码
 *                          不为空,则使用传入的帧数据进行JPEG编码
 * @param jpeg_data [in]    JPEG图片数据存储地址
 * @param jpeg_size [in,out]输入数据存储长度,输出编码后JPEG数据长度
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_jpeg(uint8_t idx, vp_video_chn_t chn, vp_video_source_t *frame,
                          uint8_t *jpeg_data, uint32_t *jpeg_size);

/**
 * 释放视频原始帧
 *
 * @param frame [in]    输出帧信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_release_source(uint8_t idx, vp_video_chn_t chn, vp_video_source_t *frame);

/**
 * 释放视频原始帧
 *
 * @param frame [in]    输出帧信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_release_source_algo(uint8_t idx, vp_video_chn_t chn, vp_video_source_t *frame);

/**
 * 请求视频帧
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param frame [out]   输出流信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_request_frame(uint8_t idx, vp_video_chn_t chn, vp_video_coder_frame_t *frame);

/**
 * 获取视频帧数据
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param frame [in]    输入帧信息
 * @param data  [out]   输出帧数据指针地址
 *
 * @return 错误码或帧数据长度
 * @retval >0 帧数据长度
 * @retval =0 帧结束
 * @retval <0 失败
 */
int vp_video_encoder_frame_data(uint8_t idx, vp_video_chn_t chn, vp_video_coder_frame_t *frame, uint8_t **data);

/**
 * 释放视频帧
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param frame [in]    输出流信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_release_frame(uint8_t idx, vp_video_chn_t chn, vp_video_coder_frame_t *frame);

/**
 * 停止视频编码
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_stop(uint8_t idx, vp_video_chn_t chn);

/**
 * 设置Sensor通道码率信息
 *
 * @param idx       [in]     Sensor索引
 * @param chn       [in]     通道索引
 * @param bit_rate  [in]     码率大小
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_set_bitrate(uint8_t idx, vp_video_chn_t chn, int bitrate);

/**
 * 销毁视频编码
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_deinit();

#ifdef __cplusplus
}
#endif

#endif
    