//
// Created by 杨柳 on 2024/8/12.
//

#ifndef VP_VIDEO_CODER_VP_VIDEO_OSD_H
#define VP_VIDEO_CODER_VP_VIDEO_OSD_H

#include "vp_video_coder_type.h"
#include <string.h>
#include "sample_comm.h"
#include "vp_events.h"
#include "vp_video_encoder.h"
#include "vp_printf.h"
#include "pthread.h"
#include "vp_osd/vp_font.h"

#define OSD_ROI_DOWN_ALIGN16(_x) ((_x) & ~0xf)
#define OSD_ROI_UP_ALIGN16(_x) (((_x) + 0xf) & ~0xf)

static pthread_mutex_t g_osd_mutex = PTHREAD_MUTEX_INITIALIZER;

static int vp_video_osd_roi_set(VENC_CHN s32Chn, int s32Enable, int s32Index, int s32Qp, RECT_S *pstRoiRect)
{
    int ret = -1;
    if(pstRoiRect == NULL || s32Chn < 0 || s32Chn >= 4)
    {
        vp_error("prarm error!\r\n");
        return -1;
    }
    VENC_ROI_ATTR_S stRoiAttr = { 0 };

    memset(&stRoiAttr, 0, sizeof(VENC_ROI_ATTR_S));
    stRoiAttr.bEnable           = s32Enable;
    stRoiAttr.u32Index          = s32Index;
    stRoiAttr.bAbsQp            = 1;
    stRoiAttr.s32Qp             = s32Qp;
    stRoiAttr.stRect.s32X       = pstRoiRect->s32X;
    stRoiAttr.stRect.s32Y       = pstRoiRect->s32Y;
    stRoiAttr.stRect.u32Width   = pstRoiRect->u32Width;
    stRoiAttr.stRect.u32Height  = pstRoiRect->u32Height;

    ret = TS_MPI_VENC_SetRoiAttr(s32Chn, &stRoiAttr);
    if(ret) {
        vp_error("bsd osd roi set ok!\n");
        return -1;
    }

    //vp_error("[chn=%d][x:y=%d:%d][w:h=%d:%d]\n", s32Chn, stRoiAttr.stRect.s32X, stRoiAttr.stRect.s32Y,
    //    stRoiAttr.stRect.u32Width, stRoiAttr.stRect.u32Height);

    return TS_SUCCESS;
}

static int vp_video_osd_create(vp_channel_config_t *channel, vp_osd_item_t *item) {
    if (channel == NULL || item == NULL || 0 == item->handle) return -1;
    int ret = 0;
    RGN_HANDLE osd_handle = (RGN_HANDLE)item->handle;
    pthread_mutex_lock(&g_osd_mutex);
    MPP_CHN_S stChn;
    stChn.enModId = TS_ID_VPSS;
    stChn.s32DevId = channel->osd_config.dev;
    stChn.s32ChnId = channel->osd_config.chn;

    RGN_ATTR_S region = {
        .enType = OVERLAY_RGN,
        .unAttr = {
            .stOverlay = {
                .enPixelFmt = PIXEL_FORMAT_ARGB_2BIT,
                .u32BgColor = 0,
                .stSize = {
                    .u32Width = item->width,
                    .u32Height = item->height,
                },
                .u32CanvasNum = 2,
            },
        },
    };
    
    RGN_CHN_ATTR_S chn_attr = { 0 };

    chn_attr.bShow = TS_TRUE;
    chn_attr.enType = OVERLAY_RGN;
    chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = item->x;
    chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = item->y;
    chn_attr.unChnAttr.stOverlayChn.u32BgAlpha = 0;
    chn_attr.unChnAttr.stOverlayChn.u32FgAlpha = 0;
    chn_attr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = TS_FALSE;
    chn_attr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = TS_TRUE;
    chn_attr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 30;
    chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
    chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 16;
    chn_attr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;

    ret = TS_MPI_RGN_BatchBegin(stChn.s32DevId, stChn.s32ChnId);
    if (ret) {
        vp_error("TS_MPI_RGN_BatchBegin failed %d!\n", ret);
        pthread_mutex_unlock(&g_osd_mutex);
        return -1;
    }

    ret = TS_MPI_RGN_Create(osd_handle, &region);
    if (ret) {
        vp_error("TS_MPI_RGN_Create %d failed %d!\n", osd_handle, ret);
        pthread_mutex_unlock(&g_osd_mutex);
        return -1;
    }

    ret = TS_MPI_RGN_AttachToChn(osd_handle, &stChn, &chn_attr);
    if (ret) {
        vp_error("TS_MPI_RGN_AttachToChn %d failed %d!\n", osd_handle, ret);
        pthread_mutex_unlock(&g_osd_mutex);
        return -1;
    }

    ret = TS_MPI_RGN_BatchEnd(stChn.s32DevId, stChn.s32ChnId);
    if (ret) {
        vp_error("TS_MPI_RGN_BatchEnd failed %d!\n", ret);
        pthread_mutex_unlock(&g_osd_mutex);
        return -1;
    }

    if (item->type == VP_VIDEO_OSD_TYPE_TIME) {
        RECT_S stRoiRect = { 0 };
        memset(&stRoiRect, 0, sizeof(RECT_S));
        stRoiRect.s32X      = OSD_ROI_DOWN_ALIGN16(item->x);
        stRoiRect.s32Y      = OSD_ROI_DOWN_ALIGN16(item->y);
        stRoiRect.u32Width  = OSD_ROI_UP_ALIGN16(item->width);
        stRoiRect.u32Height = OSD_ROI_UP_ALIGN16(item->height);
        
        if (channel->enc_chn % 2 == 0)
            ret = vp_video_osd_roi_set(channel->enc_chn, 1, 0, 28, &stRoiRect);
        else 
            ret = vp_video_osd_roi_set(channel->enc_chn, 1, 0, 25, &stRoiRect);
        if(ret)
        {
            vp_error("osd roi set fail!\n");
        }
    }

    pthread_mutex_unlock(&g_osd_mutex);
    return 0;
}

