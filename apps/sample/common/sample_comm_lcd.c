#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <sys/ioctl.h>
//#include <sys/poll.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "gc9108reg.h"
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

#include "sample_comm_base.h"
#include "sample_comm_util.h"
#include "ts_comm_vb.h"
#include "mpi_vb.h"

#define DEVICE_FILE_NODE_LCD    "/dev/gc9108"                   //GC9108_MODULE_PATH
#define LCD_GC9108_WH           128
#define LCD_GC9108_SIZE         (LCD_GC9108_WH*LCD_GC9108_WH*2) //RGB565
#define INBUF_SIZE              4096
#define SAVE_YUV_DATA           0

static unsigned char    *tmp_gc9108_addr    = NULL;
static VB_POOL          scda_pool           = VB_INVALID_POOLID;
static VB_BLK           scda_blk            = VB_INVALID_HANDLE;
static TS_S32           glcd_fd             = -1;
static TS_S32           gvideo_thread_run   = 0;
static pthread_t        gvideo_thread       = 0;

static int thread_create(pthread_t *pt_id,void * proc,void * arg)
{
    pthread_attr_t attr;
    int ret;

    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    int stacksize = (4 << 10 ) << 10;
    pthread_attr_setstacksize(&attr, stacksize);

    ret = pthread_create(pt_id,&attr,(void *)proc,arg);
    if (ret != 0)
    {
        pthread_attr_destroy (&attr);
        printf("pthread_create error %s\n", strerror(ret));
        return -1;
    }
    pthread_attr_destroy (&attr);

    return 0;
}


//128x128, RGB565
static TS_S32 lcd_gc9108_init(TS_U64 u64PhyAddr, TS_U32 u32Size)
{
    if(0 >= u64PhyAddr || 0 >= u32Size)
    {
        SAMPLE_PRT("param error u64PhyAddr = [%lld] u32Size = [%d]!\r\n", u64PhyAddr, u32Size);
        return -1;
    }

    lcd_buffer_t stLcdBufferCfg = {.phys_addr  = u64PhyAddr,
                                   .size       = u32Size };

    glcd_fd = open(DEVICE_FILE_NODE_LCD, O_RDWR);
    if (glcd_fd < 0) {
        SAMPLE_PRT("can't open device file %s\r\n", DEVICE_FILE_NODE_LCD);
        return -1;
    }

    CHECK_RET(ioctl(glcd_fd, GC9108_IOC_ALLOCATE_PHYSICAL_MEMORY, &stLcdBufferCfg), "lcd_gc9108_init");

    return 0;
}

static TS_S32 lcd_gc9108_refresh(TS_VOID)
{
    CHECK_RET(ioctl(glcd_fd, GC9108_IOC_REFRESH, NULL), "lcd_gc9108_refresh");
    return 0;
}

static TS_S32 lcd_gc9108_clear(TS_VOID)
{
    memset(tmp_gc9108_addr, 0, LCD_GC9108_SIZE);
    return lcd_gc9108_refresh();
}

static void screen_video_decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, const char *filename)
{
    int                     ret;
    static unsigned int     cnt     = 1;
    static struct timeval   start   = {0};
    static struct timeval   end     = {0};

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }

#if SAVE_YUV_DATA
    char        buf[1024];
    static FILE *fp = NULL;
#endif

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            SAMPLE_PRT("Error during decoding\n");
            return;
        }

        static struct SwsContext *context = NULL;
        context = sws_getCachedContext( context,
                                        frame->width,
                                        frame->height,
                                        AV_PIX_FMT_YUV420P,    //src
                                        frame->width,
                                        frame->height,
                                        AV_PIX_FMT_BGR565BE,   //dst AV_PIX_FMT_RGB565BE AV_PIX_FMT_RGB565LE
                                        SWS_FAST_BILINEAR,
                                        NULL, NULL, NULL );

        uint8_t *dst_data[1]    = {(uint8_t *)tmp_gc9108_addr};
        int     dst_linesize[1] = {frame->width * 2};

        sws_scale(  context,
                    (const uint8_t * const *)frame->data,
                    frame->linesize,
                    0,
                    frame->height,
                    dst_data,
                    dst_linesize );

