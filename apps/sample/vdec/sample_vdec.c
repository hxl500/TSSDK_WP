
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "soft_line.h"

#include "sample_comm_base.h"
#include "sample_comm_util.h"
#include "sample_comm_sys.h"
#include "sample_comm_vi.h"
#include "sample_comm_isp.h"
#include "sample_comm_vpss.h"
#include "sample_comm_vdec.h"

#include "ts_comm_cpm.h"

#include "ts_alg_type.h"
#include "ts_alg_log.h"
#include "ts_alg_imgproc.h"
#include "soft_line.h"


#include <sys/time.h>
#include <sys/prctl.h>


#define H264_VIDEO_URL		 		"rtsp://192.168.1.88:8554/video/1080P.264"
#define H265_VIDEO_URL		 		"rtsp://192.168.1.88:8554/video/1080P.265"
#define SAMPLE_PROG_NAME            "Live555_Client"


#define RTSP_URL_STRING_MAX_LEN				(256)
#define RTSP_CHN_NUM_MAX                    (32)
#define SAMPLE_VDEC_RTSP_URL_FILE 			"/root/rtsp_url.json"
#define SAMPLE_STREAM_PATH   				"./"
#define FFMPEG_THREAD_STACK_SIZE   			(0x40000)

#define SAMPLE_H265_FILE_PATH "1080P.h265"
#define SAMPLE_H264_FILE_PATH "1080P.h264"
#define SAMPLE_JPEG_FILE_PATH "1080P.jpg"

typedef enum {
    RTSP_VIDEO_ENCODE_H264,
    RTSP_VIDEO_ENCODE_H265,
} RTSP_VIDEO_ENCODE_E;

typedef enum {
    BL_AVFRM_TYPE_A = 0,
    BL_AVFRM_TYPE_I = 1,
    BL_AVFRM_TYPE_P = 2,
    BL_AVFRM_TYPE_B = 3,
    BL_AVFRM_TYPE_JPEG = 4,
} BL_AVFRM_TYPE_E;

typedef struct {
    RTSP_VIDEO_ENCODE_E m_eEncodeType;
    BL_AVFRM_TYPE_E     m_eFrameType;
    char                *m_pdata;
    int                 m_s32DataSize;
    unsigned long long  m_u64Pts;
} BL_FRAME_DATA_S;

typedef struct FFContext {
	uint8_t isEnableRun;
	void (*video_cb)(TS_S32 _s32Chn, BL_FRAME_DATA_S* _pstFrameData);
	char url[256];
	pthread_t pid;
	int index;
    int64_t recv_stamp_ms; // 判断 av_read_frame 是否阻塞
    char 	check_network; // flag:check network is connection
} FFContext;

typedef struct tsSAMPLE_VDEC_RTSP_URL_ONECH_S{
	TS_S8 			 s8CHNum;
	PAYLOAD_TYPE_E 	 enType;
	TS_S32           s32Width;
	TS_S32           s32Height;
	TS_S32           s32RefFrame;
	TS_S32           s32DisplayFrame;
	TS_CHAR          c8RtspUrl[RTSP_URL_STRING_MAX_LEN];
} SAMPLE_VDEC_RTSP_URL_ONECH_S;

typedef struct tsSAMPLE_ALGO_S{
	TS_S32		s32Width;
	TS_S32		s32Height;
} SAMPLE_ALGO_S;

typedef struct tsSAMPLE_VDEC_RTSP_URL_MULTICH_S{
	TS_S8 			 			   			  s8CHToatalNum;
	TS_S8 			 			   			  s8VdecBlock;
	SAMPLE_ALGO_S							  stAlgoParam;
	SAMPLE_VDEC_RTSP_URL_ONECH_S              stMutiRtspUrl[RTSP_CHN_NUM_MAX];
} SAMPLE_VDEC_RTSP_URL_MULTICH_S;

typedef struct tsSAMPLE_VDEC_RTSP_VIDEO //for debug
{
    TS_S32 			s32CH;
    TS_S32 			s32Size;
    TS_CHAR 			c8Url[RTSP_URL_STRING_MAX_LEN];
}SAMPLE_VDEC_RTSP_VIDEO_t;

FFContext                       ffCtxArr[10] = {0};
SAMPLE_VDEC_RTSP_VIDEO_t 		tRtspVideoAarray[RTSP_CHN_NUM_MAX] = {0};
SAMPLE_VDEC_RTSP_URL_MULTICH_S  stRTSP_URL_MutiCH = {0};

