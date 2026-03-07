//
// Created by 杨柳 on 2024/11/20.
//

#ifndef VP_VIDEO_CODER_VP_VIDEO_DEF_H
#define VP_VIDEO_CODER_VP_VIDEO_DEF_H

#define MAIN(idx, def)  VP_VIDEO_##idx##_MAIN_##def
#define SEC(idx, def)  VP_VIDEO_##idx##_SEC_##def
#define THR(idx, def)  VP_VIDEO_##idx##_THR_##def
#define SENSOR_WIDTH(idx) VP_SENSOR_##idx##_WIDTH
#define SENSOR_HEIGHT(idx) VP_SENSOR_##idx##_HEIGHT
#define SCALER_ENABLE(width, height, sensor_width, sensor_height) ((width) != (sensor_width) || (height) != (sensor_height))
#define PAYLOAD_TYPE(format) (format == VP_VIDEO_ENCODER_FORMAT_H265 ? PT_H265 : (format == VP_VIDEO_ENCODER_FORMAT_H264 ? PT_H264 : PT_JPEG))

#endif //VP_VIDEO_CODER_VP_VIDEO_DEF_H
