//
// Created by 杨柳 on 2022/12/8.
//
#include <stddef.h>
#include "vp_printf.h"
#include "vp_time.h"
#include "vp_video_encoder.h"
#include "vp_video_encoder_platform.h"
#include "vp_video_def.h"
#include "vp_context.h"
#include "vp_context_config.h"
#include "vp_video_coder_type.h"
#include "vp_video_coder_config.h"
#include "vp_events.h"

#include "sample_comm.h"
#include "soft_line.h"
#include "soft_alphabet_overlay.h"

#include <errno.h>

#ifdef PAYLOAD_TYPE
#undef PAYLOAD_TYPE
#endif

#define VENC_RC_VBR
#define FRAME_RATE  (20)
#define PAYLOAD_TYPE(format) (format == VP_VIDEO_ENCODER_FORMAT_H265 ? PT_H265 : (format == VP_VIDEO_ENCODER_FORMAT_H264 ? PT_H264 : PT_JPEG))


#define MAIN_CHANNEL(num, vpss_group, vpss_channel, enc_channel, jpeg_enc_chn)  .enable = MAIN(num, ENABLE),                \
            .events = NULL,                                                                                                 \
            .timestamp = 0,                                                                                                 \
            .type = vp_video_chn_main,                                                                                      \
            .format = MAIN(num, FORMAT),                                                                                    \
            .fps = FRAME_RATE/*MAIN(num, FPS)*/,                                                                                          \
            .bitrate = MAIN(num, BITRATE),                                                                                  \
            .enable_jpeg = MAIN(num, JPEG),                                                                                 \
            .enable_aov = MAIN(num, AOV),                                                                                   \
            .vpss_grp = vpss_group, .vpss_chn = vpss_channel, .enc_chn = enc_channel,.chn_jpeg = jpeg_enc_chn,              \
            .start_flag = 0, .pause_flag = 0, .frame_source_flag = 0,.encoder_flag = 0,                                     \
            .jpeg_en_flag = 0, .jpeg_st_flag = 0, .aov_idr = 0,.error_flag = 0,                                             \
            .payloadType = PAYLOAD_TYPE(MAIN(num, FORMAT)),                                                                 \
            .fs_chn_attr = {                                                                                                \
                .u32MaxW = 1920/*MAIN(num, WIDTH)*/, .u32MaxH = 1080/*MAIN(num, HEIGHT)*/,                                                  \
                .u32Width = 1920/*MAIN(num, WIDTH)*/, .u32Height = 1080/*MAIN(num, HEIGHT)*/,                                               \
                .u32Depth = 10, .u32MemCount = 1, .bMirror = 0, .bFlip = 0,                                                  \
                .enChnMode = VPSS_CHN_MODE_USER, .enCompressMode = COMPRESS_MODE_NONE,                                      \
                .enDynamicRange = DYNAMIC_RANGE_SDR8, .enPixelFormat = PIXEL_FORMAT_NV_12,                                  \
                .enVideoFormat = VIDEO_FORMAT_LINEAR,                                                                       \
                .stFrameRate = {                                                                                            \
                    .s32SrcFrameRate = -1, .s32DstFrameRate = -1,                                                           \
                }                                                                                                           \
            },                                                                                                              \
            .osd_config = {                                                                                                 \
                .init_flag = 0, .start_flag = 0, .pause_flag = 0, .isp_chn = 0, .items = {0},                    \
            }

#define SEC_CHANNEL(num, vpss_group, vpss_channel, enc_channel, jpeg_enc_chn)   .enable = SEC(num, ENABLE),                 \
            .events = NULL,                                                                                                 \
            .timestamp = 0,                                                                                                 \
            .type = vp_video_chn_sec,                                                                                       \
            .format = SEC(num, FORMAT),                                                                                     \
            .fps = FRAME_RATE/*SEC(num, FPS)*/,                                                                                           \
            .bitrate = SEC(num, BITRATE),                                                                                   \
            .enable_jpeg = SEC(num, JPEG),                                                                                  \
            .enable_aov = SEC(num, AOV),                                                                                    \
            .vpss_grp = vpss_group, .vpss_chn = vpss_channel, .enc_chn = enc_channel, .chn_jpeg = jpeg_enc_chn,             \
            .start_flag = 0, .pause_flag = 0, .frame_source_flag = 0,.encoder_flag = 0,                                     \
            .jpeg_en_flag = 0, .jpeg_st_flag = 0, .aov_idr = 0,.error_flag = 0,                                             \
            .payloadType = PAYLOAD_TYPE(SEC(num, FORMAT)),                                                                  \
            .fs_chn_attr = {                                                                                                \
                .u32MaxW = 640/*SEC(num, WIDTH)*/, .u32MaxH = 360/*SEC(num, HEIGHT)*/,                                                    \
                .u32Width = 640/*SEC(num, WIDTH)*/, .u32Height = 360/*SEC(num, HEIGHT)*/,                                                 \
                .u32Depth = 10, .u32MemCount = 3, .bMirror = 0, .bFlip = 0,                                                  \
                .enChnMode = VPSS_CHN_MODE_USER, .enCompressMode = COMPRESS_MODE_NONE,                                      \
                .enDynamicRange = DYNAMIC_RANGE_SDR8, .enPixelFormat = PIXEL_FORMAT_NV_12,                                  \
                .enVideoFormat = VIDEO_FORMAT_LINEAR,                                                                       \
                .stFrameRate = {                                                                                            \
                    .s32SrcFrameRate = -1, .s32DstFrameRate = -1,                                                           \
                }                                                                                                           \
            },                                                                                                              \
            .osd_config = {                                                                                                 \
                .init_flag = 0, .start_flag = 0, .pause_flag = 0, .items = {0},                                             \
            }

#define THR_CHANNEL(num, vpss_group, vpss_channel, enc_channel, jpeg_enc_chn)   .enable = THR(num, ENABLE),                 \
            .events = NULL,                                                                                                 \
            .timestamp = 0,                                                                                                 \
            .format = THR(num, FORMAT),                                                                                     \
            .fps = THR(num, FPS),                                                                                           \
            .bitrate = THR(num, BITRATE),                                                                                   \
            .enable_jpeg = THR(num, JPEG),                                                                                  \
            .enable_aov = THR(num, AOV),                                                                                    \
            .vpss_grp = vpss_group, .vpss_chn = vpss_channel, .enc_chn = enc_channel, .chn_jpeg = jpeg_enc_chn,             \
            .start_flag = 0, .pause_flag = 0, .frame_source_flag = 0,.encoder_flag = 0,                                     \
            .jpeg_en_flag = 0, .jpeg_st_flag = 0, .aov_idr = 0,.error_flag = 0,                                             \
            .payloadType = PAYLOAD_TYPE(THR(num, FORMAT)),                                                                  \
            .fs_chn_attr = {                                                                                                \
                .u32MaxW = THR(num, WIDTH), .u32MaxH = THR(num, HEIGHT),                                                    \
                .u32Width = THR(num, WIDTH), .u32Height = THR(num, HEIGHT),                                                 \
                .u32Depth = 3, .u32MemCount = 1, .bMirror = 0, .bFlip = 0,                                                  \
                .enChnMode = VPSS_CHN_MODE_USER, .enCompressMode = COMPRESS_MODE_NONE,                                      \
                .enDynamicRange = DYNAMIC_RANGE_SDR8, .enPixelFormat = PIXEL_FORMAT_NV_12,                                  \
                .enVideoFormat = VIDEO_FORMAT_LINEAR,                                                                       \
                .stFrameRate = {                                                                                            \
                    .s32SrcFrameRate = 10, .s32DstFrameRate = 10,                                                           \
                }                                                                                                           \
            },                                                                                                              \
            .osd_config = {                                                                                                 \
                .init_flag = 0, .start_flag = 0, .pause_flag = 0, .items = {0},                                             \
            }

#if defined(VENC_RC_VBR)
/*encoder QP*/
#define MAIN_RC_MIN_QP_I	(30)
#define MAIN_RC_MAX_QP_I	(35)
#define MAIN_RC_MIN_QP_P	(30)
#define MAIN_RC_MAX_QP_P	(45)
#define SUB_RC_MIN_QP_I		(30)
#define SUB_RC_MAX_QP_I		(35)
#define SUB_RC_MIN_QP_P		(30)
#define SUB_RC_MAX_QP_P		(45)
#else
#define MAIN_RC_MIN_QP_I    (30)
#define MAIN_RC_MAX_QP_I    (51)
#define MAIN_RC_MIN_QP_P    (30)
#define MAIN_RC_MAX_QP_P    (51)
#define SUB_RC_MIN_QP_I     (30)
#define SUB_RC_MAX_QP_I     (51)
#define SUB_RC_MIN_QP_P     (30)
#define SUB_RC_MAX_QP_P     (51)
#endif