typedef struct ts_cpm_ctrl
{
    AVS_GRP  num;
    TS_BOOL out_enable;
    TS_U32         u32Width;
    TS_U32         u32Height;
    PIXEL_FORMAT_E enPixelFormat;
    TS_U32  u32Interval;
    CPM_Handle_S stCPMHandle;
}cpm_ctrl;

static TS_U32 u32SampleIndex = 0;
static TS_S32 garg_index = -1;

TS_VOID SAMPLE_VDEC_HandleSig(TS_S32 signo)
{
    if (SIGINT == signo || SIGTSTP == signo || SIGTERM == signo)
    {
        // SAMPLE_COMM_VO_HdmiStop();
        // SAMPLE_COMM_SYS_Exit();
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    exit(0);
}

TS_VOID SAMPLE_VDEC_Usage(char *sPrgNm)
{
#if 0
    printf("\n/************************************/\n");
    printf("Usage : %s <index>\n", sPrgNm);
    printf("index:\n");
    printf("\t0:  VDEC(H265 PLAYBACK)-VPSS-VO\n");
    printf("\t1:  VDEC(H264 PLAYBACK)-VPSS-VO\n");
    printf("\t2:  VDEC(JPEG PLAYBACK)-VPSS-VO\n");
    printf("\t3:  0: decode 1080P.h265 1: decode 1080P.h264 2: decode 1080P.jpg\n");
#else
    printf("\n/************************************/\n");
    printf("Usage : %s <index>\n", sPrgNm);

    printf("\n\noutput file is in current directory: vdec_file \n\n");

    printf("index:\n");
    printf("\t0: decode 1080P.h265\n");
    printf("\t1: decode 1080P.h264\n");
    printf("\t2: decode 1080P.jpg\n");
#endif
}


#define SAMPLE_CPM_NAME "vdec-cpm"

void debug_save2file_render(void * buf, int len, char *fileName){
    int fd = -1;
    //unsigned long bytes;
    if (-1 == fd){
        fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0){
            SAMPLE_PRT("unable to create debug file.");
            fd = -2;
        }
    }

    if (fd > 0){
        write(fd, buf, len);
       // SAMPLE_PRT(" file %s write bytes %ld yuv debug file closed .",fileName, bytes);
        close(fd);
        fd = -2;
    }
}

typedef struct _SAMPLE_TEST_FUNC_T {
	TS_CHAR name[32];
	TS_U32 count;
} SAMPLE_TEST_FUNC_T;
TS_S32  VdecCpmSampleInit(TS_VOID **pHandle)
{
    TS_S32   s32Ret = TS_SUCCESS;
    static TS_U8  index = 0;
    SAMPLE_PRT("Enter!!!\n");
    index++;
    SAMPLE_TEST_FUNC_T *test_func = malloc(sizeof(SAMPLE_TEST_FUNC_T));
    if (test_func == NULL)
        return TS_FALSE;

    *pHandle = (TS_VOID *)test_func;
    SAMPLE_PRT("Enter!!!test_func=%p\n", test_func);

	char fname[128] = {0};

	snprintf(fname, sizeof(fname), "%s%d", SAMPLE_CPM_NAME, index);

    memcpy(test_func->name, fname, strlen(fname));

    SAMPLE_PRT("CPM name:%s\n", test_func->name);

    return s32Ret;
}

TS_S32  VdecCpmSampleInitVenc(TS_VOID **pHandle)
{
    TS_S32   s32Ret = TS_SUCCESS;
    static TS_U8  index = 0;
    SAMPLE_PRT("Enter!!!\n");
    index++;
    SAMPLE_TEST_FUNC_T *test_func = malloc(sizeof(SAMPLE_TEST_FUNC_T));
    if (test_func == NULL)
        return TS_FALSE;

    *pHandle = (TS_VOID *)test_func;
    SAMPLE_PRT("Enter!!!test_func=%p\n", test_func);

	char fname[128] = {0};

	snprintf(fname, sizeof(fname), "%s%d", SAMPLE_CPM_NAME, index);

    memcpy(test_func->name, fname, strlen(fname));

    SAMPLE_PRT("Venc CPM name:%s\n", test_func->name);

    return s32Ret;
}