static int vp_video_mosaic_create(vp_channel_config_t *channel, vp_osd_item_t *item) {
    if (channel == NULL || item == NULL || 0 == item->handle) return -1;
    int ret = 0;
    RGN_HANDLE mosaic_handle = (RGN_HANDLE)item->handle;
    MPP_CHN_S stChn;
    stChn.enModId = TS_ID_VPSS;
    stChn.s32DevId = channel->osd_config.dev;
    stChn.s32ChnId = channel->osd_config.chn;

    RGN_ATTR_S stRegion;
    memset(&stRegion, 0, sizeof(stRegion));
	stRegion.enType = MOSAIC_RGN;
    ret = TS_MPI_RGN_Create(mosaic_handle, &stRegion);
    if (ret) {
        vp_error("Failed to call TS_MPI_RGN_Create(err-%d).\n", ret);
        return ret;
    }

    RGN_CHN_ATTR_S stChnAttr;
    memset(&stChnAttr, 0, sizeof(stChnAttr));
    stChnAttr.bShow = TS_FALSE;
    stChnAttr.enType = MOSAIC_RGN;
    stChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_32;
    stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = item->x;
    stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = item->y;
    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = item->height;
    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width  = item->width;
    stChnAttr.unChnAttr.stMosaicChn.u32Layer = mosaic_handle;

    ret = TS_MPI_RGN_AttachToChn(mosaic_handle, &stChn, &stChnAttr);
    if (ret) {
        vp_error("Failed to call TS_MPI_RGN_AttachToChn(err-%d).\n", ret);
        return ret;
    }

    return 0;
}

static void vp_video_osd_destroy(vp_channel_config_t *channel, vp_osd_item_t *item) {
    if (channel == NULL || item == NULL || 0 == item->handle) return;

    RGN_HANDLE osd_handle = (RGN_HANDLE)item->handle;

    if (osd_handle) {
        MPP_CHN_S stChn = { 0 };
        stChn.enModId = TS_ID_VPSS;
        stChn.s32DevId = channel->osd_config.dev;
        stChn.s32ChnId = channel->osd_config.chn;

        int ret = TS_MPI_RGN_DetachFromChn(osd_handle, &stChn);
        if (ret) {
            vp_error("TS_MPI_RGN_DetachFromChn failed with:%d.\n", ret);
            return;
        }

        ret = TS_MPI_RGN_Destroy(osd_handle);
        if (ret) {
            vp_error("TS_MPI_RGN_Destroy failed with:%d.\n", ret);
            return;
        }
    }
}

