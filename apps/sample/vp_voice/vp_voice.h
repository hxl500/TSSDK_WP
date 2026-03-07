#ifndef VP_VOICE_H
#define VP_VOICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * 启用音频输入
 *
 * @param sample_rate   [in]   音频采样频率
 * @param bits          [in]   音频采样位宽
 * @param channels      [in]   音频采样通道数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_enable(uint32_t sample_rate, uint8_t bits, uint8_t channels);

/**
 * 设置输入音量
 *
 * @param vol   [in]    音量百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_set_vol(float vol);

/**
 * 获取输入音量
 *
 * @param vol   [out]    音量百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_get_vol(float *vol);

/**
 * 设置输入模拟增益
 *
 * @param gain   [in]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_set_gain(float gain);

/**
 * 获取输入模拟增益
 *
 * @param gain   [out]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_get_gain(float *gain);

/**
 * 设置输入数字增益
 *
 * @param gain   [in]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_set_digital_gain(float gain);

/**
 * 获取输入数字增益
 *
 * @param gain   [out]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_get_digital_gain(float *gain);

/**
 * 设置输入静音
 *
 * @param mute   [in]    1 启用,0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_set_mute(uint8_t mute);

/**
 * 获取输入静音状态
 *
 * @param mute   [in]    静音状态.1 启用,0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_get_mute(uint8_t *mute);


typedef struct {
    /* 帧序号 */
    uint32_t frame_no;

    /* 帧时间戳 单位微秒(us)*/
    uint64_t timestamp;

    /* 帧数据长度 */
    uint32_t frame_size;

    /* 帧数据源 */
    uint8_t *frame_data;

    /* 私有流信息,注意不是指向的帧数据 */
    void *priv_data;
} vp_voice_frame_t;

/**
 * 获取输入音频帧大小
 *
 * @return 错误码或音频帧数据大小
 *
 * @retval \>0 成功
 * @retval \<0 失败
 */
int vp_voice_input_frame_size();

/**
 * 获取输入音频帧时长
 *
 * @return 错误码或音频帧数据大小
 *
 * @retval \>0 成功
 * @retval \<0 失败
 */
uint32_t vp_voice_input_frame_duration();

/**
 * 暂停音频输入
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_pause();

/**
 * 恢复音频输入
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_resume();

/**
 * 请求输入音频帧
 *
 * @note 请求的音频帧需要调用`vp_voice_release_frame`进行释放
 *
 * @param frame     [out]   输出音频帧信息
 * @param timeout   [in]    请求超时时间,单位毫秒(ms)
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_request_frame(vp_voice_frame_t *frame, uint32_t timeout);

/**
 * 释放音频帧数据
 *
 * @param frame     [in]   音频帧信息
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_release_frame(vp_voice_frame_t *frame);

/**
 * 禁用音频输入
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_disable();


/**
 * 启用音频输出
 *
 * @param sample_rate   [in]   音频采样频率
 * @param bits          [in]   音频采样位宽
 * @param channels      [in]   音频采样通道数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_enable(uint32_t sample_rate, uint8_t bits, uint8_t channels);

/**
 * 获取输出音频帧大小
 *
 * @return 错误码或音频帧数据大小
 *
 * @retval \>0 成功
 * @retval \<0 失败
 */
int vp_voice_output_frame_size();

/**
 * 设置输出音量
 *
 * @param vol   [in]    音量百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_set_vol(float vol);

/**
 * 获取输出音量
 *
 * @param vol   [out]    音量百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_get_vol(float *vol);

/**
 * 设置输出模拟增益
 *
 * @param gAOn   [in]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_set_gain(float gain);

/**
 * 获取输出模拟增益
 *
 * @param gain   [out]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_get_gain(float *gain);

/**
 * 设置输出数字增益
 *
 * @param gain   [in]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_set_digital_gain(float gain);

/**
 * 获取输出数字增益
 *
 * @param gain   [out]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_get_digital_gain(float *gain);

/**
 * 设置输出静音
 *
 * @param mute   [in]    1 启用,0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_set_mute(uint8_t mute);

/**
 * 获取输出静音状态
 *
 * @param mute   [in]    静音状态.1 启用,0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_get_mute(uint8_t *mute);

/**
 * 暂停输出音频
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_pause();

/**
 * 恢复输出音频
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_resume();

/**
 * 发送输出音频数据
 *
 * @param data     [in]   音频数据
 * @param len      [in]   音频数据长度
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_send_frame(uint8_t *data, uint32_t len);

/**
 * 刷新音频输出缓存
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_send_flush();

/**
 * 禁用音频输出
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_disable();

/**
 * 设置声音侦测灵敏度
 *
 * @param sensitivity 声音侦测灵敏度值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_set_audio_detect_sensitivity(int sensitivity);

/**
 * 设置声音侦测使能
 *
 * @param enable 声音侦测使能
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_set_audio_detect_enable(int enable);

/**
 * 加载音频驱动
 */
void vp_voice_insert_driver();

/**
 * 卸载音频驱动
 */
void vp_voice_rm_driver();


/**
 * 启用回声消除
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_aec_enable();

/**
 * 禁用回声消除
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_aec_disable();

#ifdef __cplusplus
}
#endif

#endif
    