TS_VOID VdecCpmSampleExit(TS_VOID *pHandle)
{
    //TS_S32   s32Ret = TS_SUCCESS;
    SAMPLE_TEST_FUNC_T *test_func = (SAMPLE_TEST_FUNC_T *)pHandle;

    if(TS_NULL == pHandle){
        SAMPLE_PRT("para pHandle invalid！\n");
        return;
    }

    SAMPLE_PRT("Enter!!!pHandle=%p\n", pHandle);
    SAMPLE_PRT("%s, count=%d\n", test_func->name, test_func->count);
    free(pHandle);

    return;
}

TS_S32  VdecCpmSampleProcess(TS_VOID *pHandle, TS_VOID **in, TS_VOID **out)
{
    TS_S32   s32Ret = TS_SUCCESS;
    // SAMPLE_PRT("pHandle=%p\n", pHandle);
    SAMPLE_TEST_FUNC_T *test_func = (SAMPLE_TEST_FUNC_T *)pHandle;
    unsigned char * YuvBuf = TS_NULL;
	TS_S32 width = 0;
	TS_S32 height = 0;

    if(TS_NULL == pHandle){
        SAMPLE_PRT("para pHandle invalid！\n");
        return TS_FAILURE;
    }

    test_func->count++;
    if (test_func->count%100 == 0)
        SAMPLE_PRT("%s process, count=%d\n", test_func->name, test_func->count);

    if(TS_NULL == in){
        SAMPLE_PRT("para in invalid！\n");
        return TS_FAILURE;
    }

    if(TS_NULL == out){
        SAMPLE_PRT("para out invalid！\n");
        return TS_FAILURE;
    }

    VIDEO_FRAME_INFO_S *pstFrameIn = in[0];
    //VIDEO_FRAME_INFO_S *pstFrameOut = out[0];

    if(TS_NULL == pstFrameIn){
        SAMPLE_PRT("para stFrameIn invalid！\n");
        return TS_FAILURE;
    }

    YuvBuf = (unsigned char *)(TS_UL)(pstFrameIn->stVFrame.u64VirAddr[0]);

    width = pstFrameIn->stVFrame.u32Width;
    height = pstFrameIn->stVFrame.u32Height;
#if 0
    rect.left = (test_func->count)%width;
    rect.top = (test_func->count)%height;
    rect.right = (500 + test_func->count)%width;
    rect.bottom = (500 + test_func->count)%height;
    YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_BLUE, 5);

    if(pstFrameOut != TS_NULL){
       test_func->count++;
       memcpy(pstFrameOut, pstFrameIn, sizeof(VIDEO_FRAME_INFO_S));
    }
#endif
    if(9 == u32SampleIndex) {
        int len = width*height*3/2;
        static int indexFile = 0;
        char filename[30] = {0};
        sprintf(filename, "frame_cpm_%d.yuv",  indexFile);
        indexFile++;
        indexFile=indexFile%5;

        debug_save2file_render(YuvBuf, len, filename);
    }
    /*need call mpi vb api to add reference times*/
    //mpi vb (poolid, phyaddr)

    return s32Ret;
}


TS_S32  VdecCpmSampleProcessVenc(TS_VOID *pHandle, TS_VOID **in, TS_VOID **out)
{
    TS_S32   s32Ret = TS_SUCCESS;
    // SAMPLE_PRT("pHandle=%p\n", pHandle);
    SAMPLE_TEST_FUNC_T *test_func = (SAMPLE_TEST_FUNC_T *)pHandle;
    unsigned char * YuvBuf = TS_NULL;
	TS_S32 width = 0;
	TS_S32 height = 0;

    if(TS_NULL == pHandle){
        SAMPLE_PRT("para pHandle invalid！\n");
        return TS_FAILURE;
    }

    test_func->count++;
    if (test_func->count%100 == 0)
        SAMPLE_PRT("%s processVenc, count=%d\n", test_func->name, test_func->count);

    if(TS_NULL == in){
        SAMPLE_PRT("para in invalid！\n");
        return TS_FAILURE;
    }

    if(TS_NULL == out){
        SAMPLE_PRT("para out invalid！\n");
        return TS_FAILURE;
    }

    VIDEO_FRAME_INFO_S *pstFrameIn = in[0];
    //VIDEO_FRAME_INFO_S *pstFrameOut = out[0];

    if(TS_NULL == pstFrameIn){
        SAMPLE_PRT("para stFrameIn invalid！\n");
        return TS_FAILURE;
    }

    YuvBuf = (unsigned char *)(TS_UL)(pstFrameIn->stVFrame.u64VirAddr[0]);

    width = pstFrameIn->stVFrame.u32Width;
    height = pstFrameIn->stVFrame.u32Height;
#if 0
    rect.left = (test_func->count)%width;
    rect.top = (test_func->count)%height;
    rect.right = (500 + test_func->count)%width;
    rect.bottom = (500 + test_func->count)%height;
    YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_BLUE, 5);

    if(pstFrameOut != TS_NULL){
       test_func->count++;
       memcpy(pstFrameOut, pstFrameIn, sizeof(VIDEO_FRAME_INFO_S));
    }