static int vp_osd_get_osd_buffer(vp_channel_config_t *channel, vp_osd_item_t *item, RGN_CANVAS_INFO_S* stCanvasInfo) {
    if (channel == NULL || stCanvasInfo == NULL || item == NULL || item->create == 0) return -1;
    RGN_HANDLE osd_handle = (RGN_HANDLE)item->handle;

    int ret = TS_MPI_RGN_GetCanvasInfo(osd_handle, stCanvasInfo);

    return ret;
}

static int vp_osd_update_osd_buffer(vp_channel_config_t *channel, vp_osd_item_t *item) {
    if (channel == NULL || item == NULL || item->create == 0) return -1;
    RGN_HANDLE osd_handle = (RGN_HANDLE)item->handle;
    return TS_MPI_RGN_UpdateCanvas(osd_handle);
}

static int vp_osd_draw_text(vp_channel_config_t *channel, vp_osd_item_t *item, char *str) {
    if (channel == NULL || item == NULL || str == NULL) return -1;
    int ret = 0;
    uint8_t color = 3;
    uint32_t img_width = channel->fs_chn_attr.u32Width - item->x;
    uint32_t img_height = channel->fs_chn_attr.u32Height - item->y;
    uint32_t img_size = vp_font_draw_size(item->text.font, str, &img_width, &img_height, 0, 0);

    if (item->width < img_width || item->height < img_height) {
        vp_video_osd_destroy(channel, item);
        item->width = (img_width + 64) & ~63;
        item->height = img_height;
        vp_video_osd_create(channel, item);
    }

    RGN_CANVAS_INFO_S stCanvasInfo;
    memset(&stCanvasInfo, 0, sizeof(stCanvasInfo));
    ret = vp_osd_get_osd_buffer(channel, item, &stCanvasInfo);
    if (0 == ret) {
        uint8_t *img = calloc(img_size, 1);
        if (NULL == img) return -1;
        memset((uint8_t*)(TS_UL)(stCanvasInfo.u64VirtAddr), 0, item->width * item->height >> 2);
        ret = vp_font_draw_text(item->text.font, str, img, color, img_width, img_height, 0, 0, (uint8_t*)(TS_UL)(stCanvasInfo.u64VirtAddr), 0, 0, item->width, item->height);
        free(img);

        if (0 == ret) {
            ret = vp_osd_update_osd_buffer(channel, item);
        }
    }

    return ret;
}

static uint8_t vp_osd_color(vp_osd_rect_color_t color) {
    switch (color) {
        case VP_OSD_RECT_RED:
            return 1;
        case VP_OSD_RECT_BLACK:
            return 3;
        case VP_OSD_RECT_GREEN:
            return 2;
        default:
            return 3;
    }

    return 3;
}

static int vp_osd_fill_rect(uint8_t* osd_buffer, uint32_t width, uint32_t hight, vp_osd_rect_color_t rect_color, uint32_t x, uint32_t y, uint32_t draw_width, uint32_t draw_height) {
    uint8_t color = vp_osd_color(rect_color);
    uint32_t pos = 0;
    uint32_t row_pos = 0;
    osd_buffer += ((y * width + x) >> 2);
    for (uint32_t h = 0; h < draw_height; ++h) {
        row_pos = h * width;
        for (uint32_t w = 0; w < draw_width; ++w) {
            // pos =  ((y + h) * width + x + w);
            pos = row_pos + w;
            osd_buffer[pos >> 2] |= (color << ((pos & 3) << 1));
        }
    }
    return 0;
}