SAMPLE_VI_CONFIG_S stViConfig;
static vp_video_encoder_specific_t specifics[VP_SENSOR_NUM][VP_VIDEO_CHANNEL_MAX] = {0};
static vp_video_enc_config_f configs[VP_SENSOR_NUM] = {
#if VP_SENSOR_NUM == 1
/*
 *====================<单摄码流绑定配置>=========================
 * Sensor0:                                                   |
 *                                                            |
 * 主码流----fs_chn0----enc_chn0----enc_group0---isp_osd_chn1  |
 *                                                            |
 * 子码流----fs_chn1----enc_chn1----enc_group1---IPU_OSD       |
 *                                   |                        |
 * Jpeg-----fs_chn2----enc_chn2------|                        |
 *                                                            |
 *=============================================================
 *
 * */
        {
            .channels = {
                {MAIN_CHANNEL(1,0,0,0,0)},
                {SEC_CHANNEL(1,0,1,1,2)},
            }
        },
#elif VP_SENSOR_NUM == 2
/*
 *====================<双摄码流绑定配置>=========================
 * Sensor0 :                                                  |
 *                                                            |
 * 主码流----fs_chn0----enc_chn0----enc_group0---isp_osd_chn1  |
 *                                                            |
 * 子码流----fs_chn1----enc_chn4----enc_group2---IPU_OSD       |
 *                                   |                        |
 * Jpeg-----fs_chnXX---enc_chn2------|                        |
 *                                                            |
 *=============================================================
 * Sensor1 :                                                  |
 *                                                            |
 * 主码流----fs_chn3----enc_chn1----enc_group1---isp_osd_chn2  |
 *                                                            |
 * 子码流----fs_chn4----enc_chn5----enc_group3---IPU_OSD       |
 *                                   |                        |
 * Jpeg-----fs_chnXX---enc_chn3------|                        |
 *                                                            |
 *=============================================================
 *
 * */
        {
                .channels = {
                    {MAIN_CHANNEL(1,0,0,0,0)},
                    {SEC_CHANNEL(1,0,1,1,4)},
                }
        },
        {
                .channels = {
                    {MAIN_CHANNEL(2,1,0,2,0)},
                    {SEC_CHANNEL(2,1,1,3,4)},
                }
        },

#elif VP_SENSOR_NUM == 3
/*
 * ===================<三摄码流绑定配置>==========================
 * Sensor0 :                                                  |
 *                                                            |
 * 主码流----vpss_grp0_chn0----enc_chn0|
 *                                                            |
 * 子码流----vpss_grp_0_chn1----enc_chn1----enc_group3---IPU_OSD       |
 *                                   |                        |
 * Jpeg-----vpss_chn2----enc_chn6------|                        |
 *                                                            |
 *=============================================================
 * Sensor1 :                                                  |
 *                                                            |
 * 主码流----fs_chn3----enc_chn1----enc_group1---isp_osd_chn2  |
 *                                                            |
 * 子码流----fs_chn4----enc_chn3----enc_group4---IPU_OSD       |
 *                                   |                        |
 * Jpeg-----fs_chn5----enc_chn7------|                        |
 *                                                            |
 * ============================================================
 * Sensor2 :                                                  |
 *                                                            |
 * 主码流----fs_chn6----enc_chn4----enc_group2---isp_osd_chn3  |
 *                                                            |
 * 子码流----fs_chn7----enc_chn5----enc_group5---IPU_OSD       |
 *                                   |                        |
 * Jpeg-----fs_chn8----enc_chn8------|                        |
 *                                                            |
 * ============================================================
 * */
        {
            .channels = {
                    {MAIN_CHANNEL(1,0,0,0,0)},
                    {SEC_CHANNEL(1,0,1,1,6)},
            }
        },
        {
            .channels = {
                    {MAIN_CHANNEL(2,1,0,2,0)},
                    {SEC_CHANNEL(2,1,1,3,6)},
            }
        },
        {
            .channels = {
                    {MAIN_CHANNEL(3,2,0,4,0)},
                    {SEC_CHANNEL(3,2,1,5,6)},
            }
        },
#endif
};


static void vp_video_config_print(uint8_t idx, vp_video_chn_t chn, vp_channel_config_t *config) {
    vp_debug("idx:%d chn:%d enable:%d format:%d fps:%d bitrate:%d enc_jpeg:%d",
             idx, chn, config->enable, config->format,
             config->fps, config->bitrate, config->enable_jpeg);
}

/**
 * 创建JPEG编码通道
 *
 * @param channel   [in]    通道信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
static int vp_video_jpeg_create(vp_channel_config_t *channel, int chnNum) {
    if (channel == NULL || !channel->enable || channel->frame_source_flag == 0) return -1;
    if (channel->jpeg_en_flag) return 0;

    VENC_GOP_MODE_E     enGopMode       = VENC_GOPMODE_NORMALP;
    SAMPLE_RC_E         enRcMode        = SAMPLE_RC_CBR;
    VENC_GOP_ATTR_S     stGopAttr       = { 0 };
    int enc_chnNum  = chnNum;
    int payload     = PT_JPEG;
    PIC_SIZE_E enSize = PIC_360P;//(channel->fs_chn_attr.u32Height == 1080) ? PIC_1080P :
                        //(channel->fs_chn_attr.u32Height == 360) ? PIC_360P : PIC_360P;
    int ret;

    enRcMode    = SAMPLE_RC_CBR;
    enGopMode   = VENC_GOPMODE_NORMALP;

    ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (ret) {
        vp_error("venc get gop attr error:0x%x", ret);
        return -1;
    }

    ret = SAMPLE_COMM_VENC_Creat(enc_chnNum, payload, enSize, enRcMode, 0, 1, &stGopAttr);
    if (ret) {
        vp_error("venc start chn_%d error:0x%x", enc_chnNum, ret);
        return -1;
    }

    channel->jpeg_en_flag = 1;
    return 0;
}

/**
 * 创建H264|H265数据编码通道
 *
 * @note step 2
 *
 * @param channel   [in]    通道信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
static int vp_video_encoder_create(vp_channel_config_t *channel) {
    if (channel == NULL || !channel->enable || channel->frame_source_flag == 0) return -1;
    if (channel->encoder_flag) return 0;

    VENC_GOP_MODE_E     enGopMode       = VENC_GOPMODE_NORMALP;
    SAMPLE_RC_E         enRcMode        = SAMPLE_RC_CBR;
    VENC_GOP_ATTR_S     stGopAttr       = { 0 };
    int enc_chnNum  = channel->enc_chn;
    int vpss_grpNum = channel->vpss_grp;
    int vpss_chnNum = channel->vpss_chn;
    int payload     = channel->payloadType;
    int u32FrameRate = channel->fps;
    int bit_rate = 2000;
    int max_bit_rate = 3000;

    PIC_SIZE_E enSize = (channel->fs_chn_attr.u32Height == 1080) ? PIC_1080P : 
                        (channel->fs_chn_attr.u32Height == 360) ? PIC_360P : PIC_360P;
    int ret;

    #if defined(VENC_RC_VBR)
    enRcMode = SAMPLE_RC_VBR;
    #else
    enRcMode = SAMPLE_RC_CBR;
    #endif
    enGopMode = VENC_GOPMODE_NORMALP;

    ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (ret) {
        vp_error("venc get gop attr error:0x%x", ret);
        return -1;
    }

    if (vpss_chnNum == 0) {
        max_bit_rate = 3000;
        bit_rate = 2000;
    }
    else if (vpss_chnNum == 1) {
        max_bit_rate = 1500;
        bit_rate = 1000;
    }

    SAMPLE_VENC_CHNL_ATTR_S venc_chn_attr = {
		.enType = payload,
		.enSize = enSize,
		.u32FrameRate = u32FrameRate,
		.enRcMode = enRcMode,
		.u32Profile = 0,
		.u32Bitrate = bit_rate,
        #if defined(VENC_RC_VBR)
		.u32Gop = u32FrameRate * 2,
        #else
		.u32Gop = u32FrameRate * 3,
        #endif
		.stGopAttr = stGopAttr,
        .u32Width = channel->fs_chn_attr.u32Width,
        .u32Height = channel->fs_chn_attr.u32Height,
        .u32AvgBitRate = bit_rate,
        .u32MaxBitrate = max_bit_rate
	};

    ret = SAMPLE_COMM_VENC_StartEx(enc_chnNum, &venc_chn_attr);
    if (ret) {
        vp_error("venc start chn_%d error:0x%x", enc_chnNum, ret);
        return -1;
    }

    ret = SAMPLE_COMM_CPM_Bind_VENC(enc_chnNum, 0, enc_chnNum);
    if (ret) {
        SAMPLE_COMM_VENC_Stop(enc_chnNum);
        vp_error("vpss grp_%d chn_%d bind venc chn_%d error:0x%x", vpss_grpNum, vpss_chnNum, enc_chnNum, ret);
        return -1;
    }

    channel->encoder_flag = 1;
    return 0;
}

/**
 * 从编码流读取数据
 *
 * @param stream    [in]    编码流信息
 * @param data      [out]   数据存储地址
 * @param size      [in]    数据存储长度
 *
 * @return 错误码或读取长度
 *
 * @retval < 0 错误码
 * @retval > 0 成功读取数据长度
 */
static int64_t vp_video_read_stream(VENC_STREAM_S *stream, uint8_t *data, uint32_t size) {
    uint32_t nr_pack = stream->u32PackCount, offset = 0;
    for (int i = 0; i < nr_pack; i++) {
        VENC_PACK_S *pack = &stream->pstPack[i];
        if (pack->u32Len) {
            if (size - offset < pack->u32Len) return -1;
            memcpy(data, (void *) (pack->pu8Addr), pack->u32Len);
            offset += pack->u32Len;
            data += pack->u32Len;
        }
    }
    return offset;
}

void *vp_video_jpeg_thread_func(void *args) {
    pthread_detach(pthread_self());

    VENC_STREAM_S stream = { 0 };
    VENC_CHN_STATUS_S stStat;
    int ret;
    int jpegFd;
    fd_set readFds;
    struct timeval timeoutVal;
    int64_t len;
    vp_channel_config_t *channel = (vp_channel_config_t *) args;
    vp_debug("jpeg func vpss_grp_%d vpss_chn_%d jpeg_chn_%d start", channel->vpss_grp, channel->vpss_chn, channel->chn_jpeg);

    char acFile[128] = { 0 };
    FILE *pFile;

    snprintf(acFile, 128, "snap_vgrp_%d_vchn_%d_jchn_%d.jpg", channel->vpss_grp, channel->vpss_chn, channel->chn_jpeg);
    pFile = fopen(acFile, "wb");
    if (pFile == NULL)
        vp_error("fopen fail.");

    while(1){

#if 0
        jpegFd = TS_MPI_VENC_GetFd(channel->chn_jpeg);
        if (jpegFd < 0) {
            usleep(10*1000);
            vp_error("TS_MPI_VENC_GetFd jpeg fd error:0x%x", jpegFd);
            continue;//return -1;
        }

        FD_ZERO(&readFds);
        FD_SET(jpegFd, &readFds);
        timeoutVal.tv_sec = 4;
        timeoutVal.tv_usec = 0;

        ret = TS_MPI_VENC_SELECT(jpegFd+1, &readFds, NULL, NULL, &timeoutVal);
        if (ret < 0) {
            vp_error("TS_MPI_VENC_SELECT error:0x%x", ret);
            return -1;
        }
#endif
        if(1/*FD_ISSET(jpegFd, &readFds)*/) {
#if 1
            ret = TS_MPI_VENC_QueryStatus(channel->chn_jpeg, &stStat);
            if (ret) {
                vp_error("venc query status error:0x%x", ret);
                return -1;
            }
            if (stStat.u32CurPacks == 0) {
                usleep(1*1000);
                continue;
            }
#endif

            stream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
            if (NULL == stream.pstPack) {
                vp_error("malloc stream pack failed!");
                return -1;
            }

            ret = TS_MPI_VENC_GetStream(channel->chn_jpeg, &stream, -1);
            if (ret)
            {
                free(stream.pstPack);
                stream.pstPack = NULL;
                vp_error("TS_MPI_VENC_GetStream chn:%d error", channel->chn_jpeg);
                return -1;
            }

            ret = SAMPLE_COMM_VENC_SaveStream(pFile, &stream);
            if (ret)
                vp_error("save jpeg error.");

            //len = vp_video_read_stream(&stream, channel->jpeg_data, *channel->jpeg_size);
            //*channel->jpeg_size = len;

            ret = TS_MPI_VENC_ReleaseStream(channel->chn_jpeg, &stream);
            if (ret) vp_error("IMP_Encoder_ReleaseStream chn:%d error", channel->chn_jpeg);
            if (stream.pstPack) {
                free(stream.pstPack);
                stream.pstPack = NULL;
            }
            channel->jpeg_snap_ok = 1;
            break;
        }
    }

    if (pFile) fclose(pFile);

    return NULL;
}