#endif
    if(9 == u32SampleIndex) {
        int len = width*height*3/2;
        static int indexFile = 0;
        char filename[30] = {0};
        sprintf(filename, "frame_cpm_venc_%d.yuv",  indexFile);
        indexFile++;
        indexFile=indexFile%5;

        debug_save2file_render(YuvBuf, len, filename);
    }
    /*need call mpi vb api to add reference times*/
    //mpi vb (poolid, phyaddr)

    return s32Ret;
}



TS_S32  VdecCpmSampleProcess2(TS_VOID *pHandle, TS_VOID **in, TS_VOID **out)
{
    TS_S32   s32Ret = TS_SUCCESS;
    SAMPLE_PRT("pHandle=%p\n", pHandle);
    SAMPLE_TEST_FUNC_T *test_func = (SAMPLE_TEST_FUNC_T *)pHandle;
    unsigned char * YuvBuf = TS_NULL;
	TS_S32 width = 0;
	TS_S32 height = 0;

    if(TS_NULL == pHandle){
        SAMPLE_PRT("para pHandle invalid！\n");
        return TS_FAILURE;
    }

    test_func->count++;
    if (test_func->count%100 == 0)
        SAMPLE_PRT("%s process2, count=%d\n", test_func->name, test_func->count);

    if(TS_NULL == in){
        SAMPLE_PRT("para in invalid！\n");
        return TS_FAILURE;
    }

    // if(TS_NULL == out){
    //     SAMPLE_PRT("para out invalid！\n");
    //     return TS_FAILURE;
    // }

    VIDEO_FRAME_INFO_S *pstFrameIn = in[0];
    // VIDEO_FRAME_INFO_S *pstFrameOut = out[0];

    if(TS_NULL == pstFrameIn){
        SAMPLE_PRT("para stFrameIn invalid！\n");
        return TS_FAILURE;
    }

    YuvBuf = (unsigned char *)(TS_UL)(pstFrameIn->stVFrame.u64VirAddr[0]);

    width = pstFrameIn->stVFrame.u32Width;
    height = pstFrameIn->stVFrame.u32Height;

    // rect.left = (test_func->count)%width;
    // rect.top = (test_func->count)%height;
    // rect.right = (500 + test_func->count)%width;
    // rect.bottom = (500 + test_func->count)%height;
    // YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_BLUE, 5);

    // if(pstFrameOut != TS_NULL){
    //    test_func->count++;
    //    memcpy(pstFrameOut, pstFrameIn, sizeof(VIDEO_FRAME_INFO_S));
    // }

    // if(9 == u32SampleIndex) {
        int len = width*height*3/2;
        static int indexFile = 0;
        char filename[30] = {0};
        sprintf(filename, "frame_cpm2_%d.yuv",  indexFile);
        indexFile++;
        indexFile=indexFile%5;

        debug_save2file_render(YuvBuf, len, filename);
    // }
    /*need call mpi vb api to add reference times*/
    //mpi vb (poolid, phyaddr)

    return s32Ret;
}