static int vp_osd_draw_rect(vp_channel_config_t *channel, vp_osd_item_t *item, uint8_t* osd_buffer) {
    if (channel == NULL || item == NULL || osd_buffer == NULL) return -1;

    if (item->width == 0 || item->height == 0) return 0;

    if (item->x + item->width > channel->fs_chn_attr.u32Width) {
        vp_error("rect with is over range.\n");
        return -1;
    }

    if (item->y + item->height > channel->fs_chn_attr.u32Height) {
        vp_error("rect height(y-%d, height-%d, chlheight-%d) is over range.\n", item->y, item->height, channel->fs_chn_attr.u32Height);
        return -1;
    }

    #if 0 // FILL rect
    vp_osd_fill_rect(osd_buffer, channel->fs_chn_attr.u32Width, channel->fs_chn_attr.u32Height, item->rect.color, item->x, item->y, item->width, item->height);
    #else // draw rect
    // draw left 
    uint32_t line_width = ((item->rect.line >> 1) + (item->rect.line & 1)) << 1;
    uint32_t x = item->x;
    uint32_t y = item->y;
    uint32_t draw_width = line_width > item->width ? item->width : line_width;
    uint32_t draw_height = item->height;
    vp_osd_fill_rect(osd_buffer, channel->fs_chn_attr.u32Width, channel->fs_chn_attr.u32Height, item->rect.color, x, y, draw_width, draw_height);

    //draw right
    if (line_width < item->width) {
        x = item->x + item->width - line_width;
        y = item->y;
        vp_osd_fill_rect(osd_buffer, channel->fs_chn_attr.u32Width, channel->fs_chn_attr.u32Height, item->rect.color, x, y, draw_width, draw_height);
    }

    //draw top
    x = item->x;
    y = item->y;
    draw_height = line_width > item->height ? item->height : line_width;
    draw_width = item->width;
    vp_osd_fill_rect(osd_buffer, channel->fs_chn_attr.u32Width, channel->fs_chn_attr.u32Height, item->rect.color, x, y, draw_width, draw_height);

    //draw bottom
    if (line_width < item->height) {
        y = item->y + item->height - line_width;
        x = item->x;
        vp_osd_fill_rect(osd_buffer, channel->fs_chn_attr.u32Width, channel->fs_chn_attr.u32Height, item->rect.color, x, y, draw_width, draw_height);
    }

    #endif

    return 0;
}

static void *vp_osd_update_fn(void *args) {
    vp_channel_config_t *channel = (vp_channel_config_t *) args;
    vp_pthread_name("vp_osd");
    uint64_t timestamp, start, end, bits, next;
    char text[256] = { 0 };
    struct tm now_tm;
    int save_timezone = vp_timezone_get();
    RGN_CANVAS_INFO_S stCanvasInfo = { 0 };
    while (channel->osd_config.start_flag) {
        start = vp_system_time_s();
        next = (start + 1) * 1000;
        timestamp = vp_local_time_s();
        gmtime_r((const time_t *) &timestamp, &now_tm);

        for (int j = 0; j < VP_VIDEO_OSD_MAX; ++j) {
            vp_osd_item_t *item = &channel->osd_config.items[j];
            if (item->create == 0 || item->show == 0) continue;
            switch (item->type) {
                case VP_VIDEO_OSD_TYPE_TEXT:
                    if (item->update) {
                        vp_lock(&item->lock);
                        strncpy(text, item->text.text, sizeof(text) - 1);
                        item->update = 0;
                        vp_osd_draw_text(channel, item, text);
                        vp_unlock(&item->lock);
                    }
                    break;
                case VP_VIDEO_OSD_TYPE_TIME:
                    if (item->update || save_timezone != vp_timezone_get()) {
                        save_timezone = vp_timezone_get();
                        gmtime_r((const time_t *) &item->timestamp, &now_tm);
                    } else if (timestamp <= item->timestamp || channel->osd_config.pause_flag) {
                        break;
                    }
                    vp_lock(&item->lock);
                    strftime(text, sizeof(text), item->text.text, &now_tm);
                    vp_osd_draw_text(channel, item, text);
                    item->timestamp = timestamp;
                    if (item->update) {
                        vp_events_send(channel->osd_config.events, VP_EVENT_BIT2);
                    }
                    item->update = 0;
                    vp_unlock(&item->lock);
                    break;
                case VP_VIDEO_OSD_TYPE_RECT:
                    if (item->update) {
                        item->update = 0;
                        //vp_osd_draw_rect(channel, item, osd_buffer);
                    }
                    break;
                case VP_VIDEO_OSD_TYPE_MAX:
                    break;
            }
        }

        bits = VP_EVENT_BIT0 | VP_EVENT_BIT1;
        end = vp_system_time_ms();
        if (end < next) {
            int ret = vp_events_wait(channel->osd_config.events, &bits, VP_EVENTS_FLAG_OR_CLEAR, next - end);
            if (ret && ret != -2) {
                vp_error("vp_events_wait error:%d", ret);
                break;
            }
            if (bits & VP_EVENT_BIT1) break;
        }
    }

    for (int j = 0; j < VP_VIDEO_OSD_MAX; ++j) {
        vp_osd_item_t *item = &channel->osd_config.items[j];
        if (item->create == 0) continue;
        item->create = 0;
        switch (item->type) {
            case VP_VIDEO_OSD_TYPE_TEXT:
            case VP_VIDEO_OSD_TYPE_TIME: {
                vp_lock(&item->lock);
                if (item->text.data) free(item->text.data);
                item->text.data = NULL;
                vp_unlock(&item->lock);
                vp_video_osd_destroy(channel, item);
                vp_lock_deinit(&item->lock);
                item->create = 0;
                }
                break;
            case VP_VIDEO_OSD_TYPE_RECT:
            case VP_VIDEO_OSD_TYPE_MAX:
                break;
        }
    }

    pthread_detach(pthread_self());
    return NULL;
}