#if SAVE_YUV_DATA
                if (!fp){
                    snprintf(buf, sizeof(buf), "output-%s-%lld.yuv", filename, dec_ctx->frame_num);
                    fp = fopen(buf, "wb");
                }
                if(NULL != fp)
                {
                    //fwrite(frame->data[0], 1, frame->width * frame->height, fp);
                    //fwrite(frame->data[1], 1, frame->width * frame->height / 4, fp);
                    //fwrite(frame->data[2], 1, frame->width * frame->height / 4, fp);
                    fwrite(tmp_gc9108_addr, 1, frame->width * frame->height * 2, fp);
                }
#endif

        if (!((cnt++) % 256)) {
            gettimeofday(&end, NULL);
            long time_cost = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
            printf("soft decode + screen FPS: %f\n", 256000000.0f/(float)time_cost);
            start = end;
        }

        if (0 != lcd_gc9108_refresh()) {
            SAMPLE_PRT("failed! \n");
        }
    }

}

static void* screen_video_worker(void* param)
{
    char filename[64] = "/root/128x128.h264";
    const AVCodec *codec;
    AVCodecParserContext *parser;
    AVCodecContext *c= NULL;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t	 data_size;
    int ret;
    int eof;
    AVPacket *pkt;
    enum AVCodecID decoder_id;

    prctl(PR_SET_NAME, "SoftDec Display", 0,0,0);
    decoder_id = AV_CODEC_ID_H264;
    pkt = av_packet_alloc();
    if (!pkt){
		 SAMPLE_PRT("failed! \n");
		 return NULL;
	}
       

    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    codec = avcodec_find_decoder(decoder_id);
    if (!codec) {
        SAMPLE_PRT("Codec not found\n");
         return NULL;
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
        SAMPLE_PRT("parser not found\n");
         return NULL;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        SAMPLE_PRT("Could not allocate video codec context\n");
         return NULL;
    }

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        SAMPLE_PRT("Could not open codec\n");
         return NULL;
    }

    f = fopen(filename, "rb");
    if (!f) {
        SAMPLE_PRT("File open fail, please copy %s file to /root dir and ensure file property is RW!!!\n", filename);
         return NULL;
    }

    frame = av_frame_alloc();
    if (!frame) {
        SAMPLE_PRT("Could not allocate video frame\n");
         return NULL;
    }

    long time_cost_us = 0;

    do {
    /* read raw data from the input file */
    data_size = fread(inbuf, 1, INBUF_SIZE, f);
    if (ferror(f))
        break;
    eof = !data_size;

    if (eof) {
        SAMPLE_PRT("rewind file !\n");
        fseek(f, 0 ,SEEK_SET);
        eof = !eof;
        continue;
    }

    /* use the parser to split the data into frames */
    data = inbuf;
    while ((data_size > 0 || eof) && (gvideo_thread_run)) {
        ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
                                data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0) {
            fprintf(stderr, "Error while parsing\n");
            exit(1);
        }
        data        += ret;
        data_size   -= ret;

        if (pkt->size) {
            struct timeval start, end;
            //gettimeofday(&start, NULL);
            screen_video_decode(c, frame, pkt, "test");
            //usleep(62266);
            //gettimeofday(&end, NULL);
            /*long time_cost = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
            printf("time cost: %ldus, %lfms\n", time_cost, (double)time_cost / 1000); */
            time_cost_us += (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
        } else if (eof)
            break;
        }
    } while (!eof && gvideo_thread_run);

    //printf("time: %ld\n", time_cost_us/2978);

    /* flush the decoder */
    //screen_video_decode(c, frame, NULL, NULL);
    av_parser_close(parser);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    return 0;
}