TS_S32  VdecCpmSampleProcess3(TS_VOID *pHandle, TS_VOID **in, TS_VOID **out)
{
    TS_S32   s32Ret = TS_SUCCESS;
    // SAMPLE_PRT("pHandle=%p\n", pHandle);
    SAMPLE_TEST_FUNC_T *test_func = (SAMPLE_TEST_FUNC_T *)pHandle;
    unsigned char * YuvBuf = TS_NULL;
	TS_S32 width = 0;
	TS_S32 height = 0;

    if(TS_NULL == pHandle){
        SAMPLE_PRT("para pHandle invalid！\n");
        return TS_FAILURE;
    }

    test_func->count++;
    if (test_func->count%100 == 0)
        SAMPLE_PRT("%s process3, count=%d\n", test_func->name, test_func->count);

    if(TS_NULL == in){
        SAMPLE_PRT("para in invalid！\n");
        return TS_FAILURE;
    }

    // if(TS_NULL == out){
    //     SAMPLE_PRT("para out invalid！\n");
    //     return TS_FAILURE;
    // }

    VIDEO_FRAME_INFO_S *pstFrameIn = in[0];
    // VIDEO_FRAME_INFO_S *pstFrameOut = out[0];

    if(TS_NULL == pstFrameIn){
        SAMPLE_PRT("para stFrameIn invalid！\n");
        return TS_FAILURE;
    }

    YuvBuf = (unsigned char *)(TS_UL)(pstFrameIn->stVFrame.u64VirAddr[0]);

    width = pstFrameIn->stVFrame.u32Width;
    height = pstFrameIn->stVFrame.u32Height;

    // rect.left = (test_func->count)%width;
    // rect.top = (test_func->count)%height;
    // rect.right = (500 + test_func->count)%width;
    // rect.bottom = (500 + test_func->count)%height;
    // YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_BLUE, 5);

    // if(pstFrameOut != TS_NULL){
    //    test_func->count++;
    //    memcpy(pstFrameOut, pstFrameIn, sizeof(VIDEO_FRAME_INFO_S));
    // }

    // if(9 == u32SampleIndex) {
        int len = width*height*3/2;
        static int indexFile = 0;
        char filename[30] = {0};
        sprintf(filename, "frame_cpm3_%d.yuv",  indexFile);
        indexFile++;
        indexFile=indexFile%5;

        debug_save2file_render(YuvBuf, len, filename);
    // }
    /*need call mpi vb api to add reference times*/
    //mpi vb (poolid, phyaddr)

    return s32Ret;
}

static pthread_t gs_VdecPid;
TS_VOID* SAMPLE_COMM_VDEC_FILE(TS_VOID* p);