static int vp_video_osd_ctrl(vp_osd_item_t *item, int show) {
    if (NULL == item) return -1;
    item->show = show;
    item->update = 1;
    return 0;
}

/**
 * 初始化视频OSD水印
 *
 * @param idx       [in]    视频编号,与 Sensor 对应
 * @param chn       [in]    视频流通道
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_video_osd_init(uint8_t idx, vp_video_chn_t chn) {
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (!channel->enable) return -1;
    if (channel->osd_config.init_flag) return 0;
    memset(&channel->osd_config, 0, sizeof(channel->osd_config));
    channel->osd_config.events = vp_events_create();
    channel->osd_config.dev = idx;
    channel->osd_config.chn = chn;
    channel->osd_config.init_flag = 1;

    VPSS_OVERLAY_LUT_S pstChnLut;
    TS_MPI_VPSS_GetChnOverlayLut(idx, chn, &pstChnLut);
    pstChnLut.pixel_2bit[0].alpha = 0x0;

    //1 - red
    pstChnLut.pixel_2bit[1].alpha = 0xff;
    pstChnLut.pixel_2bit[1].value_R = 0x0;
    pstChnLut.pixel_2bit[1].value_G = 0x0;
    pstChnLut.pixel_2bit[1].value_B = 0x0;

    //2 - green
    pstChnLut.pixel_2bit[2].alpha = 0xff;
    pstChnLut.pixel_2bit[2].value_R = 0x0;
    pstChnLut.pixel_2bit[2].value_G = 0xff;
    pstChnLut.pixel_2bit[2].value_B = 0x0;

    //3 - white
    pstChnLut.pixel_2bit[3].alpha = 0xff;
    pstChnLut.pixel_2bit[3].value_R = 0xff;
    pstChnLut.pixel_2bit[3].value_G = 0xff;
    pstChnLut.pixel_2bit[3].value_B = 0xff;
    TS_MPI_VPSS_SetChnOverlayLut(idx, chn, &pstChnLut);
    return 0;
}

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
int vp_video_osd_get_item(uint8_t idx, vp_video_chn_t chn, uint8_t index, vp_osd_item_t **item) {
    if (index >= VP_VIDEO_OSD_MAX) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    *item = &channel->osd_config.items[index];
    return 0;
}

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
                             vp_osd_config_text_t *config, const char *str) {
    if (index >= 4) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[index];
    int ret = 0;
    if (index == 0) item->type = VP_VIDEO_OSD_TYPE_TIME;
    else item->type = VP_VIDEO_OSD_TYPE_TEXT;

    item->x = config->x;
    item->y = config->y;
    item->text.font = config->font;
    item->text.color = config->color;
    item->handle = idx * VP_VIDEO_CHANNEL_MAX * VP_VIDEO_OSD_MAX + chn * VP_VIDEO_OSD_MAX + index + 1;
    item->width = 64;
    item->height = 64;
    
    if (0 == item->create) {
        vp_lock_init(&item->lock);
        ret = vp_video_osd_create(channel, item);
        if (ret == 0) {
            item->create = 1;
        } else {
            vp_error("Failed to create osd text, ret=%d\n", ret);
            return ret;
        }
    }

    item->x = config->x;
    item->y = config->y;
    item->text.font = config->font;
    item->text.color = config->color;
    memset(item->text.text, 0, sizeof(item->text.text));
    strncpy(item->text.text, str, sizeof(item->text.text) - 1);

    return ret;
}

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
int vp_video_osd_config_rect(uint8_t idx, vp_video_chn_t chn, uint8_t index, vp_osd_config_rect_t *config) {
    if (index >= 10 || NULL == config) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[4 + index];
    int ret;
    item->type = VP_VIDEO_OSD_TYPE_RECT;

    if (0 == item->create) {
        item->create = 1;
        vp_lock_init(&item->lock);
    }

    item->rect.line = config->line;
    item->rect.color = config->color;
    return 0;
}

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
int vp_video_osd_config_mosaic(uint8_t idx, vp_video_chn_t chn, uint8_t index, vp_osd_config_rect_t *config) {
    if (index >= 10 || NULL == config) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[14 + index];
    int ret;
    item->type = VP_VIDEO_OSD_TYPE_MOSAIC;

    item->x = 0;
    item->y = 0;
    item->handle = idx * VP_VIDEO_CHANNEL_MAX * VP_VIDEO_OSD_MAX + chn * VP_VIDEO_OSD_MAX + index + 15;
    // 设置默认有效尺寸，避免创建时传入0导致驱动错误
    item->width = 32;
    item->height = 32;

    if (0 == item->create) {
        vp_lock_init(&item->lock);
        ret = vp_video_mosaic_create(channel, item);
        if (ret == 0) {
            item->create = 1;
        } else {
            vp_error("Failed to create mosaic, ret=%d\n", ret);
            return ret;
        }
    }

    return 0;
}

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
int vp_video_osd_start(uint8_t idx, vp_video_chn_t chn) {
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    if (channel->osd_config.start_flag) return 0;

    channel->osd_config.start_flag = 1;
    pthread_t pt;
    int ret = vp_pthread_create(&pt, 64 * 1024, vp_osd_update_fn, channel);
    if (ret) {
        channel->osd_config.start_flag = 0;
        // vp_debug("create osd thread error:%d", ret);
        return -1;
    }
    return 0;
}

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
int vp_video_osd_pause(uint8_t idx, vp_video_chn_t chn) {
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    channel->osd_config.pause_flag = 1;
    return 0;
}

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
int vp_video_osd_resume(uint8_t idx, vp_video_chn_t chn) {
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    channel->osd_config.pause_flag = 0;
    return 0;
}

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
int vp_video_osd_show(uint8_t idx, vp_video_chn_t chn, uint8_t index) {
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.start_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[index];
    if (item->create == 0) return -1;
    if (item->show) return 0;

    vp_lock(&item->lock);
    int ret = vp_video_osd_ctrl(item, 1);
    vp_unlock(&item->lock);

    if (index > 13) {
        MPP_CHN_S stChn = { 0 };
        stChn.enModId = TS_ID_VPSS;
        stChn.s32DevId = channel->osd_config.dev;
        stChn.s32ChnId = channel->osd_config.chn;

        RGN_CHN_ATTR_S stChnAttr;
        memset(&stChnAttr, 0, sizeof(stChnAttr));
        ret = TS_MPI_RGN_GetDisplayAttr(item->handle, &stChn, &stChnAttr);
        if (ret) {
            vp_error("Failed to call TS_MPI_RGN_GetDisplayAttr.\n");
            return ret;
        }

        if (stChnAttr.bShow == TS_FALSE) {
            stChnAttr.bShow = TS_TRUE;
            ret = TS_MPI_RGN_SetDisplayAttr(item->handle, &stChn, &stChnAttr);

            if (ret) {
                vp_error("Failed to call TS_MPI_RGN_SetDisplayAttr.\n");
            }
        }
    }

    return ret;
}

/**
 * 设置OSD 12小时制
 *
 * @param idx       [in]     视频编号,与 Sensor 对应
 * @param chn       [in]     视频流通道
 * @param index     [in]     OSD编号:0~3
 *
 * @return 错误码
 * @retval =0 成功
 * @retval \<0 失败
 */
