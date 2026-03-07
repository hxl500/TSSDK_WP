/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_define.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_DEFINES_H__
#define __TS_DEFINES_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define TX5368_AV200				   0x50001001

#define TX5239_CV200				   0x50002001
#define TX5239_CV201				   0x50002002
#define TX5239_DV200				   0x50002003
#define TX5239_DV201				   0x50002004
#define TX5239_DV220				   0x50002005
#define TX5239_DV300				   0x50002006


#define TX5215_CV200				   0x50003001
#define TX5215_DV200				   0x50003002
#define TX5215_DV300				   0x50003003
#define TX5215_EV300				   0x50003004
#define TX5119_DV200				   0x50003005
#define TX5119_DV201				   0x50003006

#define TX5112_DV200				   0x50004001
#define TX5112_DV201				   0x50004002
#define TX5112_DV300				   0x50004003
#define TX5112_CV200				   0x50004004
#define TX5112_CV201				   0x50004005
#define TX5112_CV300				   0x50004006
#define TX5102_CV300				   0x50004007

#define TX5336_AV200				   0x50005001

#define CHIP_NAME                      "TX5xx"


#define MPP_VER_PRIX                   "_MPP_V"

#define ALIGN_NUM                      4

#define LUMA_PHY_ALIGN                 16

#define NVMEN_SIZE					   64

#define DEFAULT_ALIGN                  32
#define MAX_ALIGN                      1024
#define SEG_CMP_LENGTH                 256

#define MAX_MMZ_NAME_LEN               16

#define VO_MAX_NODE_NUM                16

/* For VB */
#define VB_MAX_POOLS                   512

/* For VENC */
#define VENC_MAX_NAME_LEN              16
#define VENC_MAX_CHN_NUM               6
#define VEDU_IP_NUM                    1
#define H264E_MAX_WIDTH                2688
#define H264E_MAX_HEIGHT               1944
#define H264E_MIN_WIDTH                256
#define H264E_MIN_HEIGHT               128
#define H265E_MAX_WIDTH                2688
#define H265E_MAX_HEIGHT               1944
#define H265E_MIN_WIDTH                256
#define H265E_MIN_HEIGHT               128
#define JPEGE_MAX_WIDTH                2688
#define JPEGE_MAX_HEIGHT               1944
#define JPEGE_MIN_WIDTH                64
#define JPEGE_MIN_HEIGHT               64
#define JPGE_MAX_NUM                   4
#define VENC_MAX_ROI_NUM               2
#define VENC_QP_HISGRM_NUM             52
#define MAX_TILE_NUM                   1
#define H265E_ADAPTIVE_QP_TYPE         5

#define VENC_MIN_INPUT_FRAME_RATE      1
#define VENC_MAX_INPUT_FRAME_RATE      240

/* For RC */
#define RC_TEXTURE_THR_SIZE            16
#define MIN_BITRATE                    2
#define MAX_BITRATE                    (100 * 1024)
#define MAX_EXTRA_BITRATE              (1000 * 1024)

/* For VDEC */
#define VDEC_MAX_CHN_NUM               16
#define VDH_MAX_CHN_NUM                0
#define VEDU_CHN_START                 VDH_MAX_CHN_NUM
#define VEDU_H264D_ERRRATE             10
#define VEDU_H264D_FULLERR             100

#define H264D_ALIGN_W                  16
#define H264D_ALIGN_H                  16
#define H265D_ALIGN_W                  16
#define H265D_ALIGN_H                  16
#define JPEGD_ALIGN_W                  16
#define JPEGD_ALIGN_H                  16
#define JPEGD_RGB_ALIGN                16

#define H264D_MAX_SPS                  32
#define H264D_MIN_SPS                  1
#define H264D_MAX_PPS                  256
#define H264D_MIN_PPS                  1
#define H264D_MAX_SLICE                300
#define H264D_MIN_SLICE                1

#define H265D_MAX_VPS                  16
#define H265D_MIN_VPS                  1
#define H265D_MAX_SPS                  16
#define H265D_MIN_SPS                  1
#define H265D_MAX_PPS                  64
#define H265D_MIN_PPS                  1
#define H265D_MAX_SLICE                200
#define H265D_MIN_SLICE                1

