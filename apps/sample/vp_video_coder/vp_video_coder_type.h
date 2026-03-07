//
// Created by 杨柳 on 2023/8/7.
//

#ifndef VP_VIDEO_CODER_VP_VIDEO_CODER_TYPE_H
#define VP_VIDEO_CODER_VP_VIDEO_CODER_TYPE_H

#include <stdint.h>
#include "vp_lock.h"

#define VP_VIDEO_OSD_MAX 16
#define VP_VIDEO_OSD_RECT_MAX 10
#define VP_VIDEO_OSD_MOSAIC_MAX 2

/**
 * 视频AOV监听回调函数
 *
 * @param state   [in]    AOV状态.0,睡眠 1.自唤醒 2,外部唤醒
 */
typedef void (*vp_video_aov_callback_f)(uint8_t state);

typedef enum {
    vp_video_chn_main = 0, // 主码流通道
    vp_video_chn_sec = 1, // 子码流通道
    vp_video_chn_thr = 2, // 次码流通道
    vp_video_chn_max,
} vp_video_chn_t;

typedef enum {
    vp_video_mode_day, // 白天模式
    vp_video_mode_night, // 夜晚模式
    vp_video_mode_max,
} vp_video_mode_t;

typedef enum {
    vp_video_encoder_format_none = 0,
    vp_video_encoder_format_h264 = 1,
    vp_video_encoder_format_h265 = 2,
    vp_video_encoder_format_jpeg = 3,
    vp_video_encoder_format_max,
} vp_video_encoder_format_t;

typedef enum {
    VP_VIDEO_DETECT_AREA_MOTION = 0,  //移动侦测
    VP_VIDEO_DETECT_AREA_HUMAN,       //人形检测
    VP_VIDEO_DETECT_AREA_MAX = 8,
} vp_video_detect_area_type_t;