int vp_video_osd_mode(uint8_t idx, vp_video_chn_t chn, uint8_t index, uint8_t mode) {
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.start_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[index];
    if (item->create == 0) return -1;

    if (mode != item->osd_mode) {
        vp_lock(&item->lock);
        item->osd_mode = mode;
        memset(item->text.text, 0, sizeof(item->text.text));
        if (item->osd_mode == 1) {
            strncpy(item->text.text, "%Y-%m-%d %I:%M:%S %p", sizeof(item->text.text) - 1);
        } else {
            strncpy(item->text.text, "%Y-%m-%d %H:%M:%S", sizeof(item->text.text) - 1);
        }
        vp_unlock(&item->lock);
    }

    return 0;
}

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
int vp_video_osd_show_text(uint8_t idx, vp_video_chn_t chn, uint8_t index) {
    return vp_video_osd_show(idx, chn, index);
}

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
int vp_video_osd_show_rect(uint8_t idx, vp_video_chn_t chn, uint8_t index) {
    return vp_video_osd_show(idx, chn, 4 + index);
}

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
int vp_video_osd_show_mosaic(uint8_t idx, vp_video_chn_t chn, uint8_t index) {
    if (index >= 2) return -1;
    return vp_video_osd_show(idx, chn, 14 + index);
}

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
void vp_video_osd_show_all(uint8_t idx, vp_video_chn_t chn) {
    for (int i = 0; i < VP_VIDEO_OSD_MAX; ++i) {
        vp_video_osd_show(idx, chn, i);
    }
}

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
int vp_video_osd_update_timestamp(uint8_t idx, vp_video_chn_t chn, uint64_t timestamp) {
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[0];
    item->timestamp = timestamp;
    if (item->show) {
        item->update = 1;
        vp_events_send(channel->osd_config.events, VP_EVENT_BIT0);
    }
    return 0;
}

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
int vp_video_osd_update_timestamp_wait(uint8_t idx, vp_video_chn_t chn, uint64_t timestamp, uint32_t timeout) {
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[0];
    if (item->create == 0) return -1;
    vp_events_clear(channel->osd_config.events, VP_EVENT_BIT2);
    item->timestamp = timestamp;
    if (item->show) {
        item->update = 1;
        vp_events_send(channel->osd_config.events, VP_EVENT_BIT0);
        uint64_t bits = VP_EVENT_BIT2;
        vp_events_wait(channel->osd_config.events, &bits, VP_EVENTS_FLAG_OR_CLEAR, timeout);
    }
    return 0;
}

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
int vp_video_osd_update_text(uint8_t idx, vp_video_chn_t chn, uint8_t index, const char *str) {
    if (index >= 4) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[index];
    if (item->create == 0) return -1;

    vp_lock(&item->lock);
    memset(item->text.text, 0, sizeof(item->text.text));
    strncpy(item->text.text, str, sizeof(item->text.text) - 1);
    if (item->show) {
        item->update = 1;
        vp_events_send(channel->osd_config.events, VP_EVENT_BIT0);
    }
    vp_unlock(&item->lock);
    return 0;
}

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
                             uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if (index >= 10) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[4 + index];
    if (item->create == 0) return -1;

    vp_lock(&item->lock);
    item->x = x;
    item->y = y;
    item->width = width;
    item->height = height;
    vp_unlock(&item->lock);

    return 0;
}

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
                             uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if (index >= 10) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[14 + index];
    if (item->create == 0) return -1;
    uint32_t max_width = channel->fs_chn_attr.u32Width;
    uint32_t max_height = channel->fs_chn_attr.u32Height;

    int ret = 0;
    vp_lock(&item->lock);
    item->x = x & 0xffe0;
    item->y = y & 0xffe0;
    item->width = (width + 32) & 0xffe0;
    item->height = (height + 32) & 0xffe0;
    // 确保坐标和尺寸在有效范围内
    if (item->x >= max_width) item->x = max_width - 1;
    if (item->y >= max_height) item->y = max_height - 1;
    item->width = (item->x + item->width) >= max_width ? (max_width - item->x) : item->width;
    item->height = (item->y + item->height) >= max_height ? (max_height - item->y) : item->height;
    // 确保宽度和高度不为零
    if (item->width == 0 || item->height == 0) {
        vp_unlock(&item->lock);
        return -1;
    }
    vp_unlock(&item->lock);

    MPP_CHN_S stChn = { 0 };
    stChn.enModId = TS_ID_VPSS;
    stChn.s32DevId = channel->osd_config.dev;
    stChn.s32ChnId = channel->osd_config.chn;

    RGN_CHN_ATTR_S stChnAttr;
    memset(&stChnAttr, 0, sizeof(stChnAttr));
    ret = TS_MPI_RGN_GetDisplayAttr(item->handle, &stChn, &stChnAttr);
    if (ret) {
        vp_error("Failed to call TS_MPI_RGN_GetDisplayAttr(err_%d).\n", ret);
        return ret;
    }

    stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = item->x;
    stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = item->y;
    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = item->height;
    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width  = item->width;
    // vp_error("rect-%d-%d-%d-%d.\n", item->x, item->y, item->width, item->height);
    ret = TS_MPI_RGN_SetDisplayAttr(item->handle, &stChn, &stChnAttr);
    if (ret) {
        vp_error("Failed to call TS_MPI_RGN_SetDisplayAttr(ret-%d).\n", ret);
    }

    return ret;
}

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
int vp_video_osd_hide(uint8_t idx, vp_video_chn_t chn, uint8_t index) {
    if (index >= VP_VIDEO_OSD_MAX) return -1;
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.start_flag == 0) return -1;
    vp_osd_item_t *item = &channel->osd_config.items[index];
    if (item->create == 0) return -1;
    if (item->show == 0) return 0;

    int ret = vp_video_osd_ctrl(item, 0);

    if (index < 4) {
        RGN_CANVAS_INFO_S stCanvasInfo;
        memset(&stCanvasInfo, 0, sizeof(stCanvasInfo));
        ret = vp_osd_get_osd_buffer(channel, item, &stCanvasInfo);
        if (0 == ret) {
            memset((uint8_t*)(TS_UL)(stCanvasInfo.u64VirtAddr), 0, (item->width * item->height) >> 2);
        }

        ret = vp_osd_update_osd_buffer(channel, item);
    }

    if (index > 13) {
        MPP_CHN_S stChn = { 0 };
        stChn.enModId = TS_ID_VPSS;
        stChn.s32DevId = channel->osd_config.dev;
        stChn.s32ChnId = channel->osd_config.chn;

        RGN_CHN_ATTR_S stChnAttr;
        memset(&stChnAttr, 0, sizeof(stChnAttr));
        ret = TS_MPI_RGN_GetDisplayAttr(item->handle, &stChn, &stChnAttr);
        if (ret) {
            vp_error("Failed to call TS_MPI_RGN_GetDisplayAttr.\n");
            return ret;
        }

        if (stChnAttr.bShow) {
            stChnAttr.bShow = TS_FALSE;
            ret = TS_MPI_RGN_SetDisplayAttr(item->handle, &stChn, &stChnAttr);

            if (ret) {
                vp_error("Failed to call TS_MPI_RGN_SetDisplayAttr.\n");
            }
        }
    }

    return ret;
}

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
int vp_video_osd_hide_text(uint8_t idx, vp_video_chn_t chn, uint8_t index) {
    if (index >= 4) return -1;
    return vp_video_osd_hide(idx, chn, index);
}

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
int vp_video_osd_hide_rect(uint8_t idx, vp_video_chn_t chn, uint8_t index) {
    if (index >= 10) return -1;
    return vp_video_osd_hide(idx, chn, 4 + index);
}

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
int vp_video_osd_hide_mosaic(uint8_t idx, vp_video_chn_t chn, uint8_t index) {
    if (index >= 2) return -1;
    return vp_video_osd_hide(idx, chn, 14 + index);
}

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
void vp_video_osd_hide_all(uint8_t idx, vp_video_chn_t chn) {
    for (int i = 0; i < VP_VIDEO_OSD_MAX; ++i) {
        vp_video_osd_hide(idx, chn, i);
    }
    return;
}

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
int vp_video_osd_stop(uint8_t idx, vp_video_chn_t chn) {
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    if (channel->osd_config.start_flag == 0) return 0;
    channel->osd_config.start_flag = 0;
    vp_events_send(channel->osd_config.events, VP_EVENT_BIT1);
    return 0;
}

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
int vp_video_osd_deinit(uint8_t idx, vp_video_chn_t chn) {
    vp_channel_config_t *channel = vp_video_get_channel(idx, chn);
    if (NULL == channel) return -1;
    if (channel->osd_config.init_flag == 0) return -1;
    if (channel->osd_config.start_flag) vp_video_osd_stop(idx, chn);
    
    for (int i = 0; i < VP_VIDEO_OSD_MAX; ++i) {
        vp_osd_item_t *item = &channel->osd_config.items[i];
        if (item->create) {
            vp_video_osd_destroy(channel, item);
            item->create = 0;
            item->show = 0;
        }
    }
    
    channel->osd_config.init_flag = 0;
    if (channel->osd_config.events) {
        vp_events_destroy(channel->osd_config.events);
        channel->osd_config.events = NULL;
    }
    return 0;
}

#endif //VP_VIDEO_CODER_VP_VIDEO_OSD_H
