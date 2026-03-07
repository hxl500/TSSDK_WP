//
// Created by 杨柳 on 2024/8/12.
//

#ifndef VP_VIDEO_CODER_VP_VIDEO_OSD_H
#define VP_VIDEO_CODER_VP_VIDEO_OSD_H

#include "vp_video_coder_type.h"

/**
 * 初始化视频OSD水印
 *
 * @param idx       [in]    视频编号,与 Sensor 对应
 * @param chn       [in]    视频流通道
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_osd_init(uint8_t idx, vp_video_chn_t chn);

/**
 * 获取OSD信息
 *
 * @param idx       [in]    视频编号,与 Sensor 对应
 * @param chn       [in]    视频流通道
 * @param index     [in]    OSD编号:0~13 0~3为文本.4~13为矩形框
 * @param item      [out]   OSD信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_get_item(uint8_t idx, vp_video_chn_t chn, uint8_t index, vp_osd_item_t **item);

/**
 * 配置OSD文字
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~3.其中0为时间戳.1~3为自定义
 * @param config    [in]     配置信息
 * @param str       [in]     文字内容.对于0时间编号,该值为时间格式字符串:e.g %Y-%m-%d %H:%M:%S
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_config_text(uint8_t idx, vp_video_chn_t chn, uint8_t index,
                             vp_osd_config_text_t *config, const char *str);

/**
 * 配置OSD矩形框
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~9
 * @param config    [in]     配置信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_config_rect(uint8_t idx, vp_video_chn_t chn, uint8_t index, vp_osd_config_rect_t *config);

/**
 * 开始视频OSD水印
 *
 * @param idx       [in]    视频编号,与 Sensor 对应
 * @param chn       [in]    视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_osd_start(uint8_t idx, vp_video_chn_t chn);

/**
 * 暂停视频OSD水印
 *
 * @param idx       [in]    视频编号,与 Sensor 对应
 * @param chn       [in]    视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_pause(uint8_t idx, vp_video_chn_t chn);

/**
 * 恢复视频OSD水印
 *
 * @param idx       [in]    视频编号,与 Sensor 对应
 * @param chn       [in]    视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_resume(uint8_t idx, vp_video_chn_t chn);

/**
 * 显示OSD时间戳
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~13 0~3为文本.4~13为矩形框
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_show(uint8_t idx, vp_video_chn_t chn, uint8_t index);

/**
 * 显示OSD文本
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~3
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_show_text(uint8_t idx, vp_video_chn_t chn, uint8_t index);

/**
 * 设置OSD模式
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~3
 * @param mode     [in]      OSD模式 0: 12h  1: 24h
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_mode(uint8_t idx, vp_video_chn_t chn, uint8_t index, uint8_t mode);


/**
 * 显示OSD矩形框
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~9
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_show_rect(uint8_t idx, vp_video_chn_t chn, uint8_t index);

/**
 * 显示OSD马赛克
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~9
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_show_mosaic(uint8_t idx, vp_video_chn_t chn, uint8_t index);

/**
 * 更新OSD时间戳
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param timestamp [in]     OSD显示时间戳
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_update_timestamp(uint8_t idx, vp_video_chn_t chn, uint64_t timestamp);

/**
 * 更新OSD时间戳
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param timestamp [in]     OSD显示时间戳
 * @param timeout   [in]     更新等待超时时间(ms)
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_update_timestamp_wait(uint8_t idx, vp_video_chn_t chn, uint64_t timestamp, uint32_t timeout);

/**
 * 更新OSD文本
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~3.其中0为时间戳.1~3为自定义
 * @param str       [in]     文字内容.对于0时间编号,该值为时间格式字符串:e.g %Y-%m-%d %H:%M:%S
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_update_text(uint8_t idx, vp_video_chn_t chn, uint8_t index, const char *str);

/**
 * 更新OSD矩形框
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~9
 * @param x         [in]     x坐标
 * @param y         [in]     y坐标
 * @param width     [in]     宽度
 * @param height    [in]     高度
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_update_rect(uint8_t idx, vp_video_chn_t chn, uint8_t index,
                             uint32_t x, uint32_t y, uint32_t width, uint32_t height);

/**
 * 更新马赛克矩形框
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~9
 * @param x         [in]     x坐标
 * @param y         [in]     y坐标
 * @param width     [in]     宽度
 * @param height    [in]     高度
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_update_mosaic(uint8_t idx, vp_video_chn_t chn, uint8_t index,
                             uint32_t x, uint32_t y, uint32_t width, uint32_t height);

/**
 * 隐藏OSD
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~13 0~3为文本.4~13为矩形框
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_hide(uint8_t idx, vp_video_chn_t chn, uint8_t index);

/**
 * 隐藏OSD文本
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~3
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_hide_text(uint8_t idx, vp_video_chn_t chn, uint8_t index);

/**
 * 配置马赛克
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~9
 * @param config    [in]     配置信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_config_mosaic(uint8_t idx, vp_video_chn_t chn, uint8_t index, vp_osd_config_rect_t *config);

/**
 * 隐藏OSD矩形框
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~9
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_hide_rect(uint8_t idx, vp_video_chn_t chn, uint8_t index);

/**
 * 隐藏马赛克
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~9
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_hide_mosaic(uint8_t idx, vp_video_chn_t chn, uint8_t index);

/**
 * 隐藏所有OSD
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
void vp_video_osd_hide_all(uint8_t idx, vp_video_chn_t chn);

/**
 * 视频OSD叠加
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号
 * @param y         [in]     Y分量数据
 * @param uv        [in]     UV分量数据
 * @param width     [in]     视频宽度
 * @param height    [in]     视频高度
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_overlay(uint8_t idx, vp_video_chn_t chn, uint8_t index, uint8_t *y, uint8_t *uv,
                         uint32_t width, uint32_t height);

/**
 * 停止视频OSD水印
 *
 * @param idx       [in]    视频编号,与 Sensor 对应
 * @param chn       [in]    视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_osd_stop(uint8_t idx, vp_video_chn_t chn);

/**
 * 反初始化视频OSD水印
 *
 * @param idx       [in]    视频编号,与 Sensor 对应
 * @param chn       [in]    视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_osd_deinit(uint8_t idx, vp_video_chn_t chn);

#endif //VP_VIDEO_CODER_VP_VIDEO_OSD_H