typedef enum {
    VP_VIDEO_PIXEL_FORMAT_YUV420P,   /**< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples) */
    VP_VIDEO_PIXEL_FORMAT_YUYV422,   /**< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr */
    VP_VIDEO_PIXEL_FORMAT_UYVY422,   /**< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1 */
    VP_VIDEO_PIXEL_FORMAT_YUV422P,   /**< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples) */
    VP_VIDEO_PIXEL_FORMAT_YUV444P,   /**< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples) */
    VP_VIDEO_PIXEL_FORMAT_YUV410P,   /**< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples) */
    VP_VIDEO_PIXEL_FORMAT_YUV411P,   /**< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) */
    VP_VIDEO_PIXEL_FORMAT_GRAY8,     /**<	   Y	    ,  8bpp */
    VP_VIDEO_PIXEL_FORMAT_MONOWHITE, /**<	   Y	    ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb */
    VP_VIDEO_PIXEL_FORMAT_MONOBLACK, /**<	   Y	    ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb */

    VP_VIDEO_PIXEL_FORMAT_NV12,      /**< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V) */
    VP_VIDEO_PIXEL_FORMAT_NV21,      /**< as above, but U and V bytes are swapped */

    VP_VIDEO_PIXEL_FORMAT_RGB24,     /**< packed RGB 8:8:8, 24bpp, RGBRGB... */
    VP_VIDEO_PIXEL_FORMAT_BGR24,     /**< packed RGB 8:8:8, 24bpp, BGRBGR... */

    VP_VIDEO_PIXEL_FORMAT_ARGB,      /**< packed ARGB 8:8:8:8, 32bpp, ARGBARGB... */
    VP_VIDEO_PIXEL_FORMAT_RGBA,         /**< packed RGBA 8:8:8:8, 32bpp, RGBARGBA... */
    VP_VIDEO_PIXEL_FORMAT_ABGR,         /**< packed ABGR 8:8:8:8, 32bpp, ABGRABGR... */
    VP_VIDEO_PIXEL_FORMAT_BGRA,         /**< packed BGRA 8:8:8:8, 32bpp, BGRABGRA... */

    VP_VIDEO_PIXEL_FORMAT_RGB565BE,  /**< packed RGB 5:6:5, 16bpp, (msb)	  5R 6G 5B(lsb), big-endian */
    VP_VIDEO_PIXEL_FORMAT_RGB565LE,  /**< packed RGB 5:6:5, 16bpp, (msb)	  5R 6G 5B(lsb), little-endian */
    VP_VIDEO_PIXEL_FORMAT_RGB555BE,  /**< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), big-endian, most significant bit to 0 */
    VP_VIDEO_PIXEL_FORMAT_RGB555LE,  /**< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), little-endian, most significant bit to 0 */

    VP_VIDEO_PIXEL_FORMAT_BGR565BE,  /**< packed BGR 5:6:5, 16bpp, (msb)	 5B 6G 5R(lsb), big-endian */
    VP_VIDEO_PIXEL_FORMAT_BGR565LE,  /**< packed BGR 5:6:5, 16bpp, (msb)	 5B 6G 5R(lsb), little-endian */
    VP_VIDEO_PIXEL_FORMAT_BGR555BE,  /**< packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), big-endian, most significant bit to 1 */
    VP_VIDEO_PIXEL_FORMAT_BGR555LE,  /**< packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), little-endian, most significant bit to 1 */

    VP_VIDEO_PIXEL_FORMAT_0RGB,      /**< packed RGB 8:8:8, 32bpp, 0RGB0RGB... */
    VP_VIDEO_PIXEL_FORMAT_RGB0,         /**< packed RGB 8:8:8, 32bpp, RGB0RGB0... */
    VP_VIDEO_PIXEL_FORMAT_0BGR,         /**< packed BGR 8:8:8, 32bpp, 0BGR0BGR... */
    VP_VIDEO_PIXEL_FORMAT_BGR0,         /**< packed BGR 8:8:8, 32bpp, BGR0BGR0... */

    VP_VIDEO_PIXEL_FORMAT_BAYER_BGGR8,    /**< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples */
    VP_VIDEO_PIXEL_FORMAT_BAYER_RGGB8,    /**< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples */
    VP_VIDEO_PIXEL_FORMAT_BAYER_GBRG8,    /**< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples */
    VP_VIDEO_PIXEL_FORMAT_BAYER_GRBG8,    /**< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples */

    VP_VIDEO_PIXEL_FORMAT_RAW,

    VP_VIDEO_PIXEL_FORMAT_HSV,

    VP_VIDEO_PIXEL_FORMAT_NB,
    VP_VIDEO_PIXEL_FORMAT_YUV422,
    VP_VIDEO_PIXEL_FORMAT_YVU422,
    VP_VIDEO_PIXEL_FORMAT_UVY422,
    VP_VIDEO_PIXEL_FORMAT_VUY422,
    VP_VIDEO_PIXEL_FORMAT_RAW8,
    VP_VIDEO_PIXEL_FORMAT_RAW16,
} vp_video_pixel_format_t;

typedef struct {
    /* 帧宽 */
    uint32_t width;

    /* 帧高 */
    uint32_t height;

    /* 帧时间戳 单位微秒(us)*/
    uint64_t timestamp;

    /* 帧数据 */
    uint8_t *frame_data;

    /* 帧数据长度 */
    uint32_t frame_size;

    /* 私有流信息,注意不是指向的帧数据 */
    void *priv_data;
} vp_video_source_t;

typedef struct {
    /* 帧序号 */
    uint32_t frame_no;

    /* 帧时间戳 单位微秒(us)*/
    uint64_t timestamp;

    /* 帧时长 单位微秒(us)*/
    uint32_t duration;

    /* 当前帧模式,0为白天模式 1为夜视模式 */
    uint8_t frame_mode;

    /* 当前帧触发事件 */
    uint8_t frame_event;

    /* 关键帧标识,1为关键帧 0为普通帧 */
    uint8_t frame_key;

    /* 帧图像格式 */
    vp_video_encoder_format_t format;

    /* 帧数据长度 */
    uint32_t frame_size;

    /* 帧数据偏移长度 */
    uint32_t frame_offset;

    /* 私有流信息,注意不是指向的帧数据 */
    void *priv_data;
} vp_video_coder_frame_t;