#define VEDU_H264D_MAX_WIDTH           3840
#define VEDU_H264D_MAX_HEIGHT          2160
#define VEDU_H264D_MIN_WIDTH           256
#define VEDU_H264D_MIN_HEIGHT          128

#define VEDU_H265D_MAX_WIDTH           3840
#define VEDU_H265D_MAX_HEIGHT          2160
#define VEDU_H265D_MIN_WIDTH           256
#define VEDU_H265D_MIN_HEIGHT          128

#define JPEGD_IP_NUM                   1
#define JPEGD_MAX_WIDTH                3840
#define JPEGD_MAX_HEIGHT               2160
#define JPEGD_MIN_WIDTH                16
#define JPEGD_MIN_HEIGHT               16

/* For Region */
#define RGN_MIN_WIDTH                  2
#define RGN_MIN_HEIGHT                 2

#define RGN_COVER_MIN_X                0
#define RGN_COVER_MIN_Y                0
#define RGN_COVER_MAX_X                8190
#define RGN_COVER_MAX_Y                8190
#define RGN_COVER_MAX_WIDTH            8192
#define RGN_COVER_MAX_HEIGHT           8192
#define RGN_COVER_MIN_THICK            2
#define RGN_COVER_MAX_THICK            8

#define RGN_MOSAIC_X_ALIGN             4
#define RGN_MOSAIC_Y_ALIGN             2
#define RGN_MOSAIC_WIDTH_ALIGN         4
#define RGN_MOSAIC_HEIGHT_ALIGN        4

#define RGN_MOSAIC_MIN_X               0
#define RGN_MOSAIC_MIN_Y               0
#define RGN_MOSAIC_MAX_X               8188
#define RGN_MOSAIC_MAX_Y               8190
#define RGN_MOSAIC_MIN_WIDTH           8
#define RGN_MOSAIC_MIN_HEIGHT          8
#define RGN_MOSAIC_MAX_WIDTH           8192
#define RGN_MOSAIC_MAX_HEIGHT          8192

#define RGN_ALIGN                      2

#define RGN_HANDLE_MAX                 0XFFFFFFFF
#define RGN_MAX_BUF_NUM                6

#define COVER_MAX_NUM_VI               0
#define OVERLAY_MAX_NUM_VI             0
#define MOSAIC_MAX_NUM_VI              0

#define OVERLAY_MAX_NUM_VENC           8

#define COVER_MAX_NUM_VPSS             8
#define OVERLAY_MAX_NUM_VPSS           8//per chn
#define MOSAIC_MAX_NUM_VPSS            4

#define RGN_VGS_TASK_WIDTH_MAX         8192

#define VENC_MAX_SSE_NUM               8
#define TS_MAX_SENSOR_NUM            2

/* For VI */
/* number of channle and device on video input unit of chip
 * Note! VI_MAX_CHN_NUM is NOT equal to VI_MAX_DEV_NUM
 * multiplied by VI_MAX_CHN_NUM, because all VI devices
 * can't work at mode of 4 channles at the same time.
 */
#define VI_MAX_PHY_PIPE_NUM            2
#define VI_MAX_PIPE_NUM                VI_MAX_PHY_PIPE_NUM

#define VI_MAX_PHY_CHN_NUM             1
#define VI_MAX_CHN_NUM                 VI_MAX_PHY_CHN_NUM

#define ISP_MAX_PIPE_NUM               VI_MAX_PIPE_NUM

#define ISP_AAC_BLOCK_ROW               15
#define ISP_AAC_BLOCK_COLUMN               15

#define VI_PIPE_OFFLINE_MIN_WIDTH      128
#define VI_PIPE_OFFLINE_MIN_HEIGHT     128
#define VI_PIPE_OFFLINE_MAX_WIDTH      2688
#define VI_PIPE_OFFLINE_MAX_HEIGHT     1944