/**
 * 打开JPEG编码通道流
 *
 * @param channel   [in]    通道信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
static int vp_video_jpeg_start(vp_channel_config_t *channel, int chnNum) {
    if (channel == NULL || !channel->enable || channel->jpeg_en_flag == 0) return -1;
    if (channel->jpeg_st_flag) return 0;

    VENC_RECV_PIC_PARAM_S  stRecvParam;
    stRecvParam.s32RecvPicNum = -1;

    int ret = TS_MPI_VENC_StartRecvFrame(chnNum, &stRecvParam);
    if (ret < 0) {
        vp_error("TS_MPI_VENC_StartRecvFrame idx:%d error:%d", chnNum, ret);
        return ret;
    }
#if 0
    pthread_t ph;
    pthread_create(&ph, NULL, vp_video_jpeg_thread_func, (void *) channel);
#endif

    channel->jpeg_st_flag = 1;
    return ret;
}

/**
 * 打开通道流
 *
 * @note step 3
 *
 * @param channel   [in]    通道信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
static int vp_video_frame_source_start(vp_channel_config_t *channel) {
    if (channel == NULL || !channel->enable || channel->encoder_flag == 0) return -1;

    int ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;
    stRecvParam.s32RecvPicNum = -1;
    int enc_chn = channel->enc_chn;

    switch (channel->format) {
        case vp_video_encoder_format_h264:
        case vp_video_encoder_format_h265: 
        case vp_video_encoder_format_jpeg:{
            ret = TS_MPI_VENC_StartRecvFrame(enc_chn, &stRecvParam);
            if (ret) {
                vp_error("TS_MPI_VENC_StartRecvFrame(%d) error:%d", enc_chn, ret);
                return -1;
            }

            VENC_RC_PARAM_S    stRcParm;
			memset(&stRcParm, 0, sizeof(VENC_RC_PARAM_S));
			TS_MPI_VENC_GetRcParam(enc_chn, &stRcParm);
			if (enc_chn <= 5) {
				if (enc_chn % 2 == 0) {	// 主码流
                    #if defined(VENC_RC_VBR)
					stRcParm.stParamH265Vbr.u32MaxQp = MAIN_RC_MAX_QP_P;
					stRcParm.stParamH265Vbr.u32MinQp = MAIN_RC_MIN_QP_P;
					stRcParm.stParamH265Vbr.u32MaxIQp = MAIN_RC_MAX_QP_I;
					stRcParm.stParamH265Vbr.u32MinIQp = MAIN_RC_MIN_QP_I;
                    #else
                    stRcParm.stParamH265Cbr.u32MaxQp = MAIN_RC_MAX_QP_P;
                    stRcParm.stParamH265Cbr.u32MinQp = MAIN_RC_MIN_QP_P;
                    stRcParm.stParamH265Cbr.u32MaxIQp = MAIN_RC_MAX_QP_I;
                    stRcParm.stParamH265Cbr.u32MinIQp = MAIN_RC_MIN_QP_I;
                    #endif
				} else {  // 子码流
                    #if defined(VENC_RC_VBR)
					stRcParm.stParamH265Vbr.u32MaxQp = SUB_RC_MAX_QP_P;
					stRcParm.stParamH265Vbr.u32MinQp = SUB_RC_MIN_QP_P;
					stRcParm.stParamH265Vbr.u32MaxIQp = SUB_RC_MAX_QP_I;
					stRcParm.stParamH265Vbr.u32MinIQp = SUB_RC_MIN_QP_I;
                    #else
                    stRcParm.stParamH265Cbr.u32MaxQp = SUB_RC_MAX_QP_P;
                    stRcParm.stParamH265Cbr.u32MinQp = SUB_RC_MIN_QP_P;
                    stRcParm.stParamH265Cbr.u32MaxIQp = SUB_RC_MAX_QP_I;
                    stRcParm.stParamH265Cbr.u32MinIQp = SUB_RC_MIN_QP_I;
                    #endif
				}
				TS_MPI_VENC_SetRcParam(enc_chn, &stRcParm);
            }
        }
            break;
        default:
            return -1;
    }
    if (channel->enable_jpeg) vp_video_jpeg_start(channel, channel->chn_jpeg);
    return 0;
}

/**
 * 关闭JPEG通道流
 *
 * @param channel   [in]    通道信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
static int vp_video_jpeg_stop(vp_channel_config_t *channel, int chnNum) {
    if (channel == NULL || !channel->enable || channel->jpeg_en_flag == 0) return -1;
    if (channel->jpeg_st_flag == 0) return 0;
    channel->jpeg_st_flag = 0;
    int ret = TS_MPI_VENC_StopRecvFrame(chnNum);
    if (ret < 0) vp_error("TS_MPI_VENC_StopRecvFrame idx:%d error:%d", chnNum, ret);
    return ret;
}

static int vp_video_wait_frame_release(vp_channel_config_t *channel) {
    uint64_t bits = VP_EVENT_BIT1 | VP_EVENT_BIT2 | VP_EVENT_BIT3;
    vp_debug("vp_video_wait_frame_release bits:%llu", bits);
    int ret = vp_events_wait(channel->events, &bits, VP_EVENTS_FLAG_AND, 500);
    vp_debug("vp_video_wait_frame_release ret:%d", ret);
    return ret;
}

/**
 * 关闭通道流
 *
 * @note step 4
 *
 * @param channel   [in]    通道信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
static int vp_video_frame_source_stop(vp_channel_config_t *channel) {
    if (channel == NULL || !channel->enable || channel->encoder_flag == 0) return -1;
    int chnNum = channel->enc_chn;
    int ret;
    vp_video_wait_frame_release(channel);
    switch (channel->format) {
        case vp_video_encoder_format_h264:
        case vp_video_encoder_format_h265: {

            ret = SAMPLE_COMM_CPM_UnBind_VENC(chnNum, 0, chnNum);
            if (ret) {
                vp_error("SAMPLE_COMM_CPM_UnBind_VENC(%d) error:%d", chnNum, ret);
                return -1;
            }

            ret = TS_MPI_VENC_StopRecvFrame(chnNum);
            if (ret) {
                vp_error("TS_MPI_VENC_StopRecvFrame(%d) error:%d", chnNum, ret);
                return -1;
            }

            ret = SAMPLE_COMM_VENC_Stop(chnNum);
            if (ret) {
                vp_error("SAMPLE_COMM_VENC_Stop(%d) error:%d", chnNum, ret);
                return -1;
            }
        }
            break;
        case vp_video_encoder_format_jpeg:
            ret = vp_video_jpeg_stop(channel, chnNum);
            if (ret) {
                vp_error("vp_video_jpeg_stop(%d) error:%d", chnNum, ret);
                return -1;
            }
            break;
        default:
            return -1;
    }
    if (channel->enable_jpeg) vp_video_jpeg_stop(channel, channel->chn_jpeg);

    return 0;
}

/**
 * 销毁JPEG编码通道
 *
 * @param channel   [in]    通道信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
static int vp_video_jpeg_destroy(vp_channel_config_t *channel, int chnNum) {
    if (channel == NULL || !channel->enable || channel->jpeg_en_flag == 0) return 0;
    int ret;
    channel->jpeg_en_flag = 0;

    ret = TS_MPI_VENC_DestroyChn(chnNum);
    if (ret)
        vp_error("venc destroy chn_%d error:0x%x", chnNum, ret);

    
    return 0;
}

/**
 * 销毁数据编码通道
 *
 * @note step 5
 *
 * @param channel   [in]    通道信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
static int vp_video_encoder_destroy(vp_channel_config_t *channel) {
    if (channel == NULL || !channel->enable || channel->encoder_flag == 0) return 0;
    channel->encoder_flag = 0;
    int vpss_grp = channel->vpss_grp;
    int vpss_chn = channel->vpss_chn;
    int venc_chn = channel->enc_chn;
    int ret;

    ret = SAMPLE_COMM_VPSS_UnBind_CPM(vpss_grp, vpss_chn, venc_chn, 0);
    if (ret)
        vp_error("vpss grp_%d chn_%d unbind venc_%d error:0x%x", vpss_grp, vpss_chn, venc_chn, ret);

    ret = TS_MPI_CPM_StopGrp(venc_chn);
    if (ret)
        vp_error("StopGrp failed with %#x!\n", ret);

    ret = TS_MPI_CPM_Unregister(venc_chn);
    if (ret)
        vp_error("CPM_Unregister failed with %#x!\n", ret);

    ret = TS_MPI_CPM_DestroyGrp(channel->enc_chn);
    if (ret)
        vp_error("cpm destroy grp_%d error:%d", channel->enc_chn, ret);

    ret = TS_MPI_VPSS_DisableChn(vpss_grp, vpss_chn);
    if (ret)
        vp_error("vpss disable grp_%d chn_%d error:0x%x", vpss_grp, vpss_chn, ret);

    if (channel->enable_jpeg) vp_video_jpeg_destroy(channel, channel->chn_jpeg);

    return 0;
}

static int vp_video_ts_system(const char* pszCmd)
{
    pid_t pid;
    int status;

    if (pszCmd == NULL) {
        return (1); /**< if cmdstring is NULL  return no zero*/
    }
    if ((pid = vfork())<0) { /**<vfork,child pid share resource with parrent,not copy */
        status = TS_FAILURE ; /**<vfork fail */
    } else if (pid == 0) {
        execl("/bin/sh", "sh", "-c", pszCmd, (char *)0);
        _exit(127); /**< return 127 only exec fail;the chid procee is not exist normore if exec success */
    } else {/** parrent pid*/
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                status = TS_FAILURE ; /**< return -1 when interrupted by signal except EINTR*/
                break;
            }
        }
    }

    return status; /**< return the state of child progress if waitpid success*/
}