typedef struct vp_video_encoder_config {

    /**
     * 使能标识
     */
    uint8_t enable;
    /**
     * 图像数据通道
     */
    uint8_t chn;
    /**
     * 图像宽度
     */
    uint16_t width;

    /**
     * 图像高度
     */
    uint16_t height;

    /**
     * 图像大小
     */
    uint32_t frame_size;
    /**
     * 编码类型
     */
    vp_video_encoder_format_t format;

    /**
     * 帧率
     */
    uint8_t fps;

    /**
     * 码率,单位Kb/s
     */
    uint16_t bitrate;

    /**
     * 是否启用JPEG编码
     */
    uint8_t enable_jpeg;
    /**
     * 是否支持AOV功能
     */
    uint8_t enable_aov;
} vp_video_encoder_config_t;

typedef struct {
    /* 编码器类型 */
    vp_video_encoder_format_t format;
    /* 编码特征数据 */
    uint8_t *data;
    /* 编码特征数据大小 */
    uint32_t size;
    /* 仅H265具有该信息 */
    uint8_t *vps;
    uint32_t vps_size;
    /* H264和H265具有该信息 */
    uint8_t *sps;
    uint32_t sps_size;
    /* H264和H265具有该信息 */
    uint8_t *pps;
    uint32_t pps_size;
} vp_video_encoder_specific_t;

typedef enum {
    /* 大号字体 */
    VP_VIDEO_OSD_FONT_LARGE,

    /* 中号字体 */
    VP_VIDEO_OSD_FONT_MEDIUM,

    /* 小号字体 */
    VP_VIDEO_OSD_FONT_SMALL,

    /* 超大号字体 */
    VP_VIDEO_OSD_FONT_SUPER_LARGE,
} vp_osd_font_t;

typedef union {
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };
    uint32_t value;
} vp_osd_font_color_t;

typedef enum {
    VP_OSD_RECT_RED,        /* red */
    VP_OSD_RECT_BLACK,      /* black */
    VP_OSD_RECT_GREEN,      /* green */
    VP_OSD_RECT_YELLOW,     /* yellow */
} vp_osd_rect_color_t;

typedef enum {
    VP_VIDEO_OSD_TYPE_TEXT = 0,
    VP_VIDEO_OSD_TYPE_TIME = 1,
    VP_VIDEO_OSD_TYPE_RECT = 2,
    VP_VIDEO_OSD_TYPE_MOSAIC = 3,
    VP_VIDEO_OSD_TYPE_MAX
} vp_osd_type_t;

typedef struct {
    /* OSD 字体大小 */
    vp_osd_font_t font;
    /* OSD 颜色 */
    vp_osd_font_color_t color;
    /* OSD 字符串 */
    char text[128];
    /* OSD 图像数据 */
    uint8_t *data;
    /* OSD 图像数据大小 */
    uint32_t data_size;
} vp_osd_text_t;

typedef struct {
    /* OSD 颜色 */
    vp_osd_rect_color_t color;
    /* 矩形线宽 */
    uint32_t line;
} vp_osd_rect_t;

typedef struct {
    void *handle;
    /* OSD 是否创建 */
    uint8_t create: 2;
    /* OSD 是否显示 */
    uint8_t show: 2;
    /* OSD 是否更新 */
    uint8_t update: 2;
    uint8_t temp: 2;
    uint8_t osd_standard: 4;
    uint8_t osd_mode: 4;    // 0: 12h  1:24h
    /* 上次绘制时间 */
    uint32_t timestamp;
    /* OSD类型 */
    vp_osd_type_t type;

    /* OSD左上X偏移坐标 */
    uint32_t x;
    /* OSD左上Y偏移坐标 */
    uint32_t y;
    /* OSD宽度 */
    uint32_t width;
    /* OSD高度 */
    uint32_t height;
    vp_lock_t lock;
    union {
        vp_osd_text_t text;
        vp_osd_rect_t rect;
    };
} vp_osd_item_t;

typedef struct {
    /* OSD 字体大小 */
    vp_osd_font_t font;
    /* OSD 颜色 */
    vp_osd_font_color_t color;
    /* OSD左上X偏移坐标 */
    uint32_t x;
    /* OSD左上Y偏移坐标 */
    uint32_t y;
} vp_osd_config_text_t;

typedef struct {
    /* OSD 颜色 */
    vp_osd_rect_color_t color;
    /* 矩形线宽 */
    uint32_t line;
} vp_osd_config_rect_t;


#endif //VP_VIDEO_CODER_VP_VIDEO_CODER_TYPE_H