static TS_S32 screen_video_start(void)
{
    int s32Ret = -1;

    gvideo_thread_run = 1;
    s32Ret = thread_create(&gvideo_thread, screen_video_worker, (void *)NULL);
    if (0 != s32Ret) {
        SAMPLE_PRT("create screen_video_worker thread failed !.");
        return -1;
    }

    return 0;
}

static TS_S32 screen_video_stop(void)
{
    gvideo_thread_run = 0;
    return 0;
}

static TS_S32 SAMPLE_COMM_LCD_CreateDispBuf(TS_U64 u64Size, TS_U64 *pu64PhyAddr, TS_U8 **ppu8VirAddr)
{
    if(NULL == pu64PhyAddr && NULL == ppu8VirAddr)
    {
        SAMPLE_PRT("param error!\r\n");
    }

    VB_POOL_CONFIG_S stVbPoolCfg = {0};
    memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));

    stVbPoolCfg.u64BlkSize  = u64Size ? u64Size : LCD_GC9108_SIZE;
    stVbPoolCfg.u32BlkCnt   = 1;

    scda_pool = TS_MPI_VB_CreatePool(&stVbPoolCfg);
    if (scda_pool == VB_INVALID_POOLID)
    {
        SAMPLE_PRT("failed, blkSize=%lld, count=%d\n",stVbPoolCfg.u64BlkSize, stVbPoolCfg.u32BlkCnt);
        return -1;
    }

    scda_blk = TS_MPI_VB_GetBlock(scda_pool, stVbPoolCfg.u64BlkSize, NULL);
    if (VB_INVALID_HANDLE == scda_blk)
    {
        SAMPLE_PRT("err! size:%lld\n", stVbPoolCfg.u64BlkSize);
        return -1;
    }

    CHECK_RET(TS_MPI_VB_MmapPool(scda_pool), "TS_MPI_VB_MmapPool");
    *pu64PhyAddr = TS_MPI_VB_Handle2PhysAddr(scda_blk);
    CHECK_RET(TS_MPI_VB_GetBlockVirAddr(scda_pool, *pu64PhyAddr, (TS_VOID**)ppu8VirAddr), "TS_MPI_VB_GetBlockVirAddr");

    return 0;

}

static TS_S32 SAMPLE_COMM_LCD_ReleaseDispBuf()
{
    if(VB_INVALID_HANDLE != scda_blk) {
        TS_MPI_VB_ReleaseBlock(scda_blk);
    }

    if(VB_INVALID_POOLID != scda_pool) {
        TS_MPI_VB_MunmapPool(scda_pool);
        TS_MPI_VB_ReleaseBlock(scda_blk);
    }

    return 0;
}

TS_S32 SAMPLE_COMM_LCD_Init(void)
{
    TS_U64  u64PhyAddr      = 0;
    TS_U32  u32DispZoneSize = LCD_GC9108_SIZE;

    SAMPLE_COMM_LCD_CreateDispBuf(u32DispZoneSize, &u64PhyAddr, &tmp_gc9108_addr);
    CHECK_RET(lcd_gc9108_init(u64PhyAddr, u32DispZoneSize), "lcd_gc9108_init");

    return 0;
}

TS_S32 SAMPLE_COMM_LCD_Start(void)
{
    CHECK_RET(screen_video_start(), "screen_video_start");
    return 0;
}

TS_S32 SAMPLE_COMM_LCD_Stop(void)
{
    CHECK_RET(screen_video_stop(), "screen_video_stop");
    if(gvideo_thread) {
        pthread_join(gvideo_thread, TS_NULL);
        gvideo_thread = 0;
    }
    lcd_gc9108_clear();
    SAMPLE_COMM_LCD_ReleaseDispBuf();
	close(glcd_fd);
    glcd_fd = -1;
    return 0;
}

