#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#include "sample_comm.h"
#include "vp_printf.h"
#include "vp_video_encoder.h"

#include "vp_video_soft_pho_sens.h"

#define RTSP 1

volatile sig_atomic_t running = 1;

static void handle_signal(int signum) {
    (void) signum;
    running = 0;
}

void *video_thread_func(void *args) {
    pthread_detach(pthread_self());
    int chn = (int) args;
    vp_debug("idx:0 chn:%d start", chn);
    char path[1024];
    sprintf(path, "/opt/stream_ch%d.h265", chn);
    vp_video_coder_frame_t frame;
    //uint8_t *data;
    //int data_size;

    FILE *file = fopen(path, "wb");
    while (running) {
        int ret = vp_video_encoder_request_frame(chn/2, chn%2, &frame);
        //if (ret < 0) break;
        if (ret == 1) continue;

        //while ((data_size = vp_video_encoder_frame_data(chn/2, chn%2, &frame, &data)) > 0) {
        //    if (file) fwrite(data, data_size, 1, file);
        //}

        vp_video_encoder_release_frame(chn/2, chn%2, &frame);

    }

    if (file) fclose(file);

    vp_debug("idx:0 chn:%d exit", chn);
    return NULL;
}

void show_osd() 
{
    const int vp_sensor_num = VP_SENSOR_NUM;
    const int channel_num = 2;

    vp_osd_config_text_t text = {
        .color = {.value = 0xFF000000},
        .font = VP_VIDEO_OSD_FONT_MEDIUM,
        .x = 8,
        .y = 8,
    };

    vp_osd_config_rect_t rect = {
        .line = 8,
    };
    
    //init
    for (int i = 0; i < vp_sensor_num; ++i) {
        for (int j = 0; j < channel_num; ++j) 
        {
            vp_video_osd_init(i, j);
        }
    }

    for (int i = 0; i < vp_sensor_num; ++i) {
        for (int j = 0; j < channel_num; ++j) 
        {
            text.font = VP_VIDEO_OSD_FONT_MEDIUM;

            text.y = 8;
            vp_video_osd_config_text(i, j, 0, &text, "%Y-%m-%d %H:%M:%S");
            // text.y = text.y + 50;
            // vp_video_osd_config_text(i, j, 1, &text, "中国电信: 100");
            // text.y = text.y + 50;
            // vp_video_osd_config_text(i, j, 2, &text, "电     量: 80%");
            // text.y = text.y + 50;
            // vp_video_osd_config_text(i, j, 3, &text, "流     量: 50%");

            rect.color = VP_OSD_RECT_RED;
            vp_video_osd_config_rect(i, j, 0, &rect);
            rect.color = VP_OSD_RECT_BLACK;
            vp_video_osd_config_rect(i, j, 1, &rect);
            rect.color = VP_OSD_RECT_GREEN;
            vp_video_osd_config_rect(i, j, 2, &rect);
            rect.color = VP_OSD_RECT_YELLOW;
            vp_video_osd_config_rect(i, j, 3, &rect);

            text.y = text.y + 50;
            vp_video_osd_update_rect(i, j, 0, text.x, text.y, 600, 20);
            text.y = text.y + 20;
            vp_video_osd_update_rect(i, j, 1, text.x, text.y, 600, 20);
            text.y = text.y + 20;
            vp_video_osd_update_rect(i, j, 2, text.x, text.y, 600, 20);
            text.y = text.y + 20;
            vp_video_osd_update_rect(i, j, 3, text.x, text.y, 600, 20);

            vp_video_osd_start(i, j);

            vp_video_osd_show_text(i, j, 0);
            // vp_video_osd_show_text(i, j, 1);
            // vp_video_osd_show_text(i, j, 2);
            // vp_video_osd_show_text(i, j, 3);

            vp_video_osd_show_rect(i, j, 0);
            vp_video_osd_show_rect(i, j, 1);
            vp_video_osd_show_rect(i, j, 2);
            vp_video_osd_show_rect(i, j, 3);

            text.y = text.y + 50;
        }
    }
}

static char get_input_dbg(void)
{
    FILE *file;
    char c;
    size_t ret;

    file = fopen("/tmp/input_dbg", "r");
    if (!file) return 0x0;

    ret = fread(&c, 1 ,1, file);
    if (ret != 1) {
        fclose(file);
        return 0x0;
    }

    fclose(file);
    remove("/tmp/input_dbg");
    vp_error("cmd = %c.\n", c);
    return c;
}

uint8_t jpeg_data[1024*100];

int main(int argc, char *argv[])
{
	int ret = 0;

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    vp_video_encoder_init(0);

    #if VP_SENSOR_NUM > 0
    vp_video_encoder_start(0, 0);
    vp_video_encoder_start(0, 1);
    #endif
    #if VP_SENSOR_NUM > 1
    vp_video_encoder_start(1, 0);
    vp_video_encoder_start(1, 1);
    #endif
    #if VP_SENSOR_NUM > 2
    vp_video_encoder_start(2, 0);
    vp_video_encoder_start(2, 1);
    #endif
#if 0
    pthread_t ph;
    pthread_create(&ph, NULL, video_thread_func, (void *) 0);
    pthread_create(&ph, NULL, video_thread_func, (void *) 1);
    pthread_create(&ph, NULL, video_thread_func, (void *) 2);
    pthread_create(&ph, NULL, video_thread_func, (void *) 3);
    pthread_create(&ph, NULL, video_thread_func, (void *) 4);
    pthread_create(&ph, NULL, video_thread_func, (void *) 5);
#endif
#if 1
    VENC_CHN        VencChn[6] = {0,1,2,3,4,5};
    SAMPLE_COMM_VENC_StartGetStreamV2(VencChn, (VP_SENSOR_NUM << 1), SAMPLE_VENC_RTSP);
#endif

    show_osd();
    // //while (running) pause();
    // vp_video_soft_pho_sens_init();
    const int buffer_size = 102400;
    uint32_t jpeg_size = 0;
    FILE* fout = NULL;
    int index = 0;
    char szPath[260] = { 0 };

    char ch;
    while (1) {
        ch = get_input_dbg();
        if (ch == 'q') break;
        else if (ch == '1') {
            vp_error("getchar is %c", ch);
            jpeg_size = buffer_size;
            ret = vp_video_encoder_jpeg(0, 1, NULL, jpeg_data, &jpeg_size);
            vp_error("jpeg_size = %d, ret=%d.\n", jpeg_size, ret);
            if (ret) {
                vp_error("Failed to call vp_video_encoder_jpeg:%d.\n", ret);
                continue;
            }
            sprintf(szPath, "/tmp/%02d.jpg", index++);
            fout = fopen(szPath, "wb");
            fwrite(jpeg_data, jpeg_size, 1, fout);
            fclose(fout);
        } else if (ch >= 'a') {
            float frame_rate = ch - 'a' + 10;
            vp_error("Set frame rate to %f.\n", frame_rate);
            ISP_PUB_ATTR_S      stPubAttr;
            if (TS_MPI_ISP_GetPubAttr(0, &stPubAttr))  {
                vp_error("Failed to call TS_MPI_ISP_GetPubAttr.\n");
            }
            stPubAttr.f32FrameRate = frame_rate;
            if (TS_MPI_ISP_SetPubAttr(0, &stPubAttr)) {
                vp_error("Failed to call TS_MPI_ISP_SetPubAttr.\n");
            }
        }
        sleep(1);
    }

    vp_video_osd_deinit();
    sleep(2);

	return ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