TS_S32 SAMPLE_H264_H265_VDEC_FILE(TS_U8 VdecType)
{
    VB_CONFIG_S stVbConfig;
    TS_S32 i, s32Ret = TS_SUCCESS;
    VDEC_THREAD_PARAM_S stVdecSend[VDEC_MAX_CHN_NUM];
    SIZE_S stDispSize;
    //VO_LAYER VoLayer;
    TS_U32 u32VdecChnNum;
    //VPSS_GRP VpssGrp;
    pthread_t   VdecThread[2*VDEC_MAX_CHN_NUM];
    PIC_SIZE_E enDispPicSize;
    SAMPLE_VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM];
    //VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM];
    //SAMPLE_VO_CONFIG_S stVoConfig;
    //VPSS_GRP_ATTR_S stVpssGrpAttr;
    //TS_BOOL abChnEnable[VPSS_MAX_CHN_NUM];
    //VO_INTF_SYNC_E enIntfSync;
    //VO_INTF_TYPE_E enVoIntfType;

    u32VdecChnNum = 1;
    //VpssGrpNum    = u32VdecChnNum;

    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/
	enDispPicSize = PIC_1080P;
	//enIntfSync	  = VO_OUTPUT_1080P60;
	//enVoIntfType  = VO_INTF_HDMI;




    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
    stVbConfig.u32MaxPoolCnt             = 1;
    stVbConfig.astCommPool[0].u32BlkCnt  = 20*u32VdecChnNum;
    stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stDispSize.u32Width, stDispSize.u32Height,
                                                PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("init sys fail for %#x!\n", s32Ret);
        goto END1;
    }

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        if (0 ==VdecType) {
            astSampleVdec[i].enType                           = PT_H265;
        } else if (1 == VdecType) {
            astSampleVdec[i].enType                           = PT_H264;
        } else if (2 == VdecType) {
            astSampleVdec[i].enType                           = PT_JPEG;
        }

        astSampleVdec[i].u32Width                         = 320;
        astSampleVdec[i].u32Height                        = 240;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].stSapmleVdecVideo.enDecMode      = VIDEO_DEC_MODE_IP;
        astSampleVdec[i].stSapmleVdecVideo.enBitWidth     = DATA_BITWIDTH_8;
        astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 2;
        astSampleVdec[i].u32DisplayFrameNum               = 2;
        astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
    }
    s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("init mod common vb fail for %#x!\n", s32Ret);
        goto END2;
    }

    /************************************************
    step3:  start VDEC
    *************************************************/
    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }
	// TS_MPI_SYS_ACTIVE();
    //TS_MPI_VDEC_ACTIVE();
    /************************************************
    step8:  send stream to VDEC
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        if (0 ==VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), "android.hevc");
        } else if (1 == VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), "1080P.h264");
        } else if (2 == VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), "1080P.jpg");
        }

        snprintf(stVdecSend[i].cFilePath, sizeof(stVdecSend[i].cFilePath), "%s", SAMPLE_STREAM_PATH);
        stVdecSend[i].enType          = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode   = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId        = i;
        stVdecSend[i].s32IntervalTime = 1000;
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].u64PtsIncrease  = 0;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = TS_TRUE;
        stVdecSend[i].s32MilliSec     = -1;
        stVdecSend[i].s32MinBufSize   = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
    }

	pthread_create(&gs_VdecPid, 0, SAMPLE_COMM_VDEC_FILE, 0);


    SAMPLE_COMM_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_CmdCtrl(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_StopSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

	pthread_join(gs_VdecPid, TS_NULL);

	SAMPLE_PRT("BF SAMPLE_COMM_VDEC_Stop\n");

END3:

    SAMPLE_COMM_VDEC_Stop(u32VdecChnNum);

	SAMPLE_PRT("BF SAMPLE_COMM_VDEC_ExitVBPool\n");

END2:
    SAMPLE_COMM_VDEC_ExitVBPool();

	SAMPLE_PRT("BF SAMPLE_COMM_SYS_Exit\n");

END1:
    SAMPLE_COMM_SYS_Exit();

	SAMPLE_PRT("Exit done\n");

    return s32Ret;
}

static TS_S32 SAMPLE_SYS_Init()
{
    TS_S32 s32Ret;
    VB_CONFIG_S stVbConf = {0};

	stVbConf.astCommPool[0].u64BlkSize   = 640*360*3/2;
	stVbConf.astCommPool[0].u32BlkCnt	  = 1;
	stVbConf.u32MaxPoolCnt = 1;
	SAMPLE_PRT("blk size %llu, blk cnt %u, max pool cnt %u\n", stVbConf.astCommPool[0].u64BlkSize ,
		stVbConf.astCommPool[0].u32BlkCnt, stVbConf.u32MaxPoolCnt);

   s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    return TS_SUCCESS;
}



TS_S32 SAMPLE_DEC_H265_H264_JPG_FILE(TS_U8 VdecType)
{

    TS_S32 i, s32Ret;

    VDEC_THREAD_PARAM_S stVdecSend[VDEC_MAX_CHN_NUM] = {0};
    pthread_t           VdecThread[2*VDEC_MAX_CHN_NUM] = {0};

    SIZE_S              stDispSize;
    PIC_SIZE_E          enDispPicSize  = PIC_1080P;

    SAMPLE_VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM] = {0};

    TS_S32 u32VdecChnNum = 1;


    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    s32Ret = SAMPLE_SYS_Init();
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("init sys fail for %#x!\n", s32Ret);
        goto END1;
    }

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for(i=0; i< u32VdecChnNum; i++)
    {
        if (0 ==VdecType) {
            astSampleVdec[i].enType                           = PT_H265;
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), SAMPLE_H265_FILE_PATH);
        } else if (1 == VdecType) {
            astSampleVdec[i].enType                           = PT_H264;
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), SAMPLE_H264_FILE_PATH);
        } else if (2 == VdecType) {
            astSampleVdec[i].enType                           = PT_JPEG;
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), SAMPLE_JPEG_FILE_PATH);
        }

        snprintf(stVdecSend[i].cFilePath, sizeof(stVdecSend[i].cFilePath), "%s", SAMPLE_STREAM_PATH);
        astSampleVdec[i].u32Width                         = stDispSize.u32Width;
        astSampleVdec[i].u32Height                        = stDispSize.u32Height;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].stSapmleVdecVideo.enDecMode      = VIDEO_DEC_MODE_IP;
        astSampleVdec[i].stSapmleVdecVideo.enBitWidth     = DATA_BITWIDTH_8;
        astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 2;
        astSampleVdec[i].u32DisplayFrameNum               = 2;
        astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
    }


    /************************************************
    step3:  start VDEC
    *************************************************/
    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }

    /************************************************
    step8:  send stream to VDEC
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        if (0 ==VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), SAMPLE_H265_FILE_PATH);
        } else if (1 == VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), SAMPLE_H264_FILE_PATH);
        } else if (2 == VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), SAMPLE_JPEG_FILE_PATH);
        }

        snprintf(stVdecSend[i].cFilePath, sizeof(stVdecSend[i].cFilePath), "%s", SAMPLE_STREAM_PATH);
        stVdecSend[i].enType          = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode   = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId        = i;
        stVdecSend[i].s32IntervalTime = 1000;
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].u64PtsIncrease  = 0;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = TS_TRUE;
        stVdecSend[i].s32MilliSec     = -1;
        stVdecSend[i].s32MinBufSize   = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
    }

	pthread_create(&gs_VdecPid, 0, SAMPLE_COMM_VDEC_FILE, 0);

    SAMPLE_COMM_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_CmdCtrl(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_StopSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

	pthread_join(gs_VdecPid, TS_NULL);

	SAMPLE_PRT("BF SAMPLE_COMM_VDEC_Stop\n");

END3:

    SAMPLE_COMM_VDEC_Stop(u32VdecChnNum);

	SAMPLE_PRT("BF SAMPLE_COMM_VDEC_ExitVBPool\n");

    SAMPLE_COMM_VDEC_ExitVBPool();

	SAMPLE_PRT("BF SAMPLE_COMM_SYS_Exit\n");

END1:
    SAMPLE_COMM_SYS_Exit();

	SAMPLE_PRT("Exit done\n");

    return s32Ret;
}

#define ALG_IMAGE_H ALG_DETECT_IMAGE_H
#define ALG_IMAGE_W ALG_DETECT_IMAGE_W
#define ALG_RGB_CHN 	   4

typedef enum {
    BUFFER_STATUS_INIT = 0,
	BUFFER_STATUS_FILL,
	BUFFER_STATUS_ALGO_PROCESS,
    BUFFER_STATUS_ALGO_BYPASS
} ALGO_BUFFER_STATUS;

typedef	struct{
	TS_U32 						u32Algo_mask;
	TS_U32						u32Algo_num;
	TS_U32						u32CpmPipe_num;
	TS_U32						recv_frames;

	VIDEO_FRAME_INFO_S			algo_proc_buffer;
	pthread_t					algo_proc_pid;
	ALGO_BUFFER_STATUS			algo_proc_running_status;
	TS_BOOL						algo_proc_running_flag;
}sample_vdec_cpm_param_s;


static TS_U32 gU32Argc = 0;
static TS_CHAR **gppcArgv = NULL;

/******************************************************************************
* function    : main()
* Description : video vdec sample
******************************************************************************/

