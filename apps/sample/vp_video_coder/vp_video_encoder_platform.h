//
// Created by 杨柳 on 2023/3/4.
//

#ifndef VP_VIDEO_ENCODER_PLATFORM_H
#define VP_VIDEO_ENCODER_PLATFORM_H


#include <stdlib.h>
#include <string.h>
#include "vp_video_coder_config.h"
//#include "vp_sensor_config.h"
#include "vp_video_coder_type.h"
#include "vp_events.h"
#include "vp_context_config.h"
#include "ts_comm_vpss.h"
#include "ts_comm_venc.h"

#define VP_VIDEO_CHANNEL_MAX 3
#define VP_VIDEO_OSD_MAX 16
#define VP_VIDEO_OSD_MOSAIC_MAX 2

typedef struct {
    uint8_t init_flag: 2;
    uint8_t start_flag: 2;
    uint8_t pause_flag: 2;
    uint8_t temp_flag: 2;
    uint8_t isp_chn;
    int     dev;
    int     chn;
    vp_events_p events;
    vp_osd_item_t items[VP_VIDEO_OSD_MAX];//0 ~ 3,文本水印.4 ~ 13,矩形水印
} vp_channel_osd_config_t;

typedef struct {
    uint8_t enable;
    vp_video_chn_t type;
    vp_events_p events;
    uint64_t timestamp;
    vp_video_encoder_format_t format;
    uint32_t frame_size;
    uint16_t bitrate;
    uint8_t fps;
    uint8_t enable_jpeg: 2;
    uint8_t enable_aov: 2;
    uint8_t enable_tmp: 4;
    uint8_t chn;
    uint8_t vpss_grp;
    uint8_t vpss_chn;
    uint8_t enc_chn;
    uint8_t chn_jpeg;
    uint8_t group_jpeg;
    uint8_t start_flag: 1;
    uint8_t pause_flag: 1;
    uint8_t frame_source_flag: 1;
    uint8_t encoder_flag: 1;
    uint8_t jpeg_en_flag: 1;
    uint8_t jpeg_st_flag: 1;
    uint8_t aov_idr: 1;
    uint8_t error_flag: 1;
    //IMPPayloadType payloadType;
    PAYLOAD_TYPE_E payloadType;
    //VPSS_CHN_ATTR_S fs_chn_attr;
    VPSS_CHN_ATTR_S fs_chn_attr;
    //IMPCell source_cell;
    //IMPCell encoder_cell;
    //IMPEncoderStream stream;
    VENC_STREAM_S stream;
    uint8_t *jpeg_data;
    uint32_t *jpeg_size;
    uint8_t jpeg_snap_ok;
    vp_channel_osd_config_t osd_config;
    VIDEO_FRAME_INFO_S frame;
    int8_t frame_flag;
    pthread_t frame_thread;
    char* algo_frame;
} vp_channel_config_t;


typedef struct {
    vp_channel_config_t channels[VP_VIDEO_CHANNEL_MAX];
} vp_video_enc_config_f;

#endif //VP_VIDEO_ENCODER_PLATFORM_H