#define VI_PIPE_ONLINE_MIN_WIDTH       128
#define VI_PIPE_ONLINE_MIN_HEIGHT      128
#define VI_PIPE_ONLINE_MAX_WIDTH       2688
#define VI_PIPE_ONLINE_MAX_HEIGHT      1944

#define VI_PHYCHN_OFFLINE_MIN_WIDTH    128
#define VI_PHYCHN_OFFLINE_MIN_HEIGHT   128
#define VI_PHYCHN_OFFLINE_MAX_WIDTH    2688
#define VI_PHYCHN_OFFLINE_MAX_HEIGHT   1944

#define VI_PHYCHN_ONLINE_MIN_WIDTH     128
#define VI_PHYCHN_ONLINE_MIN_HEIGHT    128
#define VI_PHYCHN_ONLINE_MAX_WIDTH     2688
#define VI_PHYCHN_ONLINE_MAX_HEIGHT    1944

#define VI_PHY_CHN_ONLINE_MAX_ZOOMIN 	1
#define VI_PHY_CHN_ONLINE_MAX_ZOOMOUT 	4
#define VI_PHY_CHN_OFFLINE_MAX_ZOOMIN 	1
#define VI_PHY_CHN_OFFLINE_MAX_ZOOMOUT 	4

/* For AUDIO */
#define AI_DEV_MAX_NUM                 1
#define AO_DEV_MIN_NUM                 1
#define AO_DEV_MAX_NUM                 1
#define AIO_MAX_NUM                    1
#define AENC_MAX_CHN_NUM               1
#define ADEC_MAX_CHN_NUM               1

#define AI_MAX_CHN_NUM                 1
#define AO_MAX_CHN_NUM                 1
#define AO_SYSCHN_CHNID                AO_MAX_CHN_NUM

#define AIO_MAX_CHN_NUM                ((AO_MAX_CHN_NUM > AI_MAX_CHN_NUM) ? AO_MAX_CHN_NUM : AI_MAX_CHN_NUM)

/* For VPSS */
#define VPSS_IP_NUM                    1
#define VPSS0                          0

#define VPSS_MAX_GRP_NUM               3
#define VPSS_MAX_GRP_PIPE_NUM          1
#define VPSS_MAX_PHY_CHN_NUM           3

//adjust to 0, by desheng
#define VPSS_MAX_CHN_NUM               VPSS_MAX_PHY_CHN_NUM
#define VPSS_MIN_IMAGE_WIDTH           128
#define VPSS_MIN_IMAGE_HEIGHT          128
#define VPSS_MAX_IN_IMAGE_WIDTH        2688
#define VPSS_MAX_IMAGE_WIDTH           8192
#define VPSS_MAX_IMAGE_HEIGHT          8192
#define VPSS_MAX_ZOOMIN                30
#define VPSS_MAX_ZOOMOUT               15

#define SPREAD_MIN_IMAGE_WIDTH         640
#define SPREAD_MIN_IMAGE_HEIGHT        480

/* For VO */
#define VO_MAX_VIRT_DEV_NUM            0 /* max virtual dev num */
#define VO_MAX_PHY_DEV_NUM             1 /* max physical dev num */

#define VO_MIN_CHN_WIDTH               1080 /* channel minimal width */
#define VO_MIN_CHN_HEIGHT              1920 /* channel minimal height */
#define VO_MAX_ZOOM_RATIO              1000 /* max zoom ratio, 1000 means 100% scale */
#define VO_MAX_DEV_NUM                 1 /* max dev num */
#define VO_MAX_LAYER_NUM               4 /* max layer num */
#define VO_MAX_PRIORITY                1 /* max layer priority */
#define VO_MAX_CHN_NUM                 1 /* max chn num */
#define VO_MAX_LAYER_IN_DEV            1 /* max layer num of each dev */
#define VO_MAX_GRAPHICS_LAYER_NUM      1
#define VO_MAX_WBC_NUM                 0

/* For VGS */
#define VGS_IP_NUM                     1
#define VGS0                           0
#define VGS1                           1
#define VGS_MAX_COVER_NUM              1
#define VGS_MAX_OSD_NUM                1

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_DEFINES_H__ */

