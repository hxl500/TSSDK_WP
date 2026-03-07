/* DO NOT EDIT */
#ifndef vp_video_coder_config_h
#define vp_video_coder_config_h

#define VP_VIDEO_CODER_VERSION_MAJOR (1)
#define VP_VIDEO_CODER_VERSION_MINOR (0)
#define VP_VIDEO_CODER_VERSION_PATCH (0)
#define VP_VIDEO_CODER_VERSION_TWEAK (17)
#define VP_VIDEO_CODER_VERSION "1.0.0.17"

#pragma region "编码格式选项"

// 无编码
#define VP_VIDEO_ENCODER_FORMAT_NONE 0x00

// H264编码
#define VP_VIDEO_ENCODER_FORMAT_H264 0x01

// H265编码
#define VP_VIDEO_ENCODER_FORMAT_H265 0x02

// Jpeg编码
#define VP_VIDEO_ENCODER_FORMAT_JPEG 0x03

#pragma endregion


/**
 * 是否支持AOV模式
 */
#define VP_VIDEO_AOV_SUPPORT 0x01

/**
 * 配置AOV支持的视频间隔:以英文逗号为间隔
 * 字符长度:32
 */
#define VP_VIDEO_AOV_INTERVAL "1,5,10,15,30,45,60"

#pragma region "传感器1视频编码"

/**
 * 视频主码流编码启用标识
 */
#define VP_VIDEO_1_MAIN_ENABLE 0x01

/**
 * 视频主码流码编码帧率
 * 取值范围:[0,60]
 */
#define VP_VIDEO_1_MAIN_FPS 25

/**
 * 视频主码流编码格式
 */
#define VP_VIDEO_1_MAIN_FORMAT VP_VIDEO_ENCODER_FORMAT_H265

/**
 * 视频主码流码率单位Kb
 * 取值范围:[0,2000]
 */
#define VP_VIDEO_1_MAIN_BITRATE 1024

/**
 * 视频主码流编码分辨率宽度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_1_MAIN_WIDTH 2560

/**
 * 视频主码流编码分辨率高度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_1_MAIN_HEIGHT 1440

/**
 * 视频主码流是否启用JPEG编码
 */
#define VP_VIDEO_1_MAIN_JPEG 0x00

/**
 * 视频主码流是否启用AOV功能
 */
#define VP_VIDEO_1_MAIN_AOV 0x01

/**
 * 视频子码流编码启用标识
 */
#define VP_VIDEO_1_SEC_ENABLE 0x01

/**
 * 视频子码流码编码帧率
 * 取值范围:[0,60]
 */
#define VP_VIDEO_1_SEC_FPS 25

/**
 * 视频子码流编码格式
 */
#define VP_VIDEO_1_SEC_FORMAT VP_VIDEO_ENCODER_FORMAT_H265

/**
 * 视频子码流码率单位Kb
 * 取值范围:[0,2000]
 */
#define VP_VIDEO_1_SEC_BITRATE 512

/**
 * 视频子码流编码分辨率宽度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_1_SEC_WIDTH 640

/**
 * 视频子码流编码分辨率高度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_1_SEC_HEIGHT 360

/**
 * 视频子码流是否启用JPEG编码
 */
#define VP_VIDEO_1_SEC_JPEG 0x00

/**
 * 视频子码流是否启用AOV功能
 */
#define VP_VIDEO_1_SEC_AOV 0x01

/**
 * 视频次码流编码启用标识
 */
#define VP_VIDEO_1_THR_ENABLE 0x00

/**
 * 视频次码流码编码帧率
 * 取值范围:[0,60]
 */
#define VP_VIDEO_1_THR_FPS 25

/**
 * 视频次码流编码格式
 */
#define VP_VIDEO_1_THR_FORMAT VP_VIDEO_ENCODER_FORMAT_NONE

/**
 * 视频次码流码率单位Kb
 * 取值范围:[0,2000]
 */
#define VP_VIDEO_1_THR_BITRATE 512

/**
 * 视频次码流编码分辨率宽度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_1_THR_WIDTH 640

/**
 * 视频次码流编码分辨率高度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_1_THR_HEIGHT 384

/**
 * 视频次码流是否启用JPEG编码
 */
#define VP_VIDEO_1_THR_JPEG 0x00





#pragma region "传感器1视频编码"

/**
 * 视频主码流编码启用标识
 */
#define VP_VIDEO_2_MAIN_ENABLE 0x01

/**
 * 视频主码流码编码帧率
 * 取值范围:[0,60]
 */
#define VP_VIDEO_2_MAIN_FPS 25

/**
 * 视频主码流编码格式
 */
#define VP_VIDEO_2_MAIN_FORMAT VP_VIDEO_ENCODER_FORMAT_H265

/**
 * 视频主码流码率单位Kb
 * 取值范围:[0,2000]
 */
#define VP_VIDEO_2_MAIN_BITRATE 1024

/**
 * 视频主码流编码分辨率宽度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_2_MAIN_WIDTH 2560

/**
 * 视频主码流编码分辨率高度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_2_MAIN_HEIGHT 1440

/**
 * 视频主码流是否启用JPEG编码
 */
#define VP_VIDEO_2_MAIN_JPEG 0x00