int main(int argc, char *argv[])
{
    TS_S32 s32Ret = TS_SUCCESS;
	TS_S32 s32Index = 0;

    SAMPLE_COMM_SYS_ShowVersion(argv[0]);

	gU32Argc = argc;
	gppcArgv = argv;

    if (argc < 2 || argc > 3)
    {
        SAMPLE_VDEC_Usage(argv[0]);
        return TS_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2))
    {
        SAMPLE_VDEC_Usage(argv[0]);
        return TS_SUCCESS;
    }

	garg_index = atoi(argv[1]);

    signal(SIGINT, SAMPLE_VDEC_HandleSig);
    signal(SIGTERM, SAMPLE_VDEC_HandleSig);

    if(3 == argc)
        s32Index = atoi(argv[2]);


    /******************************************
     choose the case
    ******************************************/
    switch (garg_index)
    {

		case 0:
		case 1:
		case 2:
        {
            s32Ret = SAMPLE_DEC_H265_H264_JPG_FILE(garg_index);
            break;
        }
		case 13:
        {
            s32Ret = SAMPLE_H264_H265_VDEC_FILE(s32Index);
            break;
        }

        default :
        {
            SAMPLE_PRT("the index is invaild!\n");
            SAMPLE_VDEC_Usage(argv[0]);
            s32Ret = TS_FAILURE;
            break;
        }
    }

    if (TS_SUCCESS == s32Ret)
    {
        SAMPLE_PRT("program exit normally!\n");
    }
    else
    {
        SAMPLE_PRT("program exit abnormally!\n");
    }


    return s32Ret;
}