static void* vp_video_mipi_reset_thread(void* p) {
    const uint32_t mipi_status_addr[2] = {0xf247000c, 0xf247100c};
    const uint32_t mipi_reset_addr[2] =  {0xf2470008, 0xf2471008};
    char mipi_op_tmp[64] = { 0 };
    char buffer[32] = { 0 };
    char *endptr;
    int mipi_status = 0;
    FILE* fp = NULL;
    int i = 0;

    sleep(10);

    while (1) {
        for (i = 0; i < 2; ++i) {
            snprintf(mipi_op_tmp, 64, "devmem 0x%x", mipi_status_addr[i]);
            fp = popen(mipi_op_tmp, "r");

            if (fp) {
                if (fgets(buffer, sizeof(buffer), fp)) {
                    mipi_status = strtol(buffer, &endptr, 16);
                    if (buffer == endptr) {
                        mipi_status = 0;
                        vp_error("Conversion failed.\n");
                    }

                    if (mipi_status & (1 << 18)) {
                        snprintf(mipi_op_tmp, 64, "devmem 0x%x 32 0", mipi_reset_addr[i]);
                        vp_video_ts_system(mipi_op_tmp);
                        usleep(1000);
                        snprintf(mipi_op_tmp, 64, "devmem 0x%x 32 1", mipi_reset_addr[i]);
                        vp_video_ts_system(mipi_op_tmp);
                    }
                }
                pclose(fp);
            }

            sleep(1);
        }
    }
    
    return TS_NULL;
}

static void* vp_video_frame_source_thread(void*  param) {
    vp_channel_config_t * channel = (vp_channel_config_t *)param;
    int i = 0;
    channel->algo_frame = (char *)malloc(640*360*3/2 * sizeof(char));
    while (1) {
        if (channel->frame_flag == 0) {
            if (0 == TS_MPI_VPSS_GetChnFrame(channel->vpss_grp, channel->vpss_chn, &channel->frame, 1000)) {
                memcpy(channel->algo_frame, channel->frame.stVFrame.u64VirAddr[0], 640*360*3/2);
                channel->frame_flag = 1;
                TS_MPI_VPSS_ReleaseChnFrame(channel->vpss_grp, channel->vpss_chn, &channel->frame);
            }
        } else {
            usleep(20 * 1000);
        }
    }

    return 0;
}

/**
 * 销毁数据通道
 *
 * @note step 6
 *
 * @param channel   [in]    通道信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_frame_source_destroy(vp_channel_config_t *channel) {
    if (channel == NULL || !channel->enable || channel->frame_source_flag == 0) return 0;
    int ret;
    channel->frame_source_flag = 0;

    ret = TS_MPI_VPSS_StopGrp(channel->vpss_grp);
    if (ret)
        vp_error("vpss stop grp_%d error:0x%x", channel->vpss_grp, ret);

    ret = TS_MPI_VPSS_DestroyGrp(channel->vpss_grp);
    if (ret)
        vp_error("vpss stop grp_%d error:0x%x", channel->vpss_grp, ret);

    return 0;
}

/**
 * 获取通道所有配置信息
 *
 * @param idx [in]    通道索引
 *
 * @return  通道配置信息
 */
vp_channel_config_t *vp_video_channels(uint8_t idx) {
    if (idx >= VP_SENSOR_NUM) return NULL;
    return configs[idx].channels;
}

vp_channel_config_t *vp_video_get_channel(uint8_t idx, vp_video_chn_t chn) {
    if (idx >= VP_SENSOR_NUM || chn >= VP_VIDEO_CHANNEL_MAX) return NULL;
    return &configs[idx].channels[chn];
}

static int vp_video_draw_rect_by_soft(VIDEO_FRAME_INFO_S *in, vp_channel_config_t *channel)
{
    int i, color;
    int lineWidth = 0;
    int width = in->stVFrame.u32Width;
    int height = in->stVFrame.u32Height;

    unsigned char * YuvBuf = (unsigned char *)(TS_UL)(in->stVFrame.u64VirAddr[0]);
    RECT rect = {0};
    const char* person = "person";

    for (int j = 4; j < VP_VIDEO_OSD_MAX; ++j) {
        vp_osd_item_t *item = &channel->osd_config.items[j];
        if (item->create == 0 || item->show == 0) continue;
        switch (item->type) {
            case VP_VIDEO_OSD_TYPE_RECT:
                vp_lock(&item->lock);
                rect.left   = item->x;
                rect.top    = item->y;
                rect.right  = (item->x + item->width);
                rect.bottom = (item->y + item->height);
                vp_unlock(&item->lock);
                switch (item->rect.color) {
                    case VP_OSD_RECT_RED:
                    color = YUV_RED;
                    break;
                    case VP_OSD_RECT_BLACK:
                    color = YUV_DARK_BLACK;
                    break;
                    case VP_OSD_RECT_GREEN:
                    color = YUV_DARK_GREEN;
                    break;
                    case VP_OSD_RECT_YELLOW:
                    color = YUV_YELLOW;
                    break;
                    default:
                    color = YUV_DARK_GREEN;
                    break;
                }

                lineWidth = item->rect.line;
                YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, color, lineWidth);
                overlay_letter(person, YuvBuf, width, height, rect.left, rect.top, color, 2);
                break;
            case VP_VIDEO_OSD_TYPE_MAX:
                break;
        }
    }

    return 0;
}

static int vp_video_encoder_cpm_init_main1(void **pHandle)
{
    vp_debug("cpm init");
    *pHandle = (void *)vp_video_get_channel(0, 0);
    return 0;
}

static int vp_video_encoder_cpm_init_sub1(void **pHandle)
{
    vp_debug("cpm init");
    *pHandle = (void *)vp_video_get_channel(0, 1);
    return 0;
}

static int vp_video_encoder_cpm_init_main2(void **pHandle)
{
    vp_debug("cpm init");
    *pHandle = (void *)vp_video_get_channel(1, 0);
    return 0;
}

static int vp_video_encoder_cpm_init_sub2(void **pHandle)
{
    vp_debug("cpm init");
    *pHandle = (void *)vp_video_get_channel(1, 1);
    return 0;
}

static int vp_video_encoder_cpm_init_main3(void **pHandle)
{
    vp_debug("cpm init");
    *pHandle = (void *)vp_video_get_channel(2, 0);
    return 0;
}

static int vp_video_encoder_cpm_init_sub3(void **pHandle)
{
    vp_debug("cpm init");
    *pHandle = (void *)vp_video_get_channel(2, 1);
    return 0;
}

static void vp_video_encoder_cpm_exit(void *pHandle)
{
    vp_debug("cpm exit");
}

static int vp_video_encoder_cpm_get_param(void *pHandle, void *pParam)
{
    vp_debug("cpm get param");
    return 0;
}

static int vp_video_encoder_cpm_set_param(void *pHandle, void *pParam)
{
    vp_debug("cpm set param");
    return 0;
}

static int vp_video_encoder_cpm_get_result(void *pHandle, void *pResult)
{
    vp_debug("cpm get result");
    return 0;
}

static int vp_video_encoder_cpm_release_result(void *pHandle, void *pResult)
{
    vp_debug("cpm release result");
    return 0;
}

static int vp_video_encoder_cpm_process(void *pHandle, void **in, void **out)
{
    CHECK_NULL_PTR(in);
    CHECK_NULL_PTR(*in);
    CHECK_NULL_PTR(out);
    CHECK_NULL_PTR(*out);

    vp_channel_config_t *vp_channel = (vp_channel_config_t *)pHandle;

    VIDEO_FRAME_INFO_S *inPipeFrameVenc = in[0];
    vp_video_draw_rect_by_soft(inPipeFrameVenc, vp_channel);

    TS_MPI_SYS_MflushCache(inPipeFrameVenc->stVFrame.u64PhyAddr[0],
                                (TS_VOID *)(uintptr_t)(inPipeFrameVenc->stVFrame.u64VirAddr[0]),
                                inPipeFrameVenc->stVFrame.size);

    memcpy(out[0], in[0], sizeof(VIDEO_FRAME_INFO_S));

    VIDEO_FRAME_INFO_S *bufinfo = (VIDEO_FRAME_INFO_S *)in[0];
    if(bufinfo)
        TS_MPI_VB_DupBlock_UID(bufinfo->u32PoolId, bufinfo->stVFrame.u64PhyAddr[0], VB_UID_CPM);

    return 0;
}

/**
 * 初始化视频编码模块
 *
 * @note 一般情况不需要用户进行调用,SDK会自行进行初始化
 *
 * @param format   [in]        1：视频264编码使能  其他： 使用默认编码
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */

int vp_video_encoder_init(int format)
{
    (void) format;
    int ret;// init_vw, init_vh;
    int i = 0;
    //SAMPLE_VI_CONFIG_S  stViConfig;
    DYNAMIC_RANGE_E     enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E      enPixFormat = PIXEL_FORMAT_NV_12;
    VPSS_GRP_ATTR_S     stVpssGrpAttr = {0};

    memset(specifics, 0, sizeof(specifics));
    //init_vw = 640;//1920;//640;
    //init_vh = 360;//1080;//360;

    //sys
    VB_CONFIG_S stVbConf;
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
    stVbConf.astCommPool[0].u64BlkSize	 = 640*360*3/2;
    stVbConf.astCommPool[0].u32BlkCnt	  = 1;
    stVbConf.u32MaxPoolCnt = 1;

    ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (ret != TS_SUCCESS) {
        vp_error("sys init error:0x%x", ret);
        return -1;
    }

    //vi
    memset(&stViConfig, 0, sizeof(stViConfig));
    stViConfig.s32WorkingViNum = VP_SENSOR_NUM;
    SAMPLE_PIPE_INFO_S * pstPipeInfo = NULL;
    SAMPLE_CHN_INFO_S * pstChnInfo = NULL;

    for (i = 0; i < VP_SENSOR_NUM; i ++) {
        pstPipeInfo = &stViConfig.astViInfo[i].stPipeInfo;
		pstChnInfo = &stViConfig.astViInfo[i].stChnInfo;

        pstPipeInfo->aPipe		= i;
        pstPipeInfo->enBitWid	= DATA_BITWIDTH_8;
        pstPipeInfo->enBayer    = BAYER_GRBG;
        pstPipeInfo->width      = 1920;
        pstPipeInfo->height     = 1080;
        pstPipeInfo->frameRate  = FRAME_RATE;
        pstPipeInfo->enWdrMode	= WDR_MODE_NONE;
        pstPipeInfo->bIspByFly  = TS_FALSE;
        pstPipeInfo->bDynFpsSync = TS_FALSE;
        pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_ONLINE;
		pstPipeInfo->enPixFmt	= PIXEL_FORMAT_RGB_888;

        pstChnInfo->ViChn[0]	= 0;
		pstChnInfo->width[0]	= 1920;
		pstChnInfo->height[0]	= 1080;
		pstChnInfo->validChnlNum= 1;
		pstChnInfo->enPixFormat = PIXEL_FORMAT_NV_12;
    }

    ret = SAMPLE_COMM_VI_SetParam(&stViConfig);
    if (TS_SUCCESS != ret)
    {
        vp_error("SAMPLE_COMM_VI_SetParam failed with %d!\n", ret);
        return -1;
    }

    ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (ret != TS_SUCCESS) {
        SAMPLE_COMM_SYS_Exit();
        vp_error("vi start error:0x%x", ret);
        return -1;
    }

    //vpss
    memset(&stVpssGrpAttr, 0, sizeof(VPSS_GRP_ATTR_S));

    for (int s = 0; s < VP_SENSOR_NUM; ++s) {
        vp_channel_config_t *channels = vp_video_channels(s);
        stVpssGrpAttr.enGrpMode = VPSS_GRP_MODE_STREAM;
        stVpssGrpAttr.enDynamicRange = enDynamicRange;
        stVpssGrpAttr.enPixelFormat  = enPixFormat;
        stVpssGrpAttr.u32MaxW        = 1920;
        stVpssGrpAttr.u32MaxH        = 1080;
        stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;//10;
        stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;//10;

        ret = TS_MPI_VPSS_CreateGrp(s, &stVpssGrpAttr);
        if (ret) {
            vp_warn("vpss create grp grp_%d error:0x%x", s, ret);
            goto EXIT_VPSS_STOP;
        }

        for (int i = 0; i < VP_VIDEO_CHANNEL_MAX; ++i) {
            vp_channel_config_t *channel = &channels[i];
            channel->events = vp_events_create();
            memset(channel->osd_config.items, 0, sizeof(channel->osd_config.items));
            vp_video_config_print(s, i, channel);

            if (channel->enable) {
                vp_events_send(channel->events, VP_EVENT_BIT1 | VP_EVENT_BIT2 | VP_EVENT_BIT3);
                //channel->fs_chn_attr.u32MaxW        = init_vw;
                //channel->fs_chn_attr.u32MaxH        = init_vh;
                //channel->fs_chn_attr.u32Width       = init_vw;
                //channel->fs_chn_attr.u32Height      = init_vh;
                //channel->fs_chn_attr.u32MemCount    = 1;

                uint32_t height = channel->fs_chn_attr.u32Height;
                uint32_t width  = channel->fs_chn_attr.u32Width;
                channel->frame_size = width * height * 3 / 2;

                ret = TS_MPI_VPSS_SetChnAttr(s, i, &channel->fs_chn_attr);
                if (ret) {
                    vp_error("vpss set chn attr grp_%d chn_%d error:0x%x", s, i, ret);
                    goto EXIT_VPSS_STOP;
                }

                ret = TS_MPI_VPSS_EnableChn(s, i);
                if (ret) {
                    vp_error("vpss enable chn grp_%d chn_%d error:0x%x", s, i, ret);
                    goto EXIT_VPSS_STOP;
                }
                channel->frame_source_flag = 1;
            }
        }

        ret = TS_MPI_VPSS_StartGrp(s);
        if (ret) {
            vp_error("vpss enable grp_%d error:0x%x", s, ret);
            goto EXIT_VPSS_STOP;
        }
    }

    #if VP_SENSOR_NUM > 0
    //cpm
    CPM_GRP         CPMGrp = 0;
    CPM_GRP_ATTR_S  stCpmGrpAttr = {0};
    CPM_Handle_S    stCPMHandle = {0};

    stCpmGrpAttr.u32GrpId                   = CPMGrp;
    stCpmGrpAttr.u32PipeNum                 = 1;
    stCpmGrpAttr.u32ChnNum                  = 1;
    stCpmGrpAttr.u32Interval                = 0;
    stCpmGrpAttr.bSyncPipe                  = 0;
    stCpmGrpAttr.stChnAttr[0].u32Width      = 1920;
    stCpmGrpAttr.stChnAttr[0].u32Height     = 1080;
    stCpmGrpAttr.stChnAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;

    stCPMHandle.s32PipeNum                  = 1;
    stCPMHandle.stPipeAttr[0].u32Width      = 1920;
    stCPMHandle.stPipeAttr[0].u32Height     = 1080;
    stCPMHandle.stPipeAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;

    stCPMHandle.init            = vp_video_encoder_cpm_init_main1;
    stCPMHandle.exit            = vp_video_encoder_cpm_exit;
    stCPMHandle.process         = vp_video_encoder_cpm_process;
    stCPMHandle.set_param       = vp_video_encoder_cpm_set_param;
    stCPMHandle.get_param       = vp_video_encoder_cpm_get_param;
    stCPMHandle.get_result      = vp_video_encoder_cpm_get_result;
    stCPMHandle.release_result  = vp_video_encoder_cpm_release_result;

    ret = SAMPLE_COMM_CPM_Start(CPMGrp, &stCpmGrpAttr, &stCPMHandle);
    if (ret) {
        vp_error("SAMPLE_COMM_CPM_Start failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }

    ret = SAMPLE_COMM_VPSS_Bind_CPM(0, 0, CPMGrp, 0);
    if (ret) {
        vp_error("SAMPLE_COMM_VPSS_Bind_CPM failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }

    CPMGrp++;
    stCpmGrpAttr.u32GrpId                   = CPMGrp;
    stCpmGrpAttr.stChnAttr[0].u32Width      = 640;
    stCpmGrpAttr.stChnAttr[0].u32Height     = 360;
    stCpmGrpAttr.stChnAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;

    stCPMHandle.s32PipeNum                  = 1;
    stCPMHandle.stPipeAttr[0].u32Width      = 640;
    stCPMHandle.stPipeAttr[0].u32Height     = 360;
    stCPMHandle.stPipeAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;
    stCPMHandle.init = vp_video_encoder_cpm_init_sub1;

    ret = SAMPLE_COMM_CPM_Start(CPMGrp, &stCpmGrpAttr, &stCPMHandle);
    if (ret) {
        vp_error("SAMPLE_COMM_CPM_Start failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }

    ret = SAMPLE_COMM_VPSS_Bind_CPM(0, 1, CPMGrp, 0);
    if (ret) {
        vp_error("SAMPLE_COMM_VPSS_Bind_CPM failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }
    #endif

    #if VP_SENSOR_NUM > 1
    CPMGrp++;
    stCpmGrpAttr.u32GrpId                   = CPMGrp;
    stCpmGrpAttr.stChnAttr[0].u32Width      = 1920;
    stCpmGrpAttr.stChnAttr[0].u32Height     = 1080;
    stCpmGrpAttr.stChnAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;

    stCPMHandle.s32PipeNum                  = 1;
    stCPMHandle.stPipeAttr[0].u32Width      = 1920;
    stCPMHandle.stPipeAttr[0].u32Height     = 1080;
    stCPMHandle.stPipeAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;
    stCPMHandle.init = vp_video_encoder_cpm_init_main2;

    ret = SAMPLE_COMM_CPM_Start(CPMGrp, &stCpmGrpAttr, &stCPMHandle);
    if (ret) {
        vp_error("SAMPLE_COMM_CPM_Start failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }

    ret = SAMPLE_COMM_VPSS_Bind_CPM(1, 0, CPMGrp, 0);
    if (ret) {
        vp_error("SAMPLE_COMM_VPSS_Bind_CPM failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }

    CPMGrp++;
    stCpmGrpAttr.u32GrpId                   = CPMGrp;
    stCpmGrpAttr.stChnAttr[0].u32Width      = 640;
    stCpmGrpAttr.stChnAttr[0].u32Height     = 360;
    stCpmGrpAttr.stChnAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;

    stCPMHandle.s32PipeNum                  = 1;
    stCPMHandle.stPipeAttr[0].u32Width      = 640;
    stCPMHandle.stPipeAttr[0].u32Height     = 360;
    stCPMHandle.stPipeAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;
    stCPMHandle.init = vp_video_encoder_cpm_init_sub2;

    ret = SAMPLE_COMM_CPM_Start(CPMGrp, &stCpmGrpAttr, &stCPMHandle);
    if (ret) {
        vp_error("SAMPLE_COMM_CPM_Start failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }

    ret = SAMPLE_COMM_VPSS_Bind_CPM(1, 1, CPMGrp, 0);
    if (TS_SUCCESS != ret)
    {
        vp_error("SAMPLE_COMM_VPSS_Bind_CPM failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }
    #endif

    #if VP_SENSOR_NUM > 2
    CPMGrp++;
    stCpmGrpAttr.u32GrpId                   = CPMGrp;
    stCpmGrpAttr.stChnAttr[0].u32Width      = 1920;
    stCpmGrpAttr.stChnAttr[0].u32Height     = 1080;
    stCpmGrpAttr.stChnAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;

    stCPMHandle.s32PipeNum                  = 1;
    stCPMHandle.stPipeAttr[0].u32Width      = 1920;
    stCPMHandle.stPipeAttr[0].u32Height     = 1080;
    stCPMHandle.stPipeAttr[0].enPixelFormat	= PIXEL_FORMAT_NV_12;
    stCPMHandle.init = vp_video_encoder_cpm_init_main3;

    ret = SAMPLE_COMM_CPM_Start(CPMGrp, &stCpmGrpAttr, &stCPMHandle);
    if (ret) {
        vp_error("SAMPLE_COMM_CPM_Start failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }

    ret = SAMPLE_COMM_VPSS_Bind_CPM(2, 0, CPMGrp, 0);
    if (ret) {
        vp_error("SAMPLE_COMM_VPSS_Bind_CPM failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }

    CPMGrp++;
    stCpmGrpAttr.u32GrpId                   = CPMGrp;
    stCpmGrpAttr.stChnAttr[0].u32Width      = 640;
    stCpmGrpAttr.stChnAttr[0].u32Height     = 360;
    stCpmGrpAttr.stChnAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;

    stCPMHandle.s32PipeNum                  = 1;
    stCPMHandle.stPipeAttr[0].u32Width      = 640;
    stCPMHandle.stPipeAttr[0].u32Height     = 360;
    stCPMHandle.stPipeAttr[0].enPixelFormat	= PIXEL_FORMAT_NV_12;
    stCPMHandle.init = vp_video_encoder_cpm_init_sub3;

    ret = SAMPLE_COMM_CPM_Start(CPMGrp, &stCpmGrpAttr, &stCPMHandle);
    if (ret) {
        vp_error("SAMPLE_COMM_CPM_Start failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }

    ret = SAMPLE_COMM_VPSS_Bind_CPM(2, 1, CPMGrp, 0);
    if (ret) {
        vp_error("SAMPLE_COMM_VPSS_Bind_CPM failed for %#x!\n", ret);
        goto EXIT_CPM_UNBIND_STOP;
    }
    #endif
    
    //encoder
    for (int s = 0; s < VP_SENSOR_NUM; ++s) {
        vp_channel_config_t *channels = vp_video_channels(s);
        for (int i = 0; i < VP_VIDEO_CHANNEL_MAX; ++i) {
            vp_channel_config_t *channel = &channels[i];
            switch (channel->format) {
            case vp_video_encoder_format_h264:
            case vp_video_encoder_format_h265:
            case vp_video_encoder_format_jpeg:
                ret = vp_video_encoder_create(channel);
                if (ret) {
                    vp_error("vp_video_encoder_create idx:%d format:%d error:%d", i, channel->format, ret);
                    goto EXIT_VENC_UNBIND_STOP;
                }
                break;
            case vp_video_encoder_format_none:
                break;
            default:
                vp_warn("index:%d config format not support %d", i, channel->format);
                break;
            }
        }
    }

    //start camera
    ret = TS_MPI_VPSS_Start_Camera();
    if (ret) {
        vp_error("start camera error:0x%x", ret);
        goto EXIT_VENC_UNBIND_STOP;
    }

    pthread_t pt;
    ret = vp_pthread_create(&pt, 64 * 1024, vp_video_mipi_reset_thread, NULL);
    if (ret) {
        vp_error("Failed to start vidoe mipi reset thread.");
    }

    return 0;

EXIT_VENC_UNBIND_STOP:
	vp_warn("venc unbind and stop starting");
    for (int s = 0; s < VP_SENSOR_NUM; ++s) {
        vp_channel_config_t *channels = vp_video_channels(s);
        for (int i = 0; i < VP_VIDEO_CHANNEL_MAX; ++i) {
            vp_channel_config_t *channel = &channels[i];
            if (!channel->enable)
                continue;
            vp_warn("vpss grp_%d chn_%d unbind venc chn_%d", s, channel->vpss_chn, channel->enc_chn);
            ret = SAMPLE_COMM_VPSS_UnBind_VENC(s, channel->vpss_chn, channel->enc_chn);
            if (ret)
                vp_error("vpss grp_%d chn_%d unbind venc chn_%d error:0x%x", s, channel->vpss_chn, channel->enc_chn, ret);

            vp_error("venc stop chn_%d", i);
            ret = SAMPLE_COMM_VENC_Stop(channel->enc_chn);
            if (ret)
                vp_error("venc stop chn_%d error:%d", i, ret);
        }
    }

EXIT_CPM_UNBIND_STOP:
    vp_warn("cpm unbind and stop.\n");
    for (int s = 0; s < VP_SENSOR_NUM; ++s) {
        vp_channel_config_t *channels = vp_video_channels(s);
        for (int i = 0; i < VP_VIDEO_CHANNEL_MAX; ++i) {
            vp_channel_config_t *channel = &channels[i];
            if (!channel->enable)
                continue;
            vp_warn("vpss grp_%d chn_%d unbind cpm grp_%d", s, channel->vpss_chn, channel->enc_chn);
            ret = SAMPLE_COMM_VPSS_UnBind_CPM(s, channel->vpss_chn, channel->enc_chn, 0);
            if (ret)
                vp_error("vpss grp_%d chn_%d unbind cpm grp_%d error:0x%x", s, channel->vpss_chn, channel->enc_chn, ret);

            vp_error("venc stop chn_%d", i);
            ret = TS_MPI_CPM_DestroyGrp(channel->enc_chn);
            if (ret)
                vp_error("cpm stop grp_%d error:%d", channel->enc_chn, ret);
        }
    }

EXIT_VPSS_STOP:
	vp_warn("stop vpss");
    for (int s = 0; s < VP_SENSOR_NUM; ++s) {
        vp_channel_config_t *channels = vp_video_channels(s);
        for (int i = 0; i < VP_VIDEO_CHANNEL_MAX; ++i) {
            vp_channel_config_t *channel = &channels[i];
            if (!channel->enable)
                continue;
            vp_warn("vpss stop group_%d chn_%d", s, channel->vpss_chn);
            ret = TS_MPI_VPSS_DisableChn(s, channel->vpss_chn);
            if (ret)
                vp_error("vpss disable group_%d chn_%d error:0x%x", s, channel->vpss_chn, ret);
        }

        vp_warn("vpss stop group_%d", s);
        ret = TS_MPI_VPSS_StopGrp(s);
        if (ret)
            vp_error("vpss stop group_%d error:0x%x", s, ret);

        vp_warn("vpss destroy group_%d", s);
        ret = TS_MPI_VPSS_DestroyGrp(s);
        if (ret)
            vp_error("vpss destroy group_%d error:0x%x", s, ret);
    }

    vp_error("stop vi");
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    vp_error("exit sys");
    SAMPLE_COMM_SYS_Exit();

    return ret;
}

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
int vp_video_encoder_get_config(uint8_t idx, vp_video_chn_t chn, vp_video_encoder_config_t *config) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;
    config->enable = channel->enable;
    config->chn = channel->chn;
    config->width = channel->fs_chn_attr.u32Width;
    config->height = channel->fs_chn_attr.u32Height;
    config->frame_size = channel->frame_size;
    config->format = channel->format;
    config->fps = channel->fps;
    config->bitrate = channel->bitrate;
    config->enable_jpeg = channel->enable_jpeg;
    config->enable_aov = channel->enable_aov;
    return 0;
}

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
int vp_video_encoder_specific(uint8_t idx, vp_video_chn_t chn, vp_video_encoder_specific_t *specific) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || specific == NULL) return -1;
    memcpy(specific, &specifics[idx][chn], sizeof(vp_video_encoder_specific_t));
    return 0;
}

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
int vp_video_encoder_error_flag(uint8_t idx) {
    if (idx >= VP_SENSOR_NUM) return -1;
    for (int chn = 0; chn < vp_video_chn_max; ++chn) {
        if (configs[idx].channels[chn].error_flag) return 1;
    }
    return 0;
}

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
int vp_video_encoder_get_fps(uint8_t idx, vp_video_chn_t chn, float *fps) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || fps == NULL) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;
    VPSS_CHN_ATTR_S vpssChnAttr;

    int ret = TS_MPI_VPSS_GetChnAttr(channel->vpss_grp, channel->vpss_chn, &vpssChnAttr);
    if (ret == 0) {
        *fps = vpssChnAttr.stFrameRate.s32DstFrameRate;
    }
    return ret;
}

/**
 * 设置GOP大小
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param gop   [in]    GOP,单位:秒,每多少秒编码一个关键帧
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_set_gop(uint8_t idx, vp_video_chn_t chn, uint32_t gop) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max || gop == 0) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;
    VENC_CHN_ATTR_S vencChnAttr = { 0 };

    int ret = TS_MPI_VENC_GetChnAttr(channel->enc_chn, &vencChnAttr);
    if (ret == 0) {
        if (channel->payloadType == PT_H265) {
            #if defined(VENC_RC_VBR)
            vencChnAttr.stRcAttr.stH265Vbr.u32Gop = gop*channel->fs_chn_attr.stFrameRate.s32DstFrameRate;
            #else
            vencChnAttr.stRcAttr.stH265Cbr.u32Gop = gop*channel->fs_chn_attr.stFrameRate.s32DstFrameRate;
            #endif
            ret = TS_MPI_VENC_SetChnAttr(channel->enc_chn, &vencChnAttr);
        }
        else if (channel->payloadType == PT_H264) {
            #if defined(VENC_RC_VBR)
            vencChnAttr.stRcAttr.stH264Vbr.u32Gop = gop*channel->fs_chn_attr.stFrameRate.s32DstFrameRate;
            #else
            vencChnAttr.stRcAttr.stH264Cbr.u32Gop = gop*channel->fs_chn_attr.stFrameRate.s32DstFrameRate;
            #endif
            ret = TS_MPI_VENC_SetChnAttr(channel->enc_chn, &vencChnAttr);
        }
        else
            ret = -1;
        vp_debug("gop:%d", gop);
        ret = TS_MPI_VENC_SetChnAttr(channel->enc_chn, &vencChnAttr);
    }
    return ret;
}

/**
 * 获取GOP大小
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param gop   [out]   GOP,单位:秒,每多少秒编码一个关键帧
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_get_gop(uint8_t idx, vp_video_chn_t chn, uint32_t *gop) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;
    VENC_CHN_ATTR_S vencChnAttr = { 0 };

    int ret = TS_MPI_VENC_GetChnAttr(channel->enc_chn, &vencChnAttr);
    if (ret == 0 && gop) {
        if(channel->payloadType == PT_H265)
            #if defined(VENC_RC_VBR)
            *gop = vencChnAttr.stRcAttr.stH265Vbr.u32Gop / channel->fs_chn_attr.stFrameRate.s32DstFrameRate;
            #else
            *gop = vencChnAttr.stRcAttr.stH265Cbr.u32Gop / channel->fs_chn_attr.stFrameRate.s32DstFrameRate;
            #endif
        else if (channel->payloadType == PT_H264)
            #if defined(VENC_RC_VBR)
            *gop = vencChnAttr.stRcAttr.stH264Vbr.u32Gop / channel->fs_chn_attr.stFrameRate.s32DstFrameRate;
            #else
            *gop = vencChnAttr.stRcAttr.stH264Cbr.u32Gop / channel->fs_chn_attr.stFrameRate.s32DstFrameRate;
            #endif
        else
            ret = -1;
    }
    return ret;
}

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
int vp_video_encoder_set_bitrate(uint8_t idx, vp_video_chn_t chn, int bitrate) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);

    VENC_CHN_ATTR_S vencChnAttr = { 0 };

    int ret = TS_MPI_VENC_GetChnAttr(channel->enc_chn, &vencChnAttr);
    if (ret == 0) {
        if (channel->payloadType == PT_H265) {
            #if defined(VENC_RC_VBR)
            vencChnAttr.stRcAttr.stH265Vbr.u32MaxBitRate = (uint32_t)(bitrate*1.5);
            #else
            vencChnAttr.stRcAttr.stH265Cbr.u32BitRate = bitrate;
            vencChnAttr.stRcAttr.stH265Cbr.u32MaxBitRate = (uint32_t)(bitrate*1.5);
            #endif
            ret = TS_MPI_VENC_SetChnAttr(channel->enc_chn, &vencChnAttr);
        } else if (channel->payloadType == PT_H264) {
            #if defined(VENC_RC_VBR)
            vencChnAttr.stRcAttr.stH264Vbr.u32MaxBitRate = (uint32_t)(bitrate*1.5);
            #else
            vencChnAttr.stRcAttr.stH264Cbr.u32BitRate = bitrate;
            vencChnAttr.stRcAttr.stH264Cbr.u32MaxBitRate = (uint32_t)(bitrate*1.5);
            #endif
            ret = TS_MPI_VENC_SetChnAttr(channel->enc_chn, &vencChnAttr);
        }
        else
            ret = -1;
        vp_debug("bitrate:%d maxbitrate:%d", bitrate, (uint32_t)(bitrate*1.5));
    }
    return ret;
}

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
int vp_video_encoder_start(uint8_t idx, vp_video_chn_t chn) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;
    if (!channel->enable || !channel->encoder_flag) return -1;
    if (channel->start_flag) return 0;
    int ret = vp_video_frame_source_start(channel);
    if (ret == 0) channel->start_flag = 1;
    return ret;
}

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
int vp_video_encoder_pause(uint8_t idx, vp_video_chn_t chn) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;
    if (!channel->enable || !channel->encoder_flag || !channel->start_flag) return -1;
    if (channel->pause_flag) return 0;
    vp_debug("start idx:%d chn:%d", idx, chn);
    vp_events_clear(channel->events, VP_EVENT_BIT0);
    channel->pause_flag = 1;
    int ret = vp_video_frame_source_stop(channel);
    vp_debug("end idx:%d chn:%d ret:%d", idx, chn, ret);
    return ret;
}

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
int vp_video_encoder_resume(uint8_t idx, vp_video_chn_t chn) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;
    if (!channel->enable || !channel->encoder_flag || !channel->start_flag) return -1;
    if (!channel->pause_flag) return 0;
    vp_debug("start idx:%d chn:%d", idx, chn);
    //vp_video_osd_update_timestamp(idx, chn, vp_local_time_s());
    int ret = vp_video_frame_source_start(channel);
    channel->pause_flag = 0;
    vp_events_send(channel->events, VP_EVENT_BIT0);
    vp_debug("end idx:%d chn:%d ret:%d", idx, chn, ret);
    return ret;
}

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
int vp_video_encoder_install_key(uint8_t idx, vp_video_chn_t chn) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;
    if (channel->enable != 1 || channel->start_flag != 1 || channel->pause_flag) return -1;
    return TS_MPI_VENC_RequestIDR(channel->chn, 1);
}

#if 1
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
                          uint8_t *jpeg_data, uint32_t *jpeg_size) {

    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;

    if (jpeg_data == NULL || jpeg_size == NULL || channel->start_flag != 1) return -1;

    int ret;
    int64_t len;

    // char acFile[128] = { 0 };
    // FILE *pFile;

    // snprintf(acFile, 128, "snap_vgrp_%d_vchn_%d_jchn_%d.jpg", channel->vpss_grp, channel->vpss_chn, channel->chn_jpeg);
    // pFile = fopen(acFile, "wb");
    // if (pFile == NULL)
    //     vp_error("fopen fail.");
    VIDEO_FRAME_INFO_S video_frame = { 0 };
    VIDEO_FRAME_INFO_S *pvideo_frame = &video_frame;

    int vpss_grp = channel->vpss_grp;
    int vpss_chn = channel->vpss_chn;
    channel->jpeg_data = jpeg_data;
    channel->jpeg_size = jpeg_size;
    channel->jpeg_snap_ok = 0;
    VENC_STREAM_S stream;
    VENC_CHN_STATUS_S stStat;
    static int init_flag = 0;
    pause:
    if (channel->pause_flag) {
        uint64_t bits = VP_EVENT_BIT0;
        vp_events_wait(channel->events, &bits, VP_EVENTS_FLAG_OR, 0);
    }

    int chnNum = (int) channel->chn_jpeg;
    if (channel->enable_jpeg == 0 && init_flag == 0) {
        ret = vp_video_jpeg_create(channel, chnNum);
        if (ret < 0) goto jpeg_error;
        ret = vp_video_jpeg_start(channel, chnNum);
        if (ret < 0) goto jpeg_error;
        init_flag = 1;
    }

    //sleep(2);
    if (frame == NULL || frame->priv_data == NULL) {
        ret = TS_MPI_VPSS_GetChnFrame(vpss_grp, vpss_chn, pvideo_frame, -1);
        if (ret) {
            vp_error("get vpss_%d chn_%d frame  error:0x%x", vpss_grp, vpss_chn, ret);
            return -1;
        }
    } else {
        pvideo_frame = (VIDEO_FRAME_INFO_S *)frame->priv_data;
    }

#if 1
    ret = TS_MPI_VENC_SendFrame(chnNum, pvideo_frame, -1);
    if (ret) {
        vp_error("TS_MPI_VB_DupBlock  error:0x%x", ret);
        return -1;
    }
#endif

#if 0
    while(!channel->jpeg_snap_ok) {
        usleep(10*1000);
    }
#endif
#if 1
    ret = TS_MPI_VENC_QueryStatus(channel->chn_jpeg, &stStat);
    if (ret) {
        vp_error("venc query status error:0x%x", ret);
        return -1;
    }

    stream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
    if (NULL == stream.pstPack) {
        vp_error("malloc stream pack failed!");
        return -1;
    }

    ret = TS_MPI_VENC_GetStream(chnNum, &stream, -1);
    if (ret)
    {
        free(stream.pstPack);
        stream.pstPack = NULL;
        vp_error("TS_MPI_VENC_GetStream chn:%d error", chnNum);
        return -1;
    }
#if 0
    ret = SAMPLE_COMM_VENC_SaveStream(pFile, &stream);
    if (ret)
        vp_error("save jpeg error.");
#endif
    len = vp_video_read_stream(&stream, jpeg_data, *jpeg_size);

    ret = TS_MPI_VENC_ReleaseStream(chnNum, &stream);
    if (ret) vp_error("IMP_Encoder_ReleaseStream chn:%d error", chnNum);
    if (stream.pstPack) {
        free(stream.pstPack);
        stream.pstPack = NULL;
    }

    // if (pFile) fclose(pFile);

#endif
    if (frame == NULL || frame->priv_data == NULL) {
        ret = TS_MPI_VPSS_ReleaseChnFrame(vpss_grp, vpss_chn, pvideo_frame);
        vp_events_send(channel->events, VP_EVENT_BIT3);
        if (ret) {
            ret = (int) len;
            vp_error("TS_MPI_VPSS_ReleaseChnFrame  error:0x%x", ret);
            goto jpeg_error;
        }
    }

    ret = 0;
    if (len > 0)
        *jpeg_size = len;
    else 
        ret = (int)len;

    jpeg_error:
    // if (channel->enable_jpeg == 0) {
    //     vp_video_jpeg_stop(channel, chnNum);
    //     vp_video_jpeg_destroy(channel, chnNum);
    // }
    if (channel->pause_flag) goto pause;
    return ret;
}
#endif
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
int vp_video_encoder_request_source_algo(uint8_t idx, vp_video_chn_t chn, vp_video_source_t *frame) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;
    if (frame == NULL || channel->start_flag != 1) return -1;
    if (frame->frame_data != NULL && frame->frame_size < channel->frame_size) return -2;

    int vpss_grp = channel->vpss_grp;
    int vpss_chn = channel->vpss_chn;
    VIDEO_FRAME_INFO_S *frame_info;

    if (channel->frame_thread == 0) {
        if (vp_pthread_create(&channel->frame_thread, 64 * 1024, vp_video_frame_source_thread, channel)) {
            vp_error("failed to create vp video frame source thread.\n");
            return -1;
        }
    }

    pause:
    if (channel->pause_flag) {
        uint64_t bits = VP_EVENT_BIT0;
        vp_events_wait(channel->events, &bits, VP_EVENTS_FLAG_OR, 0);
    }

    vp_events_clear(channel->events, VP_EVENT_BIT2);
    while (0 == channel->frame_flag) {
        usleep(10 * 1000);
    }
    vp_events_send(channel->events, VP_EVENT_BIT2);

    frame_info = &channel->frame;
    frame->priv_data = frame_info;

    // frame->frame_data = (uint8_t*)(uintptr_t)frame_info->stVFrame.u64VirAddr[0];
    frame->frame_data = (uint8_t*)(uintptr_t)channel->algo_frame;
    frame->timestamp = vp_system_time_us();
    frame->width = frame_info->stVFrame.u32Width;
    frame->height = frame_info->stVFrame.u32Height;
    return 0;
}

/**
 * 释放视频原始帧
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param frame [in]    帧信息
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_release_source_algo(uint8_t idx, vp_video_chn_t chn, vp_video_source_t *frame) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;

    int vpss_grp = channel->vpss_grp;
    int vpss_chn = channel->vpss_chn;
    // int ret;

    if (frame->frame_data)
        frame->frame_data = NULL;

    // ret = TS_MPI_VPSS_ReleaseChnFrame(vpss_grp, vpss_chn, frame->priv_data);
    channel->frame_flag = 0;
    // if(ret)
    //     vp_error("TS_MPI_VPSS_ReleaseChnFrame vpss_%d vpss_%d error:0x%x", vpss_grp, vpss_chn, ret);

    return 0;
}

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
int vp_video_encoder_request_source(uint8_t idx, vp_video_chn_t chn, vp_video_source_t *frame) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (channel == NULL) return -1;
    if (frame == NULL || channel->start_flag != 1) return -1;
    if (frame->frame_data != NULL && frame->frame_size < channel->frame_size) return -2;

    int vpss_grp = channel->vpss_grp;
    int vpss_chn = channel->vpss_chn;
    VIDEO_FRAME_INFO_S *frame_info;

    int ret;
    pause:
    if (channel->pause_flag) {
        uint64_t bits = VP_EVENT_BIT0;
        vp_events_wait(channel->events, &bits, VP_EVENTS_FLAG_OR, 0);
    }
    if (frame->priv_data == NULL) {
        frame->priv_data = calloc(1, sizeof(VIDEO_FRAME_INFO_S));
    }
    frame_info = frame->priv_data;

    vp_events_clear(channel->events, VP_EVENT_BIT2);
    ret = TS_MPI_VPSS_GetChnFrame(vpss_grp, vpss_chn, frame_info, 1000);
    vp_events_send(channel->events, VP_EVENT_BIT2);
    if (ret) {
        if (channel->pause_flag) goto pause;
        return ret;
    }

    frame->frame_data = (uint8_t*)(uintptr_t)frame_info->stVFrame.u64VirAddr[0];
    frame->timestamp = vp_system_time_us();
    frame->width = frame_info->stVFrame.u32Width;
    frame->height = frame_info->stVFrame.u32Height;
    return ret;
}

/**
 * 释放视频原始帧
 *
 * @param idx   [in]    视频编号,与 Sensor 对应
 * @param chn   [in]    视频流通道
 * @param frame [in]    帧信息
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_release_source(uint8_t idx, vp_video_chn_t chn, vp_video_source_t *frame) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);

    int vpss_grp = channel->vpss_grp;
    int vpss_chn = channel->vpss_chn;
    int ret;

    if (frame->frame_data)
        frame->frame_data = NULL;

    if (frame->priv_data) {
        ret = TS_MPI_VPSS_ReleaseChnFrame(vpss_grp, vpss_chn, frame->priv_data);
        if(ret)
            vp_error("TS_MPI_VPSS_ReleaseChnFrame vpss_%d vpss_%d error:0x%x", vpss_grp, vpss_chn, ret);

        free(frame->priv_data);
        frame->priv_data = NULL;
    }

    return 0;
}



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
int vp_video_encoder_request_frame(uint8_t idx, vp_video_chn_t chn, vp_video_coder_frame_t *frame) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (frame == NULL || channel->start_flag != 1) return -1;
    int enc_chn = (int) channel->enc_chn;
    int ret = -1;
    VENC_STREAM_S *stream = &channel->stream;
    VENC_CHN_STATUS_S stStat;

pause:
    if (channel->pause_flag) {
        uint64_t bits = VP_EVENT_BIT0;
        vp_events_wait(channel->events, &bits, VP_EVENTS_FLAG_OR, 0);
        channel->timestamp = 0;
    }
    switch (channel->format) {
        case vp_video_encoder_format_h264:
        case vp_video_encoder_format_h265:
        case vp_video_encoder_format_jpeg:
            vp_events_clear(channel->events, VP_EVENT_BIT1);

            ret = TS_MPI_VENC_QueryStatus(enc_chn, &stStat);
            if (ret) {
                vp_error("venc query status error:0x%x", ret);
                return -1;
            }
            if (stStat.u32CurPacks == 0) {
                usleep(10*1000);
                return 1;
            }

            stream->pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
            if (NULL == stream->pstPack) {
                vp_error("malloc stream pack failed!");
                return -1;
            }
            
            stream->u32PackCount = stStat.u32CurPacks;
            ret = TS_MPI_VENC_GetStream(enc_chn, stream, -1);
            if (ret)
            {
                free(stream->pstPack);
                stream->pstPack = NULL;
                vp_events_send(channel->events, VP_EVENT_BIT1);
                if (channel->start_flag != 1) return -1;
                if (channel->pause_flag) goto pause;
                channel->error_flag = 1;
                vp_error("IMP_Encoder_GetStream chn:%d error", enc_chn);
                return -1;
            }

            channel->error_flag = 0;
            frame->format = channel->format;
            frame->frame_event = 0;
            frame->priv_data = stream;
            frame->timestamp = vp_system_time_us();
            if (channel->timestamp == 0) {
                frame->duration = 1000000L / channel->fps;
            } else {
                frame->duration = frame->timestamp - channel->timestamp;
            }
            frame->frame_size = 0;
            frame->frame_offset = 0;
            frame->frame_no = stream->u32Seq;
            if (stream->pstPack[0].DataType.enH265EType == H265E_NALU_IDRSLICE
                || stream->pstPack[0].DataType.enH265EType == H265E_NALU_ISLICE
                || stream->pstPack[0].DataType.enH264EType == H264E_NALU_IDRSLICE
                || stream->pstPack[0].DataType.enH264EType == H264E_NALU_ISLICE){
            	frame->frame_key = 1;
            }else{
            	frame->frame_key = 0;
            }
            vp_video_encoder_specific_t *specific = &specifics[idx][chn];
            for (int i = 0; i < stream->u32PackCount ; i++) {
                VENC_PACK_S *pack = &stream->pstPack[i];
                if (pack->u32Len) {
                    frame->frame_size += (pack->u32Len - pack->u32Offset);
                    if (frame->frame_key) {
                        specific->format = channel->format;
                        uint8_t *data = NULL;
                        if (channel->format == vp_video_encoder_format_h265) {
                            if (pack->DataType.enH265EType == H265E_NALU_VPS) {
                                if (specific->vps_size < pack->u32Len || specific->vps == NULL) {
                                    if (specific->vps) free(specific->vps);
                                    specific->vps = malloc(pack->u32Len);
                                    specific->vps_size = pack->u32Len;
                                }
                                data = specific->vps;
                            } else if (pack->DataType.enH265EType == H265E_NALU_SPS) {
                                if (specific->sps_size < pack->u32Len || specific->sps == NULL) {
                                    if (specific->sps) free(specific->sps);
                                    specific->sps = malloc(pack->u32Len);
                                    specific->sps_size = pack->u32Len;
                                }
                                data = specific->sps;
                            } else if (pack->DataType.enH265EType == H265E_NALU_PPS) {
                                if (specific->pps_size < pack->u32Len || specific->pps == NULL) {
                                    if (specific->pps) free(specific->pps);
                                    specific->pps = malloc(pack->u32Len);
                                    specific->pps_size = pack->u32Len;
                                }
                                data = specific->pps;
                            }
                        } else if (channel->format == vp_video_encoder_format_h264) {
                            if (pack->DataType.enH264EType == H264E_NALU_SPS) {
                                if (specific->sps_size < pack->u32Len || specific->sps == NULL) {
                                    if (specific->sps) free(specific->sps);
                                    specific->sps = malloc(pack->u32Len);
                                    specific->sps_size = pack->u32Len;
                                }
                                data = specific->sps;
                            } else if (pack->DataType.enH264EType == H264E_NALU_PPS) {
                                if (specific->pps_size < pack->u32Len || specific->pps == NULL) {
                                    if (specific->pps) free(specific->pps);
                                    specific->pps = malloc(pack->u32Len);
                                    specific->pps_size = pack->u32Len;
                                }
                                data = specific->pps;
                            }
                        }
                        if (data == NULL) continue;
                        memcpy(data, (void *) pack->pu8Addr + pack->u32Offset, pack->u32Len - pack->u32Offset);
                    }
                }
            }
            channel->timestamp = frame->timestamp;
            break;
        default:
            break;
    }
    return ret;
}

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
int vp_video_encoder_frame_data(uint8_t idx, vp_video_chn_t chn, vp_video_coder_frame_t *frame, uint8_t **data) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (frame == NULL || channel->start_flag != 1) return -1;
    if (frame->frame_offset >= frame->frame_size) return 0;
    switch (channel->format) {
        case vp_video_encoder_format_h264:
        case vp_video_encoder_format_h265:
        case vp_video_encoder_format_jpeg: {
            VENC_STREAM_S *stream = (VENC_STREAM_S *) frame->priv_data;
            uint32_t offset = 0;
            for (int i = 0; i < stream->u32PackCount; i++) {
                VENC_PACK_S *pack = &stream->pstPack[i];
                if (pack->u32Len) {
                    offset += (pack->u32Len - pack->u32Offset);
                    if (frame->frame_offset >= offset) continue;
                    *data = (void *) (pack->pu8Addr + pack->u32Offset);
                    frame->frame_offset += (pack->u32Len - pack->u32Offset);
                    return (int) (pack->u32Len - pack->u32Offset);
                }
            }
            return 0;
        }
        default:
            break;
    }
    return -1;
}

/**
 * 释放流信息
 *
 * @param idx   [in]   视频编号,与 Sensor 对应
 * @param chn   [in]   视频流通道
 * @param frame [in]   输出流信息
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_release_frame(uint8_t idx, vp_video_chn_t chn, vp_video_coder_frame_t *frame) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (frame == NULL || frame->priv_data == NULL) return -1;
    int encoderNum = (int) channel->enc_chn;
    int ret = -1;

    VENC_STREAM_S *stream = frame->priv_data;
    switch (channel->format) {
        case vp_video_encoder_format_h264:
        case vp_video_encoder_format_h265:
        case vp_video_encoder_format_jpeg:
            ret = TS_MPI_VENC_ReleaseStream(encoderNum, stream);
            if (ret) vp_error("IMP_Encoder_ReleaseStream chn:%d error", encoderNum);
            if (stream->pstPack) {
                free(stream->pstPack);
                stream->pstPack = NULL;
            }
            break;
        default:
            break;
    }
    vp_events_send(channel->events, VP_EVENT_BIT1);
    return ret;
}

/**
 * 停止视频编码
 *
 * @param idx   [in]   视频编号,与 Sensor 对应
 * @param chn   [in]   视频流通道
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_stop(uint8_t idx, vp_video_chn_t chn) {
    if (idx >= VP_SENSOR_NUM || chn >= vp_video_chn_max) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (!channel->enable || !channel->encoder_flag) return -1;
    if (!channel->start_flag) return 0;
    channel->start_flag = 0;
    return vp_video_frame_source_stop(channel);
}

/**
 * 销毁视频编码
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_encoder_deinit() {
    int ret;
    for (int s = 0; s < VP_SENSOR_NUM; ++s) {
        vp_channel_config_t *channels = vp_video_channels(s);
        for (int i = 0; i < VP_VIDEO_CHANNEL_MAX; ++i) {
            channels[i].start_flag = 0;
        }
    }

    for (int s = 0; s < VP_SENSOR_NUM; ++s) {
        vp_channel_config_t *channels = vp_video_channels(s);
        for (int i = VP_VIDEO_CHANNEL_MAX - 1; i >= 0; --i) {
            vp_channel_config_t *channel = &channels[i];
            if (channel->enable != 1) continue;
            vp_debug("idx:%d chn:%d deinit", s, i);
            ret = vp_video_frame_source_stop(channel);
            if (ret) vp_warn("vp_video_frame_source_stop idx:%d error:%d", i, ret);
            switch (channels[i].format) {
                case vp_video_encoder_format_h264:
                case vp_video_encoder_format_h265:
                case vp_video_encoder_format_jpeg:
                    ret = vp_video_encoder_destroy(channel);
                    if (ret) vp_warn("vp_video_encoder_destroy idx:%d format:%d error:%d", i, channel->format, ret);
                    break;
                default:
                    vp_warn("index:%d config format not support %d", i, channel->format);
                    break;
            }
        }
        ret = vp_video_frame_source_destroy(channels);
        if (ret) vp_warn("vp_video_frame_source_destroy idx:%d error:%d", s, ret);
    }

    ret = SAMPLE_COMM_VI_StopVi(&stViConfig);
    if (ret) vp_warn("SAMPLE_COMM_VI_StopVi error:0x%x", ret);

    SAMPLE_COMM_SYS_Exit();

    return 0;
}