/**
 * 视频主码流是否启用AOV功能
 */
#define VP_VIDEO_2_MAIN_AOV 0x01

/**
 * 视频子码流编码启用标识
 */
#define VP_VIDEO_2_SEC_ENABLE 0x01

/**
 * 视频子码流码编码帧率
 * 取值范围:[0,60]
 */
#define VP_VIDEO_2_SEC_FPS 25

/**
 * 视频子码流编码格式
 */
#define VP_VIDEO_2_SEC_FORMAT VP_VIDEO_ENCODER_FORMAT_H265

/**
 * 视频子码流码率单位Kb
 * 取值范围:[0,2000]
 */
#define VP_VIDEO_2_SEC_BITRATE 512

/**
 * 视频子码流编码分辨率宽度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_2_SEC_WIDTH 640

/**
 * 视频子码流编码分辨率高度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_2_SEC_HEIGHT 360

/**
 * 视频子码流是否启用JPEG编码
 */
#define VP_VIDEO_2_SEC_JPEG 0x00

/**
 * 视频子码流是否启用AOV功能
 */
#define VP_VIDEO_2_SEC_AOV 0x01

/**
 * 视频次码流编码启用标识
 */
#define VP_VIDEO_2_THR_ENABLE 0x00

/**
 * 视频次码流码编码帧率
 * 取值范围:[0,60]
 */
#define VP_VIDEO_2_THR_FPS 25

/**
 * 视频次码流编码格式
 */
#define VP_VIDEO_2_THR_FORMAT VP_VIDEO_ENCODER_FORMAT_NONE

/**
 * 视频次码流码率单位Kb
 * 取值范围:[0,2000]
 */
#define VP_VIDEO_2_THR_BITRATE 512

/**
 * 视频次码流编码分辨率宽度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_2_THR_WIDTH 640

/**
 * 视频次码流编码分辨率高度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_2_THR_HEIGHT 384

/**
 * 视频次码流是否启用JPEG编码
 */
#define VP_VIDEO_2_THR_JPEG 0x00











#pragma region "传感器1视频编码"

/**
 * 视频主码流编码启用标识
 */
#define VP_VIDEO_3_MAIN_ENABLE 0x01

/**
 * 视频主码流码编码帧率
 * 取值范围:[0,60]
 */
#define VP_VIDEO_3_MAIN_FPS 25

/**
 * 视频主码流编码格式
 */
#define VP_VIDEO_3_MAIN_FORMAT VP_VIDEO_ENCODER_FORMAT_H265

/**
 * 视频主码流码率单位Kb
 * 取值范围:[0,2000]
 */
#define VP_VIDEO_3_MAIN_BITRATE 1024

/**
 * 视频主码流编码分辨率宽度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_3_MAIN_WIDTH 2560

/**
 * 视频主码流编码分辨率高度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_3_MAIN_HEIGHT 1440

/**
 * 视频主码流是否启用JPEG编码
 */
#define VP_VIDEO_3_MAIN_JPEG 0x00

/**
 * 视频主码流是否启用AOV功能
 */
#define VP_VIDEO_3_MAIN_AOV 0x01

/**
 * 视频子码流编码启用标识
 */
#define VP_VIDEO_3_SEC_ENABLE 0x01

/**
 * 视频子码流码编码帧率
 * 取值范围:[0,60]
 */
#define VP_VIDEO_3_SEC_FPS 25

/**
 * 视频子码流编码格式
 */
#define VP_VIDEO_3_SEC_FORMAT VP_VIDEO_ENCODER_FORMAT_H265

/**
 * 视频子码流码率单位Kb
 * 取值范围:[0,2000]
 */
#define VP_VIDEO_3_SEC_BITRATE 512

/**
 * 视频子码流编码分辨率宽度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_3_SEC_WIDTH 640

/**
 * 视频子码流编码分辨率高度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_3_SEC_HEIGHT 360

/**
 * 视频子码流是否启用JPEG编码
 */
#define VP_VIDEO_3_SEC_JPEG 0x00

/**
 * 视频子码流是否启用AOV功能
 */
#define VP_VIDEO_3_SEC_AOV 0x01

/**
 * 视频次码流编码启用标识
 */
#define VP_VIDEO_3_THR_ENABLE 0x00

/**
 * 视频次码流码编码帧率
 * 取值范围:[0,60]
 */
#define VP_VIDEO_3_THR_FPS 25

/**
 * 视频次码流编码格式
 */
#define VP_VIDEO_3_THR_FORMAT VP_VIDEO_ENCODER_FORMAT_NONE

/**
 * 视频次码流码率单位Kb
 * 取值范围:[0,2000]
 */
#define VP_VIDEO_3_THR_BITRATE 512

/**
 * 视频次码流编码分辨率宽度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_3_THR_WIDTH 640

/**
 * 视频次码流编码分辨率高度
 * 取值范围:[0,9999]
 */
#define VP_VIDEO_3_THR_HEIGHT 384

/**
 * 视频次码流是否启用JPEG编码
 */
#define VP_VIDEO_3_THR_JPEG 0x00







/**
 * 视频次码流是否启用AOV功能
 */
#define VP_VIDEO_1_THR_AOV 0x00

#pragma endregion


#endif